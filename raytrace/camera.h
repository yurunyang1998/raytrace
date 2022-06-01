#ifndef CAMERA_H
#define CAMERA_H


#include "ray.h"
class Camera
{
private:
    int image_width;
    int image_height;
    double viewport_height;
    double viewport_width;
    double focal_length;

    Cartesian3 origin;
    Cartesian3 horizontal;
    Cartesian3 vertical;
    Cartesian3 lower_left_corner;

public:
    Camera(int image_width_,
           int image_height_,
           double viewport_height_,
           double viewport_width,
           double focal_length,
           Cartesian3 origin=Cartesian3());

    Ray castRay(double u, double v);


};

#endif // CAMERA_H
