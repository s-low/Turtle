
/* 
MINUNIT: I'm using minunit - mainly because it's so dead simple it's barely
a test harness. It just sort of gets out of the way.

I've modified it slightly to suit my purposes.
It now counts both tests "suites" and two types of asserts (white and black 
box). A suite in this case meaning a set of test cases on one function.

I've also added MESSAGE_RETURN() so that any asserts within test_case 
functions succesfully rise to the main test.
*/

// Black Box Assert: Return a message to the test function
#define bb_assert(message, condition) do { test->bb_asserts++; if (!(condition)) return message; } while (0)

// Run Test: Shorthand for running black box tests based on the above message returns
#define mu_run_test(name, args) do { char *message = name(args); test->tests_run++; \
                                if (message) return message; } while (0)

// My addition for nesting test_cases in test functions
#define MESSAGE_RETURN(PHRASE) {message=PHRASE; if (message) return message;}

// White Box Assert: No message return, just stdout and exit
#define wb_assert(condition, message) {test->wb_asserts++; if (!(condition)){ \
printf("WB_ASSERT FAIL: %s\n",message); exit(2);}}

