# Multiplatform template 

This is a template that supports pixel/line based games and apps on Magic Leap 2, macOS, Windows and Linux

## Magic Leap 2

### Prerequisites

Refer to https://developer.magicleap.cloud/learn/docs/guides/native/capi-getting-started

**Define Environment Variables**

```sh
$ source {MLSDK_PATH}/envsetup.sh
```

**Build the app_framework (first)**

```sh
$ cd %MLSDK%
$ cd samples\c_api\app_framework
$ python3 build.py --build device
$ set MAGICLEAP_APP_FRAMEWORK_ROOT=%MLSDK%\samples\c_api\app_framework\build\install
```

Now you're ready to build the project code for the ML2!

Important:
It might be needed, depending on the code you have, that you switch off **-Werror**
You do that by removing it from line ```101``` in file ```FindMagicLeapAppFramework.cmake```
Don't change in the original, but in the install folder %MAGICLEAP_APP_FRAMEWORK_ROOT%\cmake\FindMagicLeapAppFramework.cmake

```python
# asks for -Wall/-Werror
99  if(ML_TARGET STREQUAL ml2)
100    target_compile_options(app_framework INTERFACE -Wall -Werror ) <- Remove here
101 else()

```

### Build

```sh
$ gradlew assembleDebug
```

### Install app on device

```sh
$ adb install app/build/outputs/apk/ml2/debug/com.magicleap.rbo.MyApp-debug.apk
```

### Launch app on device

```sh
$ adb shell am start -a android.intent.action.MAIN -n com.magicleap.rbo.MyApp/android.appNativeActivity
```

### Log from device

```sh
$ adb logcat -s "MyApp"
```

### Stop app on device

```sh
$ adb shell am force-stop com.magicleap.rbo.MyApp
```

### Remove app from device

```sh
$ adb uninstall com.magicleap.rbo.MyApp
```


## macOS

To build ```MyApp.app``` you have to install ```SDL2``` on your macOS machine. For this I use homebrew (package manager).

```sh
    $ brew install sdl2
    $ git clone https://github.com/rogerboesch/multi-platform-template.git
    $ cd multi-platform-template/playground
```

after that create the XCode project with

```sh
    $ cmake -B build -GXCode
```

Now you have a XCode solution ```MyApp.sln``` file which you can use in the ```build``` folder.


## Windows 10/11

1. Download SDL2 from https://github.com/libsdl-org/SDL/releases/download/release-2.26.2/SDL2-devel-2.26.2-VC.zip
2. Copy the folder SDL2-2.26.2 to a folder (ex. C:\Dev)

```sh
    C:\> SET SDL2DIR={PATH_TO_FOLDER}
    C:\> git clone https://github.com/rogerboesch/multi-platform-template.git
    C:\> cd multi-platform-template\playground
```

after that create the Visual Studio (I use 2022) project with

```sh
    C:\> cmake -B build -G"Visual Studio 17 2022"
```

Now you have a Visual Studio solution file ```MyApp.sln``` which you can use in the ```build``` folder.
To execute ```MyApp.exe``` from within Visual Studio, ```SDL2.dll``` has to be in the same folder as the executable.


## Linux

To build Playground you have to install SDL2 on your Linux machine.

```sh
    $ sudo apt-get install libsdl2-dev
    $ git clone https://github.com/rogerboesch/multi-platform-template.git
    $ cd multi-platform-template\playground
```

after that create the make file with

```sh
    $ cmake .
```

Now you have a ```Makefile``` which you can use in the ```build``` folder. To build it you simply type in

```sh
    $ make
```

After you can execute it directly 

```sh
    $ ./MyApp
```

## Your App

There is already a template app file in the folder ```app\src\main\cpp\myapp```

This is the starting point where you can add your platform independent app/game logic.


```c++
#include "rb_base.h"
#include "rb_platform.h"

extern "C" {
    void on_app_start() {                                               // 1.)
        // Called when app starts

        // platform_on_init() must be called here
        platform_on_init("MyApp", 320, 240);                            // 2.)
    }
    void on_app_frame() {                                               // 3.)
        // Called every frame
    }
    void on_app_stop() {                                                // 4.)
        // Called before app quits
    }

    void on_app_key_pressed(int code, int state) {                      // 5.)
        // Handle key pressed
    }

    rgb_color app_get_palette_color(byte color, byte brightness) {      // 6.)
        // Return palette color
        return { 255, 0, 0 };
    }
}
```

1. This function gets called on app start
2. It's essential that you callback platform_init() (see below) inside of this function
3. This function gets called every frame (normally 60fps)
4. This function gets called before the app quits
5. This function gets called when a key is pressed. The values from other input devices will also be translated to "key codes"
6. The entire template works with palette colors. From this function you have to deliver RGB values for the palette color given 

So those are the interface functions between the template and your code.
On the other hand there are several platform functions you can call.
The most important you find below: 

```c++
    void platform_on_init(const char* name, int width, int height);                     // 1.)
    void platform_set_pixel(int x, int y, byte color, int brightness);                  // 2.)
    void platform_draw_line(int x1, int y1, int x2, int y2, byte color, int invert);    // 3.)
```

1. Initialize MyApp by pass an app name, screen with and height
2. Draw a pixel (used for raster graphic games/apps)
3. Draw a line (used for vector based games/apps)