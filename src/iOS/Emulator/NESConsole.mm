//
//  NESBridge.m
//  NintendoishiOS
//
//  Created by Drew Ingebretsen on 6/5/18.
//  Copyright © 2018 Drew Ingebretsen. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "NES.h"
#include "NESConsole.h"


@interface NESConsole()
@property NES::Console *nes;
@end

@implementation NESConsole


-(instancetype)init {
    self = [super init];
    if (self) {
        [self initializeConsole];
    }
    return self;
}

-(void) dealloc {
    delete self.nes;
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

-(void)clearAudioBuffer {
    self.nes->audioBufferLength = 0;
}

-(unsigned int)getAudioBufferLength {
    return self.nes->audioBufferLength;
}

-(float*)getAudioBuffer {
    return self.nes->audioBuffer;
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
