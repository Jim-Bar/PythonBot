# What is PythonBot ?

PythonBot is a game where you can write an AI (in Python) which will fight in an arena for its survival.

Each bot in the arena has its own AI, written by a player. The bot can move around, scan its environment, and shoot bullets at others. The game ends when there is only one survivor (or when the game master decide to stop it, as it can be very long), but you can also attribute victory on other critters (like the number of kills among the survivors).

You do not need to have a deep knowledge of Python to play; even without any basis, you could easily write your bot in a short amount of time (provided you already have some understanding with programming languages).

# Installation

## Requirements

* The game engine uses SFML (http://www.sfml-dev.org/index.php) so you will need it.
* The bots are written in Python, and consequently need Python (v2.7).
* Linux, Windows or Mac

## Linux

1. Install SFML (for Ubuntu: `sudo apt-get install libsfml-dev`)
2. Install Python (already installed in Ubuntu)
3. Download the game (clone the repository or download the zip archive)
4. Compile it: go into the game directory and type `make`

## Windows

I installed the game on Windows only once (a long time ago) and so do not remember well the steps in detail. It should be straighforward, and I give the main steps here:

1. Install SFML (See http://www.sfml-dev.org/tutorials/2.3/index.php)
2. Install Python (Choose "Windows x86-64 MSI installer" in https://www.python.org/downloads/release/python-2710/)
3. Download the game (clone the repository or download the zip archive)
4. Compile it using your favorite IDE (make sure to link against `sfml-system`, `sfml-window`, `sfml-graphics`, and `sfml-network`)
5. Move the executable in the root directory (the one where `pythonbot.py` is), and rename it to `pythonbot_core`

## Mac

I never tried on Mac, but I assume it works the same way.

1. Install SFML (See http://www.sfml-dev.org/tutorials/2.3/start-osx-fr.php)
2. Install Python (Choose "Mac OS X 64-bit/32-bit installer" in https://www.python.org/downloads/release/python-2710/)
3. Download the game (clone the repository or download the zip archive)
4. Compile it using Xcode (make sure to link against `sfml-system`, `sfml-window`, `sfml-graphics`, and `sfml-network`), or do it in the Unix way
5. Move the executable in the root directory (the one where `pythonbot.py` is), and rename it to `pythonbot_core`

# Test it

## Start the program

Once the game is compiled, just run the script `pythonbot.py` in a terminal: `python pythonbot.py`. Pause/unpause the game by pressing the space bar. Admire how the bots seek and destroy each others (actually the default bots are not that smart...).

## Use different bots

By default, 5 bots are participating into the fight, and you will see that they are not very intelligent. These bots are located in the subdirectory `Bots/`. You can find other examples in the `BotLibrary/` folder. To use a bot, just copy it to the `Bots/` folder.

# Write your own bot !

Creating your bot is very simple. What you need is to create a python file named after the name of your bot (for example `myBot.py`).
Inside this file, create a function `main` that takes one argument `bot`:
```
def main(bot):
```
The object `bot` gives you access to five functions:
- `skip()`: do nothing
- `move()`: advance your bot by one unit of distance
- `rotate(angle)`: rotate your bot by `angle` degrees (not radians)
- `fire()`: fire, but only if you have some bullets left.
- `scan(distance, radius, scanCircles, scanBots, scanBullets)`: Err, this one is harder. Basically scan the map in a circle to see if there are enemy bots, bullets, or just walls.

If for instance I want to move forward, I write: `bot.move()`

Description of the `scan()` function:

1. `distance` is the relative distance from your bot to the center of the circle you're scanning.
2. `radius` is the radius of this very circle.
3. `scanCircles` is a boolean that tells if walls (and wall disks) should be scanned.
4. `scanBots` is the same but for the bots. Note that dead bots nor your bot are scanned.
5. `scanBullets` is also the same, but for enemys' bullets.

This function returns a triplet which contains respectively:

1. The number of walls (if there is at least one wall, you probably want to rotate)
2. The number of enemy bots
3. The number of enemy bullets

Finally, your bot has access to its life remaining, its number of bullets available, and its number of kill (see the bot "dumb" for an example). These are not functions, just attributes, so you can only read it, like this:
```
bot.health
bot.bullets
bot.kills
```
And that's it ! Just build your AI from these functions, and don't forget that you can look at the bots provided in `BotLibrary` to find inspiration ;)



Here is the most basic bot you can build (does nothing):
```
def main(bot):
  while True:
    bot.skip()
```

# Improve PythonBot !

Modifications to PythonBot are encouraged ! Feel free to fork, clone, download... and change it in the way you like :)

The smartest brains out there will notice that other languages could easily replace Python (overlay of the Python functions, or rewritting of the Python module in another language, thanks to TCP).

I used a GPL license, so PythonBot will stay open forever. You can still do whatever you want with it, as long as you provide the sources when releasing your version. More information on the GPL here: http://www.gnu.org/licenses/

# Notes

## Status of the development

I am no longer releasing updates as I consider that the game reached a really good shape (compared to what I had in mind at the beginning). I would still love to receive feedback, so feel free to contact me if you liked the game. I am open to suggestions, but I will implement them only if I really (**really**) like your idea −so you can try−. However let me now if you encounter bugs, I will fix these !

## License

*PythonBot - A game by a developer for developers.*
*Copyright (C) 2015 Jean-Marie BARAN*

*This program is free software: you can redistribute it and/or modify*
*it under the terms of the GNU General Public License as published by*
*the Free Software Foundation, either version 3 of the License, or*
*(at your option) any later version.*

*This program is distributed in the hope that it will be useful,*
*but WITHOUT ANY WARRANTY; without even the implied warranty of*
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the*
*GNU General Public License for more details.*

*You should have received a copy of the GNU General Public License*
*along with this program. If not, see <http://www.gnu.org/licenses/>.*

*Refer to 'LICENSE.txt' for the full notice.*

The license apply for all source files (both C++ and Python), `README.md` and `LICENSE.txt`. The bots provided in `Bots/` and `BotLibrary/` are not part of the license. The Makefile, and Git-related files (`.gitignore`) are not part of the license.
This is a wonderfull game and you should try it
