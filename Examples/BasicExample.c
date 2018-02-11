#include <Tarp/TarpGL.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// we use GLFW to open a simple window
#include <GLFW/glfw3.h>

// #define TARP_STATIC_ASSERT(pred) switch(0){case 0:case pred:;}

// #define TARP_COMPILER_CLANG 0
// #define TARP_COMPILER_GCC 1

// #if defined(__clang__)
// #define TARP_COMPILER TARP_COMPILER_CLANG
// #elif defined(__GNUC__)
// #define TARP_COMPILER TARP_COMPILER_GCC
// #endif

// #if TARP_COMPILER == TARP_COMPILER_CLANG || TARP_COMPILER == TARP_COMPILER_GCC
// #define TARP_POINTER_SIZE __SIZEOF_POINTER__
// #define TARP_INT_SIZE __SIZEOF_INT__
// #else
// #error "Can't detect pointer/integer size at preprocessor stage on your platform :("
// #endif

// #if TARP_POINTER_SIZE == TARP_INT_SIZE * 2
// #define TARP_HANDLE_FITS_POINTER
// #endif

// // TARP_DECLARE_ARRAY(tpIntArray, int);
// #define _TARP_ARRAY_T tpIntArray
// #define _TARP_ITEM_T int
// #include <Tarp/TarpArray.h>

#include <time.h>

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

typedef struct
{
    float x, y;
} SomeVect;

static SomeVect addByValue(SomeVect _a, SomeVect _b)
{
    return (SomeVect) {_a.x + _b.x, _a.y + _b.y};
}

static SomeVect addByPtr(const SomeVect * _a, const SomeVect * _b)
{
    return (SomeVect) {_a->x + _b->x, _a->y + _b->y};
}

static void addByPtr2(const SomeVect * _a, const SomeVect * _b, SomeVect * _result)
{
    _result->x = _a->x + _b->x;
    _result->y = _a->y + _b->y;
}

typedef float SomeVec2[2];

static void addWeird(const SomeVec2 _a, const SomeVec2 _b, SomeVec2 _result)
{
    _result[0] = _a[0] + _b[0];
    _result[1] = _a[1] + _b[1];
}

static float randomFloat(float _a, float _b)
{
    float ret = (float)rand() / (float)RAND_MAX;
    return _a + (_b - _a) * ret;
}

