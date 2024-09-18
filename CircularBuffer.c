#include "circularBuffer.h"

// ----------------------------------------------------------------------------- //
// Function used to initialise a circular buffer.
// size_t size	- the number of Datapoint structures to be stored in the buffer.
//
cbuf_handle_t cbuf_Init(size_t size) {
	assert(size);	//Display an error if the input arguments are not supplied properly.
	//Allocate memory for the circular buffer structure:
	cbuf_handle_t cbuf = malloc(sizeof(circular_buffer_t));
	assert(cbuf);
	//Allocate memory for the buffer:
	cbuf->Buffer = malloc(sizeof(DataPoint) * size);
	cbuf->Capacity = size;
	cbuf_Reset(cbuf);
	assert(cbuf_IsEmpty(cbuf));
	return cbuf;
}

// ----------------------------------------------------------------------------- //
// Function used to reset the state of the circular buffer.
// cbuf_handle_t buf	- the handle of the target circular buffer.
//
void cbuf_Reset(cbuf_handle_t buf) {
	assert(buf);
	buf->Head = 0;
	buf->Tail = 0;
	buf->Count = 0;
	buf->Full = 0;
}

// ----------------------------------------------------------------------------- //
// Function used to check if the circular buffer is empty.
// cbuf_handle_t buf	- the handle of the target circular buffer.
//
bool cbuf_IsEmpty(cbuf_handle_t buf) {
	assert(buf);
	return (!buf->Full && (buf->Head == buf->Tail));
}

// ----------------------------------------------------------------------------- //
// Function used to if the circular buffer is full.
// cbuf_handle_t buf	- the handle of the target circular buffer.
//
bool cbuf_IsFull(cbuf_handle_t buf) {
	assert(buf);
	return buf->Full;
}

// ----------------------------------------------------------------------------- //
// Function used to dispose of the circular buffer.
// cbuf_handle_t buf	- the handle of the target circular buffer.
//
void cbuf_Dispose(cbuf_handle_t buf) {
	assert(buf);
	free(buf->Buffer);
	free(buf);
}

// ----------------------------------------------------------------------------- //
// Function used to dipose of the circular buffer's internal buffer.
// cbuf_handle_t buf	- the handle of the target circular buffer.
//
void cbuf_DisposeOfBuffer(cbuf_handle_t buf){
	assert(buf);
	free(buf->Buffer);
}

// ----------------------------------------------------------------------------- //
// Function used to check the capacity of the circular buffer.
// cbuf_handle_t buf	- the handle of the target circular buffer.
//
size_t cbuf_Capacity(cbuf_handle_t buf) {
	assert(buf);
	return buf->Capacity;
}

// ----------------------------------------------------------------------------- //
// Function used to check the number of elements currently stored in the circular buffer.
// cbuf_handle_t buf	- the handle of the target circular buffer.
//
size_t cbuf_Count(cbuf_handle_t buf) {
	assert(buf);
	return buf->Count;
}

// ----------------------------------------------------------------------------- //
// Function used to advance the internal element pointer in the circular buffer.
// cbuf_handle_t buf	- the handle of the target circular buffer.
//
static void _advancePointer(cbuf_handle_t buf) {
	assert(buf);
	//Increase the item stored count:
	if (buf->Count < buf->Capacity) {
		if (++(buf->Count) == buf->Capacity) {
			buf->Full = true;
		}
	}
	//If the head reaches the buffer maximum, reset it to 0:
	if (++(buf->Head) == buf->Capacity) {
		buf->Head = 0;
	}
	if (buf->Full) {
		//Increment the tail:
		buf->Tail = buf->Head;	//Head and tail indexes match (head will not be used until overwrite, i.e. next add, so value at that index is the eldest).
	}
}

// ----------------------------------------------------------------------------- //
// Function used to retract the internal element pointer in the circular buffer.
// cbuf_handle_t buf	- the handle of the target circular buffer.
//
static void _retreatPointer(cbuf_handle_t buf) {
	assert(buf);
	buf->Full = false;
	if (buf->Count > 0) {
		buf->Count--;
	}
	//Reset the head and tail if the count reaches 0:
	if (buf->Count == 0) {
		buf->Tail = 0;
		buf->Head = 0;
	}
	//Advance the tail as items are removed (reset when at max):
	if (++(buf->Tail) == buf->Capacity) {
		buf->Tail = 0;					//The tail denotes items that were added first.
	}
}

// ----------------------------------------------------------------------------- //
// Function used to add an element to the circular buffer.
// cbuf_handle_t buf	- the handle of the target circular buffer.
// DataPoint* data		- pointer to the incoming DataPoint.
//
void cbuf_Add(cbuf_handle_t buf, DataPoint* data) {
	assert(buf && buf->Buffer);
	buf->Buffer[buf->Head].Time = data->Time;
	buf->Buffer[buf->Head].Amplitude = data->Amplitude;
	_advancePointer(buf);			//Old data will be overwritten as the head pointer advances.
}

// ----------------------------------------------------------------------------- //
// Function used to remove an element from the circular buffer.
// cbuf_handle_t buf	- the handle of the target circular buffer.
// DataPoint* data		- pointer to the outgoing DataPoint.
//
int cbuf_Remove(cbuf_handle_t buf, DataPoint* data) {
	int error = -1;
	assert(buf && buf->Buffer);
	if (!cbuf_IsEmpty(buf)){
		data->Time = buf->Buffer[buf->Tail].Time;
		data->Amplitude = buf->Buffer[buf->Tail].Amplitude;
		_retreatPointer(buf);
		error = 0;
	}
	return error;
}


