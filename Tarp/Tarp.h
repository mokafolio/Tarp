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
#define TARP_MAX_COLOR_STOPS 128
#define TARP_MAX_DASH_ARRAY_SIZE 64
#define TARP_MAX_ERROR_MESSAGE 256

// some constants
#define TARP_KAPPA 0.55228474983

// #define TARP_API extern

#define TARP_INCLUDE_OPENGL_IMPLEMENTATION
#ifdef TARP_INCLUDE_OPENGL_IMPLEMENTATION

#include <stdio.h>
#include <float.h>
#include <math.h>

#if TARP_PLATFORM == TARP_PLATFORM_OSX
#include <OpenGL/OpenGL.h> //for CGL functions
//check if opengl 3+ is available
#if CGL_VERSION_1_3 == 1
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#define __gl_h_ //to prevent other libraries such as glfw to include the old opengl headers
#else
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif //CGL_VERSION_1_3
#elif TARP_PLATFORM == TARP_PLATFORM_LINUX
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>
// #undef None //??? some glx thing
// #undef GL_GLEXT_PROTOTYPES
#endif

#ifdef TARP_DEBUG
#define _TARP_ASSERT_NO_GL_ERROR(_func) do { _func; \
GLenum err = glGetError(); \
if(err != GL_NO_ERROR) \
{ \
switch(err) \
{ \
case GL_NO_ERROR: \
printf("%s line %i GL_NO_ERROR: No error has been recorded.\n", __FILE__, __LINE__);\
break; \
case GL_INVALID_ENUM: \
printf("%s line %i GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.\n", __FILE__, __LINE__);\
break; \
case GL_INVALID_VALUE: \
printf("%s line %i GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.\n", __FILE__, __LINE__);\
break; \
case GL_INVALID_OPERATION: \
printf("%s line %i GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.\n", __FILE__, __LINE__);\
break; \
case GL_INVALID_FRAMEBUFFER_OPERATION: \
printf("%s line %i GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.\n", __FILE__, __LINE__);\
break; \
case GL_OUT_OF_MEMORY: \
printf("%s line %i GL_OUT_OF_MEMORY: There is not enough memory left to executeLua the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n", __FILE__, __LINE__);\
break; \
} \
exit(EXIT_FAILURE); \
} \
} while(0)
#else
#define _TARP_ASSERT_NO_GL_ERROR(_func) _func
#endif

//some helper macros used by the opengl implementation
#define TARP_MIN(a,b) (((a)<(b))?(a):(b))
#define TARP_MAX(a,b) (((a)>(b))?(a):(b))
#define TARP_CLAMP(a,l,h) TARP_MAX(TARP_MIN(a, h),l)

//some tarp related opengl related settings
#define TARP_GL_RAMP_TEXTURE_SIZE 1024
#define TARP_GL_MAX_CLIPPING_STACK_DEPTH 64
#define TARP_GL_ERROR_MESSAGE_SIZE 512

#define TARP_PI 3.14159265358979323846
#define TARP_HALF_PI TARP_PI * 0.5

#define TARP_MAX_CURVE_SUBDIVISIONS 16

#endif

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
    kTpFillRuleEvenOdd,
    kTpFillRuleNonZero
} tpFillRule;

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

typedef struct
{
    void * pointer;
} tpHandle;
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

typedef struct
{
    void * _impl;
    char errorMessage[TARP_MAX_ERROR_MESSAGE];
} tpContext;

/*struct _tpContextData;
typedef struct _tpContextData tpContext;*/

// function signatures so that we can dynamically bind an implementation
typedef tpPath (*_tpPathCreateFn)();
typedef void (*_tpPathDestroyFn)(tpPath);
typedef tpBool (*_tpPathClearFn)(tpPath);
typedef tpBool (*_tpPathSetTransformFn)(tpPath, const tpMat3 *);
typedef tpBool (*_tpPathAddCircleFn)(tpPath, tpFloat, tpFloat, tpFloat);
typedef tpBool (*_tpPathAddEllipseFn)(tpPath, tpFloat, tpFloat, tpFloat, tpFloat);
typedef tpBool (*_tpPathAddRectFn)(tpPath, tpFloat, tpFloat, tpFloat, tpFloat);
typedef tpBool (*_tpPathAddSegmentFn)(tpPath, tpFloat, tpFloat, tpFloat, tpFloat, tpFloat, tpFloat);
typedef tpBool (*_tpPathMoveToFn)(tpPath, tpFloat, tpFloat);
typedef tpBool (*_tpPathLineToFn)(tpPath, tpFloat, tpFloat);
typedef tpBool (*_tpPathCubicCurveToFn)(tpPath, tpFloat, tpFloat, tpFloat, tpFloat, tpFloat, tpFloat);
typedef tpBool (*_tpPathQuadraticCurveToFn)(tpPath, tpFloat, tpFloat, tpFloat, tpFloat);
typedef tpBool (*_tpPathCloseFn)(tpPath);
typedef tpBool (*_tpPathRemoveContourFn)(tpPath, int);
typedef tpBool (*_tpPathAddSegmentsFn)(tpPath, tpSegment *, int);
typedef tpBool (*_tpPathAddContourFn)(tpPath, tpSegment *, int, tpBool);

typedef tpStyle (*_tpStyleCreateFn)();
typedef void (*_tpStyleDestroyFn)(tpStyle);
typedef void (*_tpStyleSetDashArrayFn)(tpStyle, tpFloat *, int);
typedef void (*_tpStyleSetDashOffsetFn)(tpStyle, tpFloat);
typedef void (*_tpStyleSetFillColorFn)(tpStyle, tpFloat, tpFloat, tpFloat, tpFloat);
typedef void (*_tpStyleSetStrokeColorFn)(tpStyle, tpFloat, tpFloat, tpFloat, tpFloat);
typedef void (*_tpStyleSetFillGradientFn)(tpStyle, tpGradient);
typedef void (*_tpStyleSetStrokeGradientFn)(tpStyle, tpGradient);
typedef void (*_tpStyleSetStrokeWidthFn)(tpStyle, tpFloat);
typedef void (*_tpStyleSetStrokeJoinFn)(tpStyle, tpStrokeJoin);
typedef void (*_tpStyleSetScaleStrokeFn)(tpStyle, tpBool);
typedef void (*_tpStyleSetMiterLimitFn)(tpStyle, tpFloat);
typedef void (*_tpStyleSetStrokeCapFn)(tpStyle, tpStrokeCap);
typedef void (*_tpStyleSetFillRuleFn)(tpStyle, tpFillRule);
typedef void (*_tpStyleRemoveFillFn)(tpStyle);
typedef void (*_tpStyleRemoveStrokeFn)(tpStyle);

typedef tpGradient (*_tpGradientCreateLinearFn)(tpFloat, tpFloat, tpFloat, tpFloat);
typedef void (*_tpGradientAddColorStopFn)(tpGradient, tpFloat, tpFloat, tpFloat, tpFloat, tpFloat);
typedef void (*_tpGradientDestroyFn)(tpGradient);

typedef tpBool (*_tpContextInitFn)(tpContext *);
typedef tpBool (*_tpContextDeallocateFn)(tpContext *);
typedef tpBool (*_tpPrepareDrawingFn)(tpContext *);
typedef tpBool (*_tpFinishDrawingFn)(tpContext *);
typedef tpBool (*_tpSetProjectionFn)(tpContext *, const tpMat4 *);
typedef tpBool (*_tpDrawPathFn)(tpContext *, tpPath, const tpStyle);
typedef tpBool (*_tpBeginClippingFn)(tpContext *, tpPath);
typedef tpBool (*_tpEndClippingFn)(tpContext *);
typedef tpBool (*_tpResetClippingFn)(tpContext *);

// Moka: While we could turn the API functions in to function pointers that we bind
// to directly, I prefer having it in a separate struct so that the API functions
// can be seen in the header for a cleaner documentation.
// Note: In my tests this in direction did not have any measurable performance
// penalty in release builds anyways, hooray!
typedef struct
{
    _tpPathCreateFn pathCreate;
    _tpPathDestroyFn pathDestroy;
    _tpPathClearFn pathClear;
    _tpPathSetTransformFn pathSetTransform;
    _tpPathAddCircleFn pathAddCircle;
    _tpPathAddEllipseFn pathAddEllipse;
    _tpPathAddRectFn pathAddRect;
    _tpPathAddSegmentFn pathAddSegment;
    _tpPathMoveToFn pathMoveTo;
    _tpPathLineToFn pathLineTo;
    _tpPathCubicCurveToFn pathCubicCurveTo;
    _tpPathQuadraticCurveToFn pathQuadraticCurveTo;
    _tpPathCloseFn pathClose;
    _tpPathRemoveContourFn pathRemoveContour;
    _tpPathAddSegmentsFn pathAddSegments;
    _tpPathAddContourFn pathAddContour;

    _tpStyleCreateFn styleCreate;
    _tpStyleDestroyFn styleDestroy;
    _tpStyleSetDashArrayFn styleSetDashArray;
    _tpStyleSetDashOffsetFn styleSetDashOffset;
    _tpStyleSetFillColorFn styleSetFillColor;
    _tpStyleSetStrokeColorFn styleSetStrokeColor;
    _tpStyleSetFillGradientFn styleSetFillGradient;
    _tpStyleSetStrokeGradientFn styleSetStrokeGradient;
    _tpStyleSetStrokeWidthFn styleSetStrokeWidth;
    _tpStyleSetStrokeJoinFn styleSetStrokeJoin;
    _tpStyleSetScaleStrokeFn styleSetScaleStroke;
    _tpStyleSetMiterLimitFn styleSetMiterLimit;
    _tpStyleSetStrokeCapFn styleSetStrokeCap;
    _tpStyleSetFillRuleFn styleSetFillRule;
    _tpStyleRemoveFillFn styleRemoveFill;
    _tpStyleRemoveStrokeFn styleRemoveStroke;

    _tpGradientCreateLinearFn gradientCreateLinear;
    _tpGradientAddColorStopFn gradientAddColorStop;
    _tpGradientDestroyFn gradientDestroy;

    _tpContextInitFn contextInit;
    _tpContextDeallocateFn contextDeallocate;
    _tpPrepareDrawingFn prepareDrawing;
    _tpFinishDrawingFn finishDrawing;
    _tpSetProjectionFn setProjection;
    _tpDrawPathFn drawPath;
    _tpBeginClippingFn beginClipping;
    _tpEndClippingFn endClipping;
    _tpResetClippingFn resetClipping;
} tpImplementation;

// Color Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

TARP_API tpColor tpColorMake(tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a);

// Vector Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

TARP_API tpVec2 tpVec2Make(tpFloat _x, tpFloat _y);

TARP_API tpVec2 tpVec2Add(tpVec2 _a, tpVec2 _b);

TARP_API tpVec2 tpVec2Sub(tpVec2 _a, tpVec2 _b);

TARP_API tpVec2 tpVec2Mult(tpVec2 _a, tpVec2 _b);

TARP_API tpVec2 tpVec2MultScalar(tpVec2 _a, tpFloat _b);

TARP_API tpVec2 tpVec2Div(tpVec2 _a, tpVec2 _b);

TARP_API tpBool tpVec2Equals(tpVec2 _a, tpVec2 _b);

TARP_API tpFloat tpVec2Length(tpVec2 _vec);

TARP_API tpFloat tpVec2Dot(tpVec2 _a, tpVec2 _b);

TARP_API tpFloat tpVec2Cross(tpVec2 _a, tpVec2 _b);

TARP_API void tpVec2NormalizeSelf(tpVec2 * _vec);

TARP_API tpVec2 tpVec2Normalize(tpVec2 _vec);

TARP_API tpVec2 tpVec2Perp(tpVec2 _a);

TARP_API tpFloat tpVec2Distance(tpVec2 _a, tpVec2 _b);


// Matrix Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

TARP_API tpMat3 tpMat3Make(tpFloat _v0, tpFloat _v1, tpFloat _v2, //row1
                           tpFloat _v3, tpFloat _v4, tpFloat _v5, //row2
                           tpFloat _v6, tpFloat _v7, tpFloat _v8); //row3

TARP_API tpMat3 tpMat3MakeIdentity();

TARP_API tpMat3 tpMat3MakeTranslation(tpFloat _x, tpFloat _y);

TARP_API tpMat3 tpMat3MakeScale(tpFloat _x, tpFloat _y);

TARP_API tpMat3 tpMat3MakeSkew(tpFloat _x, tpFloat _y);

TARP_API tpMat3 tpMat3MakeRotation(tpFloat _angle);

TARP_API int tpMat3Decompose(const tpMat3 * _mat, tpVec2 * _outTranslation,
                             tpVec2 * _outScale, tpVec2 * _outSkew, tpFloat * _outRotation);

TARP_API tpBool tpMat3Equals(const tpMat3 * _a, const tpMat3 * _b);

TARP_API tpVec2 tpMat3MultVec2(const tpMat3 * _mat, tpVec2 _vec);

TARP_API tpMat3 tpMat3Mult(const tpMat3 * _a, const tpMat3 * _b);

TARP_API tpMat4 tpMat4Make(tpFloat _v0, tpFloat _v1, tpFloat _v2, tpFloat _v3, //row1
                           tpFloat _v4, tpFloat _v5, tpFloat _v6, tpFloat _v7, //row2
                           tpFloat _v8, tpFloat _v9, tpFloat _v10, tpFloat _v11, //row3
                           tpFloat _v12, tpFloat _v13, tpFloat _v14, tpFloat _v15); //row4

TARP_API tpMat4 tpMat4MakeIdentity();

TARP_API tpMat4 tpMat4MakeOrtho(tpFloat _left, tpFloat _right, tpFloat _bottom, tpFloat _top, tpFloat _near, tpFloat _far);

TARP_API tpMat4 tpMat4MakeFrom2DTransform(const tpMat3 * _transform);

TARP_API tpMat4 tpMat4Mult(const tpMat4 * _a, const tpMat4 * _b);


// Segment Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

TARP_API tpSegment tpSegmentMake(tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y);


// Path Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

TARP_API tpPath tpPathCreate();

TARP_API void tpPathDestroy(tpPath _path);

TARP_API tpBool tpPathClear(tpPath _path);

TARP_API tpBool tpPathSetTransform(tpPath _path, const tpMat3 * _transform);

TARP_API tpBool tpPathAddCircle(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _r);

TARP_API tpBool tpPathAddEllipse(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height);

TARP_API tpBool tpPathAddRect(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height);

TARP_API tpBool tpPathAddSegment(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y);

TARP_API tpBool tpPathMoveTo(tpPath _path, tpFloat _x, tpFloat _y);

TARP_API tpBool tpPathLineTo(tpPath _path, tpFloat _x, tpFloat _y);

TARP_API tpBool tpPathCubicCurveTo(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _h1x, tpFloat _h1y, tpFloat _px, tpFloat _py);

TARP_API tpBool tpPathQuadraticCurveTo(tpPath _path, tpFloat _hx, tpFloat _hy, tpFloat _px, tpFloat _py);

TARP_API tpBool tpPathClose(tpPath _path);

TARP_API tpBool tpPathRemoveContour(tpPath _path, int _index);

TARP_API tpBool tpPathAddSegments(tpPath _path, tpSegment * _segments, int _count);

TARP_API tpBool tpPathAddContour(tpPath _path, tpSegment * _segments, int _count, tpBool _bClosed);


// Style Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

TARP_API tpStyle tpStyleCreate();

TARP_API void tpStyleDestroy(tpStyle _style);

TARP_API void tpStyleSetDashArray(tpStyle _style, tpFloat * _dashArray, int _count);

TARP_API void tpStyleSetDashOffset(tpStyle _style, tpFloat _offset);

TARP_API void tpStyleSetFillColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a);

TARP_API void tpStyleSetStrokeColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a);

TARP_API void tpStyleSetFillGradient(tpStyle _style, const tpGradient _gradient);

TARP_API void tpStyleSetStrokeGradient(tpStyle _style, const tpGradient _gradient);

TARP_API void tpStyleSetStrokeWidth(tpStyle _style, tpFloat _strokeWidth);

TARP_API void tpStyleSetStrokeJoin(tpStyle _style, tpStrokeJoin _join);

TARP_API void tpStyleSetScaleStroke(tpStyle _style, tpBool _b);

TARP_API void tpStyleSetMiterLimit(tpStyle _style, tpFloat _limit);

TARP_API void tpStyleSetStrokeCap(tpStyle _style, tpStrokeCap _cap);

TARP_API void tpStyleSetFillRule(tpStyle _style, tpFillRule _fillType);

TARP_API void tpStyleRemoveFill(tpStyle _style);

TARP_API void tpStyleRemoveStroke(tpStyle _style);

TARP_API tpGradient tpGradientCreateLinear(tpFloat _x0, tpFloat _y0, tpFloat _x1, tpFloat _y1);

TARP_API void tpGradientAddColorStop(tpGradient _gradient, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a, tpFloat _offset);

TARP_API void tpGradientDestroy(tpGradient _gradient);


// Context Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

TARP_API tpBool tpContextInit(tpContext * _ctx);

TARP_API const char * tpContextErrorMessage(tpContext * _ctx);

TARP_API tpBool tpContextDeallocate(tpContext * _ctx);

TARP_API tpBool tpPrepareDrawing(tpContext * _ctx);

TARP_API tpBool tpFinishDrawing(tpContext * _ctx);

TARP_API tpBool tpSetProjection(tpContext * _ctx, const tpMat4 * _projection);

TARP_API tpBool tpDrawPath(tpContext * _ctx, tpPath _path, const tpStyle _style);

TARP_API tpBool tpBeginClipping(tpContext * _ctx, tpPath _path);

TARP_API tpBool tpEndClipping(tpContext * _ctx);

TARP_API tpBool tpResetClipping(tpContext * _ctx);


TARP_API tpImplementation * tpGetImplementation();

TARP_API void tpSetImplementation(tpImplementation _impl);

#ifdef TARP_IMPLEMENTATION

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

tpVec2 tpVec2Add(tpVec2 _a, tpVec2 _b)
{
    return (tpVec2) {_a.x + _b.x, _a.y + _b.y};
}

tpVec2 tpVec2Sub(tpVec2 _a, tpVec2 _b)
{
    return (tpVec2) {_a.x - _b.x, _a.y - _b.y};
}

