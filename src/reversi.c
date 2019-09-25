#include <getopt.h> /*for use getopt_long*/
#include "reversi.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *optString = ":s:b:w:cvVh?";/*valid options*/

static const struct option longOpts[] = {
    { "size", required_argument, NULL, 's' },
    { "black-ai", required_argument, NULL, 'b' },
    { "white-ai", required_argument, NULL, 'w' },
    { "contest", no_argument, NULL, 'c' },
    { "verbose", no_argument, NULL, 'v' },
    { "version", no_argument, NULL, 'V' },
    { "help", no_argument, NULL, 'h' },/*help option*/
    {NULL, 0, NULL, 0}
};

/**/
void print_usage(){
    printf("Usage: reversi [-s SIZE|-b [N]|-w |-c|-v|-V|-h] [FILE]\n");
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


int main (int argc, char* argv[]){

    /* global variables */
    //opterr = 0; /* (variable in getopt.h) set to 0 for ignore error when a invalid option is given */
    int sflag=0;
    int bflag=0;


    bool verbose = false; /* verbose variable */
    bool contest_mod = false;
    int value_opt; /* value of option */
    int size_t=4;
    int board_size=size_t*2;
    int tactic_b_player=0;/* 0 if humain plays, 1 if it is random */
    int tactic_w_player=0;/* 0 if humain plays, 1 if it is random */
    int longIndex;
    int optc;
    while ((optc = getopt_long( argc, argv, optString, longOpts, &longIndex )) != -1) {
        switch (optc) {
            case 's': /* 'size' option */
                sflag=1;
                value_opt = atoi(optarg);
                if (value_opt<1 || value_opt>5) {
                    printf("The argument of -s option must be between 1 and 5\n");
                    exit(EXIT_FAILURE);
                }
                size_t = value_opt;
                board_size=value_opt*2;
                break;
            case 'b': /* 'black-ai' option */
                bflag=1;
                value_opt = atoi(&optarg[0]);
                printf("%d\n", value_opt);
                if (value_opt!= 1 && value_opt!=0) {
                    printf("the argument of -b option must be 0 or 1\n");
                    exit(EXIT_FAILURE);
                }
                tactic_b_player=value_opt;
            case 'w': /* 'white-ai' option */
                value_opt = atoi(optarg);
                if (value_opt!= 1 && value_opt!=0) {
                    printf("The argument of -w option must be 0 or 1\n");
                    exit(EXIT_FAILURE);
                }
                tactic_w_player=value_opt;
            case 'c': /* 'contest' mode */
                contest_mod = true;
                break;
            case 'v': /* 'verbose' option */
                verbose = true;
                printf("%d\n", verbose);
                break;
            case 'V': /*'version' option*/
                printf("reversi %d.%d.%d\n",VERSION,SUBVERSION,REVISION);
                printf("This software allows to play to reversi game.\n");
                exit(EXIT_SUCCESS);
                break;
            case 'h': /*'help' option*/
                print_usage();
                exit(EXIT_SUCCESS);
                break;
            case ':': /* if value of argument is not given */
                printf("-%c commande must have a value\n",optopt);
                break;
            case '?': /* if an unknown option is given */
                opterr = 0;
                printf("Try 'reversi --help' for more information\n");
                break;
            default:
                printf("Try 'reversi --help' for more information\n");
                exit(EXIT_FAILURE);
                break;
        }
    }


    exit(EXIT_SUCCESS);
}
