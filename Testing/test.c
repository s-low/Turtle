/* 
MINUNIT: I'm using minunit - mainly because it's so dead simple it's barely
a testing framework. It just sort of gets out of the way.

I've modified minunit slightly to count both tests "suites" and two types of
asserts (white and black box). A suite in this case meaning a set of test cases 
on one function.

I've also added MESSAGE_RETURN() so that any asserts within test_case 
functions succesfully rise to the main test.
*/

/*
The below is BLACK BOX testing on a unit scale. Each function is supplied
with a range of different input cases, and the output/effect of the function 
call is tested against expectation. 

Each test is based on the same structure:

				test_function_A(){
					setup_some_conditions;
					function_A(conditions);
					assertion(expected_result);
				}

Recursive calls have been removed. Meaning that functions have been allowed to
call one another as long as it does not lead to a recursive loop. 

EXAMPLE: 
Do() can call Var() to check that a valid Var() appears in the program as 
expected, but cannot call Prog() on the nested loop contents. Also:

	- InstructionList cannot call itself.

	- Instruction has been stubbed.

*/

#include "minunit.h"
#include "testing_prototypes.h"

// MAIN
int main(int argc, char const *argv[])
{	
	Testing testing; // Test structure to pass around counters and useful items
	Testing *test = &testing;
	Init_Test_Struct(test);

	char * result = all_tests(test);

	printf("\nBLACK BOX: ");

	if (result != 0)
	{
		printf("\nFAIL: %s\n", result);
	}
	else
	{
		printf("ALL TESTS PASSED\n");
	}

	// Regardless of outcome display the number of tests run to that point
	printf(" Test Suites Run: %d\n", test->tests_run);
	printf("Total BB Asserts: %d\n\n", test->bb_asserts);

	printf("\nWHITE BOX: ALL TESTS PASSED");
	printf("\nTotal WB Asserts: %d\n\n", test->wb_asserts);
	return 0;
}

void Init_Test_Struct(Testing *test){
	
	test->tests_run = 0;
	test->bb_asserts = 0;
	test->wb_asserts = 0;
	test->p = &test->program;
	test->t = &test->turtle;
}

/* RUN ALL */
char * all_tests(Testing *test) {

	mu_run_test(test_Prog, test);
	mu_run_test(test_InstructionList, test);
	mu_run_test(test_Instruction, test);
	mu_run_test(test_Varnum, test);
	mu_run_test(test_Do, test);
	mu_run_test(test_Set, test);
	mu_run_test(test_Var, test);
	mu_run_test(test_Polish, test);
	mu_run_test(test_isOperator, test);

	mu_run_test(test_Stack, test);

	mu_run_test(test_Initialise_Program, test);
	mu_run_test(test_Scan_Program, test);
	mu_run_test(test_StringMatch, test);
	mu_run_test(test_DrawLine, test);
	mu_run_test(test_Track_Bounds, test);
	mu_run_test(test_Scaled, test);
	mu_run_test(test_Rint, test);

	return 0;
}


/* Useful little float comparison function */
int float_compare(float a, float b, float tolerance){

	if (fabs(a - b) < tolerance) {
		return 1;
	}
	else {
		return 0;
	}
}


/* SUITE 1: PROGRAM */
char * test_Prog(Testing *test){
	
	char * message;
	MESSAGE_RETURN( Prog_Case(test, "{", 1, "Prog() ignored opening bracket"); )

	MESSAGE_RETURN( Prog_Case(test, "", 0, "Prog() ignored missing opening bracket"); )

	return 0;
}

char * Prog_Case(Testing *test, char * c, int x, char * message){
	
	test->p->counter = 0;
	strcpy(test->p->wds[test->p->counter], c);
	Prog(test, test->p, test->t);
	bb_assert(message, test->p->counter == x);

	return 0;
}

/* SUITE 2: INSTRUCTION LIST */
char * test_InstructionList(Testing *test){
	
	char * message; 
	MESSAGE_RETURN(InstructionList_Case(test, "}", 0, "InstructionList() ignored closing bracket");)
	
	MESSAGE_RETURN(InstructionList_Case(test, "", 1, "InstructionList() ignored missing closing bracket");)

	return 0;
}

char * InstructionList_Case(Testing *test, char * c, int x, char * message){

	test->p->counter = 0;
	strcpy(test->p->wds[test->p->counter], c);	
	InstructionList(test, test->p, test->t);
	bb_assert(message, test->p->counter == x);	

	return 0;
}