tpVec2 tpVec2Mult(tpVec2 _a, tpVec2 _b)
{
    return (tpVec2) {_a.x * _b.x, _a.y * _b.y};
}

tpVec2 tpVec2MultScalar(tpVec2 _a, tpFloat _b)
{
    return (tpVec2) {_a.x * _b, _a.y * _b};
}

tpVec2 tpVec2Div(tpVec2 _a, tpVec2 _b)
{
    return (tpVec2) {_a.x / _b.x, _a.y / _b.y};
}

tpBool tpVec2Equals(tpVec2 _a, tpVec2 _b)
{
    return _a.x == _b.x && _a.y == _b.y;
}

tpFloat tpVec2Length(tpVec2 _vec)
{
    return sqrt(_vec.x * _vec.x + _vec.y * _vec.y);
}

void tpVec2NormalizeSelf(tpVec2 * _vec)
{
    tpFloat s = 1.0 / tpVec2Length(*_vec);
    _vec->x *= s;
    _vec->y *= s;
}

tpVec2 tpVec2Normalize(tpVec2 _vec)
{
    tpFloat s = 1.0 / tpVec2Length(_vec);
    return (tpVec2) {_vec.x * s, _vec.y * s};
}

tpVec2 tpVec2Perp(tpVec2 _a)
{
    return (tpVec2) {_a.y, -_a.x};
}

tpFloat tpVec2Dot(tpVec2 _a, tpVec2 _b)
{
    return _a.x * _b.x + _a.y * _b.y;
}

tpFloat tpVec2Cross(tpVec2 _a, tpVec2 _b)
{
    return _a.x * _b.y - _a.y * _b.x;
}

tpFloat tpVec2Distance(tpVec2 _a, tpVec2 _b)
{
    return tpVec2Length(tpVec2Sub(_a, _b));
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

tpMat3 tpMat3MakeIdentity()
{
    return (tpMat3)
    {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };
}

tpMat3 tpMat3MakeTranslation(tpFloat _x, tpFloat _y)
{
    return tpMat3Make(1, 0, _x,
                      0, 1, _y,
                      0, 0, 1);
}

tpMat3 tpMat3MakeScale(tpFloat _x, tpFloat _y)
{
    return tpMat3Make(_x, 0, 0,
                      0, _y, 0,
                      0, 0, 1);
}

tpMat3 tpMat3MakeSkew(tpFloat _x, tpFloat _y)
{
    return tpMat3Make(1, tan(_x), 0,
                      tan(_y), 1, 0,
                      0, 0, 1);
}

tpMat3 tpMat3MakeRotation(tpFloat _angle)
{
    tpFloat c = cos(_angle);
    tpFloat s = sin(_angle);
    return tpMat3Make(c, -s, 0,
                      s, c, 0,
                      0, 0, 1);
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

tpMat4 tpMat4MakeIdentity()
{
    return (tpMat4)
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

tpMat4 tpMat4MakeOrtho(tpFloat _left, tpFloat _right, tpFloat _bottom, tpFloat _top, tpFloat _near, tpFloat _far)
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
    return tpMat4Make(_transform->v[0], _transform->v[3], 0, _transform->v[6],
                      _transform->v[1], _transform->v[4], 0, _transform->v[7],
                      _transform->v[2], _transform->v[5], 1, 0,
                      0, 0, 0, 1.0);
}

tpMat4 tpMat4Mult(const tpMat4 * _a, const tpMat4 * _b)
{
    return (tpMat4)
    {
        //column one
        _b->v[0] * _a->v[0] + _b->v[1] * _a->v[4] + _b->v[2] * _a->v[8]  + _b->v[3] * _a->v[12],
        _b->v[0] * _a->v[1] + _b->v[1] * _a->v[5] + _b->v[2] * _a->v[9]  + _b->v[3] * _a->v[13],
        _b->v[0] * _a->v[2] + _b->v[1] * _a->v[6] + _b->v[2] * _a->v[10] + _b->v[3] * _a->v[14],
        _b->v[0] * _a->v[3] + _b->v[1] * _a->v[7] + _b->v[2] * _a->v[11] + _b->v[3] * _a->v[15],

        //column two
        _b->v[4] * _a->v[0] + _b->v[5] * _a->v[4] + _b->v[6] * _a->v[8]  + _b->v[7] * _a->v[12],
        _b->v[4] * _a->v[1] + _b->v[5] * _a->v[5] + _b->v[6] * _a->v[9]  + _b->v[7] * _a->v[13],
        _b->v[4] * _a->v[2] + _b->v[5] * _a->v[6] + _b->v[6] * _a->v[10] + _b->v[7] * _a->v[14],
        _b->v[4] * _a->v[3] + _b->v[5] * _a->v[7] + _b->v[6] * _a->v[11] + _b->v[7] * _a->v[15],

        //column three
        _b->v[8] * _a->v[0] + _b->v[9] * _a->v[4] + _b->v[10] * _a->v[8]  + _b->v[11] * _a->v[12],
        _b->v[8] * _a->v[1] + _b->v[9] * _a->v[5] + _b->v[10] * _a->v[9]  + _b->v[11] * _a->v[13],
        _b->v[8] * _a->v[2] + _b->v[9] * _a->v[6] + _b->v[10] * _a->v[10] + _b->v[11] * _a->v[14],
        _b->v[8] * _a->v[3] + _b->v[9] * _a->v[7] + _b->v[10] * _a->v[11] + _b->v[11] * _a->v[15],

        //column four
        _b->v[12] * _a->v[0] + _b->v[13] * _a->v[4] + _b->v[14] * _a->v[8]  + _b->v[15] * _a->v[12],
        _b->v[12] * _a->v[1] + _b->v[13] * _a->v[5] + _b->v[14] * _a->v[9]  + _b->v[15] * _a->v[13],
        _b->v[12] * _a->v[2] + _b->v[13] * _a->v[6] + _b->v[14] * _a->v[10] + _b->v[15] * _a->v[14],
        _b->v[12] * _a->v[3] + _b->v[13] * _a->v[7] + _b->v[14] * _a->v[11] + _b->v[15] * _a->v[15]
    };
}

tpVec2 tpMat3MultVec2(const tpMat3 * _mat, tpVec2 _vec)
{
    return (tpVec2)
    {
        (_vec.x * _mat->v[0] + _vec.y * _mat->v[3] + _mat->v[6]),
        (_vec.x * _mat->v[1] + _vec.y * _mat->v[4] + _mat->v[7])
    };
}

tpMat3 tpMat3Mult(const tpMat3 * _a, const tpMat3 * _b)
{
    return (tpMat3)
    {
        //column one
        _b->v[0] * _a->v[0] + _b->v[1] * _a->v[3] + _b->v[2] * _a->v[6],
        _b->v[0] * _a->v[1] + _b->v[1] * _a->v[4] + _b->v[2] * _a->v[7],
        _b->v[0] * _a->v[2] + _b->v[1] * _a->v[5] + _b->v[2] * _a->v[8],

        //column two
        _b->v[3] * _a->v[0] + _b->v[4] * _a->v[3] + _b->v[5] * _a->v[6],
        _b->v[3] * _a->v[1] + _b->v[4] * _a->v[4] + _b->v[5] * _a->v[7],
        _b->v[3] * _a->v[2] + _b->v[4] * _a->v[5] + _b->v[5] * _a->v[8],

        //column three
        _b->v[6] * _a->v[0] + _b->v[7] * _a->v[3] + _b->v[8] * _a->v[6],
        _b->v[6] * _a->v[1] + _b->v[7] * _a->v[4] + _b->v[8] * _a->v[7],
        _b->v[6] * _a->v[2] + _b->v[7] * _a->v[5] + _b->v[8] * _a->v[8]
    };
}

tpSegment tpSegmentMake(tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y)
{
    return (tpSegment)
    {
        {_h0x, _h0y},
        {_px, _py},
        {_h1x, _h1y}
    };
}

TARP_API tpImplementation * tpGetImplementation()
{
    static tpImplementation s_impl;
    return &s_impl;
}

TARP_API void tpSetImplementation(tpImplementation _impl)
{
    *tpGetImplementation() = _impl;
}

TARP_API tpPath tpPathCreate()
{
    return tpGetImplementation()->pathCreate();
}

TARP_API void tpPathDestroy(tpPath _path)
{
    tpGetImplementation()->pathDestroy(_path);
}

TARP_API tpBool tpPathClear(tpPath _path)
{
    return tpGetImplementation()->pathClear(_path);
}

TARP_API tpBool tpPathSetTransform(tpPath _path, const tpMat3 * _transform)
{
    return tpGetImplementation()->pathSetTransform(_path, _transform);
}

TARP_API tpBool tpPathAddCircle(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _r)
{
    return tpGetImplementation()->pathAddCircle(_path, _x, _y, _r);
}

TARP_API tpBool tpPathAddEllipse(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height)
{
    return tpGetImplementation()->pathAddEllipse(_path, _x, _y, _width, _height);
}

TARP_API tpBool tpPathAddRect(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height)
{
    return tpGetImplementation()->pathAddRect(_path, _x, _y, _width, _height);
}

TARP_API tpBool tpPathAddSegment(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y)
{
    return tpGetImplementation()->pathAddSegment(_path, _h0x, _h0y, _px, _py, _h1x, _h1y);
}

TARP_API tpBool tpPathMoveTo(tpPath _path, tpFloat _x, tpFloat _y)
{
    return tpGetImplementation()->pathMoveTo(_path, _x, _y);
}

TARP_API tpBool tpPathLineTo(tpPath _path, tpFloat _x, tpFloat _y)
{
    return tpGetImplementation()->pathLineTo(_path, _x, _y);
}

TARP_API tpBool tpPathCubicCurveTo(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _h1x, tpFloat _h1y, tpFloat _px, tpFloat _py)
{
    return tpGetImplementation()->pathCubicCurveTo(_path, _h0x, _h0y, _h1x, _h1y, _px, _py);
}

TARP_API tpBool tpPathQuadraticCurveTo(tpPath _path, tpFloat _hx, tpFloat _hy, tpFloat _px, tpFloat _py)
{
    return tpGetImplementation()->pathQuadraticCurveTo(_path, _hx, _hy, _px, _py);
}

TARP_API tpBool tpPathClose(tpPath _path)
{
    return tpGetImplementation()->pathClose(_path);
}

TARP_API tpBool tpPathRemoveContour(tpPath _path, int _index)
{
    return tpGetImplementation()->pathRemoveContour(_path, _index);
}

TARP_API tpBool tpPathAddSegments(tpPath _path, tpSegment * _segments, int _count)
{
    return tpGetImplementation()->pathAddSegments(_path, _segments, _count);
}

TARP_API tpBool tpPathAddContour(tpPath _path, tpSegment * _segments, int _count, tpBool _bClosed)
{
    return tpGetImplementation()->pathAddContour(_path, _segments, _count, _bClosed);
}

TARP_API tpStyle tpStyleCreate()
{
    return tpGetImplementation()->styleCreate();
}

TARP_API void tpStyleDestroy(tpStyle _style)
{
    tpGetImplementation()->styleDestroy(_style);
}

TARP_API void tpStyleSetDashArray(tpStyle _style, tpFloat * _dashArray, int _count)
{
    tpGetImplementation()->styleSetDashArray(_style, _dashArray, _count);
}

TARP_API void tpStyleSetDashOffset(tpStyle _style, tpFloat _offset)
{
    tpGetImplementation()->styleSetDashOffset(_style, _offset);
}

TARP_API void tpStyleSetFillColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a)
{
    tpGetImplementation()->styleSetFillColor(_style, _r, _g, _b, _a);
}

TARP_API void tpStyleSetStrokeColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a)
{
    tpGetImplementation()->styleSetStrokeColor(_style, _r, _g, _b, _a);
}

TARP_API void tpStyleSetFillGradient(tpStyle _style, const tpGradient _gradient)
{
    tpGetImplementation()->styleSetFillGradient(_style, _gradient);
}

TARP_API void tpStyleSetStrokeGradient(tpStyle _style, const tpGradient _gradient)
{
    tpGetImplementation()->styleSetStrokeGradient(_style, _gradient);
}

TARP_API void tpStyleSetStrokeWidth(tpStyle _style, tpFloat _strokeWidth)
{
    tpGetImplementation()->styleSetStrokeWidth(_style, _strokeWidth);
}

TARP_API void tpStyleSetStrokeJoin(tpStyle _style, tpStrokeJoin _join)
{
    tpGetImplementation()->styleSetStrokeJoin(_style, _join);
}

TARP_API void tpStyleSetScaleStroke(tpStyle _style, tpBool _b)
{
    tpGetImplementation()->styleSetScaleStroke(_style, _b);
}

TARP_API void tpStyleSetMiterLimit(tpStyle _style, tpFloat _limit)
{
    tpGetImplementation()->styleSetMiterLimit(_style, _limit);
}

TARP_API void tpStyleSetStrokeCap(tpStyle _style, tpStrokeCap _cap)
{
    tpGetImplementation()->styleSetStrokeCap(_style, _cap);
}

TARP_API void tpStyleSetFillRule(tpStyle _style, tpFillRule _fillType)
{
    tpGetImplementation()->styleSetFillRule(_style, _fillType);
}

TARP_API void tpStyleRemoveFill(tpStyle _style)
{
    tpGetImplementation()->styleRemoveFill(_style);
}

TARP_API void tpStyleRemoveStroke(tpStyle _style)
{
    tpGetImplementation()->styleRemoveStroke(_style);
}

TARP_API tpGradient tpGradientCreateLinear(tpFloat _x0, tpFloat _y0, tpFloat _x1, tpFloat _y1)
{
    return tpGetImplementation()->gradientCreateLinear(_x0, _y0, _x1, _y1);
}

TARP_API void tpGradientAddColorStop(tpGradient _gradient, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a, tpFloat _offset)
{
    tpGetImplementation()->gradientAddColorStop(_gradient, _r, _g, _b, _a, _offset);
}

TARP_API void tpGradientDestroy(tpGradient _gradient)
{
    tpGetImplementation()->gradientDestroy(_gradient);
}

TARP_API tpBool tpContextInit(tpContext * _context)
{
    return tpGetImplementation()->contextInit(_context);
}

TARP_API const char * tpContextErrorMessage(tpContext * _ctx)
{
    return _ctx->errorMessage;
}

TARP_API tpBool tpContextDeallocate(tpContext * _ctx)
{
    return tpGetImplementation()->contextDeallocate(_ctx);
}

TARP_API tpBool tpPrepareDrawing(tpContext * _ctx)
{
    return tpGetImplementation()->prepareDrawing(_ctx);
}

TARP_API tpBool tpFinishDrawing(tpContext * _ctx)
{
    return tpGetImplementation()->finishDrawing(_ctx);
}

TARP_API tpBool tpSetProjection(tpContext * _ctx, const tpMat4 * _projection)
{
    return tpGetImplementation()->setProjection(_ctx, _projection);
}

TARP_API tpBool tpDrawPath(tpContext * _ctx, tpPath _path, const tpStyle _style)
{
    return tpGetImplementation()->drawPath(_ctx, _path, _style);
}

TARP_API tpBool tpBeginClipping(tpContext * _ctx, tpPath _path)
{
    return tpGetImplementation()->beginClipping(_ctx, _path);
}

TARP_API tpBool tpEndClipping(tpContext * _ctx)
{
    return tpGetImplementation()->endClipping(_ctx);
}

TARP_API tpBool tpResetClipping(tpContext * _ctx)
{
    return tpGetImplementation()->resetClipping(_ctx);
}

#endif //TARP_IMPLEMENTATION

#ifdef TARP_INCLUDE_OPENGL_IMPLEMENTATION

TARP_API tpImplementation tpOpenGLImplementation();

#ifdef TARP_IMPLEMENTATION
//The shader programs used by the renderer
static const char * _vertexShaderCode =
    "#version 150 \n"
    "uniform mat4 transformProjection; \n"
    "uniform vec4 meshColor; \n"
    "in vec2 vertex; \n"
    "out vec4 icol;\n"
    "void main() \n"
    "{ \n"
    "gl_Position = transformProjection * vec4(vertex, 0.0, 1.0); \n"
    "icol = meshColor;\n"
    "} \n";

static const char * _fragmentShaderCode =
    "#version 150 \n"
    "in vec4 icol; \n"
    "out vec4 pixelColor; \n"
    "void main() \n"
    "{ \n"
    "pixelColor = icol; \n"
    "} \n";

static const char * _vertexShaderCodeTexture =
    "#version 150 \n"
    "uniform mat4 transformProjection; \n"
    "in vec2 vertex; \n"
    "in float tc; \n"
    "out float itc;\n"
    "void main() \n"
    "{ \n"
    "gl_Position = transformProjection * vec4(vertex, 0.0, 1.0); \n"
    "itc = tc; \n"
    "} \n";

static const char * _fragmentShaderCodeTexture =
    "#version 150 \n"
    "uniform sampler1D tex;\n"
    "in float itc; \n"
    "out vec4 pixelColor; \n"
    "void main() \n"
    "{ \n"
    "pixelColor = texture(tex, itc); \n"
    // "pixelColor = vec4(0, 0, 0, 1.0);\n"
    "} \n";

typedef struct _tpGLContext _tpGLContext;

typedef enum
{
    //The mask values are important! FillRaster needs to have at least
    //2 bits. These need to be the lower bits in order to work with the
    //Increment, Decrement stencil operations
    //http://www.opengl.org/discussion_boards/showthread.php/149740-glStencilOp-s-GL_INCR-GL_DECR-behaviour-when-masked
    _kTpGLFillRasterStencilPlane = 0x1F, //binary mask    00011111
    _kTpGLClippingStencilPlaneOne = 1 << 5, //binary mask     00100000
    _kTpGLClippingStencilPlaneTwo = 1 << 6, //binary mask     01000000
    _kTpGLStrokeRasterStencilPlane = 1 << 7 //binary mask 10000000
} _tpGLStencilPlane;

typedef struct
{
    tpVec2 min, max;
} _tpGLRect;

typedef struct
{
    tpVec2 p0, h0, h1, p1;
} _tpGLCurve;

