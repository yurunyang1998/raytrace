#include "raytracetools.h"





triangle::triangle(std::vector<Cartesian3> *face){
  this->facePosition = face;

}


bool triangle::hit(Ray r, float min_t, float max_t, hit_record &rec){

    auto P0 = facePosition->operator[](0);
    auto P1 = facePosition->operator[](1);
    auto P2 = facePosition->operator[](2);

    auto E1 = P1-P0;
    auto E2 = P2-P0;
    auto S  = r.origin()-P0;
    auto S1 = r.direction().cross(E2);
    auto S2 = S.cross(E1);

    float beta = 1/S1.dot(E1);

    float t = beta*(S2.dot(E2));
    float b1 = beta*(S1.dot(S));
    float b2 = beta*(S2.dot(r.direction()));

    if(b1>=0 && b1<=1 && b2>=0 && b2<=1 && (1-b1-b2)>=0 && (1-b1-b2)<=1){ // hitted
        if(t<min_t || t > max_t)   //the hit is not the closest point
            return -1;
        rec.hitPoint = r.at(t);
        rec.t = t;
        return true;
    }else // not histted
        return false;
}


bool hitList::hit(Ray r, float min_t, float max_t, hit_record &rec){

    bool hittedAnything = false;
    for(auto obj : this->objects){
        if(obj->hit(r, min_t, max_t, rec)){
           max_t = rec.t; // max_t means the closest obj so far
           hittedAnything = true;
        }
    }
    return hittedAnything;



}

RGBAValue RayTrace::rayColor(Ray &r, int depth, hitList &world){

    RGBAValue a;

    return a;
}



int RayTrace::run(){
    auto viewport_height = fakegl->frameBuffer.height;
    auto viewport_width = fakegl->frameBuffer.width;
    auto focal_length = 1.0;

    int image_width = fakegl->frameBuffer.width;
    int image_height = fakegl->frameBuffer.height;

    auto origin = Cartesian3(0, 0, 0);
    auto horizontal = Cartesian3(viewport_width, 0, 0);
    auto vertical = Cartesian3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal/2 - vertical/2 - Cartesian3(0, 0, focal_length);

    for (int j = image_height-1; j >= 0; --j) {
            std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                auto u = double(i) / (image_width-1);
                auto v = double(j) / (image_height-1);
                Ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);
                //color pixel_color = ray_color(r);


            }
        }


    return 1;
}
