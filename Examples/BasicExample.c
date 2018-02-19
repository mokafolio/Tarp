// tell Tarp to include the opengl implementation
#define TARP_INCLUDE_OPENGL_IMPLEMENTATION
// tell Tarp to also include function definitions
#define TARP_IMPLEMENTATION
#include <Tarp/Tarp.h>

// we use GLFW to open a simple window
#include <GLFW/glfw3.h>

// for timing
#include <sys/time.h>

//for rand
#include <stdlib.h>

typedef void(*RepeatingFn)(void *);

typedef struct
{
    RepeatingFn callback;
    tpFloat interval;
    struct timeval lastCallbackTime;
    void * userData;
} RepeatingCallback;

static RepeatingCallback makeRepeatingCallback(RepeatingFn _callback, tpFloat _interval, void * _userData)
{
    return (RepeatingCallback) {_callback, _interval, {}, _userData};
}

static void updateRepeatingCallback(RepeatingCallback * _cb)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    double ns = now.tv_sec + (double)now.tv_usec / 1000000.0;
    double ls = _cb->lastCallbackTime.tv_sec + (double)_cb->lastCallbackTime.tv_usec / 1000000.0;
    if (ns - ls >= _cb->interval)
    {
        _cb->callback(_cb->userData);
        _cb->lastCallbackTime = now;
    }
}

typedef struct
{
    tpPath path;
    tpStyle style;
} PathWithStyle;

typedef struct
{
    tpPath path;
    tpStyle style;
    RepeatingCallback cb;
} PathWithStyleAndCallback;

static float randomFloat(float _a, float _b)
{
    float ret = (float)rand() / (float)RAND_MAX;
    return _a + (_b - _a) * ret;
}

static void updateStrokeOffsetDrawing(void * _userData)
{
    PathWithStyleAndCallback * drawing = (PathWithStyleAndCallback *) _userData;
    tpStyleSetDashOffset(drawing->style, tpStyleDashOffset(drawing->style) + 0.2);
}

static void updateStarFillRuleDrawing(void * _userData)
{
    PathWithStyleAndCallback * starDrawing = (PathWithStyleAndCallback *) _userData;
    tpFillRule fr = tpStyleFillRule(starDrawing->style);
    tpStyleSetFillRule(starDrawing->style, fr == kTpFillRuleEvenOdd ? kTpFillRuleNonZero : kTpFillRuleEvenOdd);
}

static void updateRandomDrawing(void * _userData)
{
    PathWithStyleAndCallback * drawing = (PathWithStyleAndCallback *) _userData;
    tpPathClear(drawing->path);
    tpPathMoveTo(drawing->path, randomFloat(350, 450), randomFloat(50, 150));
    for (int i = 0; i < ((int)randomFloat(3, 12)); ++i)
    {
        // tpPathLineTo(drawing->path, randomFloat(350, 450), randomFloat(50, 150));
        tpPathQuadraticCurveTo(drawing->path, randomFloat(350, 450), randomFloat(50, 150), randomFloat(350, 450), randomFloat(50, 150));
    }
    tpStyleSetStrokeWidth(drawing->style, randomFloat(1, 6));
    tpStyleSetStrokeColor(drawing->style, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0);
    tpStyleSetFillColor(drawing->style, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0);
}

static void updateRotatingDrawing(void * _userData)
{
    static tpFloat rot = 0;
    static tpFloat rotTimer = 0;
    PathWithStyleAndCallback * drawing = (PathWithStyleAndCallback *) _userData;
    tpMat3 transform = tpMat3MakeTranslation(100, 250);
    tpMat3 rotMat = tpMat3MakeRotation(rot);
    transform = tpMat3Mult(&transform, &rotMat);
    tpPathSetTransform(drawing->path, &transform);
    rotTimer += 0.1;
    rot += (sin(rot) + 2.0) * 0.5 * 0.05;
}

static void updateWormDrawing(void * _userData)
{
    static tpFloat rot = 0;
    PathWithStyleAndCallback * drawing = (PathWithStyleAndCallback *) _userData;
    tpMat3 transform = tpMat3MakeTranslation(400, 250);
    tpMat3 rotMat = tpMat3MakeRotation(rot);
    transform = tpMat3Mult(&transform, &rotMat);
    tpPathSetStrokePaintTransform(drawing->path, &transform);
    rot += 0.1;
}

