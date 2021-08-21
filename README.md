# Frozen Electrocuted Combustion

Dll for [Skyrim mod FEC](https://www.nexusmods.com/skyrimspecialedition/mods/3532) that applies visual effects to NPCs and the player, when killed by air, ash, dragons, drain, fire, frost, fear, lightning, poison, soultrap, steam and sun damage status effects.

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

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Building
```
git clone https://github.com/powerof3/FEC
cd PapyrusExtenderSSE
cmake -B build -S .
```
Open build/po3_FEC.sln in Visual Studio to build dll.

## License
[MIT](LICENSE)