typedef struct
{
    _tpGLCurve first, second;
} _tpGLCurvePair;

#define _TARP_ARRAY_T _tpFloatArray
#define _TARP_ITEM_T tpFloat
#include <Tarp/TarpArray.h>

#define _TARP_ARRAY_T _tpBoolArray
#define _TARP_ITEM_T tpBool
#include <Tarp/TarpArray.h>

#define _TARP_ARRAY_T _tpSegmentArray
#define _TARP_ITEM_T tpSegment
#define _TARP_COMPARATOR_T 0
#include <Tarp/TarpArray.h>

#define _TARP_ARRAY_T _tpVec2Array
#define _TARP_ITEM_T tpVec2
#define _TARP_COMPARATOR_T 0
#include <Tarp/TarpArray.h>

#define _TARP_ARRAY_T _tpColorStopArray
#define _TARP_ITEM_T tpColorStop
#define _TARP_COMPARATOR_T 0
#include <Tarp/TarpArray.h>

typedef struct
{
    _tpSegmentArray segments;
    tpBool bDirty;
    int lastSegmentIndex;
    tpBool bIsClosed;

    //some rendering specific data
    int fillVertexOffset;
    int fillVertexCount;
    int strokeVertexOffset;
    int strokeVertexCount;

    _tpGLRect bounds;
} _tpGLContour;

#define _TARP_ARRAY_T _tpGLContourArray
#define _TARP_ITEM_T _tpGLContour
#define _TARP_COMPARATOR_T 0
#include <Tarp/TarpArray.h>

typedef struct
{
    tpVec2 vertex;
    tpVec2 tc;
} _tpGLTextureVertex;

#define _TARP_ARRAY_T _tpGLTextureVertexArray
#define _TARP_ITEM_T _tpGLTextureVertex
#define _TARP_COMPARATOR_T 0
#include <Tarp/TarpArray.h>

typedef struct
{
    tpPaintType strokeType;
    tpFloat strokeWidth;
    tpFloat dashOffset;
    tpFloat dashArray[TARP_MAX_DASH_ARRAY_SIZE];
    int dashCount;
    tpStrokeJoin join;
    tpStrokeCap cap;
    tpBool bScaleStroke;
} _tpGLStrokeData;

typedef struct
{
    int gradientID;
    tpVec2 origin;
    tpVec2 destination;
    _tpColorStopArray stops;
    tpGradientType type;

    //rendering specific data/caches
    tpBool bDirty;
    GLuint rampTexture;
} _tpGLGradient;

typedef struct
{
    int lastGradientID;
    _tpGLRect * bounds; //the bounds used by this gradient (i.e. stroke or fill)
    int vertexOffset;
    int vertexCount;

} _tpGLGradientCacheData;

typedef struct
{
    _tpGLContourArray contours;
    int currentContourIndex;
    char errorMessage[TARP_GL_ERROR_MESSAGE_SIZE];
    tpMat3 transform;

    //rendering specific data/caches
    _tpVec2Array geometryCache;
    _tpGLTextureVertexArray textureGeometryCache;
    _tpBoolArray jointCache;

    tpBool bPathGeometryDirty;
    tpFloat lastTransformScale;
    tpMat4 renderTransform;
    tpMat4 transformProjection;
    tpBool bTransformProjDirty;
    _tpGLRect boundsCache;
    _tpGLRect strokeBoundsCache;

    //holds the properties that the stroke geometry
    //was generated with.
    _tpGLStrokeData lastStroke;

    _tpGLGradientCacheData fillGradientData;
    _tpGLGradientCacheData strokeGradientData;

    int strokeVertexOffset;
    int strokeVertexCount;
    int boundsVertexOffset;

    _tpGLContext * lastDrawContext;
    int lastProjectionID;
} _tpGLPath;

typedef union
{
    tpGradient gradient;
    tpColor color;
} _tpGLPaintUnion;

typedef struct
{
    _tpGLPaintUnion data;
    tpPaintType type;
} _tpGLPaint;

typedef struct
{
    _tpGLPaint fill;
    _tpGLPaint stroke;
    tpFloat strokeWidth;
    tpStrokeCap strokeCap;
    tpStrokeJoin strokeJoin;
    tpFillRule fillRule;
    tpFloat dashArray[TARP_MAX_DASH_ARRAY_SIZE];
    int dashCount;
    tpFloat dashOffset;
    tpFloat miterLimit;
    tpBool bScaleStroke;
} _tpGLStyle;

#define _TARP_ARRAY_T _tpGLPathPtrArray
#define _TARP_ITEM_T _tpGLPath *
#include <Tarp/TarpArray.h>

#define _TARP_ARRAY_T _tpGLStylePtrArray
#define _TARP_ITEM_T _tpGLStyle *
#include <Tarp/TarpArray.h>

#define _TARP_ARRAY_T _tpGLGradientPtrArray
#define _TARP_ITEM_T _tpGLGradient *
#include <Tarp/TarpArray.h>

typedef struct
{
    GLuint vao;
    GLuint vbo;
    GLuint vboSize;
} _tpGLVAO;

struct _tpGLContext
{
    GLuint program;
    GLuint textureProgram;

    _tpGLVAO vao;
    _tpGLVAO textureVao;

    _tpGLPath * clippingStack[TARP_GL_MAX_CLIPPING_STACK_DEPTH];
    int clippingStackDepth;
    int currentClipStencilPlane;
    tpBool bCanSwapStencilPlanes;
    tpMat4 projection;
    int projectionID;
    // _tpGLPathPtrArray paths;
    // _tpGLStylePtrArray styles;
    // _tpGLGradientPtrArray gradients;
    tpStyle clippingStyle;

    //used to temporarily store vertex/stroke data (think double buffering)
    _tpVec2Array tmpVertices;
    _tpBoolArray tmpJoints;

    //same helper for texture vertices
    _tpGLTextureVertexArray tmpTexVertices;

    _tpColorStopArray tmpColorStops;
    char errorMessage[TARP_GL_ERROR_MESSAGE_SIZE];
};

typedef struct _tpGLContext _tpGLContext;

typedef struct
{
    char message[TARP_GL_ERROR_MESSAGE_SIZE];
    int length;
} _ErrorMessage;

void _tpGLGradientCacheDataInit(_tpGLGradientCacheData * _gd, _tpGLRect * _bounds)
{
    _gd->lastGradientID = -1;
    _gd->bounds = _bounds;
    _gd->vertexOffset = 0;
    _gd->vertexCount = 0;
}

tpBool _compileShader(const char * _shaderCode, GLenum _shaderType, GLuint * _outHandle, _ErrorMessage * _outError)
{
    GLenum glHandle = glCreateShader(_shaderType);
    GLint len = strlen(_shaderCode);
    _TARP_ASSERT_NO_GL_ERROR(glShaderSource(glHandle, 1, &_shaderCode, &len));
    _TARP_ASSERT_NO_GL_ERROR(glCompileShader(glHandle));

    //check if the shader compiled
    GLint state;
    _TARP_ASSERT_NO_GL_ERROR(glGetShaderiv(glHandle, GL_COMPILE_STATUS, &state));
    if (state == GL_FALSE)
    {
        if (_outError)
        {
            GLint infologLength;
            _TARP_ASSERT_NO_GL_ERROR(glGetShaderiv(glHandle, GL_INFO_LOG_LENGTH, &infologLength));
            infologLength = TARP_MIN(TARP_GL_ERROR_MESSAGE_SIZE, infologLength);
            _TARP_ASSERT_NO_GL_ERROR(glGetShaderInfoLog(glHandle, infologLength, &infologLength, _outError->message));
            _outError->length = infologLength;
        }

        glDeleteShader(glHandle);
        return tpTrue;
    }
    else
    {
        *_outHandle = glHandle;
    }
    return tpFalse;
}

tpBool _createProgram(const char * _vertexShader, const char * _fragmentShader, int _bTexProgram, GLuint * _outHandle, _ErrorMessage * _outError)
{
    GLuint vertexShader, fragmentShader;
    tpBool err = _compileShader(_vertexShader, GL_VERTEX_SHADER, &vertexShader, _outError);
    if (!err)
    {
        err = _compileShader(_fragmentShader, GL_FRAGMENT_SHADER, &fragmentShader, _outError);
    }
    if (err) return err;

    GLuint program = glCreateProgram();
    _TARP_ASSERT_NO_GL_ERROR(glAttachShader(program, vertexShader));
    _TARP_ASSERT_NO_GL_ERROR(glAttachShader(program, fragmentShader));

    //bind the attribute locations from the layout
    /*const BufferLayout::BufferElementArray & elements = _settings.vertexLayout.elements();
    auto sit = elements.begin();

    for (; sit != elements.end(); ++sit)
    {
        _TARP_ASSERT_NO_GL_ERROR(glBindAttribLocation(program, (*sit).m_location, (*sit).m_name.cString()));
    }*/

    _TARP_ASSERT_NO_GL_ERROR(glBindAttribLocation(program, 0, "vertex"));
    if (_bTexProgram)
        _TARP_ASSERT_NO_GL_ERROR(glBindAttribLocation(program, 1, "tc"));

    _TARP_ASSERT_NO_GL_ERROR(glLinkProgram(program));

    //check if we had success
    GLint state;
    _TARP_ASSERT_NO_GL_ERROR(glGetProgramiv(program, GL_LINK_STATUS, &state));

    if (state == GL_FALSE)
    {
        if (_outError)
        {
            GLint infologLength;
            _TARP_ASSERT_NO_GL_ERROR(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength));
            infologLength = TARP_MIN(TARP_GL_ERROR_MESSAGE_SIZE, infologLength);
            _TARP_ASSERT_NO_GL_ERROR(glGetProgramInfoLog(program, infologLength, &infologLength, _outError->message));
            _outError->length = infologLength;
        }

        return tpTrue;
    }

    _TARP_ASSERT_NO_GL_ERROR(glDeleteShader(vertexShader));
    _TARP_ASSERT_NO_GL_ERROR(glDeleteShader(fragmentShader));

    if (err)
    {
        glDeleteProgram(program);
    }
    else
    {
        *_outHandle = program;
    }

    return tpFalse;
}

tpBool _tpGLContextInit(tpContext * _ctx)
{
    _ErrorMessage msg;
    _tpGLContext * ctx;
    tpBool ret = tpFalse;

    ctx = (_tpGLContext *)malloc(sizeof(_tpGLContext));
    assert(ctx);

    ret = _createProgram(_vertexShaderCode, _fragmentShaderCode, 0, &ctx->program, &msg);
    if (ret)
    {
        strcpy(_ctx->errorMessage, msg.message);
        return ret;
    }
    ret = _createProgram(_vertexShaderCodeTexture, _fragmentShaderCodeTexture, 1, &ctx->textureProgram, &msg);
    if (ret)
    {
        strcpy(_ctx->errorMessage, msg.message);
        return ret;
    }

    _TARP_ASSERT_NO_GL_ERROR(glGenVertexArrays(1, &ctx->vao.vao));
    _TARP_ASSERT_NO_GL_ERROR(glBindVertexArray(ctx->vao.vao));
    _TARP_ASSERT_NO_GL_ERROR(glGenBuffers(1, &ctx->vao.vbo));
    ctx->vao.vboSize = 0;
    _TARP_ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, ctx->vao.vbo));
    _TARP_ASSERT_NO_GL_ERROR(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, ((char *)0)));
    _TARP_ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(0));

    _TARP_ASSERT_NO_GL_ERROR(glGenVertexArrays(1, &ctx->textureVao.vao));
    _TARP_ASSERT_NO_GL_ERROR(glBindVertexArray(ctx->textureVao.vao));
    _TARP_ASSERT_NO_GL_ERROR(glGenBuffers(1, &ctx->textureVao.vbo));
    ctx->textureVao.vboSize = 0;
    _TARP_ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, ctx->textureVao.vbo));
    _TARP_ASSERT_NO_GL_ERROR(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(tpFloat), ((char *)0)));
    _TARP_ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(0));
    _TARP_ASSERT_NO_GL_ERROR(glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(tpFloat), ((char *)(2 * sizeof(float)))));
    _TARP_ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(1));

    // _TARP_ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->vbo));
    // memset(_ctx->clippingStack, 0, sizeof(_ctx->clippingStack));
    ctx->clippingStackDepth = 0;
    ctx->currentClipStencilPlane = _kTpGLClippingStencilPlaneOne;
    ctx->bCanSwapStencilPlanes = tpTrue;
    ctx->projection = tpMat4MakeIdentity();
    ctx->projectionID = 0;

    // _tpGLPathPtrArrayInit(&ctx->paths, 32);
    // _tpGLGradientPtrArrayInit(&ctx->gradients, 8);
    // _tpGLStylePtrArrayInit(&ctx->styles, 8);
    _tpVec2ArrayInit(&ctx->tmpVertices, 512);
    _tpBoolArrayInit(&ctx->tmpJoints, 256);
    _tpGLTextureVertexArrayInit(&ctx->tmpTexVertices, 64);
    _tpColorStopArrayInit(&ctx->tmpColorStops, 16);

    _ctx->_impl = ctx;
    ctx->clippingStyle = tpStyleCreate(_ctx);
    tpStyleRemoveStroke(ctx->clippingStyle);

    return ret;
}

tpBool _tpGLContextDeallocate(tpContext * _ctx)
{
    int i;

    _tpGLContext * ctx = (_tpGLContext *)_ctx->_impl;

    //free all opengl resources
    glDeleteProgram(ctx->program);
    glDeleteBuffers(1, &ctx->vao.vbo);
    glDeleteVertexArrays(1, &ctx->vao.vao);
    glDeleteProgram(ctx->textureProgram);
    glDeleteBuffers(1, &ctx->textureVao.vbo);
    glDeleteVertexArrays(1, &ctx->textureVao.vao);

    // // deallocate all gradients
    // for (i = 0; i < ctx->gradients.count; ++i)
    // {
    //     tpGradientDestroy((tpGradient) {_tpGLGradientPtrArrayAt(&ctx->gradients, i)});
    // }
    // _tpGLGradientPtrArrayDeallocate(&ctx->gradients);

    // // deallocate all styles
    // for (i = 0; i < ctx->styles.count; ++i)
    // {
    //     tpStyleDestroy((tpStyle) {_tpGLStylePtrArrayAt(&ctx->styles, i)});
    // }
    // _tpGLStylePtrArrayDeallocate(&ctx->styles);

    // // deallocate all paths
    // for (i = 0; i < ctx->paths.count; ++i)
    // {
    //     tpPathDestroy((tpPath) {_tpGLPathPtrArrayAt(&ctx->paths, i)});
    // }
    // _tpGLPathPtrArrayDeallocate(&ctx->paths);

    _tpBoolArrayDeallocate(&ctx->tmpJoints);
    _tpVec2ArrayDeallocate(&ctx->tmpVertices);
    _tpGLTextureVertexArrayDeallocate(&ctx->tmpTexVertices);
    _tpColorStopArrayDeallocate(&ctx->tmpColorStops);

    return tpFalse;
}

// tpBool _tpGLIsValidPath(_tpGLPath * _path)
// {
//     return _tpGLPathPtrArrayFind(&_path->context->paths, _path) != -1;
// }

// tpBool _tpGLIsValidStyle(_tpGLStyle * _style)
// {
//     return _tpGLStylePtrArrayFind(&_style->context->styles, _style) != -1;
// }

// tpBool _tpGLIsValidGradient(_tpGLGradient * _gradient)
// {
//     return _tpGLGradientPtrArrayFind(&_gradient->context->gradients, _gradient) != -1;
// }

tpPath _tpGLPathCreate()
{
    // _tpGLContext * ctx = (_tpGLContext *)_ctx->_impl;

    _tpGLPath * path = malloc(sizeof(_tpGLPath));
    _tpGLContourArrayInit(&path->contours, 4);
    path->currentContourIndex = -1;
    memset(path->errorMessage, 0, sizeof(path->errorMessage));
    path->transform = tpMat3MakeIdentity();

    _tpVec2ArrayInit(&path->geometryCache, 128);
    _tpGLTextureVertexArrayInit(&path->textureGeometryCache, 32);
    _tpBoolArrayInit(&path->jointCache, 128);
    path->bPathGeometryDirty = tpTrue;
    path->lastTransformScale = 1.0;
    path->renderTransform = tpMat4MakeIdentity();
    path->transformProjection = tpMat4MakeIdentity();
    path->bTransformProjDirty = tpTrue;

    path->strokeVertexOffset = 0;
    path->strokeVertexCount = 0;
    path->boundsVertexOffset = 0;

    _tpGLGradientCacheDataInit(&path->fillGradientData, &path->boundsCache);
    _tpGLGradientCacheDataInit(&path->strokeGradientData, &path->strokeBoundsCache);

    // path->context = ctx;
    // _tpGLPathPtrArrayAppend(&ctx->paths, path);

    path->lastDrawContext = NULL;
    path->lastProjectionID = 0;

    return (tpPath) {path};
}

void _tpGLPathDestroy(tpPath _path)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));
    // _tpGLPathPtrArrayRemoveValue(&p->context->paths, p);
    _tpVec2ArrayDeallocate(&p->geometryCache);
    _tpGLTextureVertexArrayDeallocate(&p->textureGeometryCache);
    _tpBoolArrayDeallocate(&p->jointCache);
    for (int i = 0; i < p->contours.count; ++i)
    {
        _tpSegmentArrayDeallocate(&_tpGLContourArrayAtPtr(&p->contours, i)->segments);
    }
    _tpGLContourArrayDeallocate(&p->contours);
    free(p);
}

_tpGLContour * _tpGLCurrentContour(_tpGLPath * _p)
{
    if (_p->currentContourIndex != -1)
        return _tpGLContourArrayAtPtr(&_p->contours, _p->currentContourIndex);
    return NULL;
}

_tpGLContour * _tpGLPathCreateNextContour(_tpGLPath * _p)
{
    _tpGLContour contour;
    _tpSegmentArrayInit(&contour.segments, 8);
    contour.bDirty = tpTrue;
    contour.lastSegmentIndex = -1;
    contour.bIsClosed = tpFalse;
    contour.fillVertexOffset = 0;
    contour.fillVertexCount = 0;
    contour.strokeVertexOffset = 0;
    contour.strokeVertexCount = 0;
    _p->currentContourIndex = _p->contours.count;
    _tpGLContourArrayAppend(&_p->contours, contour);
    return _tpGLContourArrayAtPtr(&_p->contours, _p->currentContourIndex);
}

