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

-(void)pressButton:(NESButton)button {
    switch (button) {
        case A:
            self.nes->controllerOne->a = true;
            break;
        case B:
            self.nes->controllerOne->b = true;
            break;
        case START:
            self.nes->controllerOne->start = true;
            break;
        case SELECT:
            self.nes->controllerOne->select = true;
            break;
        case UP:
            self.nes->controllerOne->up = true;
            break;
        case DOWN:
            self.nes->controllerOne->down = true;
            break;
        case LEFT:
            self.nes->controllerOne->left = true;
            break;
        case RIGHT:
            self.nes->controllerOne->right = true;
            break;
    }
}

-(void)releaseButton:(NESButton)button {
    switch (button) {
        case A:
            self.nes->controllerOne->a = false;
            break;
        case B:
            self.nes->controllerOne->b = false;
            break;
        case START:
            self.nes->controllerOne->start = false;
            break;
        case SELECT:
            self.nes->controllerOne->select = false;
            break;
        case UP:
            self.nes->controllerOne->up = false;
            break;
        case DOWN:
            self.nes->controllerOne->down = false;
            break;
        case LEFT:
            self.nes->controllerOne->left = false;
            break;
        case RIGHT:
            self.nes->controllerOne->right = false;
            break;
    }
}

@end
