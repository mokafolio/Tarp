//include opengl
#include <GL/gl3w.h>

// we use GLFW to open a simple window
#include <GLFW/glfw3.h>

//we need to include leackcheck before tarp!!
#if !defined(NDEBUG)
// to check for memory leaks
#define STB_LEAKCHECK_IMPLEMENTATION
#include "stb_leakcheck.h"
#endif

// tell Tarp to compile the opengl implementations
#define TARP_IMPLEMENTATION_OPENGL
#include <Tarp/Tarp.h>

// for timing
#include <time.h>

//for rand
#include <stdlib.h>

//nano svg for svg loading
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

typedef void(*DrawFn)(tpContext, void *, tpBool);

typedef struct
{
    DrawFn callback;
    tpFloat interval;
    double lastCallbackTime;
    tpContext context;
    void * userData;
} DrawCallback;

static DrawCallback makeDrawCallback(DrawFn _callback, tpFloat _interval, tpContext _ctx, void * _userData)
{
    return (DrawCallback) {_callback, _interval, -HUGE_VAL, _ctx, _userData};
}

static void updateDrawCallback(DrawCallback * _cb)
{
    double now = glfwGetTime();
    double previous = _cb->lastCallbackTime;
    if (now - previous >= _cb->interval)
    {
        _cb->callback(_cb->context, _cb->userData, tpTrue);
        _cb->lastCallbackTime = now;
    }
    else
    {
        _cb->callback(_cb->context, _cb->userData, tpFalse);
    }
}

typedef struct
{
    tpPath path;
    tpStyle style;
    tpGradient grad;
} PathWithStyle;

static PathWithStyle makePathWithStyle()
{
    PathWithStyle ret;
    ret.path = tpPathInvalidHandle();
    ret.grad = tpGradientInvalidHandle();
    ret.style = tpStyleMake();
    return ret;
}

static void deallocatePathWithStyle(PathWithStyle * _ps)
{
    tpPathDestroy(_ps->path);
    // tpStyleDestroy(_ps->style);
    tpGradientDestroy(_ps->grad);
}

typedef struct
{
    PathWithStyle paths[512];
    int pathCount;
    tpPath clipPath;
} TigerDrawing;

static float randomFloat(float _a, float _b)
{
    float ret = (float)rand() / (float)RAND_MAX;
    return _a + (_b - _a) * ret;
}

static void updateStrokeOffsetDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    PathWithStyle * drawing = (PathWithStyle *) _userData;
    drawing->style.dashOffset += 0.2;
    tpDrawPath(_context, drawing->path, &drawing->style);
}

static void updateStarFillRuleDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    PathWithStyle * starDrawing = (PathWithStyle *) _userData;
    if (_bTimeElapsed)
    {
        starDrawing->style.fillRule = starDrawing->style.fillRule == kTpFillRuleEvenOdd ? kTpFillRuleNonZero : kTpFillRuleEvenOdd;
    }
    tpDrawPath(_context, starDrawing->path, &starDrawing->style);
}

static void updateRandomDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    PathWithStyle * drawing = (PathWithStyle *) _userData;
    if (_bTimeElapsed)
    {
        tpPathClear(drawing->path);
        tpPathMoveTo(drawing->path, randomFloat(350, 450), randomFloat(50, 150));
        for (int i = 0; i < ((int)randomFloat(3, 12)); ++i)
        {
            // tpPathLineTo(drawing->path, randomFloat(350, 450), randomFloat(50, 150));
            tpPathQuadraticCurveTo(drawing->path, randomFloat(350, 450), randomFloat(50, 150), randomFloat(350, 450), randomFloat(50, 150));
        }

        drawing->style.strokeWidth = randomFloat(1, 6);
        drawing->style.stroke = tpPaintMakeColor(randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1);
        drawing->style.fill = tpPaintMakeColor(randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1);
    }
    tpDrawPath(_context, drawing->path, &drawing->style);
}

