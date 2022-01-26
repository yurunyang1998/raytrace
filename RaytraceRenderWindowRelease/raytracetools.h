#ifndef RAYTRACETOOLS_H
#define RAYTRACETOOLS_H

#include "Cartesian3.h"
#include <memory>
#include <climits>
#include "RGBAValue.h"
#include "hit_record.h"
#include <vector>
#include <omp.h>
#include <math.h>
#define M_PI 3.1415


class FakeGL;
class screenVertexWithAttributes;

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


enum Refl_t { DIFF, SPEC, REFR };  // material types, used in radiance()

class hitable{
public:
    virtual bool hit(Ray r, float min_t, float max_t, hit_record &rec)=0;
    virtual Cartesian3 emitted(const Ray& r_in, const hit_record& rec, double u, double v,
                              const Cartesian3& p) const {
            return Cartesian3(0,0,0);
        }
    Cartesian3 emitte, normal;
    Refl_t reflectType;
    int objType; //0 means triangle, 1 means shpere
};




class Sphere: public hitable{
public:
    double radius;
    Cartesian3 position, emission, color;

    Sphere(double radius_, Cartesian3 position_, Cartesian3 emission_, Cartesian3 color_, Refl_t rf_):
        radius(radius_),position(position_),emission(emission_),color(color_){
        objType = 1;
        reflectType = rf_;
    }

    bool hit(Ray r, float min_t, float max_t, hit_record &rec){
        Cartesian3 oc = position-r.origin();
        double t, eps=0.0001, b = oc.dot(r.direction()), det=b*b-oc.dot(oc)+radius*radius;
        if(det<0)
            return false;
        else
            det = sqrt(det);
       t = (t=b-det)>eps ? t:((t-b+det)>eps ?t:0);
       if(t<min_t || t>max_t){
           return false;
       }
       rec.hitPoint = r.at(t);
       rec.t = t;
       rec.objptr=this;
       return true;

    }

};



class triangle : public hitable{
private:
    std::vector<screenVertexWithAttributes>  facePosition;
public:
    triangle(){};
    triangle(std::vector<screenVertexWithAttributes> face){
      this->facePosition = face;
    //  this->normal = (*face).at(0);
      this->emitte = Cartesian3(0,0,0);
       objType = 0;
    }
    bool hit(Ray r, float min_t, float max_t, hit_record &rec);
};



class hitList{
private:
    std::vector<hitable*> objects;

public:
    hitList(){};
    void add(hitable * object){

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

    Cartesian3 radiance(Ray &r, int depth, hitList &world, int Xi);


};














#endif // RAYTRACETOOLS_H
