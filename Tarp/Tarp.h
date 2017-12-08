#ifndef TARP_TARP_H
#define TARP_TARP_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

//POSSIBLE PLATFORMS
#define TARP_PLATFORM_OSX 1
#define TARP_PLATFORM_LINUX 2
//What else do we need?

#if defined ( __APPLE__ )
#define TARP_PLATFORM TARP_PLATFORM_OSX
#define TARP_PLATFORM_UNIX
#elif __linux__
#define TARP_PLATFORM TARP_PLATFORM_LINUX
#define TARP_PLATFORM_UNIX
#endif

//debug
#if !defined(NDEBUG)
#define TARP_DEBUG
#else
#undef TARP_DEBUG
#endif

//visibility
#define TARP_API  __attribute__ ((visibility("default")))
#define TARP_LOCAL __attribute__ ((visibility("hidden")))

// some settings
#define TARP_SEGMENT_CHUNK_SIZE 512
#define TARP_MAX_COLOR_STOPS 128
#define TARP_MAX_DASH_ARRAY_SIZE 64

#ifdef __cplusplus
extern "C" {
#endif

// Constants
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef enum
{
    kGtLinear,
    kGtRadial
} GradientType;

typedef enum
{
    kPtColor,
    kPtGradient
} PaintType;

typedef enum
{
    kFtEvenOdd,
    kFtNonZero
} FillType;

typedef enum
{
    kScRound,
    kScSquare,
    kScButt
} StrokeCap;

typedef enum
{
    kSjMiter,
    kSjRound,
    kSjBevel
} StrokeJoin;

// Basic Types
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef float Float;

// Structures
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef struct
{
    Float r, g, b, a;
} tpColor;

typedef struct
{
    Float x;
    Float y;
} tpVec2;

typedef struct
{
    Float v[9];
} tpMat3;

typedef struct
{
    Float v[16];
} tpMat4;

typedef struct
{
    const void ** array;
    int count;
    int capacity;
} tpPtrArray;

typedef struct
{
    tpVec2 handleIn;
    tpVec2 position;
    tpVec2 handleOut;
} tpSegment;

typedef struct
{
    tpSegment array[TARP_SEGMENT_CHUNK_SIZE];
    int count;
} tpSegmentChunk;

typedef struct
{
    tpPtrArray segmentChunks;
    tpPtrArray children;
    int bIsClosed;
    tpSegment * lastSegment;
    char lastErrorMessage[512];
    // void * _implData;
} tpPath;

typedef struct
{
    tpColor color;
    Float offset;
} tpColorStop;

typedef struct
{
    tpVec2 origin;
    tpVec2 destination;
    tpColorStop stops[TARP_MAX_COLOR_STOPS];
    int stopCount;
    GradientType type;
} tpGradient;

typedef union
{
    tpGradient * gradient;
    tpColor color;
} _tpPaintStorageUnion;

typedef struct
{
    _tpPaintStorageUnion data;
    PaintType type;
} tpPaintStorage;

typedef struct
{
    tpPaintStorage fill;
    tpPaintStorage stroke;
    Float strokeWidth;
    StrokeCap strokeCap;
    StrokeJoin strokeJoin;
    FillType fillType;
    Float dashArray[TARP_MAX_DASH_ARRAY_SIZE];
    int dashCount;
    Float dashOffset;
} tpStyle;

typedef struct
{
    void * _implData;
    char lastErrorMessage[512];
} tpContext;

// Color Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpColor tp_color_make(Float _r, Float _g, Float _b, Float _a);

// Vector Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpVec2 tp_vec2_make(Float _x, Float _y);

tpVec2 tp_vec2_add(const tpVec2 * _a, const tpVec2 * _b);

tpVec2 tp_vec2_sub(const tpVec2 * _a, const tpVec2 * _b);

tpVec2 tp_vec2_mult(const tpVec2 * _a, const tpVec2 * _b);

tpVec2 tp_vec2_div(const tpVec2 * _a, const tpVec2 * _b);

int tp_vec2_equals(const tpVec2 * _a, const tpVec2 * _b);

tpVec2 tp_vec2_mult_mat(const tpMat3 * _mat, const tpVec2 * _vec);


// Matrix Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpMat3 tp_mat3_make(Float _v0, Float _v1, Float _v2,
                    Float _v3, Float _v4, Float _v5,
                    Float _v6, Float _v7, Float _v8);

tpMat3 tp_mat3_identity();


// Array Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

void tp_ptr_array_init(tpPtrArray * _array, int _capacity);

void tp_ptr_array_deallocate(tpPtrArray * _array);

