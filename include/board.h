#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Min/Max with board */
#define MIN_BOARD_SIZE 2
#define MAX_BOARD_SIZE 10

/* Board discs */
typedef enum {
  BLACK_DISC = 'X',
  WHITE_DISC = 'O',
  EMPTY_DISC = '_',
  HINT_DISC = '*'
} disc_t;

/* A move in the reversi game */
typedef struct {
  size_t row;
  size_t column;
} move_t;

/* Store the score of game */
typedef struct {
  unsigned short black;
  unsigned short white;
} score_t;

/* Reversi board (forwards declaration to hide the implementation) */
typedef struct board_t board_t;
/* Base bitboard type */
typedef unsigned __int128 bitboard_t;

/* allocate memory needed to creat a board of size 'size' */
board_t *board_alloc(const size_t size, const disc_t player);

/* free memory allocated to hold the board */
void board_free(board_t *board);

/* init all the squares of the board as a starting game */
board_t *board_init(const size_t size);

/* perform a deep copy of the board structure */
board_t *board_copy(const board_t *board);

/* return size of the board */
size_t board_size(const board_t *board);

/* get current player */
disc_t board_player(const board_t *board);

/* return score of the given board */
score_t board_score(const board_t *board);

/* check if a move is valid */
bool board_is_move_valid(const board_t *board, const move_t move);

/* set the current player */
void board_set_player(board_t *board, disc_t new_player);

/* get the content of the square */
disc_t board_get(const board_t *board, const size_t row, const size_t column);

/* set the given disc at the given position */
void board_set(board_t *board, const disc_t disc, const size_t row,
               const size_t column);

/* write on the file 'fd' the content of the given board */
int board_print(const board_t *board, FILE *fd);

/* store the next possible move */
move_t board_next_move(board_t *board);

/* apply a move according to rules and set the board for next move */
bool board_play(board_t *board, const move_t move);

/* check if a move is valid */
bool board_is_move_valid(const board_t *board, const move_t move);

/* count the number of possible moves */
size_t board_count_player_moves(board_t *board);

/* count the number of bits set to 1 */
static size_t bitboard_popcount(const bitboard_t bitboard);

/* set the given disc at the given position */
void board_set(board_t *board, const disc_t disc, const size_t row,
               const size_t column);

/* compute all the possible moves */
static bitboard_t compute_moves(const size_t size, const bitboard_t player,
                                const bitboard_t opponent);

#endif /* BOARD_H */
