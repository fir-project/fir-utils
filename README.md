# Fir utils

this repo contains the utils for the fir(in beta) linux distro

## tasks
[ ] core commands (ls, cd, mkdir, cat, etc)
[x] fir config parser
[ ] QoL stuff

## fir-parser

parses a `.fir` config into a plain text build report

```
cd fir-parser
make
./fir-parser examples/config.fir -o report.txt
```

if `-o` is omitted the report is written next to the config with a `.txt` extension

### example config

```
system {
    hostname = "fir"

    user.emil {
        permissions = ["wheel", "network"]
        shell = "/bin/bash"
    }

    packages = {
        global = ["gcc", "make", "git"]
        user.emil = ["firefox", "steam"]
    }
}
```

### output

```
FIR CONFIG BUILD:

HOSTNAME FIR
CREATE USER EMIL
SET SHELL /bin/bash
GIVE USER EMIL PERMISSIONS:
     WHEEL
     NETWORK

PACKAGES:
     GLOBAL:
        GCC
        MAKE
        GIT
     USER EMIL:
        FIREFOX
        STEAM
```
