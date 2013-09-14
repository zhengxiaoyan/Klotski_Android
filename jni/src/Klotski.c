#include <string.h>
#include "Klotski.h"
#include "HashTable.h"
#include "Queue.h"

#define	CEL_RND_BLNK	8

Queue				*que				= NULL;
HashTable			*ht					= NULL;
static const StartBoard	boards_for_start[]	= {
	{1, {VG,CC,CC,VG,VG,CC,CC,VG,VG,HG,HG,VG,VG,SB,SB,VG,SB,NB,NB,SB}, (unsigned char*)"Yue_Ma_Hen_Dao"}
};

struct _cl_rnd_blnk {
	int	loc;
	int	dir;
}clrndblnk[CEL_RND_BLNK];

BOOL initMemo();
void clearMemo();
void releaseMemo();
int  bfs (Board board);
BOOL isVisited(Board board);
BOOL tryMoveBlock(Board inboard, Board outboard, Block block, DIR dir, int steps, BOOL apexonly, BOOL settomemo, Node *prenode);
void setBoardtoMove(Board inboard, Board outboard, int loc_apex, int block_type, DIR dir, int steps, BOOL settomemo, Node *prenode);

/************************ API *****************************/
int getStartBoards(StartBoard **sbs) {
	int i, size = sizeof(boards_for_start) / sizeof(StartBoard);
	DEBUG("IN");
	if (sbs == NULL) {
		ERROR("parameter is NULL");
		return 0;
	}
	for (i=0; i<size; i++) {
		sbs[i] = (StartBoard *)malloc(sizeof(StartBoard));
		memset(sbs[i], 0x00, sizeof(StartBoard));
		memcpy(sbs[i], boards_for_start, sizeof(StartBoard));
	}

	DEBUG("OUT");
	return size;
}

BOOL start() {
	DEBUG("IN");
	BOOL ret = TRUE;

	/* prepare memory for analysis (which might be for hint or analyzing) */
	ret = initMemo();
	if (!ret) {
		ERROR("initializing memory failed");
		ret = FALSE;
	}
	DEBUG("OUT");
	return ret;
}

void end() {
	DEBUG("IN");
	releaseMemo();
	DEBUG("OUT");
}

BOOL hint(Board board, Board next) {
	DEBUG("IN");
	// TODO:
	DEBUG("OUT");
	return FALSE;
}

BOOL moveBlockTo(Board board, Block block, DIR dir, int steps) {
	DEBUG("IN");
	return tryMoveBlock(board,board,block,dir,steps,FALSE, FALSE, NULL);
}

int doAnalysis(Board board) {
	int ret = -1;
	DEBUG("IN");

	if (board == NULL) {
		ERROR("The board for analyzing or the memory to store result boards is NULL.");
		return ret;
	}

	if (que == NULL || ht == NULL) {
		WARN("queue or hashtable is/are NULL.");
		if(initMemo()) {
			ERROR("initializing memory failed.");
			return ret;
		}
	} else {
		clearMemo();
	}

	ret = bfs(board);

	DEBUG("OUT");
	return ret;
}

BOOL ifWin(Board board) {
	const int win_apex = 13;
	DEBUG("IN");
	if (
		board[win_apex]					== (unsigned char)CC	&&
		board[win_apex + RIGHT + DOWN]	== (unsigned char)CC
	)
	{
		DEBUG("OK, you win.");
		return TRUE;
	}
	DEBUG("OUT");
	return FALSE;
}

Board getNextBoard(Board board) {
	Board ret = NULL;
	Node *node = NULL;
	if (ht == NULL) {
		ERROR("the hash table is NULL");
		return ret;
	}

	node = ht->get(board);
	if (node && node->data && node->data->next && node->data->next->data) {
		ret = node->data->next->data->board;
	}

	return ret;
}

void printBoard(Board board) {
	int i, j;
	for (i=0; i<BOARD_ROW; i++) {
		DRAWU;
		for (j=0; j<BOARD_COL; j++) {
			DRAWL;
			DRAWC(board[i*BOARD_COL + j]);
		}
		DRAWL;
	}
	DRAWU;
}

/*********************************Private************************************/

/* initialize queue, hash table, user_board etc TRUE if success */
BOOL initMemo() {
	DEBUG("IN");

	if (!que || !ht) {
		end();
	}

	createQue(que, QUEUE_MAXSIZE);
	if (!que) {
		ERROR("create queue failed");
		return FALSE;
	}

	ht	= createHashTable(HASHTABLE_MAXSIZE);
	if (!ht) {
		ERROR("create hash table failed");
		destroyQue(que);
		return FALSE;
	}

	DEBUG("OUT");
	return TRUE;
}