_tpGLContour * _tpGLPathNextEmptyContour(_tpGLPath * _path)
{
    _tpGLContour * c = _tpGLCurrentContour(_path);
    if (!c || c->segments.count)
    {
        c = _tpGLPathCreateNextContour(_path);
    }
    return c;
}

tpBool _tpGLContourAddSegments(_tpGLPath * _p, _tpGLContour * _c, tpSegment * _segments, int count)
{
    int err = _tpSegmentArrayAppendArray(&_c->segments, _segments, count);
    if (err)
    {
        strcpy(_p->errorMessage, "Could not allocate memory for segments.");
        return tpTrue;
    }

    _c->lastSegmentIndex = _c->segments.count - 1;
    _c->bDirty = tpTrue;
    _p->bPathGeometryDirty = tpTrue;

    return tpFalse;
}

tpBool _tpGLContourAddSegment(_tpGLPath * _p, _tpGLContour * _c,
                              tpFloat _h0x, tpFloat _h0y,
                              tpFloat _px, tpFloat _py,
                              tpFloat _h1x, tpFloat _h1y)
{
    tpSegment seg = (tpSegment) { {_h0x, _h0y}, {_px, _py}, {_h1x, _h1y}};
    return _tpGLContourAddSegments(_p, _c, &seg, 1);
}

tpBool _tpGLPathAddSegment(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c)
    {
        c = _tpGLPathCreateNextContour(p);
        assert(c);
    }

    if (!c)
        return tpTrue;

    return _tpGLContourAddSegment(p, c, _h0x, _h0y, _px, _py, _h1x, _h1y);
}

tpBool _tpGLPathLineTo(tpPath _path, tpFloat _x, tpFloat _y)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c || c->lastSegmentIndex == -1)
    {
        strcpy(p->errorMessage, "You have to start a contour before issuing this command (see tpPathMoveTo).");
        return tpTrue;
    }

    return _tpGLContourAddSegment(p, c, _x, _y, _x, _y, _x, _y);
}

tpBool _tpGLPathMoveTo(tpPath _path, tpFloat _x, tpFloat _y)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));

    _tpGLContour * c = _tpGLPathNextEmptyContour(p);

    return _tpGLContourAddSegment(p, c, _x, _y, _x, _y, _x, _y);
}

tpBool _tpGLPathClear(tpPath _path)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));
    p->currentContourIndex = -1;
    for (int i = 0; i < p->contours.count; ++i)
    {
        _tpSegmentArrayDeallocate(&_tpGLContourArrayAtPtr(&p->contours, i)->segments);
    }
    _tpGLContourArrayClear(&p->contours);
    p->bPathGeometryDirty = tpTrue;

    return tpFalse;
}

tpBool _tpGLPathCubicCurveTo(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _h1x, tpFloat _h1y, tpFloat _px, tpFloat _py)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c || c->lastSegmentIndex == -1)
    {
        strcpy(p->errorMessage, "You have to start a contour before issuing this command (see tpPathMoveTo).");
        return tpTrue;
    }

    _tpSegmentArrayAtPtr(&c->segments, c->lastSegmentIndex)->handleOut = (tpVec2) {_h0x, _h0y};
    return _tpGLContourAddSegment(p, c, _h1x, _h1y, _px, _py, _px, _py);
}

tpBool _tpGLPathQuadraticCurveTo(tpPath _path, tpFloat _hx, tpFloat _hy, tpFloat _px, tpFloat _py)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c || c->lastSegmentIndex == -1)
    {
        strcpy(p->errorMessage, "You have to start a contour before issuing this command (see tpPathMoveTo).");
        return tpTrue;
    }

    _tpSegmentArrayAtPtr(&c->segments, c->lastSegmentIndex)->handleOut = (tpVec2) {_hx, _hy};
    return _tpGLContourAddSegment(p, c, _hx, _hy, _px, _py, _px, _py);
}

tpBool _tpGLPathClose(tpPath _path)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (c->segments.count > 1)
    {
        c->bIsClosed = tpTrue;
        c->bDirty = tpTrue;
        p->currentContourIndex = -1;
        p->bPathGeometryDirty = tpTrue;
    }
    return tpFalse;
}

tpBool _tpGLPathRemoveContour(tpPath _path, int _index)
{
    return tpFalse;
}

tpBool _tpGLPathAddSegmentsToCurrentContour(_tpGLPath * _p, _tpGLContour * _c, tpSegment * _segments, int _count)
{
    int err = _tpSegmentArrayAppendArray(&_c->segments, _segments, _count);
    if (err)
    {
        strcpy(_p->errorMessage, "Could not allocate memory for segments.");
        return tpTrue;
    }

    _c->lastSegmentIndex = _c->segments.count - 1;
    _c->bDirty = tpTrue;
    _p->bPathGeometryDirty = tpTrue;
    return tpFalse;
}

tpBool _tpGLPathAddSegments(tpPath _path, tpSegment * _segments, int _count)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c)
    {
        c = _tpGLPathCreateNextContour(p);
        assert(c);
    }

    return _tpGLPathAddSegmentsToCurrentContour(p, c, _segments, _count);
}

tpBool _tpGLPathAddContour(tpPath _path, tpSegment * _segments, int _count, tpBool _bClosed)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));
    _tpGLContour * c = _tpGLPathCreateNextContour(p);
    assert(c);

    tpBool ret = _tpGLPathAddSegmentsToCurrentContour(p, c, _segments, _count);
    if (ret) return ret;

    if (_bClosed)
        return tpPathClose(_path);
    return tpFalse;
}

tpBool _tpGLPathAddCircle(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _r)
{
    tpFloat dr = _r * 2.0;
    return tpPathAddEllipse(_path, _x, _y, dr, dr);
}

tpBool _tpGLPathAddEllipse(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height)
{
    //Original paper values, don't conform with SVG though :(
    // static tpVec2 s_unitSegments[12] = { tpVec2Make(-1, 0), tpVec2Make(0, s_kappa), tpVec2Make(0, -s_kappa),
    //                                      tpVec2Make(0, -1), tpVec2Make(-s_kappa, 0), tpVec2Make(s_kappa, 0),
    //                                      tpVec2Make(1, 0), tpVec2Make(0, -s_kappa), tpVec2Make(0, s_kappa),
    //                                      tpVec2Make(0, 1), tpVec2Make(s_kappa, 0), tpVec2Make(-s_kappa, 0)
    //                                    };

    static tpVec2 s_unitSegments[12] =
    {
        {1, 0}, {0, -TARP_KAPPA}, {0, TARP_KAPPA},
        {0, 1}, {TARP_KAPPA, 0}, { -TARP_KAPPA, 0},
        { -1, 0}, {0, TARP_KAPPA}, {0, -TARP_KAPPA},
        {0, -1}, { -TARP_KAPPA, 0}, {TARP_KAPPA, 0}
    };

    tpFloat rw, rh, px, py;
    tpSegment segs[4];

    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));

    _tpGLContour * c = _tpGLPathNextEmptyContour(p);

    rw = _width * 0.5;
    rh = _height * 0.5;
    for (int i = 0; i < 4; ++i)
    {
        int kappaIdx = i * 3;

        // printf("%f %f\n", s_unitSegments[kappaIdx].x, s_unitSegments[kappaIdx].y);
        // printf("%f %f\n", s_unitSegments[kappaIdx + 1].x, s_unitSegments[kappaIdx + 1].y);
        // printf("%f %f\n", s_unitSegments[kappaIdx + 2].x, s_unitSegments[kappaIdx + 2].y);

        // printf("COCK %f\n", s_unitSegments[kappaIdx + 1].x * rw + _x);
        px = s_unitSegments[kappaIdx].x * rw + _x;
        py = s_unitSegments[kappaIdx].y * rh + _y;
        segs[i] = tpSegmentMake(s_unitSegments[kappaIdx + 1].x * rw + px, s_unitSegments[kappaIdx + 1].y * rh + py,
                                px, py,
                                s_unitSegments[kappaIdx + 2].x * rw + px, s_unitSegments[kappaIdx + 2].y * rh + py);

        // printf("HANDLE IN %f %f\n", segs[i].handleIn.x, segs[i].handleIn.y);
        // printf("POS %f %f\n", segs[i].position.x, segs[i].position.y);
        // printf("HANDLE Out %f %f\n", segs[i].handleOut.x, segs[i].handleOut.y);
    }

    tpBool err = _tpGLContourAddSegments(p, c, segs, 4);
    if (err) return err;

    tpPathClose(_path);

    return tpFalse;
}

tpBool _tpGLPathAddRect(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height)
{
    tpSegment segs[4];
    tpFloat tmpa, tmpb;

    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));

    _tpGLContour * c = _tpGLPathNextEmptyContour(p);

    tmpa = _x + _width;
    tmpb = _y + _height;
    segs[0] = tpSegmentMake(_x, _y, _x, _y, _x, _y);
    segs[1] = tpSegmentMake(tmpa, _y, tmpa, _y, tmpa, _y);
    segs[2] = tpSegmentMake(tmpa, tmpb, tmpa, tmpb, tmpa, tmpb);
    segs[3] = tpSegmentMake(_x, tmpb, _x, tmpb, _x, tmpb);

    tpBool err = _tpGLContourAddSegments(p, c, segs, 4);
    if (err) return err;

    tpPathClose(_path);

    return tpFalse;
}

static void _tpGLMarkPathGeometryDirty(_tpGLPath * _p)
{
    _p->bPathGeometryDirty = tpTrue;
    for (int i = 0; i < _p->contours.count; ++i)
    {
        _tpGLContourArrayAtPtr(&_p->contours, i)->bDirty = tpTrue;
    }
}

tpBool _tpGLPathSetTransform(tpPath _path, const tpMat3 * _transform)
{
    tpVec2 scale, skew, translation;
    tpFloat rotation;
    _tpGLPath * p;

    p = (_tpGLPath *)_path.pointer;
    // assert(_tpGLIsValidPath(p));

    tpMat3Decompose(_transform, &translation, &scale, &skew, &rotation);
    //we only force a reflattening of the path if the scale changed.
    //TODO: this should take skewing into account, too AFAIK
    if (p->lastTransformScale < scale.x || p->lastTransformScale < scale.y)
    {
        p->lastTransformScale = TARP_MAX(scale.x, scale.y);
        _tpGLMarkPathGeometryDirty(p);
    }
    p->transform = *_transform;
    p->renderTransform = tpMat4MakeFrom2DTransform(_transform);
    p->lastDrawContext = NULL;

    return tpFalse;
}

tpStyle _tpGLStyleCreate(tpContext * _ctx)
{
    // _tpGLContext * ctx = (_tpGLContext *)_ctx->_impl;
    _tpGLStyle * style = malloc(sizeof(_tpGLStyle));

    style->fill.data.color = tpColorMake(1, 1, 1, 1);
    style->fill.type = kTpPaintTypeColor;
    style->stroke.data.color = tpColorMake(0, 0, 0, 1);
    style->stroke.type = kTpPaintTypeColor;
    style->strokeWidth = 1.0;
    style->strokeJoin = kTpStrokeJoinBevel;
    style->strokeCap = kTpStrokeCapButt;
    style->fillRule = kTpFillRuleEvenOdd;
    style->dashCount = 0;
    style->dashOffset = 0;
    style->miterLimit = 4;
    style->bScaleStroke = tpTrue;

    // style->context = ctx;

    // _tpGLStylePtrArrayAppend(&ctx->styles, style);

    return (tpStyle) {style};
}

void _tpGLStyleDestroy(tpStyle _style)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));
    // _tpGLStylePtrArrayRemoveValue(&s->context->styles, s);
    free(s);
}

void _tpGLStyleSetDashArray(tpStyle _style, tpFloat * _dashArray, int _count)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    assert(_count < TARP_MAX_DASH_ARRAY_SIZE);

    for (int i = 0; i < _count; ++i)
    {
        s->dashArray[i] = _dashArray[i];
    }
    s->dashCount = _count;
}

void _tpGLStyleSetDashOffset(tpStyle _style, tpFloat _offset)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->dashOffset = _offset;
}

void _tpGLStyleSetFillColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->fill.data.color = tpColorMake(_r, _g, _b, _a);
    s->fill.type = kTpPaintTypeColor;
}

void _tpGLStyleSetStrokeColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->stroke.data.color = tpColorMake(_r, _g, _b, _a);
    s->stroke.type = kTpPaintTypeColor;
}

void _tpGLStyleSetFillGradient(tpStyle _style, const tpGradient _gradient)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->fill.data.gradient = _gradient;
    s->fill.type = kTpPaintTypeGradient;
}

void _tpGLStyleSetStrokeGradient(tpStyle _style, const tpGradient _gradient)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->stroke.data.gradient = _gradient;
    s->stroke.type = kTpPaintTypeGradient;
}

void _tpGLStyleSetStrokeWidth(tpStyle _style, tpFloat _strokeWidth)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->strokeWidth = _strokeWidth;
}

void _tpGLStyleSetStrokeJoin(tpStyle _style, tpStrokeJoin _join)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->strokeJoin = _join;
}

void _tpGLStyleSetMiterLimit(tpStyle _style, tpFloat _limit)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->miterLimit = _limit;
}

void _tpGLStyleSetStrokeCap(tpStyle _style, tpStrokeCap _cap)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->strokeCap = _cap;
}

void _tpGLStyleSetScaleStroke(tpStyle _style, tpBool _b)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->bScaleStroke = _b;
}

void _tpGLStyleSetFillRule(tpStyle _style, tpFillRule _rule)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->fillRule = _rule;
}

void _tpGLStyleRemoveFill(tpStyle _style)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->fill.type = kTpPaintTypeNone;
}

void _tpGLStyleRemoveStroke(tpStyle _style)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    // assert(_tpGLIsValidStyle(s));

    s->stroke.type = kTpPaintTypeNone;
}

