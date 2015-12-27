/*

TEST.H contains testing specific prototypes.

The main program's functions can be found in a modified testing form in 
test_grammar.c, test_stack.c and test_housekeeping.c, with protoypes in 
test_turtle.c

The "Testing" STRUCT is in test_turtle.h also, because it has been used 
in the modified functions to record white_box asserts

*/

#include "test_turtle.h"


// TESTING PROTOTYPES:
// General
void Init_Test_Struct(Testing *test);

int  float_compare(float a, float b, float tolerance);

char * all_tests(Testing *test);

// Prog
char * test_Prog(Testing *test);
char * Prog_Case(Testing *test, char * c, int x, char * message);

// InstructionList
char * test_InstructionList(Testing *test);
char * InstructionList_Case(Testing *test, char * c, int x, char * message);

// Instruction
char * test_Instruction(Testing *test);
char * Instruction_Case(Testing *test, char * c, int x, char * message);

// Varnum
char * test_Varnum(Testing *test);
char * Varnum_case(Testing *test, char * varnum, Inst i, float x, char * message);

// Do
char * test_Do(Testing *test);

// Set
char * test_Set(Testing *test);
char * Set_Case(Testing *test, char *var, char *equals);

// Var
char * test_Var(Testing *test);
char * Var_case(Testing *test, char * token, int expected, char * message);

// Polish
char * test_Polish(Testing *test);
char * Polish_Case(Testing *test, int test_id, char * tkn1, char * tkn2,char * tkn3,
	char * tkn4, char * tkn5, char * tkn6, int answer);

// isOperator
char * test_isOperator(Testing *test);

// Stack (all)
char * test_Stack(Testing *test);
char * Push_case(Testing *test, Stack *s, float num, Program *p, int top);
char * Pop_case(Testing *test, Stack *s, Program *p, float * popped, int value, int top);

// IntialiseProgram
char * test_Initialise_Program(Testing *test);

// Scan_Program
char * test_Scan_Program(Testing *test);

// StringMatch
char * test_StringMatch(Testing *test);

// DrawLine
char * test_DrawLine(Testing *test);
void Setup_DrawLine_case(Testing *test, int x, int y, float bearing, int distance);

// TrackBounds
char * test_Track_Bounds(Testing *test);
void Setup_Track_Bounds_case(Testing *test, int x, int min_x, int max_x, 
								int y, int min_y, int max_y);

// Scaled
char * test_Scaled(Testing *test);
void Setup_Scaled_Case(Testing *test, int min_x, int max_x, int min_y, int max_y);

char * test_Rint(Testing *test);
