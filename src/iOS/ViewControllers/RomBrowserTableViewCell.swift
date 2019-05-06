//
//  RomBrowserTableViewCell.swift
//  Nintendoish
//
//  Created by Drew Ingebretsen on 6/13/18.
//  Copyright © 2018 Drew Ingebretsen. All rights reserved.
//

import UIKit

class RomBrowserTableViewCell: UITableViewCell {

    @IBOutlet weak var coverImage: UIImageView!
    @IBOutlet weak var titleLabel: UILabel!
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

}
