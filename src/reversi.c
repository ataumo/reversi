#include <getopt.h> /*for use getopt_long*/
#include "reversi.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *optString = "s:b:w:cvVh";/*valid options*/

static const struct option longOpts[] = {
    { "size", no_argument, NULL, 's' },
    { "black-ai", no_argument, NULL, 'b' },
    { "white-ai", no_argument, NULL, 'w' },
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
    printf("  -V, --version         display version and exit\n");
    printf("  -h, --help            display this help and exit\n");
    printf("\n");
    printf("Tactic list: human (0), random (1)\n");
}


int main (int argc, char* argv[]){

    /* global variables */
    bool verbose = false; /* verbose variable */
    int value_of_option;
    int size_t=4;
    int board_size=size_t*2;
    int tactic_b_player=0;/* 0 if humain plays, 1 if it is random */
    int tactic_w_player=0;/* 0 if humain plays, 1 if it is random */
    int longIndex;
    int optc = getopt_long( argc, argv, optString, longOpts, &longIndex );
    while (optc != -1) {
        switch (optc) {
            case 's': /* 'size' option */
                value_of_option = atoi(argv[2]);
                if (value_of_option<1 || value_of_option>5) {
                    printf("This value must be between 1 and 5\n");
                    exit(EXIT_FAILURE);
                }
                size_t = value_of_option;
                board_size=value_of_option*2;
                break;
            case 'b': /* 'black-ai' option */
                value_of_option = atoi(argv[2]);
                if (value_of_option!= 1 && value_of_option!=0) {
                    printf("This value must be 0 or 1\n");
                    exit(EXIT_FAILURE);
                }
                tactic_b_player=value_of_option;
            case 'w': /* 'white-ai' option */
                value_of_option = atoi(argv[2]);
                if (value_of_option!= 1 && value_of_option!=0) {
                    printf("This value must be 0 or 1\n");
                    exit(EXIT_FAILURE);
                }
                tactic_w_player=value_of_option;
            case 'v': /* 'verbose' option */
                verbose = true;
                break;
            case 'V': /*'version' option*/
                printf("reversi %d.%d.%d\n",VERSION,SUBVERSION,REVISION);
                printf("This software allows to play to reversi game.\n");
                break;
            case 'h': /*'help' option*/
                print_usage();
                break;
            default:
                exit(EXIT_FAILURE);
        }
        optc=-1;
    }

    exit(EXIT_SUCCESS);
}
