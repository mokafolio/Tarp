#include <Tarp/TarpGL.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// we use GLFW to open a simple window
#include <GLFW/glfw3.h>

#define TARP_STATIC_ASSERT(pred) switch(0){case 0:case pred:;}

#define TARP_COMPILER_CLANG 0
#define TARP_COMPILER_GCC 1

#if defined(__clang__)
#define TARP_COMPILER TARP_COMPILER_CLANG
#elif defined(__GNUC__)
#define TARP_COMPILER TARP_COMPILER_GCC
#endif

#if TARP_COMPILER == TARP_COMPILER_CLANG || TARP_COMPILER == TARP_COMPILER_GCC
#define TARP_POINTER_SIZE __SIZEOF_POINTER__
#define TARP_INT_SIZE __SIZEOF_INT__
#else
#error "Can't detect pointer/integer size at preprocessor stage on your platform :("
#endif

#if TARP_POINTER_SIZE == TARP_INT_SIZE * 2
#define TARP_HANDLE_FITS_POINTER
#endif

// TARP_DECLARE_ARRAY(tpIntArray, int);
#define _TARP_ARRAY_T tpIntArray
#define _TARP_ITEM_T int
#include <Tarp/TarpArray.h>

// typedef struct
// {
//     char * array;
//     int capacity;
//     int count;
//     int elementSize;
//     int * freeList;
//     int freeListCount;
// } _MemoryChunk;

// typedef struct
// {
//     int index;
//     int type;
// } Handle;

// Handle * makeHandle(int _index, int _type)
// {
// #ifdef TARP_HANDLE_FITS_POINTER
//     return (void*)(&(Handle){_index, _type});
// #else
//     Handle * ret = malloc(sizeof(Handle));
//     ret->index = _index;
//     ret->type = _type;
//     return ret;
// #endif
// }

// struct FU_;
// typedef struct FU_ FU;

// FU makeFU();

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


// struct FU_
// {
//     int index;
//     int type;
// };

// FU makeFU()
// {
//     return (FU){1, 99};
// }


