#ifndef TARP_TARP_H
#define TARP_TARP_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

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
typedef enum
{
    tpFalse = 0,
    tpTrue = 1
} tpBool;

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
    void * _implData;
    tpMat3 transform;
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
    void * _implData;
} tpGradient;

typedef union
{
    tpGradient * gradient;
    tpColor color;
} _tpPaintUnion;

typedef struct
{
    _tpPaintUnion data;
    PaintType type;
} tpPaint;

typedef struct
{
    tpPaint fill;
    tpPaint stroke;
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

tpColor tpColorMake(Float _r, Float _g, Float _b, Float _a);


// Vector Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpVec2 tpVec2Make(Float _x, Float _y);

tpVec2 tpVec2Add(const tpVec2 * _a, const tpVec2 * _b);

tpVec2 tpVec2Sub(const tpVec2 * _a, const tpVec2 * _b);

tpVec2 tpVec2Mult(const tpVec2 * _a, const tpVec2 * _b);

tpVec2 tpVec2Div(const tpVec2 * _a, const tpVec2 * _b);

tpBool tpVec2Equals(const tpVec2 * _a, const tpVec2 * _b);


// Matrix Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpMat3 tpMat3Make(Float _v0, Float _v1, Float _v2, //row1
                    Float _v3, Float _v4, Float _v5, //row2
                    Float _v6, Float _v7, Float _v8); //row3

tpMat3 tpMat3Identity();

int tpMat3Decompose(const tpMat3 * _mat, tpVec2 * _outTranslation,
                      tpVec2 * _outScale, tpVec2 * _outSkew, Float * _outRotation);

tpBool tpMat3Equals(const tpMat3 * _a, const tpMat3 * _b);

tpVec2 tpMat3MultVec2(const tpMat3 * _mat, const tpVec2 * _vec);

tpMat4 tpMat4Make(Float _v0, Float _v1, Float _v2, Float _v3, //row1
                    Float _v4, Float _v5, Float _v6, Float _v7, //row2
                    Float _v8, Float _v9, Float _v10, Float _v11, //row3
                    Float _v12, Float _v13, Float _v14, Float _v15); //row4

tpMat4 tpMat4Identity();

tpMat4 tpMat4Ortho(Float _left, Float _right, Float _bottom, Float _top, Float _near, Float _far);

tpMat4 tpMat4MakeFrom2DTransform(const tpMat3 * _transform);


// Array Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

void tpPtrArrayInit(tpPtrArray * _array, int _capacity);

void tpPtrArrayDeallocate(tpPtrArray * _array);

tpPtrArray tpPtrArrayMake(int _capacity);

void tpPtrArrayAppend(tpPtrArray * _array, const void * _element);

const void * tpPtrArrayLast(tpPtrArray * _array);


// Segment Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpSegment tpSegmentMake(Float _h0x, Float _h0y, Float _px, Float _py, Float _h1x, Float _h1y);


// Path Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpPath * tpPathCreate();

void tpPathDestroy(tpPath * _path);

int tpPathClear(tpPath * _path);

int tpPathSegmentCount(const tpPath * _path);

tpSegment * tpPathGetSegment(tpPath * _path, int _idx);

int tpPathCircle(tpPath * _path, Float _x, Float _y, Float _r);

int tpPathSetTransform(tpPath * _path, const tpMat3 * _transform);

int tpPathAddSegment(tpPath * _path, Float _h0x, Float _h0y, Float _px, Float _py, Float _h1x, Float _h1y);

int tpPathAddPoint(tpPath * _path, Float _x, Float _y);

int tpPathCubicCurveTo(tpPath * _path, Float _h0x, Float _h0y, Float _h1x, Float _h1y, Float _px, Float _py);

int tpPathQuadraticCurveTo(tpPath * _path, Float _hx, Float _hy, Float _px, Float _py);

int tpPathClose(tpPath * _path);

int tpPathAddChild(tpPath * _path, const tpPath * _child);


// Style Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpStyle tpStyleMake();

void tpStyleSetDash(tpStyle * _style, Float * _dashArray, int _count, Float _offset);

void tpStyleSetFillColor(tpStyle * _style, Float _r, Float _g, Float _b, Float _a);

