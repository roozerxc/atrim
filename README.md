# Amnesia 2000: The Dark Descent
<img width="1692" height="1332" alt="Amnesia2K" src="https://github.com/user-attachments/assets/9b26bfa7-55ec-440b-9225-ab4d84c18180" />

### WARNING: THIS IS UNFINISHED

Also known as Amnesia2K or `atrim`. This is a backport of Frictional Games' Amnesia: The Dark Descent (and A Machine for Pigs coming soon), and a fork of Amnesia64 for the Microsoft Windows 2000 operating system.

For a list of key changes please see `CHANGELOG.md`

## Building

You need Visual Studio 2005 with Service Pack 1 and some updates. For legal reasons I cannot provide an iso or crack for it (you can look elsewhere), but you can find most of the updates required to build the dependencies and solutions from this archive repository:

https://archive.org/download/vs80sp1-all-langs/SP1/

https://archive.org/download/vs80sp1-all-langs/sp1-updates/

Once you have installed all of the necessary updates, you can now start to compile the required dependencies for `HPL2` first (via `dependencies.sln`), then `HPL2` and the `amnesia` project itself (via `atrim.sln`). You have two configurations to pick:

- **Release**

This is the configuration that should be used when you set up the project for the first time. This has full code optimization and typically is the most stable especially for playing the base game and its Justine DLC, custom stories, total and full conversion mods.

Due to its optimization, debugging may be slightly more difficult; you should use the Debug configuration for that instead.

- **Debug**

This is the least stable configuration, without any code optimization. While it can be used to play Amnesia and its mods, performance is quite slow; more specifically, crashes can happen and are imminent. This configuration is only useful for diagnosing bugs and crashes, and setting a hardware breakpoint should be necessary if you are looking to fix them.

If you just want to play some mods and enjoy the game, the Debug configuration is not designed for this purpose at all; you should use the Release configuration.

## How to debug after compilation?

1. Right-click on the `amnesia` project and set it as a startup project.
2. Go to the `amnesia` project properties and click on `Debugging`
3. Change the `Working Directory` to the path of your Amnesia: The Dark Descent install.

For example this path is `C:\Games\AmnesiaTDD_Work\`

**You MUST add a backslash at the end of the path when you are specifying your own.**

## NOTE about gamepad support

Gamepads are still supported, but you need to plug in your controller before you open the game. DirectInput and XInput controllers do work and are fully functional with this port.

To use XInput-supported or compatible controllers, you will need to install the DirectX End-User Runtimes:

https://www.techpowerup.com/download/directx-redistributable-runtime/
