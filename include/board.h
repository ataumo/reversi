#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Reversi board (forwards declaration to hide the implementation) */
typedef struct board_t board_t;

/* Min/Max with board */
#define MIN_BOARD_SIZE 2
#define MAX_BOARD_SIZE 10

/* Size of shift array */
#define SIZE_SHIFT_ARRAY 8

/* Popcount masks */
#define MASK1_POPCOUNT                                                         \
  (bitboard_t)(((bitboard_t)0x5555555555555555 << 64) | 0x5555555555555555)
#define MASK2_POPCOUNT                                                         \
  (bitboard_t)(((bitboard_t)0x3333333333333333 << 64) | 0x3333333333333333)
#define MASK3_POPCOUNT                                                         \
  (bitboard_t)(((bitboard_t)0x0F0F0F0F0F0F0F0F << 64) | 0x0F0F0F0F0F0F0F0F)
#define MASK4_POPCOUNT                                                         \
  (bitboard_t)(((bitboard_t)0x0101010101010101 << 64) | 0x0101010101010101)

/* Shifts masks */
#define MASK_BITB_BOTTOM_6 (bitboard_t)(((bitboard_t)0x3F << 36))
#define MASK_BITB_BOTTOM_8 (bitboard_t)(((bitboard_t)0xFF << 64))
#define MASK_BITB_BOTTOM_10 (bitboard_t)(((bitboard_t)0x3FF << 100))
#define MASK_BITB_RIGHT_10                                                     \
  (bitboard_t)(((bitboard_t)0x2008020080200 << 50) | 0x2008020080200)
#define MASK_BITB_LEFT_10                                                      \
  (bitboard_t)(((bitboard_t)0x10040100401 << 50) | 0x10040100401)

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

/** board_t type **/
/* allocate memory needed to creat a board of size 'size'
 * EXIT_FAILURE if something wrong happened */
board_t *board_alloc(const size_t size, const disc_t player);
/* init all the squares of the board as a starting game */
board_t *board_init(const size_t size);
/* perform a deep copy of the board structure */
board_t *board_copy(const board_t *board);

/** boolean type **/
/* check if a move is valid */
bool board_is_move_valid(const board_t *board, const move_t move);
/* apply a move according to rules and set the board for next move */
bool board_play(board_t *board, const move_t move);

/** disc_t type **/
/* get current player */
disc_t board_player(const board_t *board);
/* get the content of the square */
disc_t board_get(const board_t *board, const size_t row, const size_t column);

/** int type **/
/* write on the file 'fd' the content of the given board */
int board_print(const board_t *board, FILE *fd);

/** move_t type **/
/* store the next possible move
 * fail : return -1,-1 */
move_t board_next_move(board_t *board);

/** score_t type **/
/* return score of the given board */
score_t board_score(const board_t *board);

/** size_t type **/
/* count the number of possible moves */
size_t board_count_player_moves(board_t *board);
/* count the number of opponent's possible moves */
size_t board_count_opponent_moves(board_t *board);
/* return size of the board */
size_t board_size(const board_t *board);

/** void type **/
/* set the current player */
void board_set_player(board_t *board, disc_t new_player);
/* free memory allocated to hold the board */
void board_free(board_t *board);
/* set the given disc at the given position */
void board_set(board_t *board, const disc_t disc, const size_t row,
               const size_t column);

#endif /* BOARD_H */
