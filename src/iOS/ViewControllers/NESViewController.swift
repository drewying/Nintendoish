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
class NESViewController: UIViewController {
    
    var renderer: NESRenderer!
    var audioPlayer: NESAudioPlayer!
    var nes: NESConsole = NESConsole()
    var rom:Rom?
    
    @IBOutlet weak var metalView: MTKView!
    
    // MARK: UIViewControlller Lifecycle
    
    override func viewDidLoad() {
        super.viewDidLoad()
        if let romData = rom?.romData {
            let tempFile = NSTemporaryDirectory() + "/" + ProcessInfo.processInfo.globallyUniqueString + ".nes"
            
            try? romData.write(toFile: tempFile, options: .atomic)
            nes.loadRom(tempFile)
        }
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        
        guard let defaultDevice = MTLCreateSystemDefaultDevice() else {
            print("Metal is not supported")
            return
        }
        
        metalView.device = defaultDevice
        metalView.backgroundColor = UIColor.black
        
        guard let newRenderer = NESRenderer(metalKitView: metalView, nes: nes) else {
            print("Renderer cannot be initialized")
            return
        }

        renderer = newRenderer
        renderer.mtkView(metalView, drawableSizeWillChange: metalView.drawableSize)
        metalView.delegate = renderer
        
        audioPlayer = NESAudioPlayer(nes: nes)
        audioPlayer.startAudio()
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        audioPlayer.stopAudio()
    }
    
    // MARK: IBActions
    
    @IBAction func AButtonTouchDown(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.5)
        nes.press(.A)
    }
    
    @IBAction func AButtonTouchUpInside(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.0)
        nes.release(.A)
    }
    
    @IBAction func AButtonTouchUpOutside(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.0)
        nes.release(.A)
    }
    
    @IBAction func BButtonTouchDown(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.5)
        nes.press(.B)
    }
    
    @IBAction func BButtonTouchUpInside(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.0)
        nes.release(.B)
    }
    
    @IBAction func BButtonTouchUpOutside(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.0)
        nes.release(.B)
    }
    
    @IBAction func startButtonTouchDown(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.5)
        nes.press(.START)
    }
    
    @IBAction func startButtonTouchUpInside(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.0)
        nes.release(.START)
    }
    
    @IBAction func startButtonTouchUpOutside(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.0)
        nes.release(.START)
    }
    
    @IBAction func selectButtonTouchDown(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.5)
        nes.press(.SELECT)
    }
    
    @IBAction func selectButtonTouchUpInside(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.0)
        nes.release(.SELECT)
    }
    
    @IBAction func selectButtonTouchUpOutside(_ sender: UIButton) {
        sender.backgroundColor = UIColor(white: 1.0, alpha: 0.0)
        nes.release(.SELECT)
    }
    
    var center:CGPoint = CGPoint(x:0, y:0)
    
    @IBAction func didDragTouchpad(_ sender: UIGestureRecognizer) {
        if let touchView = sender.view {
            
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
}
