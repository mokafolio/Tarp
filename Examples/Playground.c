//include opengl
#include <GL/gl3w.h>

// we use GLFW to open a simple window
#include <GLFW/glfw3.h>

// tell Tarp to compile the opengl implementations
#define TARP_IMPLEMENTATION_OPENGL
#include <Tarp/Tarp.h>

#ifdef TARP_DEBUG
//to check for memory leaks
#define STB_LEAKCHECK_IMPLEMENTATION
#include "stb_leakcheck.h"
#endif

// for timing
#include <sys/time.h>

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
    struct timeval lastCallbackTime;
    tpContext context;
    void * userData;
} DrawCallback;

static DrawCallback makeDrawCallback(DrawFn _callback, tpFloat _interval, tpContext _ctx, void * _userData)
{
    return (DrawCallback) {_callback, _interval, {}, _ctx, _userData};
}

static void updateDrawCallback(DrawCallback * _cb)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    double ns = now.tv_sec + (double)now.tv_usec / 1000000.0;
    double ls = _cb->lastCallbackTime.tv_sec + (double)_cb->lastCallbackTime.tv_usec / 1000000.0;
    if (ns - ls >= _cb->interval)
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

static void destroyPathWithStyle(PathWithStyle * _ps)
{
    tpPathDestroy(_ps->path);
    tpStyleDestroy(_ps->style);
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
    tpStyleSetDashOffset(drawing->style, tpStyleDashOffset(drawing->style) + 0.2);
    tpDrawPath(_context, drawing->path, drawing->style);
}

static void updateStarFillRuleDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    PathWithStyle * starDrawing = (PathWithStyle *) _userData;
    if (_bTimeElapsed)
    {
        tpFillRule fr = tpStyleFillRule(starDrawing->style);
        tpStyleSetFillRule(starDrawing->style, fr == kTpFillRuleEvenOdd ? kTpFillRuleNonZero : kTpFillRuleEvenOdd);
    }
    tpDrawPath(_context, starDrawing->path, starDrawing->style);
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
        tpStyleSetStrokeWidth(drawing->style, randomFloat(1, 6));
        tpStyleSetStrokeColor(drawing->style, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0);
        tpStyleSetFillColor(drawing->style, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0);
    }
    tpDrawPath(_context, drawing->path, drawing->style);
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

        tpStyleSetFillColor(drawing->style, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1);
    }
    tpMat3 transform = tpMat3MakeTranslation(550, 100);
    tpSetTransform(_context, &transform);
    tpDrawPath(_context, drawing->path, drawing->style);
    tpResetTransform(_context);
}

static void updateRotatingDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat rot = 0;
    static tpFloat rotTimer = 0;
    PathWithStyle * drawing = (PathWithStyle *) _userData;
    tpMat3 transform = tpMat3MakeTranslation(100, 250);
    tpMat3 rotMat = tpMat3MakeRotation(rot);
    transform = tpMat3Mult(&transform, &rotMat);
    // tpPathSetTransform(drawing->path, &transform);
    tpSetTransform(_context, &transform);
    tpDrawPath(_context, drawing->path, drawing->style);
    tpResetTransform(_context);
    rotTimer += 0.1;
    rot += (sin(rotTimer) + 1.0) * 0.5 * 0.05;
}

static void updateWormDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat rot = 0;
    PathWithStyle * drawing = (PathWithStyle *) _userData;
    tpMat3 transform = tpMat3MakeTranslation(400, 250);
    tpMat3 rotMat = tpMat3MakeRotation(rot);
    tpFloat s = 0.25 + (sin(rot * 1.5) + 1.0) * 0.5 * 2.0;
    tpMat3 scaleMat = tpMat3MakeScale(s, s);
    rotMat = tpMat3Mult(&scaleMat, &rotMat);
    transform = tpMat3Mult(&transform, &rotMat);
    tpPathSetStrokePaintTransform(drawing->path, &transform);
    tpDrawPath(_context, drawing->path, drawing->style);
    rot += 0.1;
}

