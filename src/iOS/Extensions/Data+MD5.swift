//
//  Data+MD5.swift
//  Nintendoish
//
//  Created by Drew Ingebretsen on 6/14/18.
//  Copyright © 2018 Drew Ingebretsen. All rights reserved.
//

import Foundation
import CommonCrypto

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
