# ArchipelagoHitmanCompanion

This is ArchipelagoHitmanCompanion, a mod for Hitman WOA!

## Description
This mod is intended to be used in tandem with [BenDipp's Archipelago Implementation](https://github.com/BenDipp/Archipelago/blob/hitman_woa/worlds/hitman_woa/docs/setup_en.md) to support the DeathLink feature in Archipelago runs.

## Status
Currently this mod is built of the master branch of the ZHMModSDK and not a formal release. Because of this, there is no release at the moment. It requires building both the ZHMModSDK and this repository off of that. Once a compatible release of the ZHMModSDK is published you can expect a release of this mod soon thereafter. If you would like to assist in development/testing of this mod and don't mind building it from source please see the instructions below.  

## Installation Instructions

1. Clone the source of [ZHMModSDK](https://github.com/OrfeasZ/ZHMModSDK), build and install it. See [here](https://github.com/OrfeasZ/ZHMModSDK/wiki/Building-&-debugging-the-SDK) (Latest confirmed working commit: 3d80fdd80b63298564f212107590f10698af18e0 )
2. Clone this repository and configure it's ZHMMODSDK_DIR setting to direct to your local ZHMSource. See [here](https://github.com/OrfeasZ/ZHMModSDK/wiki/Developing-a-mod-with-a-local-version-of-the-SDK)
3. Build and install. See [here](https://github.com/OrfeasZ/ZHMModSDK/wiki/Setting-up-Visual-Studio-for-development).
4. Once Hitman:WOA is booted press the '~' key and enable `ArchipelagoHitmanCompanion` from the menu at the top of the screen (you may need to restart your game afterwards).
5. If the mod loaded correctly, there should be a new menu at the top of the screen titled ArchipelagoHitmanCompanion. Click that. (I recommend doing this once in game and not within the main menu.)
6. In the window that appears enter your connection settings and press connect. (Optionally click the checkbox to enable the LogWindow to see Archipelago Server messages within the Hitman:WOA game.)
7. Enjoy sending and receiving deathlink to your friends!

(I recommend enabling the NoPause Mod aswell if you intend to be alt tabbing between windows as there is an issue with connection to the Archipelago Server disconnecting if the game is paused.)
<!--
1. Download the latest [ZHMModSDK](https://github.com/OrfeasZ/ZHMModSDK) and install it.
3. Download the latest version of `ArchipelagoHitmanCompanion` and copy it to the ZHMModSDK `mods` folder (e.g. `C:\Games\HITMAN 3\Retail\mods`).
4. Run the game and once in the main menu, press the `~` key (`^` on QWERTZ layouts) and enable `ArchipelagoHitmanCompanion` from the menu at the top of the screen (you may need to restart your game afterwards).
5. Enjoy!
-->
## Building

### 1. Clone this repository locally with all submodules.

You can either use `git clone --recurse-submodules` or run `git submodule update --init --recursive` after cloning.

### 2. Install Visual Studio (any edition).

Make sure you install the C++ and game development workloads.

### 3. Open the project in your IDE of choice.

See instructions for [Visual Studio](https://github.com/OrfeasZ/ZHMModSDK/wiki/Setting-up-Visual-Studio-for-development) or [CLion](https://github.com/OrfeasZ/ZHMModSDK/wiki/Setting-up-CLion-for-development).
