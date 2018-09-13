//
//  Data+MD5.swift
//  Nintendoish
//
//  Created by Ingebretsen, Andrew (HBO) on 6/14/18.
//  Copyright © 2018 Ingebretsen, Andrew (HBO). All rights reserved.
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
    
    func SHA1() -> String {
        let digestLength = Int(CC_SHA1_DIGEST_LENGTH)
        let sha1Buffer = UnsafeMutablePointer<CUnsignedChar>.allocate(capacity: digestLength)
        withUnsafeBytes {
            CC_SHA1($0, CC_LONG(count), sha1Buffer)
        }
        
        let output = NSMutableString(capacity: Int(CC_SHA1_DIGEST_LENGTH * 2))
        for i in 0..<digestLength {
            output.appendFormat("%02x", sha1Buffer[i])
        }
        
        return NSString(format: output) as String
    }
}
