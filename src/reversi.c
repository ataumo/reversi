#include <err.h>
#include <getopt.h> /*for use getopt_long*/
#include "reversi.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *optString = ":s:b::w::cvVh";/*valid options*/

static const struct option longOpts[] = {
    { "size", required_argument, NULL, 's' },
    { "black-ai", optional_argument, NULL, 'b' },
    { "white-ai", optional_argument, NULL, 'w' },
    { "contest", no_argument, NULL, 'c' },
    { "verbose", no_argument, NULL, 'v' },
    { "version", no_argument, NULL, 'V' },
    { "help", no_argument, NULL, 'h' },/*help option*/
    {NULL, 0, NULL, 0}
};

/**/
void red_color(){
    printf("\033[0;31m");/* red color text */
    printf("\n");
    printf("\033[0;0m");/* reset color to default */
}

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

    //opterr = 0; /* (variable in getopt.h) set to 0 for ignore error when a invalid option is given */

    /* boolean variables */
    bool verbose = false; /* verbose variable */
    bool contest_mode = false;
    bool tactic_b_player=0;/* 0 if humain plays, 1 if it is random */
    bool tactic_w_player=0;/* 0 if humain plays, 1 if it is random */
    bool file_argument=false;

    /* integer variables */
    int int_optarg; /* argument of option */
    int longIndex;
    int optc;
    int cpt_of_file=0;

    /* other type */
    size_t board_size=4;
    char *file_name;

    /* main while */
    while ((optc = getopt_long( argc, argv, optString, longOpts, &longIndex )) != -1) {
        switch (optc) {

            case 's': /* 'size' option */
                int_optarg = atoi(optarg);
                if (int_optarg<1 || int_optarg>5) {
                    printf("The argument of -s option must be between 1 and 5\n");
                }
                board_size=int_optarg*2;
                break;

            case 'b': /* 'black-ai' option */
                if (optarg != NULL) {
                    int_optarg = atoi(optarg);
                    if (int_optarg!= 1 && int_optarg!=0) {
                        printf("The argument of -b option must be 0 or 1\n");
                        exit(EXIT_FAILURE);
                    }
                    tactic_b_player=int_optarg;
                    printf("option %c with %d of argument\n", optc,int_optarg);
                }
                break;

            case 'w': /* 'white-ai' option */
                if (optarg != NULL) {
                    int_optarg = atoi(optarg);
                    if (int_optarg!= 1 && int_optarg!=0) {
                        printf("The argument of -w option must be 0 or 1\n");
                        exit(EXIT_FAILURE);
                    }
                    tactic_w_player=int_optarg;
                    printf("option %c with %d of argument\n", optc,int_optarg);
                }
                break;

            case 'c': /* 'contest' mode */
                contest_mode = true;
                break;

            case 'v': /* 'verbose' option */
                verbose = true;
                break;

            case 'V': /*'version' option*/
                printf("\033[0;32m");/* green color text */
                printf("reversi %d.%d.%d\n",VERSION,SUBVERSION,REVISION);
                printf("\033[0;0m");/* end green color */
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
            //case '?': /* if an unknown option is given */
                //opterr = 0;
                //printf("Try 'reversi --help' for more information\n");
                //break;
            default: /* if an unknown option is given */
                printf("Try 'reversi --help' for more information\n");
                exit(EXIT_FAILURE);
                break;
        }
    }
    for(; optind < argc; optind++){/* argument of the program */
        file_name=argv[optind];
        printf("extra arguments: %s\n", file_name);
        file_argument=true;
        cpt_of_file++;
    }
    if (cpt_of_file>1) {/* more than 1 file is given */
        red_color();
        warnx("you must to give 1 file for the contest mode");
    }else{
        if (file_argument) {/* file argument is given */
            if (contest_mode) {/* constes mode is enable */
                printf("contest mode activé.. on lit le fichier\n");
                FILE* fichier = NULL;
                fichier = fopen(file_name, "r+");
                if (fichier != NULL){
                   printf("on peut lire dans ce fichier\n");
                }
                else{
                   red_color();
                   errx(1,"The given argument \"%s\" is not a readable file",file_name);
                }
            }else{
                red_color();
                errx(1,"A file is given but the contest mode is not activated with option -c or --contest");
            }
        }else{
            if (contest_mode) {/* constes mode is enable */
                red_color();
                errx(1,"Contest mode is activated but no file is given");
            }
        }
    }



    exit(EXIT_SUCCESS);
}
