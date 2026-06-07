# Amnesia: Legacy Edition
<div id="toc" align="center">
    <img width="1024" height="512" alt="Amnesia: Legacy Edition" src="https://github.com/user-attachments/assets/e068ab0a-e6d4-4c9e-abfb-6b1b39ba69d0" />
</div>

## Information
**A legal, unmodified and DRM-free copy of *Amnesia: The Dark Descent* and/or *Amnesia: A Machine for Pigs* is required.**

*Amnesia: Legacy Edition* is a port of *Amnesia: The Dark Descent*, *Amnesia: A Machine for Pigs* and their editors to significantly weaker (or unorthodox) hardware, usually below the minimum official specifications. Besides this, the project also has several bugfixes and enhancements to the engine and base game.

For a list of changes, read [`CHANGELOG.md`](CHANGELOG.md). Special thanks are in [`THANKS.md`](THANKS.md).

### IMPORTANT!

**WARNING: If you have the latest Steam release of Amnesia: The Dark Descent version 1.5, Custom Stories and Total/Full Conversion mods that were downloaded from the Steam Workshop WILL NOT WORK WITH THIS PORT! This is because Frictional Games has not provided the source code for version 1.5 yet (and there is no ETA on when that will happen! IT MEANS THIS PORT IS COMPLETELY MISSING STEAM WORKSHOP SUPPORT!**

With this port, you should download and install mods from their original sources (*ModDB*, *Nexus Mods*, *GameBanana*) as long as they support at least **version 1.4.3** (also called version 1.41b, *GOG.com* and *Epic Games Store* copies have this version)! Additionally, you may convert *Steam Workshop* mods to make them compatible with version 1.4.3, but this is a **MANUAL PROCESS** which requires some experience and familiarity with the HPL2 engine, and is **NOT RECOMMENDED!**

*DirectInput* controller support is very poor via SDL 1.2.15 (see issue #21). *XInput* controllers, however, do work and are fully functional with this port, but in order to use *XInput* controllers (for example, any *Xbox 360*, *Xbox One*, and/or *Xbox Series* controllers), you must add the `USE_XINPUT` preprocessor definition to both the `HPL2` and `amnesia` projects on the `Debug` and `Release` configurations.

**Compiled binaries that use this preprocessor definition (`USE_XINPUT`) must require at least *Windows XP Service Pack 1*.**

## Installation
### Steps
1. Go to the Releases section which can be found on the right-hand side of the page.
2. Download the zip file of the latest release (for example, `v1.4.5-win32-release`)
3. Open the zip file and drag all of its contents into your *Amnesia: The Dark Descent* installation folder.

### System Requirements
- 1.8+ GHz single-core CPU
- *OpenGL* 2.1-compatible graphics card
- *DirectX* and/or *OpenAL*-compatible sound card

## Building
### Prerequisites
- [*Creative Labs OpenAL 1.1 Core SDK*](https://openal.org/downloads)
- [*Microsoft DirectX SDK February 2010*](https://archive.org/download/dxsdk_feb10/DXSDK_Feb10.exe)
- *Microsoft Visual Studio 2005*
- [*Microsoft Visual Studio 2005 Service Pack 1*](https://archive.org/download/vs80sp1-all-langs/SP1/)
- [*Microsoft Visual Studio 2005 + Service Pack 1 Updates*](https://archive.org/download/vs80sp1-all-langs/sp1-updates/)

You will also need to [configure the *DirectX SDK* in your *Visual C++* directories](https://stackoverflow.com/a/46762539).

### Steps
Dependencies must be manually compiled via `dependencies.sln` first to avoid LNK4099 warnings as with issue #25.

1. `git clone` the repository or download it from the **Code** button.
2. Open the `atrim.sln` solution file.
3. Press F7 to build the solution. This will compile the `HPL2` project first and then the `amnesia` project.

Compiled binaries are found in the `compiled` folder of the solution's root and in it there should be a folder for the configuration you chose to compile the project (`debug`, `release`).

## License
*Amnesia: Legacy Edition* is licensed under Version 3 of the GNU General Public License (GNU GPL).

Read the license information via the **GPL-3.0 license** tab on the top, or open the [`LICENSE.md`](LICENSE.md) file.