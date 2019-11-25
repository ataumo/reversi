#include "player.h"

/* define size of file_name string but it can be longer than 64 */
#define MAX_LENGTH_FILE_NAME 64
#define INFINITY 32767 /* borne of integer */

static disc_t last_player = EMPTY_DISC;

/******************************************************************************/
/********************************* TIME PART **********************************/
/******************************************************************************/

/* variable timer */
static clock_t start_time;
static double time_taken;
static size_t max_time = 28;
/******************/
/* init start time */
static void init_time() { start_time = clock(); }

/* return true if time's up */
static bool check_time_out() {
  time_taken = ((double)(clock() - start_time)) / CLOCKS_PER_SEC; // in seconds
  if (time_taken > max_time) {
    return true;
  }
  return false;
}

/******************************************************************************/
/******************************* HEURISTIC PART *******************************/
/******************************************************************************/

/* return the difference of score between the current player and its opponent */
static int score_heuristic(board_t *board, disc_t player) {
  score_t score = board_score(board);
  if (player == BLACK_DISC) {
    return score.black - score.white;
  }
  return score.white - score.black;
}

/* return */
static int score_heuristic_bis(board_t *board, disc_t player) {
  score_t score = board_score(board);
  size_t size = board_size(board);
  int int_score = 0;
  disc_t opponent;

  /** score managment **/
  if (player == BLACK_DISC) {
    int_score = score.black - score.white;
    opponent = WHITE_DISC;
  } else {
    int_score = score.white - score.black;
    opponent = BLACK_DISC;
  }
  /** corners managment **/
  /* if a move has already been played in a corner of board, it can increase the
   * final score */
  move_t tmp_move;
  size_t tab_move[2] = {0, size - 1};
  for (size_t i = 0; i < 2; i++) {
    for (size_t j = 0; j < 2; j++) {
      tmp_move.row = tab_move[i];
      tmp_move.column = tab_move[j];
      /* already play in corner */
      disc_t corner_value = board_get(board, tab_move[i], tab_move[i]);
      if (corner_value == player) {
        int_score = int_score + 5;
      } else if (corner_value == opponent) {
        int_score = int_score - 5;
      }
    }
  }

  /* neighbours corners managment */
  /* reduce the score if neighbours of corner was played */
  if ((board_get(board, 0, 1) == player) ||
      (board_get(board, 1, 0) == player) ||
      (board_get(board, 1, 1) == player) ||
      (board_get(board, 0, size - 2) == player) ||
      (board_get(board, 1, size - 2) == player) ||
      (board_get(board, 1, size - 1) == player) ||
      (board_get(board, size - 2, 0) == player) ||
      (board_get(board, size - 2, 1) == player) ||
      (board_get(board, size - 1, 1) == player) ||
      (board_get(board, size - 1, size - 2) == player) ||
      (board_get(board, size - 2, size - 2) == player) ||
      (board_get(board, size - 2, size - 1) == player)) {
    int_score = int_score - 20;
  }

  /* last player managment (parity) */
  /* end of game */
  if (board_player(board) == EMPTY_DISC) {
    if (last_player == player) {
      int_score = int_score + 5;
    } else if (last_player == opponent) {
      int_score = int_score - 5;
    }
  }

  return int_score;
}

/******************************************************************************/
/***************************** SIMUL BEST PLAYER ******************************/
/******************************************************************************/

move_t simul_best_player(board_t *board) {
  return simul_alpha_beta_player(board);
}

/******************************************************************************/
/************************ FAIL SOFT ALPHA BETA PLAYER *************************/
/******************************************************************************/

static int fail_soft_machine(board_t *board, size_t depth, int alpha, int beta,
                             disc_t player) {
  disc_t current_player = board_player(board);
  int current = -INFINITY;
  if (current_player == EMPTY_DISC || depth == 0) {
    return score_heuristic_bis(board, player);
  }
  if (current_player == player) {
    size_t nbr_poss_moves = board_count_player_moves(board);
    for (size_t i = 0; i < nbr_poss_moves; i++) {
      board_t *tmp_board = board_copy(board);
      move_t current_move = board_next_move(board);
      board_play(tmp_board, current_move);
      /* time managment */
      int score;
      if (check_time_out()) {
        score = fail_soft_machine(tmp_board, 0, alpha, beta, player);
      } else {
        score = fail_soft_machine(tmp_board, depth - 1, alpha, beta, player);
      }
      /******************/
      board_free(tmp_board);
      if (score >= current) {
        current = score;
        if (score >= alpha) {
          alpha = score;
          if (alpha >= beta) {
            break;
          }
        }
      }
    }
    return current;
  } else {
    size_t nbr_poss_moves = board_count_player_moves(board);
    for (size_t i = 0; i < nbr_poss_moves; i++) {
      board_t *tmp_board = board_copy(board);
      move_t current_move = board_next_move(board);
      board_play(tmp_board, current_move);
      /* time managment */
      int score;
      if (check_time_out()) {
        score = fail_soft_machine(tmp_board, 0, alpha, beta, player);
      } else {
        score = fail_soft_machine(tmp_board, depth - 1, alpha, beta, player);
      }
      /******************/
      board_free(tmp_board);
      if (score <= beta) {
        beta = score;
        if (alpha >= beta) {
          break;
        }
      }
    }
    return beta;
  }
}