tpPtrArray tp_ptr_array_make(int _capacity);

void tp_ptr_array_append(tpPtrArray * _array, const void * _element);

const void * tp_ptr_array_last(tpPtrArray * _array);


// Segment Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpSegment tp_segment_make(Float _h0x, Float _h0y, Float _px, Float _py, Float _h1x, Float _h1y);


// Path Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpPath * tp_path_new();

void tp_path_destroy(tpPath * _path);

int tp_path_segment_count(const tpPath * _path);

tpSegment * tp_path_get_segment(tpPath * _path, int _idx);

int tp_path_circle(tpPath * _path, Float _x, Float _y, Float _r);

int tp_path_add_segment(tpPath * _path, Float _h0x, Float _h0y, Float _px, Float _py, Float _h1x, Float _h1y);

int tp_path_add_point(tpPath * _path, Float _x, Float _y);

int tp_path_cubic_curve_to(tpPath * _path, Float _h0x, Float _h0y, Float _h1x, Float _h1y, Float _px, Float _py);

int tp_path_quadratic_curve_to(tpPath * _path, Float _hx, Float _hy, Float _px, Float _py);

int tp_path_close(tpPath * _path);

int tp_path_add_child(tpPath * _path, const tpPath * _child);

tpSegmentChunk * _tp_segment_chunk_new();

tpSegmentChunk * _tp_path_last_segment_chunk(tpPath * _path);

// Style Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpStyle tp_style_make();

void tp_style_set_dash(tpStyle * _style, Float * _dashArray, int _count, Float _offset);

void tp_style_set_fill_color(tpStyle * _style, Float _r, Float _g, Float _b, Float _a);

void tp_style_set_stroke_color(tpStyle * _style, Float _r, Float _g, Float _b, Float _a);

void tp_style_set_fill_gradient(tpStyle * _style, const tpGradient * _gradient);

void tp_style_set_stroke_gradient(tpStyle * _style, const tpGradient * _gradient);

void tp_style_set_stroke_width(tpStyle * _style, Float _strokeWidth);

void tp_style_set_stroke_join(tpStyle * _style, StrokeJoin _join);

void tp_style_set_stroke_cap(tpStyle * _style, StrokeCap _cap);

void tp_style_set_fill_type(tpStyle * _style, FillType _fillType);

tpGradient * tp_gradient_linear_new(Float _x0, Float _y0, Float _x1, Float _y1);

void tp_gradient_add_color_stop(tpGradient * _gradient, Float _r, Float _g, Float _b, Float _a, Float _offset);

void tp_gradient_destroy(tpGradient * _gradient);

// Context Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

int tp_context_init(tpContext * _ctx);

int tp_context_deallocate(tpContext * _ctx);

int tp_context_prepare_drawing(tpContext * _ctx);

int tp_context_finish_drawing(tpContext * _ctx);

int tp_draw_path(tpContext * _ctx, tpPath * _path, const tpStyle * _style, const tpMat3 * _transform);


// Implementations
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpColor tp_color_make(Float _r, Float _g, Float _b, Float _a)
{
    return (tpColor) {_r, _g, _b, _a};
}

tpVec2 tp_vec2_make(Float _x, Float _y)
{
    return (tpVec2) {_x, _y};
}

tpVec2 tp_vec2_add(const tpVec2 * _a, const tpVec2 * _b)
{
    return (tpVec2) {_a->x + _b->x, _a->y + _b->y};
}

tpVec2 tp_vec2_sub(const tpVec2 * _a, const tpVec2 * _b)
{
    return (tpVec2) {_a->x - _b->x, _a->y - _b->y};
}

tpVec2 tp_vec2_mult(const tpVec2 * _a, const tpVec2 * _b)
{
    return (tpVec2) {_a->x * _b->x, _a->y * _b->y};
}

tpVec2 tp_vec2_div(const tpVec2 * _a, const tpVec2 * _b)
{
    return (tpVec2) {_a->x / _b->x, _a->y / _b->y};
}

int tp_vec2_equals(const tpVec2 * _a, const tpVec2 * _b)
{
    return _a->x == _b->x && _a->y == _b->y;
}

tpMat3 tp_mat3_make(Float _v0, Float _v1, Float _v2,
                    Float _v3, Float _v4, Float _v5,
                    Float _v6, Float _v7, Float _v8)
{
    return (tpMat3)
    {
        _v0, _v1, _v2,
             _v3, _v4, _v5,
             _v6, _v7, _v8
    };
}

tpMat3 tp_mat3_identity()
{
    return (tpMat3)
    {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };
}

