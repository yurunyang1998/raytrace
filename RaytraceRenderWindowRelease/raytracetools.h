#ifndef RAYTRACETOOLS_H
#define RAYTRACETOOLS_H

#include "Cartesian3.h"
#include <memory>
#include <climits>
#include "RGBAValue.h"
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
    virtual double intersect(Ray &r)=0;
    Cartesian3 normal;
    int objType; //0 means triangle, 1 means shpere
    Refl_t reflectType;      // reflection type (DIFFuse, SPECular, REFRactive)
};

class hitList{

public:
    std::vector<hitable*> objects;
    hitList(){};
    void add(hitable * object){
        objects.push_back(object);
    }
    bool intersect(Ray &r, double &t, int &id){
       double d, inf=t=1e19;
       for(int i=int(objects.size());i--;)
           if((d=objects[i]->intersect(r)) && d<t){
               t=d;
               id=i;
           }
       return t<inf;
     }

};

class xy_rect : public hitable {
public:
    xy_rect() {}

    xy_rect(double _x0, double _x1, double _y0, double _y1, double _k, Cartesian3 normal_)
            : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k){
        normal = normal_;
    };

    double intersect(Ray& r){

        auto t = (k-r.origin().z) / r.direction().z;

            auto x = r.origin().x + t*r.direction().x;
            auto y = r.origin().y + t*r.direction().y;
            if (x < x0 || x > x1 || y < y0 || y > y1)
                return 1e20;
            return t;
    }

public:
    double x0, x1, y0, y1, k;
};

class xz_rect : public hitable {
public:
    xz_rect() {}

    xz_rect(double _x0, double _x1, double _z0, double _z1, double _k, Cartesian3 normal_)
            : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k) {
        normal = normal_;
    };

    double intersect(Ray& r){
        auto t = (k-r.origin().y) / r.direction().y;
           auto x = r.origin().x + t*r.direction().x;
           auto z = r.origin().z + t*r.direction().z;
           if (x < x0 || x > x1 || z < z0 || z > z1)
               return 1e20;
           return t;
    }
public:
    double x0, x1, z0, z1, k;
};



class yz_rect : public hitable {
public:
    yz_rect() {}

    yz_rect(double _y0, double _y1, double _z0, double _z1, double _k, Cartesian3 normal_)
            : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k) {
        normal = normal_;
    };

    double intersect(Ray& r){
        auto t = (k-r.origin().x) / r.direction().x;
            auto y = r.origin().y + t*r.direction().y;
            auto z = r.origin().z + t*r.direction().z;
            if (y < y0 || y > y1 || z < z0 || z > z1)
                return 1e20;
            return t;
    }
public:
    double y0, y1, z0, z1, k;

};



class box: public hitable{
public:
    Cartesian3 box_min, box_max;
    hitList planes;
    int hittdPlaneId;
    Cartesian3 normal;
    std::vector<hitable*> sides;
    Cartesian3 color;
    Cartesian3 emitte;

    box(Cartesian3 p0, Cartesian3 p1, Cartesian3 color_, Cartesian3 emmit_, Refl_t rf_){
        box_min = p0;
        box_max = p1;
        this->color = color_;
        this->emitte = emmit_;
        this->reflectType = rf_;
        objType = 2;

        planes.add(new xy_rect(p0.x, p1.x, p0.y, p1.y, p1.z, Cartesian3(0,0,1)));
        planes.add(new xy_rect(p0.x, p1.x, p0.y, p1.y, p0.z, Cartesian3(0,0,-1)));

        planes.add(new xz_rect(p0.x, p1.x, p0.z, p1.z, p1.y, Cartesian3(0,1,0)));
        planes.add(new xz_rect(p0.x, p1.x, p0.z, p1.z, p0.y, Cartesian3(0,-1,0)));

        planes.add(new yz_rect(p0.y, p1.y, p0.z, p1.z, p1.x, Cartesian3(1,0,0)));
        planes.add(new yz_rect(p0.y, p1.y, p0.z, p1.z, p0.x, Cartesian3(-1,0,0)));

    }
    double intersect(Ray &r){
        double t;
        int id;
        if(planes.intersect(r, t, id)){
            this->hittdPlaneId =id;
            this->normal = planes.objects[hittdPlaneId]->normal;
            return t;
        }

        return 0;
    }

};



class triangle:public hitable{
public:
    std::vector<screenVertexWithAttributes>  vertexsList;
    Cartesian3 normal;
    Cartesian3 emitte;
//    Refl_t reflectTypeTri;
    triangle(){
        this->objType = 0;
    }
    triangle(std::vector<screenVertexWithAttributes> vertexsList_);
    double intersect(Ray &r);
};


class Sphere : public hitable {
public:
      double rad;       // radius
      Refl_t reflectTypeSphere;      // reflection type (DIFFuse, SPECular, REFRactive)
      Cartesian3 position, emitte, color;      // position, emission, color
      Sphere(double rad_, Cartesian3 p_, Cartesian3 e_, Cartesian3 c_, Refl_t refl_):
        rad(rad_), position(p_), emitte(e_), color(c_), reflectTypeSphere(refl_) {
          this->objType = 1;
          this->reflectType =reflectTypeSphere;
      };

      double intersect(Ray &r)  { // returns distance, 0 if nohit
        Cartesian3 op = position-r.origin() ; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
        double t, eps=1e-4, b=op.dot(r.direction()), det=b*b-op.dot(op)+rad*rad;
        if (det<0) return 0; else det=sqrt(det);
        return (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
      }
};













class RayTrace{
private:
    FakeGL *fakegl;


public:
    RayTrace(FakeGL * fakegl_){
        this->fakegl = fakegl_;
    }
    int run(int command);

    Cartesian3 radiance(Ray &r, int depth, hitList &world, int Xi);


};














#endif // RAYTRACETOOLS_H
