#include "Queue.h"

//Initialise a generic queue:
q_handle_t q_Init(size_t allocSize, size_t maxSize) {
	//Allocate memory for the queue structure (only):
	q_handle_t q = malloc(sizeof(Queue));
	if (q == NULL) {
		return NULL;
	}
	//Initialise the queue structure:
	q->MemSize = allocSize;
	q->ItemCount = 0;
	q->MaxSize = maxSize;
	q->Head = q->Tail = NULL;
	return q;
}

//Add an item to the tail of the queue:
int q_Enqueue(q_handle_t q, void* data) {
	//If the queue is full, dispose of the item at the head of the queue:
	if (q->ItemCount >= q->MaxSize) {
		q_ClearHead(q);
	}
	//Add the new item to the tail of the queue:
	//Allocate memory for a new item structure:
	q_item_handle_t newItem = malloc(sizeof(Item));
	if (newItem == NULL) {
		return -1;
	}
	//Allocate memory for the data in the new item:
	newItem->Data = malloc(q->MemSize);
	if (newItem->Data == NULL) {
		free(newItem);
		return -1;
	}
	//Set the pointer to the next item in the queue to NULL (no item beyond this one):
	newItem->NextItem = NULL;
	//Copy data from the structure to be queued into the queue (i.e. data):
	memcpy(newItem->Data, data, q->MemSize);
	//Update the head and tail pointers:
	if (q->ItemCount == 0) {
		q->Head = q->Tail = newItem;
	}
	else {
		q->Tail->NextItem = newItem;
		q->Tail = newItem;				//Set the tail to the last item added.
	}
	q->ItemCount++;
	return 0;
}

//Remove an item from the head of the queue:
void q_Dequeue(q_handle_t q, void* out) {
	if (q->ItemCount > 0) {
		//Create a copy of the queue head item pointer (to free memory):
		q_item_handle_t temp = q->Head;
		//Copy the data into the structure provided by the output pointer:
		memcpy(out, q->Head->Data, q->MemSize);
		//Update the queue pointers:
		if (q->ItemCount > 1) {
			q->Head = q->Head->NextItem;
		}
		else {
			//The queue is empty:
			q->Head = NULL;
			q->Tail = NULL;
		}
		q->ItemCount--;
		//Free the memory used for the dequeued item:
		free(temp->Data);
		free(temp);
	}
}

void q_ClearHead(q_handle_t q) {
	if (q->ItemCount > 0) {
		//Create a copy of the queue head item pointer (to free memory):
		q_item_handle_t temp = q->Head;
		//Point the head to the next item:
		q->Head = q->Head->NextItem;
		//Free the memory used for the dequeued item:
		free(temp->Data);
		free(temp);
		q->ItemCount--;
	}
}

// Peek at the item at the head of the queue without removing it:
void q_PeekHead(q_handle_t q, void* out) {
	if (q->ItemCount > 0) {
		memcpy(out, q->Head->Data, q->MemSize);
	}
}

void q_PeekTail(q_handle_t q, void* out) {
	if (q->ItemCount > 0) {
		memcpy(out, q->Tail->Data, q->MemSize);
	}
}

//Remove all items from the queue:
void q_Clear(q_handle_t q) {
	// Create a copy of the queue head item pointer (to free memory):
	q_item_handle_t temp;
	if (q != NULL) {
		while (!q_IsEmpty(q)) {
			temp = q->Head;				// Set the temporary pointer to the object pointed to by the head pointer.
			q->Head = temp->NextItem;	// Update the item pointed to by the head pointer.
			free(temp->Data);			// Free the memory for the original item.
			free(temp);					// Free the memory for the pointer to the item.
			q->ItemCount--;
		}
		q->Head = q->Tail = NULL;
	}
}

// Reverse the order of the queue:
int8_t q_Reverse(q_handle_t q) {
	int8_t err_code = -1;
	if (q == NULL) {
		return err_code;
	}
	if (q_GetCount(q) == 0) {
		return err_code;
	}
	else {
		int i;
		q_next_handle_t* items = malloc(q->ItemCount * (sizeof(q_next_handle_t)));
		// Start at the head of the queue:
		q->CurrentItem = q->Head;
		// Check the array exists. Malloc may have returned null if there is insufficent memory.
		if (q != NULL) {
			if (items) {
				// Store the pointers to each queue item:
				for (i = 0; i < q->ItemCount; i++) {								
					items[i] = q->CurrentItem;
					// Move on to the next item:
					if (q->CurrentItem->NextItem != NULL) {
						q->CurrentItem = q->CurrentItem->NextItem;
					}
				}
				// Replace each queue item NextItem value:
				for (i = q->ItemCount - 1; i > 0; i--) {
					// Update the next item pointer:
					q->CurrentItem->NextItem = items[i-1];
					// Move on to the next item:
					q->CurrentItem = items[i-1];
				}
				// Flip the tail and head pointers and alter the tail NextItem pointer:
				q_item_handle_t temp = q->Head;
				q->Head = q->Tail;
				q->Tail = temp;
				q->Tail->NextItem = NULL;
				err_code = 0;
			}	
		}
		free(items);
	}
	return err_code;
}

void* q_FindItem(q_handle_t q, void* item, bool (fn)(const void*, const void*)) {
	int i;
	// Start at the head of the queue:
	q->CurrentItem = q->Head;				
	if (q != NULL) {
		for (i = 0; i < q->ItemCount; i++) {
			if (fn(q->CurrentItem->Data, item)) {
				return (q_item_handle_t)q->CurrentItem;
			}
			else {
				// Move on to the next item:
				q->CurrentItem = q->CurrentItem->NextItem;	
			}
		}
	}
	return NULL;
}

// Find and update a specific item based on its value:
void q_UpdateItem(q_handle_t q, void* oldVal, void* newVal, void (*fn)(const void*, const void*)) {
	q_item_handle_t position = q_FindItem(q, oldVal, (*fn));
	if (position != NULL) {
		memcpy(position->Data, newVal, q->MemSize);
	}
}

// Destroy the queue:
void q_Dispose(q_handle_t q) {
	q_Clear(q);
	free(q);
}

// Check if the queue is empty:
bool q_IsEmpty(q_handle_t q) {
	return q->ItemCount == 0 ? true : false;
}

// Check if the queue is full:
bool q_IsFull(q_handle_t q) {
	return q->ItemCount == q->MaxSize ? true : false;
}

// Get the current number of items stored in the queue:
size_t q_GetCount(q_handle_t q) {
	return q->ItemCount;
}
