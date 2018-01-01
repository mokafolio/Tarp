#ifndef TARP_TARPSTACKBUFFER_H
#define TARP_TARPSTACKBUFFER_H

#include <Tarp/TarpMeta.h>

#endif //TARP_TARPSTACKBUFFER_H

typedef struct
{
    _TARP_STACK_BUFFER_ITEM_T buffer[_TARP_STACK_BUFFER_SIZE];
    int count;
}
_TARP_STACK_BUFFER_T;

int _TARP_FN(_TARP_STACK_BUFFER_T, Init)(_TARP_STACK_BUFFER_T * _buffer)
{
    assert(_buffer);
    _buffer->count = 0;
    return 0;
}

int _TARP_FN(_TARP_STACK_BUFFER_T, Append)(_TARP_STACK_BUFFER_T * _buffer, _TARP_STACK_BUFFER_ITEM_T _val)
{
    assert(_buffer);
    _buffer->buffer[_buffer->count++] = _val;
    return 0;
}

int _TARP_FN(_TARP_STACK_BUFFER_T, Clear)(_TARP_STACK_BUFFER_T * _buffer)
{
	assert(_buffer);
    _buffer->count = 0;
    return 0;
}

_TARP_STACK_BUFFER_ITEM_T * _TARP_FN(_TARP_STACK_BUFFER_T, Ptr)(_TARP_STACK_BUFFER_T * _buffer)
{
	assert(_buffer);
	return _buffer->buffer;
}

int _TARP_FN(_TARP_STACK_BUFFER_T, Capacity)(_TARP_STACK_BUFFER_T * _buffer)
{
	return sizeof(_buffer->buffer) / sizeof(_TARP_STACK_BUFFER_ITEM_T);
}

#undef _TARP_STACK_BUFFER_T
#undef _TARP_STACK_BUFFER_SIZE
#undef _TARP_STACK_BUFFER_ITEM_T
