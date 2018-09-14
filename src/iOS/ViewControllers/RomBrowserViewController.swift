//
//  RomBrowserViewController.swift
//  Nintendoish
//
//  Created by Ingebretsen, Andrew (HBO) on 6/13/18.
//  Copyright © 2018 Ingebretsen, Andrew (HBO). All rights reserved.
//

import UIKit
import CoreData

class RomBrowserViewController: UITableViewController, UIDocumentPickerDelegate, NSFetchedResultsControllerDelegate {
    
    lazy var persistentContainer: GameLibraryPersistentContainer = {
        let container = GameLibraryPersistentContainer(name: "GameLibrary")
        
        container.loadPersistentStores(completionHandler: { (storeDescription, error) in
            if let error = error {
                fatalError("Unresolved error \(error)")
            }
        })
        return container
    }()
    
    var fetchedResultsController:NSFetchedResultsController<Rom>!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let fr = NSFetchRequest<Rom>(entityName: "Rom")
        fr.sortDescriptors = [NSSortDescriptor(keyPath: \Rom.game.name, ascending: true)]
        fetchedResultsController = NSFetchedResultsController(fetchRequest: fr, managedObjectContext: persistentContainer.viewContext, sectionNameKeyPath: nil, cacheName: nil)
        fetchedResultsController.delegate = self
        try? fetchedResultsController.performFetch()
    }

    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        let rom:Rom = sender as! Rom
        let vc:NESViewController = segue.destination as! NESViewController
        vc.rom = rom
        vc.title = rom.game.strippedName
    }
    
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        let rom:Rom = self.fetchedResultsController.object(at: indexPath)
        performSegue(withIdentifier: "playRom", sender: rom)
    }

    override func numberOfSections(in tableView: UITableView) -> Int {
        let count = fetchedResultsController.sections?.count
        return count!
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        guard let count = fetchedResultsController.sections?[section].numberOfObjects else {
            return 1
        }
        return max(count, 1)
    }
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        guard let count = fetchedResultsController.sections?[indexPath.section].numberOfObjects else {
            return tableView.dequeueReusableCell(withIdentifier: "NoRomCell", for: indexPath)
        }
        if (count == 0) {
            return tableView.dequeueReusableCell(withIdentifier: "NoRomCell", for: indexPath)
        }
        
        let rom:Rom = self.fetchedResultsController.object(at: indexPath)
        let cell = tableView.dequeueReusableCell(withIdentifier: "RomCell", for: indexPath) as! RomBrowserTableViewCell

        cell.titleLabel.text = rom.game.strippedName
        cell.coverImage.image = UIImage(data: rom.game.boxImage! as Data)

        return cell
    }

    @IBAction func addButton(_ sender: Any) {
        let vc:UIDocumentPickerViewController = UIDocumentPickerViewController.init(documentTypes: ["nintendo.nes"], in: .import)
        vc.delegate = self
        vc.allowsMultipleSelection = true
        present(vc, animated: true, completion: nil)
    }
    
    func documentPicker(_ controller: UIDocumentPickerViewController, didPickDocumentsAt urls: [URL]) {
        print("Processing \(urls.count) files")
        var successfulImports = 0
        var failedImports = 0
        persistentContainer.performBackgroundTask({ backgroundContext in
            do {
                for url in urls {
                    // Get the md5 hash
                    let md5:String = try Data(contentsOf: url).advanced(by: 16).MD5().uppercased()
                    // Get the HashEntry from the file
                    let documentDirectory = try FileManager.default.url(for: .documentDirectory, in: .userDomainMask, appropriateFor:nil, create:false)
                    let fileURL = documentDirectory.appendingPathComponent(md5 + ".nes")
                    if FileManager.default.fileExists(atPath: fileURL.path) {
                        print("File all ready exists at path")
                        failedImports += 1
                    } else {
                        print("Creating Rom Entry")
                        //Find game
                        let fetchRequest = NSFetchRequest<Game>(entityName: "Game")
                        fetchRequest.predicate = NSPredicate(format: "md5 = %@", md5)
                        fetchRequest.fetchLimit = 1
                        if let game = try backgroundContext.fetch(fetchRequest).first {
                            //Create the rom object
                            let romObj:Rom = NSEntityDescription.insertNewObject(forEntityName: "Rom", into: backgroundContext) as! Rom
                            romObj.game = game
                            romObj.filePath = fileURL.path
                            print("Created Rom")
                            
                            // Copy the file
                            try FileManager.default.copyItem(atPath: url.path, toPath: fileURL.path)
                            
                            // Save
                            try backgroundContext.save()
                            
                            successfulImports += 1
                            
                        } else {
                            //Didn't find the game
                            print("Couldn't find md5 has that matches:" + md5)
                            
                            failedImports += 1
                        }
                    }
                }
                DispatchQueue.main.async {
                    try? self.fetchedResultsController.performFetch()
                    self.tableView.reloadData()
                    if failedImports == 0 {
                        self.displayMessage("Successfully imported \(successfulImports) games.")
                    } else {
                        self.displayMessage("Failed to import \(failedImports) games.\n\n Either the rom all ready exists in your library, or you tried to import a game that is unrecognized in our rom database.")
                    }
                }
            } catch {
                print("Error creating entity:\(error)")
            }
        })
    }
    
    func displayMessage(_ message:String) {
        let alertController = UIAlertController(title: "Notice", message: message, preferredStyle: .alert)
        alertController.addAction(UIAlertAction(title: "Ok", style: .default, handler: nil))
        self.present(alertController, animated: true, completion: nil)
    }
    
    func controllerDidChangeContent(_ controller: NSFetchedResultsController<NSFetchRequestResult>) {
        tableView.reloadData()
    }
}