/* clear the queue, hash table, to prepare for the next game */
void clearMemo() {
	DEBUG("IN");

	clearQue(que);
	ht->cleartable(ht);

	DEBUG("OUT");
}

void releaseMemo() {
	DEBUG("IN");
	if (que) {destroyQue(que); que = NULL;}
	if (ht) {destroyHashTable(ht); ht = NULL;}
	DEBUG("OUT");
}

/**
 *  breadth first search, if ends with exit found, return steps.
 *  else -1
 */
int bfs (Board board) {
	Node *candidate = NULL;
	int ret = -1;
	int i,j;
	Block bl;
	Board test_bo = NULL, ret_bo = NULL;
	int idx = -1, ridx = -1;

	DEBUG("IN");

	if (board == NULL) {
		ERROR("the board should not be NULL");
		return ret;
	}

	if (!ht || !que) {
		ERROR("hash table or queue should not be NULL.");
		return ret;
	}

	clearMemo();
	candidate = ht->put(board);
	enQue(que, candidate);

	ret_bo = malloc(BOARD_CELLS);
	memset(ret_bo, 0x00, BOARD_CELLS);

	while (que->size > 0) {
		peek(que, candidate);
		if (candidate != NULL) {
			test_bo = candidate->data->board;
			if (test_bo == NULL) {
				WARN("there has no board in candidate node.");
				continue;
			}
#if FALSE
			YIELD("peek board");
			printBoard(test_bo);
#endif

			if (TRUE) {
				idx = (int)((unsigned char *)index((const char*)test_bo, NB) - test_bo);
				ridx = (int)((unsigned char *)index((const char *)test_bo + idx + 1, NB) - test_bo);

				if (idx < 0 || idx >= BOARD_CELLS ||
					ridx < 0 || ridx >= BOARD_CELLS) {
					ERROR("index went out of range: idx = %d, ridx = %d", idx, ridx);
					return ret;
				}

				memset(clrndblnk, 0x00, sizeof(clrndblnk) * CEL_RND_BLNK);

				if (idx+UP>= 0 && idx+UP<BOARD_CELLS) {
					clrndblnk[0].loc = idx + UP;
					clrndblnk[0].dir = DOWN;
				} else {
					clrndblnk[0].loc = -1;
				}
				if (idx+RIGHT>= 0 && idx+RIGHT<BOARD_CELLS) {
					clrndblnk[1].loc = idx + RIGHT;
					clrndblnk[1].dir = LEFT;
				}
				if (idx+DOWN>= 0 && idx+DOWN<BOARD_CELLS) {
					clrndblnk[2].loc = idx + DOWN;
					clrndblnk[2].dir = UP;
				}
				if (idx+LEFT>= 0 && idx+LEFT<BOARD_CELLS) {
					clrndblnk[3].loc = idx + LEFT;
					clrndblnk[3].dir = RIGHT;
				}
				if (ridx+UP>= 0 && ridx+UP<BOARD_CELLS) {
					clrndblnk[4].loc = ridx + UP;
					clrndblnk[4].dir = DOWN;
				}
				if (ridx+RIGHT>= 0 && ridx+RIGHT<BOARD_CELLS) {
					clrndblnk[5].loc = ridx + RIGHT;
					clrndblnk[5].dir = LEFT;
				}
				if (ridx+DOWN>= 0 && ridx+DOWN<BOARD_CELLS) {
					clrndblnk[6].loc = ridx + DOWN;
					clrndblnk[6].dir = UP;
				}
				if (ridx+LEFT>= 0 && ridx+LEFT<BOARD_CELLS) {
					clrndblnk[7].loc = ridx + LEFT;
					clrndblnk[7].dir = RIGHT;
				}

				for (i=0; i<CEL_RND_BLNK; i++) {
					bl.x = clrndblnk[i].loc / BOARD_COL;
					bl.y = clrndblnk[i].loc % BOARD_COL;

					if (tryMoveBlock(test_bo, ret_bo, bl, clrndblnk[i].dir, 1, FALSE, TRUE, candidate))
					{
						if (ifWin(ret_bo)) {
							ret = 0;
							break;
						}
					}

					if (tryMoveBlock(test_bo, ret_bo, bl, clrndblnk[i].dir, 2, FALSE, TRUE, candidate))
					{
						if (ifWin(ret_bo)) {
							ret = 0;
							break;
						}
					}
				}
			} else {
				for (i=0; i<BOARD_ROW; i++) {
					for (j=0;j<BOARD_COL;j++) {
						DEBUG("try move block %d,%d", i,j);
						bl.x = i;
						bl.y = j;

						if (tryMoveBlock(test_bo, ret_bo, bl, UP, 1, TRUE, TRUE, candidate)) {
							DEBUG("can move up.");
							if (ifWin(ret_bo)) {
								ret = 0;
								break;
							}
						}

						if (tryMoveBlock(test_bo, ret_bo, bl, RIGHT, 1, TRUE, TRUE, candidate)) {
							DEBUG("can move right.");
							if (ifWin(ret_bo)) {
								ret = 0;
								break;
							}
						}

						if (tryMoveBlock(test_bo, ret_bo, bl, DOWN, 1, TRUE, TRUE, candidate)) {
							DEBUG("can move down.");
							if (ifWin(ret_bo)) {
								ret = 0;
								break;
							}
						}

						if (tryMoveBlock(test_bo, ret_bo, bl, LEFT, 1, TRUE, TRUE, candidate)) {
							DEBUG("can move left.");
							if (ifWin(ret_bo)) {
								ret = 0;
								break;
							}
						}
					}
					if (ret == 0) break;
				} // end for
			} // end if(idx)

			if(ret == 0) break;
		} else {
			WARN("got NULL candidate.");
		} // end if(candidate)

		if (ret == 0) break;
		deQue(que);
	} // end while

	if (ret == 0) {
		ret = 1;
		Node *node = ht->get(ret_bo);
		while (node->data->prev) {
			node->data->prev->data->next = node;
			node = node->data->prev;
			ret ++;
		}
	}

	if (!ret_bo) free(ret_bo);

	DEBUG("out");
	return ret;
}

