# Wiard Game Engine

This project was a learning exercise to build a game engine from 'scratch'. Scratch in this case being SDL2, C++, CMake, and openGL. It might become a game about a wizard some day, but for now, it's just a learning exercise in game engine internals.

This project includes a hand rolled entity component system, a rendering system, and a rudimentary physics system.

The prefix WG_ is short for Wizard Game. All dependencies are included directly in the `vendor` folder.

### Initial Setup

#### Windows
1. `irm get.scoop.sh | iex`
2. `scoop install cmake mingw ninja`

*Note: use the mingw package, not the gcc package. The gcc package is 2 years out of date, and that version of gcc doesn't support c++20 features*
*Note: scoop will add mingw exes to your path, but it seems to trickle out slowly. Possibly just reboot after installing deps*

### Building
1. `./build.ps1`

### Running
1. `./cmake-build-debug/WizardGame.exe`


### Import Rules (to avoid circular dependencies in template methods, and just generally keep me sane):
1. File setup:
    src/private/<FileName>.cpp // <- concrete function definitions
    src/public/<FileName>.h // <- class definition
    src/public/<FileName>.impl.h // <- template method definitions
2. Include <FileName>.impl.h at the bottom of <FileName>.h
3. Use `#pragma once` in both .h files
4. When importing references, import the minimal thing possible, so:
   1. Try not importing it at all and compile
   2. If that fails, try "class <ClassName>;" (eg, forward declare it). This will only work if you don't inherit from the class and don't use any of its methods. Expect to forward declare classes in headers
                                              unless you inherit from them. In .impl.h and .cpp, do the full import, if needed
   3. Finally, do a full import of `#include <ClassName>.h`. This is a bit of a last resort, as it can introduce cyclic dependencies, but is obviously required to actually touch the dependent type (eg - in the .impl.h/.cpp, or for inheritance in the .h file)
   4. Full imports (`#include <ClassName>.h`) are also required for class templates, so be careful of circular dependencies in templated classes. Not sure how to fully get around this one yet



### Plausible IDEs
* VS Code
* Visual Studio
* CLion