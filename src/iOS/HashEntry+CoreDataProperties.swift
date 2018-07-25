//
//  HashEntry+CoreDataProperties.swift
//  Nintendoish
//
//  Created by Ingebretsen, Andrew (HBO) on 6/16/18.
//  Copyright © 2018 Ingebretsen, Andrew (HBO). All rights reserved.
//
//

import Foundation
import CoreData


extension HashEntry {

    @nonobjc public class func fetchRequest() -> NSFetchRequest<HashEntry> {
        return NSFetchRequest<HashEntry>(entityName: "HashEntry")
    }

    @NSManaged public var md5Hash: String
    @NSManaged public var game: Game

}
