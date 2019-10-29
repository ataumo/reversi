#include "player.h"

/* define size of file_name string but it can be longer than 64 */
#define MAX_LENGTH_FILE_NAME 64

/* return the difference of score between the current player and its opponent */
static int score_heuristic(board_t *board, disc_t player) {
  score_t score = board_score(board);
  if (player == BLACK_DISC) {
    return score.black - score.white;
  } else {
    return score.white - score.black;
  }
  return 0;
}

/** RANDOM PLAYER PART **/

static void rand_init(void) {
  static bool isinitialized = false;
  if (!isinitialized) {
    srandom(time(NULL) - getpid());
    isinitialized = true;
  }
}

move_t random_player(board_t *board) {
  /* nbr_poss_moves > 0 if random_player is run */
  size_t nbr_poss_moves = board_count_player_moves(board);
  rand_init();
  size_t r = (size_t)(random() % nbr_poss_moves);
  for (size_t i = 0; i < r; i++) {
    board_next_move(board);
  }
  return board_next_move(board);
}

/** GAME SAVE PART **/

static void clean_buffer() {
  int c = 0;
  while (c != '\n') {
    c = getchar();
  }
}

/* save the content of the board in a file given by the user */
static void game_save(board_t *board) {
  char *file_name = calloc(MAX_LENGTH_FILE_NAME, sizeof(char));
  fprintf(stdout, "Give a filename to save the game (default: 'board.txt'):");
  /** get file name section **/
  char current_char = getchar();
  if (current_char == '\n') { /* file name is not given */
    char *init_file_name = "board.txt";
    for (size_t i = 0; i < 9; i++) {
      file_name[i] = init_file_name[i];
    }
  } else {
    size_t i = 0;
    size_t nbr_out = 0;
    while (current_char != '\n') {
      if (current_char != ' ') {
        file_name[i] = current_char;
        i++;
        if (i == MAX_LENGTH_FILE_NAME) {
          nbr_out++;
          char *new_file_name = calloc(MAX_LENGTH_FILE_NAME, sizeof(char));
          new_file_name = file_name;
          file_name = calloc(MAX_LENGTH_FILE_NAME + nbr_out, sizeof(char));
          file_name = new_file_name;
        }
      }
      current_char = getchar();
    }
  }
  /** writing section **/
  fprintf(stdout, "creating file with name %s\n", file_name);
  FILE *file =
      fopen(file_name, "w+"); /* create this file if no already exist */
  fprintf(file, "%c\n", board_player(board));
  size_t size = board_size(board);
  disc_t current_disc;
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      current_disc = board_get(board, i, j);
      if (current_disc == HINT_DISC) {
        current_disc = EMPTY_DISC;
      }
      fprintf(file, "%c ", current_disc);
    }
    fprintf(file, "\n");
  }
  fclose(file);
  free(file_name);
}

/** HUMAN PLAYER PART **/

static size_t get_column(char current_char) {
  int alpha_maj[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
  int alpha_min[10] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
  for (size_t i = 0; i < 10; i++) {
    if (current_char == alpha_maj[i] || current_char == alpha_min[i]) {
      return i;
    }
  }
  return -1;
}

/* return -1,x or x,-1 if incorrect move is given and -2,-2 if quit */
static move_t get_move() {
  move_t move;
  char current_char = getchar();
  if (current_char == 'q' || current_char == 'Q') {
    clean_buffer();
    move.row = -2;
    move.column = -2;
    return move;
  }
  move.column = get_column(current_char);
  char *char_number = calloc(2, sizeof(char));
  for (size_t i = 0; current_char != '\n'; i++) {
    current_char = getchar();
    char_number[i] = current_char;
  }
  move.row = atoi(char_number) - 1;
  return move;
}

move_t human_player(board_t *board) {
  board_print(board, stdout);      /* print the board */
  size_t size = board_size(board); /* get size of board */
  bool error_case = true;
  move_t move;
  move.row = size;
  move.column = size;
  while (error_case) {
    fprintf(stdout,
            "Give your move (e. g. 'A5' or 'a5'), press 'q' or 'Q' to quit:");
    move = get_move();
    if ((int)move.row == -2) { /* if q or Q is given */
      fprintf(stdout, "Quitting, do you want to save this game ? (y/N):");
      char current_char = getchar();
      if (current_char == 'y' || current_char == 'Y') {
        clean_buffer();
        game_save(board);
      }
      board_set_player(board, EMPTY_DISC);
      move.row = size;
      move.column = size;
      return move;
    }
    if ((int)move.row == -1 ||
        (int)move.column == -1) { /* row or colum is invalid */
      error_case = true;
    } else {
      if (move.row >= size) {
        error_case = true;
        fprintf(stdout, "Row out of bounds. ");
      } else {
        error_case = false;
      }
      if (move.column >= size) {
        error_case = true;
        fprintf(stdout, "Column out of bounds. ");
      } else {
        error_case = false;
      }
      if (!error_case && !board_is_move_valid(board, move)) {
        error_case = true;
        fprintf(stdout, "This move is invalid. ");
      } else {
        error_case = false;
      }
    }
    if (error_case) {
      fprintf(stdout, "Wrong input, try again !\n\n");
    }
  }
  return move;
}
