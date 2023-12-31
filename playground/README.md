# Playground

This project allows to build, execute and test the app on Windows, macOS and Linux.


# Setup

- **CMake** is used as meta build tool
- **SDL2** is used to provide a (graphical) environment on all 3 major platforms


## Windows

### Build Playground.exe

To **build** Playground.exe you have to install **SDL2** on your Windows machine.

- Download **SDL2** from https://github.com/libsdl-org/SDL/releases/download/release-2.26.2/SDL2-devel-2.26.2-VC.zip
- Copy the folder *SDL2-2.26.2* to a folder (ex. *C:\Dev*)

after that create the Visual Studio (I use 2022) project with

```shell
    C:\> SET SDL2DIR={PATH_TO_FOLDER}
    C:\> cmake -B build -G"Visual Studio 17 2022"
```

Now you have a Visual Studio solution file which you can use in the *build* folder.
To execute Playground.exe from within Visual Studio, **SDL2.dll** has to be in the same folder as the executable.


## macOS

### Build Playground.app

To **build** Playground.app you have to install **SDL2** on your macOS machine.
For this I use homebrew (package manager).

```
    $ brew install sdl2
    $ brew install sdl2_image
    $ git clone https://github.com/rogerboesch/playground.git
    $ cd playground
```

after that create the XCode project with

```
    $ cmake -B build -GXCode
```

Now you have a XCode solution file which you can use in the *build* folder.


## Linux

### Build Playground

To **build** Playground you have to install **SDL2** on your Linux machine.

```
    $ sudo apt-get install libsdl2-dev
    $ sudo apt-get install libsdl2-image-2.0-0-dev
    $ git clone https://github.com/rogerboesch/playground.git
    $ cd playground
```

after that create the make file with

```
    $ cmake .
```

Now you have a make file which you can use in the *build* folder. To build it you simply type in

```
    $ make
```
