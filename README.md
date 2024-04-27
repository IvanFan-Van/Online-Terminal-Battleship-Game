### Online Battleship Game
Battleship Game!!!!!

This is a console-based implementation of the classic game Battleship, written in C++. The game includes following functions:

1. online 1 v 1 competition
2. keyboard controls
3. game save and reload
4. colorful game page

Game Rules:
1.  Each player has a fleet of ships placed on a 10x10 grid.
2.  The ships include an aircraft carrier (5 cells), battleship (4 cells), cruiser (3 cells), submarine (3 cells), and destroyer (2 cells).
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


