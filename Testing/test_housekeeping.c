#include "test_turtle.h"

/* Other functions used for setup and graphics */

/* Initialise user accessible vars and the token list */
void Initialise_Program(Testing *test, Program *p){
	
	p->counter = 0; // current word is zeroth

	int i = 0;

	/* Initially all vars are NOT NUMBERS - makes checking for if they have
	been set very simple */

	for (i = 0; i < NUM_VARS; ++i)
	{
		p->var[i]  = NAN; // GCC MACRO FOR NaN
		wb_assert(!(p->var[i] == p->var[i]), "Var not initialised to NaN")
	}

	/* According to IEEE an equality comparison between a value and itself
	will evaluate to 0 only if they are NaN.
	NaN is better than initialising to zero, as zero is technically allowed */

	for (int i = 0; i < NUM_VARS; ++i)
	{
		if ( p->var[i] == p->var[i] ) 
		{	
			p->test_flag = 1;
			return;
			//ERROR("Vars not initialised to NAN")
		}

		wb_assert(!(p->var[i] == p->var[i]),"Var not initialised to NaN")
	}

	for (i = 0; i < MAXNUMTOKENS; ++i) 
	{
		p->wds[i][0] = '\0'; // fill with terminators
		wb_assert(p->wds[i][0] == '\0', "Token not initialised to terminator")
	}
}

/* Initialise the Turtle */
void Initialise_Turtle(Testing *test, Turtle *t){

	t->bearing = 0; 		// Upwards 
	t->distance = 0;
	
	t->x = CENTRE_X;		// Centre the turtle
	t->y = CENTRE_Y;

	t->min_x = CENTRE_X;	// Set the initial min/max values 
	t->min_y = CENTRE_Y;
	t->max_x = CENTRE_X;
	t->max_y = CENTRE_Y;
	
	t->scaled = 0; 			// Default to no graphical scaling
	wb_assert(t->scaled == 0, "Scale mode not set off")
}

