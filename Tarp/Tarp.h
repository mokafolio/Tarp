/*
Tarp - v0.1.1
~~~~~~~~~~~~~~~~~~~~~~~~~~
Tarp is an almost single header C library to raster vector graphics.
It provides a lightweight and portable API purely focussed on decently
fast rendering without any gimmicks.

How to use
~~~~~~~~~~~~~~~~~~~~~~~~~~
Install Tarp or add the Tarp folder to your source directory. Include it into your project with #include <Tarp/Tarp.h>.
Specify the implementation you want to compile in one c/c++ file to create the implementation, i.e.:

#define TARP_IMPLEMENTATION_OPENGL
#include <Tarp/Tarp.h>

LICENSE
~~~~~~~~~~~~~~~~~~~~~~~~~~
See end of file.
*/

#ifndef TARP_TARP_H
#define TARP_TARP_H

/* @TODO: Double check if we actually need all these */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <float.h>

/* debug */
#if !defined(NDEBUG)
#define TARP_DEBUG
#else
#undef TARP_DEBUG
#endif

/* visibility */
/*
You can overwrite these (i.e. if your comiler does not support them or you want different visibility),
by simply defining them before including tarp.
*/
#ifndef TARP_API
#define TARP_API  __attribute__ ((visibility("default")))
#define TARP_LOCAL __attribute__ ((visibility("hidden")))
#endif

/*
memory allocation, you can define you own before including tarp for custom
memory allocation!
*/
#ifndef TARP_MALLOC
#define TARP_MALLOC(_bc) malloc(_bc)
#define TARP_REALLOC(_ptr, _bc) realloc(_ptr, _bc)
#define TARP_FREE(_ptr) free(_ptr)
#endif

#ifdef TARP_IMPLEMENTATION_OPENGL
#ifdef TARP_DEBUG
#define _TARP_ASSERT_NO_GL_ERROR(_func) do { GLenum glerr; _func; \
glerr = glGetError(); \
if(glerr != GL_NO_ERROR) \
{ \
switch(glerr) \
{ \
case GL_NO_ERROR: \
fprintf(stderr, "%s line %i GL_NO_ERROR: No error has been recorded.\n", __FILE__, __LINE__);\
break; \
case GL_INVALID_ENUM: \
fprintf(stderr, "%s line %i GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.\n", __FILE__, __LINE__);\
break; \
case GL_INVALID_VALUE: \
fprintf(stderr, "%s line %i GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.\n", __FILE__, __LINE__);\
break; \
case GL_INVALID_OPERATION: \
fprintf(stderr, "%s line %i GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.\n", __FILE__, __LINE__);\
break; \
case GL_INVALID_FRAMEBUFFER_OPERATION: \
fprintf(stderr, "%s line %i GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.\n", __FILE__, __LINE__);\
break; \
case GL_OUT_OF_MEMORY: \
fprintf(stderr, "%s line %i GL_OUT_OF_MEMORY: There is not enough memory left to executeLua the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.\n", __FILE__, __LINE__);\
break; \
} \
exit(EXIT_FAILURE); \
} \
} while(0)
#else
#define _TARP_ASSERT_NO_GL_ERROR(_func) _func
#endif /* TARP_DEBUG */

/* some tarp related opengl related settings */
#define TARP_GL_RAMP_TEXTURE_SIZE 1024
#define TARP_GL_MAX_CLIPPING_STACK_DEPTH 64
#define TARP_GL_ERROR_MESSAGE_SIZE 512

#endif /* TARP_IMPLEMENTATION_OPENGL */

/* some settings that you most likely won't have to touch*/
#define TARP_MAX_COLOR_STOPS 128
#define TARP_MAX_DASH_ARRAY_SIZE 64
#define TARP_MAX_ERROR_MESSAGE 256
#define TARP_MAX_CURVE_SUBDIVISIONS 16
#define TARP_RADIAL_GRADIENT_SLICES 64

/* some helper macros */
#define TARP_MIN(a,b) (((a)<(b))?(a):(b))
#define TARP_MAX(a,b) (((a)>(b))?(a):(b))
#define TARP_CLAMP(a,l,h) TARP_MAX(TARP_MIN(a, h),l)

/* some constants */
#define TARP_KAPPA 0.55228474983f
#define TARP_PI 3.14159265358979323846f
#define TARP_HALF_PI (TARP_PI * 0.5f)

/* define TARP_IMPLEMENTATION if any implementation is defined */
#ifdef TARP_IMPLEMENTATION_OPENGL
#define TARP_IMPLEMENTATION
#endif /* TARP_IMPLEMENTATION_OPENGL */

/*
helper to generate a typesafe handle class.
*/
#define TARP_HANDLE(_t) \
typedef struct TARP_API \
{ \
    void * pointer; \
} _t

#define TARP_HANDLE_FUNCTIONS(_t) \
tpBool _t##IsValidHandle(_t _val) \
{ \
    return (tpBool)(_val.pointer != NULL); \
} \
_t _t##InvalidHandle() \
{\
    _t ret = {NULL}; \
    return ret; \
}

#ifdef __cplusplus
extern "C" {
#endif

/*
Constants
~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
typedef enum TARP_API
{
    kTpGradientTypeLinear,
    kTpGradientTypeRadial
} tpGradientType;

typedef enum TARP_API
{
    kTpPaintTypeNone,
    kTpPaintTypeColor,
    kTpPaintTypeGradient
} tpPaintType;

typedef enum TARP_API
{
    kTpFillRuleEvenOdd,
    kTpFillRuleNonZero
} tpFillRule;

typedef enum TARP_API
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

/*
Basic Types
~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
typedef float tpFloat;
typedef enum TARP_API
{
    tpFalse = 0,
    tpTrue = 1
} tpBool;

TARP_HANDLE(tpPath);
TARP_HANDLE(tpStyle);
TARP_HANDLE(tpGradient);

/*
Structures
~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
typedef struct TARP_API
{
    tpFloat r, g, b, a;
} tpColor;

typedef struct TARP_API
{
    tpFloat x;
    tpFloat y;
} tpVec2;

typedef struct TARP_API
{
    tpFloat v[4];
} tpMat2;

typedef struct TARP_API
{
    tpMat2 m;
    tpVec2 t;
} tpTransform;

typedef struct TARP_API
{
    tpFloat v[16];
} tpMat4;

typedef struct TARP_API
{
    tpColor color;
    tpFloat offset;
} tpColorStop;

typedef struct TARP_API
{
    tpVec2 handleIn;
    tpVec2 position;
    tpVec2 handleOut;
} tpSegment;

typedef union TARP_API
{
    tpGradient gradient;
    tpColor color;
} _tpPaintUnion;

typedef struct TARP_API
{
    _tpPaintUnion data;
    tpPaintType type;
} tpPaint;

/*typedef struct TARP_API
{
    void * _impl;
} tpContext;*/
TARP_HANDLE(tpContext);

/*
NOTE: All the color, matrix and vector functions are mainly for internal use
The rotation/scale/projection functionality are mainly provided to be used in the examples.
*/

/*
Color Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
TARP_API tpColor tpColorMake(tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a);

/*
Vector Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
TARP_API tpVec2 tpVec2Make(tpFloat _x, tpFloat _y);

TARP_API tpVec2 tpVec2Add(tpVec2 _a, tpVec2 _b);

TARP_API tpVec2 tpVec2Sub(tpVec2 _a, tpVec2 _b);

TARP_API tpVec2 tpVec2Mult(tpVec2 _a, tpVec2 _b);

TARP_API tpVec2 tpVec2MultScalar(tpVec2 _a, tpFloat _b);

TARP_API tpVec2 tpVec2Div(tpVec2 _a, tpVec2 _b);

TARP_API tpBool tpVec2Equals(tpVec2 _a, tpVec2 _b);

TARP_API tpFloat tpVec2Length(tpVec2 _vec);

TARP_API tpFloat tpVec2LengthSquared(tpVec2 _vec);

TARP_API tpFloat tpVec2Dot(tpVec2 _a, tpVec2 _b);

TARP_API tpFloat tpVec2Cross(tpVec2 _a, tpVec2 _b);

TARP_API void tpVec2NormalizeSelf(tpVec2 * _vec);

TARP_API tpVec2 tpVec2Normalize(tpVec2 _vec);

TARP_API tpVec2 tpVec2Perp(tpVec2 _a);

TARP_API tpFloat tpVec2Distance(tpVec2 _a, tpVec2 _b);

TARP_API tpFloat tpVec2DistanceSquared(tpVec2 _a, tpVec2 _b);

TARP_API tpVec2 tpVec2Lerp(tpVec2 _a, tpVec2 _b, tpFloat _t);


/*
Matrix Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
TARP_API tpMat2 tpMat2Make(tpFloat _a, tpFloat _b, /* row1 */
                           tpFloat _c, tpFloat _d); /* row2 */

TARP_API tpMat2 tpMat2MakeIdentity();

TARP_API tpMat2 tpMat2MakeScale(tpFloat _x, tpFloat _y);

TARP_API tpMat2 tpMat2MakeSkew(tpFloat _x, tpFloat _y);

TARP_API tpMat2 tpMat2MakeRotation(tpFloat _angle);

TARP_API int tpMat2Decompose(const tpMat2 * _mat,
                             tpVec2 * _outScale, tpVec2 * _outSkew, tpFloat * _outRotation);

TARP_API tpBool tpMat2Equals(const tpMat2 * _a, const tpMat2 * _b);

TARP_API tpVec2 tpMat2MultVec2(const tpMat2 * _mat, tpVec2 _vec);

TARP_API tpMat2 tpMat2Mult(const tpMat2 * _a, const tpMat2 * _b);

TARP_API tpMat2 tpMat2Invert(const tpMat2 * _mat);

TARP_API tpTransform tpTransformMake(tpFloat _a, tpFloat _b, tpFloat _x,
                                     tpFloat _c, tpFloat _d, tpFloat _y);

TARP_API tpTransform tpTransformMakeIdentity();

TARP_API tpTransform tpTransformMakeScale(tpFloat _x, tpFloat _y);

TARP_API tpTransform tpTransformMakeSkew(tpFloat _x, tpFloat _y);

TARP_API tpTransform tpTransformMakeRotation(tpFloat _angle);

TARP_API int tpTransformDecompose(const tpTransform * _trafo,
                                  tpVec2 * _outTranslation, tpVec2 * _outScale, tpVec2 * _outSkew, tpFloat * _outRotation);

TARP_API tpBool tpTransformEquals(const tpTransform * _a, const tpTransform * _b);

TARP_API tpVec2 tpTransformApply(const tpTransform * _trafo, tpVec2 _vec);

TARP_API tpTransform tpTransformCombine(const tpTransform * _a, const tpTransform * _b);

TARP_API tpMat4 tpMat4Make(tpFloat _v0, tpFloat _v1, tpFloat _v2, tpFloat _v3, /* row1 */
                           tpFloat _v4, tpFloat _v5, tpFloat _v6, tpFloat _v7, /* row2 */
                           tpFloat _v8, tpFloat _v9, tpFloat _v10, tpFloat _v11, /* row3 */
                           tpFloat _v12, tpFloat _v13, tpFloat _v14, tpFloat _v15); /* row4 */

TARP_API tpMat4 tpMat4MakeIdentity();

TARP_API tpMat4 tpMat4MakeOrtho(tpFloat _left, tpFloat _right, tpFloat _bottom, tpFloat _top, tpFloat _near, tpFloat _far);

TARP_API tpMat4 tpMat4MakeFrom2DTransform(const tpTransform * _transform);

TARP_API tpMat4 tpMat4Mult(const tpMat4 * _a, const tpMat4 * _b);


/*
Segment Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
/* creates a segment, you rarely need to call this directly */
TARP_API tpSegment tpSegmentMake(tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y);


/*
Path Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
TARP_API tpPath tpPathCreate();

TARP_API void tpPathDestroy(tpPath _path);

/* Set the fill transformation */
TARP_API tpBool tpPathSetFillPaintTransform(tpPath, const tpTransform * _transform);

/* Set the stroke transformation */
TARP_API tpBool tpPathSetStrokePaintTransform(tpPath, const tpTransform * _transform);

/* Adds a circle contour to the provided path */
TARP_API tpBool tpPathAddCircle(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _r);

/* Adds an ellipse contour to the provided path */
TARP_API tpBool tpPathAddEllipse(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height);

/* Adds a rectangle contour to the provided path */
TARP_API tpBool tpPathAddRect(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height);

/* Adds a segment to the current path contour */
TARP_API tpBool tpPathAddSegment(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y);

/* Starts a new contour at x, y */
TARP_API tpBool tpPathMoveTo(tpPath _path, tpFloat _x, tpFloat _y);

/* Connects the last segment of the current contour with x, y in a straight line */
TARP_API tpBool tpPathLineTo(tpPath _path, tpFloat _x, tpFloat _y);

/* Connects the last segment of the current contour with a cubic bezier curve to x, y */
TARP_API tpBool tpPathCubicCurveTo(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _h1x, tpFloat _h1y, tpFloat _px, tpFloat _py);

/* Connects the last segment of the current contour with a quadratic bezier curve to x, y */
TARP_API tpBool tpPathQuadraticCurveTo(tpPath _path, tpFloat _hx, tpFloat _hy, tpFloat _px, tpFloat _py);

/* Closes the current contour */
TARP_API tpBool tpPathClose(tpPath _path);

/* Removes all contours from the path */
TARP_API tpBool tpPathClear(tpPath _path);

/* Removes a contour by index */
TARP_API tpBool tpPathRemoveContour(tpPath _path, int _contourIndex);

/* Returns the number of contours */
TARP_API int tpPathContourCount(tpPath _path);

/* Removes one segment from a contour */
TARP_API tpBool tpPathRemoveSegment(tpPath _path, int _contourIndex, int _segmentIndex);

/* Removes a range of segments from a contour */
TARP_API tpBool tpPathRemoveSegments(tpPath _path, int _contourIndex, int _from, int _to);

/* Adds segments to the current contour */
TARP_API tpBool tpPathAddSegments(tpPath _path, tpSegment * _segments, int _count);

/* Adds a new contour to the path from the provided segments */
TARP_API tpBool tpPathAddContour(tpPath _path, tpSegment * _segments, int _count, tpBool _bClosed);

/*
Replaces a contour in the path with the provided segments.
If contourIndex is > than the number of existing number of contours,
a new contour will be added to the path.
 */
TARP_API tpBool tpPathSetContour(tpPath _path, int _contourIndex, tpSegment * _segments, int _count, tpBool _bClosed);

/* generates tpPathInvalidHandle() and tpPathIsValidHandle(tpPath) functions to generate
an invalid handle and check a handle for validity. */
TARP_HANDLE_FUNCTIONS(tpPath)

/*
Style Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
/* Creates a default initialized style */
TARP_API tpStyle tpStyleCreate();

TARP_API void tpStyleDestroy(tpStyle _style);

/* Defines a dash pattern for the stroke */
TARP_API void tpStyleSetDashArray(tpStyle _style, const tpFloat * _dashArray, int _count);

/* Specifies an offset into the dash pattern */
TARP_API void tpStyleSetDashOffset(tpStyle _style, tpFloat _offset);

/* Set the fill to a color */
TARP_API void tpStyleSetFillColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a);

/* Set the stroke to a color */
TARP_API void tpStyleSetStrokeColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a);

/* Set the fill to a gradient */
TARP_API void tpStyleSetFillGradient(tpStyle _style, const tpGradient _gradient);

/* Set the stroke to a gradient */
TARP_API void tpStyleSetStrokeGradient(tpStyle _style, const tpGradient _gradient);

/* Set the width of the stroke */
TARP_API void tpStyleSetStrokeWidth(tpStyle _style, tpFloat _strokeWidth);

/* Set the join of the stroke */
TARP_API void tpStyleSetStrokeJoin(tpStyle _style, tpStrokeJoin _join);

/*
Set if strokes scale with the path. If false, the stroke will ignore the
path transform and maintain its thickness.
 */
TARP_API void tpStyleSetScaleStroke(tpStyle _style, tpBool _b);

/* Set the miter limit of the stroke. Only affects strokes that use kTpStrokeJoinMiter */
TARP_API void tpStyleSetMiterLimit(tpStyle _style, tpFloat _limit);

/* Set the stroke cap of the style */
TARP_API void tpStyleSetStrokeCap(tpStyle _style, tpStrokeCap _cap);

/* Set the fill rule. This affects how overlapping contours and self intersecting paths generate holes */
TARP_API void tpStyleSetFillRule(tpStyle _style, tpFillRule _fillType);

/* Removes the fill from a style */
TARP_API void tpStyleRemoveFill(tpStyle _style);

/* Removes the stroke from a style */
TARP_API void tpStyleRemoveStroke(tpStyle _style);

/* Retrieves the current dash pattern */
TARP_API tpFloat * tpStyleDashArrayPtr(tpStyle _style);

