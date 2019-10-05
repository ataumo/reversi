#include "board.h"

#include <err.h>
#include <string.h>

/* Internal board_t structure (hidden from the outside) */
struct board_t {
  size_t size;
  disc_t player;
  disc_t **board;
};

/* allocate memory needed to creat a board of size 'size' */
/* EXIT_FAILURE if something wrong happened */
board_t *board_alloc(const size_t size, const disc_t player) {
  board_t *board = malloc(sizeof(board_t));
  if (board == NULL) {
    fprintf(stderr, "board.c:board_alloc(): error: echec of malloc to "
                    "allocate size of board_t\n");
    exit(EXIT_FAILURE);
  }
  board->board = malloc(size * sizeof(disc_t *));
  if (board->board == NULL) {
    fprintf(stderr, "board.c:board_alloc(): error: echec of malloc to allocate "
                    "size of disc_t*\n");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < size; i++) {
    board->board[i] = malloc(size * sizeof(disc_t));
    if (board->board[i] == NULL) {
      fprintf(stderr, "board.c:board_alloc(): error: echec of malloc to "
                      "allocate size of disc_t\n");
      exit(EXIT_FAILURE);
    }
  }
  /* init board to EMPTY_DISC */
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      board->board[i][j] = EMPTY_DISC;
    }
  }
  board->size = size;
  board->player = player;
  return board;
}

/* free memory allocated to hold the board */
void board_free(board_t *board) {
  size_t size = board->size;
  for (size_t i = 0; i < size; i++) {
    free(board->board[i]);
  }
  free(board->board);
  free(board);
}

/* init all the squares of the board as a starting game */
board_t *board_init(const size_t size) {
  if (size % 2 != 0 || size / 2 < 1 || size / 2 > 5) {
    fprintf(stderr, "board.c:board_init(): error: error of size (%d)\n", size);
    exit(EXIT_FAILURE);
  }
  board_t *board = board_alloc(size, BLACK_DISC); /* creat a new void board */
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      if (i == (size / 2) - 1) /* if it's at the top of central square */
      {
        if (j == (size / 2) - 1) /* if it's at the left of central square */
        {
          board->board[i][j] = WHITE_DISC;
        } else if (j == (size / 2)) /* if it's at the right of central square */
        {
          board->board[i][j] = BLACK_DISC;
        }
      } else if (i == (size / 2)) /* if it's at the bottom of central square */
      {
        if (j == (size / 2) - 1) /* if it's at the left of central square */
        {
          board->board[i][j] = BLACK_DISC;
        } else if (j == (size / 2)) /* if it's at the right of central square */
        {
          board->board[i][j] = WHITE_DISC;
        }
      }
    }
  }
  return board;
}

/* perform a deep copy of the board structure */
board_t *board_copy(const board_t *board) {
  size_t size_copy = board->size;
  disc_t player_copy = board->player;
  board_t *board_copy = board_alloc(size_copy, player_copy);
  board_copy->board = board->board;
  return board_copy;
}

/* return size of the board */
size_t board_size(const board_t *board) { return board->size; }

/* get current player */
disc_t board_player(const board_t *board) { return board->player; }

/* set the current player */
void board_set_player(board_t *board, disc_t new_player) {
  board->player = new_player;
}

/* get the content of the square */
disc_t board_get(const board_t *board, const size_t row, const size_t column) {
  size_t size = board_size(board);
  if (row <= size && column <= size) {
    return board->board[row][column];
  }
  return EMPTY_DISC;
}

/* set the given disc at the given position */
void board_set(board_t *board, const disc_t disc, const size_t row,
               const size_t column) {
  size_t size = board_size(board);
  if (row <= size && column <= size) {
    board->board[row][column] = disc;
  }
}

/* return score of the given board */
score_t board_score(const board_t *board) {
  score_t score;
  score.black = 0;
  score.white = 0;
  disc_t current_disc;
  size_t size = board_size(board);
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      current_disc = board->board[i][j];
      if (current_disc == BLACK_DISC) {
        score.black++;
      } else if (current_disc == WHITE_DISC) {
        score.white++;
      }
    }
  }
  return score;
}

/* write on the file 'fd' the content of the given board */
int board_print(const board_t *board, FILE *fd) {
  int nbr_char = 0; /* number of printed characters */
  if (fd == NULL) {
    return -1;
  }
  size_t size = board_size(board);
  disc_t current_player = board_player(board);
  char *space_tampon = "  ";

  nbr_char += fprintf(fd, "\n%c player's turn.\n", current_player);
  nbr_char += fprintf(fd, "\n    ");
  char current_char = 'A';
  for (size_t i = 0; i < size; i++) {
    nbr_char += fprintf(fd, "%c ", (char)(current_char + i));
  }
  nbr_char += fprintf(fd, "\n");

  for (size_t i = 0; i < size; i++) /* write board */
  {
    if (i + 1 == 10) {
      space_tampon = " "; /* reduce two space to one space */
    }
    nbr_char += fprintf(fd, "%s%lu ", space_tampon, i + 1);
    for (size_t j = 0; j < size; j++) {
      nbr_char += fprintf(fd, "%c ", board->board[i][j]);
    }
    nbr_char += fprintf(fd, "\n");
  }
  nbr_char += fprintf(fd, "\n");
  score_t score = board_score(board);
  nbr_char +=
      fprintf(fd, "Score: 'X' = %d, 'O' = %d\n\n", score.black, score.white);
  return nbr_char;
}
