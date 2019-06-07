//
//  GameViewController.swift
//  Nintendoish-iOS
//
//  Created by Drew Ingebretsen on 6/5/18.
//  Copyright © 2018 Drew Ingebretsen. All rights reserved.
//

import UIKit
import MetalKit

// Our iOS specific view controller
class RomPlayerViewController: UIViewController {
    
    var renderer: NESRenderer!
    var audioPlayer: NESAudioPlayer!
    var nes: NESConsole = NESConsole()
    var rom:CoreDataRom?
    
    @IBOutlet weak var metalView: MTKView!
    
    @IBOutlet weak var aButton:      UIButton!
    @IBOutlet weak var bButton:      UIButton!
    @IBOutlet weak var startButton:  UIButton!
    @IBOutlet weak var selectButton: UIButton!
    
    lazy var buttonMap:[UIView: NESButton] = [
        bButton:      .B,
        aButton:      .A,
        startButton:  .START,
        selectButton: .SELECT
    ]
    
    // MARK: UIViewControlller Lifecycle
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let tempFile = NSTemporaryDirectory() + "/" + ProcessInfo.processInfo.globallyUniqueString + ".nes"
        try? rom?.romData.write(toFile: tempFile, options: .atomic)
        nes.loadRom(tempFile)
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        do {
            try initializeVideo()
            initializeAudio()
        } catch {
            print("Renderer cannot be initialized")
        }
    }
    
    deinit {
        audioPlayer.stopAudio()
    }
    
    // MARK Intialization
    
    func initializeVideo() throws {
        metalView.device = MTLCreateSystemDefaultDevice()
        metalView.backgroundColor = UIColor.black
        
        renderer = NESRenderer(metalKitView: metalView, nes: nes)
        renderer.mtkView(metalView, drawableSizeWillChange: metalView.drawableSize)
        metalView.delegate = renderer
    }
    
    func initializeAudio() {
        audioPlayer = NESAudioPlayer(nes: nes)
        audioPlayer.startAudio()
    }
    
    // MARK: IBActions
    
    @IBAction func buttonTouchDown(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.5)
        let button:NESButton = buttonMap[sender]!
        nes.press(button)
    }
    
    @IBAction func buttonTouchUpInside(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.0)
        let button:NESButton = buttonMap[sender]!
        nes.release(button)
    }
    
    @IBAction func buttonTouchUpOutside(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.0)
        let button:NESButton = buttonMap[sender]!
        nes.release(button)
    }
    
    var center:CGPoint = CGPoint(x:0, y:0)
    
    @IBAction func didDragTouchpad(_ sender: UIGestureRecognizer) {
        guard let touchView = sender.view else { return }
            
        let touchSensitivity:CGFloat = 0.2
        let touchPoint:CGPoint = sender.location(in: touchView)
        let height:CGFloat = touchView.bounds.height
        let width:CGFloat = touchView.bounds.width
        
        if (sender.state == .began) {
            center = CGPoint(x: width / 2.0, y: height / 2.0)
            let distanceFromCenter:CGFloat = touchPoint.distance(fromPoint: center)
            if distanceFromCenter < center.x * touchSensitivity {
                center = touchPoint
            }
        }
        

        let topCoordinate = CGPoint(x: center.x, y: 0)
        let bottomCoordinate = CGPoint(x: center.x, y: height)
        let leftCoordinate = CGPoint(x: 0, y: center.y)
        let rightCoordinate = CGPoint(x: width, y: center.y)
        
        let threshold:CGFloat = center.x
        
        //Release if we've ended the gesture or too close to sender
        let shouldRelease = sender.state == .ended || touchPoint.distance(fromPoint: center) < center.x * touchSensitivity
        
        if topCoordinate.distance(fromPoint: touchPoint) > threshold || shouldRelease {
            nes.release(.UP)
        } else {
            nes.press(.UP)
        }
        
        if bottomCoordinate.distance(fromPoint: touchPoint) > threshold || shouldRelease {
            nes.release(.DOWN)
        } else {
            nes.press(.DOWN)
            
        }
        
        if leftCoordinate.distance(fromPoint: touchPoint) > threshold || shouldRelease {
            nes.release(.LEFT)
        } else {
            nes.press(.LEFT)
        }
        
        if rightCoordinate.distance(fromPoint: touchPoint) > threshold || shouldRelease {
            nes.release(.RIGHT)
        } else {
            nes.press(.RIGHT)
        }
    }
}
