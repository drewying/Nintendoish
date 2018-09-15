//
//  GameViewController.swift
//  Nintendoish-iOS
//
//  Created by Ingebretsen, Andrew (HBO) on 6/5/18.
//  Copyright © 2018 Ingebretsen, Andrew (HBO). All rights reserved.
//

import UIKit
import MetalKit

// Our iOS specific view controller
class NESViewController: UIViewController {
    
    var renderer: Renderer!
    var audioPlayer: AudioPlayer!
    var nes: NESConsole = NESConsole()
    var rom:Rom?
    
    @IBOutlet weak var metalView: MTKView!
    @IBOutlet weak var actionBar: UIView!
    
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
        
        guard let newRenderer = Renderer(metalKitView: metalView, nes: nes) else {
            print("Renderer cannot be initialized")
            return
        }

        renderer = newRenderer
        renderer.mtkView(metalView, drawableSizeWillChange: metalView.drawableSize)
        metalView.delegate = renderer
        
        audioPlayer = AudioPlayer(nes: nes)
        audioPlayer.startAudio()
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        audioPlayer.stopAudio()
    }
    
    @IBAction func exitButton(_ sender: Any) {
        navigationController?.popViewController(animated: true)
    }
    
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
    
    var center:CGPoint = CGPoint(x:0, y:0);
    
    @IBAction func didDragTouchpad(_ sender: UIGestureRecognizer) {
        if let touchView = sender.view {
            
            let touchPoint = sender.location(in: touchView)
            let height:CGFloat = touchView.bounds.height
            let width:CGFloat = touchView.bounds.width
            
            if (sender.state == .began) {
                center = CGPoint(x: width / 2.0, y: height / 2.0)
                let distanceFromCenter:CGFloat = getDistance(point1: touchPoint, point2: center)
                if distanceFromCenter < center.x * 0.2 {
                    center = touchPoint
                }
            }
            

            let topCoordinate = CGPoint(x: center.x, y: 0)
            let bottomCoordinate = CGPoint(x: center.x, y: height)
            let leftCoordinate = CGPoint(x: 0, y: center.y)
            let rightCoordinate = CGPoint(x: width, y: center.y)
            
            let threshold:CGFloat = center.x
            
            //Release if we've ended the gesture or too close to sender
            let shouldRelease = sender.state == .ended || getDistance(point1: touchPoint, point2: center) < center.x * 0.2;
            
            if (getDistance(point1: topCoordinate, point2: touchPoint) > threshold || shouldRelease) {
                nes.release(.UP)
            } else {
                nes.press(.UP)
            }
            
            if (getDistance(point1: bottomCoordinate, point2: touchPoint) > threshold || shouldRelease) {
                nes.release(.DOWN)
            } else {
                nes.press(.DOWN)
                
            }
            
            if (getDistance(point1: leftCoordinate, point2: touchPoint) > threshold || shouldRelease) {
                nes.release(.LEFT)
            } else {
                nes.press(.LEFT)
            }
            
            if (getDistance(point1: rightCoordinate, point2: touchPoint) > threshold || shouldRelease) {
                nes.release(.RIGHT)
            } else {
                nes.press(.RIGHT)
            }
        }
    }
    
    func getDistance(point1:CGPoint, point2:CGPoint) -> CGFloat {
        let xDist = (point1.x - point2.x)
        let yDist = (point1.y - point2.y)
        return sqrt((xDist * xDist) + (yDist * yDist))
    }
}
