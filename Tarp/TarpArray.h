#ifndef TARP_TARPARRAY_H
#define TARP_TARPARRAY_H

#include <Tarp/TarpMeta.h>

#endif //TARP_TARPARRAY_H

typedef struct
{
    _TARP_ITEM_T * array;
    int count;
    int capacity;
} _TARP_ARRAY_T;

int _TARP_FN(_TARP_ARRAY_T, Reserve)(_TARP_ARRAY_T * _array, int _capacity)
{
    assert(_capacity > 0);
    _TARP_ITEM_T * mem;
    if (_array->capacity == 0)
    {
        mem = malloc(sizeof(_TARP_ITEM_T) * _capacity);
    }
    else
    {
        mem = realloc(_array->array, sizeof(_TARP_ITEM_T) * _capacity);
    }
    if (mem)
    {
        _array->array = mem;
        _array->capacity = _capacity;
        return 0;
    }
    return 1;
}
int _TARP_FN(_TARP_ARRAY_T, Init)(_TARP_ARRAY_T * _array, int _capacity)
{
    assert(_array);
    _array->capacity = 0;
    _array->count = 0;
    return _TARP_FN(_TARP_ARRAY_T, Reserve)(_array, _capacity);
}
void _TARP_FN(_TARP_ARRAY_T, Deallocate)(_TARP_ARRAY_T * _array)
{
    assert(_array);
    if (_array->array)
    {
        free(_array->array);
        _array->count = 0;
        _array->capacity = 0;
    }
}
void _TARP_FN(_TARP_ARRAY_T, Clear)(_TARP_ARRAY_T * _array)
{
    assert(_array);
    _array->count = 0;
}
int _TARP_FN(_TARP_ARRAY_T, Append)(_TARP_ARRAY_T * _array, _TARP_ITEM_T _value)
{
    assert(_array);
    if (_array->capacity - _array->count < 1)
    {
        int res = _TARP_FN(_TARP_ARRAY_T, Reserve)(_array, _array->capacity ? _array->capacity * 2 : 2);
        if (res) return res;
    }
    _array->array[_array->count++] = _value;
    return 0;
}
int _TARP_FN(_TARP_ARRAY_T, AppendPtr)(_TARP_ARRAY_T * _array, const _TARP_ITEM_T * _value)
{
    assert(_array);
    if (_array->capacity - _array->count < 1)
    {
        int res = _TARP_FN(_TARP_ARRAY_T, Reserve)(_array, _array->capacity ? _array->capacity * 2 : 2);
        if (res) return res;
    }
    _array->array[_array->count++] = *(_TARP_ITEM_T *)(_value);
    return 0;
}
int _TARP_FN(_TARP_ARRAY_T, AppendArray)(_TARP_ARRAY_T * _array, _TARP_ITEM_T * _values, int _count)
{
    assert(_array);
    assert(_values);
    assert(_count);
    if (_array->capacity - _array->count < _count)
    {
        int c = _count > _array->capacity ? _count * 2 : _array->capacity * 2;
        int res = _TARP_FN(_TARP_ARRAY_T, Reserve)(_array, c);
        if (res) return res;
    }
    memcpy(_array->array + _array->count, _values, _count * sizeof(_TARP_ITEM_T));
    _array->count += _count;
    return 0;
}
int _TARP_FN(_TARP_ARRAY_T, Remove)(_TARP_ARRAY_T * _array, int _index)
{
    assert(_array);
    assert(_index >= 0 && _index < _array->count);
    for (int i = _index; i < _array->count - 1; ++i)
    {
        _array->array[i] = _array->array[i + 1];
    }
    _array->count--;
    return 0;
}
int _TARP_FN(_TARP_ARRAY_T, RemoveRange)(_TARP_ARRAY_T * _array, int _from, int _to)
{
    assert(_array);
    assert(_from >= 0);
    assert(_to > _from);
    assert(_to <= _array->count);
    if (_to >= _array->count)
    {
        _array->count = _from;
    }
    else
    {
        int dist = _to - _from;
        int start = _to;
        for (int i = _from; i < _from + dist; ++i, ++start)
        {
            _array->array[i] = _array->array[start];
        }
        _array->count = dist >= _array->count ? 0 : _array->count - dist;
    }
    return 0;
}

_TARP_ITEM_T _TARP_FN(_TARP_ARRAY_T, At)(_TARP_ARRAY_T * _array, int _index)
{
    assert(_array);
    assert(_array->count);
    assert(_index >= 0 && _index < _array->count);
    return _array->array[_index];
}

_TARP_ITEM_T * _TARP_FN(_TARP_ARRAY_T, AtPtr)(_TARP_ARRAY_T * _array, int _index)
{
    assert(_array);
    assert(_array->count);
    assert(_index >= 0 && _index < _array->count);
    return &_array->array[_index];
}

int _TARP_FN(_TARP_ARRAY_T, Find)(_TARP_ARRAY_T * _array, _TARP_ITEM_T _value)
{
    assert(_array);
    assert(_array->array);
#if defined(_TARP_COMPARATOR_T) && _TARP_COMPARATOR_T == 0
    return -1;
#else
    for (int i = 0; i < _array->count; ++i)
    {
	#ifdef _TARP_COMPARATOR_T
	        if (_TARP_COMPARATOR_T(_array->array[i], _value))
	            return i;
	#else
	        if (_array->array[i] == _value)
	            return i;
	#endif
    }
#endif
    return -1;
}

int _TARP_FN(_TARP_ARRAY_T, RemoveValue)(_TARP_ARRAY_T * _array, _TARP_ITEM_T _value)
{
    int idx = _TARP_FN(_TARP_ARRAY_T, Find)(_array, _value);
    if (idx != -1)
    {
        _TARP_FN(_TARP_ARRAY_T, Remove)(_array, idx);
        return 1;
    }
    return 0;
}

_TARP_ITEM_T _TARP_FN(_TARP_ARRAY_T, Last)(_TARP_ARRAY_T * _array)
{
	assert(_array);
	assert(_array->count);
	return _array->array[_array->count - 1];
}

_TARP_ITEM_T * _TARP_FN(_TARP_ARRAY_T, LastPtr)(_TARP_ARRAY_T * _array)
{
	assert(_array);
	assert(_array->count);
	return &_array->array[_array->count - 1];
}

void _TARP_FN(_TARP_ARRAY_T, Swap)(_TARP_ARRAY_T * _a, _TARP_ARRAY_T * _b)
{
	_TARP_ITEM_T * c = _a->array;
	int cc = _a->count;
	int ccc = _a->capacity;
	_a->array = _b->array;
	_a->capacity = _b->capacity;
	_a->count = _b->count;
	_b->array = c;
	_b->capacity = ccc;
	_b->count = cc;
}

#undef _TARP_ARRAY_T
#undef _TARP_ITEM_T
#undef _TARP_COMPARATOR_T
