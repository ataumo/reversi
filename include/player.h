#ifndef PLAYER_H
#define PLAYER_H

#include <board.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* A player function 'move_t (*player_func) (board_t *)' returns a
 * chosen move depending on the given board. */

/* return a move given by the user through stdin */
move_t human_player(board_t *board);

/* return a random move among the possible ones
 * return value between [0,size-1]
 * return (size,size) if no possible move exist */
move_t random_player(board_t *board);

#endif /* PLAYER_H */
