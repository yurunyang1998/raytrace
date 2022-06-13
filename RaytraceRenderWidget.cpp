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
    frameBuffer.clear(RGBAValue(0.0f, 0.0f, 0.0f,1.0f));

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
                int depth =0;
                auto hitcolor = getHitColor(ray, scene, depth);
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

    depestZ = 0.0;
    if(textObjs->size()==0)
        return false;
    auto &vertices = textObjs->at(0).vertices;
    auto &normals = textObjs->at(0).normals;
    auto &textureCoords = textObjs->at(0).textureCoords;

    modelview.SetIdentity();
    if(this->renderParameters->centreObject){
//        modelview = modelview;
        modelview.SetTranslation(Cartesian3(this->renderParameters->xTranslate,
                                            this->renderParameters->yTranslate,
                                            this->renderParameters->zoomScale-5));
    }
    modelview = modelview * this->renderParameters->rotationMatrix;

//    modelview.SetTranslation(Cartesian3(0, 0, this->renderParameters->zoomScale));

    this->triangleObjs.clear();
    for(auto text=textObjs->begin();text!=textObjs->end();text++){
        int faceIndex =0;
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
            tri->materialptr = text->material;
            tri->faceNormal = normals[face[0]];
            tri->v0n = normals[face[0]];
            tri->v1n = normals[face[1]];
            tri->v2n = normals[face[2]];

            tri->v0t = textureCoords[text->faceTexCoords[faceIndex][0]];
            tri->v1t = textureCoords[text->faceTexCoords[faceIndex][1]];
            tri->v2t = textureCoords[text->faceTexCoords[faceIndex][2]];

            this->triangleObjs.push_back(tri);
            faceIndex++;

            float maxZ = fmin(v0.z, fmin(v1.z, v2.z));
            if(maxZ < depestZ)
                depestZ = maxZ;

        }
    }
    return true;
}

Cartesian3 RaytraceRenderWidget::BaycentricInterpolation(HitPoint &hitpoint){

    Triangle *tri = dynamic_cast<Triangle*>(hitpoint.objptr);
    Cartesian3 &point = hitpoint.point;

    Cartesian3 &vertex0 = tri->v0;
    Cartesian3 &vertex1 = tri->v1;
    Cartesian3 &vertex2 = tri->v2;

    Cartesian3 vector01 = vertex1 - vertex0;
    Cartesian3 vector12 = vertex2 - vertex1;
    Cartesian3 vector20 = vertex0 - vertex2;

    // now compute the line normal vectors
    Cartesian3 normal01(-vector01.y, vector01.x, 0.0);
    Cartesian3 normal12(-vector12.y, vector12.x, 0.0);
    Cartesian3 normal20(-vector20.y, vector20.x, 0.0);

    // we don't need to normalise them, because the square roots will cancel out in the barycentric coordinates
    float lineConstant01 = normal01.dot(vertex0);
    float lineConstant12 = normal12.dot(vertex1);
    float lineConstant20 = normal20.dot(vertex2);

    float ndotv0 = normal12.dot(vertex0);
    float ndotv1 = normal20.dot(vertex1);
    float ndotv2 = normal01.dot(vertex2);

    // and compute the distance of each vertex from the opposing side
    float distance0 = ndotv0 - lineConstant12;
    float distance1 = ndotv1 - lineConstant20;
    float distance2 = ndotv2 - lineConstant01;

    float alpha = (normal12.dot(point) - lineConstant12) / distance0;
    float beta = (normal20.dot(point) - lineConstant20) / distance1;
    float gamma = (normal01.dot(point) - lineConstant01) / distance2;


    return Cartesian3(alpha, beta, gamma);




}

Ray RaytraceRenderWidget::reflectRay(Ray &inRay, HitPoint &hitPoint){

//    Triangle * triptr = dynamic_cast<Triangle *>(hitPoint.objptr);
    Cartesian3 &normal = hitPoint.normal;
    auto reflectDir = 2*normal.dot(-1*inRay.direction())*normal + inRay.direction();
    return Ray(hitPoint.point, reflectDir);

}

Ray RaytraceRenderWidget::refractionRay(Ray &inRay, HitPoint &hitPoint){

    Triangle * triptr = dynamic_cast<Triangle *>(hitPoint.objptr);
    Cartesian3 &normal = triptr->faceNormal;
    auto ior =  triptr->materialptr->indexOfRefraction;

    auto cosTheta = std::fmin(normal.dot(inRay.direction() * -1), 1.0);
    auto perp =  ior * (inRay.direction() + cosTheta * normal);
    auto parallel = -sqrt(fabs(1.0 - perp.squared())) * normal;
    Cartesian3 refractionDir = perp + parallel;
    return Ray(hitPoint.point, refractionDir);



}



