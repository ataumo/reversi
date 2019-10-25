#include "board.h"

#include <err.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

/* Internal board_t structure (hidden from the outside) */
struct board_t {
  size_t size;
  disc_t player;
  bitboard_t black;
  bitboard_t white;
  bitboard_t moves;
  bitboard_t next_move;
};

/* set the bit to 1 at row, column on the bitboard */
/* row=[0,size-1] column=[0,size-1] */
static bitboard_t set_bitboard(const size_t size, const size_t row,
                               const size_t column) {
  bitboard_t new_bitboard = 1;
  int power = (size * row + column);
  return new_bitboard << power;
}

static bitboard_t shift_north(const size_t size, const bitboard_t bitboard) {
  return (bitboard >> size);
}

static bitboard_t shift_south(const size_t size, const bitboard_t bitboard) {
  bitboard_t bitboard_bottom;
  switch (size) {
  case 2:
    return (bitboard << size) & ~0x30;
  case 4:
    return (bitboard << size) & ~0xF0000;
  case 6:
    bitboard_bottom = 0x3F;
    return (bitboard << size) & ~(bitboard_bottom << 36);
  case 8:
    bitboard_bottom = 0xFF;
    return (bitboard << size) & ~(bitboard_bottom << 64);
  case 10:
    bitboard_bottom = 0x3FF;
    return (bitboard << size) & ~(bitboard_bottom << 100);
  }
  return -1;
}

static bitboard_t shift_west(const size_t size, const bitboard_t bitboard) {
  bitboard_t bitboard_right;
  switch (size) {
  case 2:
    return (bitboard >> 1) & ~(0xA);
  case 4:
    return (bitboard >> 1) & ~(0x8888);
  case 6:
    return (bitboard >> 1) & ~(0x820820820);
  case 8:
    return (bitboard >> 1) & ~(0x8080808080808080);
  case 10:
    bitboard_right = 0x2008020080200;
    return (bitboard >> 1) & ~((bitboard_right << 50) | bitboard_right);
  }
  return -1;
}

static bitboard_t shift_est(const size_t size, const bitboard_t bitboard) {
  bitboard_t bitboard_left;
  bitboard_t bitboard_bottom;
  switch (size) {
  case 2:
    return (bitboard << 1) & ~0x5 & ~0x30;
  case 4:
    return (bitboard << 1) & ~0x1111 & ~0xF0000;
  case 6:
    bitboard_bottom = 0x3F;
    return (bitboard << 1) & ~0x41041041 & ~(bitboard_bottom << 36);
  case 8:
    bitboard_bottom = 0xFF;
    return (bitboard << 1) & ~(0x101010101010101) & ~(bitboard_bottom << 64);
  case 10:
    bitboard_left = 0x10040100401;
    bitboard_bottom = 0x3FF;
    return (bitboard << 1) & ~((bitboard_left << 50) | bitboard_left) &
           ~(bitboard_bottom << 100);
  }
  return -1;
}

static bitboard_t shift_nw(const size_t size, const bitboard_t bitboard) {
  return shift_west(size, shift_north(size, bitboard));
}

static bitboard_t shift_ne(const size_t size, const bitboard_t bitboard) {
  return shift_est(size, shift_north(size, bitboard));
}

static bitboard_t shift_sw(const size_t size, const bitboard_t bitboard) {
  return shift_west(size, shift_south(size, bitboard));
}

static bitboard_t shift_se(const size_t size, const bitboard_t bitboard) {
  return shift_est(size, shift_south(size, bitboard));
}

/* array of shift functions */
static bitboard_t (*shift_func[SIZE_SHIFT_ARRAY])(const size_t size,
                                                  const bitboard_t bitboard) = {
    shift_north, shift_south, shift_west, shift_est,
    shift_nw,    shift_ne,    shift_sw,   shift_se};

/* compute all the possible moves */
static bitboard_t compute_moves(const size_t size, const bitboard_t player,
                                const bitboard_t opponent) {
  bitboard_t moves = 0;
  bitboard_t candidates;
  for (size_t i = 0; i < SIZE_SHIFT_ARRAY; i++) { /* for each directions */
    candidates = opponent & (shift_func[i](size, player));
    while (candidates != 0) {
      moves |= shift_func[i](size, candidates);
      candidates = opponent & shift_func[i](size, candidates);
    }
  }
  return moves & ~opponent & ~player;
}