/* Retrieves the number of dashes in the current dash pattern  */
TARP_API int tpStyleDashCount(tpStyle _style);

/* Retrieves the current dash offset into the dash pattern */
TARP_API tpFloat tpStyleDashOffset(tpStyle _style);

/* Retrieves the current fill */
TARP_API tpPaint tpStyleFillPaint(tpStyle _style);

/* Retrieves the current stroke */
TARP_API tpPaint tpStyleStrokePaint(tpStyle _style);

/* Retrieves the current stroke width */
TARP_API tpFloat tpStyleStrokeWidth(tpStyle _style);

/* Retrieves the current stroke join */
TARP_API tpStrokeJoin tpStyleStrokeJoin(tpStyle _style);

/* Retrieves if the stroke scales or not */
TARP_API tpBool tpStyleScaleStroke(tpStyle _style);

/* Retrieves the current miter limit */
TARP_API tpFloat tpStyleMiterLimit(tpStyle _style);

/* Retrieves the current stroke cap */
TARP_API tpStrokeCap tpStyleStrokeCap(tpStyle _style);

/* Retrieves the current fill rule */
TARP_API tpFillRule tpStyleFillRule(tpStyle _style);

/* generates tpStyleInvalidHandle() and tpStyleIsValidHandle(tpStyle) functions to generate
an invalid handle and check a handle for validity. */
TARP_HANDLE_FUNCTIONS(tpStyle)


/*
Gradient Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
/* Creates a linear gradient with the origin at x0, y0 and the destination at x1, y1 */
TARP_API tpGradient tpGradientCreateLinear(tpFloat _x0, tpFloat _y0, tpFloat _x1, tpFloat _y1);

/* Creates a radial gradient starting at fx, fy and running towards an ellipse with one semi-axis going from ox, oy, to x1, y1 and the other axis being scaled by the ratio */
TARP_API tpGradient tpGradientCreateRadial(tpFloat _fx, tpFloat _fy, tpFloat _ox, tpFloat _oy, tpFloat _dx, tpFloat _dy, tpFloat _ratio);

/* Creates a symmetric radial gradient with radius r at x, y */
TARP_API tpGradient tpGradientCreateRadialSymmetric(tpFloat _x, tpFloat _y, tpFloat _r);

/* Sets the origin to x0, y0 and the destination at x1, y1 */
TARP_API void tpGradientSetPositions(tpGradient _gradient, tpFloat _x0, tpFloat _y0, tpFloat _x1, tpFloat _y1);

/* Sets the focal point of a radial gradient to a position relative to the gradient's origin */
TARP_API void tpGradientSetFocalPointOffset(tpGradient _gradient, tpFloat _x, tpFloat _y);

/* Sets the ratio between the minor and major axis of an elliptical radial gradient */
TARP_API void tpGradientSetRatio(tpGradient _gradient, tpFloat _ratio);

/*
Adds a color stop to the gradient. Offset is in the range 0-1 where 0 positions the color stop at the
origin of the gradient and 1.0 at the destination.
*/
TARP_API void tpGradientAddColorStop(tpGradient _gradient, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a, tpFloat _offset);

/* remove all color stops */
TARP_API void tpGradientClearColorStops(tpGradient _gradient);

TARP_API void tpGradientDestroy(tpGradient _gradient);

/* generates tpGradientInvalidHandle() and tpGradientIsValidHandle(tpGradient) functions to generate
an invalid handle and check a handle for validity. */
TARP_HANDLE_FUNCTIONS(tpGradient)


/*
Context Related Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

/* Call this to initialize a tarp context. */
TARP_API tpContext tpContextCreate();

/* retrieves a context error message  */
TARP_API const char * tpContextErrorMessage(tpContext _ctx);

/* shut down a context */
TARP_API void tpContextDestroy(tpContext _ctx);

/* Call this at the beginning of a "frame" before you call any draw functions */
TARP_API tpBool tpPrepareDrawing(tpContext _ctx);

/* Call this at the end of a "frame" after you are done drawing */
TARP_API tpBool tpFinishDrawing(tpContext _ctx);

/* Set the projection */
TARP_API tpBool tpSetProjection(tpContext _ctx, const tpMat4 * _projection);

/* Set the path transformation. All following draw calls will be affected by it. */
TARP_API tpBool tpSetTransform(tpContext _ctx, const tpTransform * _transform);

/* Reset the transformation to the identity matrix */
TARP_API tpBool tpResetTransform(tpContext _ctx);

/* Draw a path with the provided style */
TARP_API tpBool tpDrawPath(tpContext _ctx, tpPath _path, const tpStyle _style);

/*
Define a clipping path. You can nest these calls. All following draw
calls will be clippied by the provided path.
*/
TARP_API tpBool tpBeginClipping(tpContext _ctx, tpPath _path);

/* End the most recent clipping path */
TARP_API tpBool tpEndClipping(tpContext _ctx);

/* End all clipping paths. This will remove all clipping. */
TARP_API tpBool tpResetClipping(tpContext _ctx);

/* Returns a string identifier of the current implementation */
TARP_API const char * tpImplementationName();

/* generates tpContextInvalidHandle() and tpContextIsValidHandle(tpContext) functions to generate
an invalid handle and check a handle for validity. */
TARP_HANDLE_FUNCTIONS(tpContext)


#ifdef TARP_IMPLEMENTATION

/*
Implementations
~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
TARP_API tpColor tpColorMake(tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a)
{
    tpColor ret; ret.r = _r; ret.g = _g; ret.b = _b; ret.a = _a;
    return ret;
}

TARP_API tpVec2 tpVec2Make(tpFloat _x, tpFloat _y)
{
    tpVec2 ret; ret.x = _x; ret.y = _y;
    return ret;
}

TARP_API tpVec2 tpVec2Add(tpVec2 _a, tpVec2 _b)
{
    tpVec2 ret; ret.x = _a.x + _b.x; ret.y = _a.y + _b.y;
    return ret;
}

TARP_API tpVec2 tpVec2Sub(tpVec2 _a, tpVec2 _b)
{
    tpVec2 ret; ret.x = _a.x - _b.x; ret.y = _a.y - _b.y;
    return ret;
}

TARP_API tpVec2 tpVec2Mult(tpVec2 _a, tpVec2 _b)
{
    tpVec2 ret; ret.x = _a.x * _b.x; ret.y = _a.y * _b.y;
    return ret;
}

TARP_API tpVec2 tpVec2MultScalar(tpVec2 _a, tpFloat _b)
{
    tpVec2 ret; ret.x = _a.x * _b; ret.y = _a.y * _b;
    return ret;
}

TARP_API tpVec2 tpVec2Div(tpVec2 _a, tpVec2 _b)
{
    tpVec2 ret; ret.x = _a.x / _b.x; ret.y = _a.y / _b.y;
    return ret;
}

TARP_API tpBool tpVec2Equals(tpVec2 _a, tpVec2 _b)
{
    return (tpBool)(_a.x == _b.x && _a.y == _b.y);
}

TARP_API tpFloat tpVec2Length(tpVec2 _vec)
{
    return sqrt(_vec.x * _vec.x + _vec.y * _vec.y);
}

TARP_API tpFloat tpVec2LengthSquared(tpVec2 _vec)
{
    return _vec.x * _vec.x + _vec.y * _vec.y;
}

TARP_API void tpVec2NormalizeSelf(tpVec2 * _vec)
{
    tpFloat s = 1 / tpVec2Length(*_vec);
    _vec->x *= s;
    _vec->y *= s;
}

TARP_API tpVec2 tpVec2Normalize(tpVec2 _vec)
{
    tpVec2 ret = _vec;
    tpFloat s = 1 / tpVec2Length(_vec);
    ret.x *= s; ret.y *= s;
    return ret;
}

TARP_API tpVec2 tpVec2Perp(tpVec2 _a)
{
    tpVec2 ret; ret.x = _a.y; ret.y = -_a.x;
    return ret;
}

TARP_API tpFloat tpVec2Dot(tpVec2 _a, tpVec2 _b)
{
    return _a.x * _b.x + _a.y * _b.y;
}

TARP_API tpFloat tpVec2Cross(tpVec2 _a, tpVec2 _b)
{
    return _a.x * _b.y - _a.y * _b.x;
}

TARP_API tpFloat tpVec2Distance(tpVec2 _a, tpVec2 _b)
{
    return tpVec2Length(tpVec2Sub(_a, _b));
}

TARP_API tpFloat tpVec2DistanceSquared(tpVec2 _a, tpVec2 _b)
{
    return tpVec2LengthSquared(tpVec2Sub(_a, _b));
}

TARP_API tpVec2 tpVec2Lerp(tpVec2 _a, tpVec2 _b, tpFloat _t)
{
    return tpVec2Add(tpVec2MultScalar(_a, 1.0f - _t), tpVec2MultScalar(_b, _t));
}

TARP_API tpMat2 tpMat2Make(tpFloat _a, tpFloat _b,
                           tpFloat _c, tpFloat _d)
{
    tpMat2 ret;
    ret.v[0] = _a; ret.v[2] = _b;
    ret.v[1] = _c; ret.v[3] = _d;
    return ret;
}

TARP_API tpMat2 tpMat2MakeIdentity()
{
    return tpMat2Make(1, 0,
                      0, 1);
}

TARP_API tpMat2 tpMat2MakeScale(tpFloat _x, tpFloat _y)
{
    return tpMat2Make(_x, 0,
                      0, _y);
}

TARP_API tpMat2 tpMat2MakeSkew(tpFloat _x, tpFloat _y)
{
    return tpMat2Make(1, tan(_x),
                      tan(_y), 1);
}

TARP_API tpMat2 tpMat2MakeRotation(tpFloat _angle)
{
    tpFloat c = cos(_angle);
    tpFloat s = sin(_angle);
    return tpMat2Make(c, -s,
                      s, c);
}

TARP_API int tpMat2Decompose(const tpMat2 * _mat, tpVec2 * _outScale, tpVec2 * _outSkew, tpFloat * _outRotation)
{
    tpFloat a, b, c, d, det;

    a = _mat->v[0];
    b = _mat->v[2];
    c = _mat->v[1];
    d = _mat->v[3];
    det = a * d - b * c;

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
    else /* a = b = c = d = 0 */
    {
        *_outRotation = 0;
        _outScale->x = 0;
        _outScale->y = 0;
        _outSkew->x = 0;
        _outSkew->y = 0;
    }

    return 0;
}

TARP_API tpBool tpMat2Equals(const tpMat2 * _a, const tpMat2 * _b)
{
    return (tpBool)(_a->v[0] == _b->v[0] && _a->v[1] == _b->v[1] &&
                    _a->v[2] == _b->v[2] && _a->v[3] == _b->v[3]);
}

TARP_API tpVec2 tpMat2MultVec2(const tpMat2 * _mat, tpVec2 _vec)
{
    tpVec2 ret;
    ret.x = _vec.x * _mat->v[0] + _vec.y * _mat->v[2];
    ret.y = _vec.x * _mat->v[1] + _vec.y * _mat->v[3];
    return ret;
}

TARP_API tpMat2 tpMat2Mult(const tpMat2 * _a, const tpMat2 * _b)
{
    tpMat2 ret;

    ret.v[0] = _b->v[0] * _a->v[0] + _b->v[1] * _a->v[2];
    ret.v[1] = _b->v[0] * _a->v[1] + _b->v[1] * _a->v[3];
    ret.v[2] = _b->v[2] * _a->v[0] + _b->v[3] * _a->v[2];
    ret.v[3] = _b->v[2] * _a->v[1] + _b->v[3] * _a->v[3];

    return ret;
}

TARP_API tpMat2 tpMat2Invert(const tpMat2 * _mat)
{
    tpMat2 ret;
    tpFloat inv_det = 1 / (_mat->v[0] * _mat->v[3] - _mat->v[1] * _mat->v[2]);
    ret.v[0] =  _mat->v[3] * inv_det;
    ret.v[1] = -_mat->v[1] * inv_det;
    ret.v[2] = -_mat->v[2] * inv_det;
    ret.v[3] =  _mat->v[0] * inv_det;
    return ret;
}

TARP_API tpTransform tpTransformInvert(const tpTransform * _trafo)
{
    tpTransform ret;
    ret.m = tpMat2Invert(&_trafo->m);
    ret.t = tpMat2MultVec2(&ret.m, tpVec2Make(-_trafo->t.x, -_trafo->t.y));
    return ret;
}

TARP_API tpTransform tpTransformMake(tpFloat _a, tpFloat _b, tpFloat _x,
                                     tpFloat _c, tpFloat _d, tpFloat _y)
{
    tpTransform ret;
    ret.m = tpMat2Make(_a, _b, _c, _d);
    ret.t = tpVec2Make(_x, _y);
    return ret;
}

TARP_API tpTransform tpTransformMakeIdentity()
{
    tpTransform ret;
    ret.m = tpMat2MakeIdentity();
    ret.t = tpVec2Make(0, 0);
    return ret;
}

TARP_API tpTransform tpTransformMakeTranslation(tpFloat _x, tpFloat _y)
{
    tpTransform ret;
    ret.m = tpMat2MakeIdentity();
    ret.t = tpVec2Make(_x, _y);
    return ret;
}

TARP_API tpTransform tpTransformMakeScale(tpFloat _x, tpFloat _y)
{
    tpTransform ret;
    ret.m = tpMat2MakeScale(_x, _y);
    ret.t = tpVec2Make(0, 0);
    return ret;
}

TARP_API tpTransform tpTransformMakeSkew(tpFloat _x, tpFloat _y)
{
    tpTransform ret;
    ret.m = tpMat2MakeSkew(_x, _y);
    ret.t = tpVec2Make(0, 0);
    return ret;
}

TARP_API tpTransform tpTransformMakeRotation(tpFloat _angle)
{
    tpTransform ret;
    ret.m = tpMat2MakeRotation(_angle);
    ret.t = tpVec2Make(0, 0);
    return ret;
}

TARP_API int tpTransformDecompose(const tpTransform * _trafo,
                                  tpVec2 * _outTranslation, tpVec2 * _outScale, tpVec2 * _outSkew, tpFloat * _outRotation)
{
    *_outTranslation = _trafo->t;
    return tpMat2Decompose(&_trafo->m, _outScale, _outSkew, _outRotation);
}

TARP_API tpBool tpTransformEquals(const tpTransform * _a, const tpTransform * _b)
{
    return (tpBool)(tpMat2Equals(&_a->m, &_b->m) && tpVec2Equals(_a->t, _b->t));
}

TARP_API tpVec2 tpTransformApply(const tpTransform * _trafo, tpVec2 _vec)
{
    return tpVec2Add(tpMat2MultVec2(&_trafo->m, _vec), _trafo->t);
}

TARP_API tpTransform tpTransformCombine(const tpTransform * _a, const tpTransform * _b)
{
    tpTransform ret;
    ret.t = tpTransformApply(_a, _b->t);
    ret.m = tpMat2Mult(&_a->m, &_b->m);
    return ret;
}

TARP_API tpMat4 tpMat4Make(tpFloat _v0, tpFloat _v1, tpFloat _v2, tpFloat _v3,
                           tpFloat _v4, tpFloat _v5, tpFloat _v6, tpFloat _v7,
                           tpFloat _v8, tpFloat _v9, tpFloat _v10, tpFloat _v11,
                           tpFloat _v12, tpFloat _v13, tpFloat _v14, tpFloat _v15)
{
    tpMat4 ret;
    ret.v[0] = _v0; ret.v[1] = _v4; ret.v[2] = _v8; ret.v[3] = _v12;
    ret.v[4] = _v1; ret.v[5] = _v5; ret.v[6] = _v9; ret.v[7] = _v13;
    ret.v[8] = _v2; ret.v[9] = _v6; ret.v[10] = _v10; ret.v[11] = _v14;
    ret.v[12] = _v3; ret.v[13] = _v7; ret.v[14] = _v11; ret.v[15] = _v15;
    return ret;
}

TARP_API tpMat4 tpMat4MakeIdentity()
{
    return tpMat4Make( 1, 0, 0, 0,
                       0, 1, 0, 0,
                       0, 0, 1, 0,
                       0, 0, 0, 1);
}

TARP_API tpMat4 tpMat4MakeOrtho(tpFloat _left, tpFloat _right, tpFloat _bottom, tpFloat _top, tpFloat _near, tpFloat _far)
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

TARP_API tpMat4 tpMat4MakeFrom2DTransform(const tpTransform * _transform)
{
    return tpMat4Make(_transform->m.v[0], _transform->m.v[2], 0, _transform->t.x,
                      _transform->m.v[1], _transform->m.v[3], 0, _transform->t.y,
                      0, 0, 1, 0,
                      0, 0, 0, 1.0);
}

