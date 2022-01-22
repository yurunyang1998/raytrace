#ifndef RAYTRACETOOLS_H
#define RAYTRACETOOLS_H

#include "Cartesian3.h"


class Ray{
private:
   Cartesian3 orig;
   Cartesian3 dir;
public:
   Ray(Cartesian3 origin, Cartesian3 direction);
   Cartesian3 origin(){ 
       return orig; };
   Cartesian3 direction() {
       return dir;
   }
   Cartesian3 at(double t){
       return orig+t*dir;
   }
   
};

#endif // RAYTRACETOOLS_H
