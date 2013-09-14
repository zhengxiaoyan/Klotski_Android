#ifndef KLOTSKI_H_
#define KLOTSKI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Define.h"

#define 	NB	(unsigned char)0				// no block in the cell
#define		SB	(unsigned char)1				// cell occupied by 1 block
#define		VG	(unsigned char)2				// cell occupied by horizontal 2-block
#define		HG	(unsigned char)3				// cell occupied by vertical 2-block
#define		CC	(unsigned char)4				// cell occupied by the 4-block

#define		BOARD_ROW			5
#define		BOARD_COL			4
#define		BOARD_CELLS			20				// board comprised of a [4*5] cells

typedef enum DIR {
	UP		= -BOARD_COL,
	RIGHT	= 1,
	DOWN	= BOARD_COL,
	LEFT	= -1
}DIR;

typedef unsigned char	*Board;					// playground -- the board, comprised of 4*5 cells
typedef struct Block {
	int x;
	int y;
}Block;											// chess

/* starting-boards definition */
typedef struct StartBoard {
	int				level;						// hardness level
	unsigned char	board[BOARD_CELLS];			// member number is same to BOARD_CELLS [4*5]
	unsigned char	*name;						// board name
}StartBoard;

/**
 * @param	StartBoard**		OUT
 * 			the start boards array for operating on. user has the responsibility to release it.
 * @return	int
 * 			the size of the start boards.
 */
int getStartBoards(StartBoard **sb);

/**
 * @return	BOOL
 * 			if memory can not be successfully allocated, FALSE; else TRUE.
 */
BOOL start();

/**
 * destroy the queue, hash table, user_board etc
 */
void end();

/**
 * give hint for next step.
 * @param	Board	IN
 * 			the starting board
 * @param	Board	OUT
 * 			the next board for resolving
 * @return	BOOL
 *			TRUE:	if it can be resolved.
 *			FALSE:	if it can not be resolved.
 */
BOOL hint(Board board, Board next);

/**
 * Try to move block according to the specified way.
 * @param	Board	board	IN/OUT
 * 			if the specified moving can be done, the in-coming board will be set as the out-going board.
 * @param	Block	block	IN
 * 			the block to be moved
 * @param	DIR		dir		IN
 * 			direction to move
 * @param	int		steps	IN
 * 			the distance moved along the direction
 * @return	if it can be moved in the way specified, TRUE; otherwise, FALSE.
 */
BOOL moveBlockTo(Board board, Block block, DIR dir, int steps);

/**
 * Start to analyze to see if game can be solved with the specified board.
 * If yes, return TRUE, with the specified board the starting board,
 * and the result boards array will be stored in the "result".
 *
 * @param:	Board board		IN
 * 			the starting board for analyzing.
 * @return:	the size of boards in the result.
 */
int doAnalysis(Board board);

/**
 * check if win.
 * @param	Board	IN
 * 			the board for check
 * @return	BOOL
 * 			TRUE if win; else FALSE.
 */
BOOL ifWin(Board board);

/**
 * if the solution was found, get the next board on the solution list.
 */
Board getNextBoard(Board board);

/* help function to print out the board */
void printBoard(Board board);

#ifdef __cplusplus
} //extern c
#endif

#endif /* KLOTSKI_H_ */