tpGradient _tpGLGradientCreateLinear(tpFloat _x0, tpFloat _y0, tpFloat _x1, tpFloat _y1)
{
    // _tpGLContext * ctx = (_tpGLContext *)_ctx->_impl;
    static int s_id = 0;

    _tpGLGradient * ret = malloc(sizeof(_tpGLGradient));
    ret->type = kTpGradientTypeLinear;
    ret->origin = tpVec2Make(_x0, _y0);
    ret->destination = tpVec2Make(_x1, _y1);
    _tpColorStopArrayInit(&ret->stops, 8);
    // ret->context = ctx;
    ret->bDirty = tpTrue;
    //the static id and incrementing is not multi threadding friendly...no care for now
    //thread local storage will most likely be the nicest way to make this thread safe
    ret->gradientID = s_id++;

    _TARP_ASSERT_NO_GL_ERROR(glGenTextures(1, &ret->rampTexture));
    _TARP_ASSERT_NO_GL_ERROR(glBindTexture(GL_TEXTURE_1D, ret->rampTexture));
    _TARP_ASSERT_NO_GL_ERROR(glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, TARP_GL_RAMP_TEXTURE_SIZE, 0,
                                          GL_RGBA, GL_FLOAT, NULL));
    _TARP_ASSERT_NO_GL_ERROR(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    _TARP_ASSERT_NO_GL_ERROR(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    _TARP_ASSERT_NO_GL_ERROR(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    _TARP_ASSERT_NO_GL_ERROR(glBindTexture(GL_TEXTURE_1D, 0));

    // _tpGLGradientPtrArrayAppend(&ctx->gradients, ret);

    return (tpGradient) {ret};
}

void _tpGLGradientAddColorStop(tpGradient _gradient, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a, tpFloat _offset)
{
    _tpGLGradient * g = (_tpGLGradient *)_gradient.pointer;
    // assert(_tpGLIsValidGradient(g));
    tpColorStop stop = {tpColorMake(_r, _g, _b, _a), _offset};
    _tpColorStopArrayAppendPtr(&g->stops, &stop);
}

void _tpGLGradientDestroy(tpGradient _gradient)
{
    _tpGLGradient * g = (_tpGLGradient *)_gradient.pointer;
    // assert(_tpGLIsValidGradient(g));
    _TARP_ASSERT_NO_GL_ERROR(glDeleteTextures(1, &g->rampTexture));
    _tpColorStopArrayDeallocate(&g->stops);
    // _tpGLGradientPtrArrayRemoveValue(&g->context->gradients, g);
    free(g);
}

int _tpGLIsClose(tpFloat _x, tpFloat _y, tpFloat _epsilon)
{
    tpFloat maxXYOne = TARP_MAX(1.0, TARP_MAX(fabs(_x), fabs(_y)));
    return fabs(_x - _y) <= _epsilon * maxXYOne;
}

int _tpGLIsCurveLinear(const _tpGLCurve * _curve)
{
    return _tpGLIsClose(_curve->p0.x, _curve->h0.x, FLT_EPSILON) && _tpGLIsClose(_curve->p0.y, _curve->h0.y, FLT_EPSILON) &&
           _tpGLIsClose(_curve->p1.x, _curve->h1.x, FLT_EPSILON) && _tpGLIsClose(_curve->p1.y, _curve->h1.y, FLT_EPSILON);
}

int _tpGLIsCurveFlatEnough(const _tpGLCurve * _curve, tpFloat _tolerance)
{
    if (_tpGLIsCurveLinear(_curve))
        return 1;

    // Comment from paper.js source in Curve.js:
    // Thanks to Kaspar Fischer and Roger Willcocks for the following:
    // http://hcklbrrfnn.files.wordpress.com/2012/08/bez.pdf
    tpFloat ux = _curve->h0.x * 3.0 - _curve->p0.x * 2.0 - _curve->p1.x;
    tpFloat uy = _curve->h0.y * 3.0 - _curve->p0.y * 2.0 - _curve->p1.y;
    tpFloat vx = _curve->h1.x * 3.0 - _curve->p1.x * 2.0 - _curve->p0.x;
    tpFloat vy = _curve->h1.y * 3.0 - _curve->p1.y * 2.0 - _curve->p0.y;

    return TARP_MAX(ux * ux, vx * vx) + TARP_MAX(uy * uy, vy * vy) < 10 * _tolerance * _tolerance;
}

void _tpGLSubdivideCurve(const _tpGLCurve * _curve,
                         tpFloat _t,
                         _tpGLCurvePair * _result)
{
    tpFloat v1x, v2x, v3x, v4x, v5x, v6x, v1y, v2y, v3y, v4y, v5y, v6y;
    tpFloat u = 1 - _t;

    v1x = _curve->p0.x * u + _curve->h0.x * _t;
    v1y = _curve->p0.y * u + _curve->h0.y * _t;
    v2x = _curve->h0.x * u + _curve->h1.x * _t;
    v2y = _curve->h0.y * u + _curve->h1.y * _t;
    v3x = _curve->h1.x * u + _curve->p1.x * _t;
    v3y = _curve->h1.y * u + _curve->p1.y * _t;
    v4x = v1x * u + v2x * _t;
    v4y = v1y * u + v2y * _t;
    v5x = v2x * u + v3x * _t;
    v5y = v2y * u + v3y * _t;
    v6x = v4x * u + v5x * _t;
    v6y = v4y * u + v5y * _t;

    *_result = (_tpGLCurvePair)
    {
        {_curve->p0, {v1x, v1y}, {v4x, v4y}, {v6x, v6y}},
        {{v6x, v6y}, {v5x, v5y}, {v3x, v3y}, _curve->p1}
    };
}

void _tpGLEvaluatePointForBounds(tpVec2 _vec, _tpGLRect * _bounds)
{
    _bounds->min.x = TARP_MIN(_vec.x, _bounds->min.x);
    _bounds->min.y = TARP_MIN(_vec.y, _bounds->min.y);
    _bounds->max.x = TARP_MAX(_vec.x, _bounds->max.x);
    _bounds->max.y = TARP_MAX(_vec.y, _bounds->max.y);
}

tpFloat _tpGLShortestAngle(tpVec2 _d0, tpVec2 _d1)
{
    tpFloat theta = acos(_d0.x * _d1.x + _d0.y * _d1.y);

    //make sure we have the shortest angle
    if (theta > TARP_HALF_PI)
        theta = TARP_PI - theta;

    return theta;
}

void _tpGLPushQuad(_tpVec2Array * _vertices, tpVec2 _a, tpVec2 _b, tpVec2 _c, tpVec2 _d)
{
    _tpVec2ArrayAppendPtr(_vertices, &_a);
    _tpVec2ArrayAppendPtr(_vertices, &_b);
    _tpVec2ArrayAppendPtr(_vertices, &_c);
    _tpVec2ArrayAppendPtr(_vertices, &_c);
    _tpVec2ArrayAppendPtr(_vertices, &_d);
    _tpVec2ArrayAppendPtr(_vertices, &_a);
}

void _tpGLMakeCapOrJoinRound2(tpVec2 _p, tpVec2 _d, tpFloat _theta, tpFloat _radius, _tpVec2Array * _outVertices)
{
    // printf("MAKE IT ROUND BB\n");
    tpFloat cosa, sina;
    tpVec2 dir, perp, last, current;
    int circleSubdivisionCount, i;
    tpFloat currentAngle, radStep;

    dir = tpVec2MultScalar(_d, _radius);
    perp.x = -dir.y;
    perp.y = dir.x;

    //@TODO: Make this based on the stroke width and theta?
    circleSubdivisionCount = 12;
    currentAngle = 0;
    radStep = _theta / (tpFloat)(circleSubdivisionCount);

    for (i = 0; i <= circleSubdivisionCount; ++i)
    {
        currentAngle = radStep * i;

        cosa = cos(-currentAngle);
        sina = sin(-currentAngle);

        current.x = _p.x + (perp.x * cosa - perp.y * sina);
        current.y = _p.y + (perp.x * sina - perp.y * cosa);

        if (i > 0)
        {
            _tpVec2ArrayAppendPtr(_outVertices, &last);
            _tpVec2ArrayAppendPtr(_outVertices, &current);
            _tpVec2ArrayAppendPtr(_outVertices, &_p);
        }

        last = current;
    }
}

void _tpGLMakeJoinRound(tpVec2 _p,
                        tpVec2 _e0, tpVec2 _e1,
                        tpVec2 _perp0, tpVec2 _perp1,
                        _tpVec2Array * _outVertices)
{
    tpVec2 nperp0, nperp1, last, current;
    tpFloat angle, theta, stepSize, cosa, sina;

    //@TODO: We could avoid these normalize/sqrt calls by either passing in
    //the directions (that are normalized) and getting the normalized
    //perp from there
    nperp0 = tpVec2Normalize(_perp0);
    nperp1 = tpVec2Normalize(_perp1);

    theta = acos(tpVec2Dot(nperp0, nperp1));

    // printf("ROUND JOIN\n");

    //@TODO: The stepsize should most likely take the stroke width in user space into account
    //(i.e. including the transform) to also guarantee smooth round joins for very thick/zoomed strokes.
    stepSize = TARP_PI / 16;

    last = _e0;
    for (angle = stepSize; angle < theta; angle += stepSize)
    {
        cosa = cos(angle);
        sina = sin(angle);

        current.x = _p.x + (_perp0.x * cosa - _perp0.y * sina);
        current.y = _p.y + (_perp0.x * sina + _perp0.y * cosa);

        _tpVec2ArrayAppendPtr(_outVertices, &last);
        _tpVec2ArrayAppendPtr(_outVertices, &current);
        _tpVec2ArrayAppendPtr(_outVertices, &_p);

        last = current;
    }

    _tpVec2ArrayAppendPtr(_outVertices, &last);
    _tpVec2ArrayAppendPtr(_outVertices, &_e1);
    _tpVec2ArrayAppendPtr(_outVertices, &_p);
}

void _tpGLMakeJoinBevel(tpVec2 _lePrev, tpVec2 _rePrev,
                        tpVec2 _le, tpVec2 _re,
                        tpFloat _cross, _tpVec2Array * _outVertices)
{
    if (_cross < 0)
    {
        _tpVec2ArrayAppendPtr(_outVertices, &_lePrev);
        _tpVec2ArrayAppendPtr(_outVertices, &_le);
        _tpVec2ArrayAppendPtr(_outVertices, &_rePrev);
    }
    else
    {

        _tpVec2ArrayAppendPtr(_outVertices, &_re);
        _tpVec2ArrayAppendPtr(_outVertices, &_rePrev);
        _tpVec2ArrayAppendPtr(_outVertices, &_lePrev);
    }
}

tpBool _tpGLIntersectLines(tpVec2 _p0, tpVec2 _d0,
                           tpVec2 _p1, tpVec2 _d1,
                           tpVec2 * _outResult)
{
    tpFloat cross, t;
    tpVec2 delta, doff;

    delta = tpVec2Sub(_p0, _p1);
    cross = tpVec2Cross(_d0, _d1);

    //lines are parallel
    if (cross == 0.0)
        return tpFalse;

    t = tpVec2Cross(_d1, delta) / cross;
    doff = tpVec2MultScalar(_d0, t);
    *_outResult = tpVec2Add(_p0, doff);

    return tpTrue;
}

void _tpGLMakeJoinMiter(tpVec2 _p,
                        tpVec2 _e0, tpVec2 _e1,
                        tpVec2 _dir0, tpVec2 _dir1,
                        tpFloat _cross,
                        _tpVec2Array * _outVertices)
{
    tpFloat t;
    tpVec2 intersection;

    // printf("MITER BABY!\n");
    tpVec2 inv = tpVec2MultScalar(_dir1, -1);
    _tpGLIntersectLines(_e0, _dir0, _e1, _dir1, &intersection);

    // printf("INTER %f %f\n", intersection.x, intersection.y);

    _tpGLPushQuad(_outVertices, _p, _e0, intersection, _e1);

    // _tpVec2ArrayAppendPtr(_outVertices, (tpVec2 *)_p);
    // _tpVec2ArrayAppendPtr(_outVertices, (tpVec2 *)_e0);
    // _tpVec2ArrayAppendPtr(_outVertices, &intersection);
}

void _tpGLMakeJoin(tpStrokeJoin _type,
                   tpVec2 _p,
                   tpVec2 _dir0, tpVec2 _dir1,
                   tpVec2 _perp0, tpVec2 _perp1,
                   tpVec2 _lePrev, tpVec2 _rePrev,
                   tpVec2 _le, tpVec2 _re,
                   tpFloat _cross, tpFloat _miterLimit,
                   _tpVec2Array * _outVertices)
{
    tpVec2 nperp0, nperp1;
    tpFloat miterLen, theta;

    switch (_type)
    {
        case kTpStrokeJoinRound:
            if (_cross < 0.0f)
            {
                _tpGLMakeJoinRound(_p, _lePrev, _le, _perp0, _perp1, _outVertices);
            }
            else
            {
                //negate the perpendicular vectors and reorder function arguments
                tpVec2 flippedPerp0, flippedPerp1;
                flippedPerp0 = tpVec2Make(-_perp0.x, -_perp0.y);
                flippedPerp1 = tpVec2Make(-_perp1.x, -_perp1.y);
                _tpGLMakeJoinRound(_p, _re, _rePrev, flippedPerp1, flippedPerp0, _outVertices);
            }
            break;
        case kTpStrokeJoinMiter:
            nperp0 = tpVec2Perp(_dir0);
            nperp1 = tpVec2Perp(_dir1);
            theta = acos(tpVec2Dot(nperp0, nperp1));
            miterLen = 1.0 / sin(theta / 2.0);
            if (miterLen < _miterLimit)
            {
                if (_cross < 0.0f)
                {
                    // printf("MITER A\n");
                    _tpGLMakeJoinMiter(_p, _lePrev, _le, _dir0, _dir1, _cross, _outVertices);
                }
                else
                {
                    // printf("MITER B\n");
                    _tpGLMakeJoinMiter(_p, _rePrev, _re, _dir0, _dir1, _cross, _outVertices);
                }
                break;
            }
        //fall back to bevel
        case kTpStrokeJoinBevel:
        default:
            _tpGLMakeJoinBevel(_lePrev, _rePrev, _le, _re, _cross, _outVertices);
            break;
    }
}

void _tpGLMakeCapSquare(tpVec2 _p,
                        tpVec2 _dir,
                        tpVec2 _le, tpVec2 _re,
                        _tpVec2Array * _outVertices)
{
    tpVec2 a, b;
    a = tpVec2Add(_re, _dir);
    b = tpVec2Add(_le, _dir);

    _tpGLPushQuad(_outVertices, _re, a, b, _le);
}

void _tpGLMakeCapRound(tpVec2 _p,
                       tpVec2 _perp,
                       tpVec2 _e0, tpVec2 _e1,
                       _tpVec2Array * _outVertices)
{
    tpFloat angle, stepSize, sina, cosa;
    tpVec2 last, current;

    //@TODO: The stepsize should most likely take the stroke width in user space into account
    //(i.e. including the transform) to also guarantee smooth round joins for very thick/zoomed strokes.
    stepSize = TARP_PI / 16;

    last = _e0;
    for (angle = stepSize; angle < TARP_PI; angle += stepSize)
    {
        cosa = cos(angle);
        sina = sin(angle);

        current.x = _p.x + (_perp.x * cosa - _perp.y * sina);
        current.y = _p.y + (_perp.x * sina + _perp.y * cosa);

        _tpVec2ArrayAppendPtr(_outVertices, &last);
        _tpVec2ArrayAppendPtr(_outVertices, &current);
        _tpVec2ArrayAppendPtr(_outVertices, &_p);

        last = current;
    }

    _tpVec2ArrayAppendPtr(_outVertices, &last);
    _tpVec2ArrayAppendPtr(_outVertices, &_e1);
    _tpVec2ArrayAppendPtr(_outVertices, &_p);
}

void _tpGLMakeCap(tpStrokeCap _type,
                  tpVec2 _p,
                  tpVec2 _dir,
                  tpVec2 _perp,
                  tpVec2 _le, tpVec2 _re,
                  tpBool _bStart,
                  _tpVec2Array * _outVertices)
{
    switch (_type)
    {
        case kTpStrokeCapRound:
            _tpGLMakeCapRound(_p, _perp, _bStart ? _re : _le, _bStart ? _le : _re, _outVertices);
            break;
        case kTpStrokeCapSquare:
            _tpGLMakeCapSquare(_p, _dir, _le, _re, _outVertices);
            break;
        //do nothing
        case kTpStrokeCapButt:
        default:
            break;
    }
}

void _tpGLContinousStrokeGeometry(_tpGLPath * _path, const _tpGLStyle * _style, _tpVec2Array * _vertices, _tpBoolArray * _joints)
{
    int i, j, voff;
    _tpGLContour * c;
    tpVec2 p0, p1, dir, perp, dirPrev, perpPrev;
    tpVec2 le0, le1, re0, re1, lePrev, rePrev;
    tpVec2 firstDir, firstPerp, firstLe, firstRe;
    tpFloat cross, halfSw;

    // printf("LKSJGKL %i %i\n", _vertices->count, _joints->count);
    assert(_vertices->count == _joints->count);

    halfSw = _style->strokeWidth * 0.5;

    //generate the stroke geometry for each contour
    for (i = 0; i < _path->contours.count; ++i)
    {
        // printf("BLUBB %i\n", i);

        voff = _vertices->count;
        // printf("BLUBB1 %i\n", voff);
        c = _tpGLContourArrayAtPtr(&_path->contours, i);
        c->strokeVertexOffset = voff;
        if (c->fillVertexCount <= 1)
        {
            c->strokeVertexCount = 0;
            continue;
        }

        // printf("A\n");
        for (j = c->fillVertexOffset; j < c->fillVertexOffset + c->fillVertexCount - 1; ++j)
        {
            p0 = _tpVec2ArrayAt(_vertices, j);
            p1 = _tpVec2ArrayAt(_vertices, j + 1);
            dir = tpVec2Sub(p1, p0);
            tpVec2NormalizeSelf(&dir);
            perp.x = dir.y * halfSw;
            perp.y = -dir.x * halfSw;
            cross = tpVec2Cross(perp, perpPrev);

            le0 = tpVec2Add(p0, perp);
            re0 = tpVec2Sub(p0, perp);
            le1 = tpVec2Add(p1, perp);
            re1 = tpVec2Sub(p1, perp);
            // printf("B\n");

            //check if this is the first segment / dash start
            if (j == c->fillVertexOffset)
            {
                if (!c->bIsClosed)
                {
                    //start cap?
                    firstDir = tpVec2MultScalar(dir, -1 * halfSw);
                    firstPerp.x = firstDir.y;
                    firstPerp.y = -firstDir.x;
                    _tpGLMakeCap(_style->strokeCap, p0, firstDir, firstPerp, le0, re0, tpTrue, _vertices);
                }
                else if (j == c->fillVertexOffset)
                {
                    //if the contour is closed, we cache the directions and edges
                    //to be used for the last segments stroke calculations
                    // printf("CACHING\n");
                    firstDir = dir;
                    firstPerp = perp;
                    firstLe = le0;
                    firstRe = re0;
                }
            }
            else
            {
                //check if this is a joint
                if (_tpBoolArrayAt(_joints, j))
                {
                    // printf("MAKING DA JOIIIIIIN \n");
                    _tpGLMakeJoin(_style->strokeJoin, p0,
                                  dirPrev, dir,
                                  perpPrev, perp,
                                  lePrev, rePrev, le0, re0,
                                  cross, _style->miterLimit, _vertices);
                }
                else
                {
                    //by default we join consecutive segment quads with a bevel
                    _tpGLMakeJoinBevel(lePrev, rePrev, le0, re0, cross, _vertices);
                }
            }

            //add the quad for the current segment
            _tpGLPushQuad(_vertices, le1, le0, re0, re1);

            //check if we need to do the end cap / join
            if (j == c->fillVertexOffset + c->fillVertexCount - 2 ||
                    c->fillVertexCount == 2)
            {
                // printf("PRE DEAD\n");
                if (_tpBoolArrayAt(_joints, j + 1))
                {
                    // printf("MAKING DA LAST JOIN\n");
                    //last join
                    cross = tpVec2Cross(firstPerp, perp);
                    _tpGLMakeJoin(_style->strokeJoin, p1, dir, firstDir,
                                  perp, firstPerp,
                                  le1, re1, firstLe, firstRe, cross,
                                  _style->miterLimit, _vertices);
                }
                else
                {
                    //end cap
                    // printf("DA END CAP!!!!!\n");
                    firstDir = tpVec2MultScalar(dir, halfSw);
                    _tpGLMakeCap(_style->strokeCap, p1, firstDir, perp, le1, re1, tpFalse, _vertices);
                }
            }

            perpPrev = perp;
            dirPrev = dir;
            lePrev = le1;
            rePrev = re1;
        }

        c->strokeVertexCount = _vertices->count - voff;
        _path->strokeVertexCount += c->strokeVertexCount;
        // printf("FINISHED CONTOUR STRK %i %i\n", c->strokeVertexOffset, c->strokeVertexCount);
    }
}

void _tpGLDashedStrokeGeometry(_tpGLPath * _path, const _tpGLStyle * _style, _tpVec2Array * _vertices, _tpBoolArray * _joints)
{
    int j, i, dashIndex, voff, startDashIndex;
    tpVec2 p0, p1, dir, perp, dirPrev, perpPrev;
    tpVec2 le0, le1, re0, re1, lePrev, rePrev;
    tpVec2 firstDir, firstPerp, firstLe, firstRe;
    tpVec2 current;
    tpVec2 dirr;
    tpFloat cross, halfSw, dashOffset, dashLen, segmentOff, segmentLen;
    tpFloat dashPatternOffset, patternLen, offsetIntoPattern;
    tpBool bOnDash, bDashStart, bFirstDashMightNeedJoin, bLastSegment;
    tpBool bStartDashOn, bBarelyJoined;
    tpFloat startDashLen;
    _tpGLContour * c;

    halfSw = _style->strokeWidth * 0.5;

    //no dash offset
    if (_style->dashOffset == 0)
    {
        startDashIndex = 0;
        bStartDashOn = tpTrue;
        startDashLen = _style->dashArray[startDashIndex];
    }
    else
    {
        //compute offset into dash pattern
        patternLen = 0;
        for (i = 0; i < _style->dashCount; ++i)
        {
            patternLen += _style->dashArray[i];
        }

        offsetIntoPattern = _style->dashOffset;
        if (fabsf(offsetIntoPattern) >= patternLen)
        {
            offsetIntoPattern = fmodf(offsetIntoPattern, patternLen);
        }

        startDashLen = -offsetIntoPattern;
        if (_style->dashOffset > 0)
        {
            startDashIndex = 0;
            while (startDashLen <= 0.0)
            {
                startDashLen += _style->dashArray[startDashIndex];
                startDashIndex++;
            }
            startDashIndex = TARP_MAX(0, startDashIndex - 1);
            bStartDashOn = !(startDashIndex % 2);
        }
        else
        {
            startDashIndex = _style->dashCount;
            while (startDashLen > 0.0f)
            {
                startDashIndex--;
                startDashLen -= _style->dashArray[startDashIndex];
            }
            startDashIndex = TARP_MAX(0, startDashIndex);
            bStartDashOn = !(startDashIndex % 2);
            startDashLen = _style->dashArray[startDashIndex] + startDashLen;
        }
    }

    //generate the stroke geometry for each contour
    for (i = 0; i < _path->contours.count; ++i)
    {
        voff = _vertices->count;
        c = _tpGLContourArrayAtPtr(&_path->contours, i);
        c->strokeVertexOffset = voff;

        dashIndex = startDashIndex;
        dashOffset = 0;
        j = 0;
        dashLen = startDashLen;
        bOnDash = bStartDashOn;
        bDashStart = tpTrue;
        bFirstDashMightNeedJoin = tpFalse;
        bBarelyJoined = tpFalse;

        // printf("STROK NEXST CON\n");
        // printf("=================\n");

        for (j = c->fillVertexOffset; j < c->fillVertexOffset + c->fillVertexCount - 1; ++j)
        {
            bLastSegment = j == c->fillVertexOffset + c->fillVertexCount - 2;

            // if (j == c->fillVertexOffset)
            // {
            //     printf("FIRST SEG\n");
            // }
            p0 = _tpVec2ArrayAt(_vertices, j);
            p1 = _tpVec2ArrayAt(_vertices, j + 1);
            dir = tpVec2Sub(p1, p0);
            segmentLen = tpVec2Length(dir);
            segmentOff = 0;
            dir.x /= segmentLen; dir.y /= segmentLen;

            perp.x = dir.y * halfSw;
            perp.y = -dir.x * halfSw;
            cross = tpVec2Cross(perp, perpPrev);
            // printf("NEXT SEG %f\n", tpVec2Length(dir));

            //check if this is a joint
            if (bOnDash && _tpBoolArrayAt(_joints, j))
            {
                // printf("MAKING DA JOIIIIIIN \n");
                le0 = tpVec2Add(p0, perp);
                re0 = tpVec2Sub(p0, perp);
                _tpGLMakeJoin(_style->strokeJoin, p0,
                              dirPrev, dir,
                              perpPrev, perp,
                              lePrev, rePrev, le0, re0,
                              cross, _style->miterLimit, _vertices);
            }
            // else if(bOnDash && j > c->fillVertexOffset)
            // {
            //     //by default we join consecutive segment quads with a bevel
            //     _tpGLMakeJoinBevel(&lePrev, &rePrev, &le0, &re0, cross, _vertices);
            // }

            do
            {
                tpFloat left = TARP_MIN((segmentLen - segmentOff), dashLen - dashOffset);
                // printf("LEFT %f %i %f\n", left, bOnDash, dashOffset);
                dirr = tpVec2MultScalar(dir, left);
                p1 = tpVec2Add(p0, dirr);

                le0 = tpVec2Add(p0, perp);
                re0 = tpVec2Sub(p0, perp);
                le1 = tpVec2Add(p1, perp);
                re1 = tpVec2Sub(p1, perp);

                if (bDashStart && bOnDash)
                {
                    bDashStart = tpFalse;

                    //if the current contour is not closed, or we are not at the beginning
                    //of the first segment of the contour, add a starting cap to the dash...
                    if (!c->bIsClosed || j != c->fillVertexOffset || segmentOff > 0)
                    {
                        tpVec2 tmpDir, tmpPerp;
                        tmpDir = tpVec2MultScalar(dir, -1 * halfSw);
                        tmpPerp.x = tmpDir.y;
                        tmpPerp.y = -tmpDir.x;
                        _tpGLMakeCap(_style->strokeCap, p0, tmpDir, tmpPerp, le0, re0, tpTrue, _vertices);
                    }
                    //...otherwise cache the initial values for the cap computation and mark that the contour
                    //might need a closing join in case the first and last dash of the contour touch.
                    else if (j == c->fillVertexOffset)
                    {
                        // printf("NNEED DA SPECIAL JOIN˜N\n");
                        bFirstDashMightNeedJoin = tpTrue;
                        firstDir = dir;
                        firstPerp = perp;
                        firstLe = le0;
                        firstRe = re0;
                    }
                }
                else if (!bDashStart && bOnDash)
                {
                    _tpGLMakeJoinBevel(lePrev, rePrev, le0, re0, cross, _vertices);
                }

                if (bOnDash)
                {
                    // printf("ADD QUAD\n");
                    //add the quad for the current dash on the current segment
                    _tpGLPushQuad(_vertices, le1, le0, re0, re1);
                }

                dashOffset += left;
                segmentOff += left;
                p0 = p1;

                if (dashOffset >= dashLen)
                {
                    // printf("FINISH DASH\n");

                    if (bOnDash)
                    {
                        // printf("AKSJH %i %i %i\n", bFirstDashMightNeedJoin, bLastSegment, segmentLen - segmentOff <= 0);
                        //dont make cap if the first and last dash of the contour touch and the last dash is not finished.
                        if (!bFirstDashMightNeedJoin || !bLastSegment || segmentLen - segmentOff > 0)
                        {
                            // printf("NOO WAYYY CAP\n");
                            _tpGLMakeCap(_style->strokeCap, p1, dir, perp, le1, re1, tpFalse, _vertices);
                        }
                        else
                        {
                            bBarelyJoined = tpTrue;
                        }
                    }

                    dashOffset = 0;
                    dashIndex = (dashIndex + 1) % _style->dashCount;
                    dashLen = _style->dashArray[dashIndex];
                    bDashStart = tpTrue;
                    bOnDash = !bOnDash;
                }
            }
            while ((segmentLen - segmentOff) > 0);

            if (bLastSegment)
            {
                //if the first and last dash of the contour touch, we connect them with a join
                if (bFirstDashMightNeedJoin)
                {
                    if ((bBarelyJoined || (dashOffset > 0 && bOnDash)))
                    {
                        // printf("MAKING LAST DASHED JOIN\n");
                        // printf("%f %f %f %i\n", dashOffset, segmentLen, segmentOff, bOnDash);
                        cross = tpVec2Cross(firstPerp, perp);
                        _tpGLMakeJoin(_style->strokeJoin, p1, dir, firstDir,
                                      perp, firstPerp,
                                      le1, re1, firstLe, firstRe, cross,
                                      _style->miterLimit, _vertices);
                    }
                    else
                    {
                        //otherwise we simply add a starting cap to the first dash of the contour...
                        // printf("MAKING LAST DASHED CAP\n");
                        tpVec2 tmpDir, tmpPerp;
                        tmpDir = tpVec2MultScalar(firstDir, -1 * halfSw);
                        tmpPerp.x = tmpDir.y;
                        tmpPerp.y = -tmpDir.x;
                        _tpGLMakeCap(_style->strokeCap, p1, tmpDir, tmpPerp, firstRe, firstLe, tpFalse, _vertices);

                    }
                }
                else if (dashOffset > 0 && bOnDash)
                {
                    // printf("LAST CAP\n");
                    _tpGLMakeCap(_style->strokeCap, p1, dir, perp, le1, re1, tpFalse, _vertices);
                }
            }

            dashLen -= dashOffset;
            dashOffset = 0.0;

            perpPrev = perp;
            dirPrev = dir;
            lePrev = le1;
            rePrev = re1;
        }

        c->strokeVertexCount = _vertices->count - voff;
        _path->strokeVertexCount += c->strokeVertexCount;
        // printf("CS %i %i\n", c->strokeVertexOffset, c->strokeVertexCount);
    }
}

void _tpGLStroke(_tpGLPath * _path, const _tpGLStyle * _style, _tpVec2Array * _vertices, _tpBoolArray * _joints)
{
    _path->strokeVertexCount = 0;

    if (_style->dashCount)
    {
        _tpGLDashedStrokeGeometry(_path, _style, _vertices, _joints);
    }
    else
    {
        _tpGLContinousStrokeGeometry(_path, _style, _vertices, _joints);
    }

    //
    _path->strokeVertexOffset = _tpGLContourArrayAtPtr(&_path->contours, 0)->strokeVertexOffset;
    // printf("STROKE VERTEX OFF %i\n", _tpGLContourArrayAtPtr(&_path->contours, 0)->strokeVertexOffset);

    // cache with what settings the stroke geometry was generated
    _path->lastStroke.strokeType = _style->stroke.type;
    _path->lastStroke.strokeWidth = _style->strokeWidth;
    _path->lastStroke.dashOffset = _style->dashOffset;
    memcpy(_path->lastStroke.dashArray, _style->dashArray, sizeof(tpFloat) * _style->dashCount);
    _path->lastStroke.dashCount = _style->dashCount;
    _path->lastStroke.join = _style->strokeJoin;
    _path->lastStroke.cap = _style->strokeCap;
    _path->lastStroke.bScaleStroke = _style->bScaleStroke;
}

void _tpGLFlattenCurve(_tpGLPath * _path,
                       const _tpGLCurve * _curve,
                       tpFloat _angleTolerance,
                       int _bIsClosed,
                       int _bFirstCurve,
                       int _bLastCurve,
                       _tpVec2Array * _outVertices,
                       _tpBoolArray * _outJoints,
                       _tpGLRect * _bounds,
                       int * _vertexCount)
{
    _tpGLCurve stack[TARP_MAX_CURVE_SUBDIVISIONS];
    _tpGLCurvePair cp;
    _tpGLCurve * current;
    int stackIndex = 0;
    stack[0] = *_curve;

    while (stackIndex >= 0)
    {
        current = &stack[stackIndex];

        if (stackIndex < TARP_MAX_CURVE_SUBDIVISIONS - 1 && !_tpGLIsCurveFlatEnough(current, _angleTolerance))
        {
            //subdivide curve and add continue with the left of the two curves by putting it on top
            //of the stack.
            _tpGLSubdivideCurve(current, 0.5, &cp);
            *current = cp.second;
            stack[++stackIndex] = cp.first;
        }
        else
        {
            // printf("ADD TO CACHE\n");
            //for the first curve we also add its first segment
            if (_bFirstCurve)
            {
                _tpVec2ArrayAppendPtr(_outVertices, &current->p0);
                _tpVec2ArrayAppendPtr(_outVertices, &current->p1);

                _tpBoolArrayAppend(_outJoints, tpFalse);
                _tpBoolArrayAppend(_outJoints, tpVec2Equals(current->p1, _curve->p1) && !_bLastCurve);
                _tpGLEvaluatePointForBounds(current->p0, _bounds);
                _tpGLEvaluatePointForBounds(current->p1, _bounds);
                *_vertexCount += 2;

                //we don't want to do this for the following subdivisions
                _bFirstCurve = 0;
            }
            else
            {
                _tpVec2ArrayAppendPtr(_outVertices, &current->p1);

                _tpBoolArrayAppend(_outJoints, _bIsClosed ? tpVec2Equals(current->p1, _curve->p1) :
                                   (tpVec2Equals(current->p1, _curve->p1) && !_bLastCurve));
                _tpGLEvaluatePointForBounds(current->p1, _bounds);
                (*_vertexCount)++;
            }
            stackIndex--;
        }
    }
}

void _tpGLInitBounds(_tpGLRect * _bounds)
{
    _bounds->min.x = FLT_MAX;
    _bounds->min.y = FLT_MAX;
    _bounds->max.x = -FLT_MAX;
    _bounds->max.y = -FLT_MAX;
}

void _tpGLMergeBounds(_tpGLRect * _a, const _tpGLRect * _b)
{
    _tpGLEvaluatePointForBounds(_b->min, _a);
    _tpGLEvaluatePointForBounds(_b->max, _a);
}

int _tpGLFlattenPath(_tpGLPath * _path,
                     tpFloat _angleTolerance,
                     tpBool _bTransformCurves,
                     _tpVec2Array * _outVertices,
                     _tpBoolArray * _outJoints,
                     _tpGLRect * _outBounds)
{
    printf("====================================\n");
    _tpGLRect contourBounds;
    int i = 0;
    int j = 0;
    _tpGLContour * c = NULL;
    tpSegment * last = NULL, *current = NULL;
    int recursionDepth = 0;

    _tpGLInitBounds(_outBounds);

    printf("CONTOURS %i\n", _path->contours.count);

    int off = 0;
    for (i = 0; i < _path->contours.count; ++i)
    {
        c = _tpGLContourArrayAtPtr(&_path->contours, i);
        _tpGLInitBounds(&contourBounds);

        if (c->bDirty)
        {
            //if the contour is dirty, flatten it
            c->bDirty = tpFalse;
            last = _tpSegmentArrayAtPtr(&c->segments, 0);
            printf("DIRTY SEGMENTS %i\n", c->segments.count);

            int vcount = 0;
            for (j = 1; j < c->segments.count; ++j)
            {
                current = _tpSegmentArrayAtPtr(&c->segments, j);

                printf("%f %f\n", last->position.x, last->position.y);
                printf("%f %f\n", current->position.x, current->position.y);
                _tpGLCurve curve = {last->position.x, last->position.y,
                                    last->handleOut.x, last->handleOut.y,
                                    current->handleIn.x, current->handleIn.y,
                                    current->position.x, current->position.y
                                   };

                if (_bTransformCurves)
                {
                    printf("TRANSFORM CURVES %f\n", _path->lastTransformScale);
                    printf("TRANS %f %f %f %f\n", _path->transform.v[0], _path->transform.v[1], _path->transform.v[3], _path->transform.v[4]);
                    curve.p0 = tpMat3MultVec2(&_path->transform, curve.p0);
                    curve.h0 = tpMat3MultVec2(&_path->transform, curve.h0);
                    curve.h1 = tpMat3MultVec2(&_path->transform, curve.h1);
                    curve.p1 = tpMat3MultVec2(&_path->transform, curve.p1);
                }

                _tpGLFlattenCurve(_path,
                                  &curve,
                                  _angleTolerance,
                                  c->bIsClosed,
                                  j == 1,
                                  tpFalse,
                                  _outVertices,
                                  _outJoints,
                                  &contourBounds, &vcount);

                last = current;
            }

            // if the contour is closed, flatten the last closing curve
            if (c->bIsClosed && c->segments.count)
            {
                tpSegment * fs = _tpSegmentArrayAtPtr(&c->segments, 0);
                _tpGLCurve curve = {last->position.x, last->position.y,
                                    last->handleOut.x, last->handleOut.y,
                                    fs->handleIn.x, fs->handleIn.y,
                                    fs->position.x, fs->position.y
                                   };

                if (_bTransformCurves)
                {
                    printf("AAA %f %f\n", curve.p0.x, curve.p0.y);
                    curve.p0 = tpMat3MultVec2(&_path->transform, curve.p0);
                    printf("BBB %f %f\n", curve.p0.x, curve.p0.y);
                    curve.h0 = tpMat3MultVec2(&_path->transform, curve.h0);
                    curve.h1 = tpMat3MultVec2(&_path->transform, curve.h1);
                    curve.p1 = tpMat3MultVec2(&_path->transform, curve.p1);
                }

                _tpGLFlattenCurve(_path,
                                  &curve,
                                  _angleTolerance,
                                  c->bIsClosed,
                                  tpFalse,
                                  tpTrue,
                                  _outVertices,
                                  _outJoints,
                                  &contourBounds, &vcount);
            }

            c->fillVertexOffset = off;
            c->fillVertexCount = vcount;
            c->bounds = contourBounds;
            off += vcount;

            _tpGLMergeBounds(_outBounds, &contourBounds);
        }
        else
        {
            printf("APPEND ARRAY\n");
            c->fillVertexOffset = off;
            //otherwise we just copy the contour to the tmpbuffer...
            _tpVec2ArrayAppendArray(_outVertices, _tpVec2ArrayAtPtr(&_path->geometryCache, c->fillVertexOffset), c->fillVertexCount);
            _tpBoolArrayAppendArray(_outJoints, _tpBoolArrayAtPtr(&_path->jointCache, c->fillVertexOffset), c->fillVertexCount);

            //...and merge the cached contour bounds with the path bounds
            _tpGLMergeBounds(_outBounds, &c->bounds);

            // for(int d = c->fillVertexOffset; d < c->fillVertexOffset + c->fillVertexCount; ++d)
            // {
            //     _tpVec2ArrayAppendPtr(_outVertices, _tpVec2ArrayAtPtr(&_path->geometryCache, d));
            //     _tpBoolArrayAppend(_outJoints, _tpBoolArrayAt(&_path->jointCache, d));
            // }

            off += c->fillVertexCount;
        }
    }

    printf("EEEEND\n");
    return 0;
}

int _tpGLColorStopComp(const void * _a, const void * _b)
{
    if (((tpColorStop *)_a)->offset < ((tpColorStop *)_b)->offset)
        return -1;
    return 1;
}

void _tpGLFinalizeColorStops(_tpGLContext * _ctx, _tpGLGradient * _grad)
{
    int i, j;
    tpColorStop * current;
    tpBool bAdd, bHasStartStop, bHasEndStop;
    _tpColorStopArrayClear(&_ctx->tmpColorStops);

    bHasStartStop = tpFalse;
    bHasEndStop = tpFalse;

    printf("FINALIZE COLOR STOPS %i %i\n", _grad->stops.count, _ctx->tmpColorStops.count);

    //remove duplicates
    for (i = 0; i < _grad->stops.count; ++i)
    {
        bAdd = tpTrue;
        current = _tpColorStopArrayAtPtr(&_grad->stops, i);
        if (current->offset == 0)
            bHasStartStop = tpTrue;
        else if (current->offset == 1)
            bHasEndStop = tpTrue;

        for (j = 0; j < _ctx->tmpColorStops.count; ++j)
        {
            if (current->offset == _tpColorStopArrayAtPtr(&_ctx->tmpColorStops, j)->offset)
            {
                bAdd = tpFalse;
                break;
            }
        }

        if (bAdd && current->offset >= 0 && current->offset <= 1)
            _tpColorStopArrayAppendPtr(&_ctx->tmpColorStops, current);
    }

    // sort from 0 - 1 by offset
    qsort(_ctx->tmpColorStops.array, _ctx->tmpColorStops.count, sizeof(tpColorStop), _tpGLColorStopComp);

    printf("WOOT %i\n", _ctx->tmpColorStops.count);

    // make sure there is a stop at 0 and 1 offset
    if (!bHasStartStop || !bHasEndStop)
    {
        tpColorStop tmp;

        _tpColorStopArrayClear(&_grad->stops);
        if (!bHasStartStop)
        {
            tmp.color = _tpColorStopArrayAtPtr(&_ctx->tmpColorStops, 0)->color;
            tmp.offset = 0;
            _tpColorStopArrayAppendPtr(&_grad->stops, &tmp);
        }

        _tpColorStopArrayAppendArray(&_grad->stops, _ctx->tmpColorStops.array, _ctx->tmpColorStops.count);

        if (!bHasEndStop)
        {
            tmp.color = _tpColorStopArrayLastPtr(&_ctx->tmpColorStops)->color;
            tmp.offset = 1;
            _tpColorStopArrayAppendPtr(&_grad->stops, &tmp);
        }
    }
    else
    {
        printf("SIMPLE STOP SWAP\n");
        //if they are already there, we can simply swap
        _tpColorStopArraySwap(&_grad->stops, &_ctx->tmpColorStops);
    }
}

//this function assumes that the gradients ramp texture is bound
void _tpGLUpdateRampTexture(_tpGLGradient * _grad)
{
    printf("UPDATE RAMP TEXTURE %i\n", _grad->stops.count);
    tpColor pixels[TARP_GL_RAMP_TEXTURE_SIZE] = {1.0};
    int xStart, xEnd, diff, i, j;
    tpFloat mixFact;
    tpColor mixColor;
    tpColorStop * stop1, * stop2;

    //generate the ramp texture
    xStart = 0;
    xEnd = 0;

    stop1 = _tpColorStopArrayAtPtr(&_grad->stops, 0);
    pixels[0] = stop1->color;

    for (i = 1; i < _grad->stops.count; ++i)
    {
        stop2 = _tpColorStopArrayAtPtr(&_grad->stops, i);
        xEnd = (int)(stop2->offset * (TARP_GL_RAMP_TEXTURE_SIZE - 1));

        assert(xStart >= 0 && xStart < TARP_GL_RAMP_TEXTURE_SIZE &&
               xEnd >= 0 && xEnd < TARP_GL_RAMP_TEXTURE_SIZE &&
               xStart <= xEnd);

        diff = xEnd - xStart;
        mixColor.r = stop2->color.r - stop1->color.r;
        mixColor.g = stop2->color.g - stop1->color.g;
        mixColor.b = stop2->color.b - stop1->color.b;
        mixColor.a = stop2->color.a - stop1->color.a;

        for (j = xStart + 1; j <= xEnd; ++j)
        {
            mixFact = (tpFloat)(j - xStart) / (tpFloat)(diff);
            pixels[j].r = stop1->color.r + mixColor.r * mixFact;
            pixels[j].g = stop1->color.g + mixColor.g * mixFact;
            pixels[j].b = stop1->color.b + mixColor.b * mixFact;
            pixels[j].a = stop1->color.a + mixColor.a * mixFact;

            // printf("%f %f %f %f\n", pixels[j].r, pixels[j].g, pixels[j].b, pixels[j].a);
        }
        stop1 = stop2;
        xStart = xEnd;
    }

    _TARP_ASSERT_NO_GL_ERROR(glActiveTexture(GL_TEXTURE0));
    _TARP_ASSERT_NO_GL_ERROR(glBindTexture(GL_TEXTURE_1D, _grad->rampTexture));
    _TARP_ASSERT_NO_GL_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    _TARP_ASSERT_NO_GL_ERROR(glTexSubImage1D(GL_TEXTURE_1D, 0, 0, TARP_GL_RAMP_TEXTURE_SIZE,
                             GL_RGBA, GL_FLOAT, &pixels[0].r));
}

void _tpGLUpdateVAO(_tpGLVAO * _vao, void * _data, int _byteCount)
{
    printf("UPDATE VAOOOOO\n");
    //not sure if this buffer orphaning style data upload makes a difference these days anymore. (TEST??)
    if (_byteCount > _vao->vboSize)
    {
        printf("NEW BUFFER DATA\n");
        _TARP_ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, _byteCount, _data, GL_DYNAMIC_DRAW));
        _vao->vboSize = _byteCount;
    }
    else
    {
        printf("ORPH BUFFER\n");
        _TARP_ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, _vao->vboSize, NULL, GL_DYNAMIC_DRAW));
        _TARP_ASSERT_NO_GL_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, _byteCount, _data));
    }
}

