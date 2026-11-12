# atrim: Client-Specific Patches

These are client-specific patches for atrim, intended to be put onto an existing *Amnesia: The Dark Descent* installation.

### Contents

The contents of this folder should be merged within the root of the *Amnesia: The Dark Descent* installation folder. If you have the DVD/retail version of the game, the client-specific patches should be installed at `redist` where the existing game files reside.

- `/config`
    - `config/patch_<language>.lang`

        Localized text language files for the client. Currently, **English (enUS)**, **Finnish (fiFI)**, and **German (deDE)** are supported. If you are playing the game with an unsupported language, the client will fall back these missing text entries to regular **English (enUS)**.

    - `config/pre_menu.cfg`

        This file removes the *norden (Nordic Game Program)* boot screen when the game starts up. The **"atrim is a FREE and open-source software client"** warning will always appear first before all pre-menu entries since it is hard-coded in the client executable and cannot be removed.

- `/core`
    - `core/models`

        The core mesh files required in order for the engine to function. If these are missing, the game will fail to load.

    - `core/shaders`

        The GLSL 1.20 shaders which are also required for the engine's deferred graphics renderer to function properly. If these are missing, the rendered visuals in the game can either look wrong or the engine's graphics module will crash.

        This includes `_test_deferred_light_vtx.glsl`, which is used by the start-up self-test that checks whether the full deferred light shader (`deferred_light_frag.glsl`) renders correctly on your graphics card. If it does not (e.g. on ATi Radeon 9500-X850 class cards), the client switches to `deferred_light_frag_lite.glsl` on its own, so there is no need to rename shader files anymore. The result is written to `hpl.log`. To skip the test, set `LightShaderMode` in the `Graphics` section of `main_settings.cfg` to `1` (always use full shader) or `2` (always use lite shader).

        The shaders are written to work with both GLSL 1.20 (OpenGL 2.1) and GLSL 1.10 (OpenGL 2.0). On drivers that only support GLSL 1.10, the client compiles them as `#version 110` and uses the 1.10 code paths, so older drivers (such as ATi Catalyst versions before 7.12) work without updating. When editing shaders, keep them GLSL 1.10 compatible: write float literals as `1.0` instead of `1` (no implicit int-to-float conversion), don't use `f` suffixes, and put 1.20-only features (`invariant`, array constructors) behind `@ifdef GLSL_110` / `@ifdef FeatureNotSupported_ConstArray`. To test the 1.10 path on a newer card, set `ForceGLSL110` in the `Graphics` section of `main_settings.cfg` to `true`.

    - `core/textures`

        The core textures for the game's graphics renderer. This includes the Z dissolve texture (`core_dissolve.tga`) and light falloff map (`core_falloff_linear.png`).

- `/fonts`

    Contains the fonts that were introduced as a visual improvement, part of the atrim client.

    The default fonts (including the **HINT** font) use the *Bitstream Vera* typeface.

    The improved journal text font uses the non-outlined *Book Antiqua* typeface.

- `/graphics`

    Contains the new Frictional Games logo (`pre_menu/startup_fg_logo.jpg`, extracted from *Amnesia: Collection*) and the aspect ratio-corrected version of the menu loading screen image (`general/menu_loading_screen.jpg`).

- `/models/hand_objects/lantern`

    Contains the *Amnesia: Collection* lantern model texture which was broken as a result of the porting progress, but added back as a visual flair for version 1.4.6.

- `/shaders`

    Like `core/shaders`, this contains the additional shaders for the game, such as enemy outline glow, screen gamma, and the insanity screen distortion effect. If these are missing, the rendered visuals in the game can either look wrong or the engine's graphics module will crash.

- `/sounds`

    Restores an unused Servant Brute enabled sound and mutes the deafening ear-ringing noise (`insanity_ear_ring.ogg`).

- `/static_objects/technical`

    Fixes the small plane textures as 128x128 images to avoid severe pixel-squashing errors from using the Ultra Low setting.
