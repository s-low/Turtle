#include "test_turtle.h"

/* Grammar specific functions, based around: "Expecting <x> next" idea

In order:
	PROG
	INSTRUCTIONLIST
	INSTRUCTION
	VARNUM
	VAR
	DO
	SET
	POLISH
	OPERATOR

-----TESTING VERSIONS-----

Recursive calls have been removed. Meaning that functions have been allowed to
call one another as long as it does not lead to a recursive loop. 

EXAMPLE: 
Do() can call Var() to check that a valid Var() appears in the program, but 
cannot call Prog() on the nested loop contents. 

	- InstructionList cannot call itself.

	- Instruction has been stubbed (does not call varnum, do or set)

*/

/* Check the program begins with an opening brace */
void Prog(Testing *test, Program *p, Turtle *t){

	if(!StringMatch( test,p->wds[p->counter], "{")){
		return;
	}

	int test_store = p->counter;
	(p->counter)++; // Move to next instruction
	wb_assert(p->counter == test_store + 1, "Counter not incremented")
}


/* Check code consists of either closing brace or instruction */
void InstructionList(Testing *test, Program *p, Turtle *t){

	// End of program: base case
	if( StringMatch( test, p->wds[p->counter], "}") ){
		return;
	}
	
	int test_store = p->counter;
	(p->counter)++;	    	  // Next instruction
	wb_assert(p->counter == test_store + 1, "Counter not incremented")

}


/* Expecting an instruction - STUBBED */
void Instruction(Testing *test, Program *p, Turtle *t){

	char instrct[MAXTOKENSIZE]; 
	strcpy(instrct, p->wds[p->counter]);
	(p->counter)++; // Next instruction

	if (StringMatch( test,instrct, "FD"))
	{	
		wb_assert(strcmp(instrct,"FD") == 0, "StringMatch malfunction")
		return;
	}
	else if(StringMatch( test,instrct, "LT"))
	{	
		wb_assert(strcmp(instrct,"LT") == 0, "StringMatch malfunction")
		return;
	}
	else if (StringMatch( test,instrct, "RT"))
	{	
		wb_assert(strcmp(instrct,"RT") == 0, "StringMatch malfunction")
		return;
	}
	else if (StringMatch( test,instrct, "DO"))
	{	
		wb_assert(strcmp(instrct,"DO") == 0, "StringMatch malfunction")
		return;
	}
	else if (StringMatch( test,instrct, "SET"))
	{	
		wb_assert(strcmp(instrct,"SET") == 0, "StringMatch malfunction")
		return;
	}

	int test_store = p->counter;
	(p->counter)++;	    	  // JUST FOR TESTING
	wb_assert(p->counter == test_store + 1, "Counter not incremented")
}


/* Checks that we get a valid <varnum> next, and returns it's value */
void Varnum(Testing *test, Program *p, Turtle *t, Inst instruct){

	float conversion = atoi(p->wds[p->counter]);
	float value = 0;
	int test_store = 0;

	if ( conversion != 0 ){			// If is number, take direct conversion
		value = conversion;
	}
	else							// If not a number, must be a var
	{	
		Var(test, p, t);								
		value = p->var[p->index];

		if ((value == value) == 0)  // Check that the variable has been set
		{	
			//ERROR("Variable is NaN. It has not been set yet")
		} 	
	}

	test_store = t->bearing;

	switch(instruct){
		case FD:
			t->distance = value;
			wb_assert(t->distance == value, "Distance not assigned correctly")
			// No Drawline in testing
		break;

		case LT:
			value = (value / 360) * 2 * PI;
			t->bearing -= value;
			wb_assert(t->bearing == test_store - value, "Distance not assigned correctly")
		break;

		case RT:
			value = (value / 360) * 2 * PI;
			t->bearing += value;
			wb_assert(t->bearing == test_store + value, "Distance not assigned correctly")
		break;

		case FROM:
			p->from = (int) value;
			wb_assert(p->from == value, "FROM index incorrectly set in Varnum")
		break;

		case TO:
			p->to = (int) value;
			wb_assert(p->to == value, "FROM index incorrectly set in Varnum")
		break;

		default:
		break;
	}
	
	return;
}


