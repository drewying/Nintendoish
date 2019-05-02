//
//  AudioPlayer.swift
//  Nintendoish-iOS
//
//  Created by Drew Ingebretsen on 9/13/18.
//  Copyright © 2018 Drew Ingebretsen. All rights reserved.
//

import Foundation
import CoreAudio
import AudioToolbox

class NESAudioPlayer {
    var nes:NESConsole
    var queue:AudioQueueRef? = nil
    var buffers:[AudioQueueBufferRef?] = []
    var bufferLengthInFrames:UInt32 = 1024
    var frameSizeInBytes:UInt32 = UInt32(MemoryLayout<Float32>.size)
    var audioDispatchQueue = DispatchQueue(label: "nesAudioQueue")
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
        error = AudioQueueNewOutputWithDispatchQueue(&queue, &audioFormat, 0, audioDispatchQueue, { [unowned self] (queue:AudioQueueRef, buffer:AudioQueueBufferRef) -> Void in
            self.prepareBuffer(queue: queue, buffer: buffer)
        })
        
        if (error != 0) {
            print ("Error creating audio queue:\(error)")
            return
        }
        
        for _ in 0..<3 {
            var buffer:AudioQueueBufferRef?
            error = AudioQueueAllocateBuffer(queue!, bufferLengthInBytes, &buffer)
            if (error != 0) {
                print ("Error allocating audio buffer:\(error)")
                return
            }
            self.prepareBuffer(queue: queue!, buffer: buffer!)
        }
        AudioQueueSetParameter(queue!, kAudioQueueParam_Volume, 1.0);
        error = AudioQueueStart(queue!, nil)
        if (error != 0) {
            print ("Error starting queue:\(error)")
            return
        }
    }
    
    func stopAudio() {
        AudioQueueStop(queue!, false)
        AudioQueueDispose(queue!, false)
    }
}
