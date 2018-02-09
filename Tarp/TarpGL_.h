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
    "in tpFloat tc; \n"
    "out tpFloat itc;\n"
    "void main() \n"
    "{ \n"
    "gl_Position = projection * transform * vec4(vertex, 0.0, 1.0); \n"
    "itc = tc; \n"
    "} \n";

static const char * _fragmentShaderCodeTexture =
    "#version 150 \n"
    "uniform sampler1D tex;\n"
    "in tpFloat itc; \n"
    "out vec4 pixelColor; \n"
    "void main() \n"
    "{ \n"
    "pixelColor = texture(tex, itc); \n"
    "} \n";

typedef enum
{
    //The mask values are important! FillRaster needs to have at least
    //2 bits. These need to be the lower bits in order to work with the
    //Increment, Decrement stencil operations
    //http://www.opengl.org/discussion_boards/showthread.php/149740-glStencilOp-s-GL_INCR-GL_DECR-behaviour-when-masked
    kFillRasterStencilPlane = 0x1F, //binary mask    00011111
    kClipStencilPlaneOne = 1 << 5, //binary mask     00100000
    kClipStencilPlaneTwo = 1 << 6, //binary mask     01000000
    kStrokeRasterStencilPlane = 1 << 7 //binary mask 10000000
} _tpStencilPlane;

typedef enum
{
    _kPath,
    _kPaint,
    _kStyle,
    _kGradient
} _tpItemType;

typedef struct
{
    char * array;
    int count;
    int capacity;
    int elementSize;
} _DataArray;

// typedef struct
// {
//     char * array;
//     int capacity;
//     int count;
//     int elementSize;
//     int * freeList;
//     int freeListCount;
// } _MemoryChunk;

struct _tpHandleData
{
    int index;
    _tpItemType type;
};

typedef struct
{
    _DataArray segments;
    _DataArray children;
    int bIsClosed;
    tpSegment * lastSegment;
    char lastErrorMessage[512];
    void * _implData;
    tpMat3 transform;

    //rendering specific data/caches
    _DataArray geometryCache;
    _DataArray jointCache;
    _DataArray strokeGeometryCache;
    tpBool bGeometryCacheDirty;
    tpFloat lastTransformScale;
    tpMat4 renderTransform;
} _tpGLPath;

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
} _tpGLStyle;

typedef struct
{
    tpVec2 origin;
    tpVec2 destination;
    tpColorStop stops[TARP_MAX_COLOR_STOPS];
    int stopCount;
    GradientType type;
    void * _implData;
} _tpGLGradient;

typedef union
{
    tpGradient * gradient;
    tpColor color;
} _tpGLPaintUnion;

typedef struct
{
    _tpGLPaintUnion data;
    PaintType type;
} _tpGLPaint;

typedef struct
{
    _tpGLPaint fill;
    _tpGLPaint stroke;
    tpFloat strokeWidth;
    StrokeCap strokeCap;
    StrokeJoin strokeJoin;
    FillType fillType;
    tpFloat dashArray[TARP_MAX_DASH_ARRAY_SIZE];
    int dashCount;
    tpFloat dashOffset;
} _tpGLStyle;

typedef struct
{
    char message[TARP_GL_ERROR_MESSAGE_SIZE];
    int length;
} _ErrorMessage;

typedef struct
{
    tpFloat minX, minY, maxX, maxY;
} _Rect;

// typedef struct
// {
//     _DataArray geometryCache;
//     _DataArray jointCache;
//     _DataArray strokeGeometryCache;
//     tpBool bGeometryCacheDirty;
//     tpFloat lastTransformScale;
//     tpMat4 renderTransform;
// } _GLPathData;

struct _tpContextData
{
    GLuint program;
    GLuint textureProgram;
    GLuint vao;
    GLuint textureVao;
    GLuint vbo;
    GLuint textureVbo;
    GLuint currentClipStencilPlane;
    tpPath clippingStack[TARP_GL_MAX_CLIPPING_STACK_DEPTH];
    int clippingStackDepth;
    tpMat4 projection;
    _MemoryChunk paths;
};

typedef struct
{
    tpFloat x, y;
} _tpGLVertex;

typedef struct
{
    tpFloat x, y, s;
} _tpGLTextureVertex;

typedef struct
{
    tpFloat x0, y0, h0x, h0y, h1x, h1y, x1, y1;
} _tpGLCurve;

typedef struct
{
    _Curve first, second;
} _tpGLCurvePair;

void _data_array_init(_DataArray * _array, int _capacity, int _elementSize)
{
    _array->array = malloc(_elementSize * _capacity);
    assert(_array->array);
    _array->capacity = _capacity;
    _array->count = 0;
    _array->elementSize = _elementSize;
}

void _data_array_deallocate(_DataArray * _array)
{
    if (_array->array)
    {
        free(_array->array);
        _array->capacity = 0;
        _array->count = 0;
        _array->elementSize = 0;
    }
}

void _data_array_append_array(_DataArray * _array, void * _elements, int _count)
{
    assert(_array->array);
    // printf("APPEND %i %i %i\n", _array->capacity, _array->count, _count);
    if (_array->capacity - _array->count < _count)
    {
        int c = _array->capacity * 2;
        _array->array = realloc(_array->array, c * _array->elementSize);
        assert(_array->array);
        _array->capacity = c;
    }
    memcpy(_array->array + _array->count * _array->elementSize, _elements, _count * _array->elementSize);
    _array->count += _count;
}

void _data_array_clear(_DataArray * _array)
{
    _array->count = 0;
}

void * _data_array_begin(_DataArray * _array)
{
    assert(_array->array);
    return _array->array;
}

void * _data_array_end(_DataArray * _array)
{
    assert(_array->array);
    return _array->array + _array->elementSize * _array->count;
}

