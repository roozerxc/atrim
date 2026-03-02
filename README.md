# Amnesia 2000: The Dark Descent
<img width="1692" height="1332" alt="Amnesia2K" src="https://github.com/user-attachments/assets/9b26bfa7-55ec-440b-9225-ab4d84c18180" />

### WARNING: THIS IS UNFINISHED

Also known as Amnesia2K or `atrim`. This is a backport of Frictional Games' Amnesia: The Dark Descent (and A Machine for Pigs coming soon), and a fork of Amnesia64 for the Microsoft Windows 2000 operating system.

For a list of key changes please see `CHANGELOG.md`

## Installation

Go to Releases on the right-hand side of the repository page and click on the latest version that appears. You should see a zip file which contains the precompiled binaries for Amnesia2K. Then download and extract its contents to your Amnesia game folder (where Amnesia.exe typically is found)

The binaries are compiled through the msvc compiler, so you will need to install the Visual C++ Redistributables:

https://www.techpowerup.com/download/visual-c-redistributable-runtime-package-all-in-one/

## Building

You need Visual Studio 2005 with Service Pack 1 and some updates. For legal reasons I cannot provide an iso or crack for it (you can look elsewhere), but you can find most of the updates required to build the dependencies and solutions from this archive repository:

https://archive.org/download/vs80sp1-all-langs/SP1/

https://archive.org/download/vs80sp1-all-langs/sp1-updates/

Once you have installed all of the necessary updates, you can now start to compile the required dependencies for `HPL2` first (via `dependencies.sln`), then `HPL2` and the `amnesia` project itself (via `atrim.sln`). You have two configurations to pick:

- **Release**

This is the configuration that should be used when you set up the project for the first time. This is optimal if you want to quickly test new additions to your source code

- **Debug**

This is the slowest and least stable, without any code optimization. This is useful ONLY for diagnosing bugs and crashes, and using some sort of hardware breakpoint should be next to essential. Due to its slow performance it is not ideal to play the game using a Debug build; please use the Release build instead

## How to debug after compilation?

If you want to use the built-in debugger against the `amnesia` project regardless of configuration, right-click on the project and set it as a startup project

To make the debugger work with your game, go to the project's properties and click `Debugging`. Then change the Working Directory to the path of your Amnesia: The Dark Descent install

For example this path is `C:\Games\AmnesiaTDD_Work\` and you MUST have a backslash at the end of the path when you are specifying your own.

## NOTE about gamepad support

Gamepads are still supported, but you need to plug in your controller before you open the game. DirectInput and XInput controllers do work and are fully functional with this port. To use XInput-supported or compatible controllers, you will need to install the DirectX End-User Runtimes:

https://www.techpowerup.com/download/directx-redistributable-runtime/