int main(int argc, char * argv[])
{
    // COMPILE_TIME_ASSERT(sizeof(Handle) != sizeof(void*));

    // initialize glfw
    if (!glfwInit())
        return EXIT_FAILURE;

    // and set some hints to get the correct opengl versions/profiles
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // _MemoryChunk chunk;
    // _memoryChunkInit(&chunk, 2, sizeof(int));

    // for (int i = 0; i < 10; ++i)
    // {
    //     int idx = _memoryChunkNextElementIndex(&chunk);
    //     printf("IDX %i\n", idx);

    //     int * item = _memoryChunkElementAt(&chunk, idx);
    //     *item = i;
    // }

    // for (int i = 0; i < 3; ++i)
    // {
    //     _memoryChunkReleaseElement(&chunk, i * 2);
    // }

    // for (int i = 0; i < 3; ++i)
    // {
    //     int idx = _memoryChunkNextElementIndex(&chunk);
    //     printf("IDX %i\n", idx);

    //     int * item = _memoryChunkElementAt(&chunk, idx);
    //     *item = 99;
    // }

    // for (int i = 0; i < chunk.count; ++i)
    // {
    //     printf("ITEM %i\n", *((int *)_memoryChunkElementAt(&chunk, i)));
    // }

    // _memoryChunkDeallocate(&chunk);

    // printf("SOZE OF %i %i\n", sizeof(void *), sizeof(Handle));
    // printf("PTRDIFF_MAX = %ld %ld\n", PTRDIFF_MAX, __SIZEOF_POINTER__);

    // Handle * handle = makeHandle(1, 2);
    // printf("HANDLE %i %i\n", handle->index, handle->type);

    // FU fu = makeFU();
    // printf("HANDLE2222 %i %i\n", fu.index, fu.type);

    // printf("SIZE %i\n", sizeof(tpIntArray));

    // tpIntArray array;
    // tpIntArrayInit(&array, 10);

    // tpIntArrayAppend(&array, 99);
    // tpIntArrayAppend(&array, 3);
    // tpIntArrayAppend(&array, 132);

    // assert(array.count == 3);
    // assert(tpIntArrayAt(&array, 0) == 99);
    // assert(tpIntArrayAt(&array, 1) == 3);
    // assert(tpIntArrayAt(&array, 2) == 132);

    // tpIntArrayRemove(&array, 0);
    // assert(array.count == 2);
    // assert(tpIntArrayAt(&array, 0) == 3);
    // assert(tpIntArrayAt(&array, 1) == 132);

    // int arr[] = {20, 10, 5, 2, 1, 0};
    // tpIntArrayAppendArray(&array, arr, 6);
    // assert(array.count == 8);

    // assert(tpIntArrayAt(&array, 0) == 3);
    // assert(tpIntArrayAt(&array, 1) == 132);
    // assert(tpIntArrayAt(&array, 2) == 20);
    // assert(tpIntArrayAt(&array, 3) == 10);
    // assert(tpIntArrayAt(&array, 4) == 5);
    // assert(tpIntArrayAt(&array, 5) == 2);
    // assert(tpIntArrayAt(&array, 6) == 1);
    // assert(tpIntArrayAt(&array, 7) == 0);

    // tpIntArrayRemove(&array, 4);

    // assert(tpIntArrayAt(&array, 0) == 3);
    // assert(tpIntArrayAt(&array, 1) == 132);
    // assert(tpIntArrayAt(&array, 2) == 20);
    // assert(tpIntArrayAt(&array, 3) == 10);
    // assert(tpIntArrayAt(&array, 4) == 2);
    // assert(tpIntArrayAt(&array, 5) == 1);
    // assert(tpIntArrayAt(&array, 6) == 0);

    // tpIntArrayRemoveRange(&array, 5, 7);

    // assert(array.count == 5);
    // assert(tpIntArrayAt(&array, 0) == 3);
    // assert(tpIntArrayAt(&array, 1) == 132);
    // assert(tpIntArrayAt(&array, 2) == 20);
    // assert(tpIntArrayAt(&array, 3) == 10);
    // assert(tpIntArrayAt(&array, 4) == 2);

    // tpIntArrayRemoveRange(&array, 1, 3);

    // for (int i = 0; i < array.count; ++i)
    // {
    //     printf("DA VAL %i\n", tpIntArrayAt(&array, i));
    // }

    // assert(array.count == 3);
    // assert(tpIntArrayAt(&array, 0) == 3);
    // assert(tpIntArrayAt(&array, 1) == 10);
    // assert(tpIntArrayAt(&array, 2) == 2);

    // tpIntArrayDeallocate(&array);

    //create the window
    GLFWwindow * window = glfwCreateWindow(800, 600, "Basic Tarp Example", NULL, NULL);
    if (window)
    {
        glfwMakeContextCurrent(window);

        tpContext ctx;
        tpBool err = tpContextInit(&ctx);
        if (err)
        {
            printf("COULT NOT INIT TARP CONTEXT %s\n", ctx.errorMessage);
            return EXIT_FAILURE;
        }

        tpPath path = tpPathCreate(&ctx);
        tpPathMoveTo(path, 10, 10);
        tpPathCubicCurveTo(path, 40, 40, 100, 15, 100, 100);
        tpPathCubicCurveTo(path, 115, 115, 50, 150, 50, 150);
        tpPathClose(path);
        // tpPathMoveTo(path, 50, 50);
        // tpPathLineTo(path, 50, 70);
        // tpPathLineTo(path, 30, 70);
        // tpPathLineTo(path, 30, 50);
        // tpPathClose(path);

        tpStyle style = tpStyleCreate(&ctx);
        tpStyleSetStrokeWidth(style, 10);
        tpStyleSetStrokeJoin(style, kTpStrokeJoinBevel);
        tpStyleSetFillColor(style, 1.0, 1.0, 0.0, 1.0);
        // printf("WE GOT %lu\n", tpPathSegmentCount(path));


        // tpVec2 a = tpVec2Make(10, 20);
        // tpVec2 b = tpVec2Make(2, 4);

        // tpVec2 c = tpVec2Add(&a, &b);
        // printf("%f %f\n", a.x, a.y);
        // printf("%f %f\n", b.x, b.y);
        // printf("%f %f\n", c.x, c.y);

        // tpMat3 mat = tpMat3Identity();
        // printf("%f %f %f\n%f %f %f\n%f %f %f\n", mat.v[0], mat.v[1], mat.v[2],
        //        mat.v[3], mat.v[4], mat.v[5],
        //        mat.v[6], mat.v[7], mat.v[8]);

        // tpVec2 c2 = tpMat3MultVec2(&mat, &c);
        // printf("%f %f\n", c2.x, c2.y);

        // tpGradient * grad = tpGradientCreateLinear(0, 0, 100, 100);
        // tpGradientAddColorStop(grad, 1, 0, 0, 1.0, 0.1);
        // tpGradientAddColorStop(grad, 1, 0, 1, 1.0, 0.9);
        // printf("COUNTS %i\n", grad->stopCount);

        // tpStyle style = tpStyleMake();
        // tpStyleSetFillGradient(&style, grad);
        // tpStyleSetStrokeWidth(&style, 4.0);

        // tpPath * p = tpPathCreate();
        // tpPathDestroy(p);

        // the main loop
        while (!glfwWindowShouldClose(window))
        {
            // clear the background to black
            glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            // get the window size from the glfw window
            // and set it as the viewport on the renderer
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            glViewport(0, 0, width, height);

            tpMat4 proj = tpMat4Ortho(0, 800, 600, 0, -1, 1);
            tpSetProjection(&ctx, &proj);
            tpDrawPath(&ctx, path, style);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        tpContextDeallocate(&ctx);
    }
    else
    {
        glfwTerminate();
        printf("Could not open GLFW window :(\n");
        return EXIT_FAILURE;
    }

    // clean up glfw
    glfwDestroyWindow(window);
    glfwTerminate();
}
