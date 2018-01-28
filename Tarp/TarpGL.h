#ifndef TARP_TARPGL_H
#define TARP_TARPGL_H

#include <Tarp/Tarp.h>
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
#define ASSERT_NO_GL_ERROR(_func) do { _func; \
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
#define ASSERT_NO_GL_ERROR(_func) _func
#endif

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

//The shader programs used by the renderer
static const char * _vertexShaderCode =
    "#version 150 \n"
    "uniform mat4 transform; \n"
    "uniform mat4 projection; \n"
    "uniform vec4 meshColor; \n"
    "in vec2 vertex; \n"
    "out vec4 icol;\n"
    "void main() \n"
    "{ \n"
    "gl_Position = projection * transform * vec4(vertex, 0.0, 1.0); \n"
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
    "uniform mat4 transform; \n"
    "uniform mat4 projection; \n"
    "in vec2 vertex; \n"
    "in float tc; \n"
    "out float itc;\n"
    "void main() \n"
    "{ \n"
    "gl_Position = projection * transform * vec4(vertex, 0.0, 1.0); \n"
    "itc = tc; \n"
    "} \n";

static const char * _fragmentShaderCodeTexture =
    "#version 150 \n"
    "uniform sampler1D tex;\n"
    "in float itc; \n"
    "out vec4 pixelColor; \n"
    "void main() \n"
    "{ \n"
    "pixelColor = texture(tex, itc) * vec4(0.0); \n"
    "} \n";

#define _TARP_STACK_BUFFER_T _tpFloatStackBuffer
#define _TARP_STACK_BUFFER_ITEM_T tpFloat
#define _TARP_STACK_BUFFER_SIZE 512
#include <Tarp/TarpStackBuffer.h>

typedef enum
{
    //The mask values are important! FillRaster needs to have at least
    //2 bits. These need to be the lower bits in order to work with the
    //Increment, Decrement stencil operations
    //http://www.opengl.org/discussion_boards/showthread.php/149740-glStencilOp-s-GL_INCR-GL_DECR-behaviour-when-masked
    _kTpFillRasterStencilPlane = 0x1F, //binary mask    00011111
    _kTpClipStencilPlaneOne = 1 << 5, //binary mask     00100000
    _kTpClipStencilPlaneTwo = 1 << 6, //binary mask     01000000
    _kTpStrokeRasterStencilPlane = 1 << 7 //binary mask 10000000
} _tpStencilPlane;

struct _tpHandleData
{
    void * pointer;
};

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
    tpPaintType strokeType;
    tpFloat strokeWidth;
    tpFloat dashOffset;
    tpFloat dashArray[TARP_MAX_DASH_ARRAY_SIZE];
    int dashCount;
    tpStrokeJoin join;
    tpStrokeCap cap;
} _tpGLStrokeData;

typedef struct
{
    tpVec2 origin;
    tpVec2 destination;
    _tpColorStopArray stops;
    tpGradientType type;
    tpContext * context;

    //rendering specific data/caches
    tpBool bDirty;
    GLuint rampTexture;
} _tpGLGradient;

