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
        int err = tp_context_init(&ctx);
        if (err)
        {
            printf("COULT NOT INIT TARP CONTEXT %s\n", ctx.lastErrorMessage);
            return EXIT_FAILURE;
        }

        tpPath * path = tp_path_new();
        tp_path_add_point(path, 10, 10);
        tp_path_cubic_curve_to(path, 15, 15, 100, 15, 100, 100);

        printf("WE GOT %lu\n", tp_path_segment_count(path));


        // tpVec2 a = tp_vec2_make(10, 20);
        // tpVec2 b = tp_vec2_make(2, 4);

        // tpVec2 c = tp_vec2_add(&a, &b);
        // printf("%f %f\n", a.x, a.y);
        // printf("%f %f\n", b.x, b.y);
        // printf("%f %f\n", c.x, c.y);

        // tpMat3 mat = tp_mat3_identity();
        // printf("%f %f %f\n%f %f %f\n%f %f %f\n", mat.v[0], mat.v[1], mat.v[2],
        //        mat.v[3], mat.v[4], mat.v[5],
        //        mat.v[6], mat.v[7], mat.v[8]);

        // tpVec2 c2 = tp_vec2_mult_mat(&mat, &c);
        // printf("%f %f\n", c2.x, c2.y);

        // tpGradient * grad = tp_gradient_linear_new(0, 0, 100, 100);
        // tp_gradient_add_color_stop(grad, 1, 0, 0, 1.0, 0.1);
        // tp_gradient_add_color_stop(grad, 1, 0, 1, 1.0, 0.9);
        // printf("COUNTS %i\n", grad->stopCount);

        // tpStyle style = tp_style_make();
        // tp_style_set_fill_gradient(&style, grad);
        // tp_style_set_stroke_width(&style, 4.0);

        // tpPath * p = tp_path_new();
        // tp_path_destroy(p);

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

            tpMat4 proj = tp_mat4_ortho(0, 800, 600, 0, -1, 1);
            tp_set_projection(&ctx, &proj);
            tpStyle style = tp_style_make();
            tp_draw_path(&ctx, path, &style);

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
