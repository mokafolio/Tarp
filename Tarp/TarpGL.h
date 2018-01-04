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
    "pixelColor = texture(tex, itc); \n"
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
} _tpGLContour;

#define _TARP_ARRAY_T _tpGLContourArray
#define _TARP_ITEM_T _tpGLContour
#define _TARP_COMPARATOR_T 0
#include <Tarp/TarpArray.h>

typedef struct
{
    _tpGLContourArray contours;
    int currentContourIndex;
    char errorMessage[TARP_GL_ERROR_MESSAGE_SIZE];
    tpMat3 transform;

    //rendering specific data/caches
    _tpVec2Array geometryCache;
    _tpVec2Array strokeGeometryCache;
    _tpBoolArray jointCache;

    tpBool bGeometryCacheDirty;
    tpFloat lastTransformScale;
    tpMat4 renderTransform;
    _tpGLRect boundsCache;

    //holds a pointer to the context that owns this path
    tpContext * context;
} _tpGLPath;

typedef struct
{
    tpVec2 origin;
    tpVec2 destination;
    tpColorStop stops[TARP_MAX_COLOR_STOPS];
    int stopCount;
    tpGradientType type;
    tpContext * context;
    //rendering specific data/caches
} _tpGLGradient;

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

struct _tpContextData
{
    GLuint program;
    GLuint textureProgram;
    GLuint vao;
    GLuint textureVao;
    GLuint vbo;
    GLuint vboSize;
    GLuint textureVbo;
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

    ASSERT_NO_GL_ERROR(glGenVertexArrays(1, &_ctx->vao));
    ASSERT_NO_GL_ERROR(glGenBuffers(1, &_ctx->vbo));
    ASSERT_NO_GL_ERROR(glBindVertexArray(_ctx->vao));
    ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->vbo));
    ASSERT_NO_GL_ERROR(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), ((char *)0)));

    ASSERT_NO_GL_ERROR(glGenVertexArrays(1, &_ctx->textureVao));
    ASSERT_NO_GL_ERROR(glGenBuffers(1, &_ctx->textureVbo));
    ASSERT_NO_GL_ERROR(glBindVertexArray(_ctx->textureVao));
    ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->textureVbo));
    ASSERT_NO_GL_ERROR(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), ((char *)0)));
    ASSERT_NO_GL_ERROR(glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), ((char *)(2 * sizeof(float)))));

    _ctx->currentClipStencilPlane = _kTpClipStencilPlaneOne;
    _ctx->clippingStackDepth = 0;
    _ctx->projection = tpMat4Identity();
    _ctx->vboSize = 0;

    _tpGLPathPtrArrayInit(&_ctx->paths, 32);
    _tpVec2ArrayInit(&_ctx->tmpVertices, 512);
    _tpBoolArrayInit(&_ctx->tmpJoints, 256);

    return ret;
}

