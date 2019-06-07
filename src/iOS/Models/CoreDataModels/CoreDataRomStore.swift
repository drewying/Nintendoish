//
//  RomStore.swift
//  Nintendoish-iOS
//
//  Created by Ingebretsen, Andrew (HBO) on 6/6/19.
//  Copyright © 2019 Ingebretsen, Andrew (HBO). All rights reserved.
//

import Foundation
import SwiftUI
import Combine
import CoreData

class CoreDataRomStore: NSObject, RomStore, NSFetchedResultsControllerDelegate {
    let didChange = PassthroughSubject<CoreDataRomStore, Never>()
    
    private lazy var fetchedResultsController:NSFetchedResultsController<CoreDataRom> = {
        let fr:NSFetchRequest<CoreDataRom> = CoreDataRom.fetchRequest()
        fr.sortDescriptors = [NSSortDescriptor(keyPath: \CoreDataRom.game.name, ascending: true)]
        let fetchedResultsController = NSFetchedResultsController(fetchRequest: fr, managedObjectContext: persistentContainer.viewContext, sectionNameKeyPath: nil, cacheName: "RomCache")
        fetchedResultsController.delegate = self
        try? fetchedResultsController.performFetch()
        return fetchedResultsController
    }()
    
    lazy var roms:[Rom] = {
        return fetchedResultsController.fetchedObjects?.compactMap { (coreDataRom:CoreDataRom) in
            return Rom(id: coreDataRom.game.md5, romData: coreDataRom.romData, name: coreDataRom.game.name, image: UIImage(data: coreDataRom.game.boxImage as Data)!)
            } ?? []
    }()
    
    lazy var persistentContainer: GameLibraryPersistentContainer = {
        let container = GameLibraryPersistentContainer(name: "GameLibrary")
        
        container.loadPersistentStores(completionHandler: { (storeDescription, error) in
            if let error = error {
                fatalError("Unresolved error \(error)")
            }
        })
        return container
    }()
    
    func addRom(romData: Data) {
        let md5:String = romData.advanced(by: 16).MD5().uppercased()
        let context = self.persistentContainer.viewContext
        
        //Find game
        let fetchRequest:NSFetchRequest<CoreDataGame> = CoreDataGame.fetchRequest()
        fetchRequest.predicate = NSPredicate(format: "md5 = %@", md5)
        fetchRequest.fetchLimit = 1
        
        guard let fetchedGames = try? context.fetch(fetchRequest) else {
            return
        }

        guard let game = fetchedGames.first else {
            return
        }

        if game.rom != nil {
            // Game all read exists. Return.
            return
        }
        
        // Create the rom object
        let romObj:CoreDataRom = NSEntityDescription.insertNewObject(forEntityName: "Rom", into: context) as! CoreDataRom
        romObj.game = game
        romObj.romData = romData as NSData
        
        // Save
        try? context.save()
    }
}
