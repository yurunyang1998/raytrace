#ifndef RAY_H
#define RAY_H

#include "Cartesian3.h"

class Ray{
private:
    Cartesian3 ori,dir;

public:
    Ray(Cartesian3 &ori_, Cartesian3 &direction){
        ori = ori;
        dir = direction;
    }

    Cartesian3 origin() const{
        return ori;
    }

    Cartesian3 direction() const{
        return dir;
    }

    Cartesian3 point(double t){
        return ori+t*dir;
    }
};






#endif
