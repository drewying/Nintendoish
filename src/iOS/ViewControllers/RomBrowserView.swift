//
//  ContentView.swift
//  SwiftUITestApp
//
//  Created by Ingebretsen, Andrew (HBO) on 6/4/19.
//  Copyright © 2019 Ingebretsen, Andrew (HBO). All rights reserved.
//

import SwiftUI
import UIKit
import MetalKit

struct RomBrowserView<StoreType : RomStore>: View {
    
    @ObjectBinding var romStore : StoreType
    
    var body: some View {
        NavigationView {
            List(romStore.roms) { (rom:Rom) in
                NavigationButton(
                    destination:RomDetailView(rom:rom)) {
                        RomCell(rom: rom)
                }
            }
                .navigationBarItem(title: Text("ROMS"))
                .navigationBarItems(trailing: PresentationButton(Text("Add"), destination: RomPickerView<StoreType>(store: romStore)))
        }
    }
}

struct NESEmulatorView: UIViewRepresentable {
    
    var rom:Rom
    
    func makeUIView(context: Context) -> UIView {
        var metalView = MTKView()
        guard let device = MTLCreateSystemDefaultDevice() else {
            var label = UILabel()
            label.text = "Metal Isn't Supported"
            return label
        }
        
        metalView.device = MTLCreateSystemDefaultDevice()
        metalView.backgroundColor = UIColor.black
        
        var nes:NESConsole = NESConsole()
        guard var nesRenderer = NESRenderer(metalKitView: metalView, nes: nes) else {
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
        
    }
}

struct RomDetailView: View {
    var rom:Rom
    
    var body: some View {
        NESEmulatorView(rom: rom)
    }
}

struct RomCell: View {
    var rom:Rom
    init(rom:Rom) {
        self.rom = rom
    }
    var body: some View {
        HStack {
            Image(uiImage: rom.image)
                .resizable()
                .aspectRatio(0.72, contentMode: .fit)
                .padding(8)
            Text(rom.name)
                .font(Font.custom("PixelNES", size: 20))
            }
            .frame(width: nil, height: 150, alignment: .leading)
    }
}

struct RomPickerView<StoreType:RomStore>: UIViewControllerRepresentable {
    
    var store:StoreType
    
    class RomPickerCoordinator:NSObject, UIDocumentPickerDelegate {
        var store:StoreType
        
        init(store:StoreType) {
            self.store = store
        }
        func documentPickerWasCancelled(_ controller: UIDocumentPickerViewController) {
            print("Cancelled")
            store.addRom(romData: Data())
        }
        func documentPicker(_ controller: UIDocumentPickerViewController, didPickDocumentsAt urls: [URL]) {
            print("Picked")
        }
    }
    
    
    func makeCoordinator() -> RomPickerCoordinator {
        RomPickerCoordinator(store: store)
    }
    
    func makeUIViewController(context: Context) -> UIDocumentPickerViewController {
        let vc:UIDocumentPickerViewController = UIDocumentPickerViewController.init(documentTypes: ["nintendo.nes"], in: .import)
        vc.delegate = context.coordinator
        return vc
    }
    
    func updateUIViewController(_ uiViewController: UIDocumentPickerViewController, context: UIViewControllerRepresentableContext<RomPickerView<StoreType>>) {
        
    }
}

#if DEBUG
struct ContentView_Previews : PreviewProvider {
    static var previews: some View {
        RomBrowserView<ExplicitRomStore>(romStore: ExplicitRomStore())
    }
}
#endif