static void updateScalingStrokeDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat s = 0;
    PathWithStyle * drawing = (PathWithStyle *) _userData;
    tpMat3 trans = tpMat3MakeTranslation(100, 400);
    tpMat3 scaleMat = tpMat3MakeScale(1.0, 0.25 + (sin(s * 1.5) + 1.0) * 0.5 * 2.0);
    trans = tpMat3Mult(&trans, &scaleMat);
    tpSetTransform(_context, &trans);
    tpDrawPath(_context, drawing->path, drawing->style);
    tpResetTransform(_context);
    s += 0.1;
}

static void updateNoneScalingStrokeDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat s = 0;
    PathWithStyle * drawing = (PathWithStyle *) _userData;
    tpMat3 trans = tpMat3MakeTranslation(250, 400);
    tpMat3 scaleMat = tpMat3MakeScale(1.0, 0.25 + (sin(s * 1.5) + 1.0) * 0.5 * 2.0);
    trans = tpMat3Mult(&trans, &scaleMat);
    // tpPathSetTransform(drawing->path, &trans);
    tpSetTransform(_context, &trans);
    tpDrawPath(_context, drawing->path, drawing->style);
    tpResetTransform(_context);
    s += 0.05;
}

static void updateTigerDrawing(tpContext _context, void * _userData, tpBool _bTimeElapsed)
{
    static tpFloat s = 0;
    TigerDrawing * td = (TigerDrawing *) _userData;
    tpFloat xoff = 300;
    tpFloat yoff = 300;
    tpMat3 trans = tpMat3MakeTranslation(-xoff, -yoff);
    tpFloat scale = (sin(s * 1.5) + 1.0) * 0.5 * 2.0;
    // tpMat3 scaleMat = tpMat3MakeScale(scale, scale);
    tpMat3 rot = tpMat3MakeRotation(s * 3);
    tpMat3 smat = tpMat3MakeScale(0.65, 0.65);
    rot = tpMat3Mult(&rot, &trans);
    rot = tpMat3Mult(&smat, &rot);
    trans = tpMat3MakeTranslation(180, 600);
    rot = tpMat3Mult(&trans, &rot);

    tpMat3 clipTrans = tpMat3MakeTranslation(175, 625);
    tpMat3 skew = tpMat3MakeSkew(sin(s * 10) * 0.25, cos(s * 20) * 0.5);
    clipTrans = tpMat3Mult(&clipTrans, &skew);
    tpSetTransform(_context, &clipTrans);
    tpBeginClipping(_context, td->clipPath);
    tpSetTransform(_context, &rot);
    for (int i = 0; i < td->pathCount; ++i)
    {
        tpDrawPath(_context, td->paths[i].path, td->paths[i].style);
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
        ang = randomFloat(-M_PI, M_PI);
        dir = tpVec2MultScalar(tpVec2Make(cos(ang), sin(ang)), randomFloat(100, 150));
        a = tpVec2Add(center, dir);
        b = tpVec2Sub(center, dir);
        tpGradientSetPositions(drawing->grad, a.x, a.y, b.x, b.y);
    }

    tpMat3 skew = tpMat3MakeSkew(sin(s) * 0.25, cos(s * 0.125) * 0.5);
    tpMat3 trans = tpMat3MakeTranslation(475, 475);
    trans = tpMat3Mult(&trans, &skew);
    tpSetTransform(_context, &trans);
    tpDrawPath(_context, drawing->path, drawing->style);
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
        tpMat3 trans = tpMat3MakeTranslation(250 + i * 20.0 + cos(s + i * 0.5) * 100.0, 600 + sin(s + i * 0.25) * 100.0);
        tpMat3 skew = tpMat3MakeSkew(sin(s + i * 0.1) * 0.4, cos(s * 2.0 + i * 0.3) * 0.8);
        trans = tpMat3Mult(&trans, &skew);
        tpSetTransform(_context, &trans);

        tpMat3 rot = tpMat3MakeRotation(s * 3.0 + i * 0.5);
        tpPathSetStrokePaintTransform(drawing->path, &rot);

        tpDrawPath(_context, drawing->path, drawing->style);
    }
    tpResetTransform(_context);
    s += 0.05;
}

