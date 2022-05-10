# Frozen Electrocuted Combustion

Dll for [Skyrim mod FEC](https://www.nexusmods.com/skyrimspecialedition/mods/3532) that applies visual effects to NPCs and the player, when killed by air, ash, dragons, drain, fire, frost, fear, lightning, poison, soultrap, steam and sun damage status effects.

[VR Version](https://www.nexusmods.com/skyrimspecialedition/mods/59118)
## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2019](https://visualstudio.microsoft.com/)
	* Desktop development with C++
* [CommonLibSSE](https://github.com/powerof3/CommonLibSSE/tree/dev)
	* You need to build from the powerof3/dev branch
	* Add this as as an environment variable `CommonLibSSEPath`
* [PapyrusExtender](https://github.com/powerof3/PapyrusExtenderSSE)
	* Runtime requirement
	* Also available [prebuilt](https://www.nexusmods.com/skyrimspecialedition/mods/22854)
	* VR version available [prebuilt](https://www.nexusmods.com/skyrimspecialedition/mods/58296)
* [CommonLibVR](https://github.com/alandtse/CommonLibVR/tree/vr)
	* Add this as as an environment variable `CommonLibVRPath`

## User Requirements
* [Address Library for SKSE](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
	* Needed for SSE
* [VR Address Library for SKSEVR](https://www.nexusmods.com/skyrimspecialedition/mods/58101)
	* Needed for VR

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Building
```
git clone https://github.com/powerof3/FEC
cd FEC
```
### SSE
```
cmake --preset vs2022-windows-vcpkg # for vs2019 use vs2019-windows-vcpkg
cmake --build build --config Release
```
### AE
```
cmake --preset vs2022-windows-vcpkg-ae # for vs2019 use vs2019-windows-vcpkg-ae
cmake --build buildae --config Release
```
### VR
```
cmake --preset vs2022-windows-vcpkg-vr # for vs2019 use vs2019-windows-vcpkg-vr
cmake --build buildvr --config Release
```

## License
[MIT](LICENSE)
