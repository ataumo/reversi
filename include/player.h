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
 * chosen move depending on the given board.
 * return value between [0,size-1]
 * return (size,size) if no possible move exist */

/* return a move given by the user through stdin */
move_t human_player(board_t *board);

/* return a random move among the possible ones */
move_t random_player(board_t *board);

move_t simul_best_player(board_t *board);

/* minimax algorithm with naive heuristic */
move_t simul_minimax_player(board_t *board);
#define DEPTH_MINIMAX 6

/* alpha beta algorithm with naive heuristic */
move_t simul_alpha_beta_player(board_t *board);
#define DEPTH_ALPHABETA 6

/* alpha beta algorithm with clever heuristic */
move_t simul_alpha_beta_bis_player(board_t *board);
#define DEPTH_ALPHABETA_BIS 3

/* alpha beta fail soft algorithm with clever heuristic */
move_t simul_fail_soft_player(board_t *board);
#define DEPTH_FAILSOFT 5

#endif /* PLAYER_H */
