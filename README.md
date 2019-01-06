# Nintendoish

![alt tag](https://www.dropbox.com/s/yuu31rc0n40yqvg/Screens.png?raw=1)

This is a cycle accurate, scanline accurate, emulator for the Nintendo Entertainment System (NES), written completely from scratch.

The custom built emulation core is written in C++. The user interface is written in Swift.

While the emululator's primary user experience is for iOS, a very bare bones ports of the emulator is also available for Windows and Mac OSX, mostly meant to be used for development purposes.

This emulator is very much a continual work in progress.

### Features

- Support for Mappers 0/1/2/3/4/7/9. This makes it compatible with nearly every licensed USA game released. The only games not currently supported are a handful of Mapper 5 games, (such as Castlevania III)
- Built in ROM database and ROM manager, complete with artwork and game information.
- Fast render pipeline built in Apple Metal
- Retro 80's font.

### Accuracy

The emulator current can successfully emulate nearly all known [tricky to emulate games](https://wiki.nesdev.com/w/index.php/Tricky-to-emulate_games) including two most notorious emulator accuracy challenges: it can successfully play Battletoads level 2 and 3 as well as render the status bar in Bart vs the Space Mutants correctly.

Although not a good measure of emulator accuracy, by [TAS Video nes emulator accuracy standards](http://tasvideos.org/EmulatorResources/NESAccuracyTests.html) this emulator scores 76% in accuracy, mostly because of the work in progress APU. That number bumps up to 96% when considering only PPU and CPU accuracy.

### TODO
- Add support for save/restore states
- Add support for battery based save data
- Add support for Mapper 5
- Sound is unstable on iOS. (Desktop ports do well)
- Better APU emulation
- Android port
