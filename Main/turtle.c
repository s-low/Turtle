#include "turtle.h"

// MAIN
int main(int argc, char *argv[])
{
	Program prog;
	Turtle turtle;
	
	Initialise_Program(&prog);
	Initialise_Turtle(&turtle);
	Scan_Program(&prog, argv);
	
	Neill_SDL_Init(&turtle.sw);
	Neill_SDL_SetDrawColour(&turtle.sw, 255, 255, 255);
	
	// Parse and interpret the input
	Prog(&prog, &turtle);
	
	printf("Parsed OK\n");
	SDL_Delay(DELAY);

	// If that worked, try and show a scaled version of the output for fun
	Scaled(&prog, &turtle);

	SDL_Delay(DELAY);
	SDL_Quit();
	return 0;
}

