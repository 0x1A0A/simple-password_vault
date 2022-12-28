#include "psm.h"
#include <stdio.h>
#include <string.h>

//char input_state[COUNT][256];

int main(int argc, char **argv) 
{
	psminit();
	psm_cmd();
	psmclear();
	return 0;
}
