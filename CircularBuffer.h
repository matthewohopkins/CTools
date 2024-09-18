#ifndef INC_CIRCULARBUFFER_H_
#define INC_CIRCULARBUFFER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

typedef struct {
	uint32_t 	Time;
	float		Amplitude;
}DataPoint;

//Hidden circular buffer structure definition:
typedef struct {
	DataPoint* Buffer;
	size_t Head;
	size_t Tail;
	size_t Count;		//The number of items stored in the buffer.
	size_t Capacity;
	bool Full;
}circular_buffer_t;

//Handle:
typedef circular_buffer_t* cbuf_handle_t;
//Function pointer:
typedef void(*__operation_fn_t)(const void*, const void*);

cbuf_handle_t cbuf_Init(size_t size);
void cbuf_Reset(cbuf_handle_t buf);
bool cbuf_IsEmpty(cbuf_handle_t buf);
bool cbuf_IsFull(cbuf_handle_t buf);
void cbuf_Dispose(cbuf_handle_t buf);
void cbuf_DisposeOfBuffer(cbuf_handle_t buf);
size_t cbuf_Capacity(cbuf_handle_t buf);
size_t cbuf_Count(cbuf_handle_t buf);
void cbuf_Add(cbuf_handle_t buf, DataPoint* data);
int cbuf_Remove(cbuf_handle_t buf, DataPoint* data);
void cbuf_FlushValues(cbuf_handle_t buf, size_t n);
void cbuf_FlushAll(cbuf_handle_t buf);
void cbuf_Snapshot(cbuf_handle_t buf, DataPoint* out);
void cbuf_Access(cbuf_handle_t buf, size_t idx, DataPoint* out);
int cbuf_GetIndex(cbuf_handle_t buf, size_t targetIdx);
uint32_t cbuf_GetLatestTime(cbuf_handle_t buf);
void cbuf_Operate(cbuf_handle_t buf, __operation_fn_t _op, const void* arg);
#endif /* INC_CIRCULARBUFFER_H_ */