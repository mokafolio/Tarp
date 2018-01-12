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
#define TARP_MAX_ERROR_MESSAGE 256

// some constants
#define TARP_KAPPA 0.55228474983

#ifdef __cplusplus
extern "C" {
#endif

// Constants
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef enum
{
    kTpGradientTypeLinear,
    kTpGradientTypeRadial
} tpGradientType;

typedef enum
{
    kTpPaintTypeNone,
    kTpPaintTypeColor,
    kTpPaintTypeGradient
} tpPaintType;

typedef enum
{
    kTpFillTypeEvenOdd,
    kTpFillTypeNonZero
} tpFillType;

typedef enum
{
    kTpStrokeCapRound,
    kTpStrokeCapSquare,
    kTpStrokeCapButt
} tpStrokeCap;

typedef enum
{
    kTpStrokeJoinMiter,
    kTpStrokeJoinRound,
    kTpStrokeJoinBevel
} tpStrokeJoin;

// Basic Types
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef float tpFloat;
typedef enum
{
    tpFalse = 0,
    tpTrue = 1
} tpBool;

// typedef struct
// {
//     int typeIndex;
//     int index;
// } tpHandle;

struct _tpHandleData;
typedef struct _tpHandleData tpHandle;
typedef tpHandle tpPath;
typedef tpHandle tpStyle;
typedef tpHandle tpGradient;
typedef tpHandle tpPaint;

// Structures
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef struct
{
    tpFloat r, g, b, a;
} tpColor;

typedef struct
{
    tpFloat x;
    tpFloat y;
} tpVec2;

typedef struct
{
    tpFloat v[9];
} tpMat3;

typedef struct
{
    tpFloat v[16];
} tpMat4;

typedef struct
{
    tpColor color;
    tpFloat offset;
} tpColorStop;

typedef struct
{
    tpVec2 handleIn;
    tpVec2 position;
    tpVec2 handleOut;
} tpSegment;

typedef struct
{
    char message[TARP_MAX_ERROR_MESSAGE];
} tpError;

struct _tpContextData;
typedef struct _tpContextData tpContext;


// Color Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpColor tpColorMake(tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a);


// Vector Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpVec2 tpVec2Make(tpFloat _x, tpFloat _y);

tpVec2 tpVec2Add(const tpVec2 * _a, const tpVec2 * _b);

tpVec2 tpVec2Sub(const tpVec2 * _a, const tpVec2 * _b);

tpVec2 tpVec2Mult(const tpVec2 * _a, const tpVec2 * _b);

tpVec2 tpVec2MultScalar(const tpVec2 * _a, tpFloat _b);

tpVec2 tpVec2Div(const tpVec2 * _a, const tpVec2 * _b);

tpBool tpVec2Equals(const tpVec2 * _a, const tpVec2 * _b);

tpFloat tpVec2Length(const tpVec2 * _vec);

tpFloat tpVec2Dot(const tpVec2 * _a, const tpVec2 * _b);

tpFloat tpVec2Cross(const tpVec2 * _a, const tpVec2 * _b);

void tpVec2NormalizeSelf(tpVec2 * _vec);

tpVec2 tpVec2Normalize(const tpVec2 * _vec);

tpVec2 tpVec2Perp(const tpVec2 * _a);

tpFloat tpVec2Distance(const tpVec2 * _a, const tpVec2 * _b);


// Matrix Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpMat3 tpMat3Make(tpFloat _v0, tpFloat _v1, tpFloat _v2, //row1
                  tpFloat _v3, tpFloat _v4, tpFloat _v5, //row2
                  tpFloat _v6, tpFloat _v7, tpFloat _v8); //row3

tpMat3 tpMat3Identity();

int tpMat3Decompose(const tpMat3 * _mat, tpVec2 * _outTranslation,
                    tpVec2 * _outScale, tpVec2 * _outSkew, tpFloat * _outRotation);

tpBool tpMat3Equals(const tpMat3 * _a, const tpMat3 * _b);

tpVec2 tpMat3MultVec2(const tpMat3 * _mat, const tpVec2 * _vec);

tpMat4 tpMat4Make(tpFloat _v0, tpFloat _v1, tpFloat _v2, tpFloat _v3, //row1
                  tpFloat _v4, tpFloat _v5, tpFloat _v6, tpFloat _v7, //row2
                  tpFloat _v8, tpFloat _v9, tpFloat _v10, tpFloat _v11, //row3
                  tpFloat _v12, tpFloat _v13, tpFloat _v14, tpFloat _v15); //row4

tpMat4 tpMat4Identity();

tpMat4 tpMat4Ortho(tpFloat _left, tpFloat _right, tpFloat _bottom, tpFloat _top, tpFloat _near, tpFloat _far);

