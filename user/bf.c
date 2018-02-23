/*
   Dagan's Brainfuck Interpreter
   Copyright (C) 2018 Dagan Martinez
   XV6 version

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
   */
#include "types.h"
#include "fcntl.h"
#include "user.h"
#include "stat.h"

/* Define __option_var__ as pointer to the variable declared as
   the option variable */
#define SET_OPTIONS_VARIABLE(a) int* __option_var__=&a;

/* return 'n' from options if exists */
#define HAS_OPTION(n) ((int)((*__option_var__)&n))

/* Add 'n' to options */
#define ADD_OPTION(n) (*__option_var__|=n);


/* List of possible options 
   Values are by powers of 2 */
#define OPT_NONE (0)		/* Empty option */
#define OPT_HELP (1<<1)		/* Display help message (do not interpret) */
#define OPT_VER (1<<2)		/* Show version number */
#define OPT_DEBUG (1<<3)	/* Allow debug commands */
#define OPT_STRING (1<<4)	/* Lets argument string run as code instead of file */
#define OPT_TIMER (1<<5)	/* Displays the run time of a program */
#define OPT_OPTIMIZE (1<<6)	/* Run optimizer */

/* Get option from option code */
#define GET_OPTION(n) (\
		n=='h'?OPT_HELP:\
		n=='v'?OPT_VER:\
		n=='d'?OPT_DEBUG:\
		n=='i'?OPT_STRING:\
		n=='t'?OPT_TIMER:\
		n=='o'?OPT_OPTIMIZE:\
		OPT_NONE)

/* Optimize code for faster interpretation */
void optimize(char *code, int options);
void run(const char *code, int options);


#define DBFI_VERSION "Dagan's Brainfuck Interpreter 1.2.2-xv6"
#define DBFI_USAGE "Usage: bf [options] [filename | -i cmd]\n"

int main(int argc, char *argv[])
{
	/* File Variables */
	char *filename = "";
	int filename_set = 0;

	/* Prepare options variables */
	int options = 0;
	SET_OPTIONS_VARIABLE(options);

	/* Parse arguments */
	if (argc <= 1) {
		/* No arguments */
		fprintf(stderr, "bf: no arguments\n" DBFI_USAGE);
		exit();
	} else {
		int i, j;

		/* Look through arguments */
		for (i = 1; i < argc; i++) {
			if ((argv[i])[0] == '-') {
				/* Check for valid options */
				for (j = 1; argv[i][j] != '\0'; j++) {
					int new_option =
						GET_OPTION(argv[i][j]);
					if (new_option == OPT_NONE) {
						/* Check if "--h[elp]" or "--v[ersion]" */
						if (argv[i][j] == '-'
								&& (argv[i][j + 1] ==
									'h'
									|| argv[i][j +
									1] ==
									'v')) {
							ADD_OPTION
								(GET_OPTION
								 (argv[i]
								  [j + 1]));
							break;
						} else {
							/* Option is invalid, abort */
							fprintf(stderr,
									"bf: no option `%c`\n",
									argv[i]
									[j]);
							exit();
						}
					}

					ADD_OPTION(new_option);
				}
			}

			else {
				/* Set file name */
				filename =
					malloc(sizeof(char) *
							(1 + strlen(argv[i])));
				for (j = 0; (argv[i])[j] != '\0'; j++)
					filename[j] = (argv[i])[j];
				filename[j] = '\0';
				filename_set = 1;
				break;
			}
		}
	}
	if (HAS_OPTION(OPT_HELP)) {
		fprintf(stdout, DBFI_USAGE "\n"
				"Options:\n"
				"\t-d\tAllow debugging commands\n"
				"\t-h\tPrint help message\n"
				"\t-i cmd\tPass string as code\n"
				"\t-o\tOptimize before running\n"
				"\t-t\tDisplay run time of program\n"
				"\t-v\tDisplay version number\n");
	} else if (HAS_OPTION(OPT_VER)) {
		/* Print out version number */
		fprintf(stdout, DBFI_VERSION "\n");
	} else if (HAS_OPTION(OPT_STRING)) {
		/* Interpret from argument */
		if (filename_set) {
			if (HAS_OPTION(OPT_OPTIMIZE)) {
				optimize(filename, options);
			}
			run(filename, options);
		} else {
			fprintf(stderr, "bf: no command\n" DBFI_USAGE);
			exit();
		}
	} else if (filename_set) {
		/* Open file for reading */
		int fp;
		int fp_size;
		char *fp_contents;
		fp = open(filename, O_RDONLY);
		/* Check for errors */
		if (fp == NULL) {
			fprintf(stderr,
					"bf: file \"%s\" not found\n", filename);
			free(filename);
			exit();
		}

		/* Get size */
		struct stat fp_stat;
		if(0>fstat(fp, &fp_stat))
			fprintf(stderr, "Something wrong with fstat\n");
		fp_size = fp_stat.size;

		/* Read file */
		fp_contents = malloc((sizeof(char) + 1) * fp_size);
		fp_contents[fp_size] = '\0';	/* Terminate with null */
		read(fp, fp_contents, fp_size);
		close(fp);
		/* Optimize */
		if (HAS_OPTION(OPT_OPTIMIZE)) {
			optimize(fp_contents, options);
		}
		/* Interpret source code */
		run(fp_contents, options);
		/* Free everything */
		free(fp_contents);
	} else {
		fprintf(stderr, "bf: no input files\n" DBFI_USAGE);
		exit();
	}

	/* Finish */
	if (filename_set)
		free(filename);
	exit();
}