tpBool tpContextDeallocate(tpContext * _ctx)
{
    //free all opengl resources
    glDeleteProgram(_ctx->program);
    glDeleteBuffers(1, &_ctx->vbo);
    glDeleteVertexArrays(1, &_ctx->vao);
    glDeleteProgram(_ctx->textureProgram);
    glDeleteBuffers(1, &_ctx->textureVbo);
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
    _tpVec2ArrayInit(&path->strokeGeometryCache, 256);
    _tpBoolArrayInit(&path->jointCache, 128);
    path->bGeometryCacheDirty = tpTrue;
    path->lastTransformScale = 1.0;
    path->renderTransform = tpMat4Identity();

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
    _tpVec2ArrayDeallocate(&p->strokeGeometryCache);
    _tpBoolArrayDeallocate(&p->jointCache);
    for (int i = 0; i < p->contours.count; ++i)
    {
        _tpSegmentArrayDeallocate(&_tpGLContourArrayAtPtr(&p->contours, i)->segments);
    }
    _tpGLContourArrayDeallocate(&p->contours);
    free(p);
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

_tpGLContour * _tpGLCurrentContour(_tpGLPath * _p)
{
    if (_p->currentContourIndex != -1)
        return _tpGLContourArrayAtPtr(&_p->contours, _p->currentContourIndex);
    return NULL;
}

tpBool _tpGLContourAddSegment(_tpGLPath * _p, _tpGLContour * _c,
                              tpFloat _h0x, tpFloat _h0y,
                              tpFloat _px, tpFloat _py,
                              tpFloat _h1x, tpFloat _h1y)
{
    _c->lastSegmentIndex = _c->segments.count;
    _c->bDirty = tpTrue;
    _p->bGeometryCacheDirty = tpTrue;
    int err = _tpSegmentArrayAppend(&_c->segments, (tpSegment)
    {
        {_h0x, _h0y},
        {_px, _py},
        {_h1x, _h1y}
    });
    if (err)
    {
        strcpy(_p->errorMessage, "Could not allocate memory for segments.");
        return tpTrue;
    }
    return tpFalse;
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
    _tpGLContour * c = _tpGLCurrentContour(p);

    if (!c || c->segments.count)
    {
        c = _tpGLPathCreateNextContour(p);
    }

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
    p->bGeometryCacheDirty = tpTrue;

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
        p->bGeometryCacheDirty = tpTrue;
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
    _p->bGeometryCacheDirty = tpTrue;
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
            p->bGeometryCacheDirty = tpTrue;
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

void tpStyleSetDash(tpStyle _style, tpFloat * _dashArray, int _count, tpFloat _offset)
{
    _tpGLStyle * s = (_tpGLStyle *)_style.pointer;
    assert(_tpGLIsValidStyle(s));

    assert(_count < TARP_MAX_DASH_ARRAY_SIZE);

    s->dashOffset = _offset;
    for (int i = 0; i < _count; ++i)
    {
        s->dashArray[i] = _dashArray[i];
    }
    s->dashCount = _count;
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
    ret->stopCount = 0;
    ret->context = _ctx;

    _tpGLGradientPtrArrayAppend(&_ctx->gradients, ret);

    return (tpGradient) {ret};
}

void tpGradientAddColorStop(tpGradient _gradient, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a, tpFloat _offset)
{
    _tpGLGradient * g = (_tpGLGradient *)_gradient.pointer;
    assert(_tpGLIsValidGradient(g));
    g->stops[g->stopCount++] = (tpColorStop) {tpColorMake(_r, _g, _b, _a), _offset};
}

void tpGradientDestroy(tpGradient _gradient)
{
    _tpGLGradient * g = (_tpGLGradient *)_gradient.pointer;
    assert(_tpGLIsValidGradient(g));
    _tpGLGradientPtrArrayRemoveValue(&g->context->gradients, g);
    free(g);
}

int _absolute_tolerance_compare(tpFloat _x, tpFloat _y, tpFloat _epsilon)
{
    return fabs(_x - _y) <= _epsilon;
}

int _relative_tolerance_compare(tpFloat _x, tpFloat _y, tpFloat _epsilon)
{
    tpFloat maxXY = TARP_MAX(fabs(_x) , fabs(_y));
    return fabs(_x - _y) <= _epsilon * maxXY;
}

int _combined_tolerance_compare(tpFloat _x, tpFloat _y, tpFloat _epsilon)
{
    tpFloat maxXYOne = TARP_MAX(1.0, TARP_MAX(fabs(_x), fabs(_y)));
    return fabs(_x - _y) <= _epsilon * maxXYOne;
}

int _is_close(tpFloat _a, tpFloat _b, tpFloat _epsilon)
{
    return _combined_tolerance_compare(_a, _b, _epsilon);
}

int _is_linear(const _tpGLCurve * _curve)
{
    return _is_close(_curve->p0.x, _curve->h0.x, FLT_EPSILON) && _is_close(_curve->p0.y, _curve->h0.y, FLT_EPSILON) &&
           _is_close(_curve->p1.x, _curve->h1.x, FLT_EPSILON) && _is_close(_curve->p1.y, _curve->h1.y, FLT_EPSILON);
}

int _is_flat_enough(const _tpGLCurve * _curve, tpFloat _tolerance)
{
    if (_is_linear(_curve))
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

void _subdivide_curve(const _tpGLCurve * _curve,
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

void _evaluate_point_for_bounds(const tpVec2 * _vec, _tpGLRect * _bounds)
{
    _bounds->min.x = TARP_MIN(_vec->x, _bounds->min.x);
    _bounds->min.y = TARP_MIN(_vec->y, _bounds->min.y);
    _bounds->max.x = TARP_MAX(_vec->x, _bounds->max.x);
    _bounds->max.y = TARP_MAX(_vec->y, _bounds->max.y);
}

tpFloat _tpGLShortestAngle(const tpVec2 * _d0, const tpVec2 * _d1)
{
    tpFloat theta = acos(_d0->x * _d1->x + _d0->y * _d1->y);

    //make sure we have the shortest angle
    if (theta > TARP_HALF_PI)
        theta = TARP_PI - theta;

    return theta;
}

void _tpGLMakeCapOrJoinRound(const tpVec2 * _p, const tpVec2 * _d, tpFloat _theta, _tpVec2Array * _outVertices)
{
    tpFloat cosa, sina;
    tpVec2 perp, last, current;
    int circleSubdivisionCount, i;
    tpFloat currentAngle, radStep;

    perp.x = -_d->y;
    perp.y = _d->x;

    //@TODO: Make this based on the stroke width and theta?
    circleSubdivisionCount = 12;
    currentAngle = 0;
    radStep = _theta / (tpFloat)(circleSubdivisionCount);

    for (i = 0; i <= circleSubdivisionCount; ++i)
    {
        currentAngle = radStep * i;

        cosa = cos(-currentAngle);
        sina = sin(-currentAngle);

        current.x = _p->x + perp.x * cosa - perp.y * sina;
        current.y = _p->y + perp.x * sina - perp.y * cosa;

        if (i > 0)
        {
            _tpVec2ArrayAppendPtr(_outVertices, &last);
            _tpVec2ArrayAppendPtr(_outVertices, &current);
            _tpVec2ArrayAppendPtr(_outVertices, _p);
        }

        last = current;
    }
}

void _tpGLMakeCapSquare(const tpVec2 * _p, const tpVec2 * _d, tpFloat _theta, tpBool _bStart, _tpVec2Array * _outVertices)
{
    tpVec2 dir, perp, a, b, c, d;

    dir = *_d;

    if (_bStart)
    {
        dir.x = -dir.x;
        dir.y = -dir.y;
    }

    perp.x = -dir.y;
    perp.y = dir.x;
    a = tpVec2Add(_p, &perp);
    b = tpVec2Sub(_p, &perp);
    c = tpVec2Add(&b, &dir);
    d = tpVec2Add(&a, &dir);

    //this makes sure that the triangles are counter clockwise
    if (_bStart)
    {
        _tpVec2ArrayAppendPtr(_outVertices, &d);
        _tpVec2ArrayAppendPtr(_outVertices, &b);
        _tpVec2ArrayAppendPtr(_outVertices, &a);
        _tpVec2ArrayAppendPtr(_outVertices, &b);
        _tpVec2ArrayAppendPtr(_outVertices, &d);
        _tpVec2ArrayAppendPtr(_outVertices, &c);
    }
    else
    {
        _tpVec2ArrayAppendPtr(_outVertices, &a);
        _tpVec2ArrayAppendPtr(_outVertices, &b);
        _tpVec2ArrayAppendPtr(_outVertices, &d);
        _tpVec2ArrayAppendPtr(_outVertices, &b);
        _tpVec2ArrayAppendPtr(_outVertices, &a);
        _tpVec2ArrayAppendPtr(_outVertices, &d);
    }
}

void _tpGLMakeJoinBevel(const tpVec2 * _lePrev, const tpVec2 * _rePrev,
                        const tpVec2 * _le, const tpVec2 * _re,
                        tpFloat _cross, _tpVec2Array * _outVertices)
{
    if (_cross < 0)
    {
        _tpVec2ArrayAppendPtr(_outVertices, _lePrev);
        _tpVec2ArrayAppendPtr(_outVertices, _le);
        _tpVec2ArrayAppendPtr(_outVertices, _rePrev);
    }
    else
    {
        _tpVec2ArrayAppendPtr(_outVertices, _re);
        _tpVec2ArrayAppendPtr(_outVertices, _rePrev);
        _tpVec2ArrayAppendPtr(_outVertices, _lePrev);
    }
}

// void _tpGLMiterTip(const tpVec2 * _pos,
//                    const tpVec2 * _ep0,
//                    const tpVec2 * _ed0,
//                    const tpVec2 * _ep1,
//                    const tpVec2 * _ed1,
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

// void _tpGLMakeJoinMiter(const tpVec2 * _p,
//                         const tpVec2 * _d0, const tpVec2 * _d1,
//                         const tpVec2 * _ep0, const tpVec2 * _ep1,
//                         const tpVec2 * _ep2, const tpVec2 * _ep3,
//                         _tpVec2Array * _outVertices)
// {
//     tpFloat cross, miterLen;
//     tpVec2 miterTip;

//     //to know which side of the stroke is outside
//     cross = _lastDir.x * _dir.y - _dir.x * _lastDir.y;

//     //compute the miter
//     if (cross >= 0.0)
//         miterTip = detail::joinMiter(_point, _lastLeftEdgePoint, _lastDir, _leftEdgePoint, _dir, miterLen);
//     else
//         miterTip = detail::joinMiter(_point, _lastRightEdgePoint, _lastDir, _rightEdgePoint, _dir, miterLen);
// }

void _tpGLPushQuad(_tpVec2Array * _vertices, tpVec2 * _a, tpVec2 * _b, tpVec2 * _c, tpVec2 * _d)
{
    _tpVec2ArrayAppendPtr(_vertices, _a);
    _tpVec2ArrayAppendPtr(_vertices, _b);
    _tpVec2ArrayAppendPtr(_vertices, _c);
    _tpVec2ArrayAppendPtr(_vertices, _c);
    _tpVec2ArrayAppendPtr(_vertices, _d);
    _tpVec2ArrayAppendPtr(_vertices, _a);
}

void _tpGLMakeJoin(tpStrokeJoin _type,
                   const tpVec2 * _p,
                   const tpVec2 * _d0, const tpVec2 * _d1,
                   const tpVec2 * _lePrev, const tpVec2 * _rePrev,
                   const tpVec2 * _le, const tpVec2 * _re,
                   tpFloat _cross,
                   _tpVec2Array * _outVertices)
{
    switch (_type)
    {
        case kTpStrokeJoinBevel:
            _tpGLMakeJoinBevel(_lePrev, _rePrev, _le, _re, _cross, _outVertices);
            break;
        default: break;
    }
}

tpBool _tpGLStrokeGeometry(_tpGLPath * _path, const _tpGLStyle * _style, _tpVec2Array * _vertices, _tpBoolArray * _joints)
{
    int i, j, voff;
    tpBool bIsClosed;
    _tpGLContour * c;
    tpVec2 p0, p1, dir, perp, dirPrev, perpPrev;
    tpVec2 le0, le1, re0, re1, edgeDelta, lePrev, rePrev;
    tpVec2 firstDir, firstPerp, firstLe, firstRe;
    tpFloat cross;

    printf("LKSJGKL %i %i\n", _vertices->count, _joints->count);
    assert(_vertices->count == _joints->count);

    //generate the stroke geometry for each contour
    for (i = 0; i < _path->contours.count; ++i)
    {
        voff = _vertices->count;
        c = _tpGLContourArrayAtPtr(&_path->contours, i);
        c->strokeVertexOffset = voff;

        for (j = c->fillVertexOffset; j < c->fillVertexOffset + c->fillVertexCount - 1; ++j)
        {
            p0 = _tpVec2ArrayAt(_vertices, j);
            p1 = _tpVec2ArrayAt(_vertices, j + 1);
            dir = tpVec2Sub(&p1, &p0);
            tpVec2NormalizeSelf(&dir);
            perp.x = -dir.y;
            perp.y = dir.x;
            cross = perp.x * perpPrev.y - perp.y * perpPrev.x;
            edgeDelta = tpVec2MultScalar(&perp, _style->strokeWidth * 0.5);

            le0 = tpVec2Sub(&p0, &edgeDelta);
            re0 = tpVec2Add(&p0, &edgeDelta);
            le1 = tpVec2Sub(&p1, &edgeDelta);
            re1 = tpVec2Add(&p1, &edgeDelta);

            //check if this is the first segment as we need to do things
            //slightly differently in that case :)
            if (j == c->fillVertexOffset)
            {
                //start cap?


                //add the quad for the current segment
                _tpGLPushQuad(_vertices, &le1, &le0, &re0, &re1);

                //if the path is closed, we cache the directions and edges
                //to be used for the last segments stroke calculations
                if (c->bIsClosed)
                {
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
                    _tpGLMakeJoin(_style->strokeJoin, &p0, &dirPrev, &dir,
                                  &lePrev, &rePrev, &le0, &re0, cross, _vertices);
                }
                else
                {
                    //by default we join consecutive segment quads with a bevel
                    _tpGLMakeJoinBevel(&lePrev, &rePrev, &le0, &re0, cross, _vertices);
                }

                //add the quad for the current segment
                _tpGLPushQuad(_vertices, &le1, &le0, &re0, &re1);

                //check if we need to do the end cap / join
                if (j == c->fillVertexOffset + c->fillVertexCount - 2)
                {
                    printf("PRE DEAD\n");
                    if (_tpBoolArrayAt(_joints, j + 1))
                    {
                        printf("MAKING DA LAST JOIN\n");
                        //last join
                        cross = firstPerp.x * perp.y - firstPerp.y * perp.x;
                        _tpGLMakeJoin(_style->strokeJoin, &p1, &dir, &firstDir,
                                      &le1, &re1, &firstLe, &firstRe, cross, _vertices);
                    }
                    else
                    {
                        //end cap
                    }
                }
            }

            perpPrev = perp;
            dirPrev = dir;
            lePrev = le1;
            rePrev = re1;
        }

        c->strokeVertexCount = _vertices->count - voff;
    }

    return tpFalse;
}

void _flatten_curve(_tpGLPath * _path,
                    const _tpGLCurve * _curve,
                    tpFloat _angleTolerance,
                    int _bIsClosed,
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

        if (stackIndex < TARP_MAX_CURVE_SUBDIVISIONS - 1 && !_is_flat_enough(current, _angleTolerance))
        {
            //subdivide curve and add continue with the left of the two curves by putting it on top
            //of the stack.
            _subdivide_curve(current, 0.5, &cp);
            *current = cp.second;
            stack[++stackIndex] = cp.first;
        }
        else
        {
            // printf("ADD TO CACHE\n");
            //for the first curve we also add its first segment
            if (!_outVertices->count)
            {
                _tpVec2ArrayAppendPtr(_outVertices, &current->p0);
                _tpVec2ArrayAppendPtr(_outVertices, &current->p1);

                _tpBoolArrayAppend(_outJoints, tpFalse);
                _tpBoolArrayAppend(_outJoints, tpVec2Equals(&current->p1, &_curve->p1) && !_bLastCurve);
                _evaluate_point_for_bounds(&current->p0, _bounds);
                _evaluate_point_for_bounds(&current->p1, _bounds);
                *_vertexCount += 2;
            }
            else
            {
                _tpVec2ArrayAppendPtr(_outVertices, &current->p1);

                _tpBoolArrayAppend(_outJoints, _bIsClosed ? tpVec2Equals(&current->p1, &_curve->p1) :
                                   (tpVec2Equals(&current->p1, &_curve->p1) && !_bLastCurve));
                _evaluate_point_for_bounds(&current->p1, _bounds);
                (*_vertexCount)++;
            }
            stackIndex--;
        }
    }
}

int _flatten_path(_tpGLPath * _path,
                  tpFloat _angleTolerance,
                  _tpVec2Array * _outVertices,
                  _tpBoolArray * _outJoints,
                  _tpGLRect * _outBounds)
{
    printf("====================================\n");
    int i = 0;
    int j = 0;
    _tpGLContour * c = NULL;
    tpSegment * last = NULL, *current = NULL;
    int recursionDepth = 0;
    _outBounds->min.x = FLT_MAX;
    _outBounds->min.y = FLT_MAX;
    _outBounds->max.x = -FLT_MAX;
    _outBounds->max.y = -FLT_MAX;

    printf("CONTOURS %i\n", _path->contours.count);

    int off = 0;
    for (i = 0; i < _path->contours.count; ++i)
    {
        c = _tpGLContourArrayAtPtr(&_path->contours, i);

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
                _tpGLCurve curve = {last->position.x, last->position.y,
                                    last->handleOut.x, last->handleOut.y,
                                    current->handleIn.x, current->handleIn.y,
                                    current->position.x, current->position.y
                                   };

                _flatten_curve(_path,
                               &curve,
                               _angleTolerance,
                               c->bIsClosed,
                               j == c->segments.count - 1,
                               _outVertices,
                               _outJoints,
                               _outBounds, &vcount);


                last = current;
            }

            if (c->bIsClosed && c->segments.count)
            {
                tpSegment * fs = _tpSegmentArrayAtPtr(&c->segments, 0);
                _tpVec2ArrayAppendPtr(_outVertices, &fs->position);
                _tpBoolArrayAppend(_outJoints, tpTrue);
                vcount++;
            }
            c->fillVertexOffset = off;
            c->fillVertexCount = vcount;
            off += vcount;
        }
        else
        {
            //otherwise we just copy the contour to the tmpbuffer
            _tpVec2ArrayAppendArray(_outVertices, _tpVec2ArrayAtPtr(&_path->geometryCache, c->fillVertexOffset), c->fillVertexCount);
            _tpBoolArrayAppendArray(_outJoints, _tpBoolArrayAtPtr(&_path->jointCache, c->fillVertexOffset), c->fillVertexCount);
        }
    }

    printf("EEEEND\n");
    return 0;
}

void _draw_paint(tpContext * _ctx, _tpGLPath * _path, const _tpGLPaint * _paint)
{
    if (_paint->type == kTpPaintTypeColor)
    {
        //@TODO: Cache the uniform loc
        ASSERT_NO_GL_ERROR(glUniform4fv(glGetUniformLocation(_ctx->program, "meshColor"), 1, &_paint->data.color.r));
        ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, (_path->geometryCache.count - 4), 4));
    }
}