tpMat4 tpMat4MakeFrom2DTransform(const tpMat3 * _transform);


// Segment Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpSegment tpSegmentMake(tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y);


// Path Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpPath tpPathCreate(tpContext * _ctx);

void tpPathDestroy(tpPath _path);

tpBool tpPathClear(tpPath _path);

tpBool tpPathSetTransform(tpPath _path, const tpMat3 * _transform);

tpBool tpPathAddCircle(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _r);

tpBool tpPathAddEllipse(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height);

tpBool tpPathAddRect(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height);

tpBool tpPathAddSegment(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y);

tpBool tpPathMoveTo(tpPath _path, tpFloat _x, tpFloat _y);

tpBool tpPathLineTo(tpPath _path, tpFloat _x, tpFloat _y);

tpBool tpPathCubicCurveTo(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _h1x, tpFloat _h1y, tpFloat _px, tpFloat _py);

tpBool tpPathQuadraticCurveTo(tpPath _path, tpFloat _hx, tpFloat _hy, tpFloat _px, tpFloat _py);

tpBool tpPathClose(tpPath _path);

tpBool tpPathRemoveContour(tpPath _path, int _index);

tpBool tpPathAddSegments(tpPath _path, tpSegment * _segments, int _count);

tpBool tpPathAddContour(tpPath _path, tpSegment * _segments, int _count, tpBool _bClosed);


// Style Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpStyle tpStyleCreate(tpContext * _ctx);

void tpStyleDestroy(tpStyle _style);

void tpStyleSetDashArray(tpStyle _style, tpFloat * _dashArray, int _count);

void tpStyleSetDashOffset(tpStyle _style, tpFloat _offset);

void tpStyleSetFillColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a);

void tpStyleSetStrokeColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a);

void tpStyleSetFillGradient(tpStyle _style, const tpGradient _gradient);

void tpStyleSetStrokeGradient(tpStyle _style, const tpGradient _gradient);

void tpStyleSetStrokeWidth(tpStyle _style, tpFloat _strokeWidth);

void tpStyleSetStrokeJoin(tpStyle _style, tpStrokeJoin _join);

void tpStyleSetMiterLimit(tpStyle _style, tpFloat _limit);

void tpStyleSetStrokeCap(tpStyle _style, tpStrokeCap _cap);

void tpStyleSetFillType(tpStyle _style, tpFillType _fillType);

tpGradient tpGradientCreateLinear(tpContext * _ctx, tpFloat _x0, tpFloat _y0, tpFloat _x1, tpFloat _y1);

void tpGradientAddColorStop(tpGradient _gradient, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a, tpFloat _offset);

void tpGradientDestroy(tpGradient _gradient);


// Context Functions (These are implemented by the backend)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpBool tpContextInit(tpContext * _ctx);

tpBool tpContextDeallocate(tpContext * _ctx);

tpBool tpPrepareDrawing(tpContext * _ctx);

tpBool tpFinishDrawing(tpContext * _ctx);

tpBool tpSetProjection(tpContext * _ctx, const tpMat4 * _projection);

tpBool tpDrawPath(tpContext * _ctx, tpPath _path, const tpStyle _style);


// // Hidden functions that need to be implemented
// // by the backend
// // ~~~~~~~~~~~~~~~~~~~~~~~~~~

// int _tpPathInit(tpPath * _path);
// int _tpPathDeallocate(tpPath * _path);
// int _tpPathClear(tpPath * _path);
// int _tpPathGeometryChanged(tpPath * _path);
// int _tpPathTransformChanged(tpPath * _path, const tpMat3 * _old, const tpMat3 * _new);


// Implementations
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

tpColor tpColorMake(tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a)
{
    return (tpColor) {_r, _g, _b, _a};
}

tpVec2 tpVec2Make(tpFloat _x, tpFloat _y)
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

tpVec2 tpVec2MultScalar(const tpVec2 * _a, tpFloat _b)
{
    return (tpVec2) {_a->x * _b, _a->y * _b};
}

tpVec2 tpVec2Div(const tpVec2 * _a, const tpVec2 * _b)
{
    return (tpVec2) {_a->x / _b->x, _a->y / _b->y};
}

tpBool tpVec2Equals(const tpVec2 * _a, const tpVec2 * _b)
{
    return _a->x == _b->x && _a->y == _b->y;
}

tpFloat tpVec2Length(const tpVec2 * _vec)
{
    return sqrt(_vec->x * _vec->x + _vec->y * _vec->y);
}

void tpVec2NormalizeSelf(tpVec2 * _vec)
{
    tpFloat s = 1.0 / tpVec2Length(_vec);
    _vec->x *= s;
    _vec->y *= s;
}