bool RaytraceRenderWidget::inShadow(RGBAValue &color, std::vector<Light*> lights, HitList &objList, HitPoint hp){

    Triangle * triptr = dynamic_cast<Triangle *>(hp.objptr);

    Cartesian3 averagelightColor;
    bool hitflag = false;
    int hitLightNum=0.0;
    RGBAValue tempColor;
    for(auto light:lights){
//        auto light = lights[0];
        hitflag = true;
        Cartesian3 biasVec(1,1,1);

        Cartesian3 lightPos = (modelview*light->GetPosition()).Vector();
        Cartesian3 rayori = hp.point;//+ biasVec*1e-8;
        Cartesian3 reflecDir = (rayori - lightPos).unit();
        Ray rayTolight(rayori, reflecDir);
        if(objList.hit(rayTolight, hp)==false){ // in the light area

            int a;

        }else{
            if(!(hp.point == rayori)){
                std::cout<<" in shadow "<<std::endl;
//                tempColor =  tempColor.modulate(RGBAValue(244,1,1));// + (getHitColor(rayTolight,objList, 0));
                tempColor = (RGBAValue(1,1,255));
            }{
                std::cout<<"same point"<<std::endl;
            }



        }
    }
    if(hitflag){  // in the shadow area

        color = (tempColor);

    }
    return hitflag;

}




// calcluate the proportion of reflection ray
float fresnelSchlick(float inRayReflractionRatio, float outRayReflractionRatio, Cartesian3 inRayDir, Cartesian3 normal){

    float R0 = (inRayReflractionRatio-outRayReflractionRatio)/(inRayReflractionRatio+outRayReflractionRatio);
    R0*=R0;
    float cosTheta = (-1*inRayDir).dot(normal);
    return R0 + (1.0 - R0) * pow(1.0 - cosTheta, 5.0);


}


