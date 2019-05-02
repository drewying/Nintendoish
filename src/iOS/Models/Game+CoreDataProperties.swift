//
//  Game+CoreDataProperties.swift
//  Nintendoish-iOS
//
//  Created by Drew Ingebretsen on 9/14/18.
//  Copyright © 2018 Drew Ingebretsen. All rights reserved.
//
//

import Foundation
import CoreData


extension Game {

    @nonobjc public class func fetchRequest() -> NSFetchRequest<Game> {
        return NSFetchRequest<Game>(entityName: "Game")
    }

    @NSManaged public var boxImage: NSData?
    @NSManaged public var md5: String?
    @NSManaged public var name: String?
    @NSManaged public var screenImage: NSData?
    @NSManaged public var sha1: String?
    @NSManaged public var titleImage: NSData?
    @NSManaged public var rom: Rom?

}
