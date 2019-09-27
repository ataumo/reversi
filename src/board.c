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

/* init all the squares of the board as a starting game */
board_t *board_init(const size_t size){
  board_t *board = board_alloc(size,BLACK_DISC);
  disc_t value_of_disc=EMPTY_DISC;
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      if (i==(size/2)-1) {
        if (j==(size/2)-1) {
          value_of_disc=WHITE_DISC;
        }else if (j==(size/2)) {
          value_of_disc=BLACK_DISC;
        }
      }else if (i==(size/2)) {
        if (j==(size/2)-1) {
          value_of_disc=BLACK_DISC;
        }else if (j==(size/2)) {
          value_of_disc=WHITE_DISC;
        }
      }
      board->board[i][j]=value_of_disc;
      printf("%s\n", value_of_disc);
    }
  }
  return board;
}
