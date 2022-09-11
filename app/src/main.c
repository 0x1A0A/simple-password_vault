#include "psm.h"
#include <stdio.h>
#include <string.h>
#include "passwdPromp.h"

char cmd[512];
char input_state[COUNT][256];

void promp(const char *str)
{
	printf("%s", str);
}

int main(int argc, char **argv) 
{
	int run = 1;
	enum INPUT state = -1; 

	psminit();

	while (run) {
		switch (state) {
			case PASSWD_ACC:
				promp("Enter your password _>> ");
				getpasswd(input_state[PASSWD_ACC]);				
				puts("");

				puts(input_state[PASSWD_ACC]);
				state = -1;
				break;
			default: 
				promp("_>> ");
				scanf(" %[^\n]", cmd );
				
				if ( !strcmp(cmd,"exit") || !strcmp(cmd,"quit")) { run=0; break; }

				state = psm_cmd();
				break;
		}

	}
	psmclear();
	return 0;
}