/* Scan the program in token by token from the file */
void Scan_Program(Testing *test, Program *p, char *argv[]){
	int i = 0, scanned = 1;
	FILE *progfile;
	
	// Check program was opened and catch if so
	if( !(progfile = fopen(argv[1],"r")) ){
		p->test_flag = 1;
		return;
		//ERROR("Couldn't open file")
	}

	wb_assert(progfile != NULL, "File not correctly opened")

	// Read the file contents into prog structure
	while(scanned == 1){
		
		ScanCheck(scanned, 1);
		wb_assert(scanned == 1, "fscanf error")
		wb_assert(i<MAXNUMTOKENS, "Index larger than permitted")
		
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
int StringMatch(Testing *test, char string1[], char string2[]){
	
	if ( strcmp(string1, string2) == 0)
	{
		wb_assert( strcmp(string1, string2) == 0, "Strcmp misuse")
		return 1;
	}
	else
	{	
		wb_assert( strcmp(string1, string2) != 0, "Strcmp misuse")
		return 0;
	}
}


/* INTERPRET: Simplified SDL Draw function for FD instruction */
void DrawLine(Testing *test, Turtle *t){

	// To draw a line, we need to know the coordinates at either end
	int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	float dx = 0, dy = 0;

	x1 = t->x;
	y1 = t->y;

	// Trig. Bearing defined as clockwise argument with the Y-axis:
	dx = (float) t->distance * sin(t->bearing);
	dy = (float) t->distance * cos(t->bearing);

	// If scaling, scale:
	if (t->scaled == 1) 
	{	
		wb_assert(t->scaled == 1, "Scale mode misinterpreted")
		dx = dx * t->scale;
		dy = dy * t->scale;	
	}

	// dy is negated in graphical coords. Must cast to int to get nearest pixel.
	x2 = x1 + (int) dx;
	y2 = y1 - (int) dy;

	// Point #2 is now the turtle location:
	int xstore = t->x;
	int ystore = t->y;

	t->x = x2;
	t->y = y2;

	wb_assert(t->x == xstore + dx, "New x value mis-calculated. Likely type conversion error.")
	wb_assert(t->y == ystore - dy, "New y value mis-calculated. Likely type conversion error.")

	// Draw the line to screen.
	
	/* DISABLED IN TEST
	SDL_RenderDrawLine(t->sw.renderer, x1, y1, x2, y2);
	SDL_RenderPresent(t->sw.renderer);
	SDL_UpdateWindowSurface(t->sw.win);	
	
	Neill_SDL_Events(&t->sw); 	// Check for exit
	if (t->sw.finished == 1) {
		SDL_Quit();
	}
	*/
	
	if (t->scaled == 0) { 		// On the first run 
		wb_assert(t->scaled == 0, "If statement strangeness in DrawLine")
		Track_Bounds(test, t);  		// Keep an eye on the size of graphics
	}
}

/* Hide the boring bits. Checks if x and y are further from centre than ever */
void Track_Bounds(Testing *test, Turtle *t){

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

	wb_assert(t->min_y <= t->y, "New minimum y value missed in Track_Bounds")
	wb_assert(t->min_x <= t->x, "New minimum x value missed in Track_Bounds")
	wb_assert(t->max_y >= t->y, "New maximum y value missed in Track_Bounds")
	wb_assert(t->max_x >= t->x, "New maximum x value missed in Track_Bounds")

}


// Run of the interpreter/parser but scaling the graphics to fit window
void Scaled(Testing *test, Program *p, Turtle *t){
	// DISABLED IN TEST
	//printf("DISPLAYING SCALED OUTPUT\n");

/*    Neill_SDL_SetDrawColour(&t->sw, 0, 0, 0); // Colour screen black.
    SDL_RenderClear(t->sw.renderer);
    SDL_RenderPresent(t->sw.renderer);
    Neill_SDL_SetDrawColour(&t->sw, 255, 255, 255); // Move back to white.
*/

    // Reset values that have changed during the first run:
	p->counter = 0; 	
	t->bearing = 0;
	t->x = CENTRE_X;
	t->y = CENTRE_Y;

	// Scale Factors for the +ve/-ve x/y axes
	float sf1 = 0, sf2 = 0, sf3 = 0, sf4 = 0; 

	sf1 = Scale_Calc(test, MAX_BOUND, t->max_y, CENTRE_Y);
	sf2 = Scale_Calc(test, MAX_BOUND, t->max_x, CENTRE_X);
	sf3 = Scale_Calc(test, MIN_BOUND, t->min_y, CENTRE_Y);
	sf4 = Scale_Calc(test, MIN_BOUND, t->min_x, CENTRE_X);

	// Use the smallest of the four scale values to fit graphics
	t->scale = Minimum(test, sf1, sf2, sf3, sf4);
	
	if (t->scale == VERY_LARGE)
	{	
		wb_assert(t->scale == 1000, "Scale interpreted as very large wrongly")
		p->test_flag = 1;
		return;
		//ERROR("Nothing was drawn - cannot scale to fit the window")
	}

	wb_assert(t->scale <= sf1, "Minimum scale factor not selected")
	wb_assert(t->scale <= sf2, "Minimum scale factor not selected")
	wb_assert(t->scale <= sf3, "Minimum scale factor not selected")
	wb_assert(t->scale <= sf4, "Minimum scale factor not selected")

	//printf("Scaled to fit by factor: %.3f\n", t->scale);

	t->scaled = 1; 
	wb_assert(t->scaled == 1, "Scale mode not set high")
	//Prog(p, t); NOT IN TEST
}


/* Calculate the scale factor */
float Scale_Calc(Testing *test, int max_min, int bound, int centre){

	if (bound == centre)
	{	
		wb_assert(bound == CENTRE_X || bound == CENTRE_Y, "Bound is not equal to any central coord")
		return VERY_LARGE;
	}

	if (max_min == MAX_BOUND) 
	{	
		return (float) centre / (bound - centre);
	}

	else 
	{	
		wb_assert(max_min == MIN_BOUND, "Flow control error in Scale_Calc")
		return (float) centre / (centre - bound);
	}
}


/* Return the smallest of four floats */
float Minimum(Testing *test, float a, float b, float c, float d){

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

	wb_assert(min <= a, "Minimum value not selected")
	wb_assert(min <= b, "Minimum value not selected")
	wb_assert(min <= c, "Minimum value not selected")
	wb_assert(min <= d, "Minimum value not selected")

	return min;
}

/* Round float to NEAREST int */
int Rint(Testing *test, float a){

	if (a >= 0) // Positive
	{	
		wb_assert(a >= 0, "Flow problem in Rint")
		a = (a + 0.5);
	}
	else // Negative
	{	
		wb_assert(a < 0, "Flow problem in Rint")
		a = (a - 0.5);
	}

	return a;

}