void _tpGLDrawPaint(_tpGLContext * _ctx, _tpGLPath * _path,
                    const _tpGLPaint * _paint, const _tpGLGradientCacheData * _gradCache)
{
    if (_paint->type == kTpPaintTypeColor)
    {
        printf("DRAWING COLOR PAINT\n");
        //@TODO: Cache the uniform loc
        _TARP_ASSERT_NO_GL_ERROR(glUniform4fv(glGetUniformLocation(_ctx->program, "meshColor"), 1, &_paint->data.color.r));
        _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, _path->boundsVertexOffset, 4));
    }
    else if (_paint->type == kTpPaintTypeGradient)
    {
        printf("GOT DA GRADIENT BABY\n");
        _tpGLGradient * grad = (_tpGLGradient *)_paint->data.gradient.pointer;

        //bind the gradients texture
        _TARP_ASSERT_NO_GL_ERROR(glActiveTexture(GL_TEXTURE0));
        _TARP_ASSERT_NO_GL_ERROR(glBindTexture(GL_TEXTURE_1D, grad->rampTexture));

        if (grad->type == kTpGradientTypeLinear)
        {
            printf("DRAWING LINEAR GRADIENT \n");
            _TARP_ASSERT_NO_GL_ERROR(glUseProgram(_ctx->textureProgram));
            //@TODO: Cache uniform loc
            _TARP_ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->textureProgram, "transformProjection"), 1, GL_FALSE, &_path->transformProjection.v[0]));
            // _TARP_ASSERT_NO_GL_ERROR(glUniform4fv(glGetUniformLocation(_ctx->textureProgram, "meshColor"), 1, &_paint->data.color.r));
            _TARP_ASSERT_NO_GL_ERROR(glBindVertexArray(_ctx->textureVao.vao));
            // _TARP_ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->textureVao.vbo));

            // _tpGLUpdateVAO(&_ctx->textureVao, _path->textureGeometryCache.array, sizeof(_tpGLTextureVertex) * _path->textureGeometryCache.count);

            _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, _gradCache->vertexOffset, _gradCache->vertexCount));

            _TARP_ASSERT_NO_GL_ERROR(glUseProgram(_ctx->program));
            _TARP_ASSERT_NO_GL_ERROR(glBindVertexArray(_ctx->vao.vao));
            // _TARP_ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->vao.vbo));
        }
        else
        {

        }
    }
    else
    {
        printf("WADDAFUUUUUQ\n");
    }
}

