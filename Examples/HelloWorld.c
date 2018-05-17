/* include opengl */
#include <GL/gl3w.h>

/* we use GLFW to open a simple window */
#include <GLFW/glfw3.h>

/* tell Tarp to compile the opengl implementations */
#define TARP_IMPLEMENTATION_OPENGL
#include <Tarp/Tarp.h>

void printMat(const tpMat3 * _mat)
{
    printf("MAT3(%f %f %f\n%f %f %f\n%f %f %f)\n",
           _mat->v[0], _mat->v[3], _mat->v[6],
           _mat->v[1], _mat->v[4], _mat->v[7],
           _mat->v[2], _mat->v[5], _mat->v[8]);
}

int main(int argc, char * argv[])
{
    /* this example is compile in pedantic c89, so we declare the variables up here */
    tpContext ctx;
    tpBool err;
    GLFWwindow * window;
    tpVec2 vec;

    tpMat3 id = tpMat3MakeIdentity();
    printMat(&id);

    vec = tpMat3MultVec2(&id, tpVec2Make(100, 10));
    printf("DA VEC %f %f\n", vec.x, vec.y);

    /* initialize glfw */
    if (!glfwInit())
        return EXIT_FAILURE;

    /* and set some hints to get the correct opengl versions/profiles */
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* create the window */
    window = glfwCreateWindow(800, 600, "Basic Tarp Example", NULL, NULL);
    if (window)
    {
        tpPath path;
        tpStyle style;
        tpMat4 proj;
        int wwidth, wheight;

        glfwMakeContextCurrent(window);

        /* init opengl function pointers */
        if (gl3wInit())
        {
            fprintf(stderr, "failed to initialize OpenGL\n");
            return EXIT_FAILURE;
        }

        /* initialize the tarp context */
        err = tpContextInit(&ctx);
        if (err)
        {
            printf("Could not init Tarp context: %s\n", tpContextErrorMessage(&ctx));
            return EXIT_FAILURE;
        }

        /* set an orthographic projection on the context based on the window size */
        glfwGetWindowSize(window, &wwidth, &wheight);
        proj = tpMat4MakeOrtho(0, wwidth, wheight, 0, -1, 1);
        tpSetProjection(&ctx, &proj);

        path = tpPathCreate();
        tpPathAddCircle(path, 400, 300, 100);

        style = tpStyleCreate();
        tpStyleSetFillColor(style, 1.0, 0.0, 0.0, 1.0);

        /* the main loop */
        while (!glfwWindowShouldClose(window))
        {
            int width, height;

            glClearColor(0.0, 0.0, 0.0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            glfwGetFramebufferSize(window, &width, &height);
            glViewport(0, 0, width, height);

            tpPrepareDrawing(&ctx);

            tpDrawPath(&ctx, path, style);

            tpFinishDrawing(&ctx);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        /* clean up tarp */
        tpStyleDestroy(style);
        tpPathDestroy(path);
        tpContextDeallocate(&ctx);
    }
    else
    {
        glfwTerminate();
        printf("Could not open GLFW window :(\n");
        return EXIT_FAILURE;
    }

    /* clean up glfw */
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
