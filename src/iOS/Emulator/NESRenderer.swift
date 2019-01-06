//
//  Renderer.swift
//  Nintendoish-iOS
//
//  Created by Ingebretsen, Andrew (HBO) on 6/5/18.
//  Copyright © 2018 Ingebretsen, Andrew (HBO). All rights reserved.
//

// Our platform independent renderer class

import Metal
import MetalKit
import simd

let maxBuffersInFlight = 3

enum RendererError: Error {
    case badVertexDescriptor
}

class NESRenderer: NSObject, MTKViewDelegate {
    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
        
    }
    
    public let device: MTLDevice
    public let nes:NESConsole
    
    let commandQueue: MTLCommandQueue
    var pipelineState: MTLRenderPipelineState!
    var renderTexture: MTLTexture!
    var textureBuffer: MTLBuffer!
    var vertexBuffer: MTLBuffer!
    
    let inFlightSemaphore = DispatchSemaphore(value: maxBuffersInFlight)
    
    init?(metalKitView: MTKView, nes: NESConsole) {
        self.device = metalKitView.device!
        guard let queue = self.device.makeCommandQueue() else { return nil }
        self.commandQueue = queue
        self.nes = nes;
    
        metalKitView.colorPixelFormat = MTLPixelFormat.bgra8Unorm
        
        super.init()
        
        do {
            pipelineState = try buildRenderPipelineWithDevice(device: device, metalKitView: metalKitView)
        } catch {
            print("Unable to compile render pipeline state.  Error info: \(error)")
            return nil
        }
        
        let depthStateDesciptor = MTLDepthStencilDescriptor()
        depthStateDesciptor.depthCompareFunction = MTLCompareFunction.less
        depthStateDesciptor.isDepthWriteEnabled = true
        
        
        do {
            vertexBuffer  = try loadVertexBuffer(device: device)
            textureBuffer = try loadTextureBuffer(device: device)
            renderTexture = try loadTexture(buffer: textureBuffer)
        } catch {
            print("Unable to load texture. Error info: \(error)")
            return nil
        }
    }
    
    func buildRenderPipelineWithDevice(device: MTLDevice, metalKitView: MTKView) throws -> MTLRenderPipelineState {
        /// Build a render state pipeline object
        
        let library = device.makeDefaultLibrary()
        
        let vertexFunction = library?.makeFunction(name: "vertexShader")
        let fragmentFunction = library?.makeFunction(name: "fragmentShader")
        
        let pipelineDescriptor = MTLRenderPipelineDescriptor()
        pipelineDescriptor.label = "RenderPipeline"
        pipelineDescriptor.vertexFunction = vertexFunction
        pipelineDescriptor.fragmentFunction = fragmentFunction
        
        pipelineDescriptor.colorAttachments[0].pixelFormat = metalKitView.colorPixelFormat
        return try device.makeRenderPipelineState(descriptor: pipelineDescriptor)
    }
    
    func loadVertexBuffer(device: MTLDevice) throws -> MTLBuffer {
        let yOverscan:Float = 8.0 / 240.0
        let xOverscan:Float = 8.0 / 256.0
        let xMax:Float = 1.0 - xOverscan
        let xMin:Float = 0.0 + xOverscan
        let yMax:Float = 1.0 - yOverscan
        let yMin:Float = 0.0 + yOverscan
        
        let vertexData:[vector_float2] = [
            vector2( 1.0, -1.0), vector2(xMax, yMax),
            vector2(-1.0, -1.0), vector2(xMin, yMax),
            vector2(-1.0,  1.0), vector2(xMin, yMin),
            
            vector2( 1.0, -1.0), vector2(xMax, yMax),
            vector2(-1.0,  1.0), vector2(xMin, yMin),
            vector2( 1.0,  1.0), vector2(xMax, yMin),
        ]
    
        return device.makeBuffer(bytes: vertexData, length: 96, options: [.storageModeShared])!
    }
    
    func loadTextureBuffer(device: MTLDevice) throws -> MTLBuffer {
        
        let bufferSize:Int = Int(nes.getDisplayBufferSize()) * 4
        return device.makeBuffer(bytesNoCopy: nes.getDisplayBuffer(), length: bufferSize, options: [.storageModeShared], deallocator: nil)!
        
    }
    
    func loadTexture(buffer: MTLBuffer) throws -> MTLTexture {
        let descriptor = MTLTextureDescriptor.texture2DDescriptor(pixelFormat: .rgba8Uint, width: 256, height: 240, mipmapped: false)
        return buffer.makeTexture(descriptor: descriptor, offset: 0, bytesPerRow: 256 * 4)!
    }
    
    private func updateGameState() {
        nes.stepFrame()
    }
    
    func draw(in view: MTKView) {
        /// Per frame updates hare
        
        _ = inFlightSemaphore.wait(timeout: DispatchTime.distantFuture)
        
        if let commandBuffer = commandQueue.makeCommandBuffer() {
            
            let semaphore = inFlightSemaphore
            commandBuffer.addCompletedHandler { (_ commandBuffer)-> Swift.Void in
                semaphore.signal()
            }
            
            self.updateGameState()
            
            /// Delay getting the currentRenderPassDescriptor until we absolutely need it to avoid
            ///   holding onto the drawable and blocking the display pipeline any longer than necessary
            let renderPassDescriptor = view.currentRenderPassDescriptor
            
            if let renderPassDescriptor = renderPassDescriptor, let renderEncoder = commandBuffer.makeRenderCommandEncoder(descriptor: renderPassDescriptor) {
                
                /// Final pass rendering code here
                renderEncoder.label = "Rendering NES Display"
                
                renderEncoder.pushDebugGroup("Render Texture")
                renderEncoder.setRenderPipelineState(pipelineState)
                
                renderEncoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
                renderEncoder.setFragmentTexture(renderTexture, index: 0)
                
                renderEncoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: 6)
                renderEncoder.popDebugGroup()
                
                renderEncoder.endEncoding()
                
                if let drawable = view.currentDrawable {
                    commandBuffer.present(drawable)
                }
            }
            
            commandBuffer.commit()
        }
    }
}