typedef struct
{
    _tpGLContourArray contours;
    int currentContourIndex;
    char errorMessage[TARP_GL_ERROR_MESSAGE_SIZE];
    tpMat3 transform;

    //rendering specific data/caches
    _tpVec2Array geometryCache;
    _tpBoolArray jointCache;

    tpBool bPathGeometryDirty;
    tpFloat lastTransformScale;
    tpMat4 renderTransform;
    _tpGLRect boundsCache;

    //holds the properties that the stroke geometry
    //was generated with.
    _tpGLStrokeData lastStroke;
    _tpGLGradient * lastFillGradient;
    _tpGLGradient * lastStrokeGradient;

    int strokeVertexOffset;
    int strokeVertexCount;

    int fillPaintVertexOffset;
    int fillPaintVertexCount;

    int boundsVertexOffset;

    //holds a pointer to the context that owns this path
    tpContext * context;
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
    tpFillType fillType;
    tpFloat dashArray[TARP_MAX_DASH_ARRAY_SIZE];
    int dashCount;
    tpFloat dashOffset;
    tpFloat miterLimit;
    tpContext * context;
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

struct _tpContextData
{
    GLuint program;
    GLuint textureProgram;

    // _tpGLVAO vao;
    // _tpGLVAO textureVao;

    // we use two vaos, one for basic drawing and one for textured drawing.
    // both vaos share one vbo so that we only have to upload data once for
    // each path drawn.
    GLuint vao;
    GLuint textureVao;
    GLuint textureVBO;
    GLuint vbo;
    GLuint vboSize;

    GLuint currentClipStencilPlane;
    tpPath clippingStack[TARP_GL_MAX_CLIPPING_STACK_DEPTH];
    int clippingStackDepth;
    tpMat4 projection;
    _tpGLPathPtrArray paths;
    _tpGLStylePtrArray styles;
    _tpGLGradientPtrArray gradients;
    //used to temporarily store vertex/stroke data (think double buffering)
    _tpVec2Array tmpVertices;
    _tpBoolArray tmpJoints;
    _tpColorStopArray tmpColorStops;
    char errorMessage[TARP_GL_ERROR_MESSAGE_SIZE];
};

typedef struct
{
    char message[TARP_GL_ERROR_MESSAGE_SIZE];
    int length;
} _ErrorMessage;

static tpBool _compileShader(const char * _shaderCode, GLenum _shaderType, GLuint * _outHandle, _ErrorMessage * _outError)
{
    GLenum glHandle = glCreateShader(_shaderType);
    GLint len = strlen(_shaderCode);
    ASSERT_NO_GL_ERROR(glShaderSource(glHandle, 1, &_shaderCode, &len));
    ASSERT_NO_GL_ERROR(glCompileShader(glHandle));

    //check if the shader compiled
    GLint state;
    ASSERT_NO_GL_ERROR(glGetShaderiv(glHandle, GL_COMPILE_STATUS, &state));
    if (state == GL_FALSE)
    {
        if (_outError)
        {
            GLint infologLength;
            ASSERT_NO_GL_ERROR(glGetShaderiv(glHandle, GL_INFO_LOG_LENGTH, &infologLength));
            infologLength = TARP_MIN(TARP_GL_ERROR_MESSAGE_SIZE, infologLength);
            ASSERT_NO_GL_ERROR(glGetShaderInfoLog(glHandle, infologLength, &infologLength, _outError->message));
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

static tpBool _createProgram(const char * _vertexShader, const char * _fragmentShader, int _bTexProgram, GLuint * _outHandle, _ErrorMessage * _outError)
{
    GLuint vertexShader, fragmentShader;
    tpBool err = _compileShader(_vertexShader, GL_VERTEX_SHADER, &vertexShader, _outError);
    if (!err)
    {
        err = _compileShader(_fragmentShader, GL_FRAGMENT_SHADER, &fragmentShader, _outError);
    }
    if (err) return err;

    GLuint program = glCreateProgram();
    ASSERT_NO_GL_ERROR(glAttachShader(program, vertexShader));
    ASSERT_NO_GL_ERROR(glAttachShader(program, fragmentShader));

    //bind the attribute locations from the layout
    /*const BufferLayout::BufferElementArray & elements = _settings.vertexLayout.elements();
    auto sit = elements.begin();

    for (; sit != elements.end(); ++sit)
    {
        ASSERT_NO_GL_ERROR(glBindAttribLocation(program, (*sit).m_location, (*sit).m_name.cString()));
    }*/

    ASSERT_NO_GL_ERROR(glBindAttribLocation(program, 0, "vertex"));
    if (_bTexProgram)
        ASSERT_NO_GL_ERROR(glBindAttribLocation(program, 1, "tc"));

    ASSERT_NO_GL_ERROR(glLinkProgram(program));

    //check if we had success
    GLint state;
    ASSERT_NO_GL_ERROR(glGetProgramiv(program, GL_LINK_STATUS, &state));

    if (state == GL_FALSE)
    {
        if (_outError)
        {
            GLint infologLength;
            ASSERT_NO_GL_ERROR(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength));
            infologLength = TARP_MIN(TARP_GL_ERROR_MESSAGE_SIZE, infologLength);
            ASSERT_NO_GL_ERROR(glGetProgramInfoLog(program, infologLength, &infologLength, _outError->message));
            _outError->length = infologLength;
        }

        return tpTrue;
    }

    ASSERT_NO_GL_ERROR(glDeleteShader(vertexShader));
    ASSERT_NO_GL_ERROR(glDeleteShader(fragmentShader));

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

tpBool tpContextInit(tpContext * _ctx)
{
    int ret = 0;
    _ErrorMessage msg;

    ret = _createProgram(_vertexShaderCode, _fragmentShaderCode, 0, &_ctx->program, &msg);
    if (ret)
    {
        strcpy(_ctx->errorMessage, msg.message);
        return ret;
    }
    ret = _createProgram(_vertexShaderCodeTexture, _fragmentShaderCodeTexture, 1, &_ctx->textureProgram, &msg);
    if (ret)
    {
        strcpy(_ctx->errorMessage, msg.message);
        return ret;
    }

    ASSERT_NO_GL_ERROR(glGenBuffers(1, &_ctx->vbo));
    ASSERT_NO_GL_ERROR(glGenBuffers(1, &_ctx->textureVBO));
    _ctx->vboSize = 0;

    ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->vbo));

    ASSERT_NO_GL_ERROR(glGenVertexArrays(1, &_ctx->vao));
    ASSERT_NO_GL_ERROR(glBindVertexArray(_ctx->vao));
    ASSERT_NO_GL_ERROR(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, ((char *)0)));
    ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(0));

    ASSERT_NO_GL_ERROR(glGenVertexArrays(1, &_ctx->textureVao));
    ASSERT_NO_GL_ERROR(glBindVertexArray(_ctx->textureVao));
    ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->textureVBO));
    ASSERT_NO_GL_ERROR(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), ((char *)0)));
    ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(0));
    ASSERT_NO_GL_ERROR(glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), ((char *)(2 * sizeof(float)))));
    ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(1));

    ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->vbo));

    _ctx->currentClipStencilPlane = _kTpClipStencilPlaneOne;
    _ctx->clippingStackDepth = 0;
    _ctx->projection = tpMat4Identity();

    _tpGLPathPtrArrayInit(&_ctx->paths, 32);
    _tpGLGradientPtrArrayInit(&_ctx->gradients, 8);
    _tpGLStylePtrArrayInit(&_ctx->styles, 8);
    _tpVec2ArrayInit(&_ctx->tmpVertices, 512);
    _tpBoolArrayInit(&_ctx->tmpJoints, 256);
    _tpColorStopArrayInit(&_ctx->tmpColorStops, 16);

    return ret;
}

tpBool tpContextDeallocate(tpContext * _ctx)
{
    //free all opengl resources
    glDeleteProgram(_ctx->program);
    glDeleteBuffers(1, &_ctx->vbo);
    glDeleteVertexArrays(1, &_ctx->vao);
    glDeleteProgram(_ctx->textureProgram);
    glDeleteVertexArrays(1, &_ctx->textureVao);

    // deallocate all gradients
    for (int i = 0; i < _ctx->gradients.count; ++i)
    {
        tpGradientDestroy((tpGradient) {_tpGLGradientPtrArrayAt(&_ctx->gradients, i)});
    }
    _tpGLGradientPtrArrayDeallocate(&_ctx->gradients);

    // deallocate all styles
    for (int i = 0; i < _ctx->styles.count; ++i)
    {
        tpStyleDestroy((tpStyle) {_tpGLStylePtrArrayAt(&_ctx->styles, i)});
    }
    _tpGLStylePtrArrayDeallocate(&_ctx->styles);

    // deallocate all paths
    for (int i = 0; i < _ctx->paths.count; ++i)
    {
        tpPathDestroy((tpPath) {_tpGLPathPtrArrayAt(&_ctx->paths, i)});
    }
    _tpGLPathPtrArrayDeallocate(&_ctx->paths);

    _tpBoolArrayDeallocate(&_ctx->tmpJoints);
    _tpVec2ArrayDeallocate(&_ctx->tmpVertices);
    _tpColorStopArrayDeallocate(&_ctx->tmpColorStops);

    return tpFalse;
}

tpBool _tpGLIsValidPath(_tpGLPath * _path)
{
    return _tpGLPathPtrArrayFind(&_path->context->paths, _path) != -1;
}

tpBool _tpGLIsValidStyle(_tpGLStyle * _style)
{
    return _tpGLStylePtrArrayFind(&_style->context->styles, _style) != -1;
}

