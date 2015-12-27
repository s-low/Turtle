/* 

EXTENSION: 

I got bored of writing turtle scripts, so the idea is to actually generate 
valid scripts for the turtle interpreter automatically. Inspired by the random 
maze generation from last term, and one of the software group projects who
built a tool for drawing fractals.

TWO MODES OF OPERATION:

RANDOM: word 'random' supplied as command line arg - random logo script is
generated using a reduced grammar. Sometimes it looks good sometimes it doesnt. 
Certain hardcoded parameters have been set to keep output sensible. Uses 
something like recursive descent as in the parser, but backwards.

DEFAULT/USER: A bit like a Turtle Assembler. Given simpler parameters than 
actual LOGO, generate a LOGO script that does the same thing. But less control, 
limited to a set number of nested do loops, each drawing shapes decreasing 
in size. Uses proper recusion: draw_depth calls draw_depth with a base case 
dependent on the user defined complexity.

*/

#include "turtle.h"
#include <time.h>

// USER PARAMETERS
#define SHAPE_TOKEN_LENGTH	15
#define NUM_INSTRUCTIONS 	4
#define MAX_COMPLEXITY		5
#define MIN_COMPLEXITY		1
#define NUM_SHAPES			10
#define SCALE_FACT			100

// RANDOM CONSTRAINTS
#define NOT_NESTED  		0
#define NESTED 				1
#define FIRST_VAR			0
#define DO_LOOPS  			3
#define MIN_TO				3
#define MAX_TO				10
#define MIN_ANGLE			5
#define MAX_ANGLE			180
#define MAX_FD				40
#define MIN_FD				5
#define GEN_LENGTH			5

enum shape{ triangle, square, pentagon};
typedef enum shape Shape;

struct script {
	
	// RANDOM STUFF
	float var[NUM_VARS];
	int var_index;
	int do_loops;
	FILE *output;
	int fd_min, fd_max;
	int turn_min, turn_max;
	int to_max, to_min;
	int length;
	int to;

	// USER STUFF
	int complexity;
	char * shapes[NUM_SHAPES];
	int depth;
	Shape array[NUM_SHAPES];

}; 
typedef struct script Script;

// GENERAL
void HandleArgs(Script *script, int argc, char * argv[]);
void Initialise_Script(Script *script);

// RANDOM
void Gen_Prog(Script *script);
void Gen_inst(Script *script);
void Gen_num(Script *script, int min, int max);
void Gen_Do(Script *script);

// USER/DEFAULT
void Draw(Script *script, Shape shape, int scale);
void Gen_UserProg(Script *script);
void Draw_Depth(Script *script);
int Check_Complexity(int complexity);
int Check_Shape_Input(Script *script, int i);

// MAIN
int main(int argc, char *argv[])
{
	srand(time(NULL));
	
	Script thing;
	Script *script = &thing;
	Initialise_Script(script);	
	
	HandleArgs(script, argc, argv);
	
	fprintf(script->output, "}");
	printf("\nSCRIPT PRINTED TO OUTPUT.TXT\n");

	return 0;
}

/* Initialise parameters, in particular those that affect random scripts */
void Initialise_Script(Script *script){

	 // Open file for reading
	if(!(script->output = fopen("output.txt", "w"))){
		ERROR("Couldn't open the file for writing")
	}


	script->depth = 0;	// Initially we start at level 0

	script->var_index = FIRST_VAR;  // Initially set the var index to A
	script->do_loops = 	DO_LOOPS;   // Number of do loops allowed
	script->to_min = MIN_TO;
	script->to_max = MAX_TO;
	script->turn_min = MIN_ANGLE;
	script->turn_max = MAX_ANGLE;
	script->fd_max = MAX_FD;
	script->fd_min = MIN_FD;
	script->length = GEN_LENGTH;

}


/* Hide the super dull argument handling and input checking */
void HandleArgs(Script *script, int argc, char * argv[]){
	
	if (argc == 2 ) // Generate random output
	{	
		if ( StringMatch(argv[1], "random"))
		{
			Gen_Prog(script); 
			printf("\n---------RANDOM OUTPUT---------\n\n");
		}
		else
		{
			printf("Did you mean 'random' ?\n");
			exit(2);
		}
	}
	else if(argc == 1) // Default mode. User parameters.
	{
		Gen_UserProg(script);
	}
	else
	{
		printf("Try and stick to just the one arg: 'random' or nothing at all\n");
		exit(2);
	}
}


