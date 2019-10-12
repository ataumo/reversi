#include "reversi.h"
#include "board.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>

#define MAX_LENGTH 512

/* use for keep disc player in file parser */
typedef struct {
  disc_t player;
  int x;
  int y;
} coor_disc;

static board_t *file_parser(const char *filename) {
  FILE *file;
  file = fopen(filename, "r+");
  if (!file) {
    fprintf(stderr,
            "reversi.c:file_parser(): error: failed to read file : %s\n",
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  /* boolean type */
  bool begin_board = false;
  bool comment_status = false;
  bool content = false;
  bool player_is_saved = false;

  /* int type */
  int current_column_count = 0;
  int line_count = 0;
  int size_column = 0;
  int size_raw = 0;

  /* other type */
  char current_char = '\0';
  size_t nbr_of_disc = 0;
  disc_t player;
  coor_disc tab_disc[100];

  while (current_char != EOF) {

    current_char = fgetc(file);

    switch (current_char) {
    case '#':
      comment_status = true;
      break;
    case 'O':
    case 'X':
      if (!comment_status) {    /* not comment line */
        if (!player_is_saved) { /* player is not saved */
          player = current_char;
          player_is_saved = true;
        } else {
          begin_board = true;
          content = true;
          /* keep coordinate in tab_disc */
          tab_disc[nbr_of_disc].player = current_char;
          tab_disc[nbr_of_disc].x = current_column_count;
          tab_disc[nbr_of_disc].y = size_raw;
          nbr_of_disc++;
          current_column_count++;
        }
      }
      break;
    case '_': /* beginning of board */
      if (!comment_status) {
        begin_board = true;
        if (!player_is_saved) {
          fprintf(stderr,
                  "reversi.c:file_parser(): error: player is missing\n");
          exit(EXIT_FAILURE);
        }
        content = true;
        current_column_count++;
      }
      break;
    case EOF:
    case '\n':
      line_count++;
      if (begin_board) {
        if (content) {
          size_raw++;
        }
        if (size_column == 0) {
          size_column = current_column_count;
        }
      }
      if (content) {
        if (current_column_count != size_column) {
          if (size_column > current_column_count) {
            fprintf(stderr,
                    "reversi.c:file_parser(): error: character is missed in "
                    "line %d\n",
                    line_count);
          } else {
            fprintf(stderr,
                    "reversi.c:file_parser(): error: too much character in "
                    "line %d\n",
                    line_count);
          }
          exit(EXIT_FAILURE);
        }
      }
      current_column_count = 0;
      comment_status = false;
      content = false;
      break;
    default:
      if (current_char != ' ' && current_char != '\t' && !comment_status) {
        if (!player_is_saved) {
          fprintf(stderr,
                  "reversi.c:file_parser(): error: player is incorrect\n");
        } else {
          fprintf(
              stderr,
              "reversi.c:file_parser(): error: wrong character %c at line %d\n",
              current_char, line_count + 1);
        }
        exit(EXIT_FAILURE);
      }
      break;
    }
  } /* end of while */
  if (size_raw != size_column) {
    if (size_raw > size_column) {
      fprintf(stderr,
              "reversi.c:file_parser(): error: board has %d extra raw\n",
              size_raw - size_column);
    } else {
      fprintf(stderr,
              "reversi.c:file_parser(): error: board has %d missing raw\n",
              size_column - size_raw);
    }
    exit(EXIT_FAILURE);
  }
  if (size_raw % 2 != 0) {
    fprintf(stderr,
            "reversi.c:file_parser(): error: size of board need to be pair\n");
    exit(EXIT_FAILURE);
  }
  if (size_raw / 2 < 1 || size_raw / 2 > 5) {
    if (size_raw == 0) {
      fprintf(stderr, "reversi.c:file_parser(): error: missing board\n");
    } else {
      fprintf(stderr,
              "reversi.c:file_parser(): error: size of square need to be 2, 4, "
              "6, 8 or 10, not %d\n",
              size_raw);
    }
    exit(EXIT_FAILURE);
  }
  fclose(file);
  size_t size = size_raw;
  board_t *newboard = board_alloc(size, player);
  /* set value of board */
  for (size_t i = 0; i < nbr_of_disc; i++) {
    board_set(newboard, tab_disc[i].player, tab_disc[i].x, tab_disc[i].y);
  }
  return newboard;
}

static void print_usage() {
  printf("Usage: reversi [-s SIZE|-b [N]|-w [N]|-c|-v|-V|-h] [FILE]\n"
         "Play a reversi game with human or program players.\n"
         "  -s, --size SIZE       board size (min=1, max=5 (default=4))\n"
         "  -b, --black-ai [N]    set tactic of black player (default: 0)\n"
         "  -w, --white-ai [N]    set tactic of black player (default: 0)\n"
         "  -c, --contest         enable 'contest' mode\n"
         "  -V, --version         display version and exit\n"
         "  -h, --help            display this help and exit\n"
         "\n"
         "Tactic list: human (0), random (1)\n");
}

int main(int argc, char *argv[]) {
  const struct option longopts[] = {{"size", required_argument, NULL, 's'},
                                    {"black-ai", optional_argument, NULL, 'b'},
                                    {"white-ai", optional_argument, NULL, 'w'},
                                    {"contest", no_argument, NULL, 'c'},
                                    {"verbose", no_argument, NULL, 'v'},
                                    {"version", no_argument, NULL, 'V'},
                                    {"help", no_argument, NULL, 'h'},
                                    {NULL, 0, NULL, 0}};

  /* boolean variables */
  bool contest_mode = false;  /* true if -c option is given */
  bool file_argument = false; /* true if file argument is given */
  bool tactic_b_player = 0;   /* 0 if humain plays, 1 if it is random */
  bool tactic_w_player = 0;   /* 0 if humain plays, 1 if it is random */
  bool verbose = false;       /* verbose variable */

  /* integer variables */
  int cpt_of_file = 0; /* number of file argument */
  int int_optarg;      /* argument of option */
  int optc;            /* current value of option */

  /* other type */
  size_t board_size_num = 4;
  FILE *file = NULL;
  char *file_name;
  static const char *opts = ":s:b::w::cvVh"; /*valid options*/

  while ((optc = getopt_long(argc, argv, opts, longopts, NULL)) != -1) {
    switch (optc) {

    case 's': /* 'size' option */
      if ((int_optarg < 1 || int_optarg > 5) && strlen(optarg) != 1) {
        printf("reversi.c:main(): error: the argument of -s option should be "
               "between 1 and 5\n");
        exit(EXIT_FAILURE);
      }
      board_size_num = int_optarg * 2;
      break;

    case 'b':               /* 'black-ai' option */
      if (optarg != NULL) { /* if argument is given */
        int_optarg = atoi(optarg);
        if ((int_optarg != 1 && int_optarg != 0) && strlen(optarg) != 1) {
          printf("reversi.c:main(): error: the argument of -b option should be "
                 "0 or 1\n");
          exit(EXIT_FAILURE);
        }
        tactic_b_player = int_optarg;
      }
      break;

    case 'w':               /* 'white-ai' option */
      if (optarg != NULL) { /* if argument is given */
        int_optarg = atoi(optarg);
        if ((int_optarg != 1 && int_optarg != 0) && strlen(optarg) != 1) {
          printf("reversi.c:main(): error: the argument of -w option should be "
                 "0 or 1\n");
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

    case 'V':               /* 'version' option */
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
      fprintf(stderr,
              "reversi.c:main(): error: -%c commande must have an argument\n",
              optopt);
      exit(EXIT_FAILURE);
      break;

    default: /* if an unknown option is given */
      fprintf(stderr,
              "Illegal option: Try 'reversi --help' for more information\n");
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
    fprintf(stderr, "reversi.c:main(): error: You must to give 1 file for the "
                    "contest mode\n");
  } else {
    if (file_argument) { /* file argument is given */
      file = fopen(file_name, "r+");
      if (file == NULL) { /* The given argument is not a readable file */
        fprintf(stderr, "reversi.c:main(): error: opening the file: %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
      }
      if (contest_mode) { /* contest mode is enable */
        /* read in contest file */
        fclose(file);
      } else { /* normal mode */
        board_t *newboard = file_parser(file_name);
        board_print(newboard, stdout);
        score_t score;
        score = board_score(newboard);
        printf("black = %d, white = %d\n", score.black, score.white);
      }
    } else {
      if (contest_mode) { /* constes mode is enable */
        fprintf(stderr, "reversi.c:main(): error: contest mode is activated "
                        "but no file is given\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  exit(EXIT_SUCCESS);
}
