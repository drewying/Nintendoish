//
//  NESEmulatorView.swift
//  Nintendoish-iOS
//
//  Created by Ingebretsen, Andrew (HBO) on 6/10/19.
//  Copyright © 2019 Ingebretsen, Andrew (HBO). All rights reserved.
//

import Foundation
import SwiftUI
import UIKit
import MetalKit


struct NESEmulatorView: UIViewRepresentable {
    var rom: RomViewModel
    var state: ControllerState
    
    init(rom:RomViewModel, state:ControllerState) {
        self.rom = rom
        self.state = state
    }
    
    class NESEmulatorCoordinator:NSObject, UIDocumentPickerDelegate {
        var rom: RomViewModel
        let nes: NESConsole
        let audioPlayer: NESAudioPlayer
        var nesRenderer:NESRenderer!
        
        init(rom: RomViewModel) {
            self.rom = rom
            nes = NESConsole()
            audioPlayer = NESAudioPlayer(nes: nes)
        }
        deinit {
            audioPlayer.stopAudio()
        }
    }
    
    func makeCoordinator() -> NESEmulatorCoordinator {
        NESEmulatorCoordinator(rom: rom)
    }
    
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
        
        context.coordinator.nesRenderer = NESRenderer(metalKitView: metalView, nes: context.coordinator.nes)
        
        context.coordinator.nesRenderer.mtkView(metalView, drawableSizeWillChange: metalView.drawableSize)
        metalView.delegate = context.coordinator.nesRenderer
        
        context.coordinator.audioPlayer.startAudio()
        
        let tempFile = NSTemporaryDirectory() + "/" + ProcessInfo.processInfo.globallyUniqueString + ".nes"
        try? rom.romData.write(toFile: tempFile, options: .atomic)
        context.coordinator.nes.loadRom(tempFile)
        
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
            if state.start {
                context.coordinator.nes.press(.START)
            } else {
                context.coordinator.nes.release(.START)
            }
            if state.select {
                context.coordinator.nes.press(.SELECT)
            } else {
                context.coordinator.nes.release(.SELECT)
            }
            if state.up {
                context.coordinator.nes.press(.UP)
            } else {
                context.coordinator.nes.release(.UP)
            }
            if state.down {
                context.coordinator.nes.press(.DOWN)
            } else {
                context.coordinator.nes.release(.DOWN)
            }
            if state.left {
                context.coordinator.nes.press(.LEFT)
            } else {
                context.coordinator.nes.release(.LEFT)
            }
            if state.right {
                context.coordinator.nes.press(.RIGHT)
            } else {
                context.coordinator.nes.release(.RIGHT)
            }
            if state.a {
                context.coordinator.nes.press(.A)
            } else {
                context.coordinator.nes.release(.A)
            }
            if state.b {
                context.coordinator.nes.press(.B)
            } else {
                context.coordinator.nes.release(.B)
            }
            
        }
    }
}
