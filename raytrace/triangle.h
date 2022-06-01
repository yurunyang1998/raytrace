#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hitable.h"

class Triangle : public Hitable
{
private:
    Cartesian3 v0,v1,v2;
public:
    Cartesian3 faceNormal;
    Triangle(Cartesian3 &v0_, Cartesian3 &v1_, Cartesian3 &v2_):v0(v0_), v1(v1_), v2(v2_){




    };
    virtual bool hit(Ray &ray, double &min_t, double &closest_point, HitPoint &hitpoint);
};

#endif // TRIANGLE_H
