//
//  NESBridge.h
//  NintendoishiOS
//
//  Created by Drew Ingebretsen on 6/5/18.
//  Copyright © 2018 Drew Ingebretsen. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NESConsole : NSObject

typedef NS_ENUM(NSInteger, NESButton) {
    A,
    B,
    START,
    SELECT,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

-(BOOL) loadRom:(NSString*)romPath;
-(void) stepFrame;

-(unsigned int) getDisplayBufferSize;
-(unsigned int*) getDisplayBuffer;

-(unsigned int) getAudioBufferLength;
-(float*) getAudioBuffer;
-(void) clearAudioBuffer;

-(void) pressButton:(NESButton)button;
-(void) releaseButton:(NESButton)button;

@end