/* We are expecting a <var>, parses and "selects" the relevant variable */
void Var(Testing *test, Program *p, Turtle *t){

	if (strlen(p->wds[p->counter]) != 1)
	{	
		wb_assert( strlen(p->wds[p->counter]) > 1, "Zero character string?")
		p->test_flag = 1;
		return;

		//ERROR("Expected <var> but is multi-character")
	}

	char c = p->wds[p->counter][0];

	if (c > 'Z' || c < 'A')
	{	
		p->test_flag = 2;
		return;
		//ERROR("Expected <var> or possibly <varnum>")
	}
	else
	{	
		wb_assert(p->wds[p->counter][1] == '\0', "Multi-character token accepted as VAR")
		p->test_flag = 3;
		p->index = (int) c % ANSI_A; // Set the index of the array of vars
		wb_assert(p->index < NUM_VARS, "Index assigned to non-existant VAR")
		wb_assert(p->index >= 0, "Index assigned to non-existant VAR")
	}

	return;
}


/* <Do> is a more complicated token, with a longer function to match*/
void Do(Testing *test, Program *p, Turtle *t){	

	Do_info *do_loop = (Do_info *)malloc(sizeof(Do_info));
	// One struct to hold all the loop info (variable, from, to, start counter)
	// Using a local variable prevents a nested DO call overwriting the previous
	// Manual allocation from heap to prevent overflow if a LOT of DO calls

	Var(test, p, t);						// Expect <var>
	do_loop->var = p->index; 				// Set do variable to it

	int test_store = p->counter;
	(p->counter)++;	    	  // JUST FOR TESTING
	wb_assert(p->counter == test_store + 1, "Counter not incremented")	  
	
	if ( !StringMatch( test,CURRENT,"FROM") )    	// Then expect "FROM"
	{	
		wb_assert(strcmp(p->wds[p->counter], "FROM") != 0, "StringMatch Malfunction")
		p->test_flag = 1;
		return;
		//ERROR("Expected \"FROM\" in <DO>")
	}
	
	test_store = p->counter;
	(p->counter)++;	    	  // JUST FOR TESTING
	wb_assert(p->counter == test_store + 1, "Counter not incremented")   	 

	Varnum(test, p, t, FROM);				// Then a <varnum>
	do_loop->from = p->from;				// Set FROM 'x' value to it

	test_store = p->counter;
	(p->counter)++;	    	  // JUST FOR TESTING
	wb_assert(p->counter == test_store + 1, "Counter not incremented")
	
	if ( !StringMatch( test,CURRENT, "TO") )      // Then expect "TO"
	{	
		wb_assert(strcmp(p->wds[p->counter], "TO") != 0, "StringMatch Malfunction")
		p->test_flag = 2;
		return;
		//ERROR("Expected \"TO\" in <DO>")
	}
	
	test_store = p->counter;
	(p->counter)++;	    	  // JUST FOR TESTING
	wb_assert(p->counter == test_store + 1, "Counter not incremented")
	
	Varnum(test, p, t, TO);					// Then <varnum> again
	do_loop->to = p->to;					// Set TO 'y' value to it

	test_store = p->counter;
	(p->counter)++;	    	  // JUST FOR TESTING
	wb_assert(p->counter == test_store + 1, "Counter not incremented")

	do_loop->start = p->counter;			// Where to branch/loop back to

	// Repeatedly handle the complete program within the DO loop:
	for ( p->var[do_loop->var] = do_loop->from; 
			p->var[do_loop->var] <= do_loop->to; 
				++p->var[do_loop->var] )
	{	
		p->counter = do_loop->start;	// Reset P-counter to the loop start
		//Prog(p, t); TESTING 
		p->index = do_loop->var;		// Reset P-index to the loop variable
	}

	return;
}


/* <SET> */
void Set(Testing *test, Program *p, Turtle *t){

	Var(test, p, t); 								// Expect <var> after "SET"

	p->set_target = p->index;
	(p->counter)++;

	if ( !StringMatch( test,CURRENT, ":=") )  	// Then ":="
	{	
		wb_assert(strcmp(p->wds[p->counter], ":=") != 0, "StringMatch Malfunction")
		return;
		//ERROR("Expected \":=\" in <SET>")
	}

	int test_store = p->counter;
	(p->counter)++;	    	  // JUST FOR TESTING
	wb_assert(p->counter == test_store + 1, "Counter not incremented")
	//Polish(p, t);	TESTING					// Then some Polish
	
	return;
}


