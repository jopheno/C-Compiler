//
//  main.c
//  CompiladorC-
//
//  Created by João Pedro Ballerini Bruno on 9/28/18.
//  Copyright © 2018 João Pedro Ballerini Bruno. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, const char * argv[]) {
	// insert code here...

	if (argc < 2) {
		printf("> usage: %s <filename> [FLAGs: --DEBUG --JA --BIOS -o <output_file> -i <initial_position>]\n", argv[0]);
		return -1;
	}

	char* file_name = argv[1];
	char* output_file_name = NULL;
	char* initial_value_str = NULL;

	int initial_value = 0;

	bool debug_mode = false;
	bool ja_mode = false;
	bool with_initial_value = false;
	bool bios_mode = false;

	if (argc > 2) {
		for (int i = 2; i<argc; i++) {
			if (strcmp(argv[i], "--DEBUG") == 0)
				debug_mode = true;

			if (strcmp(argv[i], "--JA") == 0)
				ja_mode = true;

			if (strcmp(argv[i], "-o") == 0) {
				if (argc > i+1) {
					output_file_name = (char*) malloc(22*sizeof(char));
					strcpy(output_file_name, argv[i+1]);
				} else {
					printf("> usage: %s <filename> [FLAGs: --DEBUG -o <output_file>]\n", argv[0]);
					return -1;
				}
			}

			if (strcmp(argv[i], "-i") == 0) {
				if (argc > i+1) {
					initial_value_str = (char*) malloc(22*sizeof(char));
					strcpy(initial_value_str, argv[i+1]);
					with_initial_value = true;
				} else {
					printf("> usage: %s <filename> [FLAGs: -i <inital_value>]\n", argv[0]);
					return -1;
				}
			}

			if (strcmp(argv[i], "--BIOS") == 0)
				bios_mode = true;
		}
	}

	if (with_initial_value)
		initial_value = atoi(initial_value_str);

	runCompiler(file_name, debug_mode, ja_mode, output_file_name, initial_value, bios_mode);

	return 0;
}