tpVec2 tp_vec2_mult_mat(const tpMat3 * _mat, const tpVec2 * _vec)
{
    return (tpVec2)
    {
        (_vec->x * _mat->v[0] + _vec->y * _mat->v[3] + _mat->v[6]),
        (_vec->x * _mat->v[1] + _vec->y * _mat->v[4] + _mat->v[7])
    };
}

void tp_ptr_array_init(tpPtrArray * _array, int _capacity)
{
    _array->array = (const void **)malloc(sizeof(void *) * _capacity);
    _array->capacity = _capacity;
    _array->count = 0;
}

void tp_ptr_array_deallocate(tpPtrArray * _array)
{
    free(_array->array);
    _array->capacity = 0;
    _array->count = 0;
}

tpPtrArray tp_ptr_array_make(int _capacity)
{
    tpPtrArray ret;
    tp_ptr_array_init(&ret, _capacity);
    return ret;
}

void tp_ptr_array_append(tpPtrArray * _array, const void * _element)
{
    if (_array->count == _array->capacity)
    {
        _array->capacity *= 2;
        _array->array = (const void **)realloc(_array->array, _array->capacity * sizeof(void *));
    }
    _array->array[_array->count++] = _element;
}

const void * tp_ptr_array_last(tpPtrArray * _array)
{
    return _array->array[_array->count - 1];
}

tpSegment tp_segment_make(Float _h0x, Float _h0y, Float _px, Float _py, Float _h1x, Float _h1y)
{
    return (tpSegment)
    {
        {_h0x, _h0y},
        {_px, _py},
        {_h1x, _h1y}
    };
}

tpSegmentChunk * _tp_segment_chunk_new()
{
    tpSegmentChunk * ret = (tpSegmentChunk *)malloc(sizeof(tpSegmentChunk));
    ret->count = 0;
    return ret;
}

tpPath * tp_path_new()
{
    tpPath * ret = (tpPath *) malloc(sizeof(tpPath));
    tp_ptr_array_init(&ret->segmentChunks, 4);
    tp_ptr_array_init(&ret->children, 16);
    ret->lastSegment = NULL;
    ret->bIsClosed = 0;
    memset(ret->lastErrorMessage, 0, sizeof(ret->lastErrorMessage));
    return ret;
}

void tp_path_destroy(tpPath * _path)
{
    for (int i = 0; i < _path->segmentChunks.count; ++i)
    {
        free((tpSegmentChunk *)_path->segmentChunks.array[i]);
    }

    for (int i = 0; i < _path->children.count; ++i)
    {
        free((tpPath *)_path->children.array[i]);
    }

    tp_ptr_array_deallocate(&_path->segmentChunks);
    tp_ptr_array_deallocate(&_path->children);
    free((void *)_path);
}

tpSegmentChunk * _tp_path_last_segment_chunk(tpPath * _path)
{
    return (tpSegmentChunk *)(tp_ptr_array_last(&_path->segmentChunks));
}

int tp_path_segment_count(const tpPath * _path)
{
    int ret = 0;
    for (int i = 0; i < _path->segmentChunks.count; ++i)
    {
        ret += ((tpSegmentChunk *)_path->segmentChunks.array[i])->count;
    }
    return ret;
}

tpSegment * tp_path_get_segment(tpPath * _path, int _idx)
{
    int chunkIdx = _idx % TARP_SEGMENT_CHUNK_SIZE;
    if (chunkIdx < _path->segmentChunks.count)
    {
        return &((tpSegmentChunk *)_path->segmentChunks.array[chunkIdx])->array[_idx - chunkIdx * TARP_SEGMENT_CHUNK_SIZE];
    }
    return NULL;
}

int tp_path_add_segment(tpPath * _path, Float _h0x, Float _h0y, Float _px, Float _py, Float _h1x, Float _h1y)
{
    // if there is no segment chunk or the current one is full, add a new one
    if (!_path->segmentChunks.count ||
            _tp_path_last_segment_chunk(_path)->count == TARP_SEGMENT_CHUNK_SIZE)
    {
        tp_ptr_array_append(&_path->segmentChunks, _tp_segment_chunk_new());
    }

    tpSegmentChunk * chunk = _tp_path_last_segment_chunk(_path);
    int idx = chunk->count;
    chunk->array[idx] = tp_segment_make(_h0x, _h0y, _px, _py, _h1x, _h1y);
    _path->lastSegment = &chunk->array[idx];
    chunk->count++;
    return 0;
}

