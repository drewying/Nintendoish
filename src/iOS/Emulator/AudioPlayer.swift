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
    var buffer:AudioQueueBufferRef? = nil
    
    init(nes: NESConsole) {
        self.nes = nes
    }
    
    func startAudio() {
        var audioFormat:AudioStreamBasicDescription = AudioStreamBasicDescription()
        
        audioFormat.mSampleRate = 44100
        audioFormat.mFormatID = kAudioFormatLinearPCM
        audioFormat.mFormatFlags = kLinearPCMFormatFlagIsFloat | kLinearPCMFormatFlagIsNonInterleaved
        audioFormat.mBitsPerChannel = UInt32(MemoryLayout<Float32>.size * 8)
        audioFormat.mChannelsPerFrame = 1;
        audioFormat.mBytesPerFrame = UInt32(MemoryLayout<Float32>.size)
        audioFormat.mFramesPerPacket = 1
        audioFormat.mBytesPerPacket = audioFormat.mBytesPerFrame * audioFormat.mFramesPerPacket;
        audioFormat.mReserved = 0
    
        func audioCallback (userData:UnsafeMutableRawPointer?, queue:AudioQueueRef, buffer:AudioQueueBufferRef) {
            print("Calling callback")
            let nes:NESConsole = userData!.assumingMemoryBound(to: NESConsole.self).pointee

            memcpy(buffer, nes.getAudioBuffer(), Int(nes.getAudioBufferLength()))
            
            var audioDescription:AudioStreamPacketDescription = AudioStreamPacketDescription()
            audioDescription.mDataByteSize = nes.getAudioBufferLength() * UInt32(MemoryLayout<Float32>.size)
            audioDescription.mStartOffset = 0
            
            AudioQueueEnqueueBuffer(queue, buffer, nes.getAudioBufferLength(), &audioDescription)
            
            nes.clearAudioBuffer()
        }
        
        AudioQueueNewOutput(&audioFormat,
                            audioCallback,
                            &nes,
                            CFRunLoopGetMain(),
                            CFRunLoopMode.commonModes.rawValue,
                            0,
                            &queue)
        
        AudioQueueAllocateBuffer(queue!, 4096, &buffer)
        
        AudioQueueStart(queue!, nil)
    }
}
