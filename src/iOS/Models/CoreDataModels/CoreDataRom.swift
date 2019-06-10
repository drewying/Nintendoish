//
//  Rom+CoreDataClass.swift
//  Nintendoish-iOS
//
//  Created by Drew Ingebretsen on 9/12/18.
//  Copyright © 2018 Drew Ingebretsen. All rights reserved.
//
//

import Foundation
import CoreData
import SwiftUI

@objc(CoreDataRom)
public class CoreDataRom: NSManagedObject {
    @nonobjc public class func fetchRequest() -> NSFetchRequest<CoreDataRom> {
        return NSFetchRequest<CoreDataRom>(entityName: "Rom")
    }
    
    @NSManaged public var romData: NSData
    @NSManaged public var game: CoreDataGame
}