int main(int argc, char * argv[])
{
    //randomize the random seed
    //NOTE: Fuck rand, we just use it for the simplicity/portability of this example
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

    // create the window
    GLFWwindow * window = glfwCreateWindow(650, 800, "Tarp Playground", NULL, NULL);
    if (window)
    {
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
            printf("Could not init Tarp context: %s\n", tpContextErrorMessage(ctx));
            return EXIT_FAILURE;
        }

        //set an orthographic projection on the context based on the window size;
        tpMat4 proj = tpMat4MakeOrtho(0, wwidth, wheight, 0, -1, 1);
        tpSetProjection(ctx, &proj);

        //create the dash offset drawing
        PathWithStyle dashOffsetDrawing = {0};
        {
            tpPath path = tpPathCreate();
            tpPathAddCircle(path, 100, 100, 50);
            tpPathAddCircle(path, 100, 100, 25);

            tpStyle style = tpStyleCreate();
            tpFloat dashArray[] = {10, 20};
            tpStyleSetDashArray(style, dashArray, 2);
            tpStyleSetStrokeWidth(style, 8);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinRound);
            tpStyleSetStrokeCap(style, kTpStrokeCapRound);
            tpStyleSetFillColor(style, 1, 1, 0, 1);

            dashOffsetDrawing.style = style;
            dashOffsetDrawing.path = path;
            drawCallbacks[callbackCount++] = makeDrawCallback(updateStrokeOffsetDrawing, 0.0, ctx, &dashOffsetDrawing);
        }

        //init the star drawing to show EvenOdd vs NonZero fill rule
        PathWithStyle starDrawing = {0};
        {
            tpPath path = tpPathCreate();
            tpPathMoveTo(path, 250, 50);
            tpPathLineTo(path, 225, 150);
            tpPathLineTo(path, 300, 100);
            tpPathLineTo(path, 200, 100);
            tpPathLineTo(path, 275, 150);
            tpPathClose(path);

            tpStyle style = tpStyleCreate();
            tpStyleSetFillColor(style, 0.4, 0.6, 1.0, 1.0);
            tpStyleSetStrokeColor(style, 1.0, 0.0, 1.0, 1.0);
            tpStyleSetStrokeWidth(style, 6.0);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinMiter);
            tpStyleSetMiterLimit(style, 25);

            starDrawing.path = path;
            starDrawing.style = style;
            drawCallbacks[callbackCount++] = makeDrawCallback(updateStarFillRuleDrawing, 1.0, ctx, &starDrawing);
        }

        //path that randomly regenerates every half second
        PathWithStyle randomDrawing = {0};
        {
            tpPath path = tpPathCreate();

            tpStyle style = tpStyleCreate();
            tpStyleSetStrokeCap(style, kTpStrokeCapRound);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinRound);

            randomDrawing.path = path;
            randomDrawing.style = style;
            drawCallbacks[callbackCount++] = makeDrawCallback(updateRandomDrawing, 0.5, ctx, &randomDrawing);
        }

        PathWithStyle confettiDrawing = {0};
        {
            tpPath path = tpPathCreate();

            tpStyle style = tpStyleCreate();
            tpStyleRemoveStroke(style);

            confettiDrawing.path = path;
            confettiDrawing.style = style;
            drawCallbacks[callbackCount++] = makeDrawCallback(updateConfettiDrawing, 0.25, ctx, &confettiDrawing);

            // tpMat3 trans = tpMat3MakeTranslation(550, 100);
            // tpPathSetTransform(path, &trans);
        }

        PathWithStyle rotatingDrawing = {0};
        {
            tpPath path = tpPathCreate();
            tpPathAddRect(path, -50, -50, 100, 100);
            tpPathMoveTo(path, -20, -20);
            tpPathQuadraticCurveTo(path, 0, 20, 20, 20);
            tpPathQuadraticCurveTo(path, 0, -20, -20, -20);
            tpPathClose(path);

            tpStyle style = tpStyleCreate();
            tpStyleSetFillColor(style, 1.0, 0.7, 0.1, 1.0);
            tpStyleSetStrokeColor(style, 0.1, 0.7, 1.0, 1.0);
            tpStyleSetStrokeWidth(style, 5.0);
            tpFloat dashes[] = {20.0, 2.0};
            tpStyleSetDashArray(style, dashes, 2);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinMiter);

            rotatingDrawing.path = path;
            rotatingDrawing.style = style;
            drawCallbacks[callbackCount++] = makeDrawCallback(updateRotatingDrawing, 0, ctx, &rotatingDrawing);
        }

        PathWithStyle wormDrawing = {0};
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

            // tpPathClose(path);

            tpStyle style = tpStyleCreate();
            tpStyleRemoveFill(style);
            tpStyleSetStrokeWidth(style, 20.0);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinRound);
            tpStyleSetStrokeCap(style, kTpStrokeCapRound);

            tpGradient grad = tpGradientCreateLinear(-200, -50, 200, 50);
            tpFloat off = 0.0;
            while (off < 1.0)
            {
                tpGradientAddColorStop(grad, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0, off);
                off += randomFloat(0.1, 0.3);
            }

            tpStyleSetStrokeGradient(style, grad);

            wormDrawing.path = path;
            wormDrawing.style = style;
            wormDrawing.grad = grad;
            drawCallbacks[callbackCount++] = makeDrawCallback(updateWormDrawing, 0.0, ctx, &wormDrawing);
        }

        PathWithStyle scalingStrokeDrawing = {0};
        {
            tpPath path = tpPathCreate();
            tpPathMoveTo(path, -50, 0);
            tpPathQuadraticCurveTo(path, 0, -30, 50, 0);
            tpPathQuadraticCurveTo(path, 0, 30, -50, 0);
            tpPathClose(path);

            tpStyle style = tpStyleCreate();
            tpStyleSetStrokeWidth(style, 10);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinRound);
            tpStyleSetStrokeCap(style, kTpStrokeCapRound);
            tpStyleSetStrokeColor(style, 1.0, 0.15, 0.5, 1.0);
            tpStyleSetFillColor(style, 0.6, 0.05, 0.25, 1.0);

            scalingStrokeDrawing.path = path;
            scalingStrokeDrawing.style = style;
            drawCallbacks[callbackCount++] = makeDrawCallback(updateScalingStrokeDrawing, 0.0, ctx, &scalingStrokeDrawing);
        }

        PathWithStyle noneScalingStrokeDrawing = {0};
        {
            tpPath path = tpPathCreate();
            tpPathMoveTo(path, -50, 0);
            tpPathQuadraticCurveTo(path, 0, -30, 50, 0);
            tpPathQuadraticCurveTo(path, 0, 30, -50, 0);
            tpPathClose(path);

            tpStyle style = tpStyleCreate();
            tpStyleSetStrokeWidth(style, 10);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinRound);
            tpStyleSetStrokeCap(style, kTpStrokeCapRound);
            tpStyleSetStrokeColor(style, 1.0, 0.15, 0.5, 1.0);
            tpStyleSetFillColor(style, 0.6, 0.05, 0.25, 1.0);
            tpStyleSetScaleStroke(style, tpFalse);

            noneScalingStrokeDrawing.path = path;
            noneScalingStrokeDrawing.style = style;
            drawCallbacks[callbackCount++] = makeDrawCallback(updateNoneScalingStrokeDrawing, 0.0, ctx, &noneScalingStrokeDrawing);
        }

        /* we use nano svg to create a drawing from an SVG in a quick n dirty way...not general purpose at all! */
        NSVGimage * image;
        image = nsvgParseFromFile("../../Examples/Tiger.svg", "px", 4096);

        TigerDrawing tigerDrawing = {0};
        tigerDrawing.pathCount = 0;
        for (NSVGshape * shape = image->shapes; shape != NULL; shape = shape->next)
        {
            tpPath tpp = tpPathCreate();
            tpStyle ds = tpStyleCreate();

            if (shape->fill.type == NSVG_PAINT_COLOR)
            {
                int cr, cg, cb, ca;
                cr = shape->fill.color & 0xff;
                cg = (shape->fill.color >> 8) & 0xff;
                cb = (shape->fill.color >> 16) & 0xff;
                ca = (shape->fill.color >> 24) & 0xff;
                tpStyleSetFillColor(ds, cr / 255.0, cg / 255.0, cb / 255.0, ca / 255.0);
            }
            else
            {
                tpStyleRemoveFill(ds);
            }

            if (shape->stroke.type == NSVG_PAINT_COLOR && shape->strokeWidth > 0)
            {
                int scr, scg, scb, sca;
                scr = shape->stroke.color & 0xff;
                scg = (shape->stroke.color >> 8) & 0xff;
                scb = (shape->stroke.color >> 16) & 0xff;
                sca = (shape->stroke.color >> 24) & 0xff;
                tpStyleSetStrokeColor(ds, scr / 255.0, scg / 255.0, scb / 255.0, sca / 255.0);
                tpStyleSetStrokeWidth(ds, shape->strokeWidth);
            }
            else
            {
                tpStyleRemoveStroke(ds);
            }

            for (NSVGpath * path = shape->paths; path != NULL; path = path->next)
            {
                tpPathMoveTo(tpp, path->pts[0], path->pts[1]);
                for (int i = 0; i < path->npts - 1; i += 3)
                {
                    float * p = &path->pts[i * 2];
                    tpPathCubicCurveTo(tpp, p[2], p[3], p[4], p[5], p[6], p[7]);
                    // drawCubicBez(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
                }
                tigerDrawing.paths[tigerDrawing.pathCount].path = tpp;
                tigerDrawing.paths[tigerDrawing.pathCount].style = ds;
                tigerDrawing.pathCount ++;
            }
        }

        drawCallbacks[callbackCount++] = makeDrawCallback(updateTigerDrawing, 0, ctx, &tigerDrawing);
        tigerDrawing.clipPath = tpPathCreate();
        tpPathAddCircle(tigerDrawing.clipPath, 0, 0, 125);
        nsvgDelete(image);
        /* done with tiger/nano svg */

        PathWithStyle gradientDrawing = {0};
        {
            tpPath path = tpPathCreate();
            tpPathAddCircle(path, 0, 0, 125);

            tpGradient grad = tpGradientCreateLinear(-100, -100, 100, 100);
            tpGradientAddColorStop(grad, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0, 0.0);
            tpGradientAddColorStop(grad, randomFloat(0, 1), randomFloat(0, 1), randomFloat(0, 1), 1.0, 1.0);

            tpStyle style = tpStyleCreate();
            tpStyleRemoveStroke(style);
            tpStyleSetFillGradient(style, grad);

            gradientDrawing.path = path;
            gradientDrawing.style = style;
            gradientDrawing.grad = grad;
            drawCallbacks[callbackCount++] = makeDrawCallback(updateGradientDrawing, 1.0, ctx, &gradientDrawing);
        }

        PathWithStyle zigZagDrawing = {0};
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

            tpStyle style = tpStyleCreate();
            tpStyleRemoveFill(style);
            tpStyleSetStrokeWidth(style, 10.0);
            tpStyleSetStrokeCap(style, kTpStrokeCapRound);
            tpStyleSetStrokeJoin(style, kTpStrokeJoinMiter);
            tpStyleSetStrokeGradient(style, grad);

            zigZagDrawing.path = path;
            zigZagDrawing.style = style;
            zigZagDrawing.grad = grad;
            drawCallbacks[callbackCount++] = makeDrawCallback(updateZigZagLineDrawing, 1.0, ctx, &zigZagDrawing);
        }

        // the main loop
        while (!glfwWindowShouldClose(window))
        {
            // clear the background to a color
            // glClearColor(0.4, 0.45, 0.43, 1);
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
                    destroyPathWithStyle(&td->paths[i]);
                }
                tpPathDestroy(td->clipPath);
            }
            else
            {
                destroyPathWithStyle((PathWithStyle *)drawCallbacks[i].userData);
            }
        }


        tpContextDestroy(ctx);
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

#ifdef TARP_DEBUG
    stb_leakcheck_dumpmem();
#endif

    return EXIT_SUCCESS;
}