void * _data_array_remove_last(_DataArray * _array)
{
    assert(_array->array);
    return _array->array + _array->elementSize * (_array->count-- - 1);
}

// static void _memoryChunkInit(_MemoryChunk * _chunk, int _capacity, int _elementSize)
// {
//     _chunk->array = malloc(_capacity * _elementSize);
//     assert(_chunk->array);
//     _chunk->count = 0;
//     _chunk->capacity = _capacity;
//     _chunk->elementSize = _elementSize;
//     _chunk->freeList = malloc(_capacity * sizeof(int));
//     _chunk->freeListCount = 0;
// }

// static void _memoryChunkClear(_MemoryChunk * _chunk, int _capacity)
// {

// }

// static tpBool _memoryChunkIsValid(_MemoryChunk * _chunk, int _index)
// {
//     for (int i = 0; i < _chunk->freeListCount; ++i)
//     {
//         if (_chunk->freeList[i] == _index)
//             return tpFalse;
//     }
//     return true;
// }

// static void _memoryChunkDeallocate(_MemoryChunk * _chunk)
// {
//     free(_chunk->freeList);
//     free(_chunk->array);
// }

// int _memoryChunkNextElementIndex(_MemoryChunk * _chunk)
// {
//     if (_chunk->freeListCount)
//     {
//         printf("RETURN FROM FREELIST\n");
//         return _chunk->freeList[_chunk->freeListCount-- - 1];
//     }
//     else if (_chunk->count < _chunk->capacity)
//     {
//         printf("RETURN FROM CHUNK\n");
//         return _chunk->count++;
//     }
//     else
//     {
//         printf("RESIZE CHUNK\n");
//         int c = _chunk->capacity * 2;
//         char * array = realloc(_chunk->array, c * _chunk->elementSize);
//         int * fl = realloc(_chunk->freeList, c * sizeof(int));
//         if (array && fl)
//         {
//             _chunk->array = array;
//             _chunk->freeList = fl;
//             _chunk->capacity = c;
//             return _chunk->count++;
//         }
//     }
//     printf("FAIL\n");
//     return -1;
// }

// void * _memoryChunkElementAt(_MemoryChunk * _chunk, int _index)
// {
//     if (_index < _chunk->count)
//         return _chunk->array + _index * _chunk->elementSize;
//     return NULL;
// }

// void _memoryChunkRemoveElement(_MemoryChunk * _chunk, int _index)
// {
//     if (_index < _chunk->count)
//         _chunk->freeList[_chunk->freeListCount++] = _index;
// }

static int _compileShader(const char * _shaderCode, GLenum _shaderType, GLuint * _outHandle, _ErrorMessage * _outError)
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
        return 1;
    }
    else
    {
        *_outHandle = glHandle;
    }
    return 0;
}

static int _createProgram(const char * _vertexShader, const char * _fragmentShader, int _bTexProgram, GLuint * _outHandle, _ErrorMessage * _outError)
{
    GLuint vertexShader, fragmentShader;
    int err = _compileShader(_vertexShader, GL_VERTEX_SHADER, &vertexShader, _outError);
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

        return 1;
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

    return 0;
}

tpBool _isValidPath(tpContext * _ctx, tpPath _path)
{

}

