#include "test_turtle.h"

/* Stack handling functions for polish expressions */

/* Self-explanatory */
void InitialiseStack(Testing *test, Stack *s)
{ 
	s->top = 0; 
  wb_assert(s->top == 0, "Stack top not set");
}


/* Push an int onto the stack */
void Push(Testing *test, Stack *s, float *num, Program *p)
{   
    int test_store = s->top;

    //assert(s->top < STACKSIZE); // Check that we have not overflown the stack
	  if ( (s->top < STACKSIZE) == 0 )
    {
      wb_assert(s->top >= STACKSIZE, "Stack overflow mistakenly caught")
   		p->test_flag = 1; // "TEST ERROR"
   		return;
    }   

    s->array[s->top] = *num; 	// push the value down
    s->top = s->top + 1; 		// increase the 'top' index
    wb_assert(s->top == test_store + 1, "Stack top not incremented")
}


/* Pop an int from the stack */
float Pop(Testing *test, Stack *s, Program *p){
	
  int test_store = s->top;

	//assert(s->top > 0); 	 	// Check that we have not underflown the stack
  	if ( !(s->top > 0) )
    {
      wb_assert(s->top <= 0, "Stack underflow mistakenly caught")
   		p->test_flag = 1; 		// "TEST ERROR"
   		return 0;
    }   
  	
  	s->top = s->top - 1;	 	// move the top index down
    wb_assert(s->top == test_store - 1, "Top index not decreased in Pop")

   	return s->array[s->top]; 	// return the value
}
