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
    @IBOutlet weak var metalView: MTKView!

    var nes: NESConsole!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Select the device to render with.  We choose the default device
        guard let defaultDevice = MTLCreateSystemDefaultDevice() else {
            print("Metal is not supported")
            return
        }
        
        metalView.device = defaultDevice
        metalView.backgroundColor = UIColor.black
        nes = NESConsole()
        nes.loadRom(Bundle.main.path(forResource: "Zelda", ofType: "nes"))
        
        guard let newRenderer = Renderer(metalKitView: metalView, nes: nes) else {
            print("Renderer cannot be initialized")
            return
        }

        renderer = newRenderer

        renderer.mtkView(metalView, drawableSizeWillChange: metalView.drawableSize)

        metalView.delegate = renderer
        
    }
    @IBAction func AButtonTouchDown(_ sender: Any) {
        nes.press(.A)
    }
    @IBAction func AButtonTouchUpInside(_ sender: Any) {
        nes.release(.A)
    }
    
    @IBAction func BButtonTouchDown(_ sender: Any) {
        nes.press(.B)
    }
    @IBAction func BButtonTouchUpInside(_ sender: Any) {
        nes.release(.B)
    }
    @IBAction func startButtonTouchDown(_ sender: Any) {
        nes.press(.START)
    }
    @IBAction func startButtonTouchUpInside(_ sender: Any) {
        nes.release(.START)
    }
    @IBAction func selectButtonTouchDown(_ sender: Any) {
        nes.press(.SELECT)
    }
    @IBAction func selectButtonTouchUpInside(_ sender: Any) {
        nes.release(.SELECT)
    }
    
}