board_t *board_alloc(const size_t size, const disc_t player) {
  board_t *board = malloc(sizeof(board_t));
  if (board == NULL) {
    fprintf(stderr, "board.c:board_alloc(): error: error of malloc board\n");
    return NULL;
  }
  board->size = size;
  board->player = player;
  board->black = 0;
  board->white = 0;
  board->moves = 0;
  board->next_move = 0;
  return board;
}

void board_free(board_t *board) {
  if (board != NULL) {
    free(board);
  }
}

board_t *board_init(const size_t size) {
  /* verification of size */
  if (size % 2 != 0 || size < MIN_BOARD_SIZE || size > MAX_BOARD_SIZE) {
    fprintf(stderr, "board.c:board_init(): error: error of size (%zu)\n", size);
    return NULL;
  }
  board_t *board = board_alloc(size, BLACK_DISC); /* creat a new void board */
  /* set white disc on the board */
  board->white = set_bitboard(size, (size / 2) - 1, (size / 2) - 1) |
                 set_bitboard(size, size / 2, size / 2);
  /* set black disc on the board */
  board->black = set_bitboard(size, (size / 2) - 1, size / 2) |
                 set_bitboard(size, size / 2, (size / 2) - 1);
  /* init possibles moves */
  board->moves = compute_moves(size, board->black, board->white);
  if (!board->moves) { /* if move is not possible */
    board->player = EMPTY_DISC;
  }
  return board;
}

board_t *board_copy(const board_t *board) {
  if (board == NULL) {
    return NULL;
  }
  size_t size_copy = board->size;     /* get size */
  disc_t player_copy = board->player; /* get player */
  board_t *board_copy = board_alloc(size_copy, player_copy);
  board_copy->black = board->black;
  board_copy->white = board->white;
  board_copy->moves = board->moves;
  board_copy->next_move = board->next_move;
  return board_copy;
}

size_t board_size(const board_t *board) { return board->size; }

disc_t board_player(const board_t *board) { return board->player; }

void board_set_player(board_t *board, disc_t new_player) {
  if (new_player != HINT_DISC) {
    board->player = new_player;
  } else {
    fprintf(stderr, "board.c:board_set_player(): error: you try to set player "
                    "with EMPTY_DISC\n");
  }
}

disc_t board_get(const board_t *board, const size_t row, const size_t column) {
  if (board != NULL) {
    size_t size = board->size;
    bitboard_t current_bitboard = set_bitboard(size, row, column);
    if ((board->moves) & current_bitboard) {
      return HINT_DISC;
    }
    if ((board->black) & current_bitboard) {
      return BLACK_DISC;
    }
    if ((board->white) & current_bitboard) {
      return WHITE_DISC;
    }
  }
  return EMPTY_DISC;
}

/* set the given disc at the given position */
void board_set(board_t *board, const disc_t disc, const size_t row,
               const size_t column) {
  if (board != NULL) {
    size_t size = board->size;
    if (row < size && column < size) {
      bitboard_t current_bitboard = set_bitboard(size, row, column);
      switch (disc) {
      case BLACK_DISC:
        board->black = (board->black) | current_bitboard;
        board->white = (board->white) & ~current_bitboard;
        break;
      case WHITE_DISC:
        board->white = (board->white) | current_bitboard;
        board->black = (board->black) & ~current_bitboard;
        break;
      case HINT_DISC:
        board->moves = (board->moves) | current_bitboard;
        break;
      case EMPTY_DISC:
        board->black = (board->black) & ~current_bitboard;
        board->white = (board->white) & ~current_bitboard;
        break;
      }
    }
    if (board->player == BLACK_DISC) {
      board->moves = compute_moves(size, board->black, board->white);
    } else {
      board->moves = compute_moves(size, board->white, board->black);
    }
  }
}

/* convert a bit sequence 64bit to 128bit */
bitboard_t _64_to_128(bitboard_t i) { return ((i << 64) | i); }

/* count the number of bits set to 1 */
static size_t bitboard_popcount(const bitboard_t bitboard) {
  bitboard_t i = bitboard;
  i = i - ((i >> 1) & _64_to_128(0x5555555555555555));
  i = (i & _64_to_128(0x3333333333333333)) +
      ((i >> 2) & _64_to_128(0x3333333333333333));
  return (((i + (i >> 4)) & _64_to_128(0x0F0F0F0F0F0F0F0F)) *
          _64_to_128(0x0101010101010101)) >>
         120;
}

