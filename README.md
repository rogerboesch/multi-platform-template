# Multiplatform template 

This is a template that supports pixel/line based games and apps on macOS, Windows, Linux AND Magic Leap 2

## Build code

### Magic Leap 2

#### Prerequisites

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

#### Build

```sh
$ gradlew assembleDebug
```

#### Install on Magic Leap 2 device

```sh
$ adb install app/build/outputs/apk/ml2/debug/com.magicleap.rbo.MyApp-debug.apk
```

#### Launch on Magic Leap 2 device

```sh
$ adb shell am start -a android.intent.action.MAIN -n com.magicleap.rbo.MyApp/android.appNativeActivity
```

#### Log

```sh
$ adb logcat -s "MyApp"
```

#### Stop app

```sh
$ adb shell am force-stop com.magicleap.rbo.MyApp
```

#### Remove from Magic Leap 2 Device

```sh
$ adb uninstall com.magicleap.rbo.MyApp
```


### macOS

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


# Windows 10/11

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


# Build from Source for Linux
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
