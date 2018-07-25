import Cocoa
import CommonCrypto
import PlaygroundSupport

extension Data {
    func MD5() -> String {
        let digestLength = Int(CC_MD5_DIGEST_LENGTH)
        let md5Buffer = UnsafeMutablePointer<CUnsignedChar>.allocate(capacity: digestLength)
        withUnsafeBytes {
            CC_MD5($0, CC_LONG(count), md5Buffer)
        }
        
        let output = NSMutableString(capacity: Int(CC_MD5_DIGEST_LENGTH * 2))
        for i in 0..<digestLength {
            output.appendFormat("%02x", md5Buffer[i])
        }
        
        return NSString(format: output) as String
    }
}

class NESPersistentContainer: NSPersistentContainer {
    /*override class func defaultDirectoryURL() -> URL {
        return URL.init(fileURLWithPath: workingDirectory)
    }*/
}

class GameDownloader {
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
    
    func downloadGameData() {
        do {
            print("Starting Download")
            // let files = try FileManager.default.contentsOfDirectory(at: Bundle.main.bundleURL, includingPropertiesForKeys: nil, options: .skipsHiddenFiles)
            let files = Bundle.main.urls(forResourcesWithExtension: "nes", subdirectory: nil)!
            for url in files {
                if url.lastPathComponent.suffix(3) == "nes" {
                    let trimSet = [
                        " & "     : " and ",
                        " VII "   : " 7 ",
                        " VI "    : " 6 ",
                        " V "     : " 5 ",
                        " IV "    : " 4 ",
                        " III "   : " 3 ",
                        " II "    : " 2 ",
                        "[!].nes" : "",
                        "(PRG0)"  : "",
                        "(PRG1)"  : "",
                        "(PRG2)"  : "",
                        "(REV0)"  : "",
                        "(REV1)"  : "",
                        "(REVA)"  : "",
                        "(JU)"    : "",
                        "(U)"     : "",
                        "(W)"     : "",
                        "(UE)"    : "",
                        ]
                    var name = url.lastPathComponent
                    print("\n Processing file:" + name)
                    for (k,v) in trimSet {
                        name = name.replacingOccurrences(of: k, with: v)
                    }
                    name = name.trimmingCharacters(in: .whitespacesAndNewlines)
                    
                    let md5:String = try Data(contentsOf: url).MD5()
                    var fetchRequest = NSFetchRequest<NSFetchRequestResult>(entityName: "HashEntry")
                    fetchRequest.predicate = NSPredicate(format: "md5Hash = %@", md5)
                    var resultCount = try persistentContainer.viewContext.count(for: fetchRequest)
                    if (resultCount == 0) {
                        createEntityDataForFile(md5: md5, name: name)
                    } else {
                        print("File exists in database")
                    }
                }
            }
    
            print ("\nFinished. Database at " + persistentContainer.persistentStoreCoordinator.persistentStores.first!.url!.path)
        } catch {
            
        }
        
    }
    
    func createEntityDataForFile(md5:String, name:String) {
        print("Querying database for title:" + name)
        let url = URL(string: "https://api-endpoint.igdb.com/games/?search=" + name.addingPercentEncoding(withAllowedCharacters: .urlHostAllowed)! + "&fields=cover,name,platforms&filter[release_dates.platform][eq]=18")
        var request = URLRequest(url: url!)
        request.setValue("application/json", forHTTPHeaderField: "Accept")
        request.setValue("dd751e4a3844fc6d5019b0689d936dab", forHTTPHeaderField: "user-key")
        
        do {
            // Perform the request
            let response: AutoreleasingUnsafeMutablePointer<URLResponse?>? = nil
            let data = try NSURLConnection.sendSynchronousRequest(request, returning: response)
            
            // Convert the data to JSON
            let jsonSerialized = try JSONSerialization.jsonObject(with: data, options: []) as? [[String : Any]]
            
            if let json = jsonSerialized {
                
                if let name = json.first?["name"] as? String,
                    let cover = (json.first?["cover"] as?[String : Any]) {
                    let coverUrl = cover["url"] as! String
                    
                    let imageURL = URL(string:"https:" + coverUrl.replacingOccurrences(of: "thumb", with: "cover_big"))
                    let imageRequest = URLRequest(url: imageURL!)
                    print("Found title:" + name)
                    print("Downloading " + imageURL!.absoluteString)
                    let imageData = try NSURLConnection.sendSynchronousRequest(imageRequest, returning: response)
                    
                    let gameObj:NSManagedObject = NSEntityDescription.insertNewObject(forEntityName: "Game", into: persistentContainer.viewContext)
                    gameObj.setValue(name, forKey: "name")
                    gameObj.setValue(imageData, forKey: "image")
                    
                    let hashObj:NSManagedObject = NSEntityDescription.insertNewObject(forEntityName: "HashEntry", into: persistentContainer.viewContext)
                    hashObj.setValue(md5, forKey: "md5Hash")
                    hashObj.setValue(gameObj, forKey: "game")
                    save()
                    print("Saved to Database")
                }
            }
        } catch let error {
            print("ERROR PARSING:" + error.localizedDescription)
        }
        
    }
}


var gameDownloader = GameDownloader()
gameDownloader.downloadGameData()

