# Amnesia 2000: The Dark Descent
<img width="1692" height="1332" alt="Amnesia2K" src="https://github.com/user-attachments/assets/9b26bfa7-55ec-440b-9225-ab4d84c18180" />

Also known as Amnesia2K or `atrim`. This is a backport of Frictional Games' *Amnesia: The Dark Descent* (and *A Machine for Pigs* coming soon), and a fork of Amnesia64 for the Microsoft *Windows 2000* operating system.

For residents living in the states of California and Colorado in the United States of America, please read the legal notice in [`LEGAL-NOTICE.md`](LEGAL-NOTICE.md)

For a list of key changes, see [`CHANGELOG.md`](CHANGELOG.md).

## NOTE about gamepad support
Both *DirectInput* and *XInput* controllers work and are fully functional with this port.

To use *DirectInput* or *XInput*-compatible controllers, you will need to install the *DirectX End-User Runtimes*:

https://www.techpowerup.com/download/directx-redistributable-runtime/

If you are on *Windows 2000* and want to use a controller to play the game, install the February 2010 version of the *DirectX End-User Runtimes*:

https://legacyupdate.net/download-center/download/9033/directx-end-user-runtimes-february-2010

## Prerequisites

You need *Visual Studio 2005* with *Service Pack 1* and some updates. For legal reasons I cannot provide an iso or crack for it (you can look elsewhere), but you can find most of the updates required to build the dependencies and solutions from this Archive.org repository:

https://archive.org/download/vs80sp1-all-langs/SP1/

https://archive.org/download/vs80sp1-all-langs/sp1-updates/

To compile the game on *Windows 2000* with *Direct*/*XInput* support, you will need the February 2010 version of the *DirectX SDK*:

https://archive.org/download/dxsdk_feb10/DXSDK_Feb10.exe

## Building

Once you have installed all of the necessary updates and the *DirectX SDK* is [properly configured in your *Visual C++* directories](https://stackoverflow.com/a/46762539), you can now start to compile the required dependencies for the `HPL2` project first (via `dependencies.sln`), then the `HPL2` and `amnesia` projects themselves (via `atrim.sln`). You have two configurations to pick:

- **Release**

This is the configuration that should be used when you set up the project for the first time. This has full code optimization and typically is the most stable especially for playing the base game and its *Justine* DLC, custom stories, total and full conversion mods.

Due to its optimization, debugging may be slightly more difficult; you should use the **Debug** configuration for that instead.

- **Debug**

This is the least stable configuration, without any code optimization. While it can be used to play *Amnesia: The Dark Descent* and its mods, performance is quite slow; more specifically, crashes can happen and are imminent. This configuration is only useful for diagnosing bugs and crashes, and using a hardware breakpoint should be necessary if you are looking to fix them.

If you just want to play some mods and enjoy the game, the **Debug** configuration is not designed for this purpose at all; you should use the **Release** configuration.

## How to debug after compilation?

1. Right-click on the `amnesia` project and set it as a startup project.
2. Go to the `amnesia` project properties and click on `Debugging`
3. Change the `Working Directory` to the path of your *Amnesia: The Dark Descent* install.

For example this path is `C:\Games\AmnesiaTDD_Work\`

**You MUST add a backslash at the end of the path when specifying your own.**