score_t board_score(const board_t *board) {
  score_t score;
  score.black = bitboard_popcount(board->black);
  score.white = bitboard_popcount(board->white);
  return score;
}

size_t board_count_player_moves(board_t *board) {
  return bitboard_popcount(board->moves);
}

size_t board_count_opponent_moves(board_t *board) {
  if (board->player == BLACK_DISC) {
    return bitboard_popcount(
        compute_moves(board->size, board->white, board->black));
  } else {
    return bitboard_popcount(
        compute_moves(board->size, board->black, board->white));
  }
}

bool board_is_move_valid(const board_t *board, const move_t move) {
  bitboard_t bitboard_move = set_bitboard(board->size, move.row, move.column);
  return (board->moves) & bitboard_move;
}

/* find all trace until the current player */
static bitboard_t trace_move(board_t *board, const move_t move) {
  disc_t current_player = board->player;
  size_t size = board->size;
  bitboard_t player = board->black; /* init with BLACK_DISC player */
  bitboard_t opponent = board->white;
  if (current_player == WHITE_DISC) {
    player = board->white;
    opponent = board->black;
  }

  bitboard_t start = set_bitboard(size, move.row, move.column);
  bitboard_t trace;
  bitboard_t final_trace = 0;
  bitboard_t shift;

  for (size_t i = 0; i < 8; i++) {
    trace = start;
    shift = shift_func[i](size, start);
    while ((shift & opponent) != 0) {
      trace |= shift;
      shift = shift_func[i](size, shift);
      if (shift & player) {
        final_trace |= trace;
      }
    }
  }
  return final_trace;
}

bool board_play(board_t *board, const move_t move) {
  /* if move is not valid */
  if (!board_is_move_valid(board, move)) {
    return 0;
  }
  bitboard_t trace = trace_move(board, move); /* get the trace of direction */
  disc_t current_player = board->player;
  size_t size = board->size;
  if (current_player == BLACK_DISC) {
    /* BLACK_DISC player plays */
    board->white &= ~trace;
    board->black |= trace;
    board->player = WHITE_DISC;
    board->moves = compute_moves(size, board->white, board->black);
  } else {
    /* WHITE_DISC player plays */
    board->black &= ~trace;
    board->white |= trace;
    board->player = BLACK_DISC;
    board->moves = compute_moves(size, board->black, board->white);
  }
  board->next_move = board->moves;

  score_t score = board_score(board);
  if (score.black + score.white == size * size) {
    board->player = EMPTY_DISC;
  }
  return 1;
}

move_t board_next_move(board_t *board) {
  move_t move;
  move.row = 0;
  move.column = 0;
  int nbr_tz = 0; /* no of zeros from last to first occurence of one */

  if (board == NULL) {
    move.row = -1;
    move.column = -1;
    return move;
  }
  if (board->next_move == 0) {
    board->next_move = board->moves;
  }
  size_t size = board->size;                     /* get the size of board */
  bitboard_t possibles_moves = board->next_move; /* get possibles moves */
  if (size < 10) {
    /* if possible_moves is a 64bit number */
    nbr_tz = __builtin_ctzll(possibles_moves);
  } else {
    nbr_tz = bitboard_popcount((possibles_moves - 1) ^ possibles_moves) - 1;
  }
  board->next_move &= possibles_moves - 1; /* remove the position */
  move.row = nbr_tz / size;                /* compute row */
  move.column = nbr_tz % size;             /* compute column */
  return move;
}

int board_print(const board_t *board, FILE *fd) {
  if (fd == NULL || board == NULL) {
    return -1;
  }
  int nbr_char = 0;                      /* number of printed characters */
  size_t size = board->size;             /* get size */
  disc_t current_player = board->player; /* get player */
  score_t score = board_score(board);    /* set the score */
  char *space_tampon = "  ";             /* set the initial space */
  char current_char = 'A';               /* set the initial character */
  nbr_char += fprintf(fd, "\n%c player's turn.\n", current_player);
  nbr_char += fprintf(fd, "\n    ");

  for (size_t i = 0; i < size; i++) { /* write A B C D... */
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
      nbr_char += fprintf(fd, "%c ", board_get(board, i, j));
    }
    nbr_char += fprintf(fd, "\n");
  }
  nbr_char += fprintf(fd, "\n");
  nbr_char +=
      fprintf(fd, "Score: 'X' = %d, 'O' = %d\n\n", score.black, score.white);
  return nbr_char;
}