int main(int argc, char * argv[])
{
    //randomize the random seed
    //NOTE: Fuck rand, we just use it for the simplicity/portability of this example
    srand (time(NULL));

    // initialize glfw
    if (!glfwInit())
        return EXIT_FAILURE;

    // and set some hints to get the correct opengl versions/profiles
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create the window
    GLFWwindow * window = glfwCreateWindow(1280, 720, "Basic Tarp Example", NULL, NULL);
    if (window)
    {
        glfwMakeContextCurrent(window);

        int wwidth, wheight;
        glfwGetWindowSize(window, &wwidth, &wheight);

        //use the Tarp OpenGL implementation
        tpSetImplementation(tpOpenGLImplementation());

        //initialize the tarp context
        tpContext ctx;
        tpBool err = tpContextInit(&ctx);
        if (err)
        {
            printf("Could not init Tarp context: %s\n", tpContextErrorMessage(&ctx));
            return EXIT_FAILURE;
        }

        //set an orthographic projection on the context based on the window size;
        tpMat4 proj = tpMat4MakeOrtho(0, wwidth, wheight, 0, -1, 1);
        tpSetProjection(&ctx, &proj);

        //create the dash offset drawing
        PathWithStyleAndCallback dashOffsetDrawing;
        {
            tpPath path = tpPathCreate();
            tpPathAddCircle(path, 100, 100, 50);
            tpPathAddCircle(path, 100, 100, 25);

            tpStyle style = tpStyleCreate();
            tpFloat dashArray[] = {10, 20};
            tpStyleSetDashArray(style, dashArray, 2);
            tpStyleSetStrokeWidth(style, 8);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinRound);
            tpStyleSetStrokeCap(style, kTpStrokeCapRound);
            tpStyleSetFillColor(style, 1, 1, 0, 1);

            dashOffsetDrawing.style = style;
            dashOffsetDrawing.path = path;
            dashOffsetDrawing.cb = makeRepeatingCallback(updateStrokeOffsetDrawing, 0.0, &dashOffsetDrawing);
        }

        //init the star drawing to show EvenOdd vs NonZero fill rule
        PathWithStyleAndCallback starDrawing;
        {
            tpPath path = tpPathCreate();
            tpPathMoveTo(path, 250, 50);
            tpPathLineTo(path, 225, 150);
            tpPathLineTo(path, 300, 100);
            tpPathLineTo(path, 200, 100);
            tpPathLineTo(path, 275, 150);
            tpPathClose(path);

            tpStyle style = tpStyleCreate();
            tpStyleSetFillColor(style, 0.4, 0.6, 1.0, 1.0);
            tpStyleSetStrokeColor(style, 1.0, 0.0, 1.0, 1.0);
            tpStyleSetStrokeWidth(style, 6.0);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinMiter);
            tpStyleSetMiterLimit(style, 25);

            starDrawing.path = path;
            starDrawing.style = style;
            starDrawing.cb = makeRepeatingCallback(updateStarFillRuleDrawing, 1.0, &starDrawing);
        }

        //path that randomly regenerates every half second
        PathWithStyleAndCallback randomDrawing;
        {
            tpPath path = tpPathCreate();

            tpStyle style = tpStyleCreate();
            tpStyleSetStrokeCap(style, kTpStrokeCapRound);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinRound);

            randomDrawing.path = path;
            randomDrawing.style = style;
            randomDrawing.cb = makeRepeatingCallback(updateRandomDrawing, 0.5, &randomDrawing);
        }

        PathWithStyleAndCallback rotatingDrawing;
        {
            tpPath path = tpPathCreate();
            tpPathAddRect(path, -50, -50, 100, 100);
            tpPathMoveTo(path, -20, -20);
            tpPathQuadraticCurveTo(path, 0, 20, 20, 20);
            tpPathQuadraticCurveTo(path, 0, -20, -20, -20);
            tpPathClose(path);

            tpStyle style = tpStyleCreate();
            tpStyleSetFillColor(style, 1.0, 0.7, 0.1, 1.0);
            tpStyleSetStrokeWidth(style, 5.0);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinMiter);

            rotatingDrawing.path = path;
            rotatingDrawing.style = style;
            rotatingDrawing.cb = makeRepeatingCallback(updateRotatingDrawing, 0, &rotatingDrawing);
        }

        PathWithStyleAndCallback wormDrawing;
        {
            tpPath path = tpPathCreate();
            tpPathMoveTo(path, 200, 200);
            tpFloat xoff = 200;
            for (int i = 0; i < 2; ++i)
            {
                // tpPathCubicCurveTo(path, 200 + TARP_KAPPA * 50, 200, 250, 250 - TARP_KAPPA * 50, 250, 250);
                // tpPathCubicCurveTo(path, 250, 250 + TARP_KAPPA * 50, 300 - TARP_KAPPA * 50, 300, 300, 300);
                // tpPathCubicCurveTo(path, 300 + TARP_KAPPA * 50, 300, 350, 250 + TARP_KAPPA * 50, 350, 250);
                // tpPathCubicCurveTo(path, 350, 250 - TARP_KAPPA * 50, 400 - TARP_KAPPA * 50, 200, 400, 200);

                tpPathCubicCurveTo(path, xoff + TARP_KAPPA * 50, 200, xoff + 50, 250 - TARP_KAPPA * 50, xoff + 50, 250);
                tpPathCubicCurveTo(path, xoff + 50, 250 + TARP_KAPPA * 50, xoff + 100 - TARP_KAPPA * 50, 300, xoff + 100, 300);
                tpPathCubicCurveTo(path, xoff + 100 + TARP_KAPPA * 50, 300, xoff + 150, 250 + TARP_KAPPA * 50, xoff + 150, 250);
                tpPathCubicCurveTo(path, xoff + 150, 250 - TARP_KAPPA * 50, xoff + 200 - TARP_KAPPA * 50, 200, xoff + 200, 200);
                xoff += 200;
            }

            // tpPathClose(path);

            tpStyle style = tpStyleCreate();
            tpStyleRemoveFill(style);
            tpStyleSetStrokeWidth(style, 20.0);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinRound);
            tpStyleSetStrokeCap(style, kTpStrokeCapRound);

            tpGradient grad = tpGradientCreateLinear(-200, -50, 200, 50);
            tpFloat off = 0.0;
            while(off < 1.0)
            {
                tpGradientAddColorStop(grad, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0, off);
                off += randomFloat(0.1, 0.3);
            }

            tpStyleSetStrokeGradient(style, grad);

            wormDrawing.path = path;
            wormDrawing.style = style;
            wormDrawing.cb = makeRepeatingCallback(updateWormDrawing, 0.0, &wormDrawing);
        }

        // the main loop
        while (!glfwWindowShouldClose(window))
        {
            // clear the background to a color
            // glClearColor(0.4, 0.45, 0.43, 1);
            glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            // get the window size from the glfw window
            // and set it as the viewport on the renderer
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            glViewport(0, 0, width, height);

            tpPrepareDrawing(&ctx);

            //update and draw the dash offset drawing
            updateRepeatingCallback(&dashOffsetDrawing.cb);
            tpDrawPath(&ctx, dashOffsetDrawing.path, dashOffsetDrawing.style);

            //update and draw the star
            updateRepeatingCallback(&starDrawing.cb);
            tpDrawPath(&ctx, starDrawing.path, starDrawing.style);

            //update and draw the random path
            updateRepeatingCallback(&randomDrawing.cb);
            tpDrawPath(&ctx, randomDrawing.path, randomDrawing.style);

            //update and draw the random path
            updateRepeatingCallback(&rotatingDrawing.cb);
            tpDrawPath(&ctx, rotatingDrawing.path, rotatingDrawing.style);

            updateRepeatingCallback(&wormDrawing.cb);
            tpDrawPath(&ctx, wormDrawing.path, wormDrawing.style);

            tpFinishDrawing(&ctx);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        //clean up tarp related things
        // tpPathDestroy(circle);
        // tpStyleDestroy(defaultStyle);
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
