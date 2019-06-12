//
//  RomPlayerView.swift
//  Nintendoish-iOS
//
//  Created by Drew Ingebretsen on 6/8/19.
//  Copyright © 2019 Drew Ingebretsen. All rights reserved.
//

import SwiftUI

struct RomDetailView: View {
    var rom: RomViewModel
    @State var state: ControllerState = ControllerState()
    
    var body: some View {
        let dpadbuttons = DragGesture(minimumDistance: 0)
            .onChanged() { state in
                let height = CGFloat(130.0)
                let width = CGFloat(130.0)
                var center = CGPoint(x:width / 2, y: height / 2)
                let threshold:CGFloat = center.x
                let touchPoint = state.location
                let touchSensitivity:CGFloat = 0.2
                
                if touchPoint.distance(fromPoint: center) < center.x * touchSensitivity {
                    center = touchPoint
                }
                
                self.state.up    = CGPoint(x: center.x, y: 0).distance(fromPoint: touchPoint) < threshold
                self.state.down  = CGPoint(x: center.x, y: height).distance(fromPoint: touchPoint) < threshold
                self.state.left  = CGPoint(x: 0, y: center.y).distance(fromPoint: touchPoint) < threshold
                self.state.right = CGPoint(x: width, y: center.y).distance(fromPoint: touchPoint) < threshold
            } .onEnded() { _ in
                self.state.up = false
                self.state.down = false
                self.state.left = false
                self.state.right = false
            }
        
        let abbuttons = DragGesture(minimumDistance: 0)
            .onChanged() { state in
                let width = CGFloat(170.0)
                if state.location.x < width / 2 {
                    self.state.b = true
                } else {
                    self.state.a = true
                }
            }
            .onEnded() { _ in
                self.state.a = false
                self.state.b = false
            }
        
        let startselectbuttons = DragGesture(minimumDistance: 0)
            .onChanged() { state in
                let width = CGFloat(170.0)
                if state.location.x < width / 2 {
                    self.state.select = true
                } else {
                    self.state.start = true
                }
            }
            .onEnded() { _ in
                self.state.select = false
                self.state.start = false
            }
        
        return VStack {
            NESEmulatorView(rom: self.rom, state: self.state)
                .frame(minWidth: 0, maxWidth: .infinity, minHeight: 0, maxHeight: .infinity)
                .aspectRatio(256.0/240.0, contentMode: .fit)
            
            HStack {
                Image("DPad")
                    .resizable()
                    .frame(width: 130, height: 130)
                    .gesture(dpadbuttons)
                Spacer()
                Image("A-B Buttons")
                    .resizable()
                    .frame(width: 170, height: 100)
                    .gesture(abbuttons)
            }
                .padding()
            Image("SelectStart")
                .resizable()
                .frame(width: 120, height: 80)
                .gesture(startselectbuttons)
        }
            .frame(minWidth: 0, maxWidth: .infinity, minHeight: 0, maxHeight: .infinity, alignment: .top)
        .navigationBarTitle(Text(rom.strippedName), displayMode: .inline)
        .background(Color.black)
    }
}
