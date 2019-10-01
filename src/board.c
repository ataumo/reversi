#include "board.h"

#include <string.h>

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
  disc_t **first_tab=malloc(size*sizeof(disc_t*));
  if (first_tab==NULL) {
    return NULL;
  }
  for (size_t i = 0; i < size; i++) {
    first_tab[i]=malloc(size*sizeof(disc_t));
    if (first_tab[i]==NULL) {
      return NULL;
    }
  }
  board->board=first_tab;
  board->size=size;
  board->player=player;
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
  disc_t value_of_disc;
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      value_of_disc=EMPTY_DISC;
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
    }
  }
  return board;
}

/* perform a deep copy of the board structure */
board_t *board_copy(const board_t *board){
  size_t size_copy = board->size;
  disc_t player_copy = board->player;
  board_t *board_copy=board_alloc(size_copy, player_copy);
  if (board_copy==NULL) {
    return NULL;
  }
  for (size_t i = 0; i < size_copy; i++) {
    for (size_t j = 0; j < size_copy; j++) {
      board_copy->board = board->board;
    }
  }
  return board_copy;
}

/* return size of the board */
size_t board_size(const board_t *board){
  return board->size;
}

/* get current player */
disc_t board_player(const board_t *board){
  return board->player;
}

/* set the current player */
void board_set_player(board_t *board, disc_t new_player){
  board->player=new_player;
}

/* get the content of the square */
disc_t board_get(const board_t *board, const size_t row, const size_t column){
  size_t size=board_size(board);
  if (row<=size && column<=size) {
    return board->board[row][column];
  }
  return EMPTY_DISC;
}

/* set the given disc at the given position */
void board_set(board_t *board, const disc_t disc, const size_t row, const size_t column){
  size_t size=board_size(board);
  if (row<=size && column<=size) {
    board->board[row][column]=disc;
  }
}

/* return score of the given board */
score_t board_score(const board_t *board){
  score_t score;
  score.black = 0;
  score.white = 0;
  disc_t current_disc;
  size_t size = board_size(board);
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      current_disc = board->board[i][j];
      if (current_disc == BLACK_DISC) {
        score.black += 1;
      }else if (current_disc == WHITE_DISC){
        score.white += 1;
      }
    }
  }
  return score;
}


/* write on the file 'fd' the content of the given board */
int board_print(const board_t *board, FILE *fd){
  if (fd == NULL) {
    return -1;
  }
  size_t size = board_size(board);
  disc_t current_player = board_player(board);
  char *space_tampon = "  ";

  fprintf(fd,"\n%c player's turn.\n", current_player);
  fprintf(fd,"\n    ");
  char current_char = 'A';
  for (size_t i = 0; i < size; i++) {
    fprintf(fd,"%c ", current_char+i);
  }
  fprintf(fd,"\n");

  for (size_t i = 0; i < size; i++) { /* write board */
    if (i+1==10) {
      space_tampon =" ";
    }
    fprintf(fd,"%s%d ",space_tampon, i+1);
    for (size_t j = 0; j < size; j++) {
      fprintf(fd,"%c ", board->board[i][j]);
    }
    fprintf(fd,"\n");
  }
  fprintf(fd,"\n");
  score_t score = board_score(board);
  fprintf(fd,"Score: 'X' = %d, 'O' = %d\n\n", score.black, score.white);
  return size*size;
}












//
