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
    tpBool err;
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

    /* create the window */
    window = glfwCreateWindow(1280, 800, "Basic Tarp Example", NULL, NULL);
    if (window)
    {
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

        /* clean up tarp */
        tpContextDeallocate(&ctx);
    }

    return EXIT_SUCCESS;
}