TARP_API tpMat4 tpMat4Mult(const tpMat4 * _a, const tpMat4 * _b)
{
    tpMat4 ret;

    ret.v[0] = _b->v[0] * _a->v[0] + _b->v[1] * _a->v[4] + _b->v[2] * _a->v[8]  + _b->v[3] * _a->v[12];
    ret.v[1] = _b->v[0] * _a->v[1] + _b->v[1] * _a->v[5] + _b->v[2] * _a->v[9]  + _b->v[3] * _a->v[13];
    ret.v[2] = _b->v[0] * _a->v[2] + _b->v[1] * _a->v[6] + _b->v[2] * _a->v[10] + _b->v[3] * _a->v[14];
    ret.v[3] = _b->v[0] * _a->v[3] + _b->v[1] * _a->v[7] + _b->v[2] * _a->v[11] + _b->v[3] * _a->v[15];
    ret.v[4] = _b->v[4] * _a->v[0] + _b->v[5] * _a->v[4] + _b->v[6] * _a->v[8]  + _b->v[7] * _a->v[12];
    ret.v[5] = _b->v[4] * _a->v[1] + _b->v[5] * _a->v[5] + _b->v[6] * _a->v[9]  + _b->v[7] * _a->v[13];
    ret.v[6] = _b->v[4] * _a->v[2] + _b->v[5] * _a->v[6] + _b->v[6] * _a->v[10] + _b->v[7] * _a->v[14];
    ret.v[7] = _b->v[4] * _a->v[3] + _b->v[5] * _a->v[7] + _b->v[6] * _a->v[11] + _b->v[7] * _a->v[15];
    ret.v[8] = _b->v[8] * _a->v[0] + _b->v[9] * _a->v[4] + _b->v[10] * _a->v[8]  + _b->v[11] * _a->v[12];
    ret.v[9] = _b->v[8] * _a->v[1] + _b->v[9] * _a->v[5] + _b->v[10] * _a->v[9]  + _b->v[11] * _a->v[13];
    ret.v[10] = _b->v[8] * _a->v[2] + _b->v[9] * _a->v[6] + _b->v[10] * _a->v[10] + _b->v[11] * _a->v[14];
    ret.v[11] = _b->v[8] * _a->v[3] + _b->v[9] * _a->v[7] + _b->v[10] * _a->v[11] + _b->v[11] * _a->v[15];
    ret.v[12] = _b->v[12] * _a->v[0] + _b->v[13] * _a->v[4] + _b->v[14] * _a->v[8]  + _b->v[15] * _a->v[12];
    ret.v[13] = _b->v[12] * _a->v[1] + _b->v[13] * _a->v[5] + _b->v[14] * _a->v[9]  + _b->v[15] * _a->v[13];
    ret.v[14] = _b->v[12] * _a->v[2] + _b->v[13] * _a->v[6] + _b->v[14] * _a->v[10] + _b->v[15] * _a->v[14];
    ret.v[15] = _b->v[12] * _a->v[3] + _b->v[13] * _a->v[7] + _b->v[14] * _a->v[11] + _b->v[15] * _a->v[15];

    return ret;
}

TARP_API tpSegment tpSegmentMake(tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y)
{
    tpSegment ret;
    ret.handleIn = tpVec2Make(_h0x, _h0y);
    ret.position = tpVec2Make(_px, _py);
    ret.handleOut = tpVec2Make(_h1x, _h1y);
    return ret;
}

#ifdef TARP_IMPLEMENTATION_OPENGL

/* @TODO: Clean up the layout of all the structs */

/* The shader programs used by the renderer */
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
    "} \n";

typedef struct _tpGLContext _tpGLContext;

typedef enum TARP_LOCAL
{
    /*
    The mask values are important! FillRaster needs to have at least
    2 bits. These need to be the lower bits in order to work with the
    Increment, Decrement stencil operations
    http://www.opengl.org/discussion_boards/showthread.php/149740-glStencilOp-s-GL_INCR-GL_DECR-behaviour-when-masked
    */
    _kTpGLFillRasterStencilPlane = 0x1F, /* binary mask    00011111 */
    _kTpGLClippingStencilPlaneOne = 1 << 5, /* binary mask     00100000 */
    _kTpGLClippingStencilPlaneTwo = 1 << 6, /* binary mask     01000000 */
    _kTpGLStrokeRasterStencilPlane = 1 << 7 /* binary mask 10000000 */
} _tpGLStencilPlane;

typedef struct TARP_LOCAL
{
    tpVec2 min, max;
} _tpGLRect;

typedef struct TARP_LOCAL
{
    tpVec2 p0, h0, h1, p1;
} _tpGLCurve;

typedef struct TARP_LOCAL
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

typedef struct TARP_LOCAL
{
    _tpSegmentArray segments;
    tpBool bDirty, bIsClosed, bLengthDirty;
    int lastSegmentIndex;

    /* some rendering specific data */
    int fillVertexOffset;
    int fillVertexCount;
    int strokeVertexOffset;
    int strokeVertexCount;

    _tpGLRect bounds;
    tpFloat length;
} _tpGLContour;

#define _TARP_ARRAY_T _tpGLContourArray
#define _TARP_ITEM_T _tpGLContour
#define _TARP_COMPARATOR_T 0
#include <Tarp/TarpArray.h>

typedef struct TARP_LOCAL
{
    tpVec2 vertex;
    tpVec2 tc;
} _tpGLTextureVertex;

#define _TARP_ARRAY_T _tpGLTextureVertexArray
#define _TARP_ITEM_T _tpGLTextureVertex
#define _TARP_COMPARATOR_T 0
#include <Tarp/TarpArray.h>

typedef struct TARP_LOCAL
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

typedef struct TARP_LOCAL
{
    int gradientID;
    tpVec2 origin;
    tpVec2 destination;
    tpFloat ratio;
    tpVec2 focal_point_offset;
    _tpColorStopArray stops;
    tpGradientType type;

    /* rendering specific data/caches */
    tpBool bDirty;
    GLuint rampTexture;
} _tpGLGradient;

typedef struct TARP_LOCAL
{
    int lastGradientID;
    _tpGLRect * bounds; /* the bounds used by this gradient (i.e. stroke or fill) */
    int vertexOffset;
    int vertexCount;

} _tpGLGradientCacheData;

typedef struct TARP_LOCAL
{
    _tpGLContourArray contours;
    int currentContourIndex;
    char errorMessage[TARP_GL_ERROR_MESSAGE_SIZE];
    tpTransform transform;

    /* rendering specific data/caches */
    _tpVec2Array geometryCache;
    _tpGLTextureVertexArray textureGeometryCache;
    _tpBoolArray jointCache;

    tpBool bPathGeometryDirty;
    tpFloat lastTransformScale;
    _tpGLRect boundsCache;
    _tpGLRect strokeBoundsCache;

    /* holds the properties that the stroke geometry was generated with */
    _tpGLStrokeData lastStroke;

    _tpGLGradientCacheData fillGradientData;
    _tpGLGradientCacheData strokeGradientData;

    int strokeVertexOffset;
    int strokeVertexCount;
    int boundsVertexOffset;

    _tpGLContext * lastDrawContext;
    int lastTransformID;

    tpTransform fillPaintTransform;
    tpTransform strokePaintTransform;
    tpBool bFillPaintTransformDirty;
    tpBool bStrokePaintTransformDirty;
} _tpGLPath;

typedef struct TARP_LOCAL
{
    tpPaint fill;
    tpPaint stroke;
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

typedef struct TARP_LOCAL
{
    GLuint vao;
    GLuint vbo;
    GLuint vboSize;
} _tpGLVAO;

typedef struct TARP_LOCAL
{
    GLenum activeTexture;
    GLboolean depthTest;
    GLboolean depthMask;
    GLboolean multisample;
    GLboolean stencilTest;
    GLuint stencilMask;
    GLenum stencilFail;
    GLenum stencilPassDepthPass;
    GLenum stencilPassDepthFail;
    GLuint clearStencil;
    GLboolean blending;
    GLenum blendSrcRGB;
    GLenum blendDestRGB;
    GLenum blendSrcAlpha;
    GLenum blendDestAlpha;
    GLenum blendEquationRGB;
    GLenum blendEquationAlpha;
    GLboolean cullFace;
    GLenum cullFaceMode;
    GLenum frontFace;
    GLuint vao;
    GLuint vbo;
    GLuint program;
} _tpGLStateBackup;

struct _tpGLContext
{
    GLuint program;
    GLuint textureProgram;
    GLuint tpLoc;
    GLuint tpTextureLoc;
    GLuint meshColorLoc;

    _tpGLVAO vao;
    _tpGLVAO textureVao;

    _tpGLPath * clippingStack[TARP_GL_MAX_CLIPPING_STACK_DEPTH];
    int clippingStackDepth;
    int currentClipStencilPlane;
    tpBool bCanSwapStencilPlanes;
    tpTransform transform;
    tpMat4 renderTransform;
    tpMat4 projection;
    tpMat4 transformProjection;
    tpFloat transformScale;
    int transformID;
    int projectionID;
    tpBool bTransformProjDirty;
    tpStyle clippingStyle;

    /* used to temporarily store vertex/stroke data (think double buffering) */
    _tpVec2Array tmpVertices;
    _tpBoolArray tmpJoints;

    /* same helper for texture vertices */
    _tpGLTextureVertexArray tmpTexVertices;

    _tpColorStopArray tmpColorStops;

    _tpGLStateBackup stateBackup;
    char errorMessage[TARP_GL_ERROR_MESSAGE_SIZE];
};

typedef struct TARP_LOCAL
{
    char message[TARP_GL_ERROR_MESSAGE_SIZE];
    int length;
} _ErrorMessage;

TARP_LOCAL void _tpGLGradientCacheDataInit(_tpGLGradientCacheData * _gd, _tpGLRect * _bounds)
{
    _gd->lastGradientID = -1;
    _gd->bounds = _bounds;
    _gd->vertexOffset = 0;
    _gd->vertexCount = 0;
}

TARP_LOCAL tpBool _compileShader(const char * _shaderCode, GLenum _shaderType, GLuint * _outHandle, _ErrorMessage * _outError)
{
    GLenum glHandle;
    GLint state, len, infologLength;

    glHandle = glCreateShader(_shaderType);
    len = (GLint)strlen(_shaderCode);
    _TARP_ASSERT_NO_GL_ERROR(glShaderSource(glHandle, 1, &_shaderCode, &len));
    _TARP_ASSERT_NO_GL_ERROR(glCompileShader(glHandle));

    /* check if the shader compiled */
    _TARP_ASSERT_NO_GL_ERROR(glGetShaderiv(glHandle, GL_COMPILE_STATUS, &state));
    if (state == GL_FALSE)
    {
        if (_outError)
        {
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

TARP_LOCAL tpBool _createProgram(const char * _vertexShader, const char * _fragmentShader, int _bTexProgram, GLuint * _outHandle, _ErrorMessage * _outError)
{
    GLuint vertexShader, fragmentShader, program;
    GLint state, infologLength;
    tpBool err;

    err = _compileShader(_vertexShader, GL_VERTEX_SHADER, &vertexShader, _outError);
    if(err) return err;
    
    err = _compileShader(_fragmentShader, GL_FRAGMENT_SHADER, &fragmentShader, _outError);
    if(err) return err;

    program = glCreateProgram();
    _TARP_ASSERT_NO_GL_ERROR(glAttachShader(program, vertexShader));
    _TARP_ASSERT_NO_GL_ERROR(glAttachShader(program, fragmentShader));

    _TARP_ASSERT_NO_GL_ERROR(glBindAttribLocation(program, 0, "vertex"));
    if (_bTexProgram)
        _TARP_ASSERT_NO_GL_ERROR(glBindAttribLocation(program, 1, "tc"));

    _TARP_ASSERT_NO_GL_ERROR(glLinkProgram(program));

    /* check if we had success */
    _TARP_ASSERT_NO_GL_ERROR(glGetProgramiv(program, GL_LINK_STATUS, &state));

    if (state == GL_FALSE)
    {
        if (_outError)
        {
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

TARP_API tpContext tpContextCreate()
{
    _ErrorMessage msg;
    _tpGLContext * ctx;
    tpBool err;
    tpContext ret = tpContextInvalidHandle();

    ctx = (_tpGLContext *)TARP_MALLOC(sizeof(_tpGLContext));
    assert(ctx);

    err = _createProgram(_vertexShaderCode, _fragmentShaderCode, 0, &ctx->program, &msg);
    if (err)
    {
        strcpy(ctx->errorMessage, msg.message);
        return ret;
    }
    err = _createProgram(_vertexShaderCodeTexture, _fragmentShaderCodeTexture, 1, &ctx->textureProgram, &msg);
    if (err)
    {
        strcpy(ctx->errorMessage, msg.message);
        return ret;
    }

    ctx->tpLoc = glGetUniformLocation(ctx->program, "transformProjection");
    ctx->meshColorLoc = glGetUniformLocation(ctx->program, "meshColor");
    ctx->tpTextureLoc = glGetUniformLocation(ctx->textureProgram, "transformProjection");

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

    ctx->clippingStackDepth = 0;
    ctx->currentClipStencilPlane = _kTpGLClippingStencilPlaneOne;
    ctx->bCanSwapStencilPlanes = tpTrue;
    ctx->transform = tpTransformMakeIdentity();
    ctx->renderTransform = tpMat4MakeIdentity();
    ctx->transformScale = 1.0;
    ctx->transformID = 0;
    ctx->projection = tpMat4MakeIdentity();
    ctx->projectionID = 0;
    ctx->bTransformProjDirty = tpFalse;
    ctx->transformProjection = tpMat4MakeIdentity();

    _tpVec2ArrayInit(&ctx->tmpVertices, 512);
    _tpBoolArrayInit(&ctx->tmpJoints, 256);
    _tpGLTextureVertexArrayInit(&ctx->tmpTexVertices, 64);
    _tpColorStopArrayInit(&ctx->tmpColorStops, 16);

    ctx->clippingStyle = tpStyleCreate();
    tpStyleRemoveStroke(ctx->clippingStyle);

    ret.pointer = ctx;
    return ret;
}

TARP_API const char * tpContextErrorMessage(tpContext _ctx)
{
    _tpGLContext * ctx = (_tpGLContext *)_ctx.pointer;
    return ctx->errorMessage;
}

TARP_API const char * tpImplementationName()
{
    return "OpenGL";
}

TARP_API void tpContextDestroy(tpContext _ctx)
{
    _tpGLContext * ctx = (_tpGLContext *)_ctx.pointer;

    /* free all opengl resources */
    glDeleteProgram(ctx->program);
    glDeleteBuffers(1, &ctx->vao.vbo);
    glDeleteVertexArrays(1, &ctx->vao.vao);
    glDeleteProgram(ctx->textureProgram);
    glDeleteBuffers(1, &ctx->textureVao.vbo);
    glDeleteVertexArrays(1, &ctx->textureVao.vao);

    _tpBoolArrayDeallocate(&ctx->tmpJoints);
    _tpVec2ArrayDeallocate(&ctx->tmpVertices);
    _tpGLTextureVertexArrayDeallocate(&ctx->tmpTexVertices);
    _tpColorStopArrayDeallocate(&ctx->tmpColorStops);

    tpStyleDestroy(ctx->clippingStyle);
    TARP_FREE(ctx);
}

TARP_API tpPath tpPathCreate()
{
    tpPath ret;

    _tpGLPath * path = (_tpGLPath *)TARP_MALLOC(sizeof(_tpGLPath));
    _tpGLContourArrayInit(&path->contours, 4);
    path->currentContourIndex = -1;
    memset(path->errorMessage, 0, sizeof(path->errorMessage));
    path->transform = tpTransformMakeIdentity();

    _tpVec2ArrayInit(&path->geometryCache, 128);
    _tpGLTextureVertexArrayInit(&path->textureGeometryCache, 32);
    _tpBoolArrayInit(&path->jointCache, 128);
    path->bPathGeometryDirty = tpTrue;
    path->lastTransformScale = 1.0;

    path->strokeVertexOffset = 0;
    path->strokeVertexCount = 0;
    path->boundsVertexOffset = 0;

    _tpGLGradientCacheDataInit(&path->fillGradientData, &path->boundsCache);
    _tpGLGradientCacheDataInit(&path->strokeGradientData, &path->strokeBoundsCache);

    path->lastDrawContext = NULL;
    path->lastTransformID = 0;

    path->fillPaintTransform = tpTransformMakeIdentity();
    path->strokePaintTransform = tpTransformMakeIdentity();
    path->bFillPaintTransformDirty = tpFalse;
    path->bStrokePaintTransformDirty = tpFalse;

    ret.pointer = path;
    return ret;
}

TARP_API void tpPathDestroy(tpPath _path)
{
    int i;
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    if (p)
    {
        _tpVec2ArrayDeallocate(&p->geometryCache);
        _tpGLTextureVertexArrayDeallocate(&p->textureGeometryCache);
        _tpBoolArrayDeallocate(&p->jointCache);
        for (i = 0; i < p->contours.count; ++i)
        {
            _tpSegmentArrayDeallocate(&_tpGLContourArrayAtPtr(&p->contours, i)->segments);
        }
        _tpGLContourArrayDeallocate(&p->contours);
        TARP_FREE(p);
    }
}

TARP_LOCAL _tpGLContour * _tpGLCurrentContour(_tpGLPath * _p)
{
    if (_p->currentContourIndex != -1)
        return _tpGLContourArrayAtPtr(&_p->contours, _p->currentContourIndex);
    return NULL;
}

TARP_LOCAL _tpGLContour * _tpGLPathCreateNextContour(_tpGLPath * _p)
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
    contour.bLengthDirty = tpTrue;
    _p->currentContourIndex = _p->contours.count;
    _tpGLContourArrayAppend(&_p->contours, contour);
    return _tpGLContourArrayAtPtr(&_p->contours, _p->currentContourIndex);
}

TARP_LOCAL _tpGLContour * _tpGLPathNextEmptyContour(_tpGLPath * _path)
{
    _tpGLContour * c = _tpGLCurrentContour(_path);
    if (!c || c->segments.count)
    {
        c = _tpGLPathCreateNextContour(_path);
    }
    return c;
}

TARP_LOCAL tpBool _tpGLContourAddSegments(_tpGLPath * _p, _tpGLContour * _c, tpSegment * _segments, int count)
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

TARP_LOCAL tpBool _tpGLContourAddSegment(
    _tpGLPath * _p, _tpGLContour * _c,
    tpFloat _h0x, tpFloat _h0y,
    tpFloat _px, tpFloat _py,
    tpFloat _h1x, tpFloat _h1y)
{
    tpSegment seg = tpSegmentMake(_h0x, _h0y, _px, _py, _h1x, _h1y);
    return _tpGLContourAddSegments(_p, _c, &seg, 1);
}

TARP_API tpBool tpPathAddSegment(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
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

TARP_API tpBool tpPathLineTo(tpPath _path, tpFloat _x, tpFloat _y)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c || c->lastSegmentIndex == -1)
    {
        strcpy(p->errorMessage, "You have to start a contour before issuing this command (see tpPathMoveTo).");
        return tpTrue;
    }

    return _tpGLContourAddSegment(p, c, _x, _y, _x, _y, _x, _y);
}

TARP_API tpBool tpPathMoveTo(tpPath _path, tpFloat _x, tpFloat _y)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    _tpGLContour * c = _tpGLPathNextEmptyContour(p);

    return _tpGLContourAddSegment(p, c, _x, _y, _x, _y, _x, _y);
}

TARP_API tpBool tpPathClear(tpPath _path)
{
    int i;
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    p->currentContourIndex = -1;
    for (i = 0; i < p->contours.count; ++i)
    {
        _tpSegmentArrayDeallocate(&_tpGLContourArrayAtPtr(&p->contours, i)->segments);
    }
    _tpGLContourArrayClear(&p->contours);
    p->bPathGeometryDirty = tpTrue;

    return tpFalse;
}

TARP_API int tpPathContourCount(tpPath _path)
{
    return ((_tpGLPath *)_path.pointer)->contours.count;
}

TARP_API tpBool tpPathCubicCurveTo(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _h1x, tpFloat _h1y, tpFloat _px, tpFloat _py)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c || c->lastSegmentIndex == -1)
    {
        strcpy(p->errorMessage, "You have to start a contour before issuing this command (see tpPathMoveTo).");
        return tpTrue;
    }

    _tpSegmentArrayAtPtr(&c->segments, c->lastSegmentIndex)->handleOut = tpVec2Make(_h0x, _h0y);
    return _tpGLContourAddSegment(p, c, _h1x, _h1y, _px, _py, _px, _py);
}

TARP_API tpBool tpPathQuadraticCurveTo(tpPath _path, tpFloat _hx, tpFloat _hy, tpFloat _px, tpFloat _py)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c || c->lastSegmentIndex == -1)
    {
        strcpy(p->errorMessage, "You have to start a contour before issuing this command (see tpPathMoveTo).");
        return tpTrue;
    }

    _tpSegmentArrayAtPtr(&c->segments, c->lastSegmentIndex)->handleOut = tpVec2Make(_hx, _hy);
    return _tpGLContourAddSegment(p, c, _hx, _hy, _px, _py, _px, _py);
}

