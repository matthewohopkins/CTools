#ifndef INC_QUEUE_H_
#define INC_QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

typedef struct QueueItem {
	void* Data;						// Generic pointer to node data.
	struct QueueItem* NextItem;		// Pointer to the next item in the queue (as memory may not be contiguous).
}Item;								// Element within the queue.
typedef struct QueueItem* q_next_handle_t;
typedef Item* q_item_handle_t;

typedef struct QueueList {
	uint32_t 	ItemCount;			// The number of items in the queue.
	size_t		MemSize;			// The memory size of each item in the queue.
	size_t		MaxSize;			// The maximum number of elements the queue can hold.
	Item*		Head;				// Pointer to the front of the queue.
	Item*		Tail;				// Pointer to the end of the queue.
	Item*		CurrentItem;		// Pointer enabling queue searching and adjustment.
}Queue;
typedef Queue*	q_handle_t;			// Handle for the queue structure.

q_handle_t q_Init(size_t allocSize, size_t maxSize);
int q_Enqueue(q_handle_t q, void* data);
void q_Dequeue(q_handle_t q, void* out);
void q_ClearHead(q_handle_t q);
void q_PeekHead(q_handle_t q, void* out);
void q_PeekTail(q_handle_t q, void* out);
void q_Clear(q_handle_t q);
int8_t q_Reverse(q_handle_t q);
void* q_FindItem(q_handle_t q, void* item, bool (fn)(const void*, const void*));
void q_UpdateItem(q_handle_t q, void* oldVal, void* newVal, void (*fn)(const void*, const void*));
void q_Dispose(q_handle_t q);
bool q_IsEmpty(q_handle_t q);
bool q_IsFull(q_handle_t q);
size_t q_GetCount(q_handle_t q);
#endif /* INC_QUEUE_H_ */