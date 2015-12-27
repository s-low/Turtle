
MAIN/ has the Parser/Interpreter and extension (generator & extension testing)

TESTING/ has the full testing of the parser and interpreter


MAIN/:

	Parser and Interpreter:

		make turtle
		./turtle {file.txt}

	Extension:

		make gen
		./generate {OPTIONAL: random}

	Test Extension:
	
		make testgen
		./testgen {OPTIONAL: random}


TESTING/:

	Full testing Parser/Interpreter:
		
		make test
		./test