#define OPPOSITE(n) (n=='+'?'-':n=='-'?'+':n=='<'?'>':n=='>'?'<':0)
#define VALID_CHAR(n) (n=='+'||n=='-'||n=='<'||n=='>'||n=='['||n==']'\
		|| n==',' || n=='.' || (HAS_OPTION(OPT_DEBUG) && (n=='~')))
void optimize(char *code, int options)
{
	unsigned int i, j;
	/* We're calling strlen here because it is brought up more than once in the code below */
	unsigned int len = strlen(code);

	/* We're only setting options to know if we need to optimize debug symbols */
	SET_OPTIONS_VARIABLE(options);

	/* Return if code is empty string */
	if (len == 0)
		return;

	/* Loop through code and add the OK characters */
	for (i = 0, j = 0; i < len; i++) {
		/* Add pairless valid characters */
		if (i < (len - 2) && code[i] == OPPOSITE(code[i + 1])) {
			/* Skip next character */
			i += 1;
		} else if (VALID_CHAR(code[i])) {
			/* Add character */
			code[j] = code[i];
			j++;
		}
	}

	/* Add null terminator */
	code[j] = '\0';

	/* Recurse */
	if (len != j) {
		optimize(code, options);
	}
}
/* Brainfuck cell */
#define CELL unsigned char

/* ctype codes */
#define BF_SHIFT '>'
#define BF_ADD '+'
#define BF_SKIP 's'
#define BF_GOTO 'g'
#define BF_PUT '.'
#define BF_GET ','
#define BF_DEBUG '~'
#define BF_END 'E'
struct Token {
	char ctype;		//command type
	int value;
};
/* Convert code into list of commands 
 * The idea here is that this is faster to run
 * than interpreting the ['s and ]'s as you go along*/
