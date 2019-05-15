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
      printf("> usage: %s <filename> [FLAGs: --TRACE --SHOW_TREE]\n", argv[0]);
      return -1;
    }

    char* file_name = argv[1];

    bool trace = false;
    bool show_tree = false;

    if (argc > 2) {
      for (int i = 2; i<argc; i++) {
        if (strcmp(argv[i], "--TRACE") == 0) {
          trace = true;
        }
        if (strcmp(argv[i], "--SHOW_TREE") == 0) {
          show_tree = true;
        }
      }
    }

    runCompiler(file_name, trace, show_tree);

    return 0;
}