static void updateConfettiDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    PathWithStyle * drawing = (PathWithStyle *) _userData;
    if (_bTimeElapsed)
    {
        tpPathClear(drawing->path);

        for (int i = 0; i < (int)randomFloat(3, 12); ++i)
        {
            tpFloat rad = randomFloat(5, 25);
            tpPathAddCircle(drawing->path, randomFloat(-50 + rad, 50 - rad), randomFloat(-50 + rad, 50 - rad), rad);
        }

        drawing->style.fill = tpPaintMakeColor(randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1);
    }
    tpTransform transform = tpTransformMakeTranslation(550, 100);
    tpSetTransform(_context, &transform);
    tpDrawPath(_context, drawing->path, &drawing->style);
    tpResetTransform(_context);
}

static void updateRotatingDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat angle = 0;
    static tpFloat rotTimer = 0;
    PathWithStyle * drawing = (PathWithStyle *) _userData;
    tpTransform transform = tpTransformMakeTranslation(100, 250);
    tpTransform rot = tpTransformMakeRotation(angle);
    transform = tpTransformCombine(&transform, &rot);
    // tpPathSetTransform(drawing->path, &transform);
    tpSetTransform(_context, &transform);
    tpDrawPath(_context, drawing->path, &drawing->style);
    tpResetTransform(_context);
    rotTimer += 0.1;
    angle += (sin(rotTimer) + 1.0) * 0.5 * 0.05;
}

static void updateWormDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat angle = 0;
    PathWithStyle * drawing = (PathWithStyle *) _userData;
    tpTransform transform = tpTransformMakeTranslation(400, 250);
    tpTransform rot = tpTransformMakeRotation(angle);
    tpFloat s = 0.25 + (sin(angle * 1.5) + 1.0) * 0.5 * 2.0;
    tpTransform scale = tpTransformMakeScale(s, s);
    rot = tpTransformCombine(&scale, &rot);
    transform = tpTransformCombine(&transform, &rot);
    tpPathSetStrokePaintTransform(drawing->path, &transform);
    tpDrawPath(_context, drawing->path, &drawing->style);
    angle += 0.1;
}

static void updateScalingStrokeDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat s = 0;
    PathWithStyle * drawing = (PathWithStyle *) _userData;
    tpTransform trans = tpTransformMakeTranslation(100, 400);
    tpTransform scale = tpTransformMakeScale(1.0, 0.25 + (sin(s * 1.5) + 1.0) * 0.5 * 2.0);
    trans = tpTransformCombine(&trans, &scale);
    tpSetTransform(_context, &trans);
    tpDrawPath(_context, drawing->path, &drawing->style);
    tpResetTransform(_context);
    s += 0.1;
}

static void updateNoneScalingStrokeDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat s = 0;
    PathWithStyle * drawing = (PathWithStyle *) _userData;
    tpTransform trans = tpTransformMakeTranslation(250, 400);
    tpTransform scale = tpTransformMakeScale(1.0, 0.25 + (sin(s * 1.5) + 1.0) * 0.5 * 2.0);
    trans = tpTransformCombine(&trans, &scale);
    // tpPathSetTransform(drawing->path, &trans);
    tpSetTransform(_context, &trans);
    tpDrawPath(_context, drawing->path, &drawing->style);
    tpResetTransform(_context);
    s += 0.05;
}

static void updateTigerDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat s = 0;
    TigerDrawing * td = (TigerDrawing *) _userData;
    tpFloat xoff = 300;
    tpFloat yoff = 300;
    tpTransform trans = tpTransformMakeTranslation(-xoff, -yoff);
    // tpFloat scale = (sin(s * 1.5) + 1.0) * 0.5 * 2.0;
    // tpTransform scaleTrans = tpTransformMakeScale(scale, scale);
    tpTransform rot = tpTransformMakeRotation(s * 3);
    tpTransform scale = tpTransformMakeScale(0.65, 0.65);
    rot = tpTransformCombine(&rot, &trans);
    rot = tpTransformCombine(&scale, &rot);
    trans = tpTransformMakeTranslation(180, 600);
    rot = tpTransformCombine(&trans, &rot);

    tpTransform clipTrans = tpTransformMakeTranslation(175, 625);
    tpTransform skew = tpTransformMakeSkew(sin(s * 10) * 0.25, cos(s * 20) * 0.5);
    clipTrans = tpTransformCombine(&clipTrans, &skew);
    tpSetTransform(_context, &clipTrans);
    tpBeginClipping(_context, td->clipPath);
    tpSetTransform(_context, &rot);
    for (int i = 0; i < td->pathCount; ++i)
    {
        tpDrawPath(_context, td->paths[i].path, &td->paths[i].style);
    }
    tpResetClipping(_context);
    tpResetTransform(_context);
    s += 0.005;
}

