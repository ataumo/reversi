#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>
#include <stdlib.h>

/* Min/Max with board */
#define MIN_BOARD_SIZE 2
#define MAX_BOARD_SIZE 10

/* Board discs */
typedef enum{
  BLACK_DISC = 'X',
  WHITE_DISC = 'O',
  EMPTY_DISC = '_',
  HINT_DISC = '*'
} disc_t;

/* A move in the reversi game */
typedef struct
{
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

/* allocate memory needed to creat a board of size 'size' */
board_t *board_alloc (const size_t size, const disc_t player);

/* free memory allocated to hold the board */
void board_free (board_t *board);

/* init all the squares of the board as a starting game */
board_t *board_init (const size_t size);

/* perform a deep copy of the board structure */
board_t *board_copy (const board_t *board);

/* return size of the board */
size_t board_size (const board_t *board);

/* set the current player */
void board_set_player (board_t *board, disc_t new_player);

/* set the given disc at the given position */
void board_set (board_t *board, const disc_t disc,
   const size_t row, const size_t column);

/* write on the file 'fd' the content of the given board */
int board_print (const board_t *board, FILE *fd);

#endif /* BOARD_H */