/**
 * if the specified board is in the visited boards hash table, return TRUE
 * else FALSE
 */
BOOL isVisited(Board board) {
	Node	*node = NULL;

	DEBUG("IN");
	if (!ht) {
		ERROR("hash table should not be null.");
		return FALSE;
	}

	node = ht->get(board);
	DEBUG("node address is 0x%x", node);
	if (node != NULL) {
		DEBUG("yes, visited");
		return TRUE;
	} else {
		DEBUG("no, not visited");
		return FALSE;
	}
}

BOOL tryMoveBlock(Board inboard, Board outboard, Block block, DIR dir, int steps, BOOL apexonly, BOOL settomemo, Node *prenode) {
	BOOL ret = FALSE;
	unsigned char block_type;
	int loc;
	int loc_apex;
	int i;
	int next_apex;

	DEBUG("IN");
	if (
		inboard == NULL	|| outboard == NULL		||
		block.x < 0		|| block.x >= BOARD_ROW	||
		block.y < 0		|| block.y >= BOARD_COL	||
		steps <= 0
		)
	{
		ERROR("there has invalid parameter(s).");
		return ret;
	}

	loc = block.x * BOARD_COL + block.y;
	block_type = (unsigned char)inboard[loc];
	switch(block_type) {
	case SB:
	case VG:
	case HG:
	case CC:
		break;
	default:
		return ret;
	}

	// set apex
	{
		int loc_tmp;
		int i;

		// Initialize apex. It applies to all types, including NB.
		loc_apex = loc;

		if (block_type == VG || block_type == CC) {
			loc_tmp = loc_apex;
			i = 0;
			while ((loc_tmp + UP) >= 0) {
				loc_tmp = loc_tmp + UP;
				if (inboard[loc_tmp] == block_type) {
					i++;
				} else {
					break;
				}
			}
			/* set apex of the VG to the tmp_loc */
			if (i % 2 != 0) {	// this is not the apex
				loc_apex = loc_apex + UP;	// set tmp_loc to apex
			}
		}

		if (block_type == HG || block_type == CC) {
			loc_tmp = loc_apex;
			i = 0;
			while ((loc_tmp + LEFT) >= 0 && (loc_tmp + LEFT)%BOARD_COL < loc_tmp % BOARD_COL) {
				loc_tmp = loc_tmp + LEFT;
				if (inboard[loc_tmp] == block_type) {
					i++;
				} else {
					break;
				}
			}
			/* set apex */
			if (i % 2 != 0) {	// this is not the apex
				loc_apex = loc_apex + LEFT;	// set tmp_loc to apex
			}
		}
	}

	if (apexonly && loc_apex != loc) {
		DEBUG("do not process for block who is not an apex");
		return ret;
	}

	for(i=1;i<=steps;i++) {
		next_apex = loc_apex + dir * i;
		if (next_apex >= 0 && next_apex < BOARD_CELLS) {
			ret = TRUE;
		} else {
			ret = FALSE;
			break;
		}

		switch(dir) {
		case UP:
			if (inboard[next_apex] != NB) {
				ret = FALSE;
			} else if ((block_type == HG || block_type == CC)	&&
					(next_apex + RIGHT >= BOARD_CELLS 			||
					inboard[next_apex + RIGHT] != NB))
			{
				ret = FALSE;
			}
			break;
		case DOWN:
			if (block_type == SB								&&
				inboard[next_apex] != NB)
			{
				ret = FALSE;
			} else if (block_type == VG							&&
				(next_apex + DOWN > BOARD_CELLS					||
				inboard[next_apex + DOWN] != NB))
			{
				ret = FALSE;
			} else if(block_type == HG							&&
					(next_apex + RIGHT >= BOARD_CELLS			||
					inboard[next_apex] != NB					||
					inboard[next_apex + RIGHT] != NB))
			{
				ret = FALSE;
			} else if (block_type == CC 						&&
					(next_apex + DOWN + RIGHT >= BOARD_CELLS 	||
					inboard[next_apex + DOWN] != NB				||
					inboard[next_apex + DOWN + RIGHT] != NB))
			{
				ret = FALSE;
			}
			break;
		case LEFT:
			if (inboard[next_apex] != NB						||
				next_apex % BOARD_COL > (next_apex + RIGHT) % BOARD_COL)
			{
				ret = FALSE;
			} else if ((block_type == VG || block_type == CC)	&&
					(next_apex + DOWN > BOARD_CELLS 			||
					inboard[next_apex + DOWN] != NB))
			{
				ret = FALSE;
			}
			break;
		case RIGHT:
			if ((next_apex + LEFT) % BOARD_COL > next_apex % BOARD_COL) {
				ret = FALSE;
			} else if (block_type == SB							&&
					(inboard[next_apex] != NB))
			{
				ret = FALSE;
			} else if (block_type == VG							&&
					(next_apex + DOWN > BOARD_CELLS 			||
					inboard[next_apex] != NB 					||
					inboard[next_apex + DOWN] != NB))
			{
				ret = FALSE;
			} else if (block_type == HG							&&
					(next_apex + RIGHT > BOARD_CELLS			||
					inboard[next_apex + RIGHT] != NB			||
					next_apex % BOARD_COL > (next_apex + RIGHT) % BOARD_COL))
			{
				ret = FALSE;
			} else if (block_type == CC							&&
					(next_apex + RIGHT + DOWN > BOARD_CELLS		||
					inboard[next_apex + RIGHT] != NB			||
					inboard[next_apex + RIGHT + DOWN] != NB		||
					next_apex % BOARD_COL > (next_apex + RIGHT) % BOARD_COL))
			{
				ret = FALSE;
			}
			break;
		default:
			break;
		}

		if (!ret) {
			DEBUG("can not be moved through this way: %d", dir);
			break;
		}
	}

	if (ret) {
		setBoardtoMove(inboard, outboard, loc_apex, block_type, dir, steps, settomemo, prenode);
	}

	DEBUG("OUT");
	return ret;
}