tpBool _tpGLIsValidGradient(_tpGLGradient * _gradient)
{
    return _tpGLGradientPtrArrayFind(&_gradient->context->gradients, _gradient) != -1;
}

tpPath tpPathCreate(tpContext * _ctx)
{
    _tpGLPath * path = malloc(sizeof(_tpGLPath));
    _tpGLContourArrayInit(&path->contours, 4);
    path->currentContourIndex = -1;
    memset(path->errorMessage, 0, sizeof(path->errorMessage));
    path->transform = tpMat3Identity();

    _tpVec2ArrayInit(&path->geometryCache, 128);
    _tpBoolArrayInit(&path->jointCache, 128);
    path->bPathGeometryDirty = tpTrue;
    path->lastTransformScale = 1.0;
    path->renderTransform = tpMat4Identity();

    path->strokeVertexOffset = 0;
    path->strokeVertexCount = 0;

    path->fillPaintVertexOffset = 0;
    path->fillPaintVertexCount = 0;

    path->boundsVertexOffset = 0;

    path->lastFillGradient = NULL;
    path->lastStrokeGradient = NULL;

    path->context = _ctx;
    _tpGLPathPtrArrayAppend(&_ctx->paths, path);

    return (tpPath) {path};
}

void tpPathDestroy(tpPath _path)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    assert(_tpGLIsValidPath(p));

    _tpGLPathPtrArrayRemoveValue(&p->context->paths, p);

    _tpVec2ArrayDeallocate(&p->geometryCache);
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

tpBool tpPathAddSegment(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    assert(_tpGLIsValidPath(p));
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

tpBool tpPathLineTo(tpPath _path, tpFloat _x, tpFloat _y)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    assert(_tpGLIsValidPath(p));
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c || c->lastSegmentIndex == -1)
    {
        strcpy(p->errorMessage, "You have to start a contour before issuing this command (see tpPathMoveTo).");
        return tpTrue;
    }

    return _tpGLContourAddSegment(p, c, _x, _y, _x, _y, _x, _y);
}

tpBool tpPathMoveTo(tpPath _path, tpFloat _x, tpFloat _y)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    assert(_tpGLIsValidPath(p));

    _tpGLContour * c = _tpGLPathNextEmptyContour(p);

    return _tpGLContourAddSegment(p, c, _x, _y, _x, _y, _x, _y);
}

tpBool tpPathClear(tpPath _path)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    assert(_tpGLIsValidPath(p));
    p->currentContourIndex = -1;
    for (int i = 0; i < p->contours.count; ++i)
    {
        _tpSegmentArrayDeallocate(&_tpGLContourArrayAtPtr(&p->contours, i)->segments);
    }
    _tpGLContourArrayClear(&p->contours);
    p->bPathGeometryDirty = tpTrue;

    return tpFalse;
}

tpBool tpPathCubicCurveTo(tpPath _path, tpFloat _h0x, tpFloat _h0y, tpFloat _h1x, tpFloat _h1y, tpFloat _px, tpFloat _py)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    assert(_tpGLIsValidPath(p));
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c || c->lastSegmentIndex == -1)
    {
        strcpy(p->errorMessage, "You have to start a contour before issuing this command (see tpPathMoveTo).");
        return tpTrue;
    }

    _tpSegmentArrayAtPtr(&c->segments, c->lastSegmentIndex)->handleOut = (tpVec2) {_h0x, _h0y};
    return _tpGLContourAddSegment(p, c, _h1x, _h1y, _px, _py, _px, _py);
}

tpBool tpPathQuadraticCurveTo(tpPath _path, tpFloat _hx, tpFloat _hy, tpFloat _px, tpFloat _py)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    assert(_tpGLIsValidPath(p));
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c || c->lastSegmentIndex == -1)
    {
        strcpy(p->errorMessage, "You have to start a contour before issuing this command (see tpPathMoveTo).");
        return tpTrue;
    }

    _tpSegmentArrayAtPtr(&c->segments, c->lastSegmentIndex)->handleOut = (tpVec2) {_hx, _hy};
    return _tpGLContourAddSegment(p, c, _hx, _hy, _px, _py, _px, _py);
}

tpBool tpPathClose(tpPath _path)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    assert(_tpGLIsValidPath(p));
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

tpBool tpPathRemoveContour(tpPath _path, int _index)
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

tpBool tpPathAddSegments(tpPath _path, tpSegment * _segments, int _count)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    assert(_tpGLIsValidPath(p));
    _tpGLContour * c = _tpGLCurrentContour(p);
    if (!c)
    {
        c = _tpGLPathCreateNextContour(p);
        assert(c);
    }

    return _tpGLPathAddSegmentsToCurrentContour(p, c, _segments, _count);
}

tpBool tpPathAddContour(tpPath _path, tpSegment * _segments, int _count, tpBool _bClosed)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    assert(_tpGLIsValidPath(p));
    _tpGLContour * c = _tpGLPathCreateNextContour(p);
    assert(c);

    tpBool ret = _tpGLPathAddSegmentsToCurrentContour(p, c, _segments, _count);
    if (ret) return ret;

    if (_bClosed)
        return tpPathClose(_path);
    return tpFalse;
}

tpBool tpPathAddCircle(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _r)
{
    tpFloat dr = _r * 2.0;
    return tpPathAddEllipse(_path, _x, _y, dr, dr);
}

tpBool tpPathAddEllipse(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height)
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
    assert(_tpGLIsValidPath(p));

    _tpGLContour * c = _tpGLPathNextEmptyContour(p);

    rw = _width * 0.5;
    rh = _height * 0.5;
    for (int i = 0; i < 4; ++i)
    {
        int kappaIdx = i * 3;

        printf("%f %f\n", s_unitSegments[kappaIdx].x, s_unitSegments[kappaIdx].y);
        printf("%f %f\n", s_unitSegments[kappaIdx + 1].x, s_unitSegments[kappaIdx + 1].y);
        printf("%f %f\n", s_unitSegments[kappaIdx + 2].x, s_unitSegments[kappaIdx + 2].y);

        printf("COCK %f\n", s_unitSegments[kappaIdx + 1].x * rw + _x);
        px = s_unitSegments[kappaIdx].x * rw + _x;
        py = s_unitSegments[kappaIdx].y * rh + _y;
        segs[i] = tpSegmentMake(s_unitSegments[kappaIdx + 1].x * rw + px, s_unitSegments[kappaIdx + 1].y * rh + py,
                                px, py,
                                s_unitSegments[kappaIdx + 2].x * rw + px, s_unitSegments[kappaIdx + 2].y * rh + py);

        printf("HANDLE IN %f %f\n", segs[i].handleIn.x, segs[i].handleIn.y);
        printf("POS %f %f\n", segs[i].position.x, segs[i].position.y);
        printf("HANDLE Out %f %f\n", segs[i].handleOut.x, segs[i].handleOut.y);
    }

    tpBool err = _tpGLContourAddSegments(p, c, segs, 4);
    if (err) return err;

    tpPathClose(_path);

    return tpFalse;
}

