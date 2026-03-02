# Changelog
### v1.4.4-win32-release
- Account for portable backport effort by creating `settings` and `screenshot` folders in Amnesia game directory
- Added customizable 24-bit bitmap icon which can be set as the icon for the game's title bar
- Fixed buggy screenshots that used an incorrect pixel format (`RGBA`)
- Replaced `GL_TEXTURE_RECTANGLE_NV` with standard `GL_TEXTURE_RECTANGLE`
- Replaced `std::to_wstring` with `LongToWString` in the main menu
- Removed all references to haptics and HaptX code
- Removed SDL2-specific code related to device broadcasting, sleeping and out-of-focus window interactions
