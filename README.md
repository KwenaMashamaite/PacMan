# Pac-Man

[Pac-Man](https://en.wikipedia.org/wiki/Pac-Man) is a classic grid-based arcade game
that was created in 1980 by [Namco](https://en.wikipedia.org/wiki/Namco).

## Overview

This project is a Pac-Man clone using my 2D game engine, [Infinite Motion Engine](https://github.com/KwenaMashamaite/IME). 
The project reuses code from my first game, [Super Pac-Man clone](https://github.com/KwenaMashamaite/SuperPacMan). 
The only major difference between the two is the gameplay maze and the actual gameplay.

## How to play

1. [Download](https://github.com/KwenaMashamaite/PacMan/releases/tag/v0.2.0)
   the latest game version
2. Extract the zip archive
3. Run _PacMan.exe_
4. Enjoy :video_game:

## Build

###### *Read this section if you want to build the game yourself, otherwise you may skip it.*

This project is a CMake project, therefore you must download and install [CMake](https://cmake.org/). 
You also need to download [Infinite Motion Engine v2.1.0](https://github.com/KwenaMashamaite/IME/releases/tag/v2.1.0). 
Then:

1. Clone the repository
```git
$ git clone https://github.com/KwenaMashamaite/PacMan
```   
2. Create a folder in the root of the directory created in step 1 and name it `extlibs`
3. Extract **IME** to the `extlibs` folder created in step 2. It should look like:

![](docs/screenshots/extlibs_directory_structure.png)
   
At this point your ready to build the project. The easiest way to do this is to
use an IDE that supports building with *CMake*. I recommend using [CLion](https://www.jetbrains.com/clion/) (Paid - Free 30 day trial), 
which uses *CMake* as the default build system for C/C++, and it comes bundled with 
*CMake*, so no need to install it yourself. Another personal recommendation is [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) (Free)

To build the game using the *CLI*, run the following commands from the project root 
directory:

```shell
mkdir build
cd build
cmake ..
cmake --build .
```

After the build process completes, you will find the game executable in `PacMan/bin`
folder

## Platform

This game only runs on the Windows platform. It was tested on Windows 10.