void _draw_fill_even_odd(tpContext * _ctx, _tpGLPath * _path, const _tpGLStyle * _style)
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

    _draw_paint(_ctx, _path, &_style->fill);
}

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

    if (p->bGeometryCacheDirty)
    {
        printf("RE BUILD STUFF\n");
        p->bGeometryCacheDirty = tpFalse;

        //flatten the path into tmp buffers
        _tpGLRect bounds;
        _flatten_path(p, 0.15, &_ctx->tmpVertices, &_ctx->tmpJoints, &bounds);

        //generate and add the stroke geometry to the tmp buffers
        _tpGLStrokeGeometry(p, s, &_ctx->tmpVertices, &_ctx->tmpJoints);

        //swap the tmp buffers with the path caches
        _tpVec2ArrayClear(&p->geometryCache);
        _tpBoolArrayClear(&p->jointCache);
        _tpVec2ArraySwap(&p->geometryCache, &_ctx->tmpVertices);
        _tpBoolArraySwap(&p->jointCache, &_ctx->tmpJoints);

        //add the bounds geometry to the end of the geometry cache
        if (s->stroke.type != kTpPaintTypeNone)
        {
            bounds.min.x -= s->strokeWidth;
            bounds.min.y -= s->strokeWidth;
            bounds.max.x += s->strokeWidth;
            bounds.max.y += s->strokeWidth;
        }
        tpVec2 boundsData[] = {bounds.min,
            {bounds.min.x, bounds.max.y},
            {bounds.max.x, bounds.min.y},
            bounds.max
        };

        _tpVec2ArrayAppendArray(&p->geometryCache, boundsData, 4);
    }
    // printf("BOUNDS\n");
    // printf("MIN %f %f\n", _ctx->boundsCache.minX, _ctx->boundsCache.minY);
    // printf("MAX %f %f\n", _ctx->boundsCache.maxX, _ctx->boundsCache.maxY);
    // printf("GEOM CACHE %i\n", ctx->geometryCacheCount);

    // for (int i = 0; i < ctx->geometryCacheCount; ++i)
    // {
    //     printf("CACHE %f\n", ctx->geometryCache[i]);
    // }

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

    ASSERT_NO_GL_ERROR(glUseProgram(_ctx->program));

    //@TODO: Cache the uniform loc
    ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->program, "transform"), 1, GL_FALSE, &p->renderTransform.v[0]));
    ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->program, "projection"), 1, GL_FALSE, &_ctx->projection.v[0]));
    ASSERT_NO_GL_ERROR(glBindVertexArray(_ctx->vao));
    ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _ctx->vbo));
    ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(0));
    // ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(1));
    // ASSERT_NO_GL_ERROR(glUniform4f(glGetUniformLocation(_ctx->program, "meshColor"), 1.0, 0.0, 0.0, 1.0));

    //upload the paths geometry cache to the gpu
    //not sure if this buffer orphaning style data upload makes a difference these days anymore.
    currentSize = sizeof(tpVec2) * p->geometryCache.count;
    if (currentSize > _ctx->vboSize)
    {
        printf("NEW BUFFER DATA\n");
        ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, currentSize, NULL, GL_DYNAMIC_DRAW));
        _ctx->vboSize = currentSize;
    }
    else
    {
        printf("ORPH BUFFER\n");
        ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, _ctx->vboSize, NULL, GL_DYNAMIC_DRAW));
        ASSERT_NO_GL_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, currentSize, p->geometryCache.array));
    }

    //draw the fill
    _draw_fill_even_odd(_ctx, p, s);

    //draw the stroke
    ASSERT_NO_GL_ERROR(glStencilMask(_kTpStrokeRasterStencilPlane));
    ASSERT_NO_GL_ERROR(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
    ASSERT_NO_GL_ERROR(glStencilFunc(GL_ALWAYS, 0, _kTpClipStencilPlaneOne));
    ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

    for (i = 0; i < p->contours.count; ++i)
    {
        c = _tpGLContourArrayAtPtr(&p->contours, i);
        ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLES, c->strokeVertexOffset, c->strokeVertexCount));
    }

    ASSERT_NO_GL_ERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    ASSERT_NO_GL_ERROR(glStencilFunc(GL_EQUAL, 0, _kTpStrokeRasterStencilPlane));
    ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT));

    _draw_paint(_ctx, p, &s->stroke);

    return tpFalse;
}

tpBool tpSetProjection(tpContext * _ctx, const tpMat4 * _projection)
{
    _ctx->projection = *_projection;
    return tpFalse;
}

#endif //TARP_TARPGL_H