static void updateGradientDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat s = 0;
    PathWithStyle * drawing = (PathWithStyle *) _userData;

    if (_bTimeElapsed)
    {
        tpFloat off, ang;
        tpVec2 a, b, center, dir;

        tpGradientClearColorStops(drawing->grad);
        off = 0.0f;
        while (off < 1.0)
        {
            tpGradientAddColorStop(drawing->grad, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), off);
            off += randomFloat(0.05, 0.5);
        }
        center = tpVec2Make(0, 0);
        ang = randomFloat(-TARP_PI, TARP_PI);
        dir = tpVec2MultScalar(tpVec2Make(cos(ang), sin(ang)), randomFloat(100, 150));
        a = tpVec2Add(center, dir);
        b = tpVec2Sub(center, dir);
        tpGradientSetPositions(drawing->grad, a.x, a.y, b.x, b.y);
    }

    tpTransform skew = tpTransformMakeSkew(sin(s) * 0.25, cos(s * 0.125) * 0.5);
    tpTransform trans = tpTransformMakeTranslation(475, 450);
    trans = tpTransformCombine(&trans, &skew);
    tpSetTransform(_context, &trans);
    tpDrawPath(_context, drawing->path, &drawing->style);
    tpResetTransform(_context);
    s += 0.5;
}

static void updateRadialGradientDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat s = 0;
    PathWithStyle * drawing = (PathWithStyle *) _userData;

    if (_bTimeElapsed)
    {
        tpFloat off, ang;
        tpVec2 a, b, center, dir;

        tpGradientClearColorStops(drawing->grad);
        off = 0.0f;
        while (off < 1.0)
        {
            tpGradientAddColorStop(drawing->grad, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), off);
            off += randomFloat(0.05, 0.5);
        }
        center = tpVec2Make(0, 0);
        ang = randomFloat(-TARP_PI, TARP_PI);
        dir = tpVec2MultScalar(tpVec2Make(cos(ang), sin(ang)), randomFloat(50, 100));
        a = tpVec2Add(center, dir);
        b = tpVec2Sub(center, dir);
        tpGradientSetPositions(drawing->grad, a.x, a.y, b.x, b.y);
        tpGradientSetFocalPointOffset(drawing->grad, dir.y, dir.x);
        tpGradientSetRatio(drawing->grad, randomFloat(-2,2));
    }

    tpTransform skew = tpTransformMakeSkew(sin(s) * -0.5, cos(s * 0.125) * 0.25);
    tpTransform trans = tpTransformMakeTranslation(475, 675);
    trans = tpTransformCombine(&trans, &skew);
    tpSetTransform(_context, &trans);
    tpDrawPath(_context, drawing->path, &drawing->style);
    tpResetTransform(_context);
    s += 0.25;
}

static void updateZigZagLineDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat s = 0;

    PathWithStyle * drawing = (PathWithStyle *) _userData;

    if (_bTimeElapsed)
    {
        tpGradientClearColorStops(drawing->grad);
        tpFloat off = 0.0f;
        while (off < 1.0)
        {
            tpGradientAddColorStop(drawing->grad, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), off > 0 ? randomFloat(0, 1) : 1.0, off);
            off += randomFloat(0.05, 0.5);
        }
    }

    for (int i = 0; i < 15; ++i)
    {
        tpTransform trans = tpTransformMakeTranslation(250 + i * 20.0 + cos(s + i * 0.5) * 100.0, 600 + sin(s + i * 0.25) * 100.0);
        tpTransform skew = tpTransformMakeSkew(sin(s + i * 0.1) * 0.4, cos(s * 2.0 + i * 0.3) * 0.8);
        trans = tpTransformCombine(&trans, &skew);
        tpSetTransform(_context, &trans);

        tpTransform rot = tpTransformMakeRotation(s * 3.0 + i * 0.5);
        tpPathSetStrokePaintTransform(drawing->path, &rot);

        tpDrawPath(_context, drawing->path, &drawing->style);
    }
    tpResetTransform(_context);
    s += 0.05;
}

