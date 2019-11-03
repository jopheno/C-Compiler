CC = gcc

## Compiler flags:

# GCC:  (also -march=pentium etc, for machine-dependent optimizing)
CFLAGS=
LDFLAGS=-ggdb -O0 -w #-Wall
SFLAGS=
LIBS=
EXECLIBS=-ll -ly

OBJFILES = build/compiler.o build/symtab.o build/analyze.o build/cgen.o build/code.o build/util.o build/assembly.o build/lex.yy.o build/bison.tab.o build/main.o

EXEC = exec

# ---------------------------------------------------------------------
# Compilation section
# ---------------------------------------------------------------------

all: clean flex bison $(EXEC)

exec: $(OBJFILES)
	@ echo '>> Building binary using GCC linker: $@'
	$(CC) -o $@ $^ $(CFLAGS) $(EXECLIBS);
	@ echo 'Finished building binary: $@'
	@ echo ' '

build/%.o:	%.c
	@ echo '>> Building target using GCC compiler: $<'
	$(CC) $(LDFLAGS) -o $@ -c $< $(CFLAGS) $(LIBS)
	@ echo ' '

build/bison.tab.o:	bison.tab.c
	@ echo '>> X Building target using GCC compiler: $<'
	$(CC) $(LDFLAGS) -o $@ -c $< $(CFLAGS) $(LIBS)
	@ echo ' '

#lex.yy.c: flex.l
#	flex flex.l

flex:
	@ echo '>> Generating flex file: $<'
	flex flex.l

bison:
	@ echo '>> Generating bison file: $<'
	bison -d -v -g bison.y

clean:
	@ echo '>> Removing previous build files:'
	rm -f $(EXEC) *.ci test_cases/*.ci lex.yy.c LUA/example.ja LUA/output.mif build/*.o bison.tab.c bison.tab.h bison.output bison.dot *.ci bison.vcg
	@ echo ' '

cle:
	@ echo '>> Removing previous build files:'
	rm -f *.ci test_cases/*.ci lex.yy.c build/*.o bison.tab.c bison.tab.h bison.output bison.dot *.ci bison.vcg
	@ echo ' '
