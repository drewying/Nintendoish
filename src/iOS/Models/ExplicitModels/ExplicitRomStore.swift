//
//  ExplicitRomStore.swift
//  Nintendoish-macOS
//
//  Created by Ingebretsen, Andrew (HBO) on 6/6/19.
//  Copyright © 2019 Ingebretsen, Andrew (HBO). All rights reserved.
//

import Foundation
import UIKit
import SwiftUI
import Combine

class ExplicitRomStore: RomStore {
    let didChange = PassthroughSubject<ExplicitRomStore, Never>()
    
    public var roms:[RomViewModel] = []
    
    private var identifier:Int = 0
    
    init() {
        self.roms = [
//            createRom(),
//            createRom(),
//            createRom(),
        ]
    }
    
    func addRom(romData: Data) {
        roms.append(createRom())
        didChange.send(self)
    }
    
    func deleteRom(id:String) {
        guard let index = roms.firstIndex(where: {
            return $0.id == id
        }) else { return }
        
        roms.remove(at: index)
        didChange.send(self)
    }
    
    private func createRom() -> RomViewModel {
        identifier += 1
        let romData = NSData(contentsOfFile: Bundle.main.path(forResource: "StubRom", ofType: "nes") ?? "") ?? NSData()
        return RomViewModel(id: "\(identifier)", romData: romData, name: "Super Rom Man 2", image: UIImage(named: "cover")!)
    }
}