int main(int argc, char * argv[])
{
    // COMPILE_TIME_ASSERT(sizeof(Handle) != sizeof(void*));

    // clock_t start, end;
    // double cpu_time_used;

    // SomeVect adder = {0.1, 0.1};
    // start = clock();
    // SomeVect sval = {0, 0};
    // for (size_t i = 0; i < 10000000000; ++i)
    // {
    //     sval = addByValue(sval, adder);
    // }
    // end = clock();
    // cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    // printf("TIME USED BY VALUE %f %f %f\n", cpu_time_used, sval.x, sval.y);

    // start = clock();
    // SomeVect sval3 = {0, 0};
    // for (size_t i = 0; i < 10000000000; ++i)
    // {
    //     sval3 = addByPtr(&sval3, &adder);
    // }
    // end = clock();
    // cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    // printf("TIME USED BY VALUE %f %f %f\n", cpu_time_used, sval3.x, sval3.y);

    // SomeVec2 adder2 = {0.1, 0.1};
    // start = clock();
    // SomeVec2 sval2 = {0, 0};
    // for (size_t i = 0; i < 10000000000; ++i)
    // {
    //     addWeird(sval2, adder2, sval2);
    // }
    // end = clock();
    // cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    // printf("TIME USED BY WEIRD %f %f %f\n", cpu_time_used, sval2[0], sval2[1]);

    // return EXIT_SUCCESS;

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

    // create the window
    GLFWwindow * window = glfwCreateWindow(1280, 720, "Basic Tarp Example", NULL, NULL);
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

        // tpPath path = tpPathCreate(&ctx);
        // tpPathMoveTo(path, 100, 105);
        // tpPathLineTo(path, 200, 110);
        // tpPathLineTo(path, 200, 200);
        // tpPathLineTo(path, 300, 170);
        // tpPathClose(path);

        tpPath path = tpPathCreate(&ctx);
        tpPathMoveTo(path, 100, 100);
        tpPathLineTo(path, 200, 100);
        tpPathLineTo(path, 200, 200);
        tpPathLineTo(path, 100, 200);
        tpPathClose(path);

        tpPathAddCircle(path, 150, 150, 30);

        tpPath clipPath = tpPathCreate(&ctx);
        tpPathAddCircle(clipPath, 150, 150, 50);

        tpPath clipPath2 = tpPathCreate(&ctx);
        tpPathAddRect(clipPath2, 100, 140, 100, 40);

        tpPath clipPath3 = tpPathCreate(&ctx);
        tpPathAddRect(clipPath3, 120, 100, 30, 100);

        tpPath anotherOne = tpPathCreate(&ctx);
        tpPathAddCircle(anotherOne, 250, 150, 80);

        tpPath circle = tpPathCreate(&ctx);
        tpPathAddCircle(circle, 450, 150, 80);

        tpPath star = tpPathCreate(&ctx);

        tpPathMoveTo(star, 500, 200);
        tpPathLineTo(star, 600, 200);
        tpPathLineTo(star, 525, 300);
        tpPathLineTo(star, 550, 150);
        tpPathLineTo(star, 575, 300);
        tpPathClose(star);

        tpPath starOnCircle = tpPathCreate(&ctx);
        tpPathAddCircle(starOnCircle, 550, 225, 50);

        // tpPathAddRect(path, 100, 100, 200, 100);
        // tpPathAddCircle(path, 100, 100, 50);
        // tpPathAddCircle(path, 100, 100, 20);

        // tpPathMoveTo(path, 150, 150);
        // // tpPathCubicCurveTo(path, 155, 144, 160, 170, 165, 150);
        // tpPathLineTo(path, 165, 150);
        // tpPathLineTo(path, 180, 180);
        // tpPathLineTo(path, 140, 180);
        // tpPathClose(path);



        tpStyle style = tpStyleCreate(&ctx);
        tpStyleSetStrokeWidth(style, 8);
        // tpStyleSetMiterLimit(style, 64);
        tpStyleSetStrokeJoin(style, kTpStrokeJoinMiter);
        // tpStyleSetStrokeJoin(style, kTpStrokeJoinRound);
        tpStyleSetStrokeCap(style, kTpStrokeCapRound);
        tpStyleSetFillColor(style, 1.0, 0.0, 0.0, 1.0);

        tpFloat dashArray[] = {10, 20};
        tpStyleSetDashArray(style, dashArray, 2);

        tpStyleSetDashOffset(style, -31);


        tpGradient grad = tpGradientCreateLinear(&ctx, 100, 100, 200, 200);
        tpGradientAddColorStop(grad, 0, 0, 0, 1, 0.0);
        tpGradientAddColorStop(grad, 1, 1, 0, 1, 1.0);
        tpStyleSetFillGradient(style, grad);

        tpGradient grad2 = tpGradientCreateLinear(&ctx, 100, 150, 200, 150);
        tpGradientAddColorStop(grad2, 1, 1, 0, 1, 0.0);
        tpGradientAddColorStop(grad2, 1, 0, 1, 1, 0.25);
        tpGradientAddColorStop(grad2, 0, 1, 1, 1, 1.0);
        tpStyleSetStrokeGradient(style, grad2);

        tpMat3 scal = tpMat3MakeScale(2.0, 2.0);
        tpMat3 rot = tpMat3MakeRotation(TARP_PI / 4);
        tpMat3 transl = tpMat3MakeTranslation(-150, -150);
        tpMat3 transl2 = tpMat3MakeTranslation(150, 150);
        tpMat3 skew = tpMat3MakeSkew(TARP_PI / 10, 0);
        tpMat3 ident = tpMat3MakeIdentity();
        tpMat3 trans = tpMat3Mult(&scal, &transl);
        trans = tpMat3Mult(&transl2, &trans);

        tpMat4 m4 = tpMat4MakeFrom2DTransform(&trans);
        printf("MAT4 (%f %f %f %f,\n %f %f %f %f, \n %f %f %f %f, \n %f %f %f %f)\n",
               m4.v[0], m4.v[4], m4.v[8], m4.v[12],
               m4.v[1], m4.v[5], m4.v[9], m4.v[13],
               m4.v[2], m4.v[6], m4.v[10], m4.v[14],
               m4.v[3], m4.v[7], m4.v[11], m4.v[15]
              );

        tpStyle simple = tpStyleCreate(&ctx);
        tpStyleSetFillColor(simple, 1.0, 0.5, 0.3, 1.0);
        tpStyleSetFillRule(simple, kTpFillRuleNonZero);

        tpPath circle2 = tpPathCreate(&ctx);
        tpPathAddCircle(circle2, 100, 200, 60);
        tpMat3 scal2 = tpMat3MakeScale(2.0, 2.0);
        // tpPathSetTransform(circle2, &scal2);
        tpStyle nonScaling = tpStyleCreate(&ctx);
        tpStyleSetScaleStroke(nonScaling, tpFalse);

        // tpPathSetTransform(path, &skew);

        // printf("WE GOT %lu\n", tpPathSegmentCount(path));


        // tpVec2 a = tpVec2Make(10, 20);
        // tpVec2 b = tpVec2Make(2, 4);

        // tpVec2 c = tpVec2Add(&a, &b);
        // printf("%f %f\n", a.x, a.y);
        // printf("%f %f\n", b.x, b.y);
        // printf("%f %f\n", c.x, c.y);

        // tpMat3 mat = tpMat3MakeIdentity();
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

        tpFloat off = 0.0;
        tpFloat scoff = 0.0;
        int counter = 0;
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

            // tpStyleSetDashOffset(style, off);
            // off -= 0.05;

            tpMat4 proj = tpMat4MakeOrtho(0, 1280, 720, 0, -1, 1);
            tpSetProjection(&ctx, &proj);
            tpPrepareDrawing(&ctx);
            tpBeginClipping(&ctx, clipPath);
            tpBeginClipping(&ctx, clipPath2);
            tpBeginClipping(&ctx, clipPath3);
            tpDrawPath(&ctx, path, style);
            tpEndClipping(&ctx);
            tpEndClipping(&ctx);
            tpEndClipping(&ctx);
            // tpResetClipping(&ctx);

            tpDrawPath(&ctx, anotherOne, style);

            // tpDrawPath(&ctx, star, simple);
            tpBeginClipping(&ctx, star);
            tpDrawPath(&ctx, starOnCircle, simple);
            tpEndClipping(&ctx);

            tpDrawPath(&ctx, circle2, nonScaling);

            tpFinishDrawing(&ctx);

            tpFloat s = 1.0 + (sin(scoff) + 1.0) * 0.5;
            tpMat3 sm = tpMat3MakeScale(s, s);
            tpPathSetTransform(circle2, &sm);
            scoff += 0.01;

            glfwSwapBuffers(window);
            glfwPollEvents();

            counter++;

            if (counter >= 60)
            {
                float a = randomFloat(100, 200);
                float b = randomFloat(100, 200);
                float c = randomFloat(100, 200);
                float d = randomFloat(100, 200);
                printf("DA GRAD BABYBBB %f %f %f %f\n", a, b, c, d);
                tpGradientDestroy(grad);

                grad = tpGradientCreateLinear(&ctx, a, b, c, d);


                tpGradientAddColorStop(grad, 1, 1, 0, 1, 0.0);
                tpGradientAddColorStop(grad, 1, 0, 1, 1, 0.25);
                tpGradientAddColorStop(grad, 0, 1, 1, 1, 1.0);
                tpStyleSetFillGradient(style, grad);

                counter = 0;
            }

            // tpStyleRemoveStroke(style);
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

    return EXIT_SUCCESS;
}
