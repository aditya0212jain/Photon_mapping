#ifndef OBJECTS_H
#define OBJECTS_H
#include "utils.h"
#include <fstream>
#include <sstream>


/*
type variable
Sphere 1
Cylinder 2
Wall 3
Triangle 4
MEsh 5

*/

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

    virtual Ray b_box(){return Ray();};
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

    Ray b_box();

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

    Ray b_box();

};


class Wall: public Object{

    public:
    glm::vec3 center;
    glm::vec3 normal;
    
    Wall(glm::vec3 cl,glm::vec3 n=glm::vec3(0.0,0.0,0.0));

    bool intersect(Ray r,float& t);

    Ray getNormal(glm::vec3 intersection);

    Ray b_box();

};

class Triangle: public Object{

    public:
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
    glm::vec3 normal;

    Triangle();

    Triangle(glm::vec3 a0,glm::vec3 a1,glm::vec3 a2,glm::vec3 translate,float scale);

    bool intersect(Ray r,float& t);

    Ray getNormal(glm::vec3 intersection);

    Ray b_box();
};

class Box: public Object{
    public:
    glm::vec3 max;
    glm::vec3 min;

    Box();

    void update(glm::vec3 point);

    bool intersect(Ray r);

};

class Mesh:public Object{

    public:
    std::vector<Triangle*> triangle_list;
    Triangle* intersected_triangle;
    Box boxAABB;

    Mesh(const char* object_file="bunny.obj",glm::vec3 translate=glm::vec3(0,0,0),float scale=1.0);

    Mesh(std::vector<Triangle> tl);

    bool intersect(Ray r,float& t);

    Ray getNormal(glm::vec3 intersection);

    Ray b_box();

};




#endif