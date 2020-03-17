#ifndef OBJECTS_H
#define OBJECTS_H
#include "utils.h"

class Object{

    public:
    int type;
    int id;
    float reflection;
    float refraction;
    glm::vec3 color;//RGB format
    float diffuse;
    float specular;
    glm::vec3 emissionColor;
    bool isGlass=false;
    bool isSnowman= false;
    int belong=0;
    float absorption;

    void set_properties(float refl,float refr,glm::vec3 colr,float diff,float spec);

    virtual bool intersect(Ray r,float& t){return false;};

    virtual Ray getNormal(glm::vec3 intersection){return Ray();};

    virtual void update_position(float angle){ return ;};
};


class Sphere: public Object{

    float radius;
    glm::vec3 center;
    glm::vec3 initial_center;

    public:
    
    Sphere(float r,glm::vec3 c);

    Sphere(float r,glm::vec3 c,int glass);

    bool intersect(Ray r,float& t);

    Ray getNormal(glm::vec3 intersection);

    void update_position(float angle);

};

class Cylinder: public Object{

    float radius;
    float length;
    glm::vec3 bottom_center;
    glm::vec3 initial_center;
    glm::vec3 direction;
    public:
    Cylinder(float r,float c,glm::vec3 b_c);

    bool intersect(Ray r,float& t);


    Ray getNormal(glm::vec3 intersection);

    void update_position(float angle);

};


class Wall: public Object{

    public:
    glm::vec3 center;
    glm::vec3 normal;
    
    Wall(glm::vec3 cl,glm::vec3 n=glm::vec3(0.0,0.0,0.0));

    bool intersect(Ray r,float& t);

    Ray getNormal(glm::vec3 intersection);

};




#endif