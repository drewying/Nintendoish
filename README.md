# Nintendoish

![alt tag](https://www.dropbox.com/s/yuu31rc0n40yqvg/Screens.png?raw=1)

This is a cycle accurate, scanline accurate, emulator for the Nintendo Entertainment System (NES), written completely from scratch.

The custom built emulation core is written in C++. The user interface is written in Swift.

While the emulator's primary user experience is for iOS, a very bare bones port of the emulator is also available for Windows and Mac OSX, created mostly to aide development of the emulation core.

This emulator is very much a continual work in progress.

### Features

- Support for Mappers 0/1/2/3/4/7/9. This makes it compatible with nearly every licensed USA game released. The only games not currently supported are a [handful of Mapper 5 games](http://bootgod.dyndns.org:7777/search.php?ines=5&group=groupid).
- Built-in ROM database and ROM manager, complete with artwork and game information
- Fast render pipeline built in Apple Metal
- Can import ROMs from any source which supports document browsing, including Dropbox, Google Drive, iCloud, and others.
- Retro 80's font

### Accuracy

The emulator can successfully emulate nearly all known [tricky to emulate games](https://wiki.nesdev.com/w/index.php/Tricky-to-emulate_games) including two notorious emulator accuracy challenges: it can successfully play Battletoads level 2 and 3 as well as render the status bar in Bart vs the Space Mutants correctly.

By the standards which [TAS Video rates emulators](http://tasvideos.org/EmulatorResources/NESAccuracyTests.html) the emulator scores 76% in accuracy, mostly because of the work in progress APU. The accuracy score bumps up to 96% when considering only PPU and CPU accuracy. 

### TODO
- Audio is highly unstable on iOS. (The audio on the desktop ports do much better)
- Improve APU emulation
- Add support for Mapper 5
- Add support for save/restore states
- Add support for battery based save data

