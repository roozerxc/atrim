# Changelog
### v1.4.6-win32-release (currently in private testing)
- [`067186b`](https://github.com/roozerxc/atrim/commit/067186ba7c6c50bfe807f7d07bd356d904b3ccca) Added customizable top menu font color settings
- [`bc98941`](https://github.com/roozerxc/atrim/commit/bc989418e2455b48983440c0dfd17386dbfc6794) Added map cache version number detection
- [`0d132a5`](https://github.com/roozerxc/atrim/commit/0d132a505bff363d721a56bc014e57196b45b404) Added "Reset List" button for Custom Story menu
- [`ef6c70e`](https://github.com/roozerxc/atrim/commit/ef6c70e534c2eb12e7cc8371bae3f74a15590c2d) Decoupled timers and scripts to use their own functions instead of using the same "Run" function
- [`c2d7155`](https://github.com/roozerxc/atrim/commit/c2d7155de5f6522622a7174a201c45a221433d45) Fixed buggy enemy collision behavior
- [`b496952`](https://github.com/roozerxc/atrim/commit/b496952608936ed83cde7a959521c7ec5c01ffff) Fixed custom story pictures not being wiped when another custom story was selected
- [`73424fd`](https://github.com/roozerxc/atrim/commit/73424fd9c3410726fa91b79d674709298a9383ff) Fixed invalid serialization of coins needed to unlock chests
- [`8c946b1`](https://github.com/roozerxc/atrim/commit/8c946b1a695330e05308fb5df4db5791a204ea3c) Fixed missing xinput support
- [`5c29b95`](https://github.com/roozerxc/atrim/commit/5c29b958d314dd2ba9d2cb44e5e7b86c25e22ea4) Fixed the SDL backquote/grave (`` ` ``) key which incorrectly returned as a backslash (`\`)
- [`45bda0f`](https://github.com/roozerxc/atrim/commit/45bda0fb26a98d19165b91bbd6aa79f5ad7bc2e6) Removed fake anti-aliasing shader (Edge Smoothing)
- [`7491cc4`](https://github.com/roozerxc/atrim/commit/7491cc43e3e7b712c8723bfb12038d2146264d2c) Replaced loading icons from loose bitmaps with header-based RWops in the executable
- [`0a3de4d`](https://github.com/roozerxc/atrim/commit/0a3de4d6e60806f30c31cb48281bd80c29efd79a) Replaced OpenAL with OpenAL-Soft
- [`1731b30`](https://github.com/roozerxc/atrim/commit/1731b3071cefc3034e3a2fd3c152bd7998d68d3e) Restored unused Penumbra-style head leaning behavior

### v1.4.5-win32-release
- [`19dc606`](https://github.com/roozerxc/atrim/commit/19dc606e47500a52f9ab685abe2531cf2b0d1c41) Added `LUXPROP_OBJECT_BREAKABLE_WORKAROUND` preprocessor definition for breakable objects
- [`0080872`](https://github.com/roozerxc/atrim/commit/00808725cbe4de2cbc021c8656dc0d682c90e9a2) Disabled `UpdateLogActive` by default to fix severe performance degradation on first launch
- [`2d5dd97`](https://github.com/roozerxc/atrim/commit/2d5dd97f921ac12ea7ae74a5a78d71c6b8d5d66a) Fixed Aspect, FOV, Lean, and Roll goals
- [`759d6ad`](https://github.com/roozerxc/atrim/commit/759d6adc4ca1ce59afaab8247e5a99b061103769) Fixed a failure to copy darkness sanity drain variable
- [`b821937`](https://github.com/roozerxc/atrim/commit/b82193765c91a69f031044966257ece1447b2c1d) Fixed a persistent sanity flash when opening Inventory or Journal
- [`5573ad5`](https://github.com/roozerxc/atrim/commit/5573ad53c1843b0a1bf48483217288c57f52d683) Fixed enemy sight range multiplication after the player drew their lantern
- [`64eef0a`](https://github.com/roozerxc/atrim/commit/64eef0a5f5dbb61261f45e4ec8cfad04d4bd9899) Fixed examine area insane messages not appearing
- [`a78779f`](https://github.com/roozerxc/atrim/commit/a78779f31c37468d731ee5cb968871ef45ff9c7f) Fixed fly camera typo when the player's sanity was too low
- [`45b4bcb`](https://github.com/roozerxc/atrim/commit/45b4bcba472eb64aa58ab2256db24bb9dc3778d7) Fixed fullscreen boolean that used to return as an integer value
- [`5d97e08`](https://github.com/roozerxc/atrim/commit/5d97e08ebd80958677958062da31bce6d11192dc) Fixed insanity event boolean that returned as nothing
- [`5146d49`](https://github.com/roozerxc/atrim/commit/5146d4994de95c16c075b213c19ac645528847d4) Fixed mipmap generation to use `GL_SGIS_generate_mipmap` instead of the framebuffer object
- [`3bcb545`](https://github.com/roozerxc/atrim/commit/3bcb5453dee5e4d02a256602515fa36b350cd07a) Fixed non-working player particle system destructor
- [`3c6ec17`](https://github.com/roozerxc/atrim/commit/3c6ec17627b23ec1d594e2ffdee65adfd7824c48) Player can now look straight up and down in a 90 degree angle
- [`4f98490`](https://github.com/roozerxc/atrim/commit/4f98490efb4e29cfb76680b05df0f750fe7d2156) Removed dangerous "suicide" (`H`) key (`eLuxAction_Holster`)
- [`ffad602`](https://github.com/roozerxc/atrim/commit/ffad602b3933baaeb5d7d7a05d860c5da90a4e66) Removed `eLuxAction_Holster` entry from `LuxInputHandler`
- [`9c1bbc9`](https://github.com/roozerxc/atrim/commit/9c1bbc96922c7bfca71db83076f0227b0735cadb) Upgraded OpenGL cube map constants to not use `ARB` extension

### v1.4.4-win32-release
- Account for portable backport effort by creating `settings` and `screenshot` folders in Amnesia game directory
- Added customizable 24-bit bitmap icon which can be set as the icon for the game's title bar
- Fixed buggy screenshots that used an incorrect pixel format (`RGBA`)
- Replaced `GL_TEXTURE_RECTANGLE_NV` with standard `GL_TEXTURE_RECTANGLE`
- Replaced `std::to_wstring` with `LongToWString` in the main menu
- Removed all references to haptics and HaptX code
- Removed SDL2-specific code related to device broadcasting, sleeping and out-of-focus window interactions