void tpStyleSetStrokeColor(tpStyle * _style, Float _r, Float _g, Float _b, Float _a);

void tpStyleSetFillGradient(tpStyle * _style, const tpGradient * _gradient);

void tpStyleSetStrokeGradient(tpStyle * _style, const tpGradient * _gradient);

void tpStyleSetStrokeWidth(tpStyle * _style, Float _strokeWidth);

void tpStyleSetStrokeJoin(tpStyle * _style, StrokeJoin _join);

void tpStyleSetStrokeCap(tpStyle * _style, StrokeCap _cap);

void tpStyleSetFillType(tpStyle * _style, FillType _fillType);

tpGradient * tpGradientCreateLinear(Float _x0, Float _y0, Float _x1, Float _y1);

void tpGradientAddColorStop(tpGradient * _gradient, Float _r, Float _g, Float _b, Float _a, Float _offset);

void tpGradientDestroy(tpGradient * _gradient);


// Context Functions (These are implemented by the backend)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

int tpContextInit(tpContext * _ctx);

int toContextDeallocate(tpContext * _ctx);

int tpPrepareDrawing(tpContext * _ctx);

int tpFinishDrawing(tpContext * _ctx);

int tpSetProjection(tpContext * _ctx, const tpMat4 * _projection);

int tpDrawPath(tpContext * _ctx, tpPath * _path, const tpStyle * _style);


// Hidden functions that need to be implemented
// by the backend
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

int _tpPathInit(tpPath * _path);
int _tpPathDeallocate(tpPath * _path);
int _tpPathGeometryChanged(tpPath * _path);
int _tpPathTransformChanged(tpPath * _path, const tpMat3 * _old, const tpMat3 * _new);


// Implementations
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpColor tpColorMake(Float _r, Float _g, Float _b, Float _a)
{
    return (tpColor) {_r, _g, _b, _a};
}

tpVec2 tpVec2Make(Float _x, Float _y)
{
    return (tpVec2) {_x, _y};
}

tpVec2 tpVec2Add(const tpVec2 * _a, const tpVec2 * _b)
{
    return (tpVec2) {_a->x + _b->x, _a->y + _b->y};
}

tpVec2 tpVec2Sub(const tpVec2 * _a, const tpVec2 * _b)
{
    return (tpVec2) {_a->x - _b->x, _a->y - _b->y};
}

tpVec2 tpVec2Mult(const tpVec2 * _a, const tpVec2 * _b)
{
    return (tpVec2) {_a->x * _b->x, _a->y * _b->y};
}

tpVec2 tpVec2Div(const tpVec2 * _a, const tpVec2 * _b)
{
    return (tpVec2) {_a->x / _b->x, _a->y / _b->y};
}

tpBool tpVec2Equals(const tpVec2 * _a, const tpVec2 * _b)
{
    return _a->x == _b->x && _a->y == _b->y;
}

tpMat3 tpMat3Make(Float _v0, Float _v1, Float _v2,
                    Float _v3, Float _v4, Float _v5,
                    Float _v6, Float _v7, Float _v8)
{
    return (tpMat3)
    {
        _v0, _v3, _v6,
             _v1, _v4, _v7,
             _v2, _v5, _v8
    };
}

tpMat3 tpMat3Identity()
{
    return (tpMat3)
    {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };
}

int tpMat3Decompose(const tpMat3 * _mat, tpVec2 * _outTranslation, tpVec2 * _outScale, tpVec2 * _outSkew, Float * _outRotation)
{
    _outTranslation->x = _mat->v[6];
    _outTranslation->y = _mat->v[7];

    Float a = _mat->v[0];
    Float b = _mat->v[3];
    Float c = _mat->v[1];;
    Float d = _mat->v[4];;
    Float det = a * d - b * c;

    if (a != 0 || b != 0)
    {
        Float r = sqrt(a * a + b * b);
        *_outRotation = acos(a / r) * (b > 0 ? 1 : -1);
        _outScale->x = r;
        _outScale->y = det / r;
        _outSkew->x = atan2(a * c + b * d, r * r);
        _outSkew->y = 0;
    }
    else if (c != 0 || d != 0)
    {
        Float s = sqrt(c * c + d * d);
        *_outRotation = asin(c / s)  * (d > 0 ? 1 : -1);
        _outScale->x = det / s;
        _outScale->y = s;
        _outSkew->x = 0;
        _outSkew->y = atan2(a * c + b * d, s * s);
    }
    else     // a = b = c = d = 0
    {
        *_outRotation = 0;
        _outScale->x = 0;
        _outScale->y = 0;
        _outSkew->x = 0;
        _outSkew->y = 0;
    }

    // _outScale->x = sqrtf(a * a + b * b);
    // _outScale->y = sqrtf(c * c + d * d);

    // *_outRotation = -atan2(b, a);

    return 0;
}