static move_t fail_soft_player(board_t *board, size_t depth) {
  disc_t current_player = board_player(board);
  int best_score = -INFINITY;
  move_t best_move;
  size_t nbr_poss_moves = board_count_player_moves(board);
  for (size_t i = 0; i < nbr_poss_moves; i++) {

    board_t *tmp_board = board_copy(board);
    move_t current_move = board_next_move(board);
    if (i == 0) {
      best_move = current_move;
    }
    board_play(tmp_board, current_move);
    int score = fail_soft_machine(tmp_board, depth - 1, -INFINITY, INFINITY,
                                  current_player);
    board_free(tmp_board);
    if (score > best_score) {
      best_score = score;
      best_move = current_move;
    }
  }
  return best_move;
}

move_t simul_fail_soft_player(board_t *board) {
  return fail_soft_player(board, DEPTH_FAILSOFT);
}

/******************************************************************************/
/***************************** ALPHA BETA PLAYER ******************************/
/******************************************************************************/

static int alpha_beta_bis_machine(board_t *board, size_t depth, int alpha,
                                  int beta, disc_t player) {
  disc_t current_player = board_player(board);
  /* end of game or depth */
  if (current_player == EMPTY_DISC || depth == 0) {
    return score_heuristic_bis(board, player);
  }

  last_player = current_player;

  if (current_player == player) {
    size_t nbr_poss_moves = board_count_player_moves(board);
    for (size_t i = 0; i < nbr_poss_moves; i++) {
      board_t *tmp_board = board_copy(board);
      move_t current_move = board_next_move(board);
      board_play(tmp_board, current_move);
      /* time managment */
      int score;
      if (check_time_out()) {
        score = alpha_beta_bis_machine(tmp_board, 0, alpha, beta, player);
      } else {
        score =
            alpha_beta_bis_machine(tmp_board, depth - 1, alpha, beta, player);
      }
      /******************/
      board_free(tmp_board);
      if (score > alpha) {
        alpha = score;
        if (alpha >= beta) {
          break;
        }
      }
    }
    return alpha;
  } else {
    size_t nbr_poss_moves = board_count_player_moves(board);
    for (size_t i = 0; i < nbr_poss_moves; i++) {
      board_t *tmp_board = board_copy(board);
      move_t current_move = board_next_move(board);
      board_play(tmp_board, current_move);
      /* time managment */
      int score;
      if (check_time_out()) {
        score = alpha_beta_bis_machine(tmp_board, 0, alpha, beta, player);
      } else {
        score =
            alpha_beta_bis_machine(tmp_board, depth - 1, alpha, beta, player);
      }
      /******************/
      board_free(tmp_board);
      if (score < beta) {
        beta = score;
        if (alpha >= beta) {
          break;
        }
      }
    }
    return beta;
  }
}

static move_t alpha_beta_bis_player(board_t *board, size_t depth) {
  disc_t current_player = board_player(board);
  int best_score = -INFINITY;
  move_t best_move;
  size_t nbr_poss_moves = board_count_player_moves(board);
  /* init of timer */
  init_time();
  /*****************/
  for (size_t i = 0; i < nbr_poss_moves; i++) {
    board_t *tmp_board = board_copy(board);
    move_t current_move = board_next_move(board);
    if (i == 0) {
      best_move = current_move;
    }
    board_play(tmp_board, current_move);
    int score = alpha_beta_bis_machine(tmp_board, depth - 1, -INFINITY,
                                       INFINITY, current_player);
    if (score >= best_score) {
      best_score = score;
      best_move = current_move;
    }
    board_free(tmp_board);
  }
  return best_move;
}

move_t simul_alpha_beta_bis_player(board_t *board) {
  return alpha_beta_bis_player(board, DEPTH_ALPHABETA_BIS);
}

/******************************************************************************/
/************************* MINIMAX ALPHA BETA PLAYER **************************/
/******************************************************************************/