static void tokenize(struct Token *commands, const char *code)
{
	int count = 0;		/* How many command objects have we put in the 'commands' list */
	int skip_max = 10;	/* Size of skip queque */
	int *skip_queue = calloc(skip_max, sizeof(int));
	int skips = 0;		/* Where we are in the skip queue */
	for (int i = 0; code[i] != '\0'; i++) {
		if (code[i] == '[') {
			/* Record where a '[' would go to */
			commands[count].ctype = BF_SKIP;
			skip_queue[skips] = count;
			count++;
			skips++;
			/*Reallocate skip_queue */
			if (skips >= skip_max) {
				int *buffer =
					calloc(skip_max, sizeof(int));
				for (int j = 0; j < skip_max; j++) {
					buffer[j] = skip_queue[j];
				}
				free(skip_queue);
				skip_queue =
					calloc(skip_max + 1, sizeof(int));
				for (int j = 0; j < skip_max; j++) {
					skip_queue[j] = buffer[j];
				}
				free(buffer);
				skip_max++;
			}
		} else if (code[i] == ']') {
			/* Record where a ']'would go back to */
			skips--;
			commands[count].ctype = BF_GOTO;
			commands[count].value = skip_queue[skips] - 1;
			commands[skip_queue[skips]].value = count;
			count++;
		} else if (code[i] == '<' || code[i] == '>') {
			/* Add up all <'s and >'s */
			commands[count].ctype = BF_SHIFT;
			commands[count].value = code[i] == '<' ? -1 : 1;
			while (code[i + 1] == '<' || code[i + 1] == '>') {
				commands[count].value +=
					code[i + 1] == '<' ? -1 : 1;
				i++;
			}
			/* Only add code if it's not dead */
			if (commands[count].value != 0) {
				count++;
			}
		} else if (code[i] == '+' || code[i] == '-') {
			commands[count].ctype = BF_ADD;
			commands[count].value = code[i] == '-' ? -1 : 1;
			while (code[i + 1] == '-' || code[i + 1] == '+') {
				commands[count].value +=
					code[i + 1] == '-' ? -1 : 1;
				i++;
			}
			/* Only add code if it's not dead */
			if (commands[count].value != 0) {
				count++;
			}
		} else if (code[i] == '.') {
			/* Note: Value doesn't do anything. Consecutive '.''s'
			 *                      are just consecutive commands. I don't think this has a performance hit */
			commands[count].ctype = BF_PUT;
			count++;
		} else if (code[i] == ',') {
			/* See above */
			commands[count].ctype = BF_GET;
			count++;
		} else if (code[i] == '~') {
			commands[count].ctype = BF_DEBUG;
			count++;
		}

	}
	/* This object tells interpreter to stop running */
	commands[count].ctype = BF_END;
	free(skip_queue);
}

/* Interpret commands */
void run(const char *code, int options)
{
	/* Create list of commands from code */
	struct Token *commands;
	commands = malloc(sizeof(struct Token) * (strlen(code) + 2));
	tokenize(commands, code);

	int tape_size = 1;
	CELL *tape = calloc(tape_size, sizeof(CELL));
	int pointer = 0;

	/* Set up options */
	SET_OPTIONS_VARIABLE(options);

	/* Go through command objects */
	for (int i = 0; commands[i].ctype != BF_END; i++) {
		switch (commands[i].ctype) {
			case BF_SHIFT:
				pointer += commands[i].value;
				/* Check for pointer errors */
				if (pointer < 0) {
					fprintf(stderr,
							"runtime error: negative pointer\n");
					exit();
				}

				/* Reallocate tape */
				else if (pointer >= tape_size) {
					CELL *buffer =
						calloc(tape_size +
								(1 + pointer - tape_size),
								sizeof(CELL));
					for (int j = 0; j < tape_size; j++) {
						buffer[j] = tape[j];
					}
					free(tape);
					tape =
						calloc(tape_size +
								(1 + pointer - tape_size),
								sizeof(CELL));
					for (int j = 0; j < tape_size; j++) {
						tape[j] = buffer[j];
					}
					tape_size += 1 + pointer - tape_size;
					free(buffer);
				}
				break;
			case BF_ADD:
				tape[pointer] += commands[i].value;
				break;
			case BF_PUT:
				putchar(tape[pointer]);
				break;
			case BF_GET:
				tape[pointer] = getchar();
				break;
			case BF_SKIP:
				if (tape[pointer] == 0) {
					i = commands[i].value;
				} else if (commands[i + 1].ctype == BF_ADD
						&& commands[i + 2].ctype == BF_GOTO) {
					tape[pointer] = 0;
					i = commands[i].value;
				}
				break;
			case BF_GOTO:
				i = commands[i].value;
				break;
			case BF_DEBUG:
				if (HAS_OPTION(OPT_DEBUG)) {
					fprintf(stdout, "{%d=>%d}", pointer, tape[pointer]);
				}
		}
	}

	free(commands);
	free(tape);
}
