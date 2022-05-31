#ifndef MATERIAL_H
#define MATERIAL_H

#include "Cartesian3.h"
#include "RGBAImage.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
class Material
{


public:
    bool setFromFile;
    std::string name;
    Cartesian3 ambient;
    Cartesian3 diffuse;
    Cartesian3 specular;
    Cartesian3 emissive;
    float shininess;
    //stuff for reflection and refraction
    float reflectivity;
    float indexOfRefraction;
    float transparency;
    RGBAImage *texture;
    Material();
    Material(Cartesian3 ambient,Cartesian3 diffuse,Cartesian3 specular,Cartesian3 emissive,float shininess,std::istream &textureStream);
    Material(Cartesian3 ambient,Cartesian3 diffuse,Cartesian3 specular,Cartesian3 emissive,float shininess); //no texture in constructor;
    ~Material();
    static std::vector<Material*> readMaterials(std::istream &materialStream);
};

#endif // MATERIAL_H
