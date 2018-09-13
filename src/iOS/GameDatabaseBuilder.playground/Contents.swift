import Cocoa
import PlaygroundSupport

class NESPersistentContainer: NSPersistentContainer {
    /*override class func defaultDirectoryURL() -> URL {
        return URL.init(fileURLWithPath: workingDirectory)
    }*/
}

class GameDatabaseBuilder: NSObject, XMLParserDelegate {
    lazy var persistentContainer: NESPersistentContainer = {
        let container = NESPersistentContainer(name: "GameLibrary")
        
        container.loadPersistentStores(completionHandler: { (storeDescription, error) in
            if let error = error {
                fatalError("Unresolved error \(error)")
            }
        })
        return container
    }()
    
    func save() {
        if !persistentContainer.viewContext.commitEditing() {
            NSLog("unable to commit editing before saving")
        }
        if persistentContainer.viewContext.hasChanges {
            do {
                try persistentContainer.viewContext.save()
            } catch {
                print("Error saving")
            }
        }
    }
    
    var currentName = ""
    var currentMD5 = ""
    var currentSHA = ""
    var verified = false
    
    func parser(_ parser: XMLParser, didStartElement elementName: String, namespaceURI: String?, qualifiedName qName: String?, attributes attributeDict: [String : String] = [:]) {
        if elementName == "game" {
            currentName = attributeDict["name"]!
        }
        if elementName == "rom" {
            currentMD5 = attributeDict["md5"]!
            currentSHA = attributeDict["sha1"]!
            if attributeDict["status"] == "verified" {
                verified = true
            } else {
                verified = false
                print("Error! Unverified game: \(currentName)")
            }
        }
    }
    
    func parser(_ parser: XMLParser, didEndElement elementName: String, namespaceURI: String?, qualifiedName qName: String?) {
        if elementName == "game" && verified == true {
            createEntityDataForFile()
            currentName = ""
            currentMD5 = ""
            currentSHA = ""
        }
    }
    
    func buildDatabase() {
        let parser = XMLParser(contentsOf: Bundle.main.url(forResource: "database", withExtension: "xml")!);
        parser?.delegate = self
        parser?.parse()
    }
    
    func createEntityDataForFile() {
        let gameObj:NSManagedObject = NSEntityDescription.insertNewObject(forEntityName: "Game", into: persistentContainer.viewContext)
        gameObj.setValue(currentName, forKey: "name")
        gameObj.setValue(currentMD5, forKey: "md5")
        gameObj.setValue(currentSHA, forKey: "sha1")
        
        do {
            if let url = Bundle.main.url(forResource: currentName.replacingOccurrences(of: "&", with: "_"), withExtension: "png", subdirectory: "thumbs/boxart") {
                let boxartImage = try Data(contentsOf: url)
                gameObj.setValue(boxartImage, forKey: "boxImage")
            } else {
                print("\n\nUnable to find box art image for \(currentName)\n\n")
            }
            
            if let url = Bundle.main.url(forResource: currentName.replacingOccurrences(of: "&", with: "_"), withExtension: "png", subdirectory: "thumbs/snaps") {
                let snapImage = try Data(contentsOf: url)
                gameObj.setValue(snapImage, forKey: "screenImage")
            } else {
                print("\n\nUnable to find screenshot image for \(currentName)\n\n")
            }
            
            if let url = Bundle.main.url(forResource: currentName.replacingOccurrences(of: "&", with: "_"), withExtension: "png", subdirectory: "thumbs/titles") {
                let titleImage = try Data(contentsOf: url)
                gameObj.setValue(titleImage, forKey: "titleImage")
            } else {
                print("\n\nUnable to find title image for \(currentName)\n\n")
            }
        } catch {
            print("Error")
        }
        save()
        print("Saved \(currentName) to Database")
    }
}

var gameDatabaseBuilder = GameDatabaseBuilder()
gameDatabaseBuilder.buildDatabase()
print("Database saved to" + NESPersistentContainer.defaultDirectoryURL().absoluteString)
