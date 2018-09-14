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
    
    class func initializeDatabase() {
        do {
            if let documentsDirectory = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first {
                let path = documentsDirectory.appendingPathComponent("GameLibrary.sqlite", isDirectory: false)
                if FileManager.default.fileExists(atPath: path.path) == false {
                    try FileManager.default.copyItem(at: Bundle.main.url(forResource: "GameLibrary", withExtension: "sqlite")!, to: path)
                }
            }
        } catch {
            
        }
    }
}
