#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "globals.h"
#include "lex.h"
#include "util.h"
#include "assembly.h"
#include "bison.tab.h"

int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = FALSE;

FILE * source;
FILE * listing;
FILE * code;
FILE * ja_file;
FILE * bison_output;

int lines_number = 0;

extern Tlist LIST;


int runCompiler(char* source_file, bool debug_mode, char* output_file) {

	if (debug_mode) {
		TraceScan = TRUE;
	}

	// If forgot to put extension .cc, auto insert.
	if (strchr (source_file, '.') == NULL)
			strcat(source_file,".cc");

	source = fopen(source_file,"r");
	if (source==NULL)
	{
		fprintf(stderr,"File %s not found\n",source_file);
		exit(1);
	}

	if (debug_mode)
		bison_output = stdout;
	else {
		#ifdef _WIN32
		bison_output = fopen("NUL", "w");
		#elif __linux__
		bison_output = fopen("/dev/null", "w");
		#elif __APPLE__
		bison_output = fopen("/dev/null", "w");
		#endif
	}

	// This is used to show ERROR messages to STDERR
	listing = stderr;
	// This is used to redirect code and others debug info to STDOUT
	code = bison_output;

	//exit(-1);

	Token tok = getToken();

	bool lexical_errors = false;

	while(tok.type != ENDFILE) {

		if (tok.type == ERROR) {
			fprintf(stderr, "> Lexical error found at line %d near '%s' classified as '%s'\n", lines_number, tok.lexical_unit, getTokenName(tok.type));
			lexical_errors = true;
		}

		tok = getToken();
	}

	lines_number = 1;
	fprintf(stdout, ">> Initializing Bison ...\n");

	openFile(source_file);

	TreeNode * syntaxTree;

	syntaxTree = parse();

	if (Error) return 1;

	if (debug_mode)
		printTreeC(syntaxTree);

	printf(">> Building symbols table...\n");

	if (debug_mode)
		TraceAnalyze = TRUE;

	buildSymtab(syntaxTree);

	printf(">> Checking types...\n");

	typeCheck(syntaxTree);

	if (Error) return 1;

	codeGen(syntaxTree, bison_output);

	if (debug_mode)
		printSymTab(stdout);
	
	char* ja_file_name = "./LUA/example.ja";

	if (output_file != NULL)
		ja_file_name = output_file;

	ja_file = fopen(ja_file_name,"w");
	if (ja_file == NULL)
	{ 
		printf("Unable to open %s\n",ja_file_name);
		exit(1);
	}

	init_assembly(ja_file);

	printf(">> Generating JA at ['%s']...\n", ja_file_name);

	generate_assembly(&LIST);

	printf("\n>>> DONE !\n\n");

	fclose(bison_output);
	fclose(ja_file);

	return 1;
}