//
//  RomStoreProtocol.swift
//  Nintendoish-iOS
//
//  Created by Ingebretsen, Andrew (HBO) on 6/6/19.
//  Copyright © 2019 Ingebretsen, Andrew (HBO). All rights reserved.
//

import Foundation
import SwiftUI

protocol RomStore: BindableObject {
    var roms:[RomViewModel] { get }
    
    // TODO these methods should return error state
    func addRom(romData:Data)
    func deleteRom(id:String)
    
}
