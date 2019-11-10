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

/** AI TEST **/
static size_t min_coor(size_t a, size_t b) {
  if (a < b) {
    return a;
  }
  return b;
}

move_t priority_borders(board_t *board) {
  size_t nbr_poss_moves = board_count_player_moves(board);
  size_t size = board_size(board);
  size_t min = 0;
  move_t best_move;
  for (size_t i = 0; i < nbr_poss_moves; i++) {
    move_t current_move = board_next_move(board);
    move_t copy_current_move = current_move;
    if (current_move.row >= size / 2) {
      copy_current_move.row = size - current_move.row - 1;
    }
    if (current_move.column >= size / 2) {
      copy_current_move.column = size - current_move.column - 1;
    }
    if (i == 0) {
      best_move = current_move;
      min = min_coor(copy_current_move.row, copy_current_move.column) + 1;
    }
    size_t current_min =
        min_coor(copy_current_move.row, copy_current_move.column) + 1;
    if (current_min <= min) {
      best_move = current_move;
      min = current_min;
    }
  }
  return best_move;
}

/** MINIMAX ALPHA BETA PLAYER **/

move_t minimax_ab_player_machine(board_t *board, size_t depth, int alpha,
                                 int beta) {
  disc_t current_player = board_player(board);
  size_t size = board_size(board);
  move_t best_move;
  best_move.row = size;
  best_move.column = size;
  if (current_player == EMPTY_DISC) {
    return best_move;
  } else {
    size_t nbr_poss_moves = board_count_player_moves(board);
    int best_score;
    for (size_t i = 0; i < nbr_poss_moves; i++) {
      board_t *tmp_board = board_copy(board);
      move_t current_move = board_next_move(board);
      if (i == 0) { /* init of tmp values */
        best_score = score_heuristic(tmp_board, current_player);
        best_move = current_move;
      }
      board_play(tmp_board, current_move); /* play the new move in tmp_board */
      int new_score = 0;
      if (depth == 1) {
        new_score = score_heuristic(tmp_board, current_player);
      } else if (current_player ==
                 board_player(tmp_board)) { /* stay the same */
        best_move = max_move(tmp_board, depth - 1);
        if (best_move.row != size) {
          new_score = score_heuristic(tmp_board, current_player);
        }
      } else {
        board_play(tmp_board, min_move(tmp_board, depth - 1));
        if (best_move.row != size) {
          new_score = score_heuristic(tmp_board, current_player);
        }
      }
      if (new_score > best_score) {
        best_score = new_score;
        best_move = current_move;
      }
      board_free(tmp_board);
    }
  }
  return best_move;
}

/** MINIMAX PLAYER PART **/
move_t simul_minimax_player(board_t *board) {
  int current_score = score_heuristic(board, board_player(board));
  printf("%d\n", current_score);
  if (current_score <= 0) { /* critical situation */
    return minimax_player(board, 4);
  }
  if (current_score <= 5) {
    return minimax_player(board, 2);
  }
  return minimax_player(board, 1);
}

move_t minimax_player(board_t *board, size_t depth) {
  return max_move(board, depth);
}