/* SUITE 3: INSTRUCTION */
char * test_Instruction(Testing *test){
	
	char * message;
	MESSAGE_RETURN(Instruction_Case(test, "FD", 1, "Instruction() ignored FD"); )
	
	MESSAGE_RETURN(Instruction_Case(test, "LT", 1, "Instruction() ignored LT");)
	
	MESSAGE_RETURN(Instruction_Case(test, "RT", 1, "Instruction() ignored RT");)
	
	MESSAGE_RETURN(Instruction_Case(test, "DO", 1, "Instruction() ignored DO");)

	MESSAGE_RETURN(Instruction_Case(test, "SET", 1, "Instruction() ignored SET");)

	MESSAGE_RETURN(Instruction_Case(test, "Other", 2, "Instruction() accepted \
		invalid instruction");)

	return 0;
}

char * Instruction_Case(Testing *test, char * c, int x, char * message){

	//Test Setup
	test->p->counter = 0;
	strcpy(test->p->wds[test->p->counter], c);
	
	// Function Call
	Instruction(test, test->p, test->t);
	
	// Check effect
	bb_assert(message, test->p->counter == x);	

	return 0;
}

/* SUITE 4: VARNUM */
char * test_Varnum(Testing *test){

	char * message;

	// Varnum_case takes: test, token, instruct, value of token, message

	// FD 10
	MESSAGE_RETURN(Varnum_case(test, "10", FD, 10, "Varnum() ignored valid FD <num>");)

	// LT 8
	MESSAGE_RETURN(Varnum_case(test, "8", LT, 8, "Varnum() ignored valid LT <num>");)

	// RT 90
	MESSAGE_RETURN(Varnum_case(test, "90", RT, 90, "Varnum() ignored valid RT <num>");)

	// FROM 3
	MESSAGE_RETURN(Varnum_case(test, "3", FROM, 3, "Varnum() ignored valid FROM <num>");)

	// TO 15
	MESSAGE_RETURN(Varnum_case(test, "15", TO, 15, "Varnum() ignored valid TO <num>");)

	// FD A
	test->p->var[0] = 45;  // Arbitrary
	MESSAGE_RETURN(Varnum_case(test, "A", FD, 45, "Varnum() accepted invalid <num>");)

	// FD Z
	test->p->var[25] = 15; // Arbitrary
	MESSAGE_RETURN(Varnum_case(test, "Z", FD, 15, "Varnum() accepted invalid <num>");)

	return 0;
}

char * Varnum_case(Testing *test, char * token, Inst i, float x, char * message){

	test->p->counter = 0;
	test->t->bearing = 0;
	test->t->distance = 0;

	strcpy(test->p->wds[test->p->counter], token); // copy the token in to the prog

	float conversion = atoi(token); // Take the conversion

	Varnum(test, test->p, test->t, i); // Should set p->index by call to Var() as well

	if (conversion == 0) // Test token not a number but a variable
	{	
		x = test->p->var[test->p->index]; 
		// we want to check the result against it's value
	}
	
	switch(i){
		case FD:
			bb_assert(message, test->t->distance == x);
		break;

		case LT:
			x = (x / 360) * 2 * PI;
			bb_assert(message, test->t->bearing == (-1 * x) );
		break;

		case RT:
			x = (x / 360) * 2 * PI;
			bb_assert(message, test->t->bearing == x);
		break;

		case FROM:
			bb_assert(message, test->p->from == x);
		break;

		case TO:
			bb_assert(message, test->p->to == x);
		break;
	}

	return 0;
}

/* SUITE 5: DO */
char * test_Do(Testing *test){

	test->p->counter = 0;
	strcpy(test->p->wds[0], "A");
	strcpy(test->p->wds[1], "FROM");
	strcpy(test->p->wds[2], "1");
	strcpy(test->p->wds[3], "TO");
	strcpy(test->p->wds[4], "5");

	Do(test, test->p, test->t);

	bb_assert("Loop Variable incorrectly set in Do()", test->p->index == 0);
	bb_assert("FROM parameter incorectly set in Do()", test->p->from == 1);
	bb_assert("TO parameter incorectly set in Do()", test->p->to == 5);
	bb_assert("Loop iterated incorectly in Do()", test->p->var[test->p->index] == 6);

	return 0;
}


