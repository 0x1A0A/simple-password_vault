#include <termios.h>
#include <unistd.h>

void getpasswd( char *p )
{
	struct termios oldt, newt;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;

	newt.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	scanf(" %[^\n]", p);

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}
