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

@objc(CoreDataGame)
public class CoreDataGame: NSManagedObject {
    @nonobjc public class func fetchRequest() -> NSFetchRequest<CoreDataGame> {
        return NSFetchRequest<CoreDataGame>(entityName: "Game")
    }
    
    @NSManaged public var boxImage: NSData
    @NSManaged public var md5: String
    @NSManaged public var name: String
    @NSManaged public var screenImage: NSData
    @NSManaged public var titleImage: NSData
    @NSManaged public var rom: CoreDataRom?
}
