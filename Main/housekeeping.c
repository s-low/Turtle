#include "turtle.h"

/* Other functions used for setup and graphics */

/* Initialise user accessible vars and the token list */
void Initialise_Program(Program *p){
	
	p->counter = 0; // current word is zeroth

	int i;

	/* Initially all vars are NOT NUMBERS - makes checking for if they have
	been set very simple */

	for (i = 0; i < NUM_VARS; ++i)
	{
		p->var[i]  = NAN; // GCC MACRO FOR NaN
	}

	/* According to IEEE an equality comparison between a value and itself
	will evaluate to 0 only if they are NaN.
	NaN is better than initialising to zero, as zero is technically allowed */

	for (int i = 0; i < NUM_VARS; ++i)
	{
		if ( p->var[i] == p->var[i] ) 
		{
			ERROR("Vars not initialised to NAN")
		}
	}

	// Initially all tokens are terminators
	for (i = 0; i < MAXNUMTOKENS; ++i) 
	{
		p->wds[i][0] = '\0'; // fill with terminators
	}
}

/* Initialise the Turtle */
void Initialise_Turtle(Turtle *t){

	t->bearing = 0; 		// Upwards 
	t->distance = 0;
	
	t->x = CENTRE_X;		// Centre the turtle
	t->y = CENTRE_Y;
	
	t->min_x = CENTRE_X;	// Set the initial min/max values 
	t->min_y = CENTRE_Y;
	t->max_x = CENTRE_X;
	t->max_y = CENTRE_Y;
	
	t->scaled = OFF; 			// Default to no graphical scaling

}

/* Scan the program in token by token from the file */
void Scan_Program(Program *p, char *argv[]){
	
	int i = 0, scanned = 1;
	FILE *progfile;

	// Check program was opened
	
	if( !( progfile = fopen(argv[1],"r") ) ){

		ERROR("Couldn't open file, make sure you supply a valid filename")
	}

	// Read the file contents into prog structure
	while( scanned == 1){
		
		ScanCheck(scanned, 1);
		scanned = fscanf(progfile, "%s", p->wds[i]);
		i++;
	}

	fclose(progfile);
}

/* Simple int comparison for scanf return value */
void ScanCheck(int a, int b){

	if (a != b)
	{
		ERROR("Scanning problem")
	}
	else
	{	
		return;
	}
}


/* Simple rehash of strcmp to simplify conditionals */
int StringMatch(char string1[], char string2[]){
	
	if (strcmp(string1, string2) == 0) // This is boring to type all the time
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/* INTERPRET: Simplified SDL Draw function for FD instruction */
void DrawLine(Turtle *t){

	// To draw a line, we need to know the coordinates at either end
	int x1, x2, y1, y2;
	double dx, dy;

	x1 = t->x;
	y1 = t->y;

	// Trig. Bearing defined as clockwise argument with the Y-axis:
	dx = (double) t->distance * sin(t->bearing);
	dy = (double) t->distance * cos(t->bearing);
	// There is an amount of creep in the graphical output possibly from here
	// But the above calc is double = double * sin(double) as it should be?

	// If scaling, scale:
	if (t->scaled == ON) 
	{
		dx = dx * t->scale;
		dy = dy * t->scale;	
	}

	// Rounding down all the time causes gradual shift. Round properly for less.
	// Creep may come from here - but is unavoidable - pixels are discrete!
	dx = Rint(dx);
	dy = Rint(dy);

	// dy is negated in graphical coords - note that we're back to ints here
	x2 = x1 + dx;
	y2 = y1 - dy;

	// Point #2 is now the turtle location:
	t->x = x2;
	t->y = y2;

	// Draw the line to screen.
	SDL_RenderDrawLine(t->sw.renderer, x1, y1, x2, y2);
	SDL_RenderPresent(t->sw.renderer);
	SDL_UpdateWindowSurface(t->sw.win);	
	
	Neill_SDL_Events(&t->sw); 	// Check for exit
	if (t->sw.finished == 1) {
		SDL_Quit();
	}
	
	if (t->scaled == OFF) { 		// On the first run 
		Track_Bounds(t);  		// Keep an eye on the size of graphics
	}
}

/* Hide the boring bits. Checks if x and y are further from centre than ever */
void Track_Bounds(Turtle *t){

	if (t->x > t->max_x) // New max_x ?
	{
		t->max_x = t->x;
	}

	if (t->y > t->max_y) // New max_y ?
	{
		t->max_y = t->y;
	}

	if (t->x < t->min_x) // New min_x ?
	{
		t->min_x = t->x;
	}

	if (t->y < t->min_y) // New min_y ?
	{
		t->min_y = t->y;
	}
}


// Run of the interpreter/parser but scaling the graphics to fit window
void Scaled(Program *p, Turtle *t){

	printf("DISPLAYING SCALED OUTPUT\n");

    Neill_SDL_SetDrawColour(&t->sw, 0, 0, 0); // Colour screen black.
    SDL_RenderClear(t->sw.renderer);
    SDL_RenderPresent(t->sw.renderer);
    Neill_SDL_SetDrawColour(&t->sw, 255, 255, 255); // Move back to white.

    // Reset values that have changed during the first run:
	p->counter = 0; 	
	t->bearing = 0;
	t->x = CENTRE_X;
	t->y = CENTRE_Y;

	// Scale Factors for the +ve/-ve x/y axes
	float sf1, sf2, sf3, sf4; 

	sf1 = Scale_Calc(MAX_BOUND, t->max_y, CENTRE_Y);
	sf2 = Scale_Calc(MAX_BOUND, t->max_x, CENTRE_X);
	sf3 = Scale_Calc(MIN_BOUND, t->min_y, CENTRE_Y);
	sf4 = Scale_Calc(MIN_BOUND, t->min_x, CENTRE_X);

	// Use the smallest of the four scale values to fit graphics
	t->scale = Minimum(sf1, sf2, sf3, sf4);
	if (t->scale == VERY_LARGE)
	{
		ERROR("Nothing was drawn - cannot scale to fit the window")
	}

	printf("Scaled to fit by factor: %.3f\n", t->scale);

	t->scaled = 1; 
	Prog(p, t);
}


/* Calculate the scale factor */
float Scale_Calc(int max_min, int bound, int centre){

	if (bound == centre)
	{
		return VERY_LARGE;
	}

	if (max_min == MAX_BOUND)
	{
		return (float) centre / (bound - centre);
	}

	else 
	{
		return (float) centre / (centre - bound);
	}
}


/* Return the smallest of four floats */
float Minimum(float a, float b, float c, float d){

	float min = a;
	
	if (b < min)
	{
		min = b;
	}
	if (c < min)
	{
		min = c;
	}
	if (d < min)
	{
		min = d;
	}

	return min;
}

/* Round float to NEAREST int */
int Rint(float a){

	if (a >= 0) // Positive
	{
		a = (a + 0.5); // exactly 0.5: (x + 0.5)/2 then round to floor 
	}
	else // Negative
	{
		a = (a - 0.5); // exactly 0.5: (x - 0.5)/2 then round to floor
	}

	return a;

}

