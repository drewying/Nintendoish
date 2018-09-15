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
    var bufferLengthInFrames:Int = 2048
    var frameSizeInBytes:Int = MemoryLayout<Float32>.size
    var bufferLengthInBytes:Int {
        return bufferLengthInFrames * frameSizeInBytes
    }
    
    init(nes: NESConsole) {
        self.nes = nes
    }
    
    func startAudio() {
        var audioFormat:AudioStreamBasicDescription = AudioStreamBasicDescription()
        audioFormat.mSampleRate = 44100
        audioFormat.mFormatID = kAudioFormatLinearPCM
        audioFormat.mFormatFlags = kLinearPCMFormatFlagIsFloat | kLinearPCMFormatFlagIsPacked
        audioFormat.mFramesPerPacket = 1
        audioFormat.mChannelsPerFrame = 1
        audioFormat.mBitsPerChannel = UInt32(frameSizeInBytes * 8)
        audioFormat.mBytesPerFrame = UInt32(frameSizeInBytes)
        audioFormat.mBytesPerPacket = UInt32(frameSizeInBytes)
        audioFormat.mReserved = 0
 
        func audioCallback (userData:UnsafeMutableRawPointer?, queue:AudioQueueRef, buffer:AudioQueueBufferRef) {
            
            let frameSizeInBytes:Int = MemoryLayout<Float32>.size
            let bufferLengthInBytes = 2048 * frameSizeInBytes
            
            let nes:NESConsole = userData!.assumingMemoryBound(to: NESConsole.self).pointee
            if (nes.getAudioBufferLength() == 0) {
                memset(buffer.pointee.mAudioData, 0, bufferLengthInBytes)
                buffer.pointee.mAudioDataByteSize = UInt32(bufferLengthInBytes)
                
                let error:OSStatus = AudioQueueEnqueueBuffer(queue, buffer, 0, nil)
                if (error != 0) {
                    print ("Error enqueing audio buffer:\(error)")
                }
            } else {
                buffer.pointee.mAudioDataByteSize = nes.getAudioBufferLength() * UInt32(frameSizeInBytes)
                memcpy(buffer.pointee.mAudioData, nes.getAudioBuffer(), Int(nes.getAudioBufferLength()) * frameSizeInBytes)
                let error:OSStatus = AudioQueueEnqueueBuffer(queue, buffer, 0, nil)
                if (error != 0) {
                    print ("Error enqueing audio buffer:\(error)")
                }
                nes.clearAudioBuffer()
            }
        }
        
        var error:OSStatus
        
        error = AudioQueueNewOutput(&audioFormat,
                                                 audioCallback,
                                                 &nes,
                                                 nil,
                                                 nil,
                                                 0,
                                                 &queue)
        
        if (error != 0) {
            print ("Error creating audio queue:\(error)")
        }
        var temp = bufferLengthInBytes
        for _ in 0..<3 {
            var buffer:AudioQueueBufferRef?
            error = AudioQueueAllocateBuffer(queue!, UInt32(bufferLengthInBytes), &buffer)
            if (error != 0) {
                print ("Error creating audio buffer:\(error)")
            }
            buffers.append(buffer)
            
            memset(buffer!.pointee.mAudioData, 0, bufferLengthInBytes);
            buffer?.pointee.mAudioDataByteSize = UInt32(bufferLengthInBytes)

            error = AudioQueueEnqueueBuffer(queue!, buffer!, 0, nil);
            if (error != 0) {
                print ("Error enqueing audio buffer:\(error)")
            }
        }
        
        error = AudioQueueStart(queue!, nil)
        if (error != 0) {
            print ("Error starting queue:\(error)")
        }
    }
}