/* SUITE 6: SET */
char * test_Set(Testing *test){

	char * message;
	MESSAGE_RETURN(Set_Case(test, "A", ":=");)

	MESSAGE_RETURN(Set_Case(test, "B", ":=");)

	MESSAGE_RETURN(Set_Case(test, "C", "=");)

	return 0;
}

char * Set_Case(Testing *test, char *var, char *equals){
 		
 	test->p->counter = 0;
	test->p->index = 0;

	strcpy(test->p->wds[0], var);
	strcpy(test->p->wds[1], equals);

	Set(test, test->p, test->t);

	if (strcmp(equals, ":=") == 0)
	{
		bb_assert("Valid <var>:= ignored in function Set()", test->p->counter == 2);
	}
	else
	{
		bb_assert("Invalid := not recognised in function Set()", test->p->counter == 1);
	}

	return 0;
}

/* SUITE 7: VAR */
char * test_Var(Testing *test){

	char * message;
	MESSAGE_RETURN(Var_case(test, "A", 3, "Valid <var> ignored in function Var()");)

	MESSAGE_RETURN(Var_case(test, "ABC", 1, "Invalid multi-character \
		<var> accepted in function Var()");)

	MESSAGE_RETURN(Var_case(test, "a", 2, "Invalid character <var> accepted in \
		function Var()"))
	return 0;
}

char * Var_case(Testing *test, char * token, int expected, char * message){

	test->p->counter = 0;
	test->p->test_flag = 0;
	strcpy(test->p->wds[0], token);
	
	Var(test, test->p, test->t);

	bb_assert(message, test->p->test_flag == expected);

	return 0;
}


/* SUITE 8: POLISH */
char * test_Polish(Testing *test){


	// Case takes: (Test ID, list of tokens, polish result)

	// num num op ;
	Polish_Case( test, 1, "4", "3", "+", ";", "", "", 7); // First arg is test index

	// num num op num op ;
	Polish_Case( test, 2, "1", "2", "*", "4", "+", ";", 6);

	// var num op ;
	Polish_Case( test, 3, "A", "4", "+", ";", "", "", 4);

	// num num op other (missing semicolon)
	Polish_Case( test, 4, "3", "4", "+", "FD", "", "", 0);

	// num op num ; (incorrect polish, could underflow stack)
	Polish_Case( test, 5, "4", "*", "3", ";", "", "", 0);

	// Var with NaN value - unset
	Polish_Case( test, 6, "A", ";", "", "","","", 0);

	return 0;
}

char * Polish_Case(Testing *test, int test_id, char * tkn1, char * tkn2,char * tkn3, 
	char * tkn4, char * tkn5, char * tkn6, int answer){

	test->p->counter = 0;
	test->p->var['A'- ANSI_A] = 0;

	strcpy(test->p->wds[0], tkn1); // Fill with parameter tokens
	strcpy(test->p->wds[1], tkn2);
	strcpy(test->p->wds[2], tkn3);
	strcpy(test->p->wds[3], tkn4);
	strcpy(test->p->wds[4], tkn5);
	strcpy(test->p->wds[5], tkn6);

	if (test_id == 6) // Case where user tries to set := NaN
	{
		test->p->var['A'- ANSI_A] = NAN;
	}

	Polish(test, test->p, test->t);

	// tests 4 and 5 are the specifically incorrect cases
	if(test_id == 4)
	{	
		bb_assert("Missing semi colon not caught in fn: Polish()", test->p->test_flag == 1 || test->p->test_flag == 2);
	}
	
	else if(test_id == 5)
	{
		bb_assert("Stack underflow permitted in fn: Polish()", test->p->test_flag == 3);
	}

	else if(test_id == 6)
	{	
		bb_assert("Unset Var used in Polish", test->p->test_flag == 6);
	}

	else // Correct polish, check the answer
	{
		bb_assert("Incorrect mathematical result in fn: Polish()", test->p->var[test->p->set_target] == answer);
	}

	return 0;
}

/* SUITE 9: OPERATOR */
char * test_isOperator(Testing *test){

	operator op;

	op = isOperator(test, "*");
	bb_assert("Valid operator ignored", op == MULT);

	op = isOperator(test, "+");
	bb_assert("Valid operator ignored", op == ADD);

	op = isOperator(test, "-");
	bb_assert("Valid operator ignored", op == SUB);

	op = isOperator(test, "/");
	bb_assert("Valid operator ignored", op == DIV);

	op = isOperator(test, "-*");
	bb_assert("Invalid operator accepted", op == NONE);

	op = isOperator(test, "ADD");
	bb_assert("Invalid operator accepted", op == NONE);

	return 0;
}


