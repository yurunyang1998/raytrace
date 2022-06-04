//////////////////////////////////////////////////////////////////////
//
//	University of Leeds
//	COMP 5812M Foundations of Modelling & Rendering
//	User Interface for Coursework
//
//	September, 2020
//
//  -----------------------------
//  Raytrace Render Widget
//  -----------------------------
//
//	Provides a widget that displays a fixed image
//	Assumes that the image will be edited (somehow) when Render() is called
//  
////////////////////////////////////////////////////////////////////////

// include guard
#ifndef _RAYTRACE_RENDER_WIDGET_H
#define _RAYTRACE_RENDER_WIDGET_H

#include <vector>
#include <mutex>

// include the relevant QT headers
#include <QOpenGLWidget>
#include <QMouseEvent>

// and include all of our own headers that we need
#include "TexturedObject.h"
#include "RenderParameters.h"
#include "raytrace/hitList.h"
#include "raytrace/hitable.h"
#include "raytrace/triangle.h"
#include "raytrace/camera.h"
#include "Matrix4.h"


// class for a render widget with arcball linked to an external arcball widget
class RaytraceRenderWidget : public QOpenGLWidget										
	{ // class RaytraceRenderWidget
	Q_OBJECT
	private:	
	// the geometric object to be rendered
    std::vector<TexturedObject> *texturedObjects;
    std::vector<Triangle * > triangleObjs;
	// the render parameters to use
	RenderParameters *renderParameters;

	// An image to use as a framebuffer
	RGBAImage frameBuffer;

	public:
	// constructor
	RaytraceRenderWidget
			(
	 		// the geometric object to show
            std::vector<TexturedObject> 		*newTexturedObject,
			// the render parameters to use
			RenderParameters 	*newRenderParameters,
			// parent widget in visual hierarchy
			QWidget 			*parent
			);
	
	// destructor
	~RaytraceRenderWidget();
			
	protected:
	// called when OpenGL context is set up
	void initializeGL();
	// called every time the widget is resized
	void resizeGL(int w, int h);
	// called every time the widget needs painting
	void paintGL();
	
    Cartesian3 MonteCarloHemisphereVector(Cartesian3 normal);

    bool convertVectice2Triangle(std::vector<TexturedObject> * textObjs);
    void readSceneObj(HitList &scene_);
    RGBAValue getHitColor(Ray &ray, HitList &objList, int depth);
    void write_color(Cartesian3 pixel_color, int samples_per_pixel, int i, int j);
    Cartesian3 BaycentricInterpolation(HitPoint &hitpoint);
    Ray reflectRay(Ray &inRay, HitPoint &hitPoint);


	// mouse-handling
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);


	// these signals are needed to support shared arcball control
	public:

    // routine that generates the image
    void Raytrace();
    //threading stuff
    void RaytraceThread();
    private:

    void forceRepaint();
    std::mutex drawingLock;
    std::mutex raytraceAgainLock;
    bool restartRaytrace;

	signals:
	// these are general purpose signals, which scale the drag to 
	// the notional unit sphere and pass it to the controller for handling
	void BeginScaledDrag(int whichButton, float x, float y);
	// note that Continue & End assume the button has already been set
	void ContinueScaledDrag(float x, float y);
	void EndScaledDrag(float x, float y);



	}; // class RaytraceRenderWidget

#endif
