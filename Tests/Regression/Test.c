/* include opengl */
#include <GL/gl3w.h>

/* we use GLFW to open a simple window */
#include <GLFW/glfw3.h>

/* tell Tarp to compile the opengl implementations */
#define TARP_IMPLEMENTATION_OPENGL
#include <Tarp/Tarp.h>


int main(int argc, char * argv[])
{
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
    GLFWwindow * window = glfwCreateWindow(512, 512, "Tarp Test", NULL, NULL);
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
    tpContext ctx = tpContextCreate();
    if (!tpContextIsValidHandle(ctx))
    {
        printf("Could not init Tarp context: %s\n", tpContextErrorMessage(ctx));
        return EXIT_FAILURE;
    }

    /* set an orthographic projection on the context based on the window size */
    int wwidth, wheight;
    glfwGetWindowSize(window, &wwidth, &wheight);

    /* set an orthographic projection based on the window size */
    tpMat4 proj = tpMat4MakeOrtho(0, wwidth, wheight, 0, -1, 1);
    tpSetProjection(ctx, &proj);

    tpPath zigzagPath = tpPathCreate();
    tpPathMoveTo(zigzagPath, -32,   0);
    tpPathLineTo(zigzagPath, -16,   0);
    tpPathLineTo(zigzagPath,   0,  16);
    tpPathLineTo(zigzagPath,   0, -16);
    tpPathLineTo(zigzagPath,  16,   0);
    tpPathLineTo(zigzagPath,  32,   0);
    
    tpPath squarePath = tpPathCreate();
    tpPathAddRect(squarePath, -32, -32, 64, 64);
    
    tpStyle roundStyle = tpStyleCreate();
    tpStyleSetFillColor(roundStyle, 0, 0, 0, 0);
    tpStyleSetStrokeColor(roundStyle, 1, 1, 1, 1);
    tpStyleSetStrokeWidth(roundStyle, 16);
    tpStyleSetStrokeJoin(roundStyle, kTpStrokeJoinRound);
    tpStyleSetStrokeCap(roundStyle, kTpStrokeCapRound);
    
    tpStyle miterStyle = tpStyleClone(roundStyle);
    tpStyleSetStrokeJoin(miterStyle, kTpStrokeJoinMiter);
    tpStyleSetStrokeCap(miterStyle, kTpStrokeCapSquare);
    
    tpStyle bevelStyle = tpStyleClone(roundStyle);
    tpStyleSetStrokeJoin(bevelStyle, kTpStrokeJoinBevel);
    tpStyleSetStrokeCap(bevelStyle, kTpStrokeCapButt);
    
    tpStyle redStyle = tpStyleClone(bevelStyle);
    tpStyleSetStrokeWidth(redStyle, 2);
    tpStyleSetStrokeColor(redStyle, 0.0f, 0.5f, 1.0f, 1.0f);
    
    tpGradient grad0 = tpGradientCreateLinear(-16, 16, 16, -16);
    tpGradientAddColorStop(grad0, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    tpGradientAddColorStop(grad0, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f);
    tpGradientAddColorStop(grad0, 1.0f, 1.0f, 1.0f, 1.0f, 0.75f);
    tpGradientAddColorStop(grad0, 0.0f, 0.5f, 1.0f, 1.0f, 0.7501f);
    tpGradientAddColorStop(grad0, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    tpStyle gradStyle0 = tpStyleCreate();
    tpStyleSetFillGradient(gradStyle0, grad0);
    
    tpGradient grad1 = tpGradientClone(grad0);
    tpGradientSetPositions(grad1, -32, 16, 32, -16);
    tpStyle gradStyle1 = tpStyleClone(gradStyle0);
    tpStyleSetFillGradient(gradStyle1, grad1);
    
    tpGradient grad2 = tpGradientClone(grad0);
    tpGradientSetPositions(grad2, -48, 48, 48, -48);
    tpStyle gradStyle2 = tpStyleClone(gradStyle0);
    tpStyleSetFillGradient(gradStyle2, grad2);
    
    tpGradient rgrad0 = tpGradientCreateRadialSymmetric(0, 0, 32);
    tpGradientAddColorStop(rgrad0, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    tpGradientAddColorStop(rgrad0, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f);
    tpGradientAddColorStop(rgrad0, 1.0f, 1.0f, 1.0f, 1.0f, 0.75f);
    tpGradientAddColorStop(rgrad0, 0.0f, 0.5f, 1.0f, 1.0f, 0.7501f);
    tpGradientAddColorStop(rgrad0, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    tpStyle rgradStyle0 = tpStyleClone(gradStyle0);
    tpStyleSetFillGradient(rgradStyle0, rgrad0);
    
    tpGradient rgrad1 = tpGradientClone(rgrad0);
    tpGradientSetPositions(rgrad1, -32, 32, 32, 32);
    tpStyle rgradStyle1 = tpStyleClone(rgradStyle0);
    tpStyleSetFillGradient(rgradStyle1, rgrad1);
    
    tpGradient rgrad2 = tpGradientClone(rgrad0);
    tpGradientSetPositions(rgrad2, 0, 0, 0, 0);
    tpStyle rgradStyle2 = tpStyleClone(rgradStyle0);
    tpStyleSetFillGradient(rgradStyle2, rgrad2);
    
    tpGradient rgrad3 = tpGradientClone(rgrad0);
    tpGradientSetFocalPointOffset(rgrad3, 32, -32);
    tpStyle rgradStyle3 = tpStyleClone(rgradStyle0);
    tpStyleSetFillGradient(rgradStyle3, rgrad3);
    
    tpGradient rgrad4 = tpGradientClone(rgrad1);
    tpGradientSetFocalPointOffset(rgrad4, 0, -64);
    tpStyle rgradStyle4 = tpStyleClone(rgradStyle0);
    tpStyleSetFillGradient(rgradStyle4, rgrad4);
    
    tpGradient rgrad5 = tpGradientClone(rgrad2);
    tpGradientSetPositions(rgrad5, 64, 0, 32, 0);
    tpStyle rgradStyle5 = tpStyleClone(rgradStyle0);
    tpStyleSetFillGradient(rgradStyle5, rgrad5);
    
    tpTransform distortion = tpTransformMake(-0.1, 0.9, 0.0,
                                             -1.1, 0.1, 0.0);

    /* the main loop */
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0, 0.0, 0.0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        /* set the viewport based on the pixel dimensions of the window */
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        /* call this at the beginning of your frame */
        tpPrepareDrawing(ctx);

        /* zig-zag paths */
        tpTransform transform;
        transform = tpTransformMakeTranslation(48, 32);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, zigzagPath, roundStyle);
        tpDrawPath(ctx, zigzagPath, redStyle);
        
        transform = tpTransformMakeTranslation(48, 96);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, zigzagPath, miterStyle);
        tpDrawPath(ctx, zigzagPath, redStyle);
        
        transform = tpTransformMakeTranslation(48, 160);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, zigzagPath, bevelStyle);
        tpDrawPath(ctx, zigzagPath, redStyle);
        
        /* linear gradients */
        transform = tpTransformMakeTranslation(128, 48);
        transform = tpTransformCombine(&transform, &distortion);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, squarePath, gradStyle0);
        
        transform = tpTransformMakeTranslation(128, 128);
        transform = tpTransformCombine(&transform, &distortion);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, squarePath, gradStyle1);
        
        transform = tpTransformMakeTranslation(128, 208);
        transform = tpTransformCombine(&transform, &distortion);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, squarePath, gradStyle2);
        
        /* radial gradients */
        transform = tpTransformMakeTranslation(192, 48);
        transform = tpTransformCombine(&transform, &distortion);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, squarePath, rgradStyle0);
        
        transform = tpTransformMakeTranslation(192, 128);
        transform = tpTransformCombine(&transform, &distortion);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, squarePath, rgradStyle1);
        
        transform = tpTransformMakeTranslation(192, 208);
        transform = tpTransformCombine(&transform, &distortion);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, squarePath, rgradStyle2);
        
        transform = tpTransformMakeTranslation(256, 48);
        transform = tpTransformCombine(&transform, &distortion);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, squarePath, rgradStyle3);
        
        transform = tpTransformMakeTranslation(256, 128);
        transform = tpTransformCombine(&transform, &distortion);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, squarePath, rgradStyle4);
        
        transform = tpTransformMakeTranslation(256, 208);
        transform = tpTransformCombine(&transform, &distortion);
        tpSetTransform(ctx, &transform);
        tpDrawPath(ctx, squarePath, rgradStyle5);

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
