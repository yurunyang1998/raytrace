#include "triangle.h"

bool Triangle::hit(Ray &ray, double &min_t, double &closest_point, HitPoint &hitpoint){


    Cartesian3 v1v0 = v1-v0;
    Cartesian3 v2v0 = v2-v0;
    Cartesian3 N = v1v0.cross(v2v0);

    float NdotRayDir = N.dot(ray.direction());
    if(fabs(NdotRayDir)<1e-8)return false;

    float t = (N.dot(v0) + N.dot(ray.origin()))/NdotRayDir;
    if(t<0 || t>closest_point)
        return false;

    Cartesian3 P = ray.origin() + t*ray.direction();

    Cartesian3 eCrossVp;
    Cartesian3 vp0 = P-v0;
    Cartesian3 e0 = v1-v0;
    eCrossVp = e0.cross(vp0);
    if(N.dot(eCrossVp)<0)
        return false;

    Cartesian3 e1 = v2-v1;
    Cartesian3 vp1 = P-v1;
    eCrossVp = e1.cross(vp1);
    if(N.dot(eCrossVp)<0)
        return false;

    Cartesian3 e2 = v0-v2;
    Cartesian3 vp2 = P-v2;
    eCrossVp = e2.cross(vp2);
    if(N.dot(eCrossVp) <0)
        return false;


    closest_point = t;
    hitpoint.t = t;
    hitpoint.point = ray.point(t);
    hitpoint.normal = this->faceNormal;
    hitpoint.objptr = this;

    return true;


}
