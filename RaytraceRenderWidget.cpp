//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  -----------------------------
//  Raytrace Render Widget
//  -----------------------------
//
//	Provides a widget that displays a fixed image
//	Assumes that the image will be edited (somehow) when Render() is called
//  
////////////////////////////////////////////////////////////////////////


#include <math.h>
#include <thread>
#include <random>
#include <QTimer>
// include the header file
#include "RaytraceRenderWidget.h"

#define N_LOOPS 1
#define N_BOUNCES 5

// constructor
RaytraceRenderWidget::RaytraceRenderWidget
        (   
        // the geometric object to show
        std::vector<TexturedObject>      *newTexturedObject,
        // the render parameters to use
        RenderParameters    *newRenderParameters,
        // parent widget in visual hierarchy
        QWidget             *parent
        )
    // the : indicates variable instantiation rather than arbitrary code
    // it is considered good style to use it where possible
    : 
    // start by calling inherited constructor with parent widget's pointer
    QOpenGLWidget(parent),
    // then store the pointers that were passed in
    texturedObjects(newTexturedObject),
    renderParameters(newRenderParameters)
    { // constructor
        std::srand(time(nullptr));
        restartRaytrace = false;
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &RaytraceRenderWidget::forceRepaint);
        timer->start(30);
    // leaves nothing to put into the constructor body
    } // constructor    

void RaytraceRenderWidget::forceRepaint(){
    update();
}
// destructor
RaytraceRenderWidget::~RaytraceRenderWidget()
    { // destructor
    // empty (for now)
    // all of our pointers are to data owned by another class
    // so we have no responsibility for destruction
    // and OpenGL cleanup is taken care of by Qt
    } // destructor                                                                 

// called when OpenGL context is set up
void RaytraceRenderWidget::initializeGL()
    { // RaytraceRenderWidget::initializeGL()
	// this should remain empty
    } // RaytraceRenderWidget::initializeGL()

// called every time the widget is resized
void RaytraceRenderWidget::resizeGL(int w, int h)
    { // RaytraceRenderWidget::resizeGL()
    // resize the render image
    frameBuffer.Resize(w, h);
    } // RaytraceRenderWidget::resizeGL()
    
// called every time the widget needs painting
void RaytraceRenderWidget::paintGL()
    { // RaytraceRenderWidget::paintGL()
    // set background colour to white
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // and display the image
    glDrawPixels(frameBuffer.width, frameBuffer.height, GL_RGBA, GL_UNSIGNED_BYTE, frameBuffer.block);
    } // RaytraceRenderWidget::paintGL()

double clamp(double x, double min, double max)
{
     if (x < min)
          return min;
     if (x > max)
          return max;
     return x;
}

void RaytraceRenderWidget::write_color(Cartesian3 pixel_color, int samples_per_pixel, int i, int j)
{
     auto r = pixel_color.x;
     auto g = pixel_color.y;
     auto b = pixel_color.z;

     auto scale = 1.0 / samples_per_pixel;
     r *= scale;
     g *= scale;
     b *= scale;

     frameBuffer[j][i] =  RGBAValue(clamp(r, 0.0, 255),clamp(g, 0.0, 255),clamp(b, 0.0, 255));

}

void RaytraceRenderWidget::RaytraceThread()
{
    HitList scene;

    if(!convertVectice2Triangle(this->texturedObjects))
        return;
    readSceneObj(scene);
    float width = frameBuffer.width, height= frameBuffer.height;
    Camera camera(width,height,2.0,(width/height)*2.0,1.0, Cartesian3(0,0,0));

//    Sphere *  s2 = new Sphere(Cartesian3(0, 0, -10), 1);
//    scene.add(s2);

     #pragma omp parallel for collapse(2) schedule(dynamic)
    for(int j = 0; j < frameBuffer.height; j++)
     {

        for(int i = 0; i < frameBuffer.width; i++)
         {
            Cartesian3 pixelColor;
            for(int loop = 0; loop < N_LOOPS; loop++)
            {
            //Your code goes here
                auto u = (i + rand() / (RAND_MAX + 1.0)) / (width - 1);
                auto v = (j + rand() / (RAND_MAX + 1.0)) / (height - 1);
                Ray ray = camera.castRay(u,v);

                auto hitcolor = getHitColor(ray, scene, 0);
//                std::cout<<color<<std::endl;
                Cartesian3 hitColorCar(hitcolor.red, hitcolor.green, hitcolor.blue);

                pixelColor = pixelColor + hitColorCar;
                //End raytracing code
                 if(restartRaytrace){
                     break;
                 }
            }
            write_color(pixelColor, N_LOOPS, i,j);
        }
    }
//        std::cout << " Done loop number " <<loop << std::endl;
        if(restartRaytrace)
        {
            return;
        }

    std::cout << "Done!" << std::endl;
}

std::thread raytraceThread;
// routine that generates the image
void RaytraceRenderWidget::Raytrace()
{ // RaytraceRenderWidget::Raytrace()

    restartRaytrace = true;
    if(raytraceThread.joinable())
        raytraceThread.join();
    restartRaytrace = false;

    //clear frame buffer before we start
    //Left side they do glClearColor(0.8, 0.8, 0.6, 1.0);
    frameBuffer.clear(RGBAValue(0.0f, 0.0f, 0.0f,1.0f));
    raytraceThread = std::thread(&RaytraceRenderWidget::RaytraceThread,this);
    raytraceThread.detach();

} // RaytraceRenderWidget::Raytrace()
    

