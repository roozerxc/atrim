# atrim
**A legal, unmodified and DRM-free copy of *Amnesia: The Dark Descent* and/or *Amnesia: A Machine for Pigs* is required.**

atrim is a client for Frictional Games' *Amnesia: The Dark Descent* that offers bug fixes and enhancements to the HPL2 engine and base game. Coming soon will be additional fixes and enhancements from, along with support for, The Chinese Room's *Amnesia: A Machine For Pigs*.

The game and engine can also be ported to significantly weaker (or unorthodox) hardware which are usually below the official minimum specifications.

atrim is currently being maintained by one person ([@RoozerXC](https://github.com/roozerxc)). If you want to contribute to atrim, feel free to fork and submit your own pull requests, as long as everything follows the [Code of Conduct (or Anti-CoC)](CODE-OF-CONDUCT.md).

> [!IMPORTANT]
> - **This port is based on the official *Amnesia: The Dark Descent* version 1.4.3 (1.41b) source code release from October 12th, 2020.**
> - **Mods installed from the Steam Workshop, including Custom Stories and Full Conversion mods will not work here!**

For a list of changes, read [`CHANGELOG.md`](CHANGELOG.md). Special thanks are in [`THANKS.md`](THANKS.md).

## Installation
### Steps
1. Go to the Releases section which can be found on the right-hand side of the page.
2. Download the zip file of the latest release (for example, `v1.4.5-win32-release`)
3. Open the zip file and drag all of its contents into your *Amnesia: The Dark Descent* installation folder.

### System Requirements
- 1.8+ GHz single-core CPU
- *OpenGL* 2.1-compatible graphics card
- *DirectSound* and/or *OpenAL*-compatible sound card

## Building & Debugging
### Prerequisites
- [*Creative Labs OpenAL 1.1 Core SDK*](https://openal.org/downloads)
- [*Microsoft DirectX SDK February 2010*](https://archive.org/download/dxsdk_feb10/DXSDK_Feb10.exe)
- *Microsoft Visual Studio 2005*
- [*Microsoft Visual Studio 2005 Service Pack 1*](https://archive.org/download/vs80sp1-all-langs/SP1/)
- [*Microsoft Visual Studio 2005 + Service Pack 1 Updates*](https://archive.org/download/vs80sp1-all-langs/sp1-updates/)

You will also need to [configure the *DirectX SDK* in your *Visual C++* directories](https://stackoverflow.com/a/46762539).

### Steps
#### Dependencies
> [!IMPORTANT]
> **You must compile the dependencies first before building `atrim.sln`**
1. `git clone` the repository or download it from the **Code** button.
2. Open the `HPL2/dependencies` folder and open the `dependencies.sln` solution file.
3. Press `F7` to build the solution. This will compile the dependencies needed for the `HPL2` project.

#### Engine and Game
1. Open the `atrim.sln` solution file.
2. Right click on the `amnesia` project and in **Properties** change the **Working Directory** to your *Amnesia: The Dark Descent* install folder.
3. Press `F5` to debug the solution. This will build the `HPL2` project first, then `amnesia`, and then launch the game after building.

## License
atrim is licensed under Version 3 of the GNU General Public License (GNU GPL).

Read the license information via the **GPL-3.0 license** tab on the top, or open the [`LICENSE.md`](LICENSE.md) file.

© 2009-2010 Frictional Games. Frictional, Amnesia: The Dark Descent, and the HPL Engine software are all registered trademarks of Frictional Games. All rights reserved.
