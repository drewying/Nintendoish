//
//  ContentView.swift
//  SwiftUITestApp
//
//  Created by Ingebretsen, Andrew (HBO) on 6/4/19.
//  Copyright © 2019 Ingebretsen, Andrew (HBO). All rights reserved.
//

import SwiftUI
import UIKit

struct RomBrowserView<StoreType : RomStore>: View {
    
    @ObjectBinding var romStore : StoreType
    
    func deleteRom(at offsets: IndexSet) {
        guard let index = offsets.first else { return }
        let rom = romStore.roms[index]
        romStore.deleteRom(id: rom.id)
    }
    
    struct AddButton<StoreType : RomStore>: View {
        var romStore : StoreType
        
        var body: some View {
            #if DEBUG
            return Button(action: {
                self.romStore.addRom(romData: Data())
            }, label: { Text("ADD") })
            #else
            return PresentationButton(Text("ADD"), destination: RomPickerView<StoreType>(store: romStore))
            #endif
        }
    }
    
    var body: some View {
        NavigationView {
            if romStore.roms.count == 0 {
                Text("NO ROMS IN LIBARY\nPRESS ADD")
                    .multilineTextAlignment(.center)
                    .lineLimit(2)
                    .navigationBarItem(
                        title: Text(""),
                        titleDisplayMode: .inline)
                    .navigationBarItems(trailing: AddButton(romStore: romStore))
                
            } else {
                List() {
                    ForEach(romStore.roms) { rom in
                        RomCell(rom: rom)
                        }
                        .onDelete(perform: deleteRom)
                    }
                    .navigationBarTitle(Text(""), displayMode: .inline)
                    .navigationBarItems(trailing: AddButton(romStore: romStore))
            }
        }
        .font(Font.custom("PixelNES", size: 18))
    }
}

struct RomCell: View {
    var rom:RomViewModel
    init(rom:RomViewModel) {
        self.rom = rom
    }
    var body: some View {
        NavigationButton(destination:RomDetailView(rom:rom)) {
            HStack {
                Image(uiImage: rom.image)
                    .resizable()
                    .aspectRatio(0.72, contentMode: .fit)
                    .padding(8)
                    .layoutPriority(1)
                Text(rom.strippedName)
                    .font(Font.custom("PixelNES", size: 18))
                    .lineLimit(2)
                }
                .frame(width: nil, height: 130, alignment: .leading)
        }
    }
}

#if DEBUG
struct ContentView_Previews : PreviewProvider {
    static var previews: some View {
        RomBrowserView<ExplicitRomStore>(romStore: ExplicitRomStore())
    }
}
#endif
