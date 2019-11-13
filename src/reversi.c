#include "reversi.h"
#include "board.h"
#include "player.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>

#define MAX_LENGTH 512
/* define number of player functions and change array */
#define NBR_PLAY_FUNC 4
static move_t (*play_func[NBR_PLAY_FUNC])(board_t *board) = {
    human_player, random_player, simul_alpha_beta_player,
    simul_alpha_beta_bis_player};
static char *name_play_func[NBR_PLAY_FUNC] = {
    "human", "random", "alpha_beta_player", "alpha_beta_bis_player"};

static bool VERBOSE = false; /* verbose variable */

/* use for keep disc player in file parser */
typedef struct {
  disc_t player;
  int x;
  int y;
} coor_disc;

static char get_alpha_column(size_t row) {
  int alpha_maj[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
  return alpha_maj[row];
}

static int game(move_t (*black)(board_t *), move_t (*white)(board_t *),
                board_t *board) {
  disc_t current_player = board_player(board); /* get current player */
  score_t score;
  size_t black_score;
  size_t white_score;
  size_t size = board_size(board);
  move_t move;
  /*welcome print part */
  char *black_player_type;
  char *white_player_type;
  char *first_player = "Black";
  if (current_player == WHITE_DISC) {
    first_player = "White";
  }
  for (size_t i = 0; i < NBR_PLAY_FUNC; i++) {
    if (black == play_func[i]) {
      black_player_type = name_play_func[i];
    }
    if (white == play_func[i]) {
      white_player_type = name_play_func[i];
    }
  }
  fprintf(stdout,
          "Welcome to this reversi game !\n"
          "Black player (X) is %s and white player (O) is %s.\n"
          "%s player start !\n",
          black_player_type, white_player_type, first_player);

  if (!VERBOSE) {
    fprintf(stdout, "\nPlaying...\n");
  }
  /* main loop */
  while (current_player != EMPTY_DISC) {
    if (current_player == BLACK_DISC) {          /* turn of black player */
      if (board_count_player_moves(board) > 0) { /* player can play */
        move = black(board);                     /* get the move */
        if (black == human_player) {
          if (move.row == size && move.column == size) {
            fprintf(stdout, "Player 'X' resigned. player 'O' win the game.\n");
            return -1;
          }
        } else {         /* if random player or others */
          if (VERBOSE) { /* if verbose option is activated */
            board_print(board, stdout);
            fprintf(stdout, "'X' plays %c%zu\n", get_alpha_column(move.column),
                    (move.row + 1));
          }
        }
      }
      board_play(board, move); /* play in all cases */
    }
    if (current_player == WHITE_DISC) { /* turn of white player */
      if (board_count_player_moves(board) > 0) {
        move = white(board); /* get the move */
        if (white == human_player) {
          if (move.row == size && move.column == size) {
            fprintf(stdout, "Player 'O' resigned. player 'X' win the game.\n");
            return -2;
          }
        } else {         /* if random player or others */
          if (VERBOSE) { /* if verbose option is activated */
            board_print(board, stdout);
            fprintf(stdout, "'O' plays %c%zu\n", get_alpha_column(move.column),
                    move.row + 1);
          }
        }
      }
      board_play(board, move); /* play in all cases */
    }
    if (VERBOSE) {
      fprintf(stdout, "==========================================\n");
    }
    current_player = board_player(board);
  }
  board_print(board, stdout);
  score = board_score(board);
  black_score = score.black;
  white_score = score.white;
  if (black_score > white_score) {
    fprintf(stdout, "Player 'X' win the game.\n");
    return 1;
  }
  if (black_score < white_score) {
    fprintf(stdout, "Player 'O' win the game.\n");
    return 2;
  }
  /* if black_score == white_score */
  fprintf(stdout, "Draw game, no winner.\n");
  return 0;
}

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
  int current_char = '\0';
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
          errx(1, "player is missing");
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
            fprintf(stderr, "reversi: character is missed in line %d\n",
                    line_count);
          } else {
            fprintf(stderr, "reversi: too much character in line %d\n",
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
          fprintf(stderr, "reversi: player is incorrect\n");
        } else {
          fprintf(stderr, "reversi: wrong character %c at line %d\n",
                  current_char, line_count + 1);
        }
        exit(EXIT_FAILURE);
      }
      break;
    }
  } /* end of while */
  if (size_raw != size_column) {
    if (size_raw > size_column) {
      fprintf(stderr, "reversi: board has %d extra raw\n",
              size_raw - size_column);
    } else {
      fprintf(stderr, "reversi: board has %d missing raw\n",
              size_column - size_raw);
    }
    exit(EXIT_FAILURE);
  }
  if (size_raw % 2 != 0) {
    errx(1, "reversi: size of board need to be pair");
  }
  if (size_raw < MIN_BOARD_SIZE || size_raw > MAX_BOARD_SIZE) {
    if (size_raw == 0) {
      fprintf(stderr, "reversi: error: missing board\n");
    } else {
      fprintf(stderr,
              "reversi: size of square need to be 2, 4, 6, 8 or 10, not %d\n",
              size_raw);
    }
    exit(EXIT_FAILURE);
  }
  fclose(file);
  size_t size = size_raw;
  board_t *newboard = board_alloc(size, player);
  /* set value of board */
  for (size_t i = 0; i < nbr_of_disc; i++) {
    board_set(newboard, tab_disc[i].player, tab_disc[i].y, tab_disc[i].x);
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
  int tactic_b_player =
      2; /* 0 if humain plays, 1 if it is random, 2 if minimax player */
  int tactic_w_player =
      2; /* 0 if humain plays, 1 if it is random, 2 if minimax player */

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
      int_optarg = atoi(optarg);
      if (strlen(optarg) != 1) {
        errx(1, "the argument of -s option is too long");
      }
      if ((int_optarg < 1 || int_optarg > 5)) {
        errx(1, "the argument of -s option should be between 1 and 5");
      }
      board_size_num = int_optarg;
      break;

    case 'b':               /* 'black-ai' option */
      if (optarg != NULL) { /* if argument is given */
        int_optarg = atoi(optarg);
        if (strlen(optarg) != 1) {
          errx(1, "the argument of -b option is too long");
        }
        if (int_optarg > NBR_PLAY_FUNC - 1) {
          errx(1, "the argument of -w option should be 0 to %d",
               NBR_PLAY_FUNC - 1);
        }
        tactic_b_player = int_optarg;
      }
      break;

    case 'w':               /* 'white-ai' option */
      if (optarg != NULL) { /* if argument is given */
        int_optarg = atoi(optarg);
        if (strlen(optarg) != 1) {
          errx(1, "the argument of -w option is too long");
        }
        if (int_optarg > NBR_PLAY_FUNC - 1) {
          errx(1, "the argument of -w option should be 0 to %d",
               NBR_PLAY_FUNC - 1);
        }
        tactic_w_player = int_optarg;
      }
      break;

    case 'c': /* 'contest mode' option */
      contest_mode = true;
      break;

    case 'v': /* 'verbose' option */
      VERBOSE = true;
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
      errx(1, "-%c commande must have an argument", optopt);
      break;

    default: /* if an unknown option is given */
      errx(1, "Illegal option: Try 'reversi --help' for more information");
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
    errx(1, "You must to give 1 file for the contest mode");
  } else {
    /***********************************************/
    move_t (*blackfunc)(board_t *) = play_func[tactic_b_player];
    move_t (*whitefunc)(board_t *) = play_func[tactic_w_player];
    /***********************************************/
    if (file_argument) { /* file argument is given */
      file = fopen(file_name, "r+");
      if (file == NULL) { /* The given argument is not a readable file */
        err(errno, "%s", file_name);
        exit(EXIT_FAILURE);
      }
      if (contest_mode) { /* contest mode is enable */
        /******************* contest mode **********************/
        board_t *board = file_parser(file_name); /* read in contest file */
        game(blackfunc, whitefunc, board);
        board_free(board);
      } else { /* normal mode */
        /******************* normal mode with file **********************/
        board_t *board = file_parser(file_name);
        game(blackfunc, whitefunc, board);
        board_free(board);
      }
      fclose(file);
    } else {              /* no file is given */
      if (contest_mode) { /* contest mode is enable */
        errx(1, "Contest mode is activated but no file is given");
      }
      /******************* normal mode without file **********************/
      board_t *board = board_init(board_size_num * 2);
      game(blackfunc, whitefunc, board);
      board_free(board);
    }
  }

  exit(EXIT_SUCCESS);
}