void setBoardtoMove(Board inboard, Board outboard, int loc_apex, int block_type, DIR dir, int steps, BOOL settomemo, Node *prenode) {
	Node *node = NULL;
	Board board = NULL;

	DEBUG("IN");
	if (settomemo) {
		board = (Board)malloc(BOARD_CELLS);
		memcpy(board, inboard, BOARD_CELLS);
	} else {
		board = outboard;
	}

	switch(block_type) {
	case SB:
		board[loc_apex] = NB;
		board[loc_apex + dir * steps] = block_type;
		break;
	case VG:
		board[loc_apex] = NB;
		board[loc_apex + DOWN] = NB;
		board[loc_apex + dir * steps] = block_type;
		board[loc_apex + dir * steps + DOWN] = block_type;
		break;
	case HG:
		board[loc_apex] = NB;
		board[loc_apex + RIGHT] = NB;
		board[loc_apex + dir * steps] = block_type;
		board[loc_apex + dir * steps + RIGHT] = block_type;
		break;
	case CC:
		board[loc_apex] = NB;
		board[loc_apex + RIGHT] = NB;
		board[loc_apex + DOWN] = NB;
		board[loc_apex + DOWN + RIGHT] = NB;
		board[loc_apex + dir * steps] = block_type;
		board[loc_apex + dir * steps + RIGHT] = block_type;
		board[loc_apex + dir * steps + DOWN] = block_type;
		board[loc_apex + dir * steps + DOWN + RIGHT] = block_type;
		break;
	default:
		ERROR("invalid block_type");
		break;
	}

	if (settomemo && !isVisited(board)) {
		memcpy(outboard, board, BOARD_CELLS);
		node = ht->put(board);
		node->data->prev = prenode;
		enQue(que, node);
#if FALSE
		YIELD("move to board");
		printBoard(board);
#endif
	}

	DEBUG("OUT");
}