bool RaytraceRenderWidget::convertVectice2Triangle(std::vector<TexturedObject> * textObjs){

    if(textObjs->size()==0)
        return false;
    auto &vertices = textObjs->at(0).vertices;
    auto &normals = textObjs->at(0).normals;
    auto &textureCoords = textObjs->at(0).textureCoords;

    Matrix4 modelview;
    modelview.SetIdentity();
    modelview.SetScale(2,2,2);
    modelview.SetTranslation(Cartesian3(0,0,-10.0));

    for(auto text=textObjs->begin();text!=textObjs->end();text++){
        for(auto &face: text->faceVertices){


            Homogeneous4 hg4v0(vertices[face[0]]);
            hg4v0=modelview*hg4v0;
            auto v0 = hg4v0.Vector();

            Homogeneous4 hg4v1(vertices[face[1]]);
            hg4v1=modelview*hg4v1;
            auto v1 = hg4v1.Vector();

            Homogeneous4 hg4v2(vertices[face[2]]);
            hg4v2=modelview*hg4v2;
            auto v2 = hg4v2.Vector();

//            Triangle *tri = new Triangle(vertices[face[0]], vertices[face[1]], vertices[face[2]]);

            Triangle *tri = new Triangle(v0,v1,v2);
            tri->faceNormal = normals[face[0]];
            this->triangleObjs.push_back(tri);
        }
    }
    return true;
}

RGBAValue RaytraceRenderWidget::getHitColor(Ray &ray, HitList &objList, int depth)
{
     if (depth > 5)
          return RGBAValue(0, 0, 0);

     HitPoint tempHp;
     if (objList.hit(ray, tempHp))
     {
//           std::cout<<"hitted "<<tempHp.point<<std::endl;
           Cartesian3 randomVec = Cartesian3::randomVector(0, 1);
           Cartesian3 dir = tempHp.normal+randomVec ;
           Ray reflectRay(tempHp.point, dir);

//          double pdf = 1;
//          RGBAValue color;
//          RGBAValue emmited = tempHp.matptr->emmitted();
//          auto scatterRay = tempHp.matptr->scatter(ray, tempHp, color, pdf);

//          return emmited + color * tempHp.matptr->scatterPdf(scatterRay, tempHp, pdf) * getHitColor(scatterRay, objList, depth + 1) * (1 / pdf);

           return 0.5 * getHitColor(reflectRay, objList, depth + 1);
     }

     Cartesian3 unit_direction = ray.direction().unit();
     auto t = 0.5 * (unit_direction.y + 1.0);
     // cout<<t<<endl;
     auto color = (1.0 - t) * RGBAValue(255, 255, 255) + t * RGBAValue(0.12 * 255, 0.24 * 255, 0.2 * 255);
     // cerr<<color<<endl;
     return color;
}




void RaytraceRenderWidget::readSceneObj(HitList &scene){

    for(auto obj : this->triangleObjs){
        scene.add(obj);
    }
}



// mouse-handling
void RaytraceRenderWidget::mousePressEvent(QMouseEvent *event)
    { // RaytraceRenderWidget::mousePressEvent()
    // store the button for future reference
    int whichButton = event->button();
    // scale the event to the nominal unit sphere in the widget:
    // find the minimum of height & width   
    float size = (width() > height()) ? height() : width();
    // scale both coordinates from that
    float x = (2.0 * event->x() - size) / size;
    float y = (size - 2.0 * event->y() ) / size;

    
    // and we want to force mouse buttons to allow shift-click to be the same as right-click
    int modifiers = event->modifiers();
    
    // shift-click (any) counts as right click
    if (modifiers & Qt::ShiftModifier)
        whichButton = Qt::RightButton;
    
    // send signal to the controller for detailed processing
    emit BeginScaledDrag(whichButton, x,y);
    } // RaytraceRenderWidget::mousePressEvent()
    
void RaytraceRenderWidget::mouseMoveEvent(QMouseEvent *event)
    { // RaytraceRenderWidget::mouseMoveEvent()
    // scale the event to the nominal unit sphere in the widget:
    // find the minimum of height & width   
    float size = (width() > height()) ? height() : width();
    // scale both coordinates from that
    float x = (2.0 * event->x() - size) / size;
    float y = (size - 2.0 * event->y() ) / size;
    
    // send signal to the controller for detailed processing
    emit ContinueScaledDrag(x,y);
    } // RaytraceRenderWidget::mouseMoveEvent()
    
void RaytraceRenderWidget::mouseReleaseEvent(QMouseEvent *event)
    { // RaytraceRenderWidget::mouseReleaseEvent()
    // scale the event to the nominal unit sphere in the widget:
    // find the minimum of height & width   
    float size = (width() > height()) ? height() : width();
    // scale both coordinates from that
    float x = (2.0 * event->x() - size) / size;
    float y = (size - 2.0 * event->y() ) / size;
    
    // send signal to the controller for detailed processing
    emit EndScaledDrag(x,y);
    } // RaytraceRenderWidget::mouseReleaseEvent()
