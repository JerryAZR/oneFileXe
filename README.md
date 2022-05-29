# oneFileXe

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Windows Build](https://github.com/JerryAZR/oneFileXe/actions/workflows/windows-build.yml/badge.svg)](https://github.com/JerryAZR/oneFileXe/actions/workflows/windows-build.yml)

Pack your application along with all its dependencies into a single executable!
The original application does not have to be statically linked.

## Important

* Unlike the legacy version, this is a Qt-g++ based version that is combatible
with [upx](https://upx.github.io/)
* Although this software is intended to be cross-platform, I would highly recommend using
[AppImage](https://appimage.org/) if you are on Linux.

## Usage

1. Create an application directory. Place the target program and all its
dependencies inside. This directory will be packed with the program loader
2. Create an entry-point file inside the application directory,
{AppRun / AppRun.exe / AppRun.bat, depending on the operating system},
or rename an existing file. This is the file the program loader will run,
so it's highly recommended to test it properly.
3. Run `onefilexe <AppDir> [options]` to generate a one-file executable

Here is an example:

```
## before packing
./onefilexe.exe
./myApp/
    | AppRun.exe
    | libgcc_s_seh-1.dll
    | libstdc++-6.dll
    | libwinpthread-1.dll
    | src/
        | main.cpp
        | Makefile
```

```
## pack
./onefilexe ./myApp/ -o out.exe
```

## How It Works

### Basic

A Qt project is created for each packed application. Everything in the
application directory is added to the Qt resource system. The project is then
statically built with a loader which unpacks the application directory and runs
`AppRun` on start up.
