#ifndef HITABLE_H
#define HITABLE_H

#include "ray.h"
#include "Cartesian3.h"
#include <math.h>
#include <iostream>
#include "material.h"

//typedef struct HitPoint
//{
//    double t;
//    Cartesian3 point;
//    Cartesian3 normal;
////    Triangle *triptr
//    Material * matptr;

//} HitPoint;
class HitPoint;


class Hitable
{
public:
    virtual bool hit(Ray &ray, double &closest_point, double &farthest_point, HitPoint &hitpoint){return true;};
};


class HitPoint{
    public:
    double t;
    Cartesian3 point;
    Cartesian3 normal;
    Hitable * objptr;
//     material * matptr;
    HitPoint(){}
};



#endif
