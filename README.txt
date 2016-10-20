FPLI  :  Functional Programming Language Interpreter

NOTE: Designed to be built on unix systems

To build:
	Simply navigate into the src/ directory and run the
	make command, this will build the project and place
	a copy of the executable into its parent directory.
	Please note: flex and bison are required programs
	for this build, as they build the tokenizer and
	parser respectively. If your system does not have
	these programs, they are usually retrievable from
	your operating system's software management program.
	
To use:
	The executable fpli will take any input files as 
	command line arguments, these will all be run
	instantly. The program will then run a text 
	interpreter for the language. Please see
	LanguageDocumentation.txt for information
	about the language itself. The beginnings of a 
	standard library and some example functions are
	located in the fpl/ directory.