//
//  RomBrowserViewController.swift
//  Nintendoish
//
//  Created by Drew Ingebretsen on 6/13/18.
//  Copyright © 2018 Drew Ingebretsen. All rights reserved.
//

import UIKit
import CoreData

class RomBrowserViewController: UIViewController {
    
    @IBOutlet weak var tableView: UITableView!
    
    lazy var persistentContainer: GameLibraryPersistentContainer = {
        let container = GameLibraryPersistentContainer(name: "GameLibrary")
        
        container.loadPersistentStores(completionHandler: { (storeDescription, error) in
            if let error = error {
                fatalError("Unresolved error \(error)")
            }
        })
        return container
    }()
    
    var fetchedResultsController:NSFetchedResultsController<CoreDataRom>!
    
    var hasRoms:Bool {
        get {
            let count = fetchedResultsController.sections?[0].numberOfObjects ?? 0
            return count > 0
        }
    }
    
    @IBAction func addButton(_ sender: Any) {
        let vc:UIDocumentPickerViewController = UIDocumentPickerViewController.init(documentTypes: ["nintendo.nes"], in: .import)
        vc.delegate = self
        vc.allowsMultipleSelection = true
        present(vc, animated: true, completion: nil)
    }
    
    // MARK: UIViewController Lifecycle
    override func viewDidLoad() {
        super.viewDidLoad()
        
        let fr:NSFetchRequest<CoreDataRom> = CoreDataRom.fetchRequest()
        fr.sortDescriptors = [NSSortDescriptor(keyPath: \CoreDataRom.game.name, ascending: true)]
        fetchedResultsController = NSFetchedResultsController(fetchRequest: fr, managedObjectContext: persistentContainer.viewContext, sectionNameKeyPath: nil, cacheName: "RomCache")
        fetchedResultsController.delegate = self
        try? fetchedResultsController.performFetch()
    }

    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        guard let selectedIndexPath = tableView.indexPathForSelectedRow else { return }
        
        tableView.deselectRow(at: selectedIndexPath, animated: true)
        let rom:CoreDataRom = self.fetchedResultsController.object(at: selectedIndexPath)
        let vc:RomPlayerViewController = segue.destination as! RomPlayerViewController
        vc.rom = rom
        vc.title = rom.game.name
    }
    
    func displayMessage(_ message:String) {
        let alertController = UIAlertController(title: "Notice", message: message, preferredStyle: .alert)
        alertController.addAction(UIAlertAction(title: "Ok", style: .default, handler: nil))
        self.present(alertController, animated: true, completion: nil)
    }
}


extension RomBrowserViewController: UITableViewDataSource {
    // MARK: UITableViewDatasource
    
    func numberOfSections(in tableView: UITableView) -> Int {
        return fetchedResultsController.sections?.count ?? 0
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if hasRoms == false {
            return 1
        }
        return fetchedResultsController.sections?[section].numberOfObjects ?? 1
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        if (hasRoms == false) {
            return tableView.dequeueReusableCell(withIdentifier: "NoRomCell", for: indexPath)
        }
        
        let rom:CoreDataRom = self.fetchedResultsController.object(at: indexPath)
        let cell = tableView.dequeueReusableCell(withIdentifier: "RomCell", for: indexPath) as! RomBrowserTableViewCell
        
        cell.titleLabel.text = rom.game.name
        cell.coverImage.image = UIImage(data: rom.game.boxImage as Data)
        
        return cell
    }
}

extension RomBrowserViewController: UITableViewDelegate {
    // MARK: UITableViewDelegate
    
    func tableView(_ tableView: UITableView, trailingSwipeActionsConfigurationForRowAt indexPath: IndexPath) -> UISwipeActionsConfiguration? {
        if (hasRoms == false) {
            return UISwipeActionsConfiguration(actions: [])
        }
        
        let deleteAction = UIContextualAction.init(style: .destructive, title: "Remove", handler: {_,_,completionHandler in
            do {
                self.persistentContainer.viewContext.delete(self.fetchedResultsController.object(at: indexPath))
                try self.persistentContainer.viewContext.save()
                completionHandler(true)
            } catch {
                completionHandler(false)
                print("Error deleting object")
            }
        })
        
        return UISwipeActionsConfiguration(actions: [deleteAction])
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        if (hasRoms == false) {
            return
        }
        performSegue(withIdentifier: "playRom", sender: nil)
    }
}

extension RomBrowserViewController: UIDocumentPickerDelegate {
    // MARK: UIDocumentPickerDelegatex
        
    func documentPicker(_ controller: UIDocumentPickerViewController, didPickDocumentsAt urls: [URL]) {
        print("Processing \(urls.count) files")
        var successfulImports = 0
        var failedImports = 0
        let context = self.persistentContainer.viewContext
        do {
            for url in urls {
                // Get the md5 hash
                let romData = try Data(contentsOf: url)
                let md5:String = romData.advanced(by: 16).MD5().uppercased()
                
                //Find game
                let fetchRequest:NSFetchRequest<CoreDataGame> = CoreDataGame.fetchRequest()
                fetchRequest.predicate = NSPredicate(format: "md5 = %@", md5)
                fetchRequest.fetchLimit = 1
                
                guard let game = try context.fetch(fetchRequest).first else {
                    //Didn't find the game
                    print("Couldn't find md5 has that matches:" + md5)
                    failedImports += 1
                    continue
                }
                
                if game.rom != nil {
                    // game all ready exists
                    failedImports += 1
                } else {
                    // create the rom object
                    let romObj:CoreDataRom = NSEntityDescription.insertNewObject(forEntityName: "Rom", into: context) as! CoreDataRom
                    // romObj.game = game
                    // romObj.romData = romData as NSData
                    
                    // Save
                    try context.save()
                    successfulImports += 1
                }
                
            }
            if failedImports != 0 {
                self.displayMessage("Failed to import \(failedImports) games.\n\n Either the rom all ready exists in your library, or you tried to import a game that is unrecognized in our rom database.")
            }
        } catch {
            print("Error creating entity:\(error)")
        }
    }
}

extension RomBrowserViewController: NSFetchedResultsControllerDelegate {
    // MARK: NSFetchedResultsControllerDelegate
    
    func controllerWillChangeContent(_ controller: NSFetchedResultsController<NSFetchRequestResult>) {
        tableView.beginUpdates()
        if hasRoms == false {
            tableView.deleteRows(at: [IndexPath(row: 0, section: 0)], with: .automatic)
        }
    }
    
    func controller(_ controller: NSFetchedResultsController<NSFetchRequestResult>, didChange anObject: Any, at indexPath: IndexPath?, for type: NSFetchedResultsChangeType, newIndexPath: IndexPath?) {
        if type == .insert, let index = newIndexPath {
            self.tableView.insertRows(at: [index], with: .automatic)
        } else if type == .delete, let index = indexPath {
            self.tableView.deleteRows(at: [index], with: .automatic)
        }
    }
    
    func controllerDidChangeContent(_ controller: NSFetchedResultsController<NSFetchRequestResult>) {
        if hasRoms == false {
            tableView.insertRows(at: [IndexPath(row: 0, section: 0)], with: .automatic)
        }
        tableView.endUpdates()
    }
}
