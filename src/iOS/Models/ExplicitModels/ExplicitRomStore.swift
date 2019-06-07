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
    
    public var roms:[Rom] = []
    
    private var identifier:Int = 0
    
    init() {
        self.roms = [
            createRom(),
            createRom(),
            createRom(),
        ]
    }
    
    func addRom(romData: Data) {
        roms.append(createRom())
        didChange.send(self)
    }
    
    private func createRom() -> Rom {
        identifier += 1
        return Rom(id: "\(identifier)", romData: NSData(), name: "MegaMan", image: UIImage(named: "cover")!)
    }
}
