#include "camera.h"

Camera::Camera(int image_width_, int image_height_,
               double viewport_height_, double viewport_width_,
               double focal_length_, Cartesian3 origin_) :
    image_width(image_width_),
    image_height(image_height_),
    viewport_height(viewport_height_),
    viewport_width(viewport_width_),
    focal_length(focal_length_),
    origin(origin_)
{
    horizontal = Cartesian3(viewport_width_, 0, 0);
    vertical = Cartesian3(0, viewport_height, 0);
    lower_left_corner = origin - horizontal/2 -vertical/2 - Cartesian3(0,0,focal_length);
}

Ray Camera::castRay(double u, double v){

    Cartesian3 dir(lower_left_corner + u*horizontal + v*vertical);
    return Ray(origin,dir);

}
