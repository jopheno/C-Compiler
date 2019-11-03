#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "globals.h"
#include "lex.h"
#include "util.h"
#include "assembly.h"

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

int runCompiler(char* source_file, bool debug_mode, bool ja_mode, char* output_file) {

	if (debug_mode) {
		TraceScan = TRUE;
	}

	// If forgot to put extension .cc, auto insert.
	if (strchr (source_file, '.') == NULL)
		strcat(source_file,".c");

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

	// This is used to send ERROR messages to STDERR
	listing = stderr;
	// This is used to redirect code and others debug info to STDOUT
	code = bison_output;

	//exit(-1);

	Token tok = getToken();

	bool lexical_errors = false;
	bool concat_file = false;

	while(tok.type != ENDFILE) {

		if (tok.type == ERROR) {
			fprintf(stderr, "> Lexical error found at line %d near '%s' classified as '%s'\n", lines_number, tok.lexical_unit, getTokenName(tok.type));
			lexical_errors = true;
		}

		if (tok.type == IMPORT) {
			concat_file = true;
		}

		if (tok.type == FILENAME) {
			if (concat_file == false) {
				fprintf(stderr, "> Lexical error found at line %d near '%s' classified as '%s'\n", lines_number, tok.lexical_unit, getTokenName(tok.type));
				lexical_errors = true;
			} else {
				if (debug_mode)
					printf("\n\t\t>> Importing '%s'\n", tok.lexical_unit);

				add_import(tok.lexical_unit, lines_number);
			}
		}

		tok = getToken();
	}

	if (generate_final_file(source_file) < 0) {
		fprintf(stdout, ">> [FATAL] Errors occured when importing files.\n");
		exit(1);
	}

	lines_number = 1;
	fprintf(stdout, "\n>> Initializing Bison ...\n");

	openFile(TEMP_FILE_NAME);

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
		printf("Unable to open %s\n", ja_file_name);
		exit(1);
	}

	if (ja_mode) {
		fclose(ja_file);
		ja_file = stdout;
	}

	init_assembly(ja_file);

	printf(">> Generating JA at ['%s']...\n", ja_file_name);

	generate_assembly(&LIST);

	fclose(ja_file);

	char* str = (char*) malloc(50*sizeof(char));
	sprintf(str, "cd LUA/ && echo '../%s\n' | lua compiler.lua auto ../output.mif\n", ja_file_name);

	system(str);

	printf("\n>>> DONE !\n\n");

	fclose(bison_output);

	fflush(stdout);

	return 1;
}