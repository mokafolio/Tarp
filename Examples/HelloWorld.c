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
    
    tpPath path;
    tpStyle style;
    tpMat4 proj;
    tpGradient grad;
    int wwidth, wheight;

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
    tpPathAddCircle(path, 400, 300, 100);

    /* create a gradient */
    grad = tpGradientCreateRadialSymmetric(400, 300, 80);
    tpGradientAddColorStop(grad, 1.0, 1.0, 0.0, 1.0, 0.0);
    tpGradientAddColorStop(grad, 1.0, 0.0, 1.0, 1.0, 0.75);
    tpGradientAddColorStop(grad, 0.0, 0.0, 1.0, 1.0, 1.0); 

    /* add another custom contour to the path */
    tpPathMoveTo(path, 400, 320);
    tpPathLineTo(path, 420, 280);
    tpPathQuadraticCurveTo(path, 400, 260, 380, 280);
    tpPathClose(path); /* close the contour */

    /* create a style that we can draw the path with */
    style = tpStyleCreate();
    tpStyleSetFillGradient(style, grad);
    tpStyleSetStrokeColor(style, 1.0, 0.6, 0.1, 1.0);
    tpStyleSetStrokeWidth(style, 10.0);
    tpStyleSetStrokeJoin(style, kTpStrokeJoinRound);

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

        /* draw the path with our style */
        tpDrawPath(ctx, path, style);

        /* call this when you are done with Tarp for the frame */
        tpFinishDrawing(ctx);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /* clean up tarp */
    tpStyleDestroy(style);
    tpGradientDestroy(grad);
    tpPathDestroy(path);
    tpContextDestroy(ctx);

    /* clean up glfw */
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
