
all: 
	gcc turtle.c neillsdl2.c grammar.c housekeeping.c stack.c -o turtle -Wall -Werror -std=c99 -pedantic -lSDL2

turtle:
	gcc turtle.c neillsdl2.c grammar.c housekeeping.c stack.c -o turtle -Wall -Werror -std=c99 -pedantic -lSDL2

gen:
	gcc generate.c neillsdl2.c grammar.c housekeeping.c stack.c -o generate -Wall -Werror -std=c99 -pedantic -lSDL2

testgen:
	gcc test_generate.c neillsdl2.c grammar.c housekeeping.c stack.c -o testgen -Wall -Werror -std=c99 -pedantic -lSDL2

cleantestgen:
	rm -f testgen

cleangen:
	rm -f generate 

clean:
	rm -f turtle