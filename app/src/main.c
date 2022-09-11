#include "psm.h"
#include <stdio.h>
#include <ncurses.h>
#include <string.h>

char cmd[512];
char input_state[COUNT][256];

void promp(const char *str)
{
	clear();
	mvprintw( 0,0, str);
}

int main(int argc, char **argv) 
{
	int run = 1;
	enum INPUT state = -1; 

	psminit();
	initscr();

	cbreak();

	while (run) {
		switch (state) {
			case PASSWD_ACC: noecho();
				promp("Enter your password _>> ");
				refresh();
				getstr(input_state[PASSWD_ACC]);				
				mvprintw(3,0, input_state[PASSWD_ACC]);
				echo(); 
				state = -1;
				getch();
				break;
			default: 
				promp("_>> ");
				getstr( cmd );
				
				if ( !strcmp(cmd,"exit") || !strcmp(cmd,"quit")) { run=0; break; }

				state = psm_cmd();
				break;
		}

	}

	endwin();

	psmclear();
	return 0;
}