TARP_API tpBool tpPathClose(tpPath _path)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
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

TARP_API tpBool tpPathRemoveContour(tpPath _path, int _index)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    _tpGLContour * c = _tpGLContourArrayAtPtr(&p->contours, _index);
    _tpSegmentArrayDeallocate(&c->segments);
    _tpGLContourArrayRemove(&p->contours, _index);
    p->bPathGeometryDirty = tpTrue;
    p->currentContourIndex = p->contours.count - 1;
    return tpFalse;
}

TARP_API tpBool tpPathRemoveSegment(tpPath _path, int _contourIndex, int _index)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    _tpGLContour * c = _tpGLContourArrayAtPtr(&p->contours, _contourIndex);
    _tpSegmentArrayRemove(&c->segments, _index);
    p->bPathGeometryDirty = tpTrue;
    c->bDirty = tpTrue;
    return tpFalse;
}

TARP_API tpBool tpPathRemoveSegments(tpPath _path, int _contourIndex, int _from, int _to)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    _tpGLContour * c = _tpGLContourArrayAtPtr(&p->contours, _contourIndex);
    _tpSegmentArrayRemoveRange(&c->segments, _from, _to);
    p->bPathGeometryDirty = tpTrue;
    c->bDirty = tpTrue;
    return tpFalse;
}

TARP_LOCAL tpBool _tpGLPathAddSegmentsToCurrentContour(_tpGLPath * _p, _tpGLContour * _c, tpSegment * _segments, int _count)
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

TARP_API tpBool tpPathAddSegments(tpPath _path, tpSegment * _segments, int _count)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c)
    {
        c = _tpGLPathCreateNextContour(p);
        assert(c);
    }

    return _tpGLPathAddSegmentsToCurrentContour(p, c, _segments, _count);
}

TARP_API tpBool tpPathAddContour(tpPath _path, tpSegment * _segments, int _count, tpBool _bClosed)
{
    tpBool ret;
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    _tpGLContour * c = _tpGLPathCreateNextContour(p);
    assert(c);

    ret = _tpGLPathAddSegmentsToCurrentContour(p, c, _segments, _count);
    if (ret) return ret;

    if (_bClosed)
        return tpPathClose(_path);
    return tpFalse;
}

TARP_API tpBool tpPathSetContour(tpPath _path, int _contourIndex, tpSegment * _segments, int _count, tpBool _bClosed)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    if (_contourIndex < p->contours.count)
    {
        _tpGLContour * c = _tpGLContourArrayAtPtr(&p->contours, _contourIndex);
        _tpSegmentArrayClear(&c->segments);
        _tpSegmentArrayAppendArray(&c->segments, _segments, _count);
        c->lastSegmentIndex = c->segments.count - 1;
        c->bIsClosed = tpTrue;
        c->bDirty = tpTrue;
        p->bPathGeometryDirty = tpTrue;
        return tpFalse;
    }
    else
        return tpPathAddContour(_path, _segments, _count, _bClosed);
}

TARP_API tpBool tpPathAddCircle(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _r)
{
    tpFloat dr = _r * 2.0;
    return tpPathAddEllipse(_path, _x, _y, dr, dr);
}

TARP_API tpBool tpPathAddEllipse(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height)
{
    static tpVec2 s_unitSegments[12] =
    {
        {1, 0}, {0, -TARP_KAPPA}, {0, TARP_KAPPA},
        {0, 1}, {TARP_KAPPA, 0}, { -TARP_KAPPA, 0},
        { -1, 0}, {0, TARP_KAPPA}, {0, -TARP_KAPPA},
        {0, -1}, { -TARP_KAPPA, 0}, {TARP_KAPPA, 0}
    };

    int i;
    tpFloat rw, rh, px, py;
    tpSegment segs[4];
    tpBool err;

    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    _tpGLContour * c = _tpGLPathNextEmptyContour(p);

    rw = _width * 0.5;
    rh = _height * 0.5;
    for (i = 0; i < 4; ++i)
    {
        int kappaIdx = i * 3;

        px = s_unitSegments[kappaIdx].x * rw + _x;
        py = s_unitSegments[kappaIdx].y * rh + _y;
        segs[i] = tpSegmentMake(s_unitSegments[kappaIdx + 1].x * rw + px, s_unitSegments[kappaIdx + 1].y * rh + py,
                                px, py,
                                s_unitSegments[kappaIdx + 2].x * rw + px, s_unitSegments[kappaIdx + 2].y * rh + py);
    }

    err = _tpGLContourAddSegments(p, c, segs, 4);
    if (err) return err;

    tpPathClose(_path);

    return tpFalse;
}

TARP_API tpBool tpPathAddRect(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height)
{
    tpSegment segs[4];
    tpFloat tmpa, tmpb;
    tpBool err;

    _tpGLPath * p = (_tpGLPath *)_path.pointer;

    _tpGLContour * c = _tpGLPathNextEmptyContour(p);

    tmpa = _x + _width;
    tmpb = _y + _height;
    segs[0] = tpSegmentMake(_x, _y, _x, _y, _x, _y);
    segs[1] = tpSegmentMake(tmpa, _y, tmpa, _y, tmpa, _y);
    segs[2] = tpSegmentMake(tmpa, tmpb, tmpa, tmpb, tmpa, tmpb);
    segs[3] = tpSegmentMake(_x, tmpb, _x, tmpb, _x, tmpb);

    err = _tpGLContourAddSegments(p, c, segs, 4);
    if (err) return err;

    tpPathClose(_path);

    return tpFalse;
}

TARP_LOCAL void _tpGLMarkPathGeometryDirty(_tpGLPath * _p)
{
    int i;
    _tpGLContour * c;
    _p->bPathGeometryDirty = tpTrue;
    for (i = 0; i < _p->contours.count; ++i)
    {
        c = _tpGLContourArrayAtPtr(&_p->contours, i);
        c->bDirty = tpTrue;
        c->bLengthDirty = tpTrue;
    }
}

TARP_API tpBool tpPathSetFillPaintTransform(tpPath _path, const tpTransform * _transform)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    p->fillPaintTransform = *_transform;
    p->bFillPaintTransformDirty = tpTrue;
    return tpFalse;
}

TARP_API tpBool tpPathSetStrokePaintTransform(tpPath _path, const tpTransform * _transform)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    p->strokePaintTransform = *_transform;
    p->bStrokePaintTransformDirty = tpTrue;
    return tpFalse;
}

TARP_API tpStyle tpStyleCreate()
{
    tpStyle ret;
    _tpGLStyle * style = (_tpGLStyle *)TARP_MALLOC(sizeof(_tpGLStyle));

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

    ret.pointer = style;
    return ret;
}

TARP_API void tpStyleDestroy(tpStyle _style)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    TARP_FREE(s);
}

TARP_API void tpStyleSetDashArray(tpStyle _style, const tpFloat * _dashArray, int _count)
{
    int i;
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;

    assert(_count < TARP_MAX_DASH_ARRAY_SIZE);

    for (i = 0; i < _count; ++i)
    {
        s->dashArray[i] = _dashArray[i];
    }
    s->dashCount = _count;
}

TARP_API void tpStyleSetDashOffset(tpStyle _style, tpFloat _offset)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    s->dashOffset = _offset;
}

TARP_API void tpStyleSetFillColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;

    s->fill.data.color = tpColorMake(_r, _g, _b, _a);
    s->fill.type = kTpPaintTypeColor;
}

TARP_API void tpStyleSetStrokeColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    s->stroke.data.color = tpColorMake(_r, _g, _b, _a);
    s->stroke.type = kTpPaintTypeColor;
}

TARP_API void tpStyleSetFillGradient(tpStyle _style, const tpGradient _gradient)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    s->fill.data.gradient = _gradient;
    s->fill.type = kTpPaintTypeGradient;
}

TARP_API void tpStyleSetStrokeGradient(tpStyle _style, const tpGradient _gradient)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    s->stroke.data.gradient = _gradient;
    s->stroke.type = kTpPaintTypeGradient;
}

TARP_API void tpStyleSetStrokeWidth(tpStyle _style, tpFloat _strokeWidth)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    s->strokeWidth = _strokeWidth;
}

TARP_API void tpStyleSetStrokeJoin(tpStyle _style, tpStrokeJoin _join)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    s->strokeJoin = _join;
}

TARP_API void tpStyleSetMiterLimit(tpStyle _style, tpFloat _limit)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    s->miterLimit = _limit;
}

TARP_API void tpStyleSetStrokeCap(tpStyle _style, tpStrokeCap _cap)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    s->strokeCap = _cap;
}

TARP_API void tpStyleSetScaleStroke(tpStyle _style, tpBool _b)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    s->bScaleStroke = _b;
}

TARP_API void tpStyleSetFillRule(tpStyle _style, tpFillRule _rule)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    s->fillRule = _rule;
}

TARP_API void tpStyleRemoveFill(tpStyle _style)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    s->fill.type = kTpPaintTypeNone;
}

TARP_API void tpStyleRemoveStroke(tpStyle _style)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    s->stroke.type = kTpPaintTypeNone;
}

TARP_API tpFloat * tpStyleDashArrayPtr(tpStyle _style)
{
    return ((_tpGLStyle *)_style.pointer)->dashArray;
}

TARP_API int tpStyleDashCount(tpStyle _style)
{
    return ((_tpGLStyle *)_style.pointer)->dashCount;
}

TARP_API tpFloat tpStyleDashOffset(tpStyle _style)
{
    return ((_tpGLStyle *)_style.pointer)->dashOffset;
}

TARP_API tpPaint tpStyleFillPaint(tpStyle _style)
{
    return ((_tpGLStyle *)_style.pointer)->fill;
}

TARP_API tpPaint tpStyleStrokePaint(tpStyle _style)
{
    return ((_tpGLStyle *)_style.pointer)->stroke;
}

TARP_API tpFloat tpStyleStrokeWidth(tpStyle _style)
{
    return ((_tpGLStyle *)_style.pointer)->strokeWidth;
}

TARP_API tpStrokeJoin tpStyleStrokeJoin(tpStyle _style)
{
    return ((_tpGLStyle *)_style.pointer)->strokeJoin;
}

TARP_API tpBool tpStyleScaleStroke(tpStyle _style)
{
    return ((_tpGLStyle *)_style.pointer)->bScaleStroke;
}

TARP_API tpFloat tpStyleMiterLimit(tpStyle _style)
{
    return ((_tpGLStyle *)_style.pointer)->miterLimit;
}

TARP_API tpStrokeCap tpStyleStrokeCap(tpStyle _style)
{
    return ((_tpGLStyle *)_style.pointer)->strokeCap;
}

TARP_API tpFillRule tpStyleFillRule(tpStyle _style)
{
    return ((_tpGLStyle *)_style.pointer)->fillRule;
}

