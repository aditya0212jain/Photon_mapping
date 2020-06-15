#ifndef LIGHT_H
#define LIGHT_H

#include <vector>
#include <iostream>
#include "glm/glm.hpp"

typedef glm::vec3 ColorRGB;


class Ray{
    public:
    glm::vec3 origin;
    glm::vec3 direction;

    Ray();

    Ray(glm::vec3 o,glm::vec3 d);

};

class Photon{
    public:
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 spectral;
};

class Scene;

class LightPoint{

    public:
    glm::vec3 origin;
    ColorRGB color;
    glm::vec3 initial_origin;

    LightPoint(glm::vec3 o);

    float DistanceDrop(glm::vec3 intersection);

    Ray getL(glm::vec3 intersection);

    float ShadowDrop(Scene* s,glm::vec3 intersection);

    void update_position(float angle);

};

#endif