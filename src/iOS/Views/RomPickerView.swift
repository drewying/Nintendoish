//
//  RomPickerView.swift
//  Nintendoish-iOS
//
//  Created by Ingebretsen, Andrew (HBO) on 6/11/19.
//  Copyright © 2019 Ingebretsen, Andrew (HBO). All rights reserved.
//

import Foundation
import SwiftUI

struct RomPickerView<StoreType:RomStore>: UIViewControllerRepresentable {
    
    var store:StoreType
    
    class RomPickerCoordinator:NSObject, UIDocumentPickerDelegate {
        var store:StoreType
        
        init(store:StoreType) {
            self.store = store
        }
        func documentPickerWasCancelled(_ controller: UIDocumentPickerViewController) {
            print("Cancelled")
            
        }
        func documentPicker(_ controller: UIDocumentPickerViewController, didPickDocumentsAt urls: [URL]) {
            urls.forEach {
                try? store.addRom(romData: Data(contentsOf: $0))
            }
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
