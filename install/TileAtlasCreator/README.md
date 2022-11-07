# TileAtlasCreator

A tool to create the Tile Atlas Images required by MDCII.

**The graphics created with this tool may not be distributed under any circumstances.**

## Requirements

* The original game is required, either as "New Islands, New Adventures" (NINA) or the History Edition.
* Download and install a Java IDE. My recommended IDE for Java is the IntelliJ IDEA Community Edition.

## Platforms

* Win
* Linux

## Build instructions

1. After the project - the folder TileAtlasCreator - has been opened with the IDE, the settings in the `creator.properties` must be adjusted. 
This file is located in `TileAtlasCreator/src/main/resources`. In any case, the `ROOT_PATH` must be the location of the original game.
The other settings can remain as they are.
2. Start the project. That's it. The `main` method is in `TileAtlasCreator/src/main/java/mdcii/Main.java`.
3. After this magic we have a new directory called `out` in `TileAtlasCreator/`.
In the `out` directory there is another directory called `atlas`. It contains the created images.
4. The newly created folder `atlas` must now be copied into the MDCII C++ project to `MDCII/resources`.

# License

TileAtlasCreator is licensed under the GPL-2.0 License, see [LICENSE](https://github.com/stwe/MDCII/blob/main/LICENSE) for more information.
