#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "Klotski.h"

#define HASHTABLE_MAXSIZE	5000

typedef struct NodeData {
	Board		board;
	struct Node	*next;
	struct Node	*prev;
}NodeData;

typedef struct Node {
	NodeData	*data;
	struct Node	*next;
}Node;

typedef struct HashTable {
	Node **table;
	int capacity;
	Node *(*put)(Board board);
	Node *(*get)(Board board);
	void (*cleartable)();
}HashTable;

HashTable *createHashTable(int capacity);
void destroyHashTable(HashTable *table);

#endif /* HASHTABLE_H_ */
