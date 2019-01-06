//
//  CGPoint+Distance.swift
//  Nintendoish-iOS
//
//  Created by Ingebretsen, Andrew (HBO) on 1/5/19.
//  Copyright © 2019 Ingebretsen, Andrew (HBO). All rights reserved.
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
