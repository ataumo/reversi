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
board_t *board_alloc(const size_t size, const disc_t player);

/* free memory allocated to hold the board */
void board_free(board_t *board);

#endif /* BOARD_H */
