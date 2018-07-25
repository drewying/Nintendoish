//
//  Game+CoreDataProperties.swift
//  Nintendoish
//
//  Created by Ingebretsen, Andrew (HBO) on 6/16/18.
//  Copyright © 2018 Ingebretsen, Andrew (HBO). All rights reserved.
//
//

import Foundation
import CoreData


extension Game {

    @nonobjc public class func fetchRequest() -> NSFetchRequest<Game> {
        return NSFetchRequest<Game>(entityName: "Game")
    }

    @NSManaged public var image: NSData?
    @NSManaged public var name: String

}
