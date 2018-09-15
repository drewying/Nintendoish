//
//  Game+CoreDataClass.swift
//  Nintendoish-iOS
//
//  Created by Ingebretsen, Andrew (HBO) on 9/12/18.
//  Copyright © 2018 Ingebretsen, Andrew (HBO). All rights reserved.
//
//

import Foundation
import CoreData

@objc(Game)
public class Game: NSManagedObject {
    var strippedName:String? {
        // returns the name stripped of region information
        if let name = name {
            if let index = name.lastIndex(of: "(") {
                return String(name.prefix(upTo: index))
            } else {
                return name + "Test"
            }
        }
        return nil
    }
}
