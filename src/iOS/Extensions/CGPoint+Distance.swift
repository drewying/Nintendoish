//
//  CGPoint+Distance.swift
//  Nintendoish-iOS
//
//  Created by Drew Ingebretsen on 1/5/19.
//  Copyright © 2019 Drew Ingebretsen. All rights reserved.
//

import Foundation
import UIKit

extension CGPoint {
    func distance(fromPoint:CGPoint) -> CGFloat {
        let xDist = (self.x - fromPoint.x)
        let yDist = (self.y - fromPoint.y)
        return sqrt((xDist * xDist) + (yDist * yDist))
    }
}
