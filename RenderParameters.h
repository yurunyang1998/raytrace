/////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  -----------------------------
//  Render Parameters
//  -----------------------------
//  
//  This is part of the "model" in the MVC paradigm
//  We separate out the render parameters from the object rendered
//
/////////////////////////////////////////////////////////////////

// include guard
#ifndef _RENDER_PARAMETERS_H
#define _RENDER_PARAMETERS_H

#include "Matrix4.h"
#include "light.h"
#include <vector>
// class for the render parameters
class RenderParameters
    { // class RenderParameters
    public:
    
    // we have a widget with an arcball which stores the rotation
    // we'll be lazy and leave it there instead of moving it here
    
    // we store x & y translations
    float xTranslate, yTranslate;

    // and a zoom scale
    float zoomScale;
    
    std::vector<Light*> lights;

    // we will want two homogeneous matrices holding the rotations for the
    // model and the light.
    Matrix4 rotationMatrix;
    Matrix4 lightMatrix;
    
    // and the booleans
    bool interpolationRendering;
    bool phongEnabled;
    bool texturedRendering;
    bool shadowsEnabled;
    bool reflectionEnabled;
    bool refractionEnabled;
    bool monteCarloEnabled;

    bool centreObject;
    bool scaleObject;

    bool orthoProjection;


    // constructor
    RenderParameters()
        :
        xTranslate(0.0), 
        yTranslate(0.0),
        zoomScale(1.0),
        interpolationRendering(false),
        phongEnabled(false),
        texturedRendering(false),
        shadowsEnabled(false),
        reflectionEnabled(false),
        refractionEnabled(false),
        monteCarloEnabled(false),
        centreObject(false),
        scaleObject(false),
        orthoProjection(false)
        { // constructor
        Light *l1  = new Light(Light::Point,Homogeneous4(0.4f,0.4f,0.4f,1.0f),
                                      Homogeneous4(-0.35f,0,0.5f,1.0f),Homogeneous4(),Homogeneous4(),Homogeneous4());
//        Light *l1  = new Light(Light::Point,Homogeneous4(0.4f,0.4f,0.4f,1.0f),
//                                      Homogeneous4(-0.35f,0,-3.5f,1.0f),Homogeneous4(),Homogeneous4(),Homogeneous4());

        Light *l2 = new Light(Light::Point,Homogeneous4(0.4f,0.4f,0.4f,1.0f),
                                Homogeneous4(0.35f,0,0.5f,1.0f),Homogeneous4(),Homogeneous4(),Homogeneous4());
        Light *l3 = new Light(Light::Point,Homogeneous4(0.4f,0.4f,0.4f,1.0f),
                                     Homogeneous4(0.0,0.0f,0.5f,1.0f),Homogeneous4(),Homogeneous4(),Homogeneous4());

        l1->enabled = true;
        l2->enabled = true;
        l3->enabled = true;

        lights.push_back(l1);
        lights.push_back(l2);
        lights.push_back(l3);

        // because we are paranoid, we will initialise the matrices to the identity
        rotationMatrix.SetIdentity();
        lightMatrix.SetIdentity();
        } // constructor

    // accessor for scaledXTranslate

    ~RenderParameters(){
        for (int i = 0;i<lights.size();i++) {
            delete(lights.at(i));
        }
    }
    }; // class RenderParameters

// now define some macros for bounds on parameters
#define TRANSLATE_MIN -1.0
#define TRANSLATE_MAX 1.0

#define ZOOM_SCALE_LOG_MIN -2.0
#define ZOOM_SCALE_LOG_MAX 2.0
#define ZOOM_SCALE_MIN 0.01
#define ZOOM_SCALE_MAX 100.0

#define LIGHTING_MIN 0.0
#define LIGHTING_MAX 1.0

#define SPECULAR_EXPONENT_LOG_MIN -2.0
#define SPECULAR_EXPONENT_LOG_MAX 2.0
#define SPECULAR_EXPONENT_MIN 0.01
#define SPECULAR_EXPONENT_MAX 100.0

// this is to scale to/from integer values
#define PARAMETER_SCALING 100


// end of include guard
#endif
