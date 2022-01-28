I don 't know how to show a triangle via raytrace. I only implementation a cornell box and a customer scence.

to run the cornell box:
./FakeGLRenderWindowRelease ../path_to/model.obj ../path_to/texture.ppm 1

to run the custom model
./FakeGLRenderWindowRelease ../path_to/model.obj ../path_to/texture.ppm 0





To compile on feng-linux / feng-gps:

module add qt/5.13.0
qmake -project QT+=opengl
qmake
make

To compile on OSX:
Use Homebrew to install qt

qmake -project QT+=opengl
qmake
make
To compile on Windows:
Unfortunately, the official OpenGL on Windows was locked at GL 1.1.  Many many hacks exist, and they all disagree.
Just to make it worse, the Qt response to this is clumsy.  Net result: there is no easy way to get this compiling on Windows.
I will aim to update these instructions at a later date.

To run on feng-linux / feng-gps:

./FakeGLRenderWindowRelease ../path_to/model.obj ../path_to/texture.ppm

To run on OSX:
./FakeGLRenderWindowRelease.app/Contents/MacOS/FakeGLRenderWindowRelease  ../path_to/model.obj ../path_to/texture.ppm


