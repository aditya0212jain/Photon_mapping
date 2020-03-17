#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <iostream>
#include <glm/glm.hpp>

class Camera{
    public:
    glm::vec3 position;

    Camera(){}

    Camera(glm::vec3 p){
        position = p;
    }
};


#endif