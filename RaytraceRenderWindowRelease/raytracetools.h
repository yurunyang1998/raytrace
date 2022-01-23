#ifndef RAYTRACETOOLS_H
#define RAYTRACETOOLS_H

#include "Cartesian3.h"
#include "fakegl.h"
#include <memory>

class Ray{
private:
   Cartesian3 orig;
   Cartesian3 dir;
public:
   Ray(Cartesian3 origin, Cartesian3 direction){
       orig = origin;
       dir = direction;
   }
   Cartesian3 origin(){ 
       return orig; };
   Cartesian3 direction() {
       return dir;
   }
   Cartesian3 at(double t){
       return orig+t*dir;
   }
   
};

class hit_record{
public:
    hit_record(){};
    Cartesian3 hitPoint;
    float t;
    Cartesian3 normal;
};


class hitable{
public:
   virtual bool hit(Ray r, float min_t, float max_t, hit_record &rec)=0;
};



class triangle : hitable{
private:
    std::vector<Cartesian3> * facePosition;
public:
    triangle(){};
    triangle(std::vector<Cartesian3> * face);
    bool  hit(Ray r, float min_t, float max_t, hit_record &rec);
};



class hitList{
private:
    std::vector<std::shared_ptr<hitable>> objects;

public:
    hitList(){};
    void add(std::shared_ptr<hitable> object){
        objects.push_back(object);
    }
    bool hit(Ray r, float min_t, float max_t, hit_record &rec);;


};



class RayTrace{
private:
    FakeGL *fakegl;


public:
    RayTrace(FakeGL * fakegl_){
        this->fakegl = fakegl_;
    }
    int run();

    RGBAValue rayColor(Ray &r, int depth, hitList &world);


};














#endif // RAYTRACETOOLS_H