TARP_LOCAL _tpGLGradient * tpGradientCreate(){
    static int s_id = 0;

    _tpGLGradient * ret = (_tpGLGradient *)TARP_MALLOC(sizeof(_tpGLGradient));
    _tpColorStopArrayInit(&ret->stops, 8);
    ret->bDirty = tpTrue;
    /*
    the static id and incrementing is not multi threadding friendly...no care for now
    thread local storage will most likely be the nicest way to make this thread safe
    */
    ret->gradientID = s_id++;

    _TARP_ASSERT_NO_GL_ERROR(glGenTextures(1, &ret->rampTexture));
    _TARP_ASSERT_NO_GL_ERROR(glBindTexture(GL_TEXTURE_1D, ret->rampTexture));
    _TARP_ASSERT_NO_GL_ERROR(glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, TARP_GL_RAMP_TEXTURE_SIZE, 0,
                                          GL_RGBA, GL_FLOAT, NULL));
    _TARP_ASSERT_NO_GL_ERROR(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    _TARP_ASSERT_NO_GL_ERROR(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    _TARP_ASSERT_NO_GL_ERROR(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    _TARP_ASSERT_NO_GL_ERROR(glBindTexture(GL_TEXTURE_1D, 0));

    return ret;
}

TARP_API tpGradient tpGradientCreateLinear(tpFloat _x0, tpFloat _y0, tpFloat _x1, tpFloat _y1)
{
    tpGradient rh;
    _tpGLGradient * ret = tpGradientCreate();
    ret->type = kTpGradientTypeLinear;
    ret->origin = tpVec2Make(_x0, _y0);
    ret->destination = tpVec2Make(_x1, _y1);
    rh.pointer = ret;
    return rh;
}

TARP_API tpGradient tpGradientCreateRadial(tpFloat _fx, tpFloat _fy, tpFloat _ox, tpFloat _oy, tpFloat _dx, tpFloat _dy, tpFloat _ratio)
{
    tpGradient rh;
    _tpGLGradient * ret = tpGradientCreate();
    ret->type = kTpGradientTypeRadial;
    ret->origin = tpVec2Make(_ox, _oy);
    ret->destination = tpVec2Make(_dx, _dy);
    ret->ratio = _ratio;
    ret->focal_point_offset = tpVec2Make(_fx, _fy);
    rh.pointer = ret;
    return rh;
}

TARP_API tpGradient tpGradientCreateRadialSymmetric(tpFloat _x, tpFloat _y, tpFloat _radius)
{
    return tpGradientCreateRadial(0, 0, _x, _y, _radius, 0, 1);
}

TARP_API void tpGradientSetPositions(tpGradient _gradient, tpFloat _x0, tpFloat _y0, tpFloat _x1, tpFloat _y1)
{
    _tpGLGradient * g = (_tpGLGradient *)_gradient.pointer;
    g->origin = tpVec2Make(_x0, _y0);
    g->destination = tpVec2Make(_x1, _y1);
    g->bDirty = tpTrue;
}

TARP_API void tpGradientSetFocalPointOffset(tpGradient _gradient, tpFloat _x, tpFloat _y)
{
    _tpGLGradient * g = (_tpGLGradient *)_gradient.pointer;
    g->focal_point_offset = tpVec2Make(_x, _y);
    g->bDirty = tpTrue;
}

TARP_API void tpGradientSetRatio(tpGradient _gradient, tpFloat _ratio)
{
    _tpGLGradient * g = (_tpGLGradient *)_gradient.pointer;
    g->ratio = _ratio;
    g->bDirty = tpTrue;
}

TARP_API void tpGradientAddColorStop(tpGradient _gradient, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a, tpFloat _offset)
{
    _tpGLGradient * g = (_tpGLGradient *)_gradient.pointer;
    tpColorStop stop;
    stop.color = tpColorMake(_r, _g, _b, _a);
    stop.offset = _offset;
    _tpColorStopArrayAppendPtr(&g->stops, &stop);
    g->bDirty = tpTrue;
}

TARP_API void tpGradientClearColorStops(tpGradient _gradient)
{
    _tpGLGradient * g = (_tpGLGradient *)_gradient.pointer;
    _tpColorStopArrayClear(&g->stops);
    g->bDirty = tpTrue;
}

TARP_API void tpGradientDestroy(tpGradient _gradient)
{
    _tpGLGradient * g = (_tpGLGradient *)_gradient.pointer;
    if (g)
    {
        _TARP_ASSERT_NO_GL_ERROR(glDeleteTextures(1, &g->rampTexture));
        _tpColorStopArrayDeallocate(&g->stops);
        TARP_FREE(g);
    }
}

TARP_LOCAL int _tpGLIsClose(tpFloat _x, tpFloat _y, tpFloat _epsilon)
{
    tpFloat maxXYOne = TARP_MAX(1.0, TARP_MAX(fabs(_x), fabs(_y)));
    return fabs(_x - _y) <= _epsilon * maxXYOne;
}

TARP_LOCAL int _tpGLIsCurveLinear(const _tpGLCurve * _curve)
{
    return _tpGLIsClose(_curve->p0.x, _curve->h0.x, FLT_EPSILON) && _tpGLIsClose(_curve->p0.y, _curve->h0.y, FLT_EPSILON) &&
           _tpGLIsClose(_curve->p1.x, _curve->h1.x, FLT_EPSILON) && _tpGLIsClose(_curve->p1.y, _curve->h1.y, FLT_EPSILON);
}

TARP_LOCAL int _tpGLIsCurveFlatEnough(const _tpGLCurve * _curve, tpFloat _tolerance)
{
    tpFloat ux, uy, vx, vy;
    if (_tpGLIsCurveLinear(_curve))
        return 1;

    /*
    Comment from paper.js source in Curve.js:
    Thanks to Kaspar Fischer and Roger Willcocks for the following:
    http://hcklbrrfnn.files.wordpress.com/2012/08/bez.pdf
    */
    ux = _curve->h0.x * 3.0 - _curve->p0.x * 2.0 - _curve->p1.x;
    uy = _curve->h0.y * 3.0 - _curve->p0.y * 2.0 - _curve->p1.y;
    vx = _curve->h1.x * 3.0 - _curve->p1.x * 2.0 - _curve->p0.x;
    vy = _curve->h1.y * 3.0 - _curve->p1.y * 2.0 - _curve->p0.y;

    return TARP_MAX(ux * ux, vx * vx) + TARP_MAX(uy * uy, vy * vy) < 10 * _tolerance * _tolerance;
}

TARP_LOCAL void _tpGLSubdivideCurve(const _tpGLCurve * _curve,
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

    _result->first.p0 = _curve->p0;
    _result->first.h0 = tpVec2Make(v1x, v1y);
    _result->first.h1 = tpVec2Make(v4x, v4y);
    _result->first.p1 = tpVec2Make(v6x, v6y);

    _result->second.p0 = tpVec2Make(v6x, v6y);
    _result->second.h0 = tpVec2Make(v5x, v5y);
    _result->second.h1 = tpVec2Make(v3x, v3y);
    _result->second.p1 = _curve->p1;
}

TARP_LOCAL void _tpGLEvaluatePointForBounds(tpVec2 _vec, _tpGLRect * _bounds)
{
    _bounds->min.x = TARP_MIN(_vec.x, _bounds->min.x);
    _bounds->min.y = TARP_MIN(_vec.y, _bounds->min.y);
    _bounds->max.x = TARP_MAX(_vec.x, _bounds->max.x);
    _bounds->max.y = TARP_MAX(_vec.y, _bounds->max.y);
}

tpFloat _tpGLShortestAngle(tpVec2 _d0, tpVec2 _d1)
{
    tpFloat theta = acos(_d0.x * _d1.x + _d0.y * _d1.y);

    /* make sure we have the shortest angle */
    if (theta > TARP_HALF_PI)
        theta = TARP_PI - theta;

    return theta;
}

TARP_LOCAL void _tpGLPushTriangle(_tpVec2Array * _vertices, tpVec2 _a, tpVec2 _b, tpVec2 _c)
{
    _tpVec2ArrayAppendPtr(_vertices, &_a);
    _tpVec2ArrayAppendPtr(_vertices, &_b);
    _tpVec2ArrayAppendPtr(_vertices, &_c);
}

TARP_LOCAL void _tpGLPushQuad(_tpVec2Array * _vertices, tpVec2 _a, tpVec2 _b, tpVec2 _c, tpVec2 _d)
{
    _tpVec2ArrayAppendPtr(_vertices, &_a);
    _tpVec2ArrayAppendPtr(_vertices, &_b);
    _tpVec2ArrayAppendPtr(_vertices, &_c);
    _tpVec2ArrayAppendPtr(_vertices, &_c);
    _tpVec2ArrayAppendPtr(_vertices, &_d);
    _tpVec2ArrayAppendPtr(_vertices, &_a);
}

TARP_LOCAL void _tpGLMakeCircleSector(tpVec2 _center, tpVec2 _r0, tpVec2 _r1, tpFloat _levelOfDetail, _tpVec2Array * _outVertices)
{
    tpMat2 rot;
    tpVec2 r, current, last;
    tpFloat stepSize;
    
    /* @TODO: The step size should depend on _levelOfDetail. */
    stepSize = TARP_PI / 16;
    rot = tpMat2MakeRotation(stepSize);
    r = _r0;
    last = tpVec2Add(_center, r);
    for(;;)
    {
        r = tpMat2MultVec2(&rot, r);
        if(tpVec2Cross(r, _r1) < 0) { break; }
        current = tpVec2Add(_center, r);
        _tpGLPushTriangle(_outVertices, _center, last, current);
        last = current;
    }
    current = tpVec2Add(_center, _r1);
    _tpGLPushTriangle(_outVertices, _center, last, current);
}

TARP_LOCAL void _tpGLMakeJoinBevel(tpVec2 _lePrev, tpVec2 _rePrev,
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

TARP_LOCAL tpBool _tpGLIntersectLines(tpVec2 _p0, tpVec2 _d0,
                                      tpVec2 _p1, tpVec2 _d1,
                                      tpVec2 * _outResult)
{
    tpFloat cross, t;
    tpVec2 delta, doff;

    delta = tpVec2Sub(_p0, _p1);
    cross = tpVec2Cross(_d0, _d1);

    /* lines are parallel */
    if (cross == 0.0)
        return tpFalse;

    t = tpVec2Cross(_d1, delta) / cross;
    doff = tpVec2MultScalar(_d0, t);
    *_outResult = tpVec2Add(_p0, doff);

    return tpTrue;
}

TARP_LOCAL void _tpGLMakeJoinMiter(tpVec2 _p,
                                   tpVec2 _e0, tpVec2 _e1,
                                   tpVec2 _dir0, tpVec2 _dir1,
                                   tpFloat _cross,
                                   _tpVec2Array * _outVertices)
{
    tpVec2 intersection;

    /* tpVec2 inv = tpVec2MultScalar(_dir1, -1); */
    _tpGLIntersectLines(_e0, _dir0, _e1, _dir1, &intersection);

    _tpGLPushQuad(_outVertices, _p, _e0, intersection, _e1);
}

TARP_LOCAL void _tpGLMakeJoin(tpStrokeJoin _type,
                              tpVec2 _p,
                              tpVec2 _dir0, tpVec2 _dir1,
                              tpVec2 _perp0, tpVec2 _perp1,
                              tpVec2 _lePrev, tpVec2 _rePrev,
                              tpVec2 _le, tpVec2 _re,
                              tpFloat _cross, tpFloat _miterLimit,
                              _tpVec2Array * _outVertices)
{
    tpVec2 nperp0, nperp1;
    tpFloat miterLen, theta, levelOfDetail;

    switch (_type)
    {
        case kTpStrokeJoinRound:
            levelOfDetail = 0; /* TODO: levelOfDetail depend on the stroke width and the transform */
            if (_cross < 0.0f)
            {
                _tpGLMakeCircleSector(_p, _perp0, _perp1, levelOfDetail, _outVertices);
            }
            else
            {
                /* negate the perpendicular vectors and reorder function arguments */
                tpVec2 flippedPerp0, flippedPerp1;
                flippedPerp0 = tpVec2Make(-_perp0.x, -_perp0.y);
                flippedPerp1 = tpVec2Make(-_perp1.x, -_perp1.y);
                _tpGLMakeCircleSector(_p, flippedPerp0, flippedPerp1, levelOfDetail, _outVertices);
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
                    _tpGLMakeJoinMiter(_p, _lePrev, _le, _dir0, _dir1, _cross, _outVertices);
                }
                else
                {
                    _tpGLMakeJoinMiter(_p, _rePrev, _re, _dir0, _dir1, _cross, _outVertices);
                }
                break;
            }
        /* fall back to bevel */
        case kTpStrokeJoinBevel:
        default:
            _tpGLMakeJoinBevel(_lePrev, _rePrev, _le, _re, _cross, _outVertices);
            break;
    }
}

TARP_LOCAL void _tpGLMakeCapSquare(tpVec2 _p,
                                   tpVec2 _dir,
                                   tpVec2 _le, tpVec2 _re,
                                   _tpVec2Array * _outVertices)
{
    tpVec2 a, b;
    a = tpVec2Add(_re, _dir);
    b = tpVec2Add(_le, _dir);

    _tpGLPushQuad(_outVertices, _re, a, b, _le);
}

TARP_LOCAL void _tpGLMakeCap(tpStrokeCap _type,
                             tpVec2 _p,
                             tpVec2 _dir,
                             tpVec2 _perp,
                             tpVec2 _le, tpVec2 _re,
                             tpBool _bStart,
                             _tpVec2Array * _outVertices)
{
    tpFloat levelOfDetail;
    tpVec2 flippedPerp;
    switch (_type)
    {
        case kTpStrokeCapRound:
            /* TODO: levelOfDetail should depend on the stroke width and the transform */
            levelOfDetail = 0;
            flippedPerp = tpVec2Make(-_perp.x, -_perp.y);
            _tpGLMakeCircleSector(_p, _perp, flippedPerp, levelOfDetail, _outVertices);
            break;
        case kTpStrokeCapSquare:
            _tpGLMakeCapSquare(_p, _dir, _le, _re, _outVertices);
            break;
        case kTpStrokeCapButt:
        default:
            break;
    }
}

TARP_LOCAL void _tpGLContinousStrokeGeometry(_tpGLPath * _path, const _tpGLStyle * _style, _tpVec2Array * _vertices, _tpBoolArray * _joints)
{
    int i, j, voff;
    _tpGLContour * c;
    tpVec2 p0, p1, dir, perp, dirPrev, perpPrev;
    tpVec2 le0, le1, re0, re1, lePrev, rePrev;
    tpVec2 firstDir, firstPerp, firstLe, firstRe;
    tpFloat cross, halfSw;

    assert(_vertices->count == _joints->count);

    halfSw = _style->strokeWidth * 0.5;

    /* generate the stroke geometry for each contour */
    for (i = 0; i < _path->contours.count; ++i)
    {
        voff = _vertices->count;
        c = _tpGLContourArrayAtPtr(&_path->contours, i);
        c->strokeVertexOffset = voff;
        if (c->fillVertexCount <= 1)
        {
            c->strokeVertexCount = 0;
            continue;
        }

        for (j = c->fillVertexOffset; j < c->fillVertexOffset + c->fillVertexCount - 1; ++j)
        {
            p0 = _tpVec2ArrayAt(_vertices, j);
            p1 = _tpVec2ArrayAt(_vertices, j + 1);
            dir = tpVec2Sub(p1, p0);
            tpVec2NormalizeSelf(&dir);
            perp.x = dir.y * halfSw;
            perp.y = -dir.x * halfSw;

            le0 = tpVec2Add(p0, perp);
            re0 = tpVec2Sub(p0, perp);
            le1 = tpVec2Add(p1, perp);
            re1 = tpVec2Sub(p1, perp);

            /* check if this is the first segment / dash start */
            if (j == c->fillVertexOffset)
            {
                if (!c->bIsClosed)
                {
                    /* start cap? */
                    firstDir = tpVec2MultScalar(dir, -1 * halfSw);
                    firstPerp.x = firstDir.y;
                    firstPerp.y = -firstDir.x;
                    _tpGLMakeCap(_style->strokeCap, p0, firstDir, firstPerp, le0, re0, tpTrue, _vertices);
                }
                else if (j == c->fillVertexOffset)
                {
                    /*
                    if the contour is closed, we cache the directions and edges
                    to be used for the last segments stroke calculations
                    */
                    firstDir = dir;
                    firstPerp = perp;
                    firstLe = le0;
                    firstRe = re0;
                }
            }
            else
            {
                cross = tpVec2Cross(perp, perpPrev);
                /* check if this is a joint */
                if (_tpBoolArrayAt(_joints, j))
                {
                    _tpGLMakeJoin(_style->strokeJoin, p0,
                                  dirPrev, dir,
                                  perpPrev, perp,
                                  lePrev, rePrev, le0, re0,
                                  cross, _style->miterLimit, _vertices);
                }
                else
                {
                    /* by default we join consecutive segment quads with a bevel */
                    _tpGLMakeJoinBevel(lePrev, rePrev, le0, re0, cross, _vertices);
                }
            }

            /* add the quad for the current segment */
            _tpGLPushQuad(_vertices, le1, le0, re0, re1);

            /* check if we need to do the end cap / join */
            if (j == c->fillVertexOffset + c->fillVertexCount - 2 ||
                    c->fillVertexCount == 2)
            {
                if (_tpBoolArrayAt(_joints, j + 1) && c->bIsClosed)
                {
                    /* last join */
                    cross = tpVec2Cross(firstPerp, perp);
                    _tpGLMakeJoin(_style->strokeJoin, p1, dir, firstDir,
                                  perp, firstPerp,
                                  le1, re1, firstLe, firstRe, cross,
                                  _style->miterLimit, _vertices);
                }
                else
                {
                    /* end cap */
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
    }
}

TARP_LOCAL void _tpGLDashedStrokeGeometry(_tpGLPath * _path, const _tpGLStyle * _style, _tpVec2Array * _vertices, _tpBoolArray * _joints)
{
    int j, i, dashIndex, voff, startDashIndex;
    tpVec2 p0, p1, dir, perp, dirPrev, perpPrev;
    tpVec2 le0, le1, re0, re1, lePrev, rePrev;
    tpVec2 firstDir, firstPerp, firstLe, firstRe;
    tpVec2 dirr;
    tpFloat cross, halfSw, dashOffset, dashLen, segmentOff, segmentLen;
    tpFloat patternLen, offsetIntoPattern;
    tpBool bOnDash, bDashStart, bFirstDashMightNeedJoin, bLastSegment;
    tpBool bStartDashOn, bBarelyJoined;
    tpFloat startDashLen;
    _tpGLContour * c;

    halfSw = _style->strokeWidth * 0.5;

    /* no dash offset */
    if (_style->dashOffset == 0)
    {
        startDashIndex = 0;
        bStartDashOn = tpTrue;
        startDashLen = _style->dashArray[startDashIndex];
    }
    else
    {
        /* compute offset into dash pattern */
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
            bStartDashOn = (tpBool)!(startDashIndex % 2);
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
            bStartDashOn = (tpBool)!(startDashIndex % 2);
            startDashLen = _style->dashArray[startDashIndex] + startDashLen;
        }
    }

    /* generate the stroke geometry for each contour */
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

        for (j = c->fillVertexOffset; j < c->fillVertexOffset + c->fillVertexCount - 1; ++j)
        {
            bLastSegment = (tpBool)(j == c->fillVertexOffset + c->fillVertexCount - 2);

            p0 = _tpVec2ArrayAt(_vertices, j);
            p1 = _tpVec2ArrayAt(_vertices, j + 1);
            dir = tpVec2Sub(p1, p0);
            segmentLen = tpVec2Length(dir);
            segmentOff = 0;
            dir.x /= segmentLen; dir.y /= segmentLen;

            perp.x = dir.y * halfSw;
            perp.y = -dir.x * halfSw;
            cross = tpVec2Cross(perp, perpPrev);

            /* check if this is a joint */
            if (bOnDash && _tpBoolArrayAt(_joints, j))
            {
                le0 = tpVec2Add(p0, perp);
                re0 = tpVec2Sub(p0, perp);
                _tpGLMakeJoin(_style->strokeJoin, p0,
                              dirPrev, dir,
                              perpPrev, perp,
                              lePrev, rePrev, le0, re0,
                              cross, _style->miterLimit, _vertices);
            }

            do
            {
                tpFloat left = TARP_MIN((segmentLen - segmentOff), dashLen - dashOffset);
                dirr = tpVec2MultScalar(dir, left);
                p1 = tpVec2Add(p0, dirr);

                le0 = tpVec2Add(p0, perp);
                re0 = tpVec2Sub(p0, perp);
                le1 = tpVec2Add(p1, perp);
                re1 = tpVec2Sub(p1, perp);

                if (bDashStart && bOnDash)
                {
                    bDashStart = tpFalse;

                    /*
                    if the current contour is not closed, or we are not at the beginning
                    of the first segment of the contour, add a starting cap to the dash...
                    */
                    if (!c->bIsClosed || j != c->fillVertexOffset || segmentOff > 0)
                    {
                        tpVec2 tmpDir, tmpPerp;
                        tmpDir = tpVec2MultScalar(dir, -1 * halfSw);
                        tmpPerp.x = tmpDir.y;
                        tmpPerp.y = -tmpDir.x;
                        _tpGLMakeCap(_style->strokeCap, p0, tmpDir, tmpPerp, le0, re0, tpTrue, _vertices);
                    }
                    /*
                    ...otherwise cache the initial values for the cap computation and mark that the contour
                    might need a closing join in case the first and last dash of the contour touch.
                    */
                    else if (j == c->fillVertexOffset)
                    {
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
                    /* add the quad for the current dash on the current segment */
                    _tpGLPushQuad(_vertices, le1, le0, re0, re1);
                }

                dashOffset += left;
                segmentOff += left;
                p0 = p1;

                if (dashOffset >= dashLen)
                {
                    if (bOnDash)
                    {
                        /* dont make cap if the first and last dash of the contour touch and the last dash is not finished. */
                        if (!bFirstDashMightNeedJoin || !bLastSegment || segmentLen - segmentOff > 0)
                        {
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
                    bOnDash = (tpBool)!bOnDash;
                }
            }
            while ((segmentLen - segmentOff) > 0);

            if (bLastSegment)
            {
                /* if the first and last dash of the contour touch, we connect them with a join */
                if (bFirstDashMightNeedJoin)
                {
                    if ((bBarelyJoined || (dashOffset > 0 && bOnDash)))
                    {
                        cross = tpVec2Cross(firstPerp, perp);
                        _tpGLMakeJoin(_style->strokeJoin, p1, dir, firstDir,
                                      perp, firstPerp,
                                      le1, re1, firstLe, firstRe, cross,
                                      _style->miterLimit, _vertices);
                    }
                    else
                    {
                        /* otherwise we simply add a starting cap to the first dash of the contour... */
                        tpVec2 tmpDir, tmpPerp;
                        tmpDir = tpVec2MultScalar(firstDir, -1 * halfSw);
                        tmpPerp.x = tmpDir.y;
                        tmpPerp.y = -tmpDir.x;
                        _tpGLMakeCap(_style->strokeCap, p1, tmpDir, tmpPerp, firstRe, firstLe, tpFalse, _vertices);

                    }
                }
                else if (dashOffset > 0 && bOnDash)
                {
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
    }
}

TARP_LOCAL void _tpGLStroke(_tpGLPath * _path, const _tpGLStyle * _style, _tpVec2Array * _vertices, _tpBoolArray * _joints)
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

    _path->strokeVertexOffset = _tpGLContourArrayAtPtr(&_path->contours, 0)->strokeVertexOffset;

    /* cache with what settings the stroke geometry was generated */
    _path->lastStroke.strokeType = _style->stroke.type;
    _path->lastStroke.strokeWidth = _style->strokeWidth;
    _path->lastStroke.dashOffset = _style->dashOffset;
    memcpy(_path->lastStroke.dashArray, _style->dashArray, sizeof(tpFloat) * _style->dashCount);
    _path->lastStroke.dashCount = _style->dashCount;
    _path->lastStroke.join = _style->strokeJoin;
    _path->lastStroke.cap = _style->strokeCap;
    _path->lastStroke.bScaleStroke = _style->bScaleStroke;
}

TARP_LOCAL void _tpGLFlattenCurve(_tpGLPath * _path,
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
            /*
            subdivide curve and add continue with the left of the two curves by putting it on top
            of the stack.
            */
            _tpGLSubdivideCurve(current, 0.5, &cp);
            *current = cp.second;
            stack[++stackIndex] = cp.first;
        }
        else
        {
            /* for the first curve we also add its first segment */
            if (_bFirstCurve)
            {
                _tpVec2ArrayAppendPtr(_outVertices, &current->p0);
                _tpVec2ArrayAppendPtr(_outVertices, &current->p1);

                _tpBoolArrayAppend(_outJoints, tpFalse);
                _tpBoolArrayAppend(_outJoints, (tpBool)(tpVec2Equals(current->p1, _curve->p1) && !_bLastCurve));
                _tpGLEvaluatePointForBounds(current->p0, _bounds);
                _tpGLEvaluatePointForBounds(current->p1, _bounds);
                *_vertexCount += 2;

                /* we don't want to do this for the following subdivisions */
                _bFirstCurve = 0;
            }
            else
            {
                _tpVec2ArrayAppendPtr(_outVertices, &current->p1);

                _tpBoolArrayAppend(_outJoints, (tpBool)(_bIsClosed ? tpVec2Equals(current->p1, _curve->p1) :
                                                        (tpVec2Equals(current->p1, _curve->p1) && !_bLastCurve)));
                _tpGLEvaluatePointForBounds(current->p1, _bounds);
                (*_vertexCount)++;
            }
            stackIndex--;
        }
    }
}

TARP_LOCAL void _tpGLInitBounds(_tpGLRect * _bounds)
{
    _bounds->min.x = FLT_MAX;
    _bounds->min.y = FLT_MAX;
    _bounds->max.x = -FLT_MAX;
    _bounds->max.y = -FLT_MAX;
}

TARP_LOCAL void _tpGLMergeBounds(_tpGLRect * _a, const _tpGLRect * _b)
{
    _tpGLEvaluatePointForBounds(_b->min, _a);
    _tpGLEvaluatePointForBounds(_b->max, _a);
}

TARP_LOCAL int _tpGLFlattenPath(_tpGLPath * _path,
                                tpFloat _angleTolerance,
                                const tpTransform * _transform,
                                _tpVec2Array * _outVertices,
                                _tpBoolArray * _outJoints,
                                _tpGLRect * _outBounds)
{
    /* @TODO: clean up dis mess */
    _tpGLRect contourBounds;
    int i = 0;
    int j = 0;
    int vcount;
    int off = 0;
    _tpGLContour * c = NULL;
    tpSegment * last = NULL, *current = NULL;
    /* int recursionDepth = 0; */
    _tpGLCurve curve;
    tpVec2 lastTransformedPos;

    _tpGLInitBounds(_outBounds);

    for (i = 0; i < _path->contours.count; ++i)
    {
        c = _tpGLContourArrayAtPtr(&_path->contours, i);
        _tpGLInitBounds(&contourBounds);

        if (c->bDirty)
        {
            /* if the contour is dirty, flatten it */
            c->bDirty = tpFalse;
            last = _tpSegmentArrayAtPtr(&c->segments, 0);

            vcount = 0;
            for (j = 1; j < c->segments.count; ++j)
            {
                current = _tpSegmentArrayAtPtr(&c->segments, j);

                curve.p0 = tpVec2Make(last->position.x, last->position.y);
                curve.h0 = tpVec2Make(last->handleOut.x, last->handleOut.y);
                curve.h1 = tpVec2Make(current->handleIn.x, current->handleIn.y);
                curve.p1 = tpVec2Make(current->position.x, current->position.y);

                if (_transform)
                {
                    curve.p0 = j > 1 ? lastTransformedPos : tpTransformApply(_transform, curve.p0);
                    curve.h0 = tpTransformApply(_transform, curve.h0);
                    curve.h1 = tpTransformApply(_transform, curve.h1);
                    curve.p1 = tpTransformApply(_transform, curve.p1);
                    lastTransformedPos = curve.p1;
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

            /* if the contour is closed, flatten the last closing curve */
            if (c->bIsClosed && c->segments.count &&
                    tpVec2Distance(_tpSegmentArrayAtPtr(&c->segments, 0)->position,
                                   _tpSegmentArrayAtPtr(&c->segments, c->segments.count - 1)->position) > FLT_EPSILON)
            {
                tpSegment * fs = _tpSegmentArrayAtPtr(&c->segments, 0);

                curve.p0 = tpVec2Make(last->position.x, last->position.y);
                curve.h0 = tpVec2Make(last->handleOut.x, last->handleOut.y);
                curve.h1 = tpVec2Make(fs->handleIn.x, fs->handleIn.y);
                curve.p1 = tpVec2Make(fs->position.x, fs->position.y);

                if (_transform)
                {
                    /* @TODO: are there cases were lastTransformedPos can still be uninitialized?!?! */
                    curve.p0 = lastTransformedPos;
                    curve.h0 = tpTransformApply(_transform, curve.h0);
                    curve.h1 = tpTransformApply(_transform, curve.h1);
                    curve.p1 = tpTransformApply(_transform, curve.p1);
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
            c->fillVertexOffset = off;
            /* otherwise we just copy the contour to the tmpbuffer... */
            _tpVec2ArrayAppendArray(_outVertices, _tpVec2ArrayAtPtr(&_path->geometryCache, c->fillVertexOffset), c->fillVertexCount);
            _tpBoolArrayAppendArray(_outJoints, _tpBoolArrayAtPtr(&_path->jointCache, c->fillVertexOffset), c->fillVertexCount);

            /* ...and merge the cached contour bounds with the path bounds */
            _tpGLMergeBounds(_outBounds, &c->bounds);
            off += c->fillVertexCount;
        }
    }

    return 0;
}

TARP_LOCAL int _tpGLColorStopComp(const void * _a, const void * _b)
{
    if (((tpColorStop *)_a)->offset < ((tpColorStop *)_b)->offset)
        return -1;
    return 1;
}

TARP_LOCAL void _tpGLFinalizeColorStops(_tpGLContext * _ctx, _tpGLGradient * _grad)
{
    int i, j;
    tpColorStop * current;
    tpBool bAdd, bHasStartStop, bHasEndStop;

    if (!_grad->stops.count) return;

    _tpColorStopArrayClear(&_ctx->tmpColorStops);

    bHasStartStop = tpFalse;
    bHasEndStop = tpFalse;

    /* remove duplicates */
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

    /* sort from 0 - 1 by offset */
    qsort(_ctx->tmpColorStops.array, _ctx->tmpColorStops.count, sizeof(tpColorStop), _tpGLColorStopComp);

    /* make sure there is a stop at 0 and 1 offset */
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
        /* if they are already there, we can simply swap */
        _tpColorStopArraySwap(&_grad->stops, &_ctx->tmpColorStops);
    }
}

TARP_LOCAL void _tpGLUpdateRampTexture(_tpGLGradient * _grad)
{
    tpColor pixels[TARP_GL_RAMP_TEXTURE_SIZE] = {};
    int xStart, xEnd, diff, i, j;
    tpFloat mixFact;
    tpColor mixColor;
    tpColorStop * stop1, * stop2;

    /* generate the ramp texture */
    xStart = 0;
    xEnd = 0;

    if (_grad->stops.count)
    {
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
            }
            stop1 = stop2;
            xStart = xEnd;
        }
    }

    _TARP_ASSERT_NO_GL_ERROR(glActiveTexture(GL_TEXTURE0));
    _TARP_ASSERT_NO_GL_ERROR(glBindTexture(GL_TEXTURE_1D, _grad->rampTexture));
    _TARP_ASSERT_NO_GL_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    _TARP_ASSERT_NO_GL_ERROR(glTexSubImage1D(GL_TEXTURE_1D, 0, 0, TARP_GL_RAMP_TEXTURE_SIZE,
                             GL_RGBA, GL_FLOAT, &pixels[0].r));
}

TARP_LOCAL void _tpGLUpdateVAO(_tpGLVAO * _vao, void * _data, int _byteCount)
{
    /* not sure if this buffer orphaning style data upload makes a difference these days anymore. (TEST??) */
    if ((GLuint)_byteCount > _vao->vboSize)
    {
        _TARP_ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, _byteCount, _data, GL_DYNAMIC_DRAW));
        _vao->vboSize = _byteCount;
    }
    else
    {
        _TARP_ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, _vao->vboSize, NULL, GL_DYNAMIC_DRAW));
        _TARP_ASSERT_NO_GL_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, _byteCount, _data));
    }
}

TARP_LOCAL void _tpGLDrawPaint(_tpGLContext * _ctx, _tpGLPath * _path,
                               const tpPaint * _paint, const _tpGLGradientCacheData * _gradCache)
{
    if (_paint->type == kTpPaintTypeColor)
    {
        /* @TODO: Cache the uniform loc */
        _TARP_ASSERT_NO_GL_ERROR(glUniform4fv(_ctx->meshColorLoc, 1, &_paint->data.color.r));
        _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, _path->boundsVertexOffset, 4));
    }
    else if (_paint->type == kTpPaintTypeGradient)
    {
        _tpGLGradient * grad = (_tpGLGradient *)_paint->data.gradient.pointer;

        /* bind the gradient's texture */
        _TARP_ASSERT_NO_GL_ERROR(glActiveTexture(GL_TEXTURE0));
        _TARP_ASSERT_NO_GL_ERROR(glBindTexture(GL_TEXTURE_1D, grad->rampTexture));

        _TARP_ASSERT_NO_GL_ERROR(glUseProgram(_ctx->textureProgram));
        _TARP_ASSERT_NO_GL_ERROR(glUniformMatrix4fv(_ctx->tpTextureLoc, 1, GL_FALSE, &_ctx->transformProjection.v[0]));
        _TARP_ASSERT_NO_GL_ERROR(glBindVertexArray(_ctx->textureVao.vao));
        _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_FAN, _gradCache->vertexOffset, _gradCache->vertexCount));
        _TARP_ASSERT_NO_GL_ERROR(glUseProgram(_ctx->program));
        _TARP_ASSERT_NO_GL_ERROR(glBindVertexArray(_ctx->vao.vao));
    }
}

