//
//  NESBridge.h
//  NintendoishiOS
//
//  Created by Ingebretsen, Andrew (HBO) on 6/5/18.
//  Copyright © 2018 Ingebretsen, Andrew (HBO). All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NESBridge : NSObject

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

+(NESBridge*)sharedNES;

-(void)initializeConsole;
-(BOOL)loadRom:(NSString*)romPath;
-(void)stepFrame;
-(unsigned int)getDisplayBufferSize;
-(unsigned int*)getDisplayBuffer;
-(void)getAudioBuffer;
-(void)pressButton:(NESButton)button;
-(void)releaseButton:(NESButton)button;


@end