/* Generate a program based on user input */
void Gen_UserProg(Script *script){

	printf("\n---------TURTLE GENERATOR---------\n");
	printf("\n- The Turtle Generator will generate logo scripts based on the\
 parameters you give it.\n- Parameters are complexity and shape.\n\
- Complexity is an integer between 1 and 5.\n\
- Valid shape inputs are square, triangle, pentagon only.\n\
- You can also just enter 's' 't' or 'p'.\n\
- You can also enter 'random' as argv[1] for random output.\n");
	
	printf("\nHow many levels of complexity: ");

	if( scanf("%d", &script->complexity) != 1 ){
		printf("ERROR: Please enter a valid integer.\n");
		exit(2);
	}   
	else if( Check_Complexity(script->complexity) != 1){
		exit(2);
	}  

	int store = script->complexity;
	int i = 0, try_again = OFF;

	// Ask the user for as many shapes as they have specified levels:
	while(store != 0){

		script->shapes[i] = malloc(sizeof(script->shapes[SHAPE_TOKEN_LENGTH]));
		
		char ch = ' ';

		// Keep prompting for strings until valid:
		do{	
			if(try_again){
				printf("\nInvalid input. Try again with: 'p' 't' or 's'\n");
			}
			
			printf("\nShape at depth %d: ", (script->complexity - store) + 1);
			scanf("%s", script->shapes[i]); // Scan input into string array
			
			while( (ch = getchar()) != '\n'); // Gobble any stdin
			
			try_again = ON;
		
		}while( Check_Shape_Input(script, i) == 0 ); 
		
		try_again = OFF;

		store--;
		i++;
	}

	fprintf(script->output, "{ \n"); // The program's opening brace

	Draw_Depth(script);
}


/* Given integer user input, constrain it */
int Check_Complexity(int complexity){

	if (complexity > MAX_COMPLEXITY || complexity < MIN_COMPLEXITY)
	{
		printf("ERROR: Please enter a number between %d and %d\n", MIN_COMPLEXITY, MAX_COMPLEXITY);
		return 0;
	}
	else
	{
		if (complexity == MIN_COMPLEXITY)
		{
			printf("Warning: Complexity of one is predictably dull\n");
		}

		printf("\nComplexity set to %d\n", complexity);

		return 1;
	}
}


/* Check the users string input is valid and if so set enum shape type */
int Check_Shape_Input(Script *script, int i){

	if ( StringMatch(script->shapes[i], "square") || StringMatch(script->shapes[i], "s"))
	{	
		script->array[i] = square;
		return 1;
	}

	if ( StringMatch(script->shapes[i], "pentagon") || StringMatch(script->shapes[i], "p"))
	{
		script->array[i] = pentagon;
		return 1;
	}

	if ( StringMatch(script->shapes[i], "triangle") || StringMatch(script->shapes[i], "t"))
	{
		script->array[i] = triangle;
		return 1;
	}

	return 0; // Input matches no valid parameter
}


/* Creates the DO loop to draw the current level, recurses down*/
void Draw_Depth(Script *script){
	
	switch( script->array[script->depth] ){

		case square: 
			Draw(script, square, SCALE_FACT * (script->complexity - script->depth) );
		break;

		case triangle:
			Draw(script, triangle, SCALE_FACT * (script->complexity - script->depth) );
		break;

		case pentagon:
			Draw(script, pentagon, SCALE_FACT * (script->complexity - script->depth) );
		break;
	}
	
	script->depth++; // One more layer done.

	if (script->depth != script->complexity)
	{
		Draw_Depth(script); // Recursive call 
	}

	fprintf(script->output, " }\n");

	return;	
}

/* Draw the given shape on a given scale */
void Draw(Script *script, Shape shape, int scale){

	int sides, angle;
	char var = script->depth + ANSI_A;


	switch(shape){
		case triangle:
			sides = 3; // Ironically 3 isn't a magic number in this case.
		break;

		case square:
			sides = 4; // It's a square -> four sides
		break;

		case pentagon:
			sides = 5; // It's a pentagon...
		break;
	}

	angle = DEGREES / sides; //360 degrees in a circle

	fprintf(script->output, "\nDO %c FROM 1 TO %d \n{ \n", var, sides);
	fprintf(script->output, "FD %d RT %d ", scale, angle);
}


// Generate the first opening brace then the program within
void Gen_Prog(Script *script){

	fprintf(script->output, "{ \n"); // Print opening brace

	for (int i = 0; i < script->length; ++i)
	{
		Gen_inst(script);
	}
	
}

// Generate a random instruction
void Gen_inst(Script *script){

	int x;

	x = rand() % NUM_INSTRUCTIONS; 

	switch(x){

		case 0:
			fprintf(script->output, "FD ");
			Gen_num(script, script->fd_min, script->fd_max);
		break;

		case 1:
			fprintf(script->output, "RT ");
			Gen_num(script, script->turn_min, script->turn_max);
		break;

		case 2:
			fprintf(script->output, "LT ");
			Gen_num(script, script->turn_min, script->turn_max);
		break;

		case 3:
			if (script->do_loops != 0) // Limit excessive loops
			{
				Gen_Do(script);
			}
		break;
	}
}


// Generate a value between min and max
void Gen_num(Script *script, int min, int max){

	int num = min + (rand() % (max - min + 1));

	fprintf(script->output, "%d \n", num);
}


// Print to file the NEXT variable sequentially
void Gen_var(Script *script){

	char c = script->var_index + ANSI_A;

	fprintf(script->output, "%c ", c);

	script->var_index++; // Move up the list
}


// Generate a complete DO loop "DO x FROM a TO b {PROG}"
void Gen_Do(Script *script){

	fprintf(script->output, "DO ");
	Gen_var(script);
	
	fprintf(script->output, "FROM 1 ");

	fprintf(script->output, "TO ");
	Gen_num(script, script->to_min, script->to_max);

	script->do_loops--;  // Decrease the counter for permissible do loops left

	Gen_Prog(script); // Recursive call by DO: NESTED
	
	fprintf(script->output, "} \n");

}
