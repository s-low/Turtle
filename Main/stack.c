#include "turtle.h"

/* Stack handling functions for polish expressions */

/* Self-explanatory */
void InitialiseStack(Stack *s)
{ 
	s->top = 0; 
}


/* Push an int onto the stack */
void Push(Stack *s, float *num)
{  	
	// Overflow
    if ((s->top < STACKSIZE) == 0)
    {
    	ERROR("Your polish expression is WAY long")
    }
    s->array[s->top] = *num; 	// push the value down
    s->top = s->top + 1; 		// increase the 'top' index
}


/* Pop an int from the stack */
float Pop(Stack *s){

	// Overflow
    if ((s->top > 0) == 0)
    {
    	ERROR("You would underflow the stack. Sort out your polish")
    }
  	s->top = s->top - 1;	 	// move the top index down
   	return s->array[s->top]; 	// return the value
}
