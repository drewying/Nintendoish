//
//  GameLibraryPersistentContainer.swift
//  Nintendoish-iOS
//
//  Created by Ingebretsen, Andrew (HBO) on 6/14/18.
//  Copyright © 2018 Ingebretsen, Andrew (HBO). All rights reserved.
//

import Foundation
import CoreData

class GameLibraryPersistentContainer : NSPersistentContainer {
    override class func defaultDirectoryURL() -> URL {
        return FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!
    }
}
