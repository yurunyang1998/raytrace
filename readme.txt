To compile on feng-linux / feng-gps:

module add qt/5.13.0
qmake -project QT+=opengl QMAKE_CXXFLAGS+=-fopenmp LIBS+=-fopenmp
qmake
make
--------------------------------------

To compile on OSX:
Use Homebrew to install qt

qmake  -project QT+=opengl QMAKE_CXXFLAGS+=-fopenmp LIBS+=-fopenmp
qmake
make
--------------------------------------

To compile on Windows:
Install QT 5.13.0 
It can be found here: 
	https://download.qt.io/archive/qt/5.13/5.13.0/qt-opensource-windows-x86-5.13.0.exe.mirrorlist

Update graphics card driver
Double click RaytraceRenderWindow_win.pro to open in QTCreator or
Run qmake -project QT+=opengl LIBS+=-lopengl32 QMAKE_CXXFLAGS+=-fopenmp LIBS+=-fopenmp

Select the platform to compile to (32 or 64 bits)
Click details to select the build folder
Click Configure Project
--------------------------------------

To run on feng-linux / feng-gps:
./RaytraceRenderWindowRelease ../path_to/model.obj ../path_to/material.mtl

---------------------------------------------

To run on OSX:
./RaytraceRenderWindowRelease.app/Contents/MacOS/RaytraceRenderWindowRelease  ../path_to/model.obj ../path_to/material.mtl

---------------------------------------------
To run on Windows
./RaytraceRenderWindowRelease.exe ../path_to/model.obj ../path_to/material.mtl
or
Click projects
Select "Run" on the left side menu under the active configuration
Add "../path_to/model.obj ../path_to/texture.ppm" to command line arguments
Click Run/Debug on the bottom left side.