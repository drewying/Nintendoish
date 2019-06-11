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
    
    func delete(at offsets: IndexSet) {
        guard let index = offsets.first else { return }
        let rom = romStore.roms[index]
        romStore.deleteRom(id: rom.id)
    }
    
    var body: some View {
        NavigationView {
            List() {
                ForEach(romStore.roms) { rom in
                    NavigationButton(
                    destination:RomDetailView(rom:rom)) {
                        RomCell(rom: rom)
                        }
                }.onDelete(perform: delete)
            }
                .navigationBarItem(title: Text("ROMS"), titleDisplayMode: .inline)
                .navigationBarItems(trailing: PresentationButton(Text("Add"), destination: RomPickerView<StoreType>(store: romStore)))
        }
    }
}

struct RomCell: View {
    var rom:RomViewModel
    init(rom:RomViewModel) {
        self.rom = rom
    }
    var body: some View {
        HStack {
            Image(uiImage: rom.image)
                .resizable()
                .aspectRatio(0.72, contentMode: .fit)
                .padding(8)
                .layoutPriority(1)
            Text(rom.strippedName)
                .font(Font.custom("PixelNES", size: 20))
                .lineLimit(2)
        }
            .frame(width: nil, height: 150, alignment: .leading)
    }
}

#if DEBUG
struct ContentView_Previews : PreviewProvider {
    static var previews: some View {
        RomBrowserView<ExplicitRomStore>(romStore: ExplicitRomStore())
    }
}
#endif