void _tpGLDrawFillEvenOdd(_tpGLContext * _ctx, _tpGLPath * _path, const _tpGLStyle * _style)
{
    _TARP_ASSERT_NO_GL_ERROR(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
    _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
    _TARP_ASSERT_NO_GL_ERROR(glStencilFunc(GL_ALWAYS, 0, _kTpGLClippingStencilPlaneOne));
    _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLFillRasterStencilPlane));
    _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT));

    //_transform ? & (*_transform).v[0] : NULL
    //@TODO: Cache the uniform loc
    // _TARP_ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->program, "transformProjection"), 1, GL_FALSE, &mat.v[0]));
    // printf("COUNT %i\n", _path->geometryCache.count);
    // _TARP_ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(tpVec2) * _path->geometryCache.count, _path->geometryCache.array, GL_DYNAMIC_DRAW));
    for (int i = 0; i < _path->contours.count; ++i)
    {
        _tpGLContour * c = _tpGLContourArrayAtPtr(&_path->contours, i);
        // printf("VC %i %i\n", c->fillVertexOffset, c->fillVertexCount);
        _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_FAN, c->fillVertexOffset, c->fillVertexCount));
    }

    _TARP_ASSERT_NO_GL_ERROR(glStencilFunc(GL_EQUAL, 255, _kTpGLFillRasterStencilPlane));
    _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLFillRasterStencilPlane));
    _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO));
    _TARP_ASSERT_NO_GL_ERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

    // printf("DRAW DA FILL\n");
    _tpGLDrawPaint(_ctx, _path, &_style->fill, &_path->fillGradientData);
}

void _tpGLCacheBoundsGeometry(_tpGLPath * _path, const _tpGLStyle * _style)
{
    _tpGLRect bounds;
    _tpGLRect * bptr;

    _path->boundsVertexOffset = _path->geometryCache.count;

    //add the bounds geometry to the end of the geometry cache.
    //If there is a stroke, upload the stroke bounds instead
    if (_style->stroke.type != kTpPaintTypeNone)
    {
        tpFloat adder;
        bounds = _path->boundsCache;
        adder = TARP_MAX(_style->strokeWidth, _style->miterLimit);
        bounds.min.x -= adder;
        bounds.min.y -= adder;
        bounds.max.x += adder;
        bounds.max.y += adder;

        _path->strokeBoundsCache = bounds;
        bptr = &bounds;
    }
    else
    {
        bptr = &_path->boundsCache;
    }
    tpVec2 boundsData[] = {bptr->min,
        {bptr->min.x, bptr->max.y},
        {bptr->max.x, bptr->min.y},
        bptr->max
    };
    _tpVec2ArrayAppendArray(&_path->geometryCache, boundsData, 4);
}

typedef struct
{
    tpVec2 vertex;
    tpFloat tc;
} TexVertex;

void _tpGLGradientLinearGeometry(_tpGLContext * _ctx,
                                 _tpGLGradient * _grad,
                                 const _tpGLRect * _bounds,
                                 _tpGLTextureVertexArray * _vertices,
                                 int * _outVertexOffset,
                                 int * _outVertexCount)
{
    // regenerate the geometry for this path/gradient combo
    _tpGLTextureVertex vertices[8];
    tpVec2 dir, ndir, perp, nperp, center;
    tpVec2 corners[4];
    tpVec2 tmp, tmp2, tmp3;
    tpFloat len, o, s, left, right, minOffset, maxOffset;
    int i;

    dir = tpVec2Sub(_grad->destination, _grad->origin);
    len = tpVec2Length(dir);
    ndir.x = dir.x / len;
    ndir.y = dir.y / len;
    perp.x = -dir.y;
    perp.y = dir.x;
    nperp.x = -ndir.y;
    nperp.y = ndir.x;

    center = _grad->origin;

    printf("GRAD ORIGIN %f %f DES %f %f\n", center.x, center.y, _grad->destination.x, _grad->destination.y);

    corners[0] = tpVec2Sub(_bounds->min, center);
    corners[1].x = _bounds->max.x - center.x;
    corners[1].y = _bounds->min.y - center.y;
    corners[2].x = _bounds->min.x - center.x;
    corners[2].y = _bounds->max.y - center.y;
    corners[3] = tpVec2Sub(_bounds->max, center);

    for (i = 0; i < 4; ++i)
    {
        o = tpVec2Dot(corners[i], ndir) / len;
        s = tpVec2Dot(corners[i], nperp);

        if (o < minOffset || i == 0) minOffset = o;
        if (o > maxOffset || i == 0) maxOffset = o;

        if (i == 0 || s < left) left = s;
        if (i == 0 || s > right) right = s;
    }

    // minOffset = TARP_MAX(0.0, minOffset);
    // maxOffset = TARP_MIN(1.0, maxOffset);

    tmp = tpVec2MultScalar(ndir, minOffset * len);
    tmp2 = tpVec2Add(center, tpVec2MultScalar(nperp, left));
    tmp3 = tpVec2Add(center, tpVec2MultScalar(nperp, right));
    vertices[0].vertex = tpVec2Add(tmp2, tmp);
    vertices[0].tc.x = minOffset;
    vertices[1].vertex = tpVec2Add(tmp3, tmp);
    vertices[1].tc.x = minOffset;
    tmp = tpVec2MultScalar(ndir, maxOffset * len);
    vertices[2].vertex = tpVec2Add(tmp2, tmp);
    vertices[2].tc.x = maxOffset;
    vertices[3].vertex = tpVec2Add(tmp3, tmp);
    vertices[3].tc.x = maxOffset;

    /*for (int i = 0; i < 4; ++i)
    {
        printf("DA LIN GRAD VERT %f %f %f\n", vertices[i].vertex.x, vertices[i].vertex.y, vertices[i].tc.x);
    }*/

    *_outVertexOffset = _vertices->count;
    *_outVertexCount = 4;
    _tpGLTextureVertexArrayAppendArray(_vertices, vertices, 4);
}

void _tpGLCacheGradientGeometry(_tpGLContext * _ctx, _tpGLGradient * _grad,
                                _tpGLPath * _path, _tpGLGradientCacheData * _gradCache, _tpGLTextureVertexArray * _vertices)
{
    _tpGLGradient * grad = _grad;

    // check if we need to update the gradient texture
    if (grad->bDirty)
    {
        grad->bDirty = tpFalse;
        // ensure that the color stops are valid/complete
        _tpGLFinalizeColorStops(_ctx, grad);
        // update the ramp texture
        _tpGLUpdateRampTexture(grad);
        _gradCache->lastGradientID = -1;
    }

    if (_gradCache->lastGradientID != grad->gradientID)
    {
        printf("REBUILDING GRAD\n");
        //rebuild the gradient
        if (grad->type == kTpGradientTypeLinear)
        {
            _tpGLGradientLinearGeometry(_ctx, grad,
                                        _gradCache->bounds, _vertices,
                                        &_gradCache->vertexOffset, &_gradCache->vertexCount);

        }
        else if (grad->type == kTpGradientTypeRadial)
        {
            //TODO :/
        }
        _gradCache->lastGradientID = grad->gradientID;
    }
    else
    {
        printf("COPY CACHED GRAD\n");
        //copy cached gradient data
        _tpGLTextureVertexArrayAppendArray(_vertices,
                                           _tpGLTextureVertexArrayAtPtr(&_path->textureGeometryCache, _gradCache->vertexOffset),
                                           _gradCache->vertexCount);
    }
}

tpBool _tpGLPrepareDrawing(tpContext * _ctx)
{
    _tpGLContext * ctx = (_tpGLContext *)_ctx->_impl;

    //TODO: Cache previous opengl state of everything we change so we can reset it in
    // tpFinishDrawing
    _TARP_ASSERT_NO_GL_ERROR(glActiveTexture(GL_TEXTURE0));

    _TARP_ASSERT_NO_GL_ERROR(glDisable(GL_DEPTH_TEST));
    _TARP_ASSERT_NO_GL_ERROR(glDepthMask(GL_FALSE));
    _TARP_ASSERT_NO_GL_ERROR(glEnable(GL_MULTISAMPLE));
    // _TARP_ASSERT_NO_GL_ERROR(glEnable(GL_BLEND));
    // _TARP_ASSERT_NO_GL_ERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    //TODO: find a way to clear all stencil planes to the same value so we can only clear once?
    _TARP_ASSERT_NO_GL_ERROR(glEnable(GL_STENCIL_TEST));
    _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLFillRasterStencilPlane));
    _TARP_ASSERT_NO_GL_ERROR(glClearStencil(0));
    _TARP_ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));
    _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLClippingStencilPlaneOne | _kTpGLClippingStencilPlaneTwo | _kTpGLStrokeRasterStencilPlane));
    _TARP_ASSERT_NO_GL_ERROR(glClearStencil(255));
    _TARP_ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));

    _TARP_ASSERT_NO_GL_ERROR(glBindVertexArray(ctx->vao.vao));
    _TARP_ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, ctx->vao.vbo));

    _TARP_ASSERT_NO_GL_ERROR(glUseProgram(ctx->program));

    ctx->clippingStackDepth = 0; //reset clipping

    return tpFalse;
}

tpBool _tpGLFinishDrawing(tpContext * _ctx)
{
    //@TODO!!!111 reset drawing state to what will be cached in _tpGLPrepareDrawing
}

void _tpGLPrepareStencilPlanes(_tpGLContext * _ctx, tpBool _bIsClippingPath, int * _outTargetStencilPlane, int * _outTestStencilPlane)
{
    *_outTargetStencilPlane = _bIsClippingPath ? _ctx->currentClipStencilPlane : _kTpGLFillRasterStencilPlane;
    *_outTestStencilPlane = _ctx->currentClipStencilPlane == _kTpGLClippingStencilPlaneOne ? _kTpGLClippingStencilPlaneTwo : _kTpGLClippingStencilPlaneOne;
}

