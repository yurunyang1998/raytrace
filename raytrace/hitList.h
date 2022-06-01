#ifndef HITLIST_H
#define HITLIST_H

#include "hitable.h"
#include "Cartesian3.h"
#include <vector>


class HitList{
public:
    std::vector<Hitable*> objList;

    void add(Hitable* obj){
        objList.push_back(obj);
    }

    bool hit(Ray &ray, HitPoint &hitPoint){

        double mint_t = 0;
        double closest_point = INFINITY;
        bool hitted = false;
        HitPoint tempHitPoint;
        for(auto obj : objList){
            if(obj->hit(ray, mint_t, closest_point, tempHitPoint)){
//                std::cout<<"hitted triangle"<<std::endl;
                hitted = true;
                closest_point = tempHitPoint.t;
                hitPoint = tempHitPoint;
            }
        }

        return hitted;

    }


};


#endif