int tp_path_add_point(tpPath * _path, Float _x, Float _y)
{
    tp_path_add_segment(_path, _x, _y, _x, _y, _x, _y);
    return 0;
}

int tp_path_circle(tpPath * _path, Float _x, Float _y, Float _r)
{
    return 0;
}

int tp_path_cubic_curve_to(tpPath * _path, Float _h0x, Float _h0y, Float _h1x, Float _h1y, Float _px, Float _py)
{
    assert(_path->lastSegment);
    _path->lastSegment->handleOut.x = _h0x;
    _path->lastSegment->handleOut.y = _h0y;
    tp_path_add_segment(_path, _h1x, _h1y, _px, _py, _px, _py);
    return 0;
}

int tp_path_quadratic_curve_to(tpPath * _path, Float _hx, Float _hy, Float _px, Float _py)
{
    assert(_path->lastSegment);
    _path->lastSegment->handleOut.x = _hx;
    _path->lastSegment->handleOut.y = _hy;
    tp_path_add_segment(_path, _hx, _hy, _px, _py, _px, _py);
    return 0;
}

int tp_path_close(tpPath * _path)
{
    _path->bIsClosed = 1;
    return 0;
}

int tp_path_add_child(tpPath * _path, const tpPath * _child)
{
    tp_ptr_array_append(&_path->children, _child);
}

tpStyle tp_style_make()
{
    tpStyle ret;

    ret.fill.data.color = tp_color_make(1, 1, 1, 1);
    ret.fill.type = kPtColor;
    ret.stroke.data.color = tp_color_make(0, 0, 0, 1);
    ret.stroke.type = kPtColor;
    ret.strokeWidth = 1.0;
    ret.strokeJoin = kSjMiter;
    ret.strokeCap = kScButt;
    ret.fillType = kFtEvenOdd;
    ret.dashCount = 0;
    ret.dashOffset = 0;

    return ret;
}

void tp_style_set_dash(tpStyle * _style, Float * _dashArray, int _count, Float _offset)
{
    assert(_count < TARP_MAX_DASH_ARRAY_SIZE);

    _style->dashOffset = _offset;
    for (int i = 0; i < _count; ++i)
    {
        _style->dashArray[i] = _dashArray[i];
    }
    _style->dashCount = _count;
}

void tp_style_set_fill_color(tpStyle * _style, Float _r, Float _g, Float _b, Float _a)
{
    _style->fill.data.color = tp_color_make(_r, _g, _b, _a);
    _style->fill.type = kPtColor;
}

void tp_style_set_stroke_color(tpStyle * _style, Float _r, Float _g, Float _b, Float _a)
{
    _style->stroke.data.color = tp_color_make(_r, _g, _b, _a);
    _style->stroke.type = kPtColor;
}

void tp_style_set_fill_gradient(tpStyle * _style, const tpGradient * _gradient)
{
    _style->fill.data.gradient = (tpGradient *)_gradient;
    _style->fill.type = kPtGradient;
}

void tp_style_set_stroke_gradient(tpStyle * _style, const tpGradient * _gradient)
{
    _style->stroke.data.gradient = (tpGradient *)_gradient;
    _style->stroke.type = kPtGradient;
}

void tp_style_set_stroke_width(tpStyle * _style, Float _strokeWidth)
{
    _style->strokeWidth = _strokeWidth;
}

void tp_style_set_stroke_join(tpStyle * _style, StrokeJoin _join)
{
    _style->strokeJoin = _join;
}

void tp_style_set_stroke_cap(tpStyle * _style, StrokeCap _cap)
{
    _style->strokeCap = _cap;
}

void tp_style_set_fill_type(tpStyle * _style, FillType _fillType)
{
    _style->fillType = _fillType;
}

tpGradient * tp_gradient_linear_new(Float _x0, Float _y0, Float _x1, Float _y1)
{
    tpGradient * ret = (tpGradient *)malloc(sizeof(tpGradient));
    ret->type = kGtLinear;
    ret->origin = tp_vec2_make(_x0, _y0);
    ret->destination = tp_vec2_make(_x1, _y1);
    ret->stopCount = 0;
    return ret;
}

void tp_gradient_add_color_stop(tpGradient * _gradient, Float _r, Float _g, Float _b, Float _a, Float _offset)
{
    _gradient->stops[_gradient->stopCount++] = (tpColorStop) {tp_color_make(_r, _g, _b, _a), _offset};
}

void tp_gradient_destroy(tpGradient * _gradient)
{
    free(_gradient);
}

#ifdef __cplusplus
}
#endif

#endif //TARP_TARP_H
