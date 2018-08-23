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

    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        let rom:Rom = self.fetchedResultsController.object(at: indexPath)
        performSegue(withIdentifier: "playRom", sender: rom.filePath)
    }

    override func numberOfSections(in tableView: UITableView) -> Int {
        let count = fetchedResultsController.sections?.count
        return count!
    }

    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        let count = fetchedResultsController.sections?[section].numberOfObjects
        return count!
    }

    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let rom:Rom = self.fetchedResultsController.object(at: indexPath)
        
        let cell = tableView.dequeueReusableCell(withIdentifier: "Cell", for: indexPath) as! RomBrowserTableViewCell
        cell.titleLabel.text = rom.game.name
        cell.coverImage.image = UIImage(data: rom.game.image! as Data)
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
        persistentContainer.performBackgroundTask({ backgroundContext in
            do {
                for url in urls {
                    // Get the md5 hash
                    let md5:String = try Data(contentsOf: url).MD5()
                    
                    // Get the HashEntry from the file
                    let fetchRequest = NSFetchRequest<HashEntry>(entityName: "HashEntry")
                    fetchRequest.predicate = NSPredicate(format: "md5Hash = %@", md5)
                    fetchRequest.fetchLimit = 1
                    let documentDirectory = try FileManager.default.url(for: .documentDirectory, in: .userDomainMask, appropriateFor:nil, create:false)
                    let fileURL = documentDirectory.appendingPathComponent(md5 + ".nes")
                    if FileManager.default.fileExists(atPath: fileURL.path) {
                        print("File all ready exists at path")
                    } else if let hashEntry = try backgroundContext.fetch(fetchRequest).first {
                        print("Creating Rom")
                        
                        // Copy the file
                        try FileManager.default.copyItem(atPath: url.path, toPath: fileURL.path)
                        
                        //Create the rom object
                        let romObj:Rom = NSEntityDescription.insertNewObject(forEntityName: "Rom", into: backgroundContext) as! Rom
                        romObj.game = hashEntry.game
                        romObj.filePath = fileURL.path
                        print("Created Rom")
                        
                        // Save
                        try backgroundContext.save()
                    } else {
                        // Handle hash not found error here
                    }
                }
                DispatchQueue.main.async {
                    try? self.fetchedResultsController.performFetch()
                    self.tableView.reloadData()
                }
            } catch {
                print("Error creating entity:\(error)")
            }
        })
    }
    
    func controllerDidChangeContent(_ controller: NSFetchedResultsController<NSFetchRequestResult>) {
        tableView.reloadData()
    }
    
   override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
    let path:String = sender as! String
    let vc:NESViewController = segue.destination as! NESViewController
        vc.loadRom(path: path)
    }
    
}
