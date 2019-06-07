//
//  RomProtocol.swift
//  Nintendoish-iOS
//
//  Created by Ingebretsen, Andrew (HBO) on 6/6/19.
//  Copyright © 2019 Ingebretsen, Andrew (HBO). All rights reserved.
//

import Foundation
import SwiftUI


struct Rom: Identifiable {
    var id: String
    var romData: NSData
    var name: String
    var image: UIImage
    var strippedName:String {
        get {
            // returns the name stripped of region information
            for region in ["(U", "(W", "(J"] {
                if let range = name.range(of:region) {
                    return String(name.prefix(upTo: range.lowerBound))
                }
            }
            return name
        }
    }
}
