#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include <iostream>
#include "glm/glm.hpp"
#include "utils.h"
#include "objects.h"
#include "camera.h"
#include "./kdtree++/kdtree.hpp"



class LightPoint;

class Scene{

    public:
    std::vector<Object> scene_objects;
    std::vector<Object *> scene_objects_ptr;
    std::vector<Object *> glassy_scene_objects_ptr;
    std::vector<Object *> snow_man;
    std::vector<Sphere> scene_spheres;
    std::vector<Cylinder> scene_cylinders;
    std::vector<Object *> tubelights;
    std::vector<LightPoint> light_objects;
    std::vector<LightPoint *> lights;
    KDTree::KDTree<3, KDTreeNode> photon_map;
    KDTree::KDTree<3, KDTreeNode> caustic_photon_map;
    int toggle=2;
    bool anti_aliasing=false;
    // just for testing
    float R=3;
    float angle_x=0;
    float angle_y=0;
    float angle_sphere=0;
    float angle_lights=0;


    glm::vec3 rotate_point_xconstant(glm::vec3 point,float angle);
    glm::vec3 rotate_point_yconstant(glm::vec3 point,float angle);
    

    Camera camera;

    Scene();

    void make_scene();

    void place_objects();

    void place_lights();

    void place_all_objects();

    void place_all_objects_2();

    void place_snowman();

    void update_camera(int type,int forward);

    void update_spheres_position(int type);

    void update_light_positions(int type);

    void trace_prep();

    void toggle_aliasing(){
        anti_aliasing = !anti_aliasing;
    }

    // Note: updated the trace_ray with trace_global_illumination
    // so the program may not run as of now
    ColorRGB trace(int x,int y,int widht,int height);

    ColorRGB trace_ray(Ray ray,int depth,Object* exclude);

    void trace_photon(Ray p,int depth,int& n_photon,glm::vec3 color);

    void trace_caustic_photon(Ray p,int depth,int& n_photon,int glass,glm::vec3 color);

    ColorRGB shade(Object* obj,Ray normal,glm::vec3 intersection,Ray incident);

    // 20th March 
    // Adding for PhotonMapping
    // this function will update the photonmap 
    // should be called everytime before tracing rays
    void compute_photon_map();

    void compute_caustic_photon_map();

    // Illumination components
    ColorRGB indirect_illumination(glm::vec3 intersectionPoint,Object* nearest_object);

    ColorRGB caustic_illumination(glm::vec3 intersectionPoint,Object* nearest_object);

    ColorRGB direct_illumination(Object* obj,Ray normal,glm::vec3 intersection,Ray incident);

    ColorRGB montecarlotrace_illumination(Object* obj,Ray normal,glm::vec3 intersection,Ray incident,int depth);
};

#endif