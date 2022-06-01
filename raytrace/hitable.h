#ifndef HITABLE_H
#define HITABLE_H

#include "ray.h"
#include "Cartesian3.h"
#include <math.h>
#include <iostream>
#include "material.h"


typedef struct HitPoint
{
    double t;
    Cartesian3 point;
    Cartesian3 normal;
    Material * matptr;

} HitPoint;

// class HitPoint{
//     public:
//     double t;
//     Cartesian3 point;
//     Cartesian3 normal;
//     material * matptr;
//     HitPoint(){}
// };


class Hitable
{
public:
    virtual bool hit(Ray &ray, double &closest_point, double &farthest_point, HitPoint &hitpoint){return true;};
};

class Sphere : public Hitable
{
public:
    Cartesian3 center;
    double radius;
    Material *matptr = nullptr;
    Sphere(Cartesian3 center_, double radius_/*, Material *matptr_*/)
    {
        center = center_;
        radius = radius_;
//        matptr = matptr_;
    }

    virtual bool hit(Ray &ray, double &min_t, double &closest_point, HitPoint &hitpoint)
    {

        Cartesian3 oc = ray.origin() - center;
        auto a = ray.direction().squared();
        auto half_b = ray.direction().dot(oc);
        auto c = oc.squared() - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0)
            return false;
        auto sqrtd = sqrt(discriminant);

        auto root = (-half_b - sqrtd) / a;
        if (root < min_t || closest_point < root)
        {
            root = (-half_b + sqrtd) / a;
            if (root < min_t || closest_point < root)
                return false;
        }

        hitpoint.t = root;
        hitpoint.point = ray.point(root);
        hitpoint.normal = (hitpoint.point - center) / radius;
//        hitpoint.matptr = this->matptr;

        // std::cout<<"hit sphere"<<std::endl;
        return true;
    }
};

#endif
