#include "neillsdl2.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#define DEGREES					360
#define DELAY					4000
#define CURRENT 				p->wds[p->counter]
#define NEXT 					p->wds[p->counter+1]
#define PI 						3.142
#define ANSI_A 					65
#define NUM_VARS 				26
#define CENTRE_X				(WWIDTH/2)
#define CENTRE_Y				(WHEIGHT/2)
#define MAX_BOUND               1
#define MIN_BOUND               0
#define VERY_LARGE 				1000	 // Arbitrary very large scale factor
#define ON                      1
#define OFF                     0

#define MAXTOKENSIZE 			7  		 // Maximum token length
#define MAXNUMTOKENS 			200		 // Maximum number of tokens
#define STACKSIZE 				50		 // Maximum length of polish exp

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
   int test_flag;
};
typedef struct program Program;

// Turtle: Graphics information
struct turtle{
	float scale;
	int scaled;
	double bearing;						 // Angular orientation of turtle
	int distance;						 // Distance to move along bearing
	int x, max_x, min_x;
	int y, max_y, min_y;
	SDL_Simplewin sw;
};
typedef struct turtle Turtle;

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

// GRAMMAR.C
void Prog(Program *p, Turtle *t);
void InstructionList(Program *p, Turtle *t);
void Instruction(Program *p, Turtle *t);
void Varnum(Program *p, Turtle *t, Inst instruct);
void Var(Program *p, Turtle *t);
void Do(Program *p, Turtle *t);
void Set(Program *p, Turtle *t);
void Polish(Program *p, Turtle *t);
operator isOperator(char token[MAXTOKENSIZE]);

// HOUSEKEEPING.C
void Initialise_Program(Program *p);
void Initialise_Turtle(Turtle *t);
void Scan_Program(Program *p, char *argv[]);
int  StringMatch(char string1[], char string2[]);
void DrawLine(Turtle *t);
void Track_Bounds(Turtle *t);
void Scaled(Program *p, Turtle *t);
float Scale_Calc(int max_min, int bound, int centre);
float Minimum(float a, float b, float c, float d);
int  Rint(float a);
void ScanCheck(int a, int b);

// STACK.C (Polish calculation)
void InitialiseStack(Stack *s);
void Push(Stack *s, float *num);
float  Pop(Stack *s);
