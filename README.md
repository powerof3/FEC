# Frozen Electrocuted Combustion

Dll for [Skyrim mod FEC](https://www.nexusmods.com/skyrimspecialedition/mods/3532) that applies visual effects to NPCs and the player, when killed by air, ash, dragons, drain, fire, frost, fear, lightning, poison, soultrap, steam and sun damage status effects.

[VR Version](https://www.nexusmods.com/skyrimspecialedition/mods/59118)

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/) or newer
	* Desktop development with C++

## User Requirements
* [Address Library for SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
	* Needed for SSE/AE
* [VR Address Library for SKSEVR](https://www.nexusmods.com/skyrimspecialedition/mods/58101)
	* Needed for VR
* [PapyrusExtender](https://www.nexusmods.com/skyrimspecialedition/mods/22854)
	* VR version available [here](https://www.nexusmods.com/skyrimspecialedition/mods/58296)

## Building
```
git clone https://github.com/powerof3/FEC
cd FEC
git submodule update --init --recursive
```

### SSE (1.5.97)
```
cmake --preset vs2022-se
cmake --build --preset vs2022-se
```
### AE (1.6.1170+)
```
cmake --preset vs2022-ae
cmake --build --preset vs2022-ae
```
### VR
```
cmake --preset vs2022-vr
cmake --build --preset vs2022-vr
```

Replace `vs2022` with `vs2026` to build with Visual Studio 2026.

## License
[GPL-3.0](LICENSE)