tpBool tpPathAddRect(tpPath _path, tpFloat _x, tpFloat _y, tpFloat _width, tpFloat _height)
{
    tpSegment segs[4];
    tpFloat tmpa, tmpb;

    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    assert(_tpGLIsValidPath(p));

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

tpBool tpPathSetTransform(tpPath _path, const tpMat3 * _transform)
{
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    assert(_tpGLIsValidPath(p));
    if (!tpMat3Equals(&p->transform, _transform))
    {
        tpVec2 scale, skew, translation;
        tpFloat rotation;
        tpMat3Decompose(_transform, &translation, &scale, &skew, &rotation);
        if (p->lastTransformScale < scale.x || p->lastTransformScale < scale.y)
        {
            p->lastTransformScale = TARP_MAX(scale.x, scale.y);
            p->bPathGeometryDirty = tpTrue;
            for (int i = 0; i < p->contours.count; ++i)
            {
                _tpGLContourArrayAtPtr(&p->contours, i)->bDirty = tpTrue;
            }
        }
        p->renderTransform = tpMat4MakeFrom2DTransform(_transform);
    }

    return tpFalse;
}

tpStyle tpStyleCreate(tpContext * _ctx)
{
    _tpGLStyle * style = malloc(sizeof(_tpGLStyle));

    style->fill.data.color = tpColorMake(1, 1, 1, 1);
    style->fill.type = kTpPaintTypeColor;
    style->stroke.data.color = tpColorMake(0, 0, 0, 1);
    style->stroke.type = kTpPaintTypeColor;
    style->strokeWidth = 1.0;
    style->strokeJoin = kTpStrokeJoinBevel;
    style->strokeCap = kTpStrokeCapButt;
    style->fillType = kTpFillTypeEvenOdd;
    style->dashCount = 0;
    style->dashOffset = 0;
    style->miterLimit = 4;

    style->context = _ctx;

    _tpGLStylePtrArrayAppend(&_ctx->styles, style);

    return (tpStyle) {style};
}

void tpStyleDestroy(tpStyle _style)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));
    _tpGLStylePtrArrayRemoveValue(&s->context->styles, s);
    free(s);
}

void tpStyleSetDashArray(tpStyle _style, tpFloat * _dashArray, int _count)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));

    assert(_count < TARP_MAX_DASH_ARRAY_SIZE);

    for (int i = 0; i < _count; ++i)
    {
        s->dashArray[i] = _dashArray[i];
    }
    s->dashCount = _count;
}

void tpStyleSetDashOffset(tpStyle _style, tpFloat _offset)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));

    s->dashOffset = _offset;
}

void tpStyleSetFillColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));

    s->fill.data.color = tpColorMake(_r, _g, _b, _a);
    s->fill.type = kTpPaintTypeColor;
}

void tpStyleSetStrokeColor(tpStyle _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));

    s->stroke.data.color = tpColorMake(_r, _g, _b, _a);
    s->stroke.type = kTpPaintTypeColor;
}

void tpStyleSetFillGradient(tpStyle _style, const tpGradient _gradient)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));

    s->fill.data.gradient = _gradient;
    s->fill.type = kTpPaintTypeGradient;
}

void tpStyleSetStrokeGradient(tpStyle _style, const tpGradient _gradient)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));

    s->stroke.data.gradient = _gradient;
    s->stroke.type = kTpPaintTypeGradient;
}

void tpStyleSetStrokeWidth(tpStyle _style, tpFloat _strokeWidth)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));

    s->strokeWidth = _strokeWidth;
}

void tpStyleSetStrokeJoin(tpStyle _style, tpStrokeJoin _join)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));

    s->strokeJoin = _join;
}

void tpStyleSetMiterLimit(tpStyle _style, tpFloat _limit)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));

    s->miterLimit = _limit;
}

void tpStyleSetStrokeCap(tpStyle _style, tpStrokeCap _cap)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));

    s->strokeCap = _cap;
}

void tpStyleSetFillType(tpStyle _style, tpFillType _fillType)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));

    s->fillType = _fillType;
}