/* Postfix Reverse Polish Notation */
void Polish(Testing *test, Program *p, Turtle *t){

	int test_store = 0;
	Stack s;
	InitialiseStack(test, &s);
	wb_assert(s.top == 0, "Stack incorrectly initialised")
	
	// Only ever need THREE variables (A, B, C) to deal with any polish expr.
	float num = 0, a = 0, b = 0, c = 0, result = 0;
	operator operator;

	// Until we find a semicolon
	while( !StringMatch( test,CURRENT, ";") ){
			
		wb_assert(strcmp(p->wds[p->counter], ";") != 0, "StringMatch Malfunction")

		if( (num = atoi(CURRENT)) != 0) // If is a (non-zero) value 				
		{ 	
			Push(test, &s, &num, p); // Push for numbers
		} 

		else if ( (operator = isOperator(test, CURRENT)) != NONE )
		{	
			if (s.top < 2)
			{	
				// Pre-empt stack underflow
				p->test_flag = 3; // In place of ERROR(stack underflow)
				return;
			}

			test_store = s.top;
			a = Pop(test, &s, p);	// Pop for operators (no ERROR within, hence above)
			b = Pop(test, &s, p);
			wb_assert(s.top == test_store - 2, "Top index not decreased")

			switch(operator){
				
				case ADD:
					c = a + b;
					wb_assert(operator == ADD, "Incorrect operator decode")
				break;

				case SUB:
					c = a - b;
					wb_assert(operator == SUB, "Incorrect operator decode")
				break;

				case MULT:
					c = a * b;
					wb_assert(operator == MULT, "Incorrect operator decode")
				break;

				case DIV:
					c = b / a;
					wb_assert(operator == DIV, "Incorrect operator decode")
				break;

				case NONE:
					p->test_flag = 4;
					return;
					//ERROR("Missing a semicolon in Polish")
				break;

				default:
				break;
			}

			Push(test, &s, &c, p);
		}

		else if ( isOperator(test, CURRENT) == NONE)
		{	
			Var(test, p, t);				// Expecting a var if not an operator
			
			//Replacement for Var()'s normal ERROR():
			if(p->test_flag == 1 || p->test_flag == 2){
				return;
			} 

			num = p->var[p->index];

			if (!(num == num)) // ergo: is NaN
			{	
				p->test_flag = 6; // Replacement for below error
				return;
				//ERROR("Variable has not been set yet, cannot use in Polish")
			}

			Push(test, &s, &num, p);
		}

		else
		{	
			p->test_flag = 9;
			return;
			//ERROR("Incorrect Polish. Possibly missing a space")
		}

		int test_store = p->counter;
		(p->counter)++;	    	  
		wb_assert(p->counter == test_store + 1, "Counter not incremented")
	}
		
	if (s.top != 1) // top will be 1 if correct syntax
	{	
		p->test_flag = 5;
		return;
		//ERROR("Incorrect Polish syntax")
	}

	result = Pop(test, &s, p);

	p->var[p->set_target] = result; // Set the target var equal to result

	return;
}


/* Check if token is any of [+ * / -] */
operator isOperator(Testing *test, char token[MAXTOKENSIZE]){

	if (StringMatch( test,token,"+"))
	{	
		wb_assert(strcmp(token, "+") == 0, "StringMatch Malfunction")
		return ADD;
	}
	else if (StringMatch( test,token,"-"))
	{
		wb_assert(strcmp(token, "-") == 0, "StringMatch Malfunction")
		return SUB;
	}
	else if (StringMatch( test,token,"*"))
	{
		wb_assert(strcmp(token, "*") == 0, "StringMatch Malfunction")
		return MULT;
	}
	else if (StringMatch( test,token,"/"))
	{
		wb_assert(strcmp(token, "/") == 0, "StringMatch Malfunction")
		return DIV;
	}
	else
	{
		return NONE;
	}
}
