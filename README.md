### Online Battleship Game
Battleship Game!!!!!

This is a console-based implementation of the classic game Battleship, written in C++. The game includes following functions:

Feature!!!:
1. online 1 v 1 competition
2. keyboard controls
3. game save and reload
4. colorful game page

Game Rules:
1.  Each player has a fleet of ships placed on a 10x10 grid.
2.  The ship consists of 
    - one aircraft carrier (4 units), 
    - two battleships (3 units), 
    - three cruisers (2 units), 
    - four submarines (1 unit).
3.  Players take turns guessing the coordinates to target their opponent's ships.
4.  The grid is marked with hits ('X') and misses ('O') to keep track of the shots.
5.  When all cells of a ship are hit, it is considered sunk.
6.  The first player to sink all of their opponent's ships wins the game.

Usage:

- Having a cloud server with a public IP
- Change the DEFUALT_IP in main.cpp with the public IP of your server.
- using `make` to generate two file: `main` and `server`. 
- Place `server` on a cloud server.
- Running command `nohup ./server &` to start the server.
- Running `./main` on your local machine to start the game.


Controls:
1.  Entering the game, content will pop up and the player needs to choose the game mode. Using the arrow keys to navigate the mode, press Enter to select.
2.  If you select Man-Machine mode, skip to Step 3. If the player battle mode is selected, both players need to enter the same room number (eg.1111) to enter the same room to play the battle game.
3.  Players should use the "WASD" or arrow keys on the keyboard to move your ships and press the [ spacebar ] to change ship's orientation.Finally press the Enter key to place these ships(Note: upperleft corner will be fixed during rotation.)
4.  After placing the ship, the game begins and the player uses the "WASD" on the keyboard to select the location of the attack and the "ENTER" key to strike.