TARP_LOCAL void _tpGLCacheBoundsGeometry(_tpGLPath * _path, const _tpGLStyle * _style)
{
    _tpGLRect bounds;
    _tpGLRect * bptr;
    tpVec2 boundsData[4];

    _path->boundsVertexOffset = _path->geometryCache.count;

    /*
    add the bounds geometry to the end of the geometry cache.
    If there is a stroke, upload the stroke bounds instead
    */
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

    boundsData[0] = bptr->min;
    boundsData[1] = tpVec2Make(bptr->min.x, bptr->max.y);
    boundsData[2] = tpVec2Make(bptr->max.x, bptr->min.y);
    boundsData[3] = bptr->max;

    _tpVec2ArrayAppendArray(&_path->geometryCache, boundsData, 4);
}

typedef struct TARP_LOCAL
{
    tpVec2 vertex;
    tpFloat tc;
} TexVertex;

TARP_LOCAL void _tpGLGradientLinearGeometry(
    _tpGLContext * _ctx,
    _tpGLGradient * _grad,
    const tpTransform * _paintTransform,
    const _tpGLRect * _bounds,
    _tpGLTextureVertexArray * _vertices,
    int * _outVertexOffset,
    int * _outVertexCount)
{
    /* regenerate the geometry for this path/gradient combo */
    _tpGLTextureVertex vertices[4];
    tpVec2 dir, ndir, dest, origin;
    tpVec2 tmp;
    tpFloat len2;
    int i;

    origin = tpTransformApply(_paintTransform, _grad->origin);
    dest = tpTransformApply(_paintTransform, _grad->destination);
    dir = tpVec2Sub(dest, origin);
    len2 = tpVec2LengthSquared(dir);
    ndir = tpVec2MultScalar(dir, 1/len2);
    
    vertices[0].vertex = _bounds->min;
    vertices[1].vertex = tpVec2Make(_bounds->max.x, _bounds->min.y);
    vertices[2].vertex = _bounds->max;
    vertices[3].vertex = tpVec2Make(_bounds->min.x, _bounds->max.y);

    for (i = 0; i < 4; ++i)
    {
        tmp = tpVec2Sub(vertices[i].vertex, origin);
        vertices[i].tc.x = tpVec2Dot(tmp, ndir);
        vertices[i].tc.y = 0;
    }

    *_outVertexOffset = _vertices->count;
    *_outVertexCount = 4;
    _tpGLTextureVertexArrayAppendArray(_vertices, vertices, 4);
}

