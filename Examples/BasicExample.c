#include <Tarp/TarpGL.h>
#include <stdio.h>

// we use GLFW to open a simple window
#include <GLFW/glfw3.h>

int main(int argc, char * argv[])
{
    // initialize glfw
    if (!glfwInit())
        return EXIT_FAILURE;

    // and set some hints to get the correct opengl versions/profiles
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //create the window
    GLFWwindow * window = glfwCreateWindow(800, 600, "Basic Tarp Example", NULL, NULL);
    if (window)
    {
        glfwMakeContextCurrent(window);

        tpContext ctx;
        int err = tpContextInit(&ctx);
        if (err)
        {
            printf("COULT NOT INIT TARP CONTEXT %s\n", ctx.lastErrorMessage);
            return EXIT_FAILURE;
        }

        tpPath * path = tpPathCreate();
        tpPathAddPoint(path, 10, 10);
        tpPathCubicCurveTo(path, 15, 15, 100, 15, 100, 100);

        printf("WE GOT %lu\n", tpPathSegmentCount(path));


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
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            // get the window size from the glfw window
            // and set it as the viewport on the renderer
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            glViewport(0, 0, width, height);

            tpMat4 proj = tpMat4Ortho(0, 800, 600, 0, -1, 1);
            tpSetProjection(&ctx, &proj);
            tpStyle style = tpStyleMake();
            tpDrawPath(&ctx, path, &style);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
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