tpGradient tpGradientCreateLinear(tpContext * _ctx, tpFloat _x0, tpFloat _y0, tpFloat _x1, tpFloat _y1)
{
    _tpGLGradient * ret = malloc(sizeof(_tpGLGradient));
    ret->type = kTpGradientTypeLinear;
    ret->origin = tpVec2Make(_x0, _y0);
    ret->destination = tpVec2Make(_x1, _y1);
    _tpColorStopArrayInit(&ret->stops, 8);
    ret->context = _ctx;
    ret->bDirty = tpTrue;

    ASSERT_NO_GL_ERROR(glGenTextures(1, &ret->rampTexture));
    ASSERT_NO_GL_ERROR(glBindTexture(GL_TEXTURE_1D, ret->rampTexture));
    ASSERT_NO_GL_ERROR(glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, TARP_GL_RAMP_TEXTURE_SIZE, 0,
                                    GL_RGBA, GL_FLOAT, NULL));
    ASSERT_NO_GL_ERROR(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    ASSERT_NO_GL_ERROR(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    ASSERT_NO_GL_ERROR(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    ASSERT_NO_GL_ERROR(glBindTexture(GL_TEXTURE_1D, 0));

    _tpGLGradientPtrArrayAppend(&_ctx->gradients, ret);

    return (tpGradient) {ret};
}

void tpGradientAddColorStop(tpGradient _gradient, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a, tpFloat _offset)
{
    _tpGLGradient * g = (_tpGLGradient *)_gradient.pointer;
    assert(_tpGLIsValidGradient(g));
    tpColorStop stop = {tpColorMake(_r, _g, _b, _a), _offset};
    _tpColorStopArrayAppendPtr(&g->stops, &stop);
}

void tpGradientDestroy(tpGradient _gradient)
{
    _tpGLGradient * g = (_tpGLGradient *)_gradient.pointer;
    assert(_tpGLIsValidGradient(g));
    _tpColorStopArrayDeallocate(&g->stops);
    _tpGLGradientPtrArrayRemoveValue(&g->context->gradients, g);
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
    printf("MAKE IT ROUND BB\n");
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

    printf("ROUND JOIN\n");

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

    printf("MITER BABY!\n");
    tpVec2 inv = tpVec2MultScalar(_dir1, -1);
    _tpGLIntersectLines(_e0, _dir0, _e1, _dir1, &intersection);

    printf("INTER %f %f\n", intersection.x, intersection.y);

    _tpGLPushQuad(_outVertices, _p, _e0, intersection, _e1);

    // _tpVec2ArrayAppendPtr(_outVertices, (tpVec2 *)_p);
    // _tpVec2ArrayAppendPtr(_outVertices, (tpVec2 *)_e0);
    // _tpVec2ArrayAppendPtr(_outVertices, &intersection);
}

// void _tpGLMiterTip(tpVec2 _pos,
//                    tpVec2 _ep0,
//                    tpVec2 _ed0,
//                    tpVec2 _ep1,
//                    tpVec2 _ed1,
//                    tpFloat _miterLimit,
//                    tpVec2 * _outPos,
//                    tpFloat * _outMiterLen)
// {
//     tpFloat theta, cross, t;

//     theta = _tpGLShortestAngle(_ed0, _ed1);
//     *_outMiterLen = 1.0 / sin(theta / 2.0);

//     if (*_outMiterLen > _miterLimit)
//         return;

//     //compute the intersection
//     cross = _ed0->x * _ed1->y - _ed0->y * _ed1->x;

//     //parallel case
//     if(cross == 0)
//         return;


// }

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
                    printf("MITER A\n");
                    _tpGLMakeJoinMiter(_p, _lePrev, _le, _dir0, _dir1, _cross, _outVertices);
                }
                else
                {
                    printf("MITER B\n");
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

    printf("LKSJGKL %i %i\n", _vertices->count, _joints->count);
    assert(_vertices->count == _joints->count);

    halfSw = _style->strokeWidth * 0.5;

    //generate the stroke geometry for each contour
    for (i = 0; i < _path->contours.count; ++i)
    {
        printf("BLUBB %i\n", i);

        voff = _vertices->count;
        printf("BLUBB1 %i\n", voff);
        c = _tpGLContourArrayAtPtr(&_path->contours, i);
        c->strokeVertexOffset = voff;
        if (c->fillVertexCount <= 1)
        {
            c->strokeVertexCount = 0;
            continue;
        }

        printf("A\n");
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
            printf("B\n");

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
                    printf("CACHING\n");
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
                    printf("MAKING DA JOIIIIIIN \n");
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
                printf("PRE DEAD\n");
                if (_tpBoolArrayAt(_joints, j + 1))
                {
                    printf("MAKING DA LAST JOIN\n");
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
                    printf("DA END CAP!!!!!\n");
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
        printf("FINISHED CONTOUR STRK %i %i\n", c->strokeVertexOffset, c->strokeVertexCount);
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

        printf("DASH STUFF %i %i %f %f %f\n", startDashIndex, bStartDashOn, startDashLen,
               offsetIntoPattern, _style->dashArray[startDashIndex]);
    }

    // printf("DASH STUFF %i %i %f %f %f %f\n", startDashIndex, bStartDashOn, startDashLen,
    //        offsetIntoPattern, off, _style->dashArray[startDashIndex]);

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

        printf("STROK NEXST CON\n");
        printf("=================\n");

        for (j = c->fillVertexOffset; j < c->fillVertexOffset + c->fillVertexCount - 1; ++j)
        {
            bLastSegment = j == c->fillVertexOffset + c->fillVertexCount - 2;

            if (j == c->fillVertexOffset)
            {
                printf("FIRST SEG\n");
            }
            p0 = _tpVec2ArrayAt(_vertices, j);
            p1 = _tpVec2ArrayAt(_vertices, j + 1);
            dir = tpVec2Sub(p1, p0);
            segmentLen = tpVec2Length(dir);
            segmentOff = 0;
            dir.x /= segmentLen; dir.y /= segmentLen;

            perp.x = dir.y * halfSw;
            perp.y = -dir.x * halfSw;
            cross = tpVec2Cross(perp, perpPrev);
            printf("NEXT SEG %f\n", tpVec2Length(dir));

            //check if this is a joint
            if (bOnDash && _tpBoolArrayAt(_joints, j))
            {
                printf("MAKING DA JOIIIIIIN \n");
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
                printf("LEFT %f %i %f\n", left, bOnDash, dashOffset);
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
                        printf("NNEED DA SPECIAL JOIN˜N\n");
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
                    printf("ADD QUAD\n");
                    //add the quad for the current dash on the current segment
                    _tpGLPushQuad(_vertices, le1, le0, re0, re1);
                }

                dashOffset += left;
                segmentOff += left;
                p0 = p1;

                if (dashOffset >= dashLen)
                {
                    printf("FINISH DASH\n");

                    if (bOnDash)
                    {
                        printf("AKSJH %i %i %i\n", bFirstDashMightNeedJoin, bLastSegment, segmentLen - segmentOff <= 0);
                        //dont make cap if the first and last dash of the contour touch and the last dash is not finished.
                        if (!bFirstDashMightNeedJoin || !bLastSegment || segmentLen - segmentOff > 0)
                        {
                            printf("NOO WAYYY CAP\n");
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
                        printf("MAKING LAST DASHED JOIN\n");
                        printf("%f %f %f %i\n", dashOffset, segmentLen, segmentOff, bOnDash);
                        cross = tpVec2Cross(firstPerp, perp);
                        _tpGLMakeJoin(_style->strokeJoin, p1, dir, firstDir,
                                      perp, firstPerp,
                                      le1, re1, firstLe, firstRe, cross,
                                      _style->miterLimit, _vertices);
                    }
                    else
                    {
                        //otherwise we simply add a starting cap to the first dash of the contour...
                        printf("MAKING LAST DASHED CAP\n");
                        tpVec2 tmpDir, tmpPerp;
                        tmpDir = tpVec2MultScalar(firstDir, -1 * halfSw);
                        tmpPerp.x = tmpDir.y;
                        tmpPerp.y = -tmpDir.x;
                        _tpGLMakeCap(_style->strokeCap, p1, tmpDir, tmpPerp, firstRe, firstLe, tpFalse, _vertices);

                    }
                }
                else if (dashOffset > 0 && bOnDash)
                {
                    printf("LAST CAP\n");
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
        printf("CS %i %i\n", c->strokeVertexOffset, c->strokeVertexCount);
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
    printf("STROKE VERTEX OFF %i\n", _tpGLContourArrayAtPtr(&_path->contours, 0)->strokeVertexOffset);

    // cache with what settings the stroke geometry was generated
    _path->lastStroke.strokeType = _style->stroke.type;
    _path->lastStroke.strokeWidth = _style->strokeWidth;
    _path->lastStroke.dashOffset = _style->dashOffset;
    memcpy(_path->lastStroke.dashArray, _style->dashArray, sizeof(tpFloat) * _style->dashCount);
    _path->lastStroke.dashCount = _style->dashCount;
    _path->lastStroke.join = _style->strokeJoin;
    _path->lastStroke.cap = _style->strokeCap;
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

void _tpGLFinalizeColorStops(tpContext * _ctx, _tpGLGradient * _grad)
{
    int i, j;
    tpColorStop * current;
    tpBool bAdd, bHasStartStop, bHasEndStop;
    _tpColorStopArrayClear(&_ctx->tmpColorStops);

    bHasStartStop = tpFalse;
    bHasEndStop = tpFalse;

    //remove duplicates
    for (i = 0; i < _grad->stops.count; ++i)
    {
        bAdd = tpTrue;
        current = _tpColorStopArrayAtPtr(&_grad->stops, i);
        if (current->offset == 0)
            bHasStartStop = tpTrue;
        else if (current->offset == 1)
            bHasEndStop = tpTrue;

        for (j = 0; i < _ctx->tmpColorStops.count; ++j)
        {
            if (current->offset == _tpColorStopArrayAtPtr(&_ctx->tmpColorStops, j)->offset)
            {
                bAdd = tpFalse;
                break;
            }

            if (bAdd && current->offset >= 0 && current->offset <= 1)
                _tpColorStopArrayAppendPtr(&_ctx->tmpColorStops, current);
        }
    }

    // sort from 0 - 1 by offset
    qsort(_ctx->tmpColorStops.array, _ctx->tmpColorStops.count, sizeof(tpColorStop), _tpGLColorStopComp);

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
        //if they are already there, we can simply swap
        _tpColorStopArraySwap(&_grad->stops, &_ctx->tmpColorStops);
    }
}

//this function assumes that the gradients ramp texture is bound
void _tpGLUpdateRampTexture(_tpGLGradient * _grad)
{
    tpColor pixels[TARP_GL_RAMP_TEXTURE_SIZE];
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
        }
        stop1 = stop2;
        xStart = xEnd;
    }

    ASSERT_NO_GL_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    ASSERT_NO_GL_ERROR(glTexSubImage1D(GL_TEXTURE_1D, 0, 0, TARP_GL_RAMP_TEXTURE_SIZE,
                                       GL_RGBA, GL_FLOAT, &pixels[0].r));
}

void _tpGLDrawPaint(tpContext * _ctx, _tpGLPath * _path, const _tpGLPaint * _paint)
{
    if (_paint->type == kTpPaintTypeColor)
    {
        printf("DRAWING COLOR PAINT\n");
        //@TODO: Cache the uniform loc
        ASSERT_NO_GL_ERROR(glUniform4fv(glGetUniformLocation(_ctx->program, "meshColor"), 1, &_paint->data.color.r));
        ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, _path->boundsVertexOffset, 4));
    }
    else if (_paint->type == kTpPaintTypeGradient)
    {
        printf("GOT DA GRADIENT BABY\n");
        _tpGLGradient * grad = (_tpGLGradient *)_paint->data.gradient.pointer;

        //bind the gradients texture
        ASSERT_NO_GL_ERROR(glActiveTexture(GL_TEXTURE0));
        ASSERT_NO_GL_ERROR(glBindTexture(GL_TEXTURE_1D, grad->rampTexture));

        if (grad->type == kTpGradientTypeLinear)
        {
            printf("DRAWING LINEAR GRADIENT \n");
            ASSERT_NO_GL_ERROR(glUseProgram(_ctx->textureProgram));
            ASSERT_NO_GL_ERROR(glBindVertexArray(_ctx->textureVao));
            ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->textureVBO));

            // ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->textureProgram, "transformProjection"), 1, false, !_tp ? _cache.transformProjection.ptr() : _tp->ptr()));

            ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

            ASSERT_NO_GL_ERROR(glUseProgram(_ctx->program));
            ASSERT_NO_GL_ERROR(glBindVertexArray(_ctx->vao));
            ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->vbo));
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

