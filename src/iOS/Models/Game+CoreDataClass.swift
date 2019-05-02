//
//  Game+CoreDataClass.swift
//  Nintendoish-iOS
//
//  Created by Drew Ingebretsen on 9/12/18.
//  Copyright © 2018 Drew Ingebretsen. All rights reserved.
//
//

import Foundation
import CoreData

@objc(Game)
public class Game: NSManagedObject {
    var strippedName:String? {
        // returns the name stripped of region information
        if let name = name {
            for region in ["(U", "(W", "(J"] {
                if let range = name.range(of:region) {
                    return String(name.prefix(upTo: range.lowerBound))
                }
            }
            return name
        }
        return nil
    }
}
