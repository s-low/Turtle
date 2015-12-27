#include "neillsdl2.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

//------------------------------------------
//------------- TEST "HARNESS" -------------
#include "minunit.h"
//------------------------------------------
//------------------------------------------

#define DELAY					4000
#define CURRENT 				p->wds[p->counter]
#define NEXT 					p->wds[p->counter+1]
#define PI 						3.14159265
#define ANSI_A 					65
#define NUM_VARS 				26
#define CENTRE_X				(WWIDTH / 2)	// 400
#define CENTRE_Y				(WHEIGHT / 2) 	// 300
#define MAX_BOUND               1
#define MIN_BOUND               0
#define VERY_LARGE				1000
#define ON                      1
#define OFF                     0

#define MAXTOKENSIZE 			7  		 // Maximum instruction length
#define MAXNUMTOKENS 			200 	 // Maximum number of tokens
#define STACKSIZE 				50		 
	
// ERROR: Exit and print associated message
#define ERROR(PHRASE) {fprintf(stderr, "ERROR: %s. \
%s, line %d\n", PHRASE, \
__FILE__, __LINE__); exit(2); }


// Program: Execution information
struct program{
   char wds[MAXNUMTOKENS][MAXTOKENSIZE]; // Array of strings/tokens
   int counter; 		 				 // Current token 
   float var[NUM_VARS];					 // 26 User variables (A-Z)
   int index, set_target;				 // Index of variable array
   int from, to;						 // Temporary stores for loop info
   int test_flag;						 // General testing flag
};
typedef struct program Program;

// Turtle: Graphics information
struct turtle{
	float scale;
	int scaled;
	float bearing;						 // Angular orientation of turtle
	int distance;						 // Distance to move along bearing
	int x, max_x, min_x;
	int y, max_y, min_y;
	SDL_Simplewin sw;					 // Window passed in turtle struct
};
typedef struct turtle Turtle;

//---------------------------------------------------
//-----------------TESTING STRUCT-------------------- 
// useful values and variables for all test functions 
struct Testing{
	int tests_run;
	int bb_asserts;
	int wb_asserts;

	Program program;
	Program *p;
	Turtle turtle;
	Turtle *t;
};
typedef struct Testing Testing;
/* This must be embedded here because it relies on above structs
but below functions rely on it */
//---------------------------------------------------
//--------------------------------------------------- 

enum inst{ FD, LT, RT, FROM, TO };
typedef enum inst Inst;

enum op{ ADD, SUB, MULT, DIV, NONE };
typedef enum op operator;

// Stack to hold vars, nums and operators of polish expression
struct stack{
    float array[STACKSIZE]; // array of floats
	int top; 				 
};
typedef struct stack Stack;

/* Struct to hold all the relevant counters for a single do loop */
struct do_info{
	int index, from, to, var, start;
};
typedef struct do_info Do_info;


// TEST_GRAMMAR.C
void Prog(Testing *test, Program *p, Turtle *t);
void InstructionList(Testing *test, Program *p, Turtle *t);
void Instruction(Testing *test, Program *p, Turtle *t);
void Varnum(Testing *test, Program *p, Turtle *t, Inst instruct);
void Var(Testing *test, Program *p, Turtle *t);
void Do(Testing *test, Program *p, Turtle *t);
void Set(Testing *test, Program *p, Turtle *t);
void Polish(Testing *test, Program *p, Turtle *t);
operator isOperator(Testing *test, char token[MAXTOKENSIZE]);

// TEST_HOUSEKEEPING.C
void Initialise_Program(Testing *test, Program *p);
void Initialise_Turtle(Testing *test, Turtle *t);
void Scan_Program(Testing *test, Program *p, char *argv[]);
int  StringMatch(Testing *test, char string1[], char string2[]);
void DrawLine(Testing *test, Turtle *t);
void Track_Bounds(Testing *test, Turtle *t);
void Scaled(Testing *test, Program *p, Turtle *t);
float Scale_Calc(Testing *test, int max_min, int bound, int centre);
float Minimum(Testing *test, float a, float b, float c, float d);
void ScanCheck(int a, int b);
int Rint(Testing *test, float a);

// TEST_STACK.C (Polish calculation)
void InitialiseStack(Testing *test, Stack *s);
void Push(Testing *test, Stack *s, float *num, Program *p);
float  Pop(Testing *test, Stack *s, Program *p);

