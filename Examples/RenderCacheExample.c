/* include opengl */
#include <GL/gl3w.h>

/* we use GLFW to open a simple window */
#include <GLFW/glfw3.h>

/* tell Tarp to compile the opengl implementations */
#define TARP_IMPLEMENTATION_OPENGL
#include <Tarp/Tarp.h>


int i;
tpBool err;
const int rcCount = 128;
tpRenderCache caches[rcCount];

static float randomFloat(float _a, float _b)
{
    float ret = (float)rand() / (float)RAND_MAX;
    return _a + (_b - _a) * ret;
}

int main(int argc, char * argv[])
{
    //randomize the random seed
    //NOTE: Screw rand, we just use it for the simplicity/portability of this example
    srand (time(NULL));

    /* this example is compile in pedantic c89, so we declare the variables up here */
    tpContext ctx;
    GLFWwindow * window;

    tpPath path;
    tpStyle style;
    tpMat4 proj;
    tpGradient grad;
    int wwidth, wheight;
    tpFloat animationTimer;

    /* initialize glfw */
    if (!glfwInit())
        return EXIT_FAILURE;

    /* and set some hints to get the correct opengl versions/profiles */
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    /* create the window */
    window = glfwCreateWindow(800, 600, "Hello Tarp", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        printf("Could not open GLFW window :(\n");
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    /* init opengl function pointers */
    if (gl3wInit())
    {
        fprintf(stderr, "failed to initialize OpenGL\n");
        return EXIT_FAILURE;
    }

    /* initialize the tarp context */
    ctx = tpContextCreate();
    if (!tpContextIsValidHandle(ctx))
    {
        printf("Could not init Tarp context: %s\n", tpErrorMessage());
        return EXIT_FAILURE;
    }

    /* set an orthographic projection on the context based on the window size */
    glfwGetWindowSize(window, &wwidth, &wheight);

    /* set an orthographic projection based on the window size */
    proj = tpMat4MakeOrtho(0, wwidth, wheight, 0, -1, 1);
    tpSetProjection(ctx, &proj);

    /* create a path and add one circle contour */
    path = tpPathCreate();
    tpPathAddCircle(path, 0, 0, 100);

    /* add another custom contour to the path */
    tpPathMoveTo(path, 0, 20);
    tpPathLineTo(path, 20, -20);
    tpPathQuadraticCurveTo(path, 0, -40, 80, -20);
    tpPathClose(path); /* close the contour */

    /* create a gradient */
    grad = tpGradientCreateRadialSymmetric(0, 0, 80);
    tpGradientAddColorStop(grad, 1.0, 1.0, 0.0, 1.0, 0.0);
    tpGradientAddColorStop(grad, 1.0, 0.0, 1.0, 1.0, 0.75);
    tpGradientAddColorStop(grad, 0.0, 0.0, 1.0, 1.0, 1.0);

    /* create a style that we can draw the path with */
    style = tpStyleMake();
    style.fill = tpPaintMakeGradient(grad);
    // style.fill = tpPaintMakeColor(1.0, 1.0, 0.0, 1.0);
    style.stroke = tpPaintMakeColor(1.0, 0.6, 0.1, 1.0);
    // style.stroke.type = kTpPaintTypeNone;
    style.strokeWidth = 10.0;
    style.strokeJoin = kTpStrokeJoinRound;

    /* now create many cached veriations of the path in question so we can render it efficiently */
    for(i = 0; i < rcCount; ++i)
    {
        tpTransform transform, scale, rot;
        tpFloat s = randomFloat(0.1, 0.6);

        caches[i] = tpRenderCacheCreate();
        scale = tpTransformMakeScale(s, s);
        rot = tpTransformMakeRotation(randomFloat(0, 3.14));
        transform = tpTransformMakeTranslation(randomFloat(0, 800), randomFloat(0, 600));

        scale = tpTransformCombine(&transform, &scale);
        transform = tpTransformCombine(&scale, &rot);

        /* change the focal point for each cached path*/
        if(randomFloat(0, 1) >= 0.5)
        {
            style.fill = tpPaintMakeGradient(grad);
            tpGradientSetFocalPointOffset(grad, randomFloat(-60, 60), randomFloat(-40, 40));
        }
        else
            style.fill= tpPaintMakeColor(randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1);

        /* randomize the style a lil for each one */
        style.stroke = tpPaintMakeColor(randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1);
        style.strokeWidth = randomFloat(2, 20);

        tpSetTransform(ctx, &transform);
        err = tpCachePath(ctx, path, &style, caches[i]);
        if(err)
        {
            fprintf(stderr, "Failed to create render cache\n");
            return EXIT_FAILURE;
        }
    }

    /* the main loop */
    while (!glfwWindowShouldClose(window))
    {
        printf("DRAW\n");
        int width, height;
        tpTransform trans, rot;

        glClearColor(0.0, 0.0, 0.0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        /* set the viewport based on the pixel dimensions of the window */
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        /* call this at the beginning of your frame */
        tpPrepareDrawing(ctx);

        //  draw the path with our style 
        // tpDrawPath(ctx, path, &style);

        for(i = 0; i < rcCount; ++i)
        {
            tpDrawRenderCache(ctx, caches[i]);
        }

        /* call this when you are done with Tarp for the frame */
        tpFinishDrawing(ctx);

        glfwSwapBuffers(window);
        glfwPollEvents();

        animationTimer += 0.1;
    }

    /* clean up tarp */
    tpGradientDestroy(grad);
    tpPathDestroy(path);
    tpContextDestroy(ctx);

    /* clean up glfw */
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
