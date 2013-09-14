#include <stdlib.h>
#include <string.h>
#include "HashTable.h"

struct HashTable *me = NULL;

void clearTable();
Node *put(Board B);
Node *get(Board B);
int hash(Board B);
void clearNode(Node *N);
Node *makeNode(Board B);

HashTable *createHashTable(int capacity) {
	HashTable *T = NULL;

	DEBUG("IN");
	if (!me) {
		destroyHashTable(me);
	}

	T = (HashTable *)malloc(sizeof(HashTable));
	if (T) {
		memset(T, 0x00, sizeof(HashTable));
		if (capacity > HASHTABLE_MAXSIZE || capacity <= 0) {
			capacity = HASHTABLE_MAXSIZE;
		}

		T->table = (Node **)malloc(sizeof(Node *) * capacity);
		if (!(T->table)) {
			free(T);
			ERROR("table is NULL.");
			return NULL;
		}
		memset(T->table, 0x00, sizeof(Node *) * capacity);

		T->capacity = capacity;
		T->put = put;
		T->get = get;
		T->cleartable = clearTable;
	}
	DEBUG("OUT");
	me = T;
	return T;
}

void destroyHashTable(HashTable *T) {
	int i;
	DEBUG("IN");
	if (T) {
		if (T->table) {
			for (i=0;i<T->capacity;i++) {
				clearNode(T->table[i]);
			}
			free(T->table);
		}
		free(T);
		T = NULL;
	}
	DEBUG("OUT");
}

void clearTable() {
	int i = 0;
	DEBUG("IN");
	if (me) {
		if (me->table) {
			for (;i<me->capacity;i++) {
				clearNode(me->table[i]);
			}
			memset(me->table, 0x00, sizeof(Node *) * me->capacity);
		}
	}
	DEBUG("OUT");
}

int hash(Board B) {
	int i;
	long h = 0;
	DEBUG("IN");
	for(i=0; i<BOARD_CELLS; i++) {
		h = (h << 3) ^ B[i];
	}
	DEBUG("OUT");
	return (int)abs(h % (long)(me->capacity));
}

/* for clearHashTable and destroyHashTable */
void clearNode(Node *N) {
	if (N) {
		if (N->next) {
			clearNode(N->next);
		} else {
			if (N->data) {
				if (N->data->board) free(N->data->board);
				free(N->data);
			}
			free(N);
			N = NULL;
		}
	}
}

Node *makeNode(Board B) {
	NodeData *ND = NULL;
	Node *N = NULL;
	DEBUG("IN");
	N = malloc(sizeof(Node));
	DEBUG("node address is 0x%x", N);
	if (N) {
		memset(N, 0x00, sizeof(Node));
		ND = malloc(sizeof(NodeData));
		DEBUG("node data address is 0x%x", ND);
		if (!ND) {
			free(N);
			N = NULL;
			ERROR("can not alloc memo for node data.");
		} else {
			memset(ND, 0x00, sizeof(NodeData));
			ND->board = B;
			ND->prev = NULL;
			ND->next = NULL;
			N->data = ND;
		}
	} else {
		ERROR("can not alloc memo.");
	}
	DEBUG("OUT");
	return N;
}

/**
 * return the node which is inserted in the hash table.
 * null if there has the specified board in the table.
 */
Node *put(Board B) {
	int key = -1;
	Node *N = NULL;
	DEBUG("IN");
	if (!B)	WARN("board is null.");

	N = get(B);
	if (!N) {
		key = hash(B);
		DEBUG("hash key: %d", key);
		N = me->table[key];
		if (!N) {
			N = makeNode(B);
			me->table[key] = N;
		} else {
			while (N->next != NULL) {
				DEBUG("go next");
				N = N->next;
			}
			N->next = makeNode(B);
			N = N->next;
		}
	}
	DEBUG("OUT");
	return N;
}

/**
 *  get the node which contains the specified board.
 *  null if there has no such board.
 */
Node *get(Board B) {
	int key = -1;
	Node *N = NULL;

	DEBUG("IN");
	if (!B)	WARN("board is null.");

	key = hash(B);
	DEBUG("hash key: %d", key);
	N = me->table[key];
	while (N) {
		if (N->data && N->data->board) {
#if FALSE
			YIELD("board in table");
			printBoard(N->data->board);
			YIELD("board to compare");
			printBoard(B);
#endif
			if (memcmp(N->data->board, B, BOARD_CELLS) == 0) {
				DEBUG("Bingo!");
				break;	// bingo
			}
		} else {
			ERROR("data or board is NULL while node is not NULL");
		}
		if (N->next != NULL) {
			DEBUG("go next");
			N = N->next;
		} else {
			DEBUG("NULL next");
			N = NULL;	// no such board
			break;
		}
	}
	DEBUG("OUT");
	return N;
}