tpBool tpMat3Equals(const tpMat3 * _a, const tpMat3 * _b)
{
    return (_a->v[0] == _b->v[0] && _a->v[1] == _b->v[1] && _a->v[2] == _b->v[2] &&
            _a->v[3] == _b->v[3] && _a->v[4] == _b->v[4] && _a->v[5] == _b->v[5] &&
            _a->v[6] == _b->v[6] && _a->v[7] == _b->v[7] && _a->v[8] == _b->v[8]);
}

tpMat4 tpMat4Make(Float _v0, Float _v1, Float _v2, Float _v3,
                    Float _v4, Float _v5, Float _v6, Float _v7,
                    Float _v8, Float _v9, Float _v10, Float _v11,
                    Float _v12, Float _v13, Float _v14, Float _v15)
{
    return (tpMat4)
    {
        _v0, _v4, _v8, _v12,
             _v1, _v5, _v9, _v13,
             _v2, _v6, _v10, _v14,
             _v3, _v7, _v11, _v15
    };
}

tpMat4 tpMat4Identity()
{
    return (tpMat4)
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

tpMat4 tpMat4Ortho(Float _left, Float _right, Float _bottom, Float _top, Float _near, Float _far)
{
    Float a = 2.0 / (_right - _left);
    Float b = 2.0 / (_top - _bottom);
    Float c = - 2.0 / (_far - _near);
    Float tx = -(_right + _left) / (_right - _left);
    Float ty = -(_top + _bottom) / (_top - _bottom);
    Float tz = -(_far + _near) / (_far - _near);

    return tpMat4Make(a, 0.0, 0.0, tx,
                        0.0, b, 0.0, ty,
                        0.0, 0.0, c, tz,
                        0.0, 0.0, 0, 1.0);
}

tpMat4 tpMat4MakeFrom2DTransform(const tpMat3 * _transform)
{
    return tpMat4Make(_transform->v[0], _transform->v[1], _transform->v[2], 0.0,
                        _transform->v[3], _transform->v[4], _transform->v[5], 0.0,
                        0, 0, 1, 0,
                        _transform->v[6], _transform->v[7], _transform->v[8], 1.0);
}

tpVec2 tpMat3MultVec2(const tpMat3 * _mat, const tpVec2 * _vec)
{
    return (tpVec2)
    {
        (_vec->x * _mat->v[0] + _vec->y * _mat->v[3] + _mat->v[6]),
        (_vec->x * _mat->v[1] + _vec->y * _mat->v[4] + _mat->v[7])
    };
}

void tpPtrArrayInit(tpPtrArray * _array, int _capacity)
{
    _array->array = malloc(sizeof(void *) * _capacity);
    _array->capacity = _capacity;
    _array->count = 0;
}

void tpPtrArrayDeallocate(tpPtrArray * _array)
{
    free(_array->array);
    _array->capacity = 0;
    _array->count = 0;
}

tpPtrArray tpPtrArrayMake(int _capacity)
{
    tpPtrArray ret;
    tpPtrArrayInit(&ret, _capacity);
    return ret;
}

void tpPtrArrayAppend(tpPtrArray * _array, const void * _element)
{
    if (_array->count == _array->capacity)
    {
        _array->capacity *= 2;
        _array->array = realloc(_array->array, _array->capacity * sizeof(void *));
    }
    _array->array[_array->count++] = _element;
}

const void * tpPtrArrayLast(tpPtrArray * _array)
{
    return _array->array[_array->count - 1];
}

tpSegment tpSegmentMake(Float _h0x, Float _h0y, Float _px, Float _py, Float _h1x, Float _h1y)
{
    return (tpSegment)
    {
        {_h0x, _h0y},
        {_px, _py},
        {_h1x, _h1y}
    };
}

tpSegmentChunk * _tpSegmentChunkCreate()
{
    tpSegmentChunk * ret = (tpSegmentChunk *)malloc(sizeof(tpSegmentChunk));
    ret->count = 0;
    return ret;
}

tpPath * tpPathCreate()
{
    tpPath * ret = (tpPath *) malloc(sizeof(tpPath));
    tpPtrArrayInit(&ret->segmentChunks, 4);
    tpPtrArrayInit(&ret->children, 16);
    ret->lastSegment = NULL;
    ret->bIsClosed = 0;
    memset(ret->lastErrorMessage, 0, sizeof(ret->lastErrorMessage));

    _tpPathInit(ret);

    tpMat3 tmp = tpMat3Identity();
    tpPathSetTransform(ret, &tmp);
    return ret;
}

void tpPathDestroy(tpPath * _path)
{
    _tpPathDeallocate(_path);
    for (int i = 0; i < _path->segmentChunks.count; ++i)
    {
        free((tpSegmentChunk *)_path->segmentChunks.array[i]);
    }

    for (int i = 0; i < _path->children.count; ++i)
    {
        free((tpPath *)_path->children.array[i]);
    }

    tpPtrArrayDeallocate(&_path->segmentChunks);
    tpPtrArrayDeallocate(&_path->children);
    free((void *)_path);
}

int tpPathClear(tpPath * _path)
{
    // _tpPathClear(_path);
    //free all segment chunks but the first
    for (int i = 1; i < _path->segmentChunks.count; ++i)
    {
        free((tpSegmentChunk *)_path->segmentChunks.array[i]);
    }
    ((tpSegmentChunk *)_path->segmentChunks.array[0])->count = 0;
}

tpSegmentChunk * _tpPathLastSegmentChunk(tpPath * _path)
{
    return (tpSegmentChunk *)(tpPtrArrayLast(&_path->segmentChunks));
}

int tpPathSegmentCount(const tpPath * _path)
{
    int ret = 0;
    for (int i = 0; i < _path->segmentChunks.count; ++i)
    {
        ret += ((tpSegmentChunk *)_path->segmentChunks.array[i])->count;
    }
    return ret;
}

tpSegment * tpPathGetSegment(tpPath * _path, int _idx)
{
    int chunkIdx = _idx % TARP_SEGMENT_CHUNK_SIZE;
    if (chunkIdx < _path->segmentChunks.count)
    {
        return &((tpSegmentChunk *)_path->segmentChunks.array[chunkIdx])->array[_idx - chunkIdx * TARP_SEGMENT_CHUNK_SIZE];
    }
    return NULL;
}

int tpPathAddSegment(tpPath * _path, Float _h0x, Float _h0y, Float _px, Float _py, Float _h1x, Float _h1y)
{
    // if there is no segment chunk or the current one is full, add a new one
    if (!_path->segmentChunks.count ||
            _tpPathLastSegmentChunk(_path)->count == TARP_SEGMENT_CHUNK_SIZE)
    {
        tpPtrArrayAppend(&_path->segmentChunks, _tpSegmentChunkCreate());
    }

    tpSegmentChunk * chunk = _tpPathLastSegmentChunk(_path);
    int idx = chunk->count;
    chunk->array[idx] = tpSegmentMake(_h0x, _h0y, _px, _py, _h1x, _h1y);
    _path->lastSegment = &chunk->array[idx];
    chunk->count++;

    _tpPathGeometryChanged(_path);
    return 0;
}

int tpPathAddPoint(tpPath * _path, Float _x, Float _y)
{
    tpPathAddSegment(_path, _x, _y, _x, _y, _x, _y);
    return 0;
}

int tpPathCircle(tpPath * _path, Float _x, Float _y, Float _r)
{
    return 0;
}

int tpPathCubicCurveTo(tpPath * _path, Float _h0x, Float _h0y, Float _h1x, Float _h1y, Float _px, Float _py)
{
    assert(_path->lastSegment);
    _path->lastSegment->handleOut.x = _h0x;
    _path->lastSegment->handleOut.y = _h0y;
    tpPathAddSegment(_path, _h1x, _h1y, _px, _py, _px, _py);
    return 0;
}

int tpPathQuadraticCurveTo(tpPath * _path, Float _hx, Float _hy, Float _px, Float _py)
{
    assert(_path->lastSegment);
    _path->lastSegment->handleOut.x = _hx;
    _path->lastSegment->handleOut.y = _hy;
    tpPathAddSegment(_path, _hx, _hy, _px, _py, _px, _py);
    return 0;
}

int tpPathClose(tpPath * _path)
{
    _path->bIsClosed = 1;
    return 0;
}

int tpPathAddChild(tpPath * _path, const tpPath * _child)
{
    tpPtrArrayAppend(&_path->children, _child);
}

int tpPathSetTransform(tpPath * _path, const tpMat3 * _transform)
{
    _tpPathTransformChanged(_path, &_path->transform, _transform);
    _path->transform = *_transform;
}

tpStyle tpStyleMake()
{
    tpStyle ret;

    ret.fill.data.color = tpColorMake(1, 1, 1, 1);
    ret.fill.type = kPtColor;
    ret.stroke.data.color = tpColorMake(0, 0, 0, 1);
    ret.stroke.type = kPtColor;
    ret.strokeWidth = 1.0;
    ret.strokeJoin = kSjMiter;
    ret.strokeCap = kScButt;
    ret.fillType = kFtEvenOdd;
    ret.dashCount = 0;
    ret.dashOffset = 0;

    return ret;
}

void tpStyleSetDash(tpStyle * _style, Float * _dashArray, int _count, Float _offset)
{
    assert(_count < TARP_MAX_DASH_ARRAY_SIZE);

    _style->dashOffset = _offset;
    for (int i = 0; i < _count; ++i)
    {
        _style->dashArray[i] = _dashArray[i];
    }
    _style->dashCount = _count;
}

void tpStyleSetFillColor(tpStyle * _style, Float _r, Float _g, Float _b, Float _a)
{
    _style->fill.data.color = tpColorMake(_r, _g, _b, _a);
    _style->fill.type = kPtColor;
}

void tpStyleSetStrokeColor(tpStyle * _style, Float _r, Float _g, Float _b, Float _a)
{
    _style->stroke.data.color = tpColorMake(_r, _g, _b, _a);
    _style->stroke.type = kPtColor;
}

void tpStyleSetFillGradient(tpStyle * _style, const tpGradient * _gradient)
{
    _style->fill.data.gradient = (tpGradient *)_gradient;
    _style->fill.type = kPtGradient;
}

void tpStyleSetStrokeGradient(tpStyle * _style, const tpGradient * _gradient)
{
    _style->stroke.data.gradient = (tpGradient *)_gradient;
    _style->stroke.type = kPtGradient;
}

void tpStyleSetStrokeWidth(tpStyle * _style, Float _strokeWidth)
{
    _style->strokeWidth = _strokeWidth;
}

void tpStyleSetStrokeJoin(tpStyle * _style, StrokeJoin _join)
{
    _style->strokeJoin = _join;
}

void tpStyleSetStrokeCap(tpStyle * _style, StrokeCap _cap)
{
    _style->strokeCap = _cap;
}

void tpStyleSetFillType(tpStyle * _style, FillType _fillType)
{
    _style->fillType = _fillType;
}

tpGradient * tpGradientCreateLinear(Float _x0, Float _y0, Float _x1, Float _y1)
{
    tpGradient * ret = (tpGradient *)malloc(sizeof(tpGradient));
    ret->type = kGtLinear;
    ret->origin = tpVec2Make(_x0, _y0);
    ret->destination = tpVec2Make(_x1, _y1);
    ret->stopCount = 0;
    return ret;
}

void tpGradientAddColorStop(tpGradient * _gradient, Float _r, Float _g, Float _b, Float _a, Float _offset)
{
    _gradient->stops[_gradient->stopCount++] = (tpColorStop) {tpColorMake(_r, _g, _b, _a), _offset};
}

void tpGradientDestroy(tpGradient * _gradient)
{
    free(_gradient);
}

#ifdef __cplusplus
}
#endif

#endif //TARP_TARP_H
