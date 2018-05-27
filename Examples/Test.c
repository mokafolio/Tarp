/* include opengl */
#include <GL/gl3w.h>

/* we use GLFW to open a simple window */
#include <GLFW/glfw3.h>

/* tell Tarp to compile the opengl implementations */
#define TARP_IMPLEMENTATION_OPENGL
#include <Tarp/Tarp.h>


int main(int argc, char * argv[])
{
    /* this example is compile in pedantic c89, so we declare the variables up here */
    tpContext ctx;
    GLFWwindow * window;

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
    window = glfwCreateWindow(512, 512, "Tarp Test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        printf("Could not open GLFW window :(\n");
        return EXIT_FAILURE;
    }
    
    tpPath path;
    tpStyle round_style, bevel_style, miter_style, red_style;
    tpMat4 proj;
    tpTransform transform;
    int wwidth, wheight;

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
        printf("Could not init Tarp context: %s\n", tpContextErrorMessage(ctx));
        return EXIT_FAILURE;
    }

    /* set an orthographic projection on the context based on the window size */
    glfwGetWindowSize(window, &wwidth, &wheight);

    /* set an orthographic projection based on the window size */
    proj = tpMat4MakeOrtho(0, wwidth, wheight, 0, -1, 1);
    tpSetProjection(ctx, &proj);

    /* create a path and add one circle contour */
    path = tpPathCreate();
    tpPathMoveTo(path, -32,   0);
    tpPathLineTo(path, -16,   0);
    tpPathLineTo(path,   0,  16);
    tpPathLineTo(path,   0, -16);
    tpPathLineTo(path,  16,   0);
    tpPathLineTo(path,  32,   0);

    round_style = tpStyleCreate();
    tpStyleSetFillColor(round_style, 0, 0, 0, 0);
    tpStyleSetStrokeColor(round_style, 1, 1, 1, 1);
    tpStyleSetStrokeWidth(round_style, 16);
    tpStyleSetStrokeJoin(round_style, kTpStrokeJoinRound);
    tpStyleSetStrokeCap(round_style, kTpStrokeCapRound);
    
    bevel_style = tpStyleClone(round_style);
    tpStyleSetStrokeJoin(bevel_style, kTpStrokeJoinBevel);
    tpStyleSetStrokeCap(bevel_style, kTpStrokeCapButt);
    
    miter_style = tpStyleClone(round_style);
    tpStyleSetStrokeJoin(miter_style, kTpStrokeJoinMiter);
    tpStyleSetStrokeCap(miter_style, kTpStrokeCapSquare);
    
    red_style = tpStyleClone(bevel_style);
    tpStyleSetStrokeWidth(red_style, 2);
    tpStyleSetStrokeColor(red_style, 1, 0, 0, 1);

    /* the main loop */
    while (!glfwWindowShouldClose(window))
    {
        int width, height;

        glClearColor(0.0, 0.0, 0.0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        /* set the viewport based on the pixel dimensions of the window */
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        /* call this at the beginning of your frame */
        tpPrepareDrawing(ctx);

        transform = tpTransformMakeTranslation(64, 32);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, path, round_style);
        tpDrawPath(ctx, path, red_style);
        
        transform = tpTransformMakeTranslation(64, 96);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, path, bevel_style);
        tpDrawPath(ctx, path, red_style);
        
        transform = tpTransformMakeTranslation(64, 160);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, path, miter_style);
        tpDrawPath(ctx, path, red_style);

        /* call this when you are done with Tarp for the frame */
        tpFinishDrawing(ctx);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /* clean up tarp */
    tpContextDestroy(ctx);

    /* clean up glfw */
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
