system {
    hostname = "fir"

    bootloader.grub.enable = true
    // bootloader.limine.enable = true


    user.john {
        permissions = ["wheel", "network"]
        shell = "/bin/bash"
    }

    packages = {
        global = [
            "gcc",
            "make",
            "git",
            "pipewire",
        ]
        
        user.john = [
            "firefox",
            "vim",
            "tree",
        ]
    }

}
