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

    tpPath clipPath, clipPath2, clipPath3, path;
    tpStyle style;
    tpTransform trans;
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
        printf("Could not init Tarp context: %s\n", tpErrorMessage());
        return EXIT_FAILURE;
    }

    /* set an orthographic projection on the context based on the window size */
    glfwGetWindowSize(window, &wwidth, &wheight);

    /* set an orthographic projection based on the window size */
    proj = tpMat4MakeOrtho(0, wwidth, wheight, 0, -1, 1);
    tpSetProjection(ctx, &proj);

    /* create a path and add one circle contour */
    clipPath = tpPathCreate();
    tpPathAddCircle(clipPath, 400, 300, 100);

    /* add another custom contour to the path */
    tpPathMoveTo(clipPath, 400, 320);
    tpPathLineTo(clipPath, 420, 280);
    tpPathQuadraticCurveTo(clipPath, 400, 260, 380, 280);
    tpPathClose(clipPath); /* close the contour */

    clipPath2 = tpPathCreate();
    tpPathAddRect(clipPath2, 300, 300, 200, 30);

    clipPath3 = tpPathCreate();
    tpPathAddRect(clipPath3, 400, 200, 50, 200);

    path = tpPathCreate();
    tpPathAddCircle(path, 400, 300, 50);

    /* create a style that we can draw the path with */
    style = tpStyleMake();
    style.fill = tpPaintMakeColor(1, 0, 0, 1);
    style.stroke.type = kTpPaintTypeNone;

    /* the main loop */
    while (!glfwWindowShouldClose(window))
    {
        int width, height;

        glClearColor(1.0, 1.0, 0.0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        /* set the viewport based on the pixel dimensions of the window */
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        /* call this at the beginning of your frame */
        tpPrepareDrawing(ctx);
        tpResetTransform(ctx);
        /* draw the path with our style */
        tpBeginClipping(ctx, clipPath);
        tpBeginClipping(ctx, clipPath2);
        tpBeginClipping(ctx, clipPath3);
        tpDrawPath(ctx, path, &style);
        tpEndClipping(ctx);

        trans = tpTransformMakeTranslation(0, 90);
        tpSetTransform(ctx, &trans);
        tpDrawPath(ctx, path, &style);
        tpEndClipping(ctx);
        tpEndClipping(ctx);

        /* call this when you are done with Tarp for the frame */
        tpFinishDrawing(ctx);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /* clean up tarp */
    tpPathDestroy(path);
    tpContextDestroy(ctx);

    /* clean up glfw */
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