TARP_LOCAL tpFloat _tpLineUnitCircleIntersection(tpVec2 _p0, tpVec2 _p1)
{
    /* Find t such that tpVec2Lerp(p0, p1, t) lies on the unit circle (taking the larger of two solutions) */
    tpFloat p0_2 = tpVec2LengthSquared(_p0);
    tpFloat p0p1 = tpVec2Dot(_p0, _p1);
    tpVec2 p0_p1 = tpVec2Sub(_p0, _p1);
    tpFloat p0_p1_2 = tpVec2LengthSquared(p0_p1);
    /* quadratic equation: a = (p0-p1)^2, b = 2 t (p0.p1 - p0^2), c = p0^2 - 1 */
    tpFloat discriminant = (p0p1-p0_2)*(p0p1-p0_2) - p0_p1_2*(p0_2-1);
    assert(discriminant >= 0);
    return (sqrt(discriminant) - p0p1 + p0_2) / p0_p1_2;
}

TARP_LOCAL tpFloat _tpLerp(tpFloat _a, tpFloat _b, tpFloat _t)
{
    return _a * (1.0f - _t) + _b * _t;
}

TARP_LOCAL tpFloat _tpInvLerp(tpFloat _a, tpFloat _b, tpFloat _x)
{
    assert(_b - _a != 0);
    return (_x - _a) / (_b - _a);
}

TARP_LOCAL void _tpGLGradientRadialGeometry(
    _tpGLContext * _ctx,
    _tpGLGradient * _grad,
    const tpTransform * _paintTransform,
    const _tpGLRect * _bounds,
    _tpGLTextureVertexArray * _vertices,
    int * _outVertexOffset,
    int * _outVertexCount)
{   
    /* regenerate the geometry for this path/gradient combo */
    _tpGLTextureVertex vertices[TARP_RADIAL_GRADIENT_SLICES + 7];
    int vertexCount;
    tpTransform ellipse, inverse;
    tpMat2 rot;
    tpVec2 focalPoint, a, b, circleFocalPoint;
    tpVec2 tmp, tmp2, tmp3;
    tpFloat phi, t;
    
    /* apply _paintTransform */
    a = tpVec2Sub(_grad->destination, _grad->origin);
    b.x = -a.y * _grad->ratio;
    b.y =  a.x * _grad->ratio;
    ellipse = tpTransformMake(a.x, b.x, _grad->origin.x,
                              a.y, b.y, _grad->origin.y);
    ellipse = tpTransformCombine(_paintTransform, &ellipse);
    a = tpVec2Make(ellipse.m.v[0], ellipse.m.v[1]);
    b = tpVec2Make(ellipse.m.v[2], ellipse.m.v[3]);
    if(tpVec2Cross(a,b) < 0){
        ellipse.m.v[2] = -ellipse.m.v[2];
        ellipse.m.v[3] = -ellipse.m.v[3];
    }
    
    tmp = tpVec2Add(_grad->focal_point_offset, _grad->origin);
    focalPoint = tpTransformApply(_paintTransform, tmp);
    
    /* avoid numerical instabilities for gradients of near-zero size */
    /* TODO: The values of 0.1 are somewhat arbitrarily chosen. This might require more rigorous analysis. */
    if(tpVec2LengthSquared(a) < 0.1 || tpVec2LengthSquared(b) < 0.1 || fabs(tpVec2Cross(a,b)) < 0.1){
        vertices[0].vertex = _bounds->min;
        vertices[1].vertex = tpVec2Make(_bounds->max.x, _bounds->min.y);
        vertices[2].vertex = _bounds->max;
        vertices[3].vertex = tpVec2Make(_bounds->min.x, _bounds->max.y);
        vertices[0].tc = vertices[1].tc = vertices[2].tc = vertices[3].tc = tpVec2Make(1,0);
        *_outVertexOffset = _vertices->count;
        *_outVertexCount = 4;
        _tpGLTextureVertexArrayAppendArray(_vertices, vertices, 4);
        return;
    }
    
    /* find a transform, which converts the ellipse into a unit circle */
    inverse = tpTransformInvert(&ellipse);
    
    /* ensure that the focal point lies inside the ellipse */
    tmp = tpTransformApply(&inverse, focalPoint);
    t = tpVec2Length(tmp);
    if(t > 0.999f){
        tmp2 = tpVec2MultScalar(tmp, 0.999f/t);
        focalPoint = tpTransformApply(&ellipse, tmp2);
    }
    circleFocalPoint = tpTransformApply(&inverse, focalPoint);
    
    vertices[0].vertex = focalPoint;
    vertices[0].tc = tpVec2Make(0,0);
    vertexCount = 1;
    
    phi = 2 * TARP_PI / TARP_RADIAL_GRADIENT_SLICES;
    rot = tpMat2MakeRotation(phi);
    
    /* max x, min y corner */
    tmp2 = tpVec2Make(_bounds->max.x, _bounds->min.y);
    vertices[vertexCount].vertex = tmp2;
    tmp3 = tpTransformApply(&inverse, tmp2);
    t = _tpLineUnitCircleIntersection(circleFocalPoint, tmp3);
    tmp = tpVec2Lerp(circleFocalPoint, tmp3, t);
    vertices[vertexCount].tc.x = 1/t;
    ++vertexCount;
    
    /* max x edge */
    if(focalPoint.x < _bounds->max.x) for(;;){
        tmp = tpMat2MultVec2(&rot, tmp);
        tmp2 = tpTransformApply(&ellipse, tmp);
        t = _tpInvLerp(focalPoint.x, tmp2.x, _bounds->max.x);
        tmp3 = tpVec2Make(_bounds->max.x, _tpLerp(focalPoint.y, tmp2.y, t));
        if(tmp2.x <= focalPoint.x || tmp3.y > _bounds->max.y) { break; }
        vertices[vertexCount].vertex = tmp3;
        vertices[vertexCount].tc.x   = t;
        ++vertexCount;
    }
    
    /* max x, max y corner */
    tmp2 = _bounds->max;
    vertices[vertexCount].vertex = tmp2;
    tmp3 = tpTransformApply(&inverse, tmp2);
    t = _tpLineUnitCircleIntersection(circleFocalPoint, tmp3);
    tmp = tpVec2Lerp(circleFocalPoint, tmp3, t);
    vertices[vertexCount].tc.x = 1/t;
    ++vertexCount;
    
    /* max y edge */
    if(focalPoint.y < _bounds->max.y) for(;;){
        tmp = tpMat2MultVec2(&rot, tmp);
        tmp2 = tpTransformApply(&ellipse, tmp);
        t = _tpInvLerp(focalPoint.y, tmp2.y, _bounds->max.y);
        tmp3 = tpVec2Make(_tpLerp(focalPoint.x, tmp2.x, t), _bounds->max.y);
        if(tmp2.y <= focalPoint.y || tmp3.x < _bounds->min.x) { break; }
        vertices[vertexCount].vertex = tmp3;
        vertices[vertexCount].tc.x   = t;
        ++vertexCount;
    }
    
    /* min x, max y corner */
    tmp2 = tpVec2Make(_bounds->min.x, _bounds->max.y);
    vertices[vertexCount].vertex = tmp2;
    tmp3 = tpTransformApply(&inverse, tmp2);
    t = _tpLineUnitCircleIntersection(circleFocalPoint, tmp3);
    tmp = tpVec2Lerp(circleFocalPoint, tmp3, t);
    vertices[vertexCount].tc.x = 1/t;
    ++vertexCount;
    
    /* min x edge */
    if(focalPoint.x > _bounds->min.x) for(;;){
        tmp = tpMat2MultVec2(&rot, tmp);
        tmp2 = tpTransformApply(&ellipse, tmp);
        t = _tpInvLerp(focalPoint.x, tmp2.x, _bounds->min.x);
        tmp3 = tpVec2Make(_bounds->min.x, _tpLerp(focalPoint.y, tmp2.y, t));
        if(tmp2.x >= focalPoint.x || tmp3.y < _bounds->min.y) { break; }
        vertices[vertexCount].vertex = tmp3;
        vertices[vertexCount].tc.x   = t;
        ++vertexCount;
    }
    
    /* min x, min y corner */
    tmp2 = _bounds->min;
    vertices[vertexCount].vertex = tmp2;
    tmp3 = tpTransformApply(&inverse, tmp2);
    t = _tpLineUnitCircleIntersection(circleFocalPoint, tmp3);
    tmp = tpVec2Lerp(circleFocalPoint, tmp3, t);
    vertices[vertexCount].tc.x = 1/t;
    ++vertexCount;
    
    /* min y edge */
    if(focalPoint.y > _bounds->min.y) for(;;){
        tmp = tpMat2MultVec2(&rot, tmp);
        tmp2 = tpTransformApply(&ellipse, tmp);
        t = _tpInvLerp(focalPoint.y, tmp2.y, _bounds->min.y);
        tmp3 = tpVec2Make(_tpLerp(focalPoint.x, tmp2.x, t), _bounds->min.y);
        if(tmp2.y >= focalPoint.y || tmp3.x > _bounds->max.x) { break; }
        vertices[vertexCount].vertex = tmp3;
        vertices[vertexCount].tc.x   = t;
        ++vertexCount;
    }
    
    /* max x, min y corner */
    vertices[vertexCount].vertex = vertices[1].vertex;
    vertices[vertexCount].tc.x   = vertices[1].tc.x;
    ++vertexCount;
    
    assert((unsigned long long)vertexCount <= sizeof(vertices) / sizeof(_tpGLTextureVertex));

    *_outVertexOffset = _vertices->count;
    *_outVertexCount = vertexCount;
    _tpGLTextureVertexArrayAppendArray(_vertices, vertices, vertexCount);
}

TARP_LOCAL void _tpGLCacheGradientGeometry(_tpGLContext * _ctx, _tpGLGradient * _grad,
        _tpGLPath * _path, _tpGLGradientCacheData * _gradCache, _tpGLTextureVertexArray * _vertices,
        const tpTransform * _paintTransform, tpBool _bPaintTransformDirty)
{
    _tpGLGradient * grad = _grad;

    /* check if we need to update the gradient texture */
    if (grad->bDirty)
    {
        grad->bDirty = tpFalse;
        /* ensure that the color stops are valid/complete */
        _tpGLFinalizeColorStops(_ctx, grad);
        /* update the ramp texture */
        /* TODO: update the ramp texture separately just before drawing to avoid multiple texture binds */
        _tpGLUpdateRampTexture(grad);
        _gradCache->lastGradientID = -1;
    }

    if (_gradCache->lastGradientID != grad->gradientID || _bPaintTransformDirty)
    {
        /* rebuild the gradient */
        if (grad->type == kTpGradientTypeLinear)
        {
            _tpGLGradientLinearGeometry(_ctx, grad, _paintTransform,
                                        _gradCache->bounds, _vertices,
                                        &_gradCache->vertexOffset, &_gradCache->vertexCount);

        }
        else if (grad->type == kTpGradientTypeRadial)
        {
            _tpGLGradientRadialGeometry(_ctx, grad, _paintTransform,
                                        _gradCache->bounds, _vertices,
                                        &_gradCache->vertexOffset, &_gradCache->vertexCount);

        }
        _gradCache->lastGradientID = grad->gradientID;
    }
    else
    {
        /* copy cached gradient data */
        _tpGLTextureVertexArrayAppendArray(_vertices,
                                           _tpGLTextureVertexArrayAtPtr(&_path->textureGeometryCache, _gradCache->vertexOffset),
                                           _gradCache->vertexCount);
    }
}

TARP_API tpBool tpPrepareDrawing(tpContext _ctx)
{
    _tpGLContext * ctx = (_tpGLContext *)_ctx.pointer;

    /* cache previous render state so we can reset it in tpFinishDrawing */
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint *)&ctx->stateBackup.activeTexture);
    ctx->stateBackup.depthTest = glIsEnabled(GL_DEPTH_TEST);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &ctx->stateBackup.depthMask);
    ctx->stateBackup.multisample = glIsEnabled(GL_MULTISAMPLE);
    ctx->stateBackup.stencilTest = glIsEnabled(GL_STENCIL_TEST);
    glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint *)&ctx->stateBackup.stencilMask);
    glGetIntegerv(GL_STENCIL_FAIL, (GLint *)&ctx->stateBackup.stencilFail);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint *)&ctx->stateBackup.stencilPassDepthPass);
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint *)&ctx->stateBackup.stencilPassDepthFail);
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, (GLint *)&ctx->stateBackup.clearStencil);
    ctx->stateBackup.blending = glIsEnabled(GL_BLEND);
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint *)&ctx->stateBackup.blendSrcRGB);
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint *)&ctx->stateBackup.blendDestRGB);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint *)&ctx->stateBackup.blendSrcAlpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint *)&ctx->stateBackup.blendDestAlpha);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint *)&ctx->stateBackup.blendEquationRGB);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint *)&ctx->stateBackup.blendEquationAlpha);
    ctx->stateBackup.cullFace = glIsEnabled(GL_CULL_FACE);
    glGetIntegerv(GL_CULL_FACE_MODE, (GLint *)&ctx->stateBackup.cullFaceMode);
    glGetIntegerv(GL_FRONT_FACE, (GLint *)&ctx->stateBackup.frontFace);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint *)&ctx->stateBackup.vao);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint *)&ctx->stateBackup.vbo);
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *)&ctx->stateBackup.program);

    _TARP_ASSERT_NO_GL_ERROR(glActiveTexture(GL_TEXTURE0));

    _TARP_ASSERT_NO_GL_ERROR(glDisable(GL_DEPTH_TEST));
    _TARP_ASSERT_NO_GL_ERROR(glDepthMask(GL_FALSE));
    _TARP_ASSERT_NO_GL_ERROR(glEnable(GL_MULTISAMPLE));

    /* @TODO: Support different ways of blending?? */
    _TARP_ASSERT_NO_GL_ERROR(glEnable(GL_BLEND));
    _TARP_ASSERT_NO_GL_ERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    _TARP_ASSERT_NO_GL_ERROR(glEnable(GL_STENCIL_TEST));
    _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLFillRasterStencilPlane | _kTpGLClippingStencilPlaneOne | _kTpGLClippingStencilPlaneTwo | _kTpGLStrokeRasterStencilPlane));
    _TARP_ASSERT_NO_GL_ERROR(glClearStencil(255));
    _TARP_ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));

    _TARP_ASSERT_NO_GL_ERROR(glBindVertexArray(ctx->vao.vao));
    _TARP_ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, ctx->vao.vbo));

    _TARP_ASSERT_NO_GL_ERROR(glUseProgram(ctx->program));

    ctx->clippingStackDepth = 0; /* reset clipping */

    return tpFalse;
}

