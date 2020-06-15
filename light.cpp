#include "light.h"
#include "scene.h"

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

float LightPoint::DistanceDrop(glm::vec3 intersection){
    glm::vec3 t = origin - intersection;
    float dis2 = dot(t,t);
    float dis = sqrt(dis2);
    float ka=1,kb=1,kc=2;
    // this was very important
    float disinc = 80/((dis2*kc)+(dis*kb)+(ka));
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
        if(s->scene_objects_ptr[i]->intersect(ray,t)&&s->scene_objects_ptr[i]->type<2){
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