tpBool _tpGLDrawPathImpl(_tpGLContext * _ctx, _tpGLPath * _path, tpStyle _style, tpBool _bIsClipPath)
{
    GLint i;
    GLuint stencilPlaneToWriteTo, stencilPlaneToTestAgainst;
    _tpGLContour * c;

    assert(_ctx);
    _tpGLPath * p = _path;
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(p && s);
    // assert(_tpGLIsValidPath(p));
    // assert(_tpGLIsValidStyle(s));

    //if this style has a stroke and its scale stroke property is different from the last style,
    //we force a full reflattening of all path contours.
    //we also do this if the style has non scaling stroke and the transform changed since we
    //last drew the path.
    if ((s->stroke.type != kTpPaintTypeNone && p->lastStroke.bScaleStroke != s->bScaleStroke) ||
            (!s->bScaleStroke && p->bTransformProjDirty))
    {
        _tpGLMarkPathGeometryDirty(p);
    }

    //check if the transform projection is dirty
    if (p->lastDrawContext != _ctx || p->lastProjectionID != _ctx->projectionID)
    {
        printf("NEW PROJECTION\n");
        p->lastProjectionID = _ctx->projectionID;
        p->lastDrawContext = _ctx;
        p->transformProjection = tpMat4Mult(&_ctx->projection, &p->renderTransform);
    }

    // check if the path geometry is dirty.
    // if so, rebuild everything!
    if (p->bPathGeometryDirty)
    {
        printf("RE BUILD STUFF\n");
        p->bPathGeometryDirty = tpFalse;

        //flatten the path into tmp buffers
        _tpGLRect bounds;
        if (s->bScaleStroke)
            _tpGLFlattenPath(p, 0.15 / p->lastTransformScale, tpFalse, &_ctx->tmpVertices, &_ctx->tmpJoints, &bounds);
        else
            _tpGLFlattenPath(p, 0.15, tpTrue, &_ctx->tmpVertices, &_ctx->tmpJoints, &bounds);

        //generate and add the stroke geometry to the tmp buffers
        if (s->stroke.type != kTpPaintTypeNone && s->strokeWidth > 0)
            _tpGLStroke(p, s, &_ctx->tmpVertices, &_ctx->tmpJoints);

        //swap the tmp buffers with the path caches
        _tpVec2ArrayClear(&p->geometryCache);
        _tpBoolArrayClear(&p->jointCache);
        _tpVec2ArraySwap(&p->geometryCache, &_ctx->tmpVertices);
        _tpBoolArraySwap(&p->jointCache, &_ctx->tmpJoints);

        //save the path bounds
        p->boundsCache = bounds;

        //add the bounds geometry to the geom cache (and potentially cache stroke bounds)
        _tpGLCacheBoundsGeometry(p, s);

        //force recalculation of gradient related geometries
        p->fillGradientData.lastGradientID = -1;
        p->strokeGradientData.lastGradientID = -1;
    }
    //check if the stroke should be removed
    else if (((s->stroke.type == kTpPaintTypeNone &&
               p->lastStroke.strokeType != kTpPaintTypeNone) ||
              (s->strokeWidth == 0 && p->lastStroke.strokeWidth > 0)))
    {
        printf("REMOVE STROKE\n");

        p->lastStroke.strokeType = s->stroke.type;
        p->lastStroke.strokeWidth = 0;
        p->strokeVertexOffset = 0;
        p->strokeVertexCount = 0;
    }
    //check if the stroke needs to be regenerated (due to a change in stroke width or dash related settings)
    else if (!_bIsClipPath && ((s->stroke.type != kTpPaintTypeNone && s->strokeWidth > 0 &&
                                (p->lastStroke.strokeWidth != s->strokeWidth ||
                                 p->lastStroke.cap != s->strokeCap ||
                                 p->lastStroke.join != s->strokeJoin ||
                                 p->lastStroke.dashCount != s->dashCount ||
                                 p->lastStroke.dashOffset != s->dashOffset ||
                                 memcmp(p->lastStroke.dashArray, s->dashArray, sizeof(tpFloat) * s->dashCount) != 0))))
    {
        printf("UPDATE STROKE\n");
        //remove all the old stoke vertices from the cache
        _tpVec2ArrayRemoveRange(&p->geometryCache, p->strokeVertexOffset, p->geometryCache.count);

        //generate and add the stroke geometry to the cache.
        _tpGLStroke(p, s, &p->geometryCache, &p->jointCache);

        //add the bounds geometry to the geom cache.
        _tpGLCacheBoundsGeometry(p, s);

        //force rebuilding of the stroke gradient geometry
        p->strokeGradientData.lastGradientID = -1;
    }

    // check if there are any gradients to be cached.
    if (!_bIsClipPath && ((s->fill.type == kTpPaintTypeGradient &&
                           p->fillGradientData.lastGradientID != ((_tpGLGradient *)s->fill.data.gradient.pointer)->gradientID) ||
                          (s->stroke.type == kTpPaintTypeGradient &&
                           p->strokeGradientData.lastGradientID != ((_tpGLGradient *)s->stroke.data.gradient.pointer)->gradientID)))
    {
        printf("GRADS GETTING UPDATED\n");
        _tpGLTextureVertexArrayClear(&_ctx->tmpTexVertices);
        if (s->fill.type == kTpPaintTypeGradient)
        {
            printf("UPDATE FILL GRADIENT\n");
            _tpGLGradient * grad = (_tpGLGradient *)s->fill.data.gradient.pointer;
            _tpGLCacheGradientGeometry(_ctx, grad, p, &p->fillGradientData, &_ctx->tmpTexVertices);
        }

        if (s->stroke.type == kTpPaintTypeGradient)
        {
            printf("UPDATE STROKE GRADIENT\n");
            _tpGLGradient * grad = (_tpGLGradient *)s->stroke.data.gradient.pointer;
            _tpGLCacheGradientGeometry(_ctx, grad, p, &p->strokeGradientData, &_ctx->tmpTexVertices);
        }

        _tpGLTextureVertexArraySwap(&p->textureGeometryCache, &_ctx->tmpTexVertices);
    }

    if (!_bIsClipPath && (s->fill.type == kTpPaintTypeGradient || s->stroke.type == kTpPaintTypeGradient))
    {
        _TARP_ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->textureVao.vbo));
        _tpGLUpdateVAO(&_ctx->textureVao, p->textureGeometryCache.array, sizeof(_tpGLTextureVertex) * p->textureGeometryCache.count);
        _TARP_ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->vao.vbo));
    }

    //upload the paths geometry cache to the gpu
    _tpGLUpdateVAO(&_ctx->vao, p->geometryCache.array, sizeof(tpVec2) * p->geometryCache.count);

    //@TODO: Cache Uniforms loc
    if (s->bScaleStroke)
        _TARP_ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->program, "transformProjection"), 1, GL_FALSE, &p->transformProjection.v[0]));
    else
    {
        _TARP_ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->program, "transformProjection"), 1, GL_FALSE, &_ctx->projection.v[0]));
    }

    //draw the fill
    stencilPlaneToWriteTo = _bIsClipPath ? _ctx->currentClipStencilPlane : _kTpGLFillRasterStencilPlane;
    stencilPlaneToTestAgainst = _ctx->currentClipStencilPlane == _kTpGLClippingStencilPlaneOne ? _kTpGLClippingStencilPlaneTwo : _kTpGLClippingStencilPlaneOne;

    if (_bIsClipPath || s->fill.type != kTpPaintTypeNone)
    {
        _TARP_ASSERT_NO_GL_ERROR(glStencilFunc(_ctx->clippingStackDepth ? GL_NOTEQUAL : GL_ALWAYS, 0, stencilPlaneToTestAgainst));
        _TARP_ASSERT_NO_GL_ERROR(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
        if (s->fillRule == kTpFillRuleEvenOdd)
        {
            _TARP_ASSERT_NO_GL_ERROR(glStencilMask(stencilPlaneToWriteTo));
            _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
            _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT));

            for (i = 0; i < p->contours.count; ++i)
            {
                _tpGLContour * c = _tpGLContourArrayAtPtr(&p->contours, i);
                _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_FAN, c->fillVertexOffset, c->fillVertexCount));
            }

            if (_bIsClipPath) return tpFalse;

            _TARP_ASSERT_NO_GL_ERROR(glStencilFunc(GL_EQUAL, 255, _kTpGLFillRasterStencilPlane));
        }
        else if (s->fillRule == kTpFillRuleNonZero)
        {
            //NonZero winding rule needs to use Increment and Decrement stencil operations.
            //we therefore render to the rasterize mask, even if this is a clipping mask, and transfer
            //the results to the clipping mask stencil plane afterwards
            _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLFillRasterStencilPlane));
            _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP));
            _TARP_ASSERT_NO_GL_ERROR(glEnable(GL_CULL_FACE));
            _TARP_ASSERT_NO_GL_ERROR(glCullFace(GL_BACK));
            _TARP_ASSERT_NO_GL_ERROR(glFrontFace(GL_CCW));

            for (i = 0; i < p->contours.count; ++i)
            {
                _tpGLContour * c = _tpGLContourArrayAtPtr(&p->contours, i);
                _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_FAN, c->fillVertexOffset, c->fillVertexCount));
            }

            _TARP_ASSERT_NO_GL_ERROR(glFrontFace(GL_CW));
            _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_DECR_WRAP));

            for (i = 0; i < p->contours.count; ++i)
            {
                _tpGLContour * c = _tpGLContourArrayAtPtr(&p->contours, i);
                _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_FAN, c->fillVertexOffset, c->fillVertexCount));
            }

            _TARP_ASSERT_NO_GL_ERROR(glDisable(GL_CULL_FACE));
            _TARP_ASSERT_NO_GL_ERROR(glFrontFace(GL_CW));

            if (_bIsClipPath)
            {
                _TARP_ASSERT_NO_GL_ERROR(glStencilMask(stencilPlaneToWriteTo));
                _TARP_ASSERT_NO_GL_ERROR(glStencilFunc(GL_NOTEQUAL, 0, _kTpGLFillRasterStencilPlane));
                _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT));

                _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, p->boundsVertexOffset, 4));

                //draw the bounds one last time to zero out the tmp data created in the _kTpGLFillRasterStencilPlane
                _TARP_ASSERT_NO_GL_ERROR(glStencilMask(stencilPlaneToWriteTo));
                _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO));
                _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, p->boundsVertexOffset, 4));

                return tpFalse;
            }
            else
            {
                _TARP_ASSERT_NO_GL_ERROR(glStencilFunc(GL_NOTEQUAL, 0, _kTpGLFillRasterStencilPlane));
            }
        }

        _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLFillRasterStencilPlane));
        _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO));
        _TARP_ASSERT_NO_GL_ERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
        _tpGLDrawPaint(_ctx, p, &s->fill, &p->fillGradientData);
    }

    // we don't care for stroke if this is a clipping path
    if (_bIsClipPath) return tpFalse;

    //draw the stroke
    if (p->strokeVertexCount)
    {
        _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLStrokeRasterStencilPlane));
        _TARP_ASSERT_NO_GL_ERROR(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
        _TARP_ASSERT_NO_GL_ERROR(glStencilFunc(_ctx->clippingStackDepth ? GL_NOTEQUAL : GL_ALWAYS, 0, stencilPlaneToTestAgainst));
        _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

        //Draw all stroke triangles of all contours at once
        _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLES, p->strokeVertexOffset, p->strokeVertexCount));

        _TARP_ASSERT_NO_GL_ERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
        _TARP_ASSERT_NO_GL_ERROR(glStencilFunc(GL_EQUAL, 0, _kTpGLStrokeRasterStencilPlane));
        _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT));

        _tpGLDrawPaint(_ctx, p, &s->stroke, &p->strokeGradientData);
    }

    return tpFalse;
}

tpBool _tpGLDrawPath(tpContext * _ctx, tpPath _path, const tpStyle _style)
{
    return _tpGLDrawPathImpl((_tpGLContext *)_ctx->_impl, (_tpGLPath *)_path.pointer, _style, tpFalse);
}

tpBool _tpGLGenerateClippingMask(_tpGLContext * _ctx, _tpGLPath * _path, tpBool _bIsRebuilding)
{
    tpBool drawResult;
    assert(ctx);

    if (!_bIsRebuilding)
        _ctx->clippingStack[_ctx->clippingStackDepth++] = _path;

    //@TODO: Instead of clearing maybe just clear it in endClipping by
    //drawing the bounds of the last clip path?
    _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_ctx->currentClipStencilPlane));
    _TARP_ASSERT_NO_GL_ERROR(glClearStencil(0));
    _TARP_ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));

    //draw path
    drawResult = _tpGLDrawPathImpl(_ctx, _path, _ctx->clippingStyle, tpTrue);
    if (drawResult) return tpTrue;

    _ctx->currentClipStencilPlane = _ctx->currentClipStencilPlane == _kTpGLClippingStencilPlaneOne ?
                                    _kTpGLClippingStencilPlaneTwo : _kTpGLClippingStencilPlaneOne;

    return tpFalse;
}

tpBool _tpGLBeginClipping(tpContext * _ctx, tpPath _path)
{
    return _tpGLGenerateClippingMask((_tpGLContext *)_ctx->_impl, (_tpGLPath *)_path.pointer, tpFalse);
}

tpBool _tpGLEndClipping(tpContext * _ctx)
{
    _tpGLPath * p;
    _tpGLContext * ctx = (_tpGLContext *)_ctx->_impl;
    assert(ctx->clippingStackDepth);
    printf("clip stack depth %i\n", ctx->clippingStackDepth);
    p = ctx->clippingStack[--ctx->clippingStackDepth];

    if (ctx->clippingStackDepth)
    {
        //check if the last clip mask is still in one of the clipping planes...
        if (ctx->bCanSwapStencilPlanes)
        {
            printf("swap END CLIP\n");
            ctx->currentClipStencilPlane = ctx->currentClipStencilPlane == _kTpGLClippingStencilPlaneOne ?
                                           _kTpGLClippingStencilPlaneTwo : _kTpGLClippingStencilPlaneOne;
            ctx->bCanSwapStencilPlanes = tpFalse;
        }
        else
        {
            printf("rebuild END CLIP %i\n", ctx->clippingStackDepth);
            //...otherwise rebuild it
            _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLClippingStencilPlaneOne | _kTpGLClippingStencilPlaneTwo));
            _TARP_ASSERT_NO_GL_ERROR(glClearStencil(255));
            _TARP_ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));

            for (int i = 0; i < ctx->clippingStackDepth; ++i)
            {
                //draw clip path
                _tpGLGenerateClippingMask(ctx, ctx->clippingStack[i], tpTrue);
            }

            ctx->bCanSwapStencilPlanes = tpTrue;
        }
    }
    else
    {
        //@TODO: Instead of clearing maybe just redrawing the clipping path bounds to
        //reset the stencil? Might scale better for a lot of paths :)
        _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLClippingStencilPlaneOne | _kTpGLClippingStencilPlaneTwo));
        _TARP_ASSERT_NO_GL_ERROR(glClearStencil(255));
        _TARP_ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));
    }
}

tpBool _tpGLResetClipping(tpContext * _ctx)
{
    _tpGLContext * ctx = (_tpGLContext *)_ctx->_impl;
    _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLClippingStencilPlaneOne | _kTpGLClippingStencilPlaneTwo));
    _TARP_ASSERT_NO_GL_ERROR(glClearStencil(0));
    _TARP_ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));

    ctx->currentClipStencilPlane = _kTpGLClippingStencilPlaneOne;
    ctx->clippingStackDepth = 0;
}

tpBool _tpGLSetProjection(tpContext * _ctx, const tpMat4 * _projection)
{
    _tpGLContext * ctx = (_tpGLContext *)_ctx->_impl;
    ctx->projection = *_projection;
    ctx->projectionID++;
    /*for (int i = 0; i < ctx->paths.count; ++i)
    {
        _tpGLPathPtrArrayAt(&ctx->paths, i)->bTransformProjDirty = tpTrue;
    }*/
    return tpFalse;
}

TARP_API tpImplementation tpOpenGLImplementation()
{
    tpImplementation ret;

    ret.pathCreate = _tpGLPathCreate;
    ret.pathDestroy = _tpGLPathDestroy;
    ret.pathClear = _tpGLPathClear;
    ret.pathSetTransform = _tpGLPathSetTransform;
    ret.pathAddCircle = _tpGLPathAddCircle;
    ret.pathAddEllipse = _tpGLPathAddEllipse;
    ret.pathAddRect = _tpGLPathAddRect;
    ret.pathAddSegment = _tpGLPathAddSegment;
    ret.pathMoveTo = _tpGLPathMoveTo;
    ret.pathLineTo = _tpGLPathLineTo;
    ret.pathCubicCurveTo = _tpGLPathCubicCurveTo;
    ret.pathQuadraticCurveTo = _tpGLPathQuadraticCurveTo;
    ret.pathClose = _tpGLPathClose;
    ret.pathRemoveContour = _tpGLPathRemoveContour;
    ret.pathAddSegments = _tpGLPathAddSegments;
    ret.pathAddContour = _tpGLPathAddContour;

    ret.styleCreate = _tpGLStyleCreate;
    ret.styleDestroy = _tpGLStyleDestroy;
    ret.styleSetDashArray = _tpGLStyleSetDashArray;
    ret.styleSetDashOffset = _tpGLStyleSetDashOffset;
    ret.styleSetFillColor = _tpGLStyleSetFillColor;
    ret.styleSetStrokeColor = _tpGLStyleSetStrokeColor;
    ret.styleSetFillGradient = _tpGLStyleSetFillGradient;
    ret.styleSetStrokeGradient = _tpGLStyleSetStrokeGradient;
    ret.styleSetStrokeWidth = _tpGLStyleSetStrokeWidth;
    ret.styleSetStrokeJoin = _tpGLStyleSetStrokeJoin;
    ret.styleSetScaleStroke = _tpGLStyleSetScaleStroke;
    ret.styleSetMiterLimit = _tpGLStyleSetMiterLimit;
    ret.styleSetStrokeCap = _tpGLStyleSetStrokeCap;
    ret.styleSetFillRule = _tpGLStyleSetFillRule;
    ret.styleRemoveFill = _tpGLStyleRemoveFill;
    ret.styleRemoveStroke = _tpGLStyleRemoveStroke;

    ret.gradientCreateLinear = _tpGLGradientCreateLinear;
    ret.gradientAddColorStop = _tpGLGradientAddColorStop;
    ret.gradientDestroy = _tpGLGradientDestroy;

    ret.contextInit = _tpGLContextInit;
    ret.contextDeallocate = _tpGLContextDeallocate;
    ret.prepareDrawing = _tpGLPrepareDrawing;
    ret.finishDrawing = _tpGLFinishDrawing;
    ret.setProjection = _tpGLSetProjection;
    ret.drawPath = _tpGLDrawPath;
    ret.beginClipping = _tpGLBeginClipping;
    ret.endClipping = _tpGLEndClipping;
    ret.resetClipping = _tpGLResetClipping;

    return ret;
}
#endif //TARP_IMPLEMENTATION
#endif //TARP_INCLUDE_OPENGL_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif //TARP_TARP_H
