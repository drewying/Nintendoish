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
    var nes: NESBridge!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Select the device to render with.  We choose the default device
        guard let defaultDevice = MTLCreateSystemDefaultDevice() else {
            print("Metal is not supported")
            return
        }
        
        metalView.device = defaultDevice
        metalView.backgroundColor = UIColor.black

        guard let newRenderer = Renderer(metalKitView: metalView) else {
            print("Renderer cannot be initialized")
            return
        }

        renderer = newRenderer

        renderer.mtkView(metalView, drawableSizeWillChange: metalView.drawableSize)

        metalView.delegate = renderer
        
        NESBridge.sharedNES().loadRom(Bundle.main.path(forResource: "Zelda", ofType: "nes"))
    }
}