/* SUITE 10: Stack (init, push and pop) */
char * test_Stack(Testing *test){
	
	Stack stack;
	Stack *s = &stack;
	float num;

	// Test setting top to zero
	stack.top = 1; // Not zero
	InitialiseStack(test, s);
	bb_assert("Stack not succesfully initialised",stack.top == 0);

	// Test valid push, top zero
	Push_case(test, s, 3.45, test->p, 0);
	bb_assert("Top index not increased", stack.top == 1);
	bb_assert("Value not pushed", float_compare(stack.array[0], 3.45, 0.01));

	// Test valid push, top not zero
	Push_case(test, s, 7.14, test->p, 1);
	bb_assert("Top index not increased", stack.top == 2);
	bb_assert("Value not pushed", float_compare(stack.array[1], 7.14, 0.01));

	// Test overflow
	Push_case(test, s, 3.45, test->p, STACKSIZE);
	bb_assert("Stack overflow permitted", test->p->test_flag == 1);

	// Test valid pop
	Pop_case(test, s, test->p, &num, 15, 1); // Top = 1, try and pop
	bb_assert("Valid pop failed",float_compare(num, 15, 0.01));

	// Test underflow
	Pop_case(test, s, test->p, &num, 14, 0); // Top = 0, still try should fail
	bb_assert("Stack underflow permitted", test->p->test_flag == 1);

	return 0;
}

// Test Push() on a stack whose top is preset 
char * Push_case(Testing *test, Stack *s, float num, Program *p, int top){
	p->test_flag = 0;
	
	s->top = top;
	Push(test, s, &num, p);	
	
	return 0;
}

// Test Pop() on a stack whose top and values are preset
char * Pop_case(Testing *test, Stack *s, Program *p, float * popped, int value, int top){

	p->test_flag = 0;

	s->top = top;
	s->array[top - 1] = value;
	*popped = Pop(test, s, p);

	return 0;
}


/* SUITE 11: PROGRAM INITIALISER */
char * test_Initialise_Program(Testing *test){

	test->p->test_flag = 0;
	test->p->counter = 5; // Non-zero
	test->p->var[0] = 4;  // A = Some number

	Initialise_Program(test, test->p);

	bb_assert("Program counter incorrectly initialised", test->p->counter == 0);

	// Every var should now be NaN
	for (int i = 0; i < NUM_VARS; ++i)
	{
		bb_assert("Var not initialised to NaN", !(test->p->var[i] == test->p->var[i]) );
	}

	// Every token should be a terminator
	for (int i = 0; i < MAXNUMTOKENS; ++i) 
	{
		bb_assert("Token not initialised to terminator",test->p->wds[i][0] == '\0');
	}

	return 0;
}


/* SUITE 12: FILE INPUT */
char * test_Scan_Program(Testing *test){
	
	test->p->counter = 0;
	test->p->test_flag = 0;

	char * argv[3]; // Declare an artificial command line arg

	argv[0] = "turtle";
	argv[1] = "filename.txt";
	argv[2] = NULL;

	Scan_Program(test, test->p, argv);
	bb_assert("Invalid file opened for input", test->p->test_flag == 1);

	test->p->counter = 0;
	test->p->test_flag = 0;

	argv[0] = "turtle";
	argv[1] = "test_input.txt"; // Below token checking corresponds
	argv[2] = NULL;

	Scan_Program(test, test->p, argv);

	// test_input contains examples of valid tokens
	bb_assert("Token incorrectly scanned", !(strcmp(test->p->wds[0], "{")) );
	bb_assert("Token incorrectly scanned", !(strcmp(test->p->wds[1], "FD")) );
	bb_assert("Token incorrectly scanned", !(strcmp(test->p->wds[2], "45")) );
	bb_assert("Token incorrectly scanned", !(strcmp(test->p->wds[3], "SET")) );
	bb_assert("Token incorrectly scanned", !(strcmp(test->p->wds[4], "A")) );
	bb_assert("Token incorrectly scanned", !(strcmp(test->p->wds[5], ":=")) );
	bb_assert("Token incorrectly scanned", !(strcmp(test->p->wds[6], "DO")) );
	bb_assert("Token incorrectly scanned", !(strcmp(test->p->wds[7], "FROM")) );
	bb_assert("Token incorrectly scanned", !(strcmp(test->p->wds[8], "TO")) );
	bb_assert("Token incorrectly scanned", !(strcmp(test->p->wds[9], ";")) );
	bb_assert("Token incorrectly scanned", !(strcmp(test->p->wds[10], "}")) );

	return 0;
}

