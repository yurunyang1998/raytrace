#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include "Cartesian3.h"
#include <memory>

class hitable;

class hit_record{
public:
    hit_record(){};
    hitable * objptr;
    Cartesian3 hitPoint;
    float t;
    Cartesian3 normal;
};

#endif // HIT_RECORD_H
