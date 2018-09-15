//
//  AudioPlayer.swift
//  Nintendoish-iOS
//
//  Created by Ingebretsen, Andrew (HBO) on 9/13/18.
//  Copyright © 2018 Ingebretsen, Andrew (HBO). All rights reserved.
//

import Foundation
import CoreAudio
import AudioToolbox

class AudioPlayer {
    var nes:NESConsole
    var queue:AudioQueueRef? = nil
    var buffers:[AudioQueueBufferRef?] = []
    var bufferLengthInFrames:UInt32 = 1024
    var frameSizeInBytes:UInt32 = UInt32(MemoryLayout<Float32>.size)
    var audioDispatchQueue = DispatchQueue(label: "audioQueue", attributes: .concurrent)
    var bufferLengthInBytes:UInt32 {
        return bufferLengthInFrames * frameSizeInBytes
    }
    
    init(nes: NESConsole) {
        self.nes = nes
    }
    
    func prepareBuffer(queue:AudioQueueRef, buffer:AudioQueueBufferRef) {
        if (nes.getAudioBufferLength() > 0) {
            let bufferLength = min(nes.getAudioBufferLength() * frameSizeInBytes, bufferLengthInBytes)
            memcpy(buffer.pointee.mAudioData, nes.getAudioBuffer(), Int(bufferLength))
            buffer.pointee.mAudioDataByteSize = bufferLength
            nes.clearAudioBuffer()
        } else {
            memset(buffer.pointee.mAudioData, 0, Int(bufferLengthInBytes))
            buffer.pointee.mAudioDataByteSize = bufferLengthInBytes
        }
        let error:OSStatus = AudioQueueEnqueueBuffer(queue, buffer, 0, nil)
        if (error != 0) {
            print ("Error enqueing audio buffer:\(error)")
        }
    }
    
    func startAudio() {
        var audioFormat:AudioStreamBasicDescription = AudioStreamBasicDescription()
        audioFormat.mSampleRate = 44100
        audioFormat.mFormatID = kAudioFormatLinearPCM
        audioFormat.mFormatFlags = kLinearPCMFormatFlagIsFloat | kLinearPCMFormatFlagIsPacked
        audioFormat.mFramesPerPacket = 1
        audioFormat.mChannelsPerFrame = 1
        audioFormat.mBitsPerChannel = frameSizeInBytes * 8
        audioFormat.mBytesPerFrame = frameSizeInBytes
        audioFormat.mBytesPerPacket = frameSizeInBytes
        audioFormat.mReserved = 0
        

        var error:OSStatus
        error = AudioQueueNewOutputWithDispatchQueue(&queue, &audioFormat, 0, audioDispatchQueue, {
            (queue:AudioQueueRef, buffer:AudioQueueBufferRef) -> Void in
            self.prepareBuffer(queue: queue, buffer: buffer)
        })
        
        if (error != 0) {
            print ("Error creating audio queue:\(error)")
        }
        
        for _ in 0..<3 {
            var buffer:AudioQueueBufferRef?
            error = AudioQueueAllocateBuffer(queue!, bufferLengthInBytes, &buffer)
            if (error != 0) {
                print ("Error allocating audio buffer:\(error)")
            }
            self.prepareBuffer(queue: queue!, buffer: buffer!)
        }
        
        error = AudioQueueStart(queue!, nil)
        if (error != 0) {
            print ("Error starting queue:\(error)")
        }
    }
    
    func stopAudio() {
        AudioQueueStop(queue!, false)
        AudioQueueDispose(queue!, false)
    }
}