int tpContextInit(tpContext * _ctx)
{
    int ret = 0;
    _ErrorMessage msg;

    ret = _createProgram(_vertexShaderCode, _fragmentShaderCode, 0, &glctx->program, &msg);
    if (ret)
    {
        strcpy(_ctx->lastErrorMessage, msg.message);
        return ret;
    }
    ret = _createProgram(_vertexShaderCodeTexture, _fragmentShaderCodeTexture, 1, &glctx->textureProgram, &msg);
    if (ret)
    {
        strcpy(_ctx->lastErrorMessage, msg.message);
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

    _ctx->currentClipStencilPlane = kClipStencilPlaneOne;
    _ctx->clippingStackDepth = 0;
    // glctx->geometryCache = NULL;
    // glctx->geometryCacheCapacity = 0;
    // glctx->geometryCacheCount = 0;
    // _data_array_init(&glctx->geometryCache, 1024, sizeof(Float));
    // _data_array_init(&glctx->jointCache, 1024, sizeof(int));
    _ctx->projection = tpMat4MakeIdentity();

    _memoryChunkInit(_ctx->paths, 8, sizeof(_tpGLPath));

    return ret;
}

int tpContextDeallocate(tpContext * _ctx)
{
    glDeleteProgram(_ctx->program);
    glDeleteBuffers(1, &_ctx->vbo);
    glDeleteVertexArrays(1, &_ctx->vao);
    glDeleteProgram(_ctx->textureProgram);
    glDeleteBuffers(1, &_ctx->textureVbo);
    glDeleteVertexArrays(1, &_ctx->textureVao);

    // _data_array_deallocate(&glctx->geometryCache);
    // _data_array_deallocate(&glctx->jointCache);

    //This is kinda slow as we need to run throgh the whole
    //free list for every path....good enough for now though
    tpBool bIsValid;
    for (int i = 0; i < _ctx->paths.count; ++i)
    {
        if (_memoryChunkIsValid(&_ctx->paths, i))
            _tpPathDestroy(_ctx, (_tpGLPath *)_ctx->paths);
    }

    _memoryChunkDeallocate(&_ctx->paths);

    return 0;
}

tpPath tpPathCreate(tpContext * _ctx)
{
    _tpGLPath path;
    _data_array_init(&path.segments, 16, sizeof(tpSegment));
    _data_array_init(&path.children, 4, sizeof(tpPath));
    path.lastSegment = NULL;
    path.bIsClosed = 0;
    memset(path.lastErrorMessage, 0, sizeof(path.lastErrorMessage));

    _data_array_init(&path.geometryCache, 128, sizeof(tpFloat));
    _data_array_init(&path.strokeGeometryCache, 128, sizeof(tpFloat));
    _data_array_init(&path.jointCache, 128, sizeof(int));
    path.bGeometryCacheDirty = tpTrue;
    path.lastTransformScale = 1.0;

    // _data_array_append_array(&_ctx->paths, &path,



    // tpMat3 tmp = tpMat3MakeIdentity();
    // tpPathSetTransform(ret, &tmp);


    return ret;
}

void _tpPathDestroy(tpContext * _ctx, _tpGLPath * _path)
{
    _data_array_deallocate(&_path->geometryCache);
    _data_array_deallocate(&_path->strokeGeometryCache);
    _data_array_deallocate(&_path->jointCache);

    for (int i = 0; i < _path->segmentChunks.count; ++i)
    {
        free((tpSegmentChunk *)_path->segmentChunks.array[i]);
    }

    // for (int i = 0; i < _path->children.count; ++i)
    // {
    //     tpPathDestroy(_ctx, (tpPath*)_path->children.array[i]);
    // }

    _data_array_deallocate(&_path->segmentChunks);
    _data_array_deallocate(&_path->children);
}

void tpPathDestroy(tpContext * _ctx, tpPath _path)
{   

    _memoryChunkRemoveElement(&_ctx->paths, _path.index);
}

int tpPathClear(tpPath * _path)
{
    _tpPathClear(_path);
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

int tpPathAddSegment(tpPath * _path, tpFloat _h0x, tpFloat _h0y, tpFloat _px, tpFloat _py, tpFloat _h1x, tpFloat _h1y)
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

int tpPathAddPoint(tpPath * _path, tpFloat _x, tpFloat _y)
{
    tpPathAddSegment(_path, _x, _y, _x, _y, _x, _y);
    return 0;
}

int tpPathCircle(tpPath * _path, tpFloat _x, tpFloat _y, tpFloat _r)
{
    return 0;
}

int tpPathCubicCurveTo(tpPath * _path, tpFloat _h0x, tpFloat _h0y, tpFloat _h1x, tpFloat _h1y, tpFloat _px, tpFloat _py)
{
    assert(_path->lastSegment);
    _path->lastSegment->handleOut.x = _h0x;
    _path->lastSegment->handleOut.y = _h0y;
    tpPathAddSegment(_path, _h1x, _h1y, _px, _py, _px, _py);
    return 0;
}

int tpPathQuadraticCurveTo(tpPath * _path, tpFloat _hx, tpFloat _hy, tpFloat _px, tpFloat _py)
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

// tpStyle tpStyleMake()
// {
//     tpStyle ret;

//     ret.fill.data.color = tpColorMake(1, 1, 1, 1);
//     ret.fill.type = kPtColor;
//     ret.stroke.data.color = tpColorMake(0, 0, 0, 1);
//     ret.stroke.type = kPtColor;
//     ret.strokeWidth = 1.0;
//     ret.strokeJoin = kSjMiter;
//     ret.strokeCap = kScButt;
//     ret.fillType = kFtEvenOdd;
//     ret.dashCount = 0;
//     ret.dashOffset = 0;

//     return ret;
// }

// void tpStyleSetDash(tpStyle * _style, tpFloat * _dashArray, int _count, tpFloat _offset)
// {
//     assert(_count < TARP_MAX_DASH_ARRAY_SIZE);

//     _style->dashOffset = _offset;
//     for (int i = 0; i < _count; ++i)
//     {
//         _style->dashArray[i] = _dashArray[i];
//     }
//     _style->dashCount = _count;
// }

// void tpStyleSetFillColor(tpStyle * _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a)
// {
//     _style->fill.data.color = tpColorMake(_r, _g, _b, _a);
//     _style->fill.type = kPtColor;
// }

// void tpStyleSetStrokeColor(tpStyle * _style, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a)
// {
//     _style->stroke.data.color = tpColorMake(_r, _g, _b, _a);
//     _style->stroke.type = kPtColor;
// }

// void tpStyleSetFillGradient(tpStyle * _style, const tpGradient * _gradient)
// {
//     _style->fill.data.gradient = (tpGradient *)_gradient;
//     _style->fill.type = kPtGradient;
// }

// void tpStyleSetStrokeGradient(tpStyle * _style, const tpGradient * _gradient)
// {
//     _style->stroke.data.gradient = (tpGradient *)_gradient;
//     _style->stroke.type = kPtGradient;
// }

// void tpStyleSetStrokeWidth(tpStyle * _style, tpFloat _strokeWidth)
// {
//     _style->strokeWidth = _strokeWidth;
// }

// void tpStyleSetStrokeJoin(tpStyle * _style, StrokeJoin _join)
// {
//     _style->strokeJoin = _join;
// }

// void tpStyleSetStrokeCap(tpStyle * _style, StrokeCap _cap)
// {
//     _style->strokeCap = _cap;
// }

// void tpStyleSetFillRule(tpStyle * _style, FillType _fillType)
// {
//     _style->fillType = _fillType;
// }

// tpGradient * tpGradientCreateLinear(tpFloat _x0, tpFloat _y0, tpFloat _x1, tpFloat _y1)
// {
//     tpGradient * ret = (tpGradient *)malloc(sizeof(tpGradient));
//     ret->type = kGtLinear;
//     ret->origin = tpVec2Make(_x0, _y0);
//     ret->destination = tpVec2Make(_x1, _y1);
//     ret->stopCount = 0;
//     return ret;
// }

// void tpGradientAddColorStop(tpGradient * _gradient, tpFloat _r, tpFloat _g, tpFloat _b, tpFloat _a, tpFloat _offset)
// {
//     _gradient->stops[_gradient->stopCount++] = (tpColorStop) {tpColorMake(_r, _g, _b, _a), _offset};
// }

// void tpGradientDestroy(tpGradient * _gradient)
// {
//     free(_gradient);
// }

// tpFloat _length_squared(tpFloat _x, tpFloat _y)
// {
//     return _x * _x + _y * _y;
// }

// tpFloat _distance_squared(tpFloat _x0, tpFloat _y0, tpFloat _x1, tpFloat _y1)
// {
//     return _length_squared(_x1 - _x0, _y1 - _y0);
// }


// typedef struct
// {
//     tpFloat values[3];
//     int count;
// } _SolveResult;

// void _solve_quadratic(tpFloat _a, tpFloat _b, tpFloat _c, tpFloat _min, tpFloat _max, _SolveResult * _outResult)
// {
//     //@TODO: These epsilons most likely need adjustment!
//     tpFloat eMin = _min - FLT_EPSILON;
//     tpFloat eMax = _max + FLT_EPSILON;
//     tpFloat x1, x2;
//     x1 = x2 = INFINITY;
//     tpFloat B = _b;
//     tpFloat D;

//     if (fabs(_a) < FLT_EPSILON)
//     {
//         // This could just be a linear equation
//         if (fabs(B) < FLT_EPSILON)
//         {
//             _outResult->count = 0;
//             if (fabs(_c) < FLT_EPSILON)
//                 _outResult->count = -1;
//             return;
//         }
//         x1 = -_c / B;
//     }
//     else
//     {
//         // a, b, c are expected to be the coefficients of the equation:
//         // Ax² - 2Bx + C == 0, so we take b = -B/2:
//         _b *= -0.5;
//         D = _b * _b - _a * _c; // Discriminant

//         // If the discriminant is very small, we can try to pre-condition
//         // the coefficients, so that we may get better accuracy
//         if (D != 0 && fabs(D) < FLT_EPSILON)
//         {
//             // If the geometric mean of the coefficients is small enough
//             tpFloat gmC = pow(fabs(_a * _b * _c), 1.0 / 3.0);
//             if (gmC < 1e-8)
//             {
//                 // We multiply with a factor to normalize the coefficients.
//                 // The factor is just the nearest exponent of 10, big enough
//                 // to raise all the coefficients to nearly [-1, +1] range.
//                 tpFloat mult = pow(10, fabs(floor(log(gmC) * 0.4342944819032518))); //number is LOG10E
//                 if (!isfinite(mult))
//                     mult = 0;
//                 _a *= mult;
//                 _b *= mult;
//                 _c *= mult;
//                 // Recalculate the discriminant
//                 D = _b * _b - _a * _c;
//             }
//         }

//         if (D >= -FLT_EPSILON)    // No real roots if D < 0
//         {
//             tpFloat Q = D < 0 ? 0 : sqrtf(D);
//             tpFloat R = _b + (_b < 0 ? -Q : Q);

//             // Try to minimize floating point noise.
//             if (R == 0)
//             {
//                 x1 = _c / _a;
//                 x2 = -x1;
//             }
//             else
//             {
//                 x1 = R / _a;
//                 x2 = _c / R;
//             }
//         }
//     }

//     // We need to include EPSILON in the comparisons with min / max,
//     // as some solutions are ever so lightly out of bounds.
//     _outResult->count = 0;
//     if (isfinite(x1) && (x1 > eMin && x1 < eMax))
//         _outResult->values[_outResult->count++] = TARP_MAX(TARP_MIN(x1, _min), _max);
//     if (x2 != x1 && isfinite(x2) && (x2 > eMin && x2 < eMax))
//         _outResult->values[_outResult->count++] = TARP_MAX(TARP_MIN(x2, _min), _max);
// }

// void _set_min_max(tpFloat _value, tpFloat _padding, tpFloat * _inOutMin, tpFloat * _inOutMax)
// {
//     tpFloat l = _value - _padding;
//     tpFloat r = _value + _padding;

//     if (l < *_inOutMin)
//         *_inOutMin = l;

//     if (l > *_inOutMax)
//         *_inOutMax = r;
// }

// //@TODO: write a separate horizontal/vertical function for
// //this so that we can save some extra cyles for when
// //it is called from the bounds calculation?
// void _curve_position_at(tpFloat _x0, tpFloat _y0,
//                         tpFloat _h0x, tpFloat _h0y,
//                         tpFloat _h1x, tpFloat _h1y,
//                         tpFloat _x1, tpFloat _y1,
//                         tpFloat _t,
//                         tpFloat * _outX, tpFloat * _outY)
// {
//     if (_t == 0)
//     {
//         *_outX = _x0;
//         *_outY = _y0;
//         return;
//     }
//     if (_t == 1)
//     {
//         *_outX = _x1;
//         *_outY = _y1;
//         return;
//     }

//     tpFloat u = 1.0 - _t;
//     tpFloat tt = _t * _t;
//     tpFloat uu = u * u;
//     tpFloat uuu = uu * u;
//     tpFloat ttt = tt * _t;

//     //first term
//     tpFloat px = _x0 * uuu;
//     tpFloat py = _y0 * uuu;

//     //second term
//     px += _h0x * 3 * uu * _t;
//     py += _h0y * 3 * uu * _t;

//     //third term
//     px += _h1x * 3 * u * tt;
//     py += _h1y * 3 * u * tt;

//     //fourth term
//     px += _x1 * ttt;
//     py += _y1 * ttt;

//     *_outX = px;
//     *_outY = py;
// }

// void _curve_bounds(tpFloat _x0, tpFloat _y0,
//                    tpFloat _h0x, tpFloat _h0y,
//                    tpFloat _h1x, tpFloat _h1y,
//                    tpFloat _x1, tpFloat _y1,
//                    tpFloat _padding,
//                    _Rect * _outRect)
// {
//     printf("CURVE BOUNDS\n");
//     tpFloat ax = (_h0x - _h1x) * 3.0 - _x0 + _x1;
//     tpFloat ay = (_h0y - _h1y) * 3.0 - _y0 + _y1;
//     tpFloat bx = (_x0 + _h1x) * 2.0 - _h0x * 4.0;
//     tpFloat by = (_y0 + _h1y) * 2.0 - _h0y * 4.0;
//     tpFloat cx = _h0x - _x0;
//     tpFloat cy = _h0y - _y0;

//     _SolveResult xRoots, yRoots;
//     _solve_quadratic(ax, bx, cx, 0, 1, &xRoots);
//     _solve_quadratic(ay, by, cy, 0, 1, &yRoots);

//     printf("CURVE BOUNDS2\n");

//     tpFloat tMin = 4e-4;
//     tpFloat tMax = 1.0 - 4e-4;

//     tpFloat minX = _x0;
//     tpFloat minY = _y0;
//     tpFloat maxX = minX;
//     tpFloat maxY = minY;

//     _set_min_max(_x1, 0, &minX, &maxX);
//     _set_min_max(_y1, 0, &minY, &maxY);

//     printf("ROOTS %i %i\n", xRoots.count, yRoots.count);

//     tpFloat tmpX, tmpY;
//     for (int i = 0; i < xRoots.count; ++i)
//     {
//         if (tMin < xRoots.values[i] && xRoots.values[i] < tMax)
//         {
//             _curve_position_at(_x0, _y0, _h0x, _h0y, _h1x, _h1y, _x1, _y1,
//                                xRoots.values[i], &tmpX, &tmpY);
//             printf("TMPX %f %f\n", tmpX, tmpY);
//             _set_min_max(tmpX, _padding, &minX, &maxX);
//         }
//     }

//     for (int i = 0; i < yRoots.count; ++i)
//     {
//         if (tMin < yRoots.values[i] && yRoots.values[i] < tMax)
//         {
//             _curve_position_at(_x0, _y0, _h0x, _h0y, _h1x, _h1y, _x1, _y1,
//                                yRoots.values[i], &tmpX, &tmpY);
//             printf("TMPY %f %f\n", tmpX, tmpY);
//             _set_min_max(tmpY, _padding, &minY, &maxY);
//         }
//     }

//     printf("CURVE BOUNDS 3\n");
//     _outRect->minX = minX;
//     _outRect->minY = minY;
//     _outRect->maxX = maxX;
//     _outRect->maxY = maxY;
// }

// int _absolute_tolerance_compare(tpFloat _x, tpFloat _y, tpFloat _epsilon)
// {
//     return fabs(_x - _y) <= _epsilon;
// }

// int _relative_tolerance_compare(tpFloat _x, tpFloat _y, tpFloat _epsilon)
// {
//     tpFloat maxXY = TARP_MAX(fabs(_x) , fabs(_y));
//     return fabs(_x - _y) <= _epsilon * maxXY;
// }

// int _combined_tolerance_compare(tpFloat _x, tpFloat _y, tpFloat _epsilon)
// {
//     tpFloat maxXYOne = TARP_MAX(1.0, TARP_MAX(fabs(_x), fabs(_y)));
//     return fabs(_x - _y) <= _epsilon * maxXYOne;
// }

// int _is_close(tpFloat _a, tpFloat _b, tpFloat _epsilon)
// {
//     return _combined_tolerance_compare(_a, _b, _epsilon);
// }

// int _is_linear(tpFloat _x0, tpFloat _y0, tpFloat _h0x, tpFloat _h0y, tpFloat _h1x, tpFloat _h1y, tpFloat _x1, tpFloat _y1)
// {
//     return _is_close(_x0, _h0x, FLT_EPSILON) && _is_close(_y0, _h0y, FLT_EPSILON) &&
//            _is_close(_x1, _h1x, FLT_EPSILON) && _is_close(_y1, _h1x, FLT_EPSILON);
// }

// int _is_flat_enough(tpFloat _x0, tpFloat _y0, tpFloat _h0x, tpFloat _h0y, tpFloat _h1x, tpFloat _h1y, tpFloat _x1, tpFloat _y1, tpFloat _tolerance)
// {
//     if (_is_linear(_x0, _y0, _h0x, _h0y, _h1x, _h1y, _x1, _y1))
//         return 1;

//     // Comment from paper.js source in Curve.js:
//     // Thanks to Kaspar Fischer and Roger Willcocks for the following:
//     // http://hcklbrrfnn.files.wordpress.com/2012/08/bez.pdf
//     tpFloat ux = _h0x * 3.0 - _x0 * 2.0 - _x1;
//     tpFloat uy = _h0y * 3.0 - _y0 * 2.0 - _y1;
//     tpFloat vx = _h1x * 3.0 - _x1 * 2.0 - _x0;
//     tpFloat vy = _h1y * 3.0 - _y1 * 2.0 - _y0;

//     return TARP_MAX(ux * ux, vx * vx) + TARP_MAX(uy * uy, vy * vy) < 10 * _tolerance * _tolerance;
// }

// void _subdivide_curve(tpFloat _x0, tpFloat _y0,
//                       tpFloat _h0x, tpFloat _h0y,
//                       tpFloat _h1x, tpFloat _h1y,
//                       tpFloat _x1, tpFloat _y1,
//                       tpFloat _t,
//                       _CurvePair * _result)
// {
//     tpFloat v1x, v2x, v3x, v4x, v5x, v6x, v1y, v2y, v3y, v4y, v5y, v6y;

//     tpFloat u = 1 - _t;

//     v1x = _x0 * u + _h0x * _t;
//     v1y = _y0 * u + _h0y * _t;
//     v2x = _h0x * u + _h1x * _t;
//     v2y = _h0y * u + _h1y * _t;
//     v3x = _h1x * u + _x1 * _t;
//     v3y = _h1y * u + _y1 * _t;
//     v4x = v1x * u + v2x * _t;
//     v4y = v1y * u + v2y * _t;
//     v5x = v2x * u + v3x * _t;
//     v5y = v2y * u + v3y * _t;
//     v6x = v4x * u + v5x * _t;
//     v6y = v4y * u + v5y * _t;

//     *_result = (_CurvePair)
//     {
//         {_x0, _y0, v1x, v1y, v4x, v4y, v6x, v6y},
//         {v6x, v6y, v5x, v5y, v3x, v3y, _x1, _y1}
//     };
// }

// void _evaluate_point_for_bounds(tpFloat _x, tpFloat _y, _Rect * _bounds)
// {
//     _bounds->minX = TARP_MIN(_x, _bounds->minX);
//     _bounds->minY = TARP_MIN(_y, _bounds->minY);
//     _bounds->maxX = TARP_MAX(_x, _bounds->maxX);
//     _bounds->maxY = TARP_MAX(_y, _bounds->maxY);
// }

// void _flatten_curve(_GLPathData * _pdata,
//                     tpFloat _x0, tpFloat _y0,
//                     tpFloat _h0x, tpFloat _h0y,
//                     tpFloat _h1x, tpFloat _h1y,
//                     tpFloat _x1, tpFloat _y1,
//                     const _Curve * _initialCurve,
//                     tpFloat _angleTolerance,
//                     tpFloat _minDistance,
//                     int _recursionDepth,
//                     int _maxRecursionDepth,
//                     int _bIsClosed,
//                     int _bLastCurve,
//                     _Rect * _bounds)
// {

//     if (_recursionDepth < _maxRecursionDepth && !_is_flat_enough(_x0, _y0, _h0x, _h0y, _h1x, _h1y, _x1, _y1, _angleTolerance))
//     {
//         printf("FLATTEN MORE %i\n", _recursionDepth);
//         int rd = _recursionDepth + 1;
//         _CurvePair p;
//         _subdivide_curve(_x0, _y0, _h0x, _h0y, _h1x, _h1y, _x1, _y1, 0.5, &p);
//         _flatten_curve(_pdata,
//                        p.first.x0, p.first.y0,
//                        p.first.h0x, p.first.h0y,
//                        p.first.h1x, p.first.h1y,
//                        p.first.x1, p.first.y1,
//                        _initialCurve,
//                        _angleTolerance, _minDistance,
//                        rd, _maxRecursionDepth,
//                        _bIsClosed, _bLastCurve, _bounds);
//         _flatten_curve(_pdata,
//                        p.second.x0, p.second.y0,
//                        p.second.h0x, p.second.h0y,
//                        p.second.h1x, p.second.h1y,
//                        p.second.x1, p.second.y1,
//                        _initialCurve,
//                        _angleTolerance, _minDistance,
//                        rd, _maxRecursionDepth,
//                        _bIsClosed, _bLastCurve, _bounds);

//     }
//     else
//     {
//         printf("ADD TO CACHE\n");
//         //for the first curve we also add its first segment
//         if (!_pdata->geometryCache.count)
//         {
//             tpFloat data[] = {_x0, _y0, _x1, _y1};
//             _data_array_append_array(&_pdata->geometryCache, data, 4);
//             int isJoint[] = {0, _x1 == _initialCurve->x1 && _y1 == _initialCurve->y1 && !_bLastCurve};
//             _data_array_append_array(&_pdata->jointCache, isJoint, 2);
//             _evaluate_point_for_bounds(_x0, _y0, _bounds);
//             _evaluate_point_for_bounds(_x1, _y1, _bounds);
//         }
//         else
//         {
//             tpFloat data[] = {_x1, _y1};
//             _data_array_append_array(&_pdata->geometryCache, data, 2);
//             int isJoint = _bIsClosed ? (_x1 == _initialCurve->x1 && _y1 == _initialCurve->y1) :
//                           (_x1 == _initialCurve->x1 && _y1 == _initialCurve->y1 && !_bLastCurve);
//             _data_array_append_array(&_pdata->jointCache, &isJoint, 1);
//             _evaluate_point_for_bounds(_x1, _y1, _bounds);
//         }


//         // tpFloat mds = _minDistance * _minDistance;
//         // tpFloat minDistSquared = _minDistance * _minDistance;
//         // if (_outPositions.count())
//         // {
//         //     if (crunch::distanceSquared(_curve.positionTwo(), _outPositions.last()) >= minDistSquared)
//         //     {
//         //         _outPositions.append(_curve.positionTwo());
//         //         if (_outJoins)
//         //         {
//         //             if (_bIsClosed)
//         //                 _outJoins->append(_curve.positionTwo() == _initialCurve.positionTwo());
//         //             else
//         //                 _outJoins->append(_curve.positionTwo() == _initialCurve.positionTwo() && !_bLastCurve);
//         //         }
//         //     }
//         // }
//         // else
//         // {
//         //     //for the first curve we also add its first segment
//         //     _outPositions.append(_curve.positionOne());
//         //     if (_outJoins)
//         //         _outJoins->append(false);
//         //     _outPositions.append(_curve.positionTwo());
//         //     if (_outJoins)
//         //     {
//         //         _outJoins->append(_curve.positionTwo() == _initialCurve.positionTwo() && !_bLastCurve);
//         //     }
//         // }
//     }
// }

// int _flatten_path(_GLContext * _ctx,
//                   tpPath * _path,
//                   _GLPathData * _pdata,
//                   tpFloat _angleTolerance,
//                   tpFloat _minDistance,
//                   int _maxRecursionDepth)
// {
//     printf("====================================\n");
//     int i = 0;
//     int j = 0;
//     tpSegmentChunk * chunk;
//     tpSegment * last = NULL, *current = NULL;
//     int recursionDepth = 0;
//     _Rect tmpBounds;
//     tmpBounds.minX = FLT_MAX;
//     tmpBounds.minY = FLT_MAX;
//     tmpBounds.maxX = -FLT_MAX;
//     tmpBounds.maxY = -FLT_MAX;
//     _Rect curveBounds;

//     _data_array_clear(&_pdata->geometryCache);
//     _data_array_clear(&_pdata->jointCache);

//     for (; i < _path->segmentChunks.count; ++i)
//     {
//         chunk = (tpSegmentChunk *)_path->segmentChunks.array[i];
//         for (j = 0; j < chunk->count; ++j)
//         {
//             if (!last)
//             {
//                 last = &chunk->array[j];
//             }
//             else
//             {
//                 current = &chunk->array[j];

//                 // _curve_bounds(last->position.x, last->position.y,
//                 //               last->handleOut.x, last->handleOut.y,
//                 //               current->handleIn.x, current->handleIn.y,
//                 //               current->position.x, current->position.y,
//                 //               0,
//                 //               &curveBounds);

//                 // tmpBounds.minX = TARP_MIN(curveBounds.minX, tmpBounds.minX);
//                 // tmpBounds.minY = TARP_MIN(curveBounds.minY, tmpBounds.minY);
//                 // tmpBounds.maxX = TARP_MAX(curveBounds.maxX, tmpBounds.maxX);
//                 // tmpBounds.maxY = TARP_MAX(curveBounds.maxY, tmpBounds.maxY);

//                 // printf("TMP BOUNDS\n");
//                 // printf("MIN %f %f\n", curveBounds.minX, curveBounds.minY);
//                 // printf("MAX %f %f\n", curveBounds.maxX, curveBounds.maxY);

//                 _Curve _initialCurve = {last->position.x, last->position.y,
//                                         last->handleOut.x, last->handleOut.y,
//                                         current->handleIn.x, current->handleIn.y,
//                                         current->position.x, current->position.y
//                                        };

//                 _flatten_curve(_pdata,
//                                last->position.x, last->position.y,
//                                last->handleOut.x, last->handleOut.y,
//                                current->handleIn.x, current->handleIn.y,
//                                current->position.x, current->position.y,
//                                &_initialCurve,
//                                _angleTolerance,
//                                _minDistance,
//                                recursionDepth,
//                                _maxRecursionDepth,
//                                _path->bIsClosed,
//                                i == _path->segmentChunks.count - 1 && j == chunk->count - 1,
//                                &tmpBounds);
//             }
//         }
//     }

//     // tpFloat bw = tmpBounds.maxX - tmpBounds.minX;
//     // tpFloat bh = tmpBounds.maxY - tmpBounds.minY;
//     // _ctx->geometryCache[0] = tmpBounds.minX + bw * 0.5;
//     // _ctx->geometryCache[1] = tmpBounds.minY + bh * 0.5;
//     _ctx->boundsCache = tmpBounds;

//     //we add the bounds geometry to the and of the vertex data
//     tpFloat boundsData[] = {tmpBounds.minX, tmpBounds.minY, tmpBounds.minX, tmpBounds.maxY,
//                             tmpBounds.maxX, tmpBounds.minY, tmpBounds.maxX, tmpBounds.maxY
//                            };
//     _data_array_append_array(&_pdata->geometryCache, boundsData, 8);

//     printf("EEEEND\n");
//     return 0;
// }

// int _draw_paint(_GLContext * _ctx, tpPath * _path, _GLPathData * _pdata, const tpPaint * _paint)
// {
//     if (_paint->type == kPtColor)
//     {
//         //@TODO: Cache the uniform loc
//         ASSERT_NO_GL_ERROR(glUniform4fv(glGetUniformLocation(_ctx->program, "meshColor"), 1, &_paint->data.color.r));
//         ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, (_pdata->geometryCache.count - 8) / 2, 4));
//     }
// }

// int _recursively_draw_even_odd(_GLContext * _ctx, tpPath * _path, const tpStyle * _style, const tpMat3 * _transform)
// {

// }

// int _draw_fill_even_odd(_GLContext * _ctx, tpPath * _path, _GLPathData * _pdata, const tpStyle * _style)
// {
//     ASSERT_NO_GL_ERROR(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
//     ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
//     ASSERT_NO_GL_ERROR(glStencilFunc(GL_ALWAYS, 0, kClipStencilPlaneOne));
//     ASSERT_NO_GL_ERROR(glStencilMask(kFillRasterStencilPlane));
//     ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT));

//     //_transform ? & (*_transform).v[0] : NULL
//     //@TODO: Cache the uniform loc
//     // ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(_ctx->program, "transformProjection"), 1, GL_FALSE, &mat.v[0]));
//     // printf("COUNT %i\n", _ctx->geometryCache.count);
//     ASSERT_NO_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(Float) * _pdata->geometryCache.count, _pdata->geometryCache.array, GL_DYNAMIC_DRAW));
//     ASSERT_NO_GL_ERROR(glDrawArrays(GL_TRIANGLE_FAN, 0, (_pdata->geometryCache.count - 8) / 2));

//     ASSERT_NO_GL_ERROR(glStencilFunc(GL_EQUAL, 255, kFillRasterStencilPlane));
//     ASSERT_NO_GL_ERROR(glStencilMask(kFillRasterStencilPlane));
//     ASSERT_NO_GL_ERROR(glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO));
//     ASSERT_NO_GL_ERROR(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

//     _draw_paint(_ctx, _path, _pdata, &_style->fill);
// }

// int _draw_fill_non_zero(_GLContext * _ctx, tpPath * _path, _GLPathData * _pdata, const tpStyle * _style)
// {

// }

// int _recursively_draw_stroke(_GLContext * _ctx, tpPath * _path, _GLPathData * _pdata, const tpStyle * _style)
// {

// }

// int _tpPathInit(tpPath * _path)
// {
//     _GLPathData * pdata = malloc(sizeof(_GLPathData));
//     _data_array_init(&pdata->geometryCache, 128, sizeof(Float));
//     _data_array_init(&pdata->strokeGeometryCache, 128, sizeof(Float));
//     _data_array_init(&pdata->jointCache, 128, sizeof(int));
//     pdata->bGeometryCacheDirty = tpTrue;
//     pdata->lastTransformScale = 1.0;
//     _path->_implData = pdata;
//     return 0;
// }

// int _tpPathDeallocate(tpPath * _path)
// {
//     _GLPathData * pdata = (_GLPathData *) _path->_implData;
//     _data_array_deallocate(&pdata->geometryCache);
//     _data_array_deallocate(&pdata->strokeGeometryCache);
//     _data_array_deallocate(&pdata->jointCache);
//     free(pdata);
// }

// int _tpPathClear(tpPath * _path)
// {
//     _GLPathData * pdata = (_GLPathData *) _path->_implData;
//     _data_array_clear(&pdata->geometryCache);
//     _data_array_clear(&pdata->strokeGeometryCache);
//     _data_array_clear(&pdata->jointCache);
//     pdata->bGeometryCacheDirty = tpTrue;
//     return 0;
// }

// int _tpPathGeometryChanged(tpPath * _path)
// {
//     ((_GLPathData *)_path->_implData)->bGeometryCacheDirty = tpTrue;
//     return 0;
// }

// int _tpPathTransformChanged(tpPath * _path, const tpMat3 * _old, const tpMat3 * _new)
// {
//     _GLPathData * pdata = (_GLPathData *)_path->_implData;
//     if (!tpMat3Equals(_old, _new))
//     {
//         tpVec2 scale, skew, translation;
//         tpFloat rotation;
//         tpMat3Decompose(_new, &translation, &scale, &skew, &rotation);
//         if (pdata->lastTransformScale < scale.x || pdata->lastTransformScale < scale.y)
//         {
//             pdata->lastTransformScale = TARP_MAX(scale.x, scale.y);
//             pdata->bGeometryCacheDirty = tpTrue;
//         }
//         pdata->renderTransform = tpMat4MakeFrom2DTransform(_new);
//     }
//     return 0;
// }

// int tpSetProjection(tpContext * _ctx, const tpMat4 * _projection)
// {
//     ((_GLContext *)_ctx->_implData)->projection = *_projection;
//     return 0;
// }

// int tpDrawPath(tpContext * _ctx, tpPath * _path, const tpStyle * _style)
// {
//     _GLContext * ctx = (_GLContext *)_ctx->_implData;
//     _GLPathData * pdata = (_GLPathData *)_path->_implData;
//     assert(ctx && pdata);

//     // _data_array_clear(&ctx->geometryCache);
//     // _data_array_clear(&ctx->jointCache);
//     // tpFloat tmp[2] = {0, 0};
//     // _geometry_cache_append(ctx, tmp, 2);
//     // tpFloat tmp2[2] = {10, 30};
//     // _geometry_cache_append(ctx, tmp2, 2);

//     if (pdata->bGeometryCacheDirty)
//     {
//         pdata->bGeometryCacheDirty = tpFalse;
//         _flatten_path(ctx, _path, pdata, 0.15, 0, 32);
//     }
//     printf("BOUNDS\n");
//     printf("MIN %f %f\n", ctx->boundsCache.minX, ctx->boundsCache.minY);
//     printf("MAX %f %f\n", ctx->boundsCache.maxX, ctx->boundsCache.maxY);
//     // printf("GEOM CACHE %i\n", ctx->geometryCacheCount);

//     // for (int i = 0; i < ctx->geometryCacheCount; ++i)
//     // {
//     //     printf("CACHE %f\n", ctx->geometryCache[i]);
//     // }

//     ASSERT_NO_GL_ERROR(glDisable(GL_DEPTH_TEST));
//     ASSERT_NO_GL_ERROR(glDepthMask(GL_FALSE));
//     ASSERT_NO_GL_ERROR(glEnable(GL_MULTISAMPLE));
//     ASSERT_NO_GL_ERROR(glEnable(GL_BLEND));
//     ASSERT_NO_GL_ERROR(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
//     ASSERT_NO_GL_ERROR(glEnable(GL_STENCIL_TEST));
//     ASSERT_NO_GL_ERROR(glStencilMask(kFillRasterStencilPlane));
//     ASSERT_NO_GL_ERROR(glClearStencil(0));
//     ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));
//     ASSERT_NO_GL_ERROR(glStencilMask(kClipStencilPlaneOne | kClipStencilPlaneTwo | kStrokeRasterStencilPlane));
//     ASSERT_NO_GL_ERROR(glClearStencil(255));
//     ASSERT_NO_GL_ERROR(glClear(GL_STENCIL_BUFFER_BIT));

//     ASSERT_NO_GL_ERROR(glUseProgram(ctx->program));

//     //@TODO: Cache the uniform loc
//     ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(ctx->program, "transform"), 1, GL_FALSE, &pdata->renderTransform.v[0]));
//     ASSERT_NO_GL_ERROR(glUniformMatrix4fv(glGetUniformLocation(ctx->program, "projection"), 1, GL_FALSE, &ctx->projection.v[0]));
//     ASSERT_NO_GL_ERROR(glBindVertexArray(ctx->vao));
//     ASSERT_NO_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo));
//     ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(0));
//     // ASSERT_NO_GL_ERROR(glEnableVertexAttribArray(1));
//     // ASSERT_NO_GL_ERROR(glUniform4f(glGetUniformLocation(ctx->program, "meshColor"), 1.0, 0.0, 0.0, 1.0));
//     _draw_fill_even_odd(ctx, _path, pdata, _style);

//     return 0;
// }

#endif //TARP_TARPGL_H
