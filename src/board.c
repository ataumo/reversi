#include "board.h"

/* Internal board_t structure (hidden from the outside) */
struct board_t
{
  size_t size;
  disc_t player;
  disc_t **board;
};

/* allocate memory needed to creat a board of size 'size' */
board_t *board_alloc(const size_t size, const disc_t player){
  board_t *board = malloc(sizeof(board_t));
  if (board==NULL) {
    return NULL;
  }
  board->size=size;
  board->player=player;
  board->board=malloc(2*sizeof(disc_t));
  if (board->board==NULL) {
    return NULL;
  }
  return board;
}

/* free memory allocated to hold the board */
void board_free(board_t *board){
  free(board->board);
  free(board); 
}
