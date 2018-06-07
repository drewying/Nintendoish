//
//  NESBridge.m
//  NintendoishiOS
//
//  Created by Ingebretsen, Andrew (HBO) on 6/5/18.
//  Copyright © 2018 Ingebretsen, Andrew (HBO). All rights reserved.
//

#import <Foundation/Foundation.h>
#include "NESBridge.h"
#include "NES.h"

@interface NESBridge()
@property NES::Console *nes;
@end

@implementation NESBridge

+(NESBridge*)sharedNES {
    static NESBridge *shared;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        shared = [NESBridge new];
        [shared initializeConsole];
    });
    return shared;
}

-(void)initializeConsole {
    self.nes = new NES::Console;
}

-(BOOL)loadRom:(NSString*)romPath {
    self.nes->loadProgram(romPath.UTF8String);
    return YES;
}

-(void)stepFrame {
    int currentFrame = self.nes->ppu->totalFrames;
    while (self.nes->ppu->totalFrames == currentFrame) {
        self.nes->emulateCycle();
    }
}

-(unsigned int)getDisplayBufferSize {
    return self.nes->DISPLAY_BUFFER_SIZE;
}

-(unsigned int*)getDisplayBuffer {
    return self.nes->displayBuffer;
}

-(void)getAudioBuffer {
    
}

@end
