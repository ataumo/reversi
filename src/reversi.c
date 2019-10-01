#include <stdio.h>
#include <stdlib.h>

#include "reversi.h"
#include "board.h"

#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

void
red_color() { /* it used for errors */
	printf("\033[0;31m"); /* red color text */
}

void
print_usage() {
	printf("Usage: reversi [-s SIZE|-b [N]|-w [N]|-c|-v|-V|-h] [FILE]\n");
	printf("Play a reversi game with human or program players.\n");
	printf("  -s, --size SIZE       board size (min=1, max=5 (default=4))\n");
	printf("  -b, --black-ai [N]    set tactic of black player (default: 0)\n");
	printf("  -w, --white-ai [N]    set tactic of black player (default: 0)\n");
	printf("  -c, --contest         enable 'contest' mode\n");
	printf("  -V, --version         display version and exit\n");
	printf("  -h, --help            display this help and exit\n");
	printf("\n");
	printf("Tactic list: human (0), random (1)\n");
}

int
main (int argc, char *argv[]) {

	static const struct option longopts[] = {
		{"size",     required_argument, NULL, 's'},
		{"black-ai", optional_argument, NULL, 'b'},
		{"white-ai", optional_argument, NULL, 'w'},
		{"contest",  no_argument, 			NULL, 'c'},
		{"verbose",  no_argument, 			NULL, 'v'},
		{"version",  no_argument, 			NULL, 'V'},
		{"help", 		 no_argument, 			NULL, 'h'},
		{NULL, 			 0, 								NULL,  0}
	};

	/* boolean variables */
	bool contest_mode = false;  /* true if -c option is given */
	bool file_argument = false; /* true if file argument is given */
	bool tactic_b_player = 0;   /* 0 if humain plays, 1 if it is random */
	bool tactic_w_player = 0;   /* 0 if humain plays, 1 if it is random */
	bool verbose = false;       /* verbose variable */

	/* integer variables */
	int cpt_of_file = 0; /* number of file argument */
	int int_optarg;      /* argument of option */
	int optc;						 /* current value of option */

	/* other type */
	size_t board_size_num = 4;
	FILE *file = NULL;
	char *file_name;
	static const char *opts = ":s:b::w::cvVh"; /*valid options*/

	while ((optc = getopt_long(argc, argv, opts, longopts, NULL)) != -1) {
		switch (optc) {

			case 's': /* 'size' option */
				if ((int_optarg < 1 || int_optarg > 5) && strlen(optarg)!=1) {
					red_color();
					printf("Error: the argument of -s option should be between 1 and 5\n");
					exit(EXIT_FAILURE);
				}
				board_size_num = int_optarg * 2;
			break;

			case 'b': /* 'black-ai' option */
				if (optarg != NULL) { /* if argument is given */
					int_optarg = atoi(optarg);
					if ((int_optarg != 1 && int_optarg != 0) && strlen(optarg)!=1) {
						red_color();
						printf("Error: the argument of -b option should be 0 or 1\n");
						exit(EXIT_FAILURE);
					}
					tactic_b_player = int_optarg;
				}
			break;

			case 'w': /* 'white-ai' option */
				if (optarg != NULL) { /* if argument is given */
					int_optarg = atoi(optarg);
					if ((int_optarg != 1 && int_optarg != 0) && strlen(optarg)!=1) {
						red_color();
						printf("Error: the argument of -w option should be 0 or 1\n");
						exit(EXIT_FAILURE);
					}
					tactic_w_player = int_optarg;
				}
			break;

			case 'c': /* 'contest mode' option */
				contest_mode = true;
			break;

			case 'v': /* 'verbose' option */
				verbose = true;
			break;

			case 'V': /* 'version' option */
				printf("\033[0;32m"); /* green color text */
				printf("reversi %d.%d.%d\n", VERSION, SUBVERSION, REVISION);
				printf("\033[0;0m"); /* end green color */
				printf("This software allows to play to reversi game.\n");
				exit(EXIT_SUCCESS);
			break;

			case 'h': /* 'help' option */
				print_usage();
				exit(EXIT_SUCCESS);
			break;

			case ':': /* if argument is not given */
				red_color();
				printf("-%c commande must have an argument\n", optopt);
			break;

			default: /* if an unknown option is given */
				red_color();
				printf("Illegal option: Try 'reversi --help' for more information\n");
				exit(EXIT_FAILURE);
			break;
		}
	}

	/* get the argument of the program */
	for (; optind < argc; optind++) {
		file_name = argv[optind];
		file_argument = true;
		cpt_of_file++;
	}
	if (cpt_of_file > 1) { /* more than 1 file is given */
		red_color();
		printf("Error: You must to give 1 file for the contest mode\n");
	} else {
		if (file_argument) {  /* file argument is given */
			if (contest_mode) { /* contest mode is enable */
				file = fopen(file_name, "r+"); /* Reading file.. */
				if (file != NULL) {
					printf("Readable file\n");
				} else { /* The given argument is not a readable file */
					red_color();
					printf("Error opening the file: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
				fclose(file);
			} else {
				red_color();
				printf("Error: a file is given but the contest mode is not"
					" activated with option -c or --contest\n");
				exit(EXIT_FAILURE);
			}
		} else {
			if (contest_mode) { /* constes mode is enable */
				red_color();
				printf("Error: contest mode is activated but no file is given\n");
				exit(EXIT_FAILURE);
			}
		}
	}

	board_t *board=board_alloc(10,BLACK_DISC);
	size_t size = board_size(board);
	board = board_init(size);
	file = fopen("file.txt", "r+"); /* Reading file.. */
	board_print(board,file);

	exit(EXIT_SUCCESS);
}
