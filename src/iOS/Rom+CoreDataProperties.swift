//
//  Rom+CoreDataProperties.swift
//  Nintendoish-iOS
//
//  Created by Ingebretsen, Andrew (HBO) on 9/12/18.
//  Copyright © 2018 Ingebretsen, Andrew (HBO). All rights reserved.
//
//

import Foundation
import CoreData


extension Rom {

    @nonobjc public class func fetchRequest() -> NSFetchRequest<Rom> {
        return NSFetchRequest<Rom>(entityName: "Rom")
    }

    @NSManaged public var filePath: String?
    @NSManaged public var game: Game

}
