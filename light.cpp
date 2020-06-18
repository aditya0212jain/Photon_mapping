#include "light.h"
#include "scene.h"
#include "objects.h"
// #include "light.h"
#include "glm/glm.hpp"


Ray::Ray(){
    origin = glm::vec3(0,0,0);
    direction = glm::vec3(0,0,0);
}

Ray::Ray(glm::vec3 o,glm::vec3 d){
        origin = o;
        direction = d;
}

LightPoint::LightPoint(glm::vec3 o){
    origin = o;
    initial_origin = o;
    color = ColorRGB(1,1,1);
}

std::vector<glm::vec3> LightPoint::get_rand(int num)
{
    std::vector<glm::vec3> randx;
    for(int i=0;i<num;i++)
    {
        // float r1 = float(rand())/float((RAND_MAX));
        glm::vec3 rand1 = glm::vec3(origin.x - (float)3 + (float(rand())/float((RAND_MAX)))*(float)6, origin.y, origin.z-(float)3 + (float(rand())/float((RAND_MAX)))*(float)6);
        // glm::vec3 rand1 = origin;

        randx.push_back(rand1);
    }
    return randx;
}

float LightPoint::DistanceDrop(glm::vec3 intersection){
    glm::vec3 t = origin - intersection;
    float dis2 = dot(t,t);
    float dis = sqrt(dis2);
    float ka=1,kb=1,kc=2;
    // this was very important
    float disinc = 70/((dis2*kc)+(dis*kb)+(ka));
    // disinc = 1;
    return disinc;
}

Ray LightPoint::getL(glm::vec3 intersection){
    glm::vec3 t = origin - intersection;
    t = normalize(t);
    Ray r(intersection,t);
    return r;
}

float LightPoint::ShadowDrop(Scene* s,glm::vec3 intersection){
    glm::vec3 po= origin - intersection;
    float t_light = dot(po,po);
    po = normalize(po);
    Ray ray(intersection,po);
    float minD = INFINITY;
    float t;
    for(int i=0;i<s->scene_objects_ptr.size();i++){
        if(s->scene_objects_ptr[i]->intersect(ray,t)&&s->scene_objects_ptr[i]->type!=2&&s->scene_objects_ptr[i]->type!=3){//
            if(t<minD){
                minD = t;
            }
        }
    }
    if(minD<t_light){
        return 0;
    }else
    {
        return 1;
    }
    
}

void LightPoint::update_position(float angle){
    // origin = initial_origin;
    // origin = rotate_point_yconstant(origin,angle);
}

void LightPoint::get_rand_origin(){
    origin = glm::vec3(initial_origin.x - (float)3 + (float(rand())/float((RAND_MAX)))*(float)6, initial_origin.y, initial_origin.z-(float)3 + (float(rand())/float((RAND_MAX)))*(float)6);
}  