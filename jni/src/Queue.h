#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdlib.h>
#include <string.h>
#include "Define.h"

#define	QUEUE_MAXSIZE	1000
#define ELEMENT_TYPE	struct Node *

#define FLAG_DELELEMENT	FALSE

// manipulate queue within this structure
typedef struct Queue {
	ELEMENT_TYPE	*elements;
	int				capacity;
	int				size;
	int				head;
	int				rear;
}Queue;

#define CREATEQUEUE(q, c)																\
		do{																				\
			DEBUG("CREATEQUEUE in");													\
			if(q) free(q); q=NULL;														\
			if (c <= 0) {																\
				ERROR("capacity should be bigger than 0.");								\
			} else {																	\
				q = (Queue *)malloc(sizeof(Queue));										\
				if (q) {																\
					q->capacity = c;													\
					q->size = 0;														\
					q->head = 0;														\
					q->rear = -1;														\
					q->elements = (ELEMENT_TYPE *)malloc (c * sizeof(ELEMENT_TYPE));	\
					memset(q->elements, 0x00, c * sizeof(ELEMENT_TYPE));				\
					DEBUG("Queue elements are allocated");								\
				} else {																\
					ERROR("Queue is null");												\
				}																		\
			}																			\
			if (q) {																	\
				if (!(q->elements)) {													\
					ERROR ("Failed to allocate for elements");							\
					free(q);															\
					q = NULL;															\
				} else {																\
					memset(q->elements, 0x00, c*sizeof(ELEMENT_TYPE));					\
				}																		\
			}																			\
			DEBUG("CREATEQUEUE out");													\
		}while(0);

#define DESTROYQUEUE(q, TYPE)															\
		do{																				\
			int de_element;																\
			char *type;																	\
			int i;																		\
			DEBUG("DESTROYQUEUE in");													\
			if (q) {																	\
				DEBUG("ELEMENT_TYPE is " #TYPE);										\
				de_element = FALSE;														\
				type = #TYPE;															\
				if (FLAG_DELELEMENT) {													\
					for (i=0;i<strlen(type);i++) {										\
						if (0==memcmp((type+i),"*",1)) {								\
							de_element = TRUE;											\
							DEBUG("de_element is TRUE");								\
							break;														\
						}																\
					}																	\
					if (de_element) {													\
						for (i=0;i<q->capacity;i++) {									\
							if (q->elements[i]) {free(q->elements[i]);}					\
						}																\
					}																	\
				}																		\
				free(q->elements);														\
				free(q); q=NULL;														\
			} else {																	\
				DEBUG("Queue is null");													\
			}																			\
			DEBUG("DESTROYQUEUE out");													\
		}while(0);

#define CLEARQUEUE(q, TYPE)																\
		do{																				\
			int de_element;																\
			char *type;																	\
			int i;																		\
			DEBUG("CLEARQUEUE in");														\
			if (FLAG_DELELEMENT && q) {													\
				DEBUG("ELEMENT_TYPE is " #TYPE);										\
				de_element = FALSE;														\
				type = #TYPE;															\
				for (i=0;i<strlen(type);i++) {											\
					if (0==memcmp((type+i),"*",1)) {									\
						de_element = TRUE;												\
						DEBUG("de_element is TRUE");									\
						break;															\
					}																	\
				}																		\
				if (de_element) {														\
					for (i=0;i<q->capacity;i++) {										\
						if (q->elements[i]) {free(q->elements[i]);}						\
					}																	\
				}																		\
			} else {																	\
				DEBUG("CLEARQUEUE: no need do clearing.");								\
			}																			\
			DEBUG("CLEARQUEUE out");													\
		}while(0);

#define ENQUEUE(q, element)																\
		do{																				\
			DEBUG("ENQUEUE in");														\
			if (!q) {																	\
				ERROR("ENQUEUE: queue is null");										\
				break;																	\
			}																			\
			if(q->size == q->capacity) {												\
				ERROR("ENQUEUE: queue is full");										\
				break;																	\
			}																			\
			q->rear ++;																	\
			if (q->rear == q->capacity) {												\
				q->rear = 0;															\
			}																			\
			q->elements[q->rear] = (ELEMENT_TYPE)element;								\
			DEBUG("ENQUEUE: element address is 0x%x", element);							\
			q->size ++;																	\
			DEBUG("ENQUEUE out, size is %d", q->size);															\
		}while(0);

#define DEQUEUE(q, TYPE)																\
		do{																				\
			int de_element;																\
			char *type;																	\
			int i;																		\
			DEBUG("DEQUEUE in");														\
			if (!q) {																	\
				ERROR("DEQUEUE: queue is null");										\
				break;																	\
			}																			\
			if(q->size == 0) {															\
				DEBUG("DEQUEUE: queue is empty");										\
				break;																	\
			}																			\
			if (FLAG_DELELEMENT) {														\
				de_element = FALSE;														\
				type = #TYPE;															\
				for (i=0;i<strlen(type);i++) {											\
					if (0==memcmp((type+i),"*",1)) {									\
						de_element = TRUE;												\
						DEBUG("de_element is TRUE");									\
						break;															\
					}																	\
				}																		\
				if (de_element) {														\
					DEBUG("DEQUEUE: free element at 0x%x", q->elements[q->head]);		\
					free(q->elements[q->head]);											\
					q->elements[q->head] = NULL;										\
				}																		\
			}																			\
			q->head ++;																	\
			DEBUG("DEQUEUE: head located at %d", q->head);								\
			if (q->head == q->capacity) {												\
				q->head = 0;															\
			}																			\
			q->size --;																	\
			DEBUG("DEQUEUE out, size is %d", q->size);									\
		}while(0);

#define PEEK(q, e)																		\
		do{																				\
			DEBUG("PEEK in");															\
			if (q->size <= 0) {															\
				ERROR("PEEK: queue has no element");									\
				e = NULL;																\
			} else {																	\
				e = (ELEMENT_TYPE)q->elements[q->head];									\
				DEBUG("PEEK: head located at %d", q->head);								\
				DEBUG("PEEK: element address is 0x%x", e);								\
			}																			\
			DEBUG("PEEK out");															\
		}while(0);

#define createQue(q,c)		CREATEQUEUE(q, c)
#define destroyQue(q)		DESTROYQUEUE(q, struct Node *)
#define clearQue(q)			CLEARQUEUE(q, struct Node *)
#define enQue(q, e)			ENQUEUE(q, e)
#define deQue(q)			DEQUEUE(q, struct Node *)
#define peek(q, e)			PEEK(q, e)

#endif /* QUEUE_H_ */