static void _flushRange(cbuf_handle_t buf, size_t n) {
	int i;
	for (i = 0; i < n; i++) {
		buf->Buffer[buf->Tail].Time = 0;
		buf->Buffer[buf->Tail].Amplitude = 0;
		_retreatPointer(buf);
	}
}
// ----------------------------------------------------------------------------- //
// Function used to flush a specified number of values from the circular buffer (eldest first).
// cbuf_handle_t buf	- the handle of the target circular buffer.
// size_t n				- the number of values to flush.
//
void cbuf_FlushValues(cbuf_handle_t buf, size_t n) {
	assert(buf && buf->Buffer);
	if (buf->Count >= n) {
		_flushRange(buf, n);
	}
	else {
		cbuf_FlushAll(buf);
	}
}

void cbuf_FlushAll(cbuf_handle_t buf) {
	_flushRange(buf, buf->Capacity);
}

// ----------------------------------------------------------------------------- //
// Function used to obtain the time value of the most recent DataPoint in the circular buffer .
// cbuf_handle_t buf	- the handle of the target circular buffer.
//
uint32_t cbuf_GetLatestTime(cbuf_handle_t buf) {
	assert(buf);
	if (!cbuf_IsEmpty(buf)) {
		if (buf->Head != 0) {
			return buf->Buffer[buf->Head - 1].Time;	//-1 to account for Head incrementing after a value is set.
		}
		else {
			return buf->Buffer[buf->Capacity - 1].Time;
		}
	}
	return 0;
}

// ----------------------------------------------------------------------------- //
// Function used to copy all values in the circular buffer without removing them.
// cbuf_handle_t buf	- the handle of the target circular buffer.
// DataPoint* data		- pointer to the outgoing DataPoint array.
//
void cbuf_Snapshot(cbuf_handle_t buf, DataPoint* out) {
	assert(buf && out);
	int i;
	if (buf->Full) {
		for (i = buf->Tail; i < buf->Capacity; i++) {
			//Copy data from the tail to capacity:
			out->Time = buf->Buffer[i].Time;
			out->Amplitude = buf->Buffer[i].Amplitude;
			out++;
		}
		//
		if (buf->Tail != 0) {
			for (i = 0; i < buf->Head; i++) {
				//Copy data from 0 to the head:
				out->Time = buf->Buffer[i].Time;
				out->Amplitude = buf->Buffer[i].Amplitude;
				out++;
			}
		}
	}
	else {
		//The buffer is partially full:
		if (buf->Head > buf->Tail) {
			for (i = buf->Tail; i < buf->Head; i++) {
				//Copy data:
				out->Time = buf->Buffer[i].Time;
				out->Amplitude = buf->Buffer[i].Amplitude;
				out++;
			}
		}
		else {
			for (i = buf->Tail; i < buf->Capacity; i++) {
				//Copy data:
				out->Time = buf->Buffer[i].Time;
				out->Amplitude = buf->Buffer[i].Amplitude;
				out++;
			}
			for (i = 0; i < buf->Head; i++) {
				//Copy data:
				out->Time = buf->Buffer[i].Time;
				out->Amplitude = buf->Buffer[i].Amplitude;
				out++;
			}
		}
	}
}

// ----------------------------------------------------------------------------- //
// Function used to access a certain value in the circular buffer without removing it.
// cbuf_handle_t buf	- the handle of the target circular buffer.
// size_t idx			- the index of the target Datapoint in the circular buffer.
// DataPoint* data		- pointer to the outgoing DataPoint.
//
void cbuf_Access(cbuf_handle_t buf, size_t idx, DataPoint* out) {
	assert(buf && out);
	if ((buf->Tail + idx) < buf->Capacity) {
		//Copy data:
		out->Time = buf->Buffer[(buf->Tail + idx)].Time;
		out->Amplitude = buf->Buffer[(buf->Tail + idx)].Amplitude;
	}
	else {
		//Copy data:
		out->Time = buf->Buffer[(idx - (buf->Capacity - buf->Tail))].Time;
		out->Amplitude = buf->Buffer[(idx - (buf->Capacity - buf->Tail))].Amplitude;
	}
}

// ----------------------------------------------------------------------------- //
// Function used to operate on the circular buffer according to the __operation_fn_t argument.
// cbuf_handle_t buf				- the handle of the target circular buffer.
// __operation_fn_t _op				- pointer to the operation function.
// arg								- an argument for the operation function.
//
void cbuf_Operate(cbuf_handle_t buf, __operation_fn_t _op, const void* arg) {
	for (int i = 0; i < buf->Capacity; i++) {
		_op(&buf->Buffer[i], arg);
	}
}

// ----------------------------------------------------------------------------- //
// Function used to get an iteration index of an item in the circular buffer based on the position of the tail.
// cbuf_handle_t buf	- the handle of the target circular buffer.
// size_t targetIdx		- the index of the target element in the circular buffer.
//
int cbuf_GetIndex(cbuf_handle_t buf, size_t targetIdx) {
	return ((targetIdx + buf->Tail) % buf->Capacity);
}