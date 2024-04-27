#include <termios.h>
#include <unistd.h>
#include "battleship/keyboard.h"
using namespace std;
char keyboard(){
	char key;
	struct termios new_settings;
	struct termios stored_settings;
	tcgetattr(0, &stored_settings);
	new_settings = stored_settings;
	new_settings.c_lflag &= ~(ICANON | ECHO); //disable line buffering and echoing of characters
	new_settings.c_cc[VTIME] = 0;
	new_settings.c_cc[VMIN] = 0;
	tcsetattr(0, TCSANOW, &new_settings);

	bool exit = false;
	while (!exit){ //prompt for input unless 
		if (read(0, &key, 1) > 0) {
			switch (key){
				case ' '://Press the spacebar, and the orientation of the ship will be changed
					key = 'C';
					exit = true;
					break;
				case '\n'://Press the enter key, and the function will return character 'Y'
					key = 'Y';
					exit = true;
					break;
				case 'w': //moving upwards, denoted by 'w'
				case 'W':
					key = 'w';
					exit = true;
					break;
				case 'A'://moving to the left, denoted by 'a'
				case 'a':
					key = 'a';
					exit = true;
					break;
				case 's'://moving downwards, denoted by 's'
				case 'S':
					key = 's';
					exit = true;
					break;
				case 'd'://moving to the right, denoted by 'd'
				case 'D':
					key = 'd';
					exit = true;
					break;
				case 'q'://moving to the right, denoted by 'd'
				case 'Q':
					key = 'N';
					exit = true;
					break;
				case '\033':  // process the arrow keys
                    if (read(STDIN_FILENO, &key, 1) > 0 && key == '[') {
                        if (read(STDIN_FILENO, &key, 1) > 0) {
                            switch (key) {
                                case 'A':
                                    key = 'w';
									exit = true;
                                    break;
                                case 'B':
                                    key = 's';
                                    exit = true;
                                    break;
                                case 'C':
                                    key = 'd';
                                    exit = true;
                                    break;
                                case 'D':
                                    key = 'a';
                                    exit = true;
                                    break;
                            }
                        }
                    }
					else{	//ESC key pressed
						key = 'N';
						exit = true;
						break;
					}
                    break;
			}
		}
		
	}
	tcsetattr(0,TCSANOW, &stored_settings);//recover the original setting for input and display mode
	return key;//return the character key, which can only be among {Y,N,w,a,s,d}
}
