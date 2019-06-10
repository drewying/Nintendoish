//
//  ContentView.swift
//  SwiftUITestApp
//
//  Created by Ingebretsen, Andrew (HBO) on 6/4/19.
//  Copyright © 2019 Ingebretsen, Andrew (HBO). All rights reserved.
//

import SwiftUI
import UIKit

struct RomBrowserView<StoreType : RomStore>: View {
    
    @ObjectBinding var romStore : StoreType
    
    var body: some View {
        NavigationView {
            List(romStore.roms) { rom in
                NavigationButton(
                    destination:RomDetailView(rom:rom)) {
                        RomCell(rom: rom)
                }
            }
                .navigationBarItem(title: Text("ROMS"), titleDisplayMode: .inline)
                .navigationBarItems(trailing: PresentationButton(Text("Add"), destination: RomPickerView<StoreType>(store: romStore)))
        }
    }
}

struct RomCell: View {
    var rom:RomViewModel
    init(rom:RomViewModel) {
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
