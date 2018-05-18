Tarp
======

Overview
--------

*Tarp* is an *almost* single header C library to raster vector graphics. It provides a lightweight and portable API purely focussed on decently fast rendering without any gimmicks.

What can tarp do?
--------
- Rasterize fills and strokees.
- Stroke joins and caps.
- Dashed strokes
- Gradients as fill and strokes (Note: only linear at the moment)
- Transformations for path, fills and strokes

What does it not want to provide?
--------
- A matrix stack.
- Any form of document representation.
- Any form of document loading.
- Bezier math that goes beyond rendering.
- Raster image rendering
- Text rendering. (Dealing with fonts is a huge task by itself, you can easily build a font rasterizer on top of Tarp using 'stb_truetype' or 'freeimage', though)

How does Tarp rasterize
--------
The only available backend right now is a custom OpenGL based one that uses the stencil and cover method to render paths.

Supported Platforms
-------------

Any platform that supports OpenGL 3.0+. Tested on *OSX* and *Linux* so far.


License
-------------

MIT License.