/* SUITE 13: STRING COMPARISON */
char * test_StringMatch(Testing *test){
	
	int result;
	
	char string1[] = "matching";
	char string2[] = "matching";

	result = StringMatch( test,string1, string2);
	bb_assert("Matching strings unrecognised",result == 1);

	char string3[] = "matching";
	char string4[] = "Matching";

	result = StringMatch( test,string3, string4);
	bb_assert("Non-matching strings unrecognised",result == 0);	

	char string5[] = "Hello";
	char string6[] = "Goodbye";

	result = StringMatch( test,string5, string6);
	bb_assert("Non-matching strings unrecognised",result == 0);	

	return 0;
}


/* SUITE 14: INTERPRET / DRAW LINE */
char * test_DrawLine(Testing *test){

/*	Given starting coords, bearing and distance - check final coords.
	SDL is disabled for test, but will just draw line between the two.
	Pixel are discrete values - The functions to calculate the next coords
	should at least be accurate to a pixel in obvious cases such as straight 
	lines (below) but can be off by a couple without problems on the angle
*/

	// Setup function takes (origin_x, origin_y, bearing, distance):

	// 100 px upwards from centre
	Setup_DrawLine_case(test, CENTRE_X, CENTRE_Y, 0, 100);
	DrawLine(test, test->t);
	bb_assert("Incorrect final x coord in drawline", test->t->x == CENTRE_X);
	bb_assert("Incorrect final y coord in drawline", test->t->y == CENTRE_Y - 100);
	
	
	// 100 px right from centre
	Setup_DrawLine_case(test, CENTRE_X, CENTRE_Y, PI / 2 , 100);
	DrawLine(test, test->t);
	bb_assert("Incorrect final x coord in drawline", test->t->x == CENTRE_X + 100);
	bb_assert("Incorrect final y coord in drawline", test->t->y == CENTRE_Y);

	// 100 px left from centre
	Setup_DrawLine_case(test, CENTRE_X, CENTRE_Y, 3 * PI / 2, 100);
	DrawLine(test, test->t);
	bb_assert("Incorrect final x coord in drawline", test->t->x == CENTRE_X - 100);
	bb_assert("Incorrect final y coord in drawline", test->t->y == CENTRE_Y);

	// 100 px down from centre
	Setup_DrawLine_case(test, CENTRE_X, CENTRE_Y, PI, 100);
	DrawLine(test, test->t);
	bb_assert("Incorrect final x coord in drawline", test->t->x == CENTRE_X);
	bb_assert("Incorrect final y coord in drawline", test->t->y == CENTRE_Y + 100);

	// Check that call to Track_bounds is made:
	Setup_DrawLine_case(test, CENTRE_X, CENTRE_Y, PI / 2, 100);	
	DrawLine(test, test->t);

	bb_assert("Call to Track_bounds not made in drawline", test->t->max_x == CENTRE_X + 100);

	return 0;

}

/* Hide away some of the setup work */
void Setup_DrawLine_case(Testing *test, int origin_x, int origin_y, float test_bearing, int distance){
	
	test->t->max_x = CENTRE_X;
	test->t->max_y = CENTRE_Y;
	test->t->x = origin_x;
	test->t->y = origin_y;
	
	test->t->scaled = 0;
	test->t->bearing = test_bearing;
	test->t->distance = distance;
}


