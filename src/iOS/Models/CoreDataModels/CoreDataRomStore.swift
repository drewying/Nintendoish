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
    
    var roms:[RomViewModel] = []
    
    override init() {
        super.init()
        fetchRoms()
    }
    
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
            // Game all read exists. Return.
            return
        }

        // Create the rom object
        let romObj:CoreDataRom = NSEntityDescription.insertNewObject(forEntityName: "Rom", into: context) as! CoreDataRom
        romObj.game = game
        romObj.romData = romData as NSData
        game.rom = romObj
        
        // Save
        do {
            try context.save()
        } catch let error as NSError {
            print("Error saving \(error.localizedDescription)")
        }
        
    }
    
    func deleteRom(id: String) {
        //Find game
        guard let romObject = self.fetchedResultsController.fetchedObjects?.first(where: {
            return $0.game.md5 == id
        }) else { return }
        
        do {
            self.persistentContainer.viewContext.delete(romObject)
            try self.persistentContainer.viewContext.save()
        } catch {
            print("Error deleting object")
        }

    }
    
    func fetchRoms() {
        roms = fetchedResultsController.fetchedObjects?.compactMap { (coreDataRom:CoreDataRom) in
            return RomViewModel(id: coreDataRom.game.md5, romData: coreDataRom.romData, name: coreDataRom.game.name, image: UIImage(data: coreDataRom.game.boxImage as Data)!)
            } ?? []
    }
    
    func controllerDidChangeContent(_ controller: NSFetchedResultsController<NSFetchRequestResult>) {
        fetchRoms()
        didChange.send(self)
    }
}