TARP_API tpBool tpFinishDrawing(tpContext _ctx)
{
    /* reset gl state to what it was before we began drawing */
    _tpGLContext * ctx = (_tpGLContext *)_ctx.pointer;

    /* we dont assert gl errors here for now...should we? */
    glActiveTexture(ctx->stateBackup.activeTexture);
    ctx->stateBackup.depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glDepthMask(ctx->stateBackup.depthMask);
    ctx->stateBackup.multisample ? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);
    ctx->stateBackup.stencilTest ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
    glStencilMask(ctx->stateBackup.stencilMask);
    glStencilOp(ctx->stateBackup.stencilFail,
                ctx->stateBackup.stencilPassDepthPass,
                ctx->stateBackup.stencilPassDepthFail);
    glClearStencil(ctx->stateBackup.clearStencil);
    ctx->stateBackup.blending ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    glBlendFuncSeparate(ctx->stateBackup.blendSrcRGB,
                        ctx->stateBackup.blendDestRGB,
                        ctx->stateBackup.blendSrcAlpha,
                        ctx->stateBackup.blendDestAlpha);
    glBlendEquationSeparate(ctx->stateBackup.blendEquationRGB,
                            ctx->stateBackup.blendEquationAlpha);

    ctx->stateBackup.cullFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    glCullFace(ctx->stateBackup.cullFaceMode);
    glFrontFace(ctx->stateBackup.frontFace);
    glBindVertexArray(ctx->stateBackup.vao);
    glBindBuffer(GL_ARRAY_BUFFER, ctx->stateBackup.vbo);
    glUseProgram(ctx->stateBackup.program);

    return tpFalse;
}

TARP_LOCAL void _tpGLPrepareStencilPlanes(_tpGLContext * _ctx, tpBool _bIsClippingPath, int * _outTargetStencilPlane, int * _outTestStencilPlane)
{
    *_outTargetStencilPlane = _bIsClippingPath ? _ctx->currentClipStencilPlane : _kTpGLFillRasterStencilPlane;
    *_outTestStencilPlane = _ctx->currentClipStencilPlane == _kTpGLClippingStencilPlaneOne ? _kTpGLClippingStencilPlaneTwo : _kTpGLClippingStencilPlaneOne;
}

TARP_LOCAL tpBool _tpGLDrawPathImpl(_tpGLContext * _ctx, _tpGLPath * _path, tpStyle _style, tpBool _bIsClipPath)
{
    GLint i;
    GLuint stencilPlaneToWriteTo, stencilPlaneToTestAgainst;
    _tpGLRect bounds;
    _tpGLPath * p = _path;
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;

    assert(_ctx);
    assert(p && s);

    /* check if the transform projection is dirty */
    if (p->lastDrawContext != _ctx ||
            p->lastTransformID != _ctx->transformID)
    {
        /* @TODO: we should also take skew into account here, not only scale */
        if (_ctx->transformScale > p->lastTransformScale || !s->bScaleStroke)
        {
            _tpGLMarkPathGeometryDirty(p);
        }
        p->lastTransformID = _ctx->transformID;
        p->lastDrawContext = _ctx;
        p->lastTransformScale = _ctx->transformScale;
    }

    /*
    if this style has a stroke and its scale stroke property is different from the last style,
    we force a full reflattening of all path contours.
    we also do this if the style has non scaling stroke and the transform changed since we
    last drew the path.
    */
    if (!p->bPathGeometryDirty && s->stroke.type != kTpPaintTypeNone && p->lastStroke.bScaleStroke != s->bScaleStroke)
    {
        _tpGLMarkPathGeometryDirty(p);
    }

    if (_ctx->bTransformProjDirty)
    {
        _ctx->bTransformProjDirty = tpFalse;
        _ctx->renderTransform = tpMat4MakeFrom2DTransform(&_ctx->transform);
        _ctx->transformProjection = tpMat4Mult(&_ctx->projection, &_ctx->renderTransform);
    }

    /*
    check if the path geometry is dirty.
    if so, rebuild everything!
    */
    if (p->bPathGeometryDirty)
    {
        p->bPathGeometryDirty = tpFalse;

        /* flatten the path into tmp buffers */
        if (s->bScaleStroke)
            _tpGLFlattenPath(p, 0.15f / _ctx->transformScale, NULL, &_ctx->tmpVertices, &_ctx->tmpJoints, &bounds);
        else
            _tpGLFlattenPath(p, 0.15f, &_ctx->transform, &_ctx->tmpVertices, &_ctx->tmpJoints, &bounds);

        /* generate and add the stroke geometry to the tmp buffers */
        if (s->stroke.type != kTpPaintTypeNone && s->strokeWidth > 0)
            _tpGLStroke(p, s, &_ctx->tmpVertices, &_ctx->tmpJoints);

        /* swap the tmp buffers with the path caches */
        _tpVec2ArrayClear(&p->geometryCache);
        _tpBoolArrayClear(&p->jointCache);
        _tpVec2ArraySwap(&p->geometryCache, &_ctx->tmpVertices);
        _tpBoolArraySwap(&p->jointCache, &_ctx->tmpJoints);

        /* save the path bounds */
        p->boundsCache = bounds;

        /* add the bounds geometry to the geom cache (and potentially cache stroke bounds) */
        _tpGLCacheBoundsGeometry(p, s);

        /* force recalculation of gradient related geometries */
        p->fillGradientData.lastGradientID = -1;
        p->strokeGradientData.lastGradientID = -1;
    }
    /* check if the stroke should be removed */
    else if (((s->stroke.type == kTpPaintTypeNone &&
               p->lastStroke.strokeType != kTpPaintTypeNone) ||
              (s->strokeWidth == 0 && p->lastStroke.strokeWidth > 0)))
    {
        p->lastStroke.strokeType = s->stroke.type;
        p->lastStroke.strokeWidth = 0;
        p->strokeVertexOffset = 0;
        p->strokeVertexCount = 0;
    }
    /* check if the stroke needs to be regenerated (due to a change in stroke width or dash related settings) */
    else if (!_bIsClipPath && ((s->stroke.type != kTpPaintTypeNone && s->strokeWidth > 0 &&
                                (p->lastStroke.strokeWidth != s->strokeWidth ||
                                 p->lastStroke.cap != s->strokeCap ||
                                 p->lastStroke.join != s->strokeJoin ||
                                 p->lastStroke.dashCount != s->dashCount ||
                                 p->lastStroke.dashOffset != s->dashOffset ||
                                 memcmp(p->lastStroke.dashArray, s->dashArray, sizeof(tpFloat) * s->dashCount) != 0))))
    {
        /* remove all the old stoke vertices from the cache */
        _tpVec2ArrayRemoveRange(&p->geometryCache, p->strokeVertexOffset, p->geometryCache.count);

        /* generate and add the stroke geometry to the cache. */
        _tpGLStroke(p, s, &p->geometryCache, &p->jointCache);

        /* add the bounds geometry to the geom cache. */
        _tpGLCacheBoundsGeometry(p, s);

        /* force rebuilding of the stroke gradient geometry */
        p->strokeGradientData.lastGradientID = -1;
    }

    /*
    check if there are any gradients to be cached.
    @TODO: This if statement could really need a cleaner rework. Basically what we are doing here is
    checking if any property changed that triggers the gradient geometry to be recached.
    */
    if (!_bIsClipPath && ((s->fill.type == kTpPaintTypeGradient &&
                           (p->fillGradientData.lastGradientID != ((_tpGLGradient *)s->fill.data.gradient.pointer)->gradientID ||
                            p->bFillPaintTransformDirty || ((_tpGLGradient *)s->fill.data.gradient.pointer)->bDirty)) ||
                          (s->stroke.type == kTpPaintTypeGradient &&
                           (p->strokeGradientData.lastGradientID != ((_tpGLGradient *)s->stroke.data.gradient.pointer)->gradientID ||
                            p->bStrokePaintTransformDirty || ((_tpGLGradient *)s->stroke.data.gradient.pointer)->bDirty))))
    {
        _tpGLTextureVertexArrayClear(&_ctx->tmpTexVertices);
        if (s->fill.type == kTpPaintTypeGradient)
        {
            _tpGLGradient * grad = (_tpGLGradient *)s->fill.data.gradient.pointer;
            _tpGLCacheGradientGeometry(_ctx, grad, p, &p->fillGradientData, &_ctx->tmpTexVertices, &p->fillPaintTransform, p->bFillPaintTransformDirty);
            p->bFillPaintTransformDirty = tpFalse;
        }

        if (s->stroke.type == kTpPaintTypeGradient)
        {
            _tpGLGradient * grad = (_tpGLGradient *)s->stroke.data.gradient.pointer;
            _tpGLCacheGradientGeometry(_ctx, grad, p, &p->strokeGradientData, &_ctx->tmpTexVertices, &p->strokePaintTransform, p->bStrokePaintTransformDirty);
            p->bStrokePaintTransformDirty = tpFalse;
        }

        _tpGLTextureVertexArraySwap(&p->textureGeometryCache, &_ctx->tmpTexVertices);
    }

    if (!_bIsClipPath && (s->fill.type == kTpPaintTypeGradient || s->stroke.type == kTpPaintTypeGradient))
    {
        _TARP_ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->textureVao.vbo));
        _tpGLUpdateVAO(&_ctx->textureVao, p->textureGeometryCache.array, sizeof(_tpGLTextureVertex) * p->textureGeometryCache.count);
        _TARP_ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->vao.vbo));
    }

    /* upload the paths geometry cache to the gpu */
    _tpGLUpdateVAO(&_ctx->vao, p->geometryCache.array, sizeof(tpVec2) * p->geometryCache.count);

    if (s->bScaleStroke)
        _TARP_ASSERT_NO_GL_ERROR(glUniformMatrix4fv(_ctx->tpLoc, 1, GL_FALSE, &_ctx->transformProjection.v[0]));
    else
    {
        _TARP_ASSERT_NO_GL_ERROR(glUniformMatrix4fv(_ctx->tpLoc, 1, GL_FALSE, &_ctx->projection.v[0]));
    }

    /* draw the fill */
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

            _TARP_ASSERT_NO_GL_ERROR(glStencilFunc(GL_EQUAL, 0, _kTpGLFillRasterStencilPlane));
        }
        else if (s->fillRule == kTpFillRuleNonZero)
        {
            /*
            NonZero winding rule needs to use Increment and Decrement stencil operations.
            we therefore render to the rasterize mask, even if this is a clipping mask, and transfer
            the results to the clipping mask stencil plane afterwards
            */
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

                /*
                draw the bounds one last time to zero out the tmp data created in the _kTpGLFillRasterStencilPlane
                */
                _TARP_ASSERT_NO_GL_ERROR(glStencilMask(stencilPlaneToWriteTo));
                _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO));
                _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, p->boundsVertexOffset, 4));

                return tpFalse;
            }
            else
            {
                _TARP_ASSERT_NO_GL_ERROR(glStencilFunc(GL_NOTEQUAL, 255, _kTpGLFillRasterStencilPlane));
            }
        }

        _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLFillRasterStencilPlane));
        _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT));
        _TARP_ASSERT_NO_GL_ERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
        _tpGLDrawPaint(_ctx, p, &s->fill, &p->fillGradientData);
    }

    /* we don't care for stroke if this is a clipping path */
    if (_bIsClipPath) return tpFalse;

    /* draw the stroke */
    if (p->strokeVertexCount)
    {
        _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLStrokeRasterStencilPlane));
        _TARP_ASSERT_NO_GL_ERROR(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
        _TARP_ASSERT_NO_GL_ERROR(glStencilFunc(_ctx->clippingStackDepth ? GL_NOTEQUAL : GL_ALWAYS, 0, stencilPlaneToTestAgainst));
        _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

        /* Draw all stroke triangles of all contours at once */
        _TARP_ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLES, p->strokeVertexOffset, p->strokeVertexCount));

        _TARP_ASSERT_NO_GL_ERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
        _TARP_ASSERT_NO_GL_ERROR(glStencilFunc(GL_EQUAL, 0, _kTpGLStrokeRasterStencilPlane));
        _TARP_ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT));

        _tpGLDrawPaint(_ctx, p, &s->stroke, &p->strokeGradientData);
    }

    /* WE DONE BABY */
    return tpFalse;
}

TARP_API tpBool tpDrawPath(tpContext _ctx, tpPath _path, const tpStyle _style)
{
    return _tpGLDrawPathImpl((_tpGLContext *)_ctx.pointer, (_tpGLPath *)_path.pointer, _style, tpFalse);
}

TARP_LOCAL tpBool _tpGLGenerateClippingMask(_tpGLContext * _ctx, _tpGLPath * _path, tpBool _bIsRebuilding)
{
    tpBool drawResult;
    assert(_ctx);

    if (!_bIsRebuilding)
        _ctx->clippingStack[_ctx->clippingStackDepth++] = _path;

    /*
    @TODO: Instead of clearing maybe just clear it in endClipping by
    drawing the bounds of the last clip path? could be a potential speed up
    */
    _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_ctx->currentClipStencilPlane));
    _TARP_ASSERT_NO_GL_ERROR(glClearStencil(0));
    _TARP_ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));

    /* draw path */
    drawResult = _tpGLDrawPathImpl(_ctx, _path, _ctx->clippingStyle, tpTrue);
    if (drawResult) return tpTrue;

    _ctx->currentClipStencilPlane = _ctx->currentClipStencilPlane == _kTpGLClippingStencilPlaneOne ?
                                    _kTpGLClippingStencilPlaneTwo : _kTpGLClippingStencilPlaneOne;

    return tpFalse;
}

TARP_API tpBool tpBeginClipping(tpContext _ctx, tpPath _path)
{
    return _tpGLGenerateClippingMask((_tpGLContext *)_ctx.pointer, (_tpGLPath *)_path.pointer, tpFalse);
}

TARP_API tpBool tpEndClipping(tpContext _ctx)
{
    int i;
    _tpGLPath * p;
    _tpGLContext * ctx = (_tpGLContext *)_ctx.pointer;
    assert(ctx->clippingStackDepth);
    p = ctx->clippingStack[--ctx->clippingStackDepth];

    if (ctx->clippingStackDepth)
    {
        /* check if the last clip mask is still in one of the clipping planes... */
        if (ctx->bCanSwapStencilPlanes)
        {
            ctx->currentClipStencilPlane = ctx->currentClipStencilPlane == _kTpGLClippingStencilPlaneOne ?
                                           _kTpGLClippingStencilPlaneTwo : _kTpGLClippingStencilPlaneOne;
            ctx->bCanSwapStencilPlanes = tpFalse;
        }
        else
        {
            /* ...otherwise rebuild it */
            _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLClippingStencilPlaneOne | _kTpGLClippingStencilPlaneTwo));
            _TARP_ASSERT_NO_GL_ERROR(glClearStencil(255));
            _TARP_ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));

            for (i = 0; i < ctx->clippingStackDepth; ++i)
            {
                /* draw clip path */
                _tpGLGenerateClippingMask(ctx, ctx->clippingStack[i], tpTrue);
            }

            ctx->bCanSwapStencilPlanes = tpTrue;
        }
    }
    else
    {
        /*
        @TODO: Instead of clearing maybe just redrawing the clipping path bounds to
        reset the stencil? Might scale better for a lot of paths :)
        */
        _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLClippingStencilPlaneOne | _kTpGLClippingStencilPlaneTwo));
        _TARP_ASSERT_NO_GL_ERROR(glClearStencil(255));
        _TARP_ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));
    }

    return tpFalse;
}

TARP_API tpBool tpResetClipping(tpContext _ctx)
{
    _tpGLContext * ctx = (_tpGLContext *)_ctx.pointer;
    _TARP_ASSERT_NO_GL_ERROR(glStencilMask(_kTpGLClippingStencilPlaneOne | _kTpGLClippingStencilPlaneTwo));
    _TARP_ASSERT_NO_GL_ERROR(glClearStencil(0));
    _TARP_ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));

    ctx->currentClipStencilPlane = _kTpGLClippingStencilPlaneOne;
    ctx->clippingStackDepth = 0;

    return tpFalse;
}

TARP_API tpBool tpSetProjection(tpContext _ctx, const tpMat4 * _projection)
{
    _tpGLContext * ctx = (_tpGLContext *)_ctx.pointer;
    ctx->projection = *_projection;
    ctx->projectionID++;
    ctx->bTransformProjDirty = tpTrue;

    return tpFalse;
}

TARP_API tpBool tpSetTransform(tpContext _ctx, const tpTransform * _transform)
{
    tpFloat rotation;
    _tpGLContext * ctx = (_tpGLContext *)_ctx.pointer;

    if (!tpTransformEquals(_transform, &ctx->transform))
    {
        tpVec2 scale, skew, translation;
        ctx->transform = *_transform;
        ctx->transformID++;
        ctx->bTransformProjDirty = tpTrue;

        tpTransformDecompose(_transform, &translation, &scale, &skew, &rotation);
        ctx->transformScale = TARP_MAX(scale.x, scale.y);
    }

    return tpFalse;
}

TARP_API tpBool tpResetTransform(tpContext _ctx)
{
    _tpGLContext * ctx = (_tpGLContext *)_ctx.pointer;
    ctx->transform = tpTransformMakeIdentity();
    ctx->transformScale = 1.0;
    ctx->transformID++;
    ctx->bTransformProjDirty = tpTrue;
    return tpFalse;
}

#endif /* TARP_IMPLEMENTATION_OPENGL */
#endif /* TARP_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* TARP_TARP_H */

/*
MIT License:
------------------------------------------------------------------------------
Copyright (c) 2018 Matthias Dörfelt
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
*/