int main(int argc, char * argv[])
{
    //randomize the random seed
    //NOTE: Screw rand, we just use it for the simplicity/portability of this example
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
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // create the window
    GLFWwindow * window = glfwCreateWindow(650, 800, "Tarp Playground", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        printf("Could not open GLFW window :(\n");
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    //init opengl function pointers
    if (gl3wInit())
    {
        fprintf(stderr, "failed to initialize OpenGL\n");
        return EXIT_FAILURE;
    }

    int wwidth, wheight;
    glfwGetWindowSize(window, &wwidth, &wheight);

    DrawCallback drawCallbacks[32];
    int callbackCount = 0;

    //initialize the tarp context
    tpContext ctx = tpContextCreate();
    if (!tpContextIsValidHandle(ctx))
    {
        printf("Could not init Tarp context: %s\n", tpErrorMessage());
        return EXIT_FAILURE;
    }

    //set an orthographic projection on the context based on the window size;
    tpMat4 proj = tpMat4MakeOrtho(0, wwidth, wheight, 0, -1, 1);
    tpSetProjection(ctx, &proj);

    //create the dash offset drawing
    PathWithStyle dashOffsetDrawing = makePathWithStyle();
    tpFloat dashOffsetDrawingDashes[] = {10, 20};
    {
        tpPath path = tpPathCreate();
        tpPathAddCircle(path, 100, 100, 50);
        tpPathAddCircle(path, 100, 100, 25);

        dashOffsetDrawing.style.strokeWidth = 8;
        dashOffsetDrawing.style.strokeJoin = kTpStrokeJoinRound;
        dashOffsetDrawing.style.strokeCap = kTpStrokeCapRound;
        dashOffsetDrawing.style.fill = tpPaintMakeColor(1.0f, 1.0f, 0.0f, 1.0f);
        dashOffsetDrawing.style.dashArray = dashOffsetDrawingDashes;
        dashOffsetDrawing.style.dashCount = 2;

        dashOffsetDrawing.path = path;
        drawCallbacks[callbackCount++] = makeDrawCallback(updateStrokeOffsetDrawing, 0.0, ctx, &dashOffsetDrawing);
    }

    //init the star drawing to show EvenOdd vs NonZero fill rule
    PathWithStyle starDrawing = makePathWithStyle();
    {
        tpPath path = tpPathCreate();
        tpPathMoveTo(path, 250, 50);
        tpPathLineTo(path, 225, 150);
        tpPathLineTo(path, 300, 100);
        tpPathLineTo(path, 200, 100);
        tpPathLineTo(path, 275, 150);
        tpPathClose(path);

        starDrawing.style.fill = tpPaintMakeColor(0.4f, 0.6f, 1.0f, 1.0f);
        starDrawing.style.stroke = tpPaintMakeColor(1.0f, 0.0f, 1.0f, 1.0f);
        starDrawing.style.strokeWidth = 6;
        starDrawing.style.strokeJoin = kTpStrokeJoinMiter;
        starDrawing.style.miterLimit = 10;

        starDrawing.path = path;
        drawCallbacks[callbackCount++] = makeDrawCallback(updateStarFillRuleDrawing, 1.0, ctx, &starDrawing);
    }

    // path that randomly regenerates every half second
    PathWithStyle randomDrawing = makePathWithStyle();
    {
        tpPath path = tpPathCreate();

        randomDrawing.style.strokeCap = kTpStrokeCapRound;
        randomDrawing.style.strokeJoin = kTpStrokeJoinRound;

        randomDrawing.path = path;
        drawCallbacks[callbackCount++] = makeDrawCallback(updateRandomDrawing, 0.5, ctx, &randomDrawing);
    }

    PathWithStyle confettiDrawing = makePathWithStyle();
    {
        tpPath path = tpPathCreate();

        confettiDrawing.path = path;
        confettiDrawing.style.stroke.type = kTpPaintTypeNone;
        drawCallbacks[callbackCount++] = makeDrawCallback(updateConfettiDrawing, 0.25, ctx, &confettiDrawing);
    }

    PathWithStyle rotatingDrawing = makePathWithStyle();
    tpFloat rotatingDrawingDashes[] = {20.0, 2.0};
    {
        tpPath path = tpPathCreate();
        tpPathAddRect(path, -50, -50, 100, 100);
        tpPathMoveTo(path, -20, -20);
        tpPathQuadraticCurveTo(path, 0, 20, 20, 20);
        tpPathQuadraticCurveTo(path, 0, -20, -20, -20);
        tpPathClose(path);

        rotatingDrawing.style.fill = tpPaintMakeColor(1.0, 0.7, 0.1, 1.0);
        rotatingDrawing.style.stroke = tpPaintMakeColor(0.1, 0.7, 1.0, 1.0);
        rotatingDrawing.style.strokeWidth = 5.0f;
        rotatingDrawing.style.dashArray = rotatingDrawingDashes;
        rotatingDrawing.style.dashCount = 2;
        rotatingDrawing.style.strokeJoin = kTpStrokeJoinMiter;

        rotatingDrawing.path = path;
        drawCallbacks[callbackCount++] = makeDrawCallback(updateRotatingDrawing, 0, ctx, &rotatingDrawing);
    }

    PathWithStyle wormDrawing = makePathWithStyle();
    {
        tpPath path = tpPathCreate();
        tpPathMoveTo(path, 200, 200);
        tpFloat xoff = 200;
        for (int i = 0; i < 2; ++i)
        {
            tpPathCubicCurveTo(path, xoff + TARP_KAPPA * 50, 200, xoff + 50, 250 - TARP_KAPPA * 50, xoff + 50, 250);
            tpPathCubicCurveTo(path, xoff + 50, 250 + TARP_KAPPA * 50, xoff + 100 - TARP_KAPPA * 50, 300, xoff + 100, 300);
            tpPathCubicCurveTo(path, xoff + 100 + TARP_KAPPA * 50, 300, xoff + 150, 250 + TARP_KAPPA * 50, xoff + 150, 250);
            tpPathCubicCurveTo(path, xoff + 150, 250 - TARP_KAPPA * 50, xoff + 200 - TARP_KAPPA * 50, 200, xoff + 200, 200);
            xoff += 200;
        }

        wormDrawing.style.fill.type = kTpPaintTypeNone;
        wormDrawing.style.strokeWidth = 20.0;
        wormDrawing.style.strokeJoin = kTpStrokeJoinRound;
        wormDrawing.style.strokeCap = kTpStrokeCapRound;

        tpGradient grad = tpGradientCreateLinear(-200, -50, 200, 50);
        tpFloat off = 0.0;
        while (off < 1.0)
        {
            tpGradientAddColorStop(grad, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0, off);
            off += randomFloat(0.1, 0.3);
        }

        wormDrawing.style.stroke = tpPaintMakeGradient(grad);

        wormDrawing.path = path;
        wormDrawing.grad = grad;
        drawCallbacks[callbackCount++] = makeDrawCallback(updateWormDrawing, 0.0, ctx, &wormDrawing);
    }

    PathWithStyle scalingStrokeDrawing = makePathWithStyle();
    {
        tpPath path = tpPathCreate();
        tpPathMoveTo(path, -50, 0);
        tpPathQuadraticCurveTo(path, 0, -30, 50, 0);
        tpPathQuadraticCurveTo(path, 0, 30, -50, 0);
        tpPathClose(path);

        scalingStrokeDrawing.style.strokeWidth = 10.0f;
        scalingStrokeDrawing.style.strokeJoin = kTpStrokeJoinRound;
        scalingStrokeDrawing.style.strokeCap = kTpStrokeCapRound;
        scalingStrokeDrawing.style.stroke = tpPaintMakeColor(1.0, 0.15, 0.5, 1.0);
        scalingStrokeDrawing.style.fill = tpPaintMakeColor(0.6, 0.05, 0.25, 1.0);

        scalingStrokeDrawing.path = path;
        drawCallbacks[callbackCount++] = makeDrawCallback(updateScalingStrokeDrawing, 0.0, ctx, &scalingStrokeDrawing);
    }

    PathWithStyle noneScalingStrokeDrawing = makePathWithStyle();
    {
        tpPath path = tpPathCreate();
        tpPathMoveTo(path, -50, 0);
        tpPathQuadraticCurveTo(path, 0, -30, 50, 0);
        tpPathQuadraticCurveTo(path, 0, 30, -50, 0);
        tpPathClose(path);

        noneScalingStrokeDrawing.style.strokeWidth = 10.0f;
        noneScalingStrokeDrawing.style.strokeJoin = kTpStrokeJoinRound;
        noneScalingStrokeDrawing.style.strokeCap = kTpStrokeCapRound;
        noneScalingStrokeDrawing.style.stroke = tpPaintMakeColor(1.0, 0.15, 0.5, 1.0);
        noneScalingStrokeDrawing.style.fill = tpPaintMakeColor(0.6, 0.05, 0.25, 1.0);
        noneScalingStrokeDrawing.style.scaleStroke = tpFalse;

        noneScalingStrokeDrawing.path = path;
        drawCallbacks[callbackCount++] = makeDrawCallback(updateNoneScalingStrokeDrawing, 0.0, ctx, &noneScalingStrokeDrawing);
    }

    //  we use nano svg to create a drawing from an SVG in a quick n dirty way...not general purpose at all!
    NSVGimage * image;
    image = nsvgParseFromFile("../../Examples/Assets/Tiger.svg", "px", 4096);
    if (!image)
    {
        printf("Could not parse \"../../Examples/Assets/Tiger.svg\" :(\n");
        return EXIT_FAILURE;
    }

    TigerDrawing tigerDrawing = {0};
    tigerDrawing.pathCount = 0;
    for (NSVGshape * shape = image->shapes; shape != NULL; shape = shape->next)
    {
        tpPath tpp = tpPathCreate();
        tpStyle style = tpStyleMake();

        if (shape->fill.type == NSVG_PAINT_COLOR)
        {
            int cr, cg, cb, ca;
            cr = shape->fill.color & 0xff;
            cg = (shape->fill.color >> 8) & 0xff;
            cb = (shape->fill.color >> 16) & 0xff;
            ca = (shape->fill.color >> 24) & 0xff;

            style.fill = tpPaintMakeColor(cr / 255.0, cg / 255.0, cb / 255.0, ca / 255.0);
        }
        else
        {
            style.fill.type = kTpPaintTypeNone;
        }

        if (shape->stroke.type == NSVG_PAINT_COLOR && shape->strokeWidth > 0)
        {
            int scr, scg, scb, sca;
            scr = shape->stroke.color & 0xff;
            scg = (shape->stroke.color >> 8) & 0xff;
            scb = (shape->stroke.color >> 16) & 0xff;
            sca = (shape->stroke.color >> 24) & 0xff;

            style.stroke = tpPaintMakeColor(scr / 255.0, scg / 255.0, scb / 255.0, sca / 255.0);
            style.strokeWidth = shape->strokeWidth;
        }
        else
        {
            style.stroke.type = kTpPaintTypeNone;
        }

        for (NSVGpath * path = shape->paths; path != NULL; path = path->next)
        {
            tpPathMoveTo(tpp, path->pts[0], path->pts[1]);
            for (int i = 0; i < path->npts - 1; i += 3)
            {
                float * p = &path->pts[i * 2];
                tpPathCubicCurveTo(tpp, p[2], p[3], p[4], p[5], p[6], p[7]);
            }
            tigerDrawing.paths[tigerDrawing.pathCount].path = tpp;
            tigerDrawing.paths[tigerDrawing.pathCount].style = style;
            tigerDrawing.pathCount++;
        }
    }

    drawCallbacks[callbackCount++] = makeDrawCallback(updateTigerDrawing, 0, ctx, &tigerDrawing);
    tigerDrawing.clipPath = tpPathCreate();
    tpPathAddCircle(tigerDrawing.clipPath, 0, 0, 125);
    nsvgDelete(image);
    /* done with tiger/nano svg */

    PathWithStyle gradientDrawing = makePathWithStyle();
    {
        tpPath path = tpPathCreate();
        tpPathAddCircle(path, 0, 0, 75);

        tpGradient grad = tpGradientCreateLinear(-100, -100, 100, 100);
        tpGradientAddColorStop(grad, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0, 0.0);
        tpGradientAddColorStop(grad, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0, 1.0);

        gradientDrawing.style.stroke.type = kTpPaintTypeNone;
        gradientDrawing.style.fill = tpPaintMakeGradient(grad);

        gradientDrawing.path = path;
        gradientDrawing.grad = grad;
        drawCallbacks[callbackCount++] = makeDrawCallback(updateGradientDrawing, 1.0, ctx, &gradientDrawing);
    }

    PathWithStyle radialGradientDrawing = makePathWithStyle();
    {
        tpPath path = tpPathCreate();
        tpPathAddCircle(path, 0, 0, 75);

        tpGradient grad = tpGradientCreateRadial(0, 0, 0, 0, 100, 100, 1);
        tpGradientAddColorStop(grad, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0, 0.0);
        tpGradientAddColorStop(grad, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0, 1.0);

        radialGradientDrawing.style.stroke.type = kTpPaintTypeNone;
        radialGradientDrawing.style.fill = tpPaintMakeGradient(grad);

        radialGradientDrawing.path = path;
        radialGradientDrawing.grad = grad;
        drawCallbacks[callbackCount++] = makeDrawCallback(updateRadialGradientDrawing, 0.5, ctx, &radialGradientDrawing);
    }

    PathWithStyle zigZagDrawing = makePathWithStyle();
    {
        int count = randomFloat(3, 6);
        tpFloat step = 100.0f / count;
        tpFloat y = -50.0f + step;
        tpPath path = tpPathCreate();
        tpPathMoveTo(path, 10, -50);
        for (int i = 0; i < count; ++i)
        {
            tpPathLineTo(path, i % 2 == 0 ? -10 : 10, y);
            y += step;
        }

        tpGradient grad = tpGradientCreateLinear(0, -50, 0, 50);
        tpGradientAddColorStop(grad, 1, 0.0, 1, 1.0, 0.0);
        tpGradientAddColorStop(grad, 1, 1, 0.0, 0.0, 1.0);

        zigZagDrawing.style.fill.type = kTpPaintTypeNone;
        zigZagDrawing.style.strokeWidth = 10.0f;
        zigZagDrawing.style.strokeCap = kTpStrokeCapRound;
        zigZagDrawing.style.strokeJoin = kTpStrokeJoinMiter;
        zigZagDrawing.style.stroke = tpPaintMakeGradient(grad);

        zigZagDrawing.path = path;
        zigZagDrawing.grad = grad;
        drawCallbacks[callbackCount++] = makeDrawCallback(updateZigZagLineDrawing, 1.0, ctx, &zigZagDrawing);
    }

    // the main loop
    while (!glfwWindowShouldClose(window))
    {
        // clear the background to a color
        glClearColor(1.0, 1.0, 1.0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // get the window size from the glfw window
        // and set it as the viewport on the renderer
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        //set the viewport based on the actual width and height in pixels
        glViewport(0, 0, width, height);

        //start drawing the current tarp frame
        tpPrepareDrawing(ctx);

        for (int i = 0; i < callbackCount; ++i)
        {
            //update and draw all the individual drawings
            updateDrawCallback(&drawCallbacks[i]);
        }

        //end the current tarp frame
        tpFinishDrawing(ctx);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //clean up tarp related things
    for (int i = 0; i < callbackCount; ++i)
    {
        if (drawCallbacks[i].callback == updateTigerDrawing)
        {
            TigerDrawing * td = (TigerDrawing *)drawCallbacks[i].userData;
            for (int i = 0; i < td->pathCount; ++i)
            {
                deallocatePathWithStyle(&td->paths[i]);
            }
            tpPathDestroy(td->clipPath);
        }
        else
        {
            deallocatePathWithStyle((PathWithStyle *)drawCallbacks[i].userData);
        }
    }

    tpContextDestroy(ctx);

    // clean up glfw
    glfwDestroyWindow(window);
    glfwTerminate();

#if !defined(NDEBUG)
    stb_leakcheck_dumpmem();
#endif

    return EXIT_SUCCESS;
}