RGBAValue RaytraceRenderWidget::getHitColor(Ray &ray, HitList &objList, int &depth)
{
    if (depth >= 5)
    {
//        if(this->renderParameters->reflectionEnabled || this->renderParameters->refractionEnabled)
        return RGBAValue(0,0,0);

    }    //         return

     HitPoint tempHp;
     depth++;


     if (objList.hit(ray, tempHp))
     {


            Triangle * triptr = dynamic_cast<Triangle*>(tempHp.objptr);
            Cartesian3 Baycentric = BaycentricInterpolation(tempHp);

            float alpha=Baycentric.x, beta=Baycentric.y, gamma=Baycentric.z;
            if ((alpha < 0.0) || (beta < 0.0) || (gamma < 0.0) || isnan(alpha) || isnan(beta) || isnan(gamma)){
                if(this->renderParameters->interpolationRendering){
                    auto normal = triptr->faceNormal.unit();
                    return RGBAValue(normal.x*255.0, normal.y*255.0, normal.z*255.0);
                }

//                std::cout<<"invalid "<<alpha<<" "<<beta<<" "<<gamma<<std::endl;
//                return RGBAValue(0, 0, 0);
            }

            Cartesian3 normalInterpolation(triptr->v0n.x*alpha + triptr->v1n.x*beta + triptr->v2n.x*gamma,
                                           triptr->v0n.y*alpha + triptr->v1n.y*beta + triptr->v2n.y*gamma,
                                           triptr->v0n.z*alpha + triptr->v1n.z*beta + triptr->v2n.z*gamma);

            //THis Is The Mark 2, Barycentric Interpolation
            if(this->renderParameters->interpolationRendering){
                normalInterpolation = normalInterpolation.unit();
                float nx = abs(normalInterpolation.x);
                float ny = abs(normalInterpolation.y);
                float nz = abs(normalInterpolation.z);

                return RGBAValue(nx*255.0, ny*255.0, nz*255.0);

            }

            Cartesian3 randomVec = Cartesian3::randomVector(0, 1);
            Cartesian3 dir = tempHp.normal+randomVec ;
            Ray diffRay = Ray(tempHp.point,dir);
            Ray reflRay = reflectRay(ray, tempHp);
            Ray refractRay = refractionRay(ray, tempHp);


            RGBAValue color;//(1,255,1);
            if(this->renderParameters->phongEnabled){

                float attenuation = 0;
                for(auto light: this->renderParameters->lights){


//                    Cartesian3 lightPos = light->GetPosition().Vector();
                    Cartesian3 lightPos = (modelview*light->GetPosition()).Vector();
                    attenuation += 1.0/(lightPos-tempHp.point).length();

                }
                float grayscale = /*1.0/*/attenuation;
                return RGBAValue(grayscale*255.0, grayscale*255.0, grayscale*255.0);


            }

            if(this->renderParameters->texturedRendering){
                auto v0t = triptr->v0t;
                auto v1t = triptr->v1t;
                auto v2t = triptr->v2t;

                RGBAImage * textureDir = nullptr;
                if(triptr->materialptr!=nullptr){
                    textureDir = triptr->materialptr->texture;
                }else{
                    textureDir = this->texturedObjects->front().texture;
                }


                float textureWidth = textureDir->width;
                float textureHeight = textureDir->height;

                int intplU = (v0t.x*alpha + v1t.x*beta + v2t.x*gamma)*textureWidth;
                int intplV = (v0t.y*alpha + v1t.y*beta + v2t.y*gamma)*textureHeight;

                auto textColor = (*textureDir)[intplV][intplU];
//                color = RGBAValue(textColor.red,textColor.green, textColor.blue);
                RGBAValue color(1-textColor.red,1-textColor.green, 1-textColor.blue);
                if(this->renderParameters->reflectionEnabled==false &&
                   this->renderParameters->refractionEnabled==false)
                    return color;

                RGBAValue mixLight;
                if(this->renderParameters->refractionEnabled)
                    mixLight = getHitColor(refractRay, objList, depth);

                if(this->renderParameters->reflectionEnabled)
                    mixLight = mixLight + getHitColor(reflRay, objList, depth);


                color = color.modulate(mixLight);
                return color;
            }

            if(this->renderParameters->shadowsEnabled){
                inShadow(color, this->renderParameters->lights, objList, tempHp);
//                return color;
            }

            auto ior =  triptr->materialptr->indexOfRefraction;
            auto normal = tempHp.normal;
            float reflectionPropotion = fresnelSchlick(1.0, ior, ray.direction(), normal);
//            std::cout<<reflectionPropotion<<std::endl;



            std::random_device rd;
            std::uniform_real_distribution<float> range(0, 1);
            std::default_random_engine e{rd()};

            float random = range(e);

            if(this->renderParameters->reflectionEnabled && this->renderParameters->refractionEnabled==false){

                if(random > triptr->materialptr->reflectivity && depth==1){ //diffuse
                    return RGBAValue(0,0,0);
                }
                //mirror ray
                auto finalColor = getHitColor(reflRay, objList, depth);
                if(finalColor == RGBAValue(0,0,0)){
                    return finalColor;
                }else{
                    std::cout<<depth<<std::endl;
                    float depthFloat = depth*1.0;
                    return RGBAValue((depthFloat/N_BOUNCES)*255.0, (depthFloat/N_BOUNCES)*255.0,(depthFloat/N_BOUNCES)*255.0);
                }

            }else if(this->renderParameters->refractionEnabled && this->renderParameters->reflectionEnabled==false)
            {
                if(triptr->materialptr->transparency == 1.0 ){ //if hit a transparent object
                    float depthFloat = depth*1.0;
                    return RGBAValue((depthFloat/N_BOUNCES)*255.0, (depthFloat/N_BOUNCES)*255.0,(depthFloat/N_BOUNCES)*255.0);
                }
                //otherwise, refraction ray
                auto finalColor = getHitColor(refractRay, objList, depth);//+getHitColor(diffRay, objList, depth + 1);
                return finalColor;

            }

            else if(this->renderParameters->reflectionEnabled && this->renderParameters->reflectionEnabled){
                auto finalColor = (1-reflectionPropotion)*getHitColor(refractRay, objList, depth)+(reflectionPropotion)*getHitColor(reflRay, objList, depth);
                return finalColor;
            }

            if(this->renderParameters->centreObject &&  // just for task 1
               !this->renderParameters->interpolationRendering &&
               !this->renderParameters->phongEnabled &&
               !this->renderParameters->shadowsEnabled &&
               !this->renderParameters->texturedRendering &&
               !this->renderParameters->reflectionEnabled &&
               !this->renderParameters->refractionEnabled){

                float fragmentDepth = tempHp.point.z / depestZ;
                return RGBAValue(fragmentDepth*255.0, fragmentDepth*255.0, fragmentDepth*255.0);

            }

//            if(this->renderParameters->shadowsEnabled)
//                return color;
            depth++;
            auto finalColor =  color + (getHitColor(diffRay, objList, depth));
            return  finalColor;
     }


     Cartesian3 unit_direction = ray.direction().unit();
     auto t = 0.5 * (unit_direction.y + 1.0);
     // cout<<t<<endl;
     float depthF = (1.0*depth)/N_BOUNCES;
     auto color = (1.0 - t) * RGBAValue(255, 255, 255) + t * RGBAValue(0.2 * 255, 0.2 * 255, 0.2 * 255);
//     std::cerr<<color<<endl;
//     auto color = RGBAValue(255,255,255);
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