void _tpGLDrawFillEvenOdd(tpContext * _ctx, _tpGLPath * _path, const _tpGLStyle * _style)
{
    ASSERT_NO_GL_ERROR(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
    ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
    ASSERT_NO_GL_ERROR(glStencilFunc(GL_ALWAYS, 0, _kTpClipStencilPlaneOne));
    ASSERT_NO_GL_ERROR(glStencilMask(_kTpFillRasterStencilPlane));
    ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT));

    //_transform ? & (*_transform).v[0] : NULL
    //@TODO: Cache the uniform loc
    // ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->program, "transformProjection"), 1, GL_FALSE, &mat.v[0]));
    printf("COUNT %i\n", _path->geometryCache.count);
    // ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(tpVec2) * _path->geometryCache.count, _path->geometryCache.array, GL_DYNAMIC_DRAW));
    for (int i = 0; i < _path->contours.count; ++i)
    {
        _tpGLContour * c = _tpGLContourArrayAtPtr(&_path->contours, i);
        printf("VC %i %i\n", c->fillVertexOffset, c->fillVertexCount);
        ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_FAN, c->fillVertexOffset, c->fillVertexCount));
    }

    ASSERT_NO_GL_ERROR(glStencilFunc(GL_EQUAL, 255, _kTpFillRasterStencilPlane));
    ASSERT_NO_GL_ERROR(glStencilMask(_kTpFillRasterStencilPlane));
    ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO));
    ASSERT_NO_GL_ERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

    printf("DRAW DA FILL\n");
    _tpGLDrawPaint(_ctx, _path, &_style->fill);
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
        bounds = _path->boundsCache;
        tpFloat adder;
        adder = TARP_MAX(_style->strokeWidth, _style->miterLimit);
        bounds.min.x -= adder;
        bounds.min.y -= adder;
        bounds.max.x += adder;
        bounds.max.y += adder;

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

