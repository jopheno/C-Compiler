#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "globals.h"
#include "lex.h"
#include "util.h"
#include "assembly.h"

#define TEMP_FILE_SIZE 10000000
#define TEMP_FILE_NAME "_temp.c"

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

extern import_file_t* imports[MAX_FILE_IMPORTS];
extern int import_amount;
extern bool ImportErrors;

int generate_final_file(char* source_file) {

	if (ImportErrors) {
		return -1;
	}

	int curr_line = 1;

	FILE* source = fopen(source_file,"r");
	if (source==NULL)
	{
		fprintf(stderr,"File '%s' not found\n",source_file);
		return -1;
	}

	fseek(source, 0, SEEK_END);
	size_t eof = ftell(source);
	fseek(source, 0, SEEK_SET);

	FILE* dest = fopen(TEMP_FILE_NAME,"w");
	if (dest==NULL)
	{
		fprintf(stderr,"File '%s' could not be created\n",TEMP_FILE_NAME);
		return -1;
	}

	char aux = 0;
	size_t pos = 0;
	bool skip_line = false;
	while(pos < eof) {
		fread(&aux, sizeof(char), 1, source);

		if (!skip_line)
			fwrite(&aux, sizeof(char), 1, dest);

		pos++;

		if (aux == '\n') {
			skip_line = false;
			curr_line++;

			for (int i = 0; i<import_amount; i++) {
				if (imports[i]->line == curr_line) {
					FILE* temp = fopen(imports[i]->filename, "r");

					if (temp==NULL)
					{
						fprintf(stderr,"File '%s' could not be found\n", imports[i]->filename);
						return -1;
					}

					fseek(temp, 0, SEEK_END);
					size_t temp_size = ftell(temp);
					fseek(temp, 0, SEEK_SET);

					char* temp_content = (char*) malloc(sizeof(char)*temp_size);

					fread(temp_content, sizeof(char), temp_size, temp);
					fwrite(temp_content, sizeof(char), temp_size, dest);
					fflush(dest);
					free(temp_content);
					fclose(temp);

					skip_line = true;
				}
			}
		}
	}

	fflush(dest);
	fclose(dest);

	return 0;

}

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