/* SUITE 15: Track_Bounds */
char * test_Track_Bounds(Testing *test){

	// Setup takes: x, minx, maxx, y, miny, maxy
	
	// New max x and y only
	Setup_Track_Bounds_case(test, 500, CENTRE_X, CENTRE_X, 500, CENTRE_Y, CENTRE_Y);
	Track_Bounds(test, test->t);
	bb_assert("New maximum x bound not set", test->t->max_x == 500);
	bb_assert("New maximum y bound not set", test->t->max_y == 500);
	bb_assert("New minimum y bound set incorrectly", test->t->min_y == CENTRE_Y);
	bb_assert("New minimum x bound set incorrectly", test->t->min_x == CENTRE_X);

	// New min x and y only
	Setup_Track_Bounds_case(test, 200, CENTRE_X, CENTRE_X, 200, CENTRE_Y, CENTRE_Y);
	Track_Bounds(test, test->t);
	bb_assert("New maximum x bound set incorrectly", test->t->max_x == CENTRE_X);
	bb_assert("New maximum y bound set incorrectly", test->t->max_y == CENTRE_Y);
	bb_assert("New minimum y bound not set", test->t->min_y == 200);
	bb_assert("New minimum x bound not set", test->t->min_x == 200);

	// No change
	Setup_Track_Bounds_case(test, CENTRE_X, CENTRE_X, CENTRE_X, CENTRE_Y, CENTRE_Y, CENTRE_Y);
	Track_Bounds(test, test->t);
	bb_assert("New maximum x bound set incorrectly", test->t->max_x == CENTRE_X);
	bb_assert("New maximum y bound set incorrectly", test->t->max_y == CENTRE_Y);
	bb_assert("New minimum y bound set incorrectly", test->t->min_y == CENTRE_Y);
	bb_assert("New minimum x bound set incorrectly", test->t->min_x == CENTRE_X);
	return 0;
}


void Setup_Track_Bounds_case(Testing *test, int x, int min_x, int max_x, 
								int y, int min_y, int max_y){
	test->t->x = x;
	test->t->min_x = min_x;
	test->t->max_x = max_x;
	
	test->t->y = y;
	test->t->min_y = min_y;
	test->t->max_y = max_y;
}


/* SUITE 16: SCALED RUN
COMBINED TEST OF Scaled(), Scale_Calc() and Minimum() */
char * test_Scaled(Testing *test){

	// Given certain bounds, check the correct scale factor:

	// +ve x extends out of window by factor of 2. SF = 0.5
	Setup_Scaled_Case(test, CENTRE_X, CENTRE_X + 2*CENTRE_X, CENTRE_Y, CENTRE_Y);
	Scaled(test, test->p, test->t);
	bb_assert("Incorrect scale factor", float_compare(test->t->scale, 0.5, 0.01));

	// Needs shrinking a by 0.5 in +ve x, but by 0.25 in -ve x. SF = 0.25
	Setup_Scaled_Case(test, CENTRE_X - 4*CENTRE_X, (CENTRE_X + 2*CENTRE_X), CENTRE_Y, CENTRE_Y);
	Scaled(test, test->p, test->t);
	bb_assert("Incorrect scale factor", float_compare(test->t->scale, 0.25, 0.01));

	// +ve x fills half of sector. Can scale up by SF = 2.0
	Setup_Scaled_Case(test, CENTRE_X, (CENTRE_X + CENTRE_X/2), CENTRE_Y, CENTRE_Y);
	Scaled(test, test->p, test->t);
	bb_assert("Incorrect scale factor", float_compare(test->t->scale, 2, 0.01));

	// Nothing drawn (central bounds) - could scale infinitely - should catch.
	test->p->test_flag = 0;
	Setup_Scaled_Case(test, CENTRE_X, CENTRE_X, CENTRE_Y, CENTRE_Y);
	Scaled(test, test->p, test->t);
	bb_assert("Infinite scaling not caught", test->p->test_flag == 1);
	bb_assert("Scale factor incorrectly set high", test->t->scale == VERY_LARGE);

	return 0;
}

void Setup_Scaled_Case(Testing *test, int min_x, int max_x, int min_y, int max_y){

	test->t->max_x = max_x;
	test->t->max_y = max_y;
	test->t->min_y = min_y;
	test->t->min_x = min_x;
}


/* SUITE 17: RINT */
char * test_Rint(Testing *test){
	
	int a = 0;
	
	// Round down
	float b = 4.4;
	a = Rint(test, b);
	bb_assert("Rounding error in Rint", a == 4);

	// Round up
	b = 4.8;
	a = Rint(test, b);
	bb_assert("Rounding error in Rint", a == 5);

	// Round "down" negatively
	b = -4.8;
	a = Rint(test, b);
	bb_assert("Rounding error in Rint", a == -5);

	// Round "up" negatively
	b = -4.4;
	a = Rint(test, b);
	bb_assert("Rounding error in Rint", a == -4);


	return 0;	
}