void _tpGLCacheStyle(_tpGLPath * _path, const _tpGLStyle * _style)
{
    if (_style->fill.type == kTpPaintTypeGradient)
    {
        printf("FINALIZING GRADIENT\n");
        _tpGLGradient * grad = (_tpGLGradient *)_style->fill.data.gradient.pointer;

        // check if we need to update the gradient
        if (grad->bDirty)
        {
            grad->bDirty = tpFalse;
            // ensure that the color stops are valid/complete
            _tpGLFinalizeColorStops(_path->context, grad);
            // update the ramp texture
            _tpGLUpdateRampTexture(grad);
            _path->lastFillGradient = NULL;
        }

        // regenerate the geometry for this path/gradient combo
        if (_path->lastFillGradient != grad)
        {
            tpVec2 vertices[8];
            TexVertex v2[4];
            tpVec2 dir, ndir, perp, nperp, center;
            tpVec2 corners[4];
            tpVec2 tmp, tmp2, tmp3;
            tpVec2 results[8];
            tpFloat len, o, s, left, right, minOffset, maxOffset;
            _tpGLRect * bounds;
            int i;

            bounds = &_path->boundsCache;

            dir = tpVec2Sub(grad->destination, grad->origin);
            len = tpVec2Length(dir);
            ndir.x = dir.x / len;
            ndir.y = dir.y / len;
            perp.x = -dir.y;
            perp.y = dir.x;
            nperp.x = -ndir.y;
            nperp.y = ndir.x;

            center = grad->origin;

            corners[0] = tpVec2Sub(bounds->min, center);
            corners[1].x = bounds->max.x - center.x;
            corners[1].y = bounds->min.y - center.y;
            corners[2].x = bounds->min.x - center.x;
            corners[2].y = bounds->max.y - center.y;
            corners[3] = tpVec2Sub(bounds->max, center);

            for (i = 0; i < 4; ++i)
            {
                o = tpVec2Dot(corners[i], ndir) / len;
                s = tpVec2Dot(corners[i], nperp);

                if (o < minOffset || i == 0) minOffset = o;
                if (o > maxOffset || i == 0) maxOffset = o;

                if (i == 0 || s < left) left = s;
                if (i == 0 || s > right) right = s;
            }

            tmp = tpVec2MultScalar(ndir, minOffset * len);
            tmp2 = tpVec2Add(center, tpVec2MultScalar(nperp, left));
            tmp3 = tpVec2Add(center, tpVec2MultScalar(nperp, right));
            vertices[0] = tpVec2Add(tmp2, tmp);
            vertices[1].x = minOffset;
            vertices[4] = tpVec2Add(tmp3, tmp);
            vertices[5].x = minOffset;
            tmp = tpVec2MultScalar(ndir, maxOffset * len);
            vertices[2] = tpVec2Add(tmp2, tmp);
            vertices[3].x = maxOffset;
            vertices[6] = tpVec2Add(tmp3, tmp);
            vertices[7].x = maxOffset;

            for(int i = 0; i < 8; ++i)
            {
                printf("GRAD CORNER: %f %f\n", vertices[i].x, vertices[i].y);
            }

            _path->fillPaintVertexOffset = _path->geometryCache.count;
            _path->fillPaintVertexCount = 4;


            // vertices[0] = tpVec2Make(0, 0);
            // vertices[1] = tpVec2Make(0, 0);
            // vertices[2] = tpVec2Make(0, 300);
            // vertices[3] = tpVec2Make(0, 0);
            // vertices[4] = tpVec2Make(300, 0);
            // vertices[5] = tpVec2Make(1, 0);
            // vertices[6] = tpVec2Make(300, 300);
            // vertices[7] = tpVec2Make(1, 0);

            v2[0] = (TexVertex){{10, 10}, 0};
            v2[1] = (TexVertex){{10, 500}, 1};
            v2[2] = (TexVertex){{500, 10}, 0};
            v2[3] = (TexVertex){{500, 500}, 1};

            // _tpVec2ArrayAppendArray(&_path->geometryCache, results, 8);
            ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _path->context->textureVBO));
            ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(TexVertex) * 4, v2, GL_DYNAMIC_DRAW));
            ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _path->context->vbo));
        }

        _path->lastFillGradient = grad;
    }

    // if (_style->stroke.type == kTpPaintTypeGradient)
    // {

    // }
}

// void _tpGLUpdateVAO(_tpGLVAO * _vao, void * _data, int _byteCount)
// {
//     //not sure if this buffer orphaning style data upload makes a difference these days anymore. (TEST??)
//     if (_byteCount > _vao->vboSize)
//     {
//         printf("NEW BUFFER DATA\n");
//         ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, _byteCount, _data, GL_DYNAMIC_DRAW));
//         _vao->vboSize = _byteCount;
//     }
//     else
//     {
//         printf("ORPH BUFFER\n");
//         ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, _vao->vboSize, NULL, GL_DYNAMIC_DRAW));
//         ASSERT_NO_GL_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, _byteCount, _data));
//     }
// }

