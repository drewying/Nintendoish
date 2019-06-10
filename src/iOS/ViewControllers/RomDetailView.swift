//
//  RomPlayerView.swift
//  Nintendoish-iOS
//
//  Created by Drew Ingebretsen on 6/8/19.
//  Copyright © 2019 Drew Ingebretsen. All rights reserved.
//

import SwiftUI
import UIKit
import MetalKit

struct ControllerState {
    var up: Bool = false
    var down: Bool = false
    var left: Bool = false
    var right: Bool = false
    var a: Bool = false
    var b: Bool = false
    var start: Bool = false
    var select: Bool = false
}

struct NESEmulatorView: UIViewRepresentable {
    var rom: RomViewModel
    var state: ControllerState
    let nes: NESConsole = NESConsole()
    
    func makeUIView(context: Context) -> UIView {
        let metalView = MTKView()
        guard let device = MTLCreateSystemDefaultDevice() else {
            let label = UILabel()
            label.textAlignment = .center
            label.backgroundColor = UIColor.darkGray
            label.textColor = UIColor.white
            return label
        }
        
        metalView.device = device
        metalView.backgroundColor = UIColor.black
        
        guard let nesRenderer = NESRenderer(metalKitView: metalView, nes: nes) else {
            fatalError("Unable to initialize NESRenderer")
        }
        nesRenderer.mtkView(metalView, drawableSizeWillChange: metalView.drawableSize)
        metalView.delegate = nesRenderer
        
        var audioPlayer: NESAudioPlayer!
        audioPlayer = NESAudioPlayer(nes: nes)
        audioPlayer.startAudio()
        
        let tempFile = NSTemporaryDirectory() + "/" + ProcessInfo.processInfo.globallyUniqueString + ".nes"
        try? rom.romData.write(toFile: tempFile, options: .atomic)
        nes.loadRom(tempFile)
        
        return metalView
    }
    
    func updateUIView(_ view: UIView, context: Context) {
        if let label = view as? UILabel {
            var text = ""
            if state.up {
                text += "Up "
            }
            if state.down {
                text += "Down "
            }
            if state.left {
                text += "Left "
            }
            if state.right {
                text += "Right "
            }
            if state.a {
                text += "A "
            }
            if state.b {
                text += "B "
            }
            if state.start {
                text += "Start "
            }
            if state.select {
                text += "Select "
            }
            if text.count == 0 {
                text = "Metal Isn't Supported On This Device"
            }
            label.text = text
        } else {
            if state.up {
                nes.press(.UP)
            } else {
                nes.press(.DOWN)
            }
        }
    }
}

struct RomDetailView: View {
    var rom: RomViewModel
    @State var state: ControllerState = ControllerState()
    
    var body: some View {
        let dpadbuttons = DragGesture(minimumDistance: 0)
            .onChanged() { state in
                let height = CGFloat(130.0)
                let width = CGFloat(130.0)
                var center = CGPoint(x:width / 2, y: height / 2)
                let threshold:CGFloat = center.x
                let touchPoint = state.location
                let touchSensitivity:CGFloat = 0.2
                
                if touchPoint.distance(fromPoint: center) < center.x * touchSensitivity {
                    center = touchPoint
                }
                
                self.state.up    = CGPoint(x: center.x, y: 0).distance(fromPoint: touchPoint) < threshold
                self.state.down  = CGPoint(x: center.x, y: height).distance(fromPoint: touchPoint) < threshold
                self.state.left  = CGPoint(x: 0, y: center.y).distance(fromPoint: touchPoint) < threshold
                self.state.right = CGPoint(x: width, y: center.y).distance(fromPoint: touchPoint) < threshold
            } .onEnded() { _ in
                self.state.up = false
                self.state.down = false
                self.state.left = false
                self.state.right = false
            }
        
        let abbuttons = DragGesture(minimumDistance: 0)
            .onChanged() { state in
                let width = CGFloat(170.0)
                if state.location.x < width / 2 {
                    self.state.b = true
                } else {
                    self.state.a = true
                }
            }
            .onEnded() { _ in
                self.state.a = false
                self.state.b = false
            }
        
        let startselectbuttons = DragGesture(minimumDistance: 0)
            .onChanged() { state in
                let width = CGFloat(170.0)
                if state.location.x < width / 2 {
                    self.state.select = true
                } else {
                    self.state.start = true
                }
            }
            .onEnded() { _ in
                self.state.select = false
                self.state.start = false
            }
        
        return VStack {
            NESEmulatorView(rom: rom, state: state)
                .aspectRatio(256.0/240.0, contentMode: .fill)
            HStack {
                Image("DPad")
                    .resizable()
                    .frame(width: 130, height: 130)
                    .gesture(dpadbuttons)
                Spacer()
                Image("A-B Buttons")
                    .resizable()
                    .frame(width: 170, height: 100)
                    .gesture(abbuttons)
            }
                .padding()
            Image("SelectStart")
                .resizable()
                .frame(width: 120, height: 80)
                .gesture(startselectbuttons)
            Spacer()
        }
        .background(Color.black)
    }
}