move_t max_move(board_t *board, size_t depth) {
  disc_t current_player = board_player(board); /* get the current player */
  size_t size = board_size(board);             /* get board size */
  move_t best_move;                            /* set the best_move */
  best_move.row = size;
  best_move.column = size;
  if (current_player == EMPTY_DISC) { /* game over */
    return best_move;
  } else {
    size_t nbr_poss_moves = board_count_player_moves(board);
    int best_score;
    for (size_t i = 0; i < nbr_poss_moves; i++) {   /* for each moves */
      board_t *tmp_board = board_copy(board);       /* create copy of board */
      move_t current_move = board_next_move(board); /* get next move */
      if (i == 0) {                                 /* init of tmp values */
        best_score = score_heuristic(tmp_board, current_player);
        best_move = current_move;
      }
      board_play(tmp_board, current_move); /* play the new move in tmp_board
      // */
      int new_score = 0;
      if (depth == 1) {
        new_score = score_heuristic(tmp_board, current_player);
      } else if (current_player ==
                 board_player(tmp_board)) { /* stay the same */
        best_move = max_move(tmp_board, depth - 1);
        if (best_move.row != size) {
          new_score = score_heuristic(tmp_board, current_player);
        }
      } else {
        board_play(tmp_board, min_move(tmp_board, depth - 1));
        if (best_move.row != size) {
          new_score = score_heuristic(tmp_board, current_player);
        }
      }
      if (new_score > best_score) {
        best_score = new_score;
        best_move = current_move;
      }
      board_free(tmp_board);
    }
  }
  return best_move;
}

move_t min_move(board_t *board, size_t depth) {
  disc_t current_player = board_player(board);
  size_t size = board_size(board);
  move_t best_move;
  best_move.row = size;
  best_move.column = size;
  if (current_player == EMPTY_DISC) {
    return best_move;
  } else {
    size_t nbr_poss_moves = board_count_player_moves(board);
    int best_score;
    for (size_t i = 0; i < nbr_poss_moves; i++) {
      board_t *tmp_board = board_copy(board);
      move_t current_move = board_next_move(board);
      if (i == 0) { /* init of tmp values */
        best_score = score_heuristic(tmp_board, current_player);
        best_move = current_move;
      }
      board_play(tmp_board, current_move); /* play the new move in tmp_board */
      int new_score = 0;
      if (depth == 1) {
        new_score = score_heuristic(tmp_board, current_player);
      } else if (current_player ==
                 board_player(tmp_board)) { /* stay the same */
        board_play(tmp_board, min_move(tmp_board, depth - 1));
        if (best_move.row != size) {
          new_score = score_heuristic(tmp_board, current_player);
        }
      } else {
        board_play(tmp_board, max_move(tmp_board, depth - 1));
        if (best_move.row != size) {
          new_score = score_heuristic(tmp_board, current_player);
        }
      }
      if (new_score < best_score) {
        best_score = new_score;
        best_move = current_move;
      }
      board_free(tmp_board);
    }
  }
  return best_move;
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
  if (file_name == NULL) {
    return;
  }
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
          if (new_file_name == NULL) {
            return;
          }
          new_file_name = file_name;
          file_name = calloc(MAX_LENGTH_FILE_NAME + nbr_out, sizeof(char));
          if (file_name == NULL) {
            return;
          }
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
  if (file == NULL) {
    return;
  }
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

/* return size if char of column is incorrect */
static size_t get_column(char current_char, size_t size) {
  int alpha_maj[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
  int alpha_min[10] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
  for (size_t i = 0; i < 10; i++) {
    if (current_char == alpha_maj[i] || current_char == alpha_min[i]) {
      return i;
    }
  }
  return size; /* incorrect column */
}

/* return size,x or x,size if incorrect move is given and size+1,size+1 if quit
 */
static move_t get_move(size_t size) {
  move_t move;
  char current_char = getchar(); /* get the first char */
  if (current_char == 'q' || current_char == 'Q') {
    clean_buffer();
    move.row = size + 1;
    move.column = size + 1;
    return move;
  }
  move.column = get_column(current_char, size);
  char char_number[2]; /* column number */
  for (size_t i = 0; current_char != '\n'; i++) {
    current_char = getchar();
    if (i < 2) {
      char_number[i] = current_char;
    }
  }
  move.row = atoi(char_number);
  if (move.row == 0 ||
      move.row > size) { /* if a10 is given, 10!=0 so move.row=9 error ! */
    move.row = size;
  } else {
    move.row--;
  }
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
    move = get_move(size);
    if (move.row == size + 1) { /* if q or Q is given */
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
    if (move.row == size) { /* row or colum is invalid */
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