static int alpha_beta_machine(board_t *board, size_t depth, int alpha, int beta,
                              disc_t player) {
  disc_t current_player = board_player(board);
  if (current_player == EMPTY_DISC || depth == 0) {
    return score_heuristic(board, player);
  }
  if (current_player == player) {
    size_t nbr_poss_moves = board_count_player_moves(board);
    for (size_t i = 0; i < nbr_poss_moves; i++) {
      board_t *tmp_board = board_copy(board);
      move_t current_move = board_next_move(board);
      board_play(tmp_board, current_move);
      int score = alpha_beta_machine(tmp_board, depth - 1, alpha, beta, player);
      board_free(tmp_board);
      if (score > alpha) {
        alpha = score;
        if (alpha >= beta) {
          break;
        }
      }
    }
    return alpha;
  } else {
    size_t nbr_poss_moves = board_count_player_moves(board);
    for (size_t i = 0; i < nbr_poss_moves; i++) {
      board_t *tmp_board = board_copy(board);
      move_t current_move = board_next_move(board);
      board_play(tmp_board, current_move);
      int score = alpha_beta_machine(tmp_board, depth - 1, alpha, beta, player);
      board_free(tmp_board);
      if (score < beta) {
        beta = score;
        if (alpha >= beta) {
          break;
        }
      }
    }
    return beta;
  }
}

static move_t alpha_beta_player(board_t *board, size_t depth) {
  disc_t current_player = board_player(board);
  int best_score = -INFINITY;
  move_t best_move;
  size_t nbr_poss_moves = board_count_player_moves(board);
  for (size_t i = 0; i < nbr_poss_moves; i++) {

    board_t *tmp_board = board_copy(board);
    move_t current_move = board_next_move(board);
    if (i == 0) {
      best_move = current_move;
    }
    board_play(tmp_board, current_move);
    int score = alpha_beta_machine(tmp_board, depth - 1, -INFINITY, INFINITY,
                                   current_player);
    board_free(tmp_board);
    if (score > best_score) {
      best_score = score;
      best_move = current_move;
    }
  }
  return best_move;
}

move_t simul_alpha_beta_player(board_t *board) {
  return alpha_beta_player(board, DEPTH_ALPHABETA);
}

/******************************************************************************/
/**************************** MINIMAX PLAYER PART *****************************/
/******************************************************************************/

static int minimax_machine(board_t *board, size_t depth, disc_t player) {
  disc_t current_player = board_player(board);
  if (current_player == EMPTY_DISC || depth == 0) {
    return score_heuristic(board, player);
  }
  int best_score;
  if (current_player == player) {
    best_score = -INFINITY;
    size_t nbr_poss_moves = board_count_player_moves(board);
    for (size_t i = 0; i < nbr_poss_moves; i++) {
      board_t *tmp_board = board_copy(board);
      move_t current_move = board_next_move(board);
      board_play(tmp_board, current_move);
      int score = minimax_machine(tmp_board, depth - 1, player);
      if (score > best_score) {
        best_score = score;
      }
      board_free(tmp_board);
    }
  } else {
    best_score = INFINITY;
    size_t nbr_poss_moves = board_count_player_moves(board);
    for (size_t i = 0; i < nbr_poss_moves; i++) {
      board_t *tmp_board = board_copy(board);
      move_t current_move = board_next_move(board);
      board_play(tmp_board, current_move);
      int score = minimax_machine(tmp_board, depth - 1, player);
      if (score < best_score) {
        best_score = score;
      }
      board_free(tmp_board);
    }
  }
  return best_score;
}

static move_t minimax_player(board_t *board, size_t depth) {
  disc_t current_player = board_player(board);
  int best_score = -INFINITY;
  move_t best_move;

  size_t nbr_poss_moves = board_count_player_moves(board);
  for (size_t i = 0; i < nbr_poss_moves; i++) {

    board_t *tmp_board = board_copy(board);
    move_t current_move = board_next_move(board);
    if (i == 0) {
      best_move = current_move;
    }
    board_play(tmp_board, current_move);
    int score = minimax_machine(tmp_board, depth - 1, current_player);
    if (score > best_score) {
      best_score = score;
      best_move = current_move;
    }
    board_free(tmp_board);
  }
  return best_move;
}

move_t simul_minimax_player(board_t *board) {
  return minimax_player(board, DEPTH_MINIMAX);
}

/******************************************************************************/
/***************************** RANDOM PLAYER PART *****************************/
/******************************************************************************/

static void rand_init(void) {
  static bool isinitialized = false;
  if (!isinitialized) {
    srandom(time(NULL) - getpid() + clock());
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

/******************************************************************************/
/******************************* GAME SAVE PART *******************************/
/******************************************************************************/

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
          free(new_file_name);
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

/******************************************************************************/
/***************************** HUMAN PLAYER PART ******************************/
/******************************************************************************/

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

/* return size,x or x,size if incorrect move is given and size+1,size+1 if
 * quit
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
      if (!error_case) { /* if error case is not already true */
        error_case = false;
      }
    }
    if (!error_case) {
      if (!board_is_move_valid(board, move)) {
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
