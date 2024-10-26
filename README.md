<p align="left">
  <img src="assets/cherry_icon.png" width="128" height="128">
</p>

## Pac-Man Game (remastered)
This is a remake of the retro-arcade game Pac-Man based on the Simple-Playground (SP) library..\
However, feel free to explore around and play some old classic retro Pac-Man..

## Dependencies
You will need the Simple-Playground library to build this project..\
If you haven't obtained the library yet, please ask the repo-owner (that is me, hidegi),\
to get a copy of the SP-library binaries..

## How to build
To build this project, you will need to install the CMake CLI-tool,\
since this is a CMake-project..\
which you can find under https://cmake.org/..

Once you have CMake installed, use the Command-Prompt or Shell to switch to\
the directory, where this repository is located on your local machine..\
(You will need to use ```cd```)..

On the Pac-Man folder, run following command:\
```$ cmake -Bbuild -G"Unix Makefiles" -D SP_LOCATION=[wherever you have the SP-library located]```

You can of course use any CMake-Generator of choice, as well as have any other name for your build-directory..\
When the project has been built, you should be able to run the game, located under ```PlaygroundPM/[your-build-directory]/src/pacman.exe```