tpBool tpDrawPath(tpContext * _ctx, tpPath _path, const tpStyle _style)
{
    GLint bufferSize, currentSize, i;
    _tpGLContour * c;

    assert(_ctx);
    _tpGLPath * p = (_tpGLPath *)_path.pointer;
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(p && s);
    assert(_tpGLIsValidPath(p));
    assert(_tpGLIsValidStyle(s));

    if (s->fill.type == kTpPaintTypeGradient)
    {
        printf("ITS A GRADIENT BABY\n");
    }

    if (p->bPathGeometryDirty)
    {
        printf("RE BUILD STUFF\n");
        p->bPathGeometryDirty = tpFalse;

        //flatten the path into tmp buffers
        _tpGLRect bounds;
        _tpGLFlattenPath(p, 0.15, &_ctx->tmpVertices, &_ctx->tmpJoints, &bounds);

        //generate and add the stroke geometry to the tmp buffers
        if (s->stroke.type != kTpPaintTypeNone && s->strokeWidth > 0)
            _tpGLStroke(p, s, &_ctx->tmpVertices, &_ctx->tmpJoints);

        //swap the tmp buffers with the path caches
        _tpVec2ArrayClear(&p->geometryCache);
        _tpBoolArrayClear(&p->jointCache);
        _tpVec2ArraySwap(&p->geometryCache, &_ctx->tmpVertices);
        _tpBoolArraySwap(&p->jointCache, &_ctx->tmpJoints);

        //cache the path bounds
        p->boundsCache = bounds;

        //add the bounds geometry to the geom cache.
        _tpGLCacheBoundsGeometry(p, s);

        //cache the style related geometry
        p->lastFillGradient = NULL;
        p->lastStrokeGradient = NULL;
        _tpGLCacheStyle(p, s);
    }
    //check if the stroke should be removed
    else if ((s->stroke.type == kTpPaintTypeNone &&
              p->lastStroke.strokeType != kTpPaintTypeNone) ||
             (s->strokeWidth == 0 && p->lastStroke.strokeWidth > 0))
    {
        printf("REMOVE STROKE\n");

        p->lastStroke.strokeType = s->stroke.type;
        p->lastStroke.strokeWidth = 0;
        p->strokeVertexOffset = 0;
        p->strokeVertexCount = 0;
    }
    //check if the stroke needs to be regenerated (due to a change in stroke width or dash related settings)
    else if (s->stroke.type != kTpPaintTypeNone && s->strokeWidth > 0 &&
             (p->lastStroke.strokeWidth != s->strokeWidth ||
              p->lastStroke.cap != s->strokeCap ||
              p->lastStroke.join != s->strokeJoin ||
              p->lastStroke.dashCount != s->dashCount ||
              p->lastStroke.dashOffset != s->dashOffset ||
              memcmp(p->lastStroke.dashArray, s->dashArray, sizeof(tpFloat) * s->dashCount) != 0))
    {
        printf("UPDATE STROKE\n");
        //remove all the old stoke vertices from the cache
        _tpVec2ArrayRemoveRange(&p->geometryCache, p->strokeVertexOffset, p->geometryCache.count);

        //generate and add the stroke geometry to the cache.
        _tpGLStroke(p, s, &p->geometryCache, &p->jointCache);

        //add the bounds geometry to the geom cache.
        _tpGLCacheBoundsGeometry(p, s);
    }

    //cache potential extra geometry for the style (only does anything for gradient fills/strokes right now)
    {

    }

    ASSERT_NO_GL_ERROR(glActiveTexture(GL_TEXTURE0));

    ASSERT_NO_GL_ERROR(glDisable(GL_DEPTH_TEST));
    ASSERT_NO_GL_ERROR(glDepthMask(GL_FALSE));
    ASSERT_NO_GL_ERROR(glEnable(GL_MULTISAMPLE));
    ASSERT_NO_GL_ERROR(glEnable(GL_BLEND));
    ASSERT_NO_GL_ERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    ASSERT_NO_GL_ERROR(glEnable(GL_STENCIL_TEST));
    ASSERT_NO_GL_ERROR(glStencilMask(_kTpFillRasterStencilPlane));
    ASSERT_NO_GL_ERROR(glClearStencil(0));
    ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));
    ASSERT_NO_GL_ERROR(glStencilMask(_kTpClipStencilPlaneOne | _kTpClipStencilPlaneTwo | _kTpStrokeRasterStencilPlane));
    ASSERT_NO_GL_ERROR(glClearStencil(255));
    ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));


    ASSERT_NO_GL_ERROR(glUseProgram(_ctx->textureProgram));

    ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->textureProgram, "transform"), 1, GL_FALSE, &p->renderTransform.v[0]));
    ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->textureProgram, "projection"), 1, GL_FALSE, &_ctx->projection.v[0]));
    ASSERT_NO_GL_ERROR(glUniform1i(glGetUniformLocation(_ctx->program, "tex"), 0));

    ASSERT_NO_GL_ERROR(glUseProgram(_ctx->program));

    //@TODO: Cache the uniform loc
    ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->program, "transform"), 1, GL_FALSE, &p->renderTransform.v[0]));
    ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->textureProgram, "projection"), 1, GL_FALSE, &_ctx->projection.v[0]));
    ASSERT_NO_GL_ERROR(glBindVertexArray(_ctx->vao));
    ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->vbo));
    // ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(0));

    // ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(1));
    // ASSERT_NO_GL_ERROR(glUniform4f(glGetUniformLocation(_ctx->program, "meshColor"), 1.0, 0.0, 0.0, 1.0));

    //upload the paths geometry cache to the gpu
    // _tpGLUpdateVAO(&_ctx->vao, p->geometryCache.array, sizeof(tpVec2) * p->geometryCache.count);

    currentSize = sizeof(tpVec2) * p->geometryCache.count;
    //not sure if this buffer orphaning style data upload makes a difference these days anymore. (TEST??)
    if (currentSize > _ctx->vboSize)
    {
        printf("NEW BUFFER DATA\n");
        ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, currentSize, p->geometryCache.array, GL_DYNAMIC_DRAW));
        _ctx->vboSize = currentSize;
    }
    else
    {
        printf("ORPH BUFFER\n");
        ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, _ctx->vboSize, NULL, GL_DYNAMIC_DRAW));
        ASSERT_NO_GL_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, currentSize, p->geometryCache.array));
    }


    //draw the fill
    _tpGLDrawFillEvenOdd(_ctx, p, s);

    //draw the stroke
    ASSERT_NO_GL_ERROR(glStencilMask(_kTpStrokeRasterStencilPlane));
    ASSERT_NO_GL_ERROR(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
    ASSERT_NO_GL_ERROR(glStencilFunc(GL_ALWAYS, 0, _kTpClipStencilPlaneOne));
    ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

    //Draw all stroke triangles at once
    ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLES, p->strokeVertexOffset, p->strokeVertexCount));
    // for(i = 0; i < p->contours.count; ++i)
    // {
    //     c = _tpGLContourArrayAtPtr(&p->contours, i);
    //     ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLES, c->strokeVertexOffset, c->strokeVertexCount));
    // }

    ASSERT_NO_GL_ERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    ASSERT_NO_GL_ERROR(glStencilFunc(GL_EQUAL, 0, _kTpStrokeRasterStencilPlane));
    ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT));

    _tpGLDrawPaint(_ctx, p, &s->stroke);

    return tpFalse;
}

tpBool tpSetProjection(tpContext * _ctx, const tpMat4 * _projection)
{
    _ctx->projection = *_projection;
    return tpFalse;
}

#endif //TARP_TARPGL_H
