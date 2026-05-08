# Amnesia 2000: The Dark Descent
<div id="toc" align="center">
    <img width="640" height="320" alt="Amnesia 2000: The Dark Descent" src="https://repository-images.githubusercontent.com/1168530724/f43a0b07-28d1-4433-a6ff-552195fff4c3" />
</div>

## Information
**NOTICE: A legal, unmodified and DRM-free copy of *Amnesia: The Dark Descent* and/or *Amnesia: A Machine for Pigs* is required.**

*Amnesia 2000: The Dark Descent* (also known as *atrim*) is a port of *Amnesia: The Dark Descent*, *Amnesia: A Machine for Pigs* and their editors to the Microsoft *Windows 2000* operating system.

The core purpose of this project is to run the game on significantly weaker hardware below the minimum official specifications. Besides this, the project also has several bugfixes and enhancements to the engine and base game.

For a list of changes, read [`CHANGELOG.md`](CHANGELOG.md). Special thanks are in [`THANKS.md`](THANKS.md).

## Building
### Prerequisites

- [*Creative Labs OpenAL 1.1 Core SDK*](https://openal.org/downloads)
- [*Microsoft DirectX SDK February 2010*](https://archive.org/download/dxsdk_feb10/DXSDK_Feb10.exe)
- *Microsoft Visual Studio 2005*
- [*Microsoft Visual Studio 2005 Service Pack 1*](https://archive.org/download/vs80sp1-all-langs/SP1/)
- [*Microsoft Visual Studio 2005 + Service Pack 1 Updates*](https://archive.org/download/vs80sp1-all-langs/sp1-updates/)
- *Microsoft Windows 2000 Professional Service Pack 4*

You will also need to [configure the *DirectX SDK* in your *Visual C++* directories](https://stackoverflow.com/a/46762539).

### Steps
The dependency libraries are precompiled, so you don't have to recompile them as from before.

1. `git clone` or download the repository from the **Code** button.
2. Open the `atrim.sln` solution file.
3. Press F7 to build the solution. This will compile the `HPL2` project first and then the `amnesia` project.

Compiled binaries are found in the `compiled` folder of the solution's root. In order to play the game with these binaries, you need to copy `amnesia-Win32-Release.exe` and paste it into your *Amnesia: The Dark Descent* installation folder. 

### Debugging
1. Right-click on the `amnesia` project and set it as a **StartUp Project**.
2. Go to the `amnesia` project properties and click on `Debugging`
3. Change the `Working Directory` field to the path of your *Amnesia: The Dark Descent* install.

For example this path is `C:\Games\AmnesiaTDD_Work\`

**You MUST add a backslash at the end of the path when specifying your own.**

To load full conversions mod via the debugger, edit the `Command Arguments` field. For example, to load [In Lucy's Eyes](https://www.moddb.com/mods/in-lucys-eyes) with the atrim client:
```
InLucysEyes/config/main_init.cfg
```
You can also test regular *Justine* functionality by simply adding `ptest` to the `Command Arguments` field. Please note that this will **NOT** work with full conversion mods.

Then, click **Apply** and **OK**.

## Project Configurations
### Release

This is the configuration that should be used when you set up the project for the first time. This has full code optimization and typically is the most stable especially for playing the base game and its *Justine* DLC, custom stories, total and full conversion mods.

### Debug

This is the least stable configuration, without any code optimization. While it can be used to play *Amnesia: The Dark Descent* and its mods, performance is quite slow; more specifically, crashes can happen and are imminent. This configuration is only useful for diagnosing bugs and crashes, and using a hardware breakpoint should be necessary if you are looking to fix them.

If you just want to play some mods and enjoy the game, the **Debug** configuration is not designed for this purpose at all; you should use the **Release** configuration.

## NOTE about gamepad support
**WARNING: Only *DirectInput* controllers are supported on *Windows 2000*! Attempting to compile the game and engine with the `USE_XINPUT` preprocessor definition will make the compiled binaries not work on *Windows 2000* unless you use an extended kernel!**

Both *DirectInput* and *XInput* controllers work and are fully functional with this port, however in order to use *XInput* controllers (for example, any *Xbox 360*, *Xbox One*, and/or *Xbox Series* controllers), you must add the `USE_XINPUT` preprocessor definition to the `HPL2` and `amnesia` projects on the `Debug` and `Release` configurations. Compiled binaries that use this preprocessor definition must require at least *Windows XP Service Pack 1*.

To use *DirectInput* or *XInput*-compatible controllers, you will need to install the *DirectX End-User Runtimes*:

https://www.techpowerup.com/download/directx-redistributable-runtime/

If you are on *Windows 2000* and want to use a *DirectInput* controller to play the game, install the February 2010 version of the *DirectX End-User Runtimes*:

https://legacyupdate.net/download-center/download/9033/directx-end-user-runtimes-february-2010

## License
The atrim project is licensed under Version 3 of the GNU General Public License (GNU GPL).

Read the license information via the **GPL-3.0 license** tab on the top, or open the [`LICENSE.md`](LICENSE.md) file.