tpVec2 tpVec2Normalize(const tpVec2 * _vec)
{
    tpVec2 ret = {_vec->x, _vec->y};
    tpVec2NormalizeSelf(&ret);
    return ret;
}

tpVec2 tpVec2Perp(const tpVec2 * _a)
{
    return (tpVec2) {_a->y, -_a->x};
}

tpFloat tpVec2Dot(const tpVec2 * _a, const tpVec2 * _b)
{
    return _a->x * _b->x + _a->y * _b->y;
}

tpFloat tpVec2Cross(const tpVec2 * _a, const tpVec2 * _b)
{
    return _a->x * _b->y - _a->y * _b->x;
}

tpFloat tpVec2Distance(const tpVec2 * _a, const tpVec2 * _b)
{
    tpVec2 tmp = tpVec2Sub(_a, _b);
    return tpVec2Length(&tmp);
}

tpMat3 tpMat3Make(tpFloat _v0, tpFloat _v1, tpFloat _v2,
                  tpFloat _v3, tpFloat _v4, tpFloat _v5,
                  tpFloat _v6, tpFloat _v7, tpFloat _v8)
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

int tpMat3Decompose(const tpMat3 * _mat, tpVec2 * _outTranslation, tpVec2 * _outScale, tpVec2 * _outSkew, tpFloat * _outRotation)
{
    _outTranslation->x = _mat->v[6];
    _outTranslation->y = _mat->v[7];

    tpFloat a = _mat->v[0];
    tpFloat b = _mat->v[3];
    tpFloat c = _mat->v[1];;
    tpFloat d = _mat->v[4];;
    tpFloat det = a * d - b * c;

    if (a != 0 || b != 0)
    {
        tpFloat r = sqrt(a * a + b * b);
        *_outRotation = acos(a / r) * (b > 0 ? 1 : -1);
        _outScale->x = r;
        _outScale->y = det / r;
        _outSkew->x = atan2(a * c + b * d, r * r);
        _outSkew->y = 0;
    }
    else if (c != 0 || d != 0)
    {
        tpFloat s = sqrt(c * c + d * d);
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

tpMat4 tpMat4Make(tpFloat _v0, tpFloat _v1, tpFloat _v2, tpFloat _v3,
                  tpFloat _v4, tpFloat _v5, tpFloat _v6, tpFloat _v7,
                  tpFloat _v8, tpFloat _v9, tpFloat _v10, tpFloat _v11,
                  tpFloat _v12, tpFloat _v13, tpFloat _v14, tpFloat _v15)
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

tpMat4 tpMat4Ortho(tpFloat _left, tpFloat _right, tpFloat _bottom, tpFloat _top, tpFloat _near, tpFloat _far)
{
    tpFloat a = 2.0 / (_right - _left);
    tpFloat b = 2.0 / (_top - _bottom);
    tpFloat c = - 2.0 / (_far - _near);
    tpFloat tx = -(_right + _left) / (_right - _left);
    tpFloat ty = -(_top + _bottom) / (_top - _bottom);
    tpFloat tz = -(_far + _near) / (_far - _near);

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

// void tpPtrArrayInit(tpPtrArray * _array, int _capacity)
// {
//     _array->array = malloc(sizeof(void *) * _capacity);
//     _array->capacity = _capacity;
//     _array->count = 0;
// }

// void tpPtrArrayDeallocate(tpPtrArray * _array)
// {
//     free(_array->array);
//     _array->capacity = 0;
//     _array->count = 0;
// }

// tpPtrArray tpPtrArrayMake(int _capacity)
// {
//     tpPtrArray ret;
//     tpPtrArrayInit(&ret, _capacity);
//     return ret;
// }

// void tpPtrArrayAppend(tpPtrArray * _array, const void * _element)
// {
//     if (_array->count == _array->capacity)
//     {
//         _array->capacity *= 2;
//         _array->array = realloc(_array->array, _array->capacity * sizeof(void *));
//     }
//     _array->array[_array->count++] = _element;
// }

// const void * tpPtrArrayLast(tpPtrArray * _array)
// {
//     return _array->array[_array->count - 1];
// }

tpSegment tpSegmentMake(tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y)
{
    return (tpSegment)
    {
        {_h0x, _h0y},
        {_px, _py},
        {_h1x, _h1y}
    };
}

// tpSegmentChunk * _tpSegmentChunkCreate()
// {
//     tpSegmentChunk * ret = (tpSegmentChunk *)malloc(sizeof(tpSegmentChunk));
//     ret->count = 0;
//     return ret;
// }

#ifdef __cplusplus
}
#endif

#endif //TARP_TARP_H
