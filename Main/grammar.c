#include "turtle.h"

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
*/

/* Check the program begins with an opening brace */
void Prog(Program *p, Turtle *t){

	if(!StringMatch(p->wds[p->counter], "{")){
		ERROR("No opening brace");
		exit(2);
	}

	(p->counter)++; // Move to next instruction
	InstructionList(p, t);
}


/* Check code consists of either closing brace or instruction */
void InstructionList(Program *p, Turtle *t){

	// End of program: base case
	if( StringMatch( p->wds[p->counter], "}") ){
		return;
	}

	// Otherwise consider the instruction:
	Instruction(p, t);  

	(p->counter)++;	    	  // Next instruction
	InstructionList(p, t);	  // Check for finish, else handle
}


/* Expecting an instruction */
void Instruction(Program *p, Turtle *t){

	char instrct[MAXTOKENSIZE]; 
	strcpy(instrct, p->wds[p->counter]);

	(p->counter)++; // Next instruction

	if (StringMatch(instrct, "FD"))
	{	
		Varnum(p, t, FD);
		return;
	}
	else if(StringMatch(instrct, "LT"))
	{
		Varnum(p, t, LT);
		return;
	}
	else if (StringMatch(instrct, "RT"))
	{
		Varnum(p, t, RT);
		return;
	}
	else if (StringMatch(instrct, "DO"))
	{	
		Do(p, t);
		return;
	}
	else if (StringMatch(instrct, "SET"))
	{
		Set(p, t);
		return;
	}

	ERROR("Expecting <instruction> or closing brace")
	exit(1);
}


/* Checks that we get a valid <varnum> next, and returns it's value */
void Varnum(Program *p, Turtle *t, Inst instruct){

	float conversion = atoi(p->wds[p->counter]);
	float value;

	if ( conversion != 0 ){			// If is number, take direct conversion
		value = conversion;
	}
	else							// If not a number, must be a var
	{									
		Var(p, t);
		value = p->var[p->index];
		
		if ((value == value) == 0)  // Check that the variable has been set
		{	
			ERROR("Variable is NaN. It has not been set yet")
		}
	}

	switch(instruct){
		case FD:
			t->distance = value;
			DrawLine(t);
		break;

		case LT:
			value = (value / DEGREES) * 2 * PI; // Radians
			t->bearing -= value;
		break;

		case RT:
			value = (value / DEGREES) * 2 * PI; // Radians
			t->bearing += value;
		break;

		case FROM:
			p->from = (int) value;
		break;

		case TO:
			p->to = (int) value;
		break;

		default:
		break;
	}
	
	return;
}


/* We are expecting a <var>, parses and "selects" the relevant variable */
void Var(Program *p, Turtle *t){

	if (strlen(p->wds[p->counter]) != 1)
	{
		ERROR("Expected <var> but is multi-character")
	}

	char c = p->wds[p->counter][0];

	if (c > 'Z' || c < 'A')
	{
		ERROR("Expected <var> or <varnum>, character is neither [A-Z] nor ")
	}
	else
	{
		p->index = (int) c % ANSI_A; // Set the index of the array of vars
	}

	return;
}


/* <Do> is a more complicated token, with a longer function to match*/
void Do(Program *p, Turtle *t){	

	Do_info *do_loop = (Do_info *)malloc(sizeof(Do_info));
	// One struct to hold all the loop info (variable, from, to, start counter)
	// Using a local variable prevents a nested DO call overwriting the previous
	// Manual allocation from heap to prevent overflow if a LOT of DO calls

	Var(p, t);								// Expect <var>
	do_loop->var = p->index; 				// Set do variable to it

	(p->counter)++;
	if ( !StringMatch(CURRENT,"FROM") )    	// Then expect "FROM"
	{
		ERROR("Expected \"FROM\" in <DO>")
	}
	
	(p->counter)++;
	Varnum(p, t, FROM);						// Then a <varnum>
	do_loop->from = p->from;				// Set FROM 'x' value to it

	(p->counter)++;
	if ( !StringMatch(CURRENT, "TO") )      // Then expect "TO"
	{
		ERROR("Expected \"TO\" in <DO>")
	}
	
	(p->counter)++;
	Varnum(p, t, TO);						// Then <varnum> again
	do_loop->to = p->to;					// Set TO 'y' value to it

	if (p->to <= p->from) {
		ERROR("Loop parameters senseless. Should be from a to b where a < b")
	}

	(p->counter)++;
	do_loop->start = p->counter;			// Where to branch/loop back to

	// Repeatedly handle the complete program within the DO loop:
	for ( p->var[do_loop->var] = do_loop->from; 
			p->var[do_loop->var] <= do_loop->to; 
				p->var[do_loop->var]++ )
	{	
		p->counter = do_loop->start;	// Reset P-counter to the loop start
		Prog(p, t);	 
		p->index = do_loop->var;		// Reset P-index to the loop variable
	}

	free(do_loop);
	return;
}

/* <SET> */
void Set(Program *p, Turtle *t){
	
	Var(p, t); 								// Expect <var> after "SET"
	p->set_target = p->index;
	(p->counter)++;

	if ( !StringMatch(CURRENT, ":=") )  	// Then ":="
	{
		ERROR("Expected \":=\" in <SET>")
	}

	(p->counter)++;
	Polish(p, t);							// Then some Polish
	
	return;
}



/* Postfix Reverse Polish Notation */
void Polish(Program *p, Turtle *t){

	Stack s;
	InitialiseStack(&s);
	
	// Only ever need THREE variables (A, B, C) to deal with any polish expr.
	float num, a = 0, b = 0, c = 0, result;
	operator operator;

	// Until we find a semicolon
	while( !StringMatch(CURRENT, ";") ){
			
		if( (num = atoi(CURRENT)) != 0) // If is a (non-zero) value 				
		{ 				
			Push(&s, &num); // Push for numbers
		} 

		else if ( (operator = isOperator(CURRENT)) != NONE )
		{	
			a = Pop(&s);	// Pop for operators
			b = Pop(&s);

			switch(operator){
				
				case ADD:
					c = a + b;
				break;

				case SUB:
					c = a - b;
				break;

				case MULT:
					c = a * b;
				break;

				case DIV:
					c = b / a;
				break;

				case NONE:
					ERROR("Missing a semicolon in Polish")
				break;

				default:
				break;
			}

			Push(&s, &c);
		}

		else if ( isOperator(CURRENT) == NONE)
		{	
			Var(p,t);				// Expecting a var if not an operator
			num = p->var[p->index];
			
			if (!(num == num)) // ergo: is NaN
			{
				ERROR("Variable has not been set yet, cannot use in Polish")
			}

			Push(&s, &num);
		}

		else
		{
			ERROR("Incorrect Polish. Possibly missing a space")
		}

		(p->counter)++;	
	}
	
	if (s.top != 1) // top will be 1 if correct syntax
	{
		ERROR("Incorrect Polish syntax")
	}

	result = Pop(&s);

	p->var[p->set_target] = result; // Set the target var equal to result

	return;
}


/* Check if token is any of [+ * / -] */
operator isOperator(char token[MAXTOKENSIZE]){

	if (StringMatch(token,"+"))
	{
		return ADD;
	}
	else if (StringMatch(token,"-"))
	{
		return SUB;
	}
	else if (StringMatch(token,"*"))
	{
		return MULT;
	}
	else if (StringMatch(token,"/"))
	{
		return DIV;
	}
	else
	{
		return NONE;
	}
}
