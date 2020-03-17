#include "objects.h"
#include "light.h"

void Object::set_properties(float refl,float refr,glm::vec3 colr,float diff,float spec){
        reflection = refl;
        refraction = refr;
        color = colr;
        diffuse = diff;
        specular = spec;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////Sphere//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Sphere::Sphere(float r,glm::vec3 c){
    // std::cout<<"Sphere radius: "<<radius<<"\n";
    type = 1;
    radius = r;
    center = c; 
    initial_center = c;
    emissionColor = ColorRGB(0,0,0);
}

Sphere::Sphere(float r,glm::vec3 c,int glass){
    type = 1;
    radius = r;
    center = c; 
    initial_center = c;
    emissionColor = ColorRGB(0,0,0);
    if(glass==0){
        isGlass = true;
    }else{
        isGlass = false;
    }
}

bool Sphere::intersect(Ray r,float& t){
    glm::vec3 ce = center - r.origin;// c - e
    float through = dot(ce,r.direction);// (c-e).l
    if(through<0){
        return false;
    }
    float di = dot(ce,ce) - through*through;//d = (c-e)^2 - ((c-e).l)^2
    di = radius*radius - di;// r^2 - d^2
    if(di<0){
        return false;
    }
    float st = sqrt(di);
    float t0 = through - st;
    float t1 = through +st;
    //TODO : Check this part again
    if (t0<0){
        t = t1;
    }else{
        t = t0;
    }
    return true;
}

Ray Sphere::getNormal(glm::vec3 intersection){
    glm::vec3 normal = intersection - center;
    normal = normalize(normal);
    Ray r(intersection,normal);
    return r;
}

void Sphere::update_position(float angle){
    // center = initial_center;
    // center = rotate_point_yconstant(center,angle);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////Cylinder//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Cylinder::Cylinder(float r,float c,glm::vec3 b_c){
    type = 2;
    radius = r;
    length = c;
    bottom_center = b_c;
    initial_center = b_c;
    emissionColor = ColorRGB(1,1,1);
    direction = glm::vec3(0,1,0);
}

bool Cylinder::intersect(Ray r,float& t){
    glm::vec3 new_c = r.origin - bottom_center;
    float b = 2*(new_c.z*r.direction.z + new_c.x*r.direction.x);
    float c = pow(new_c.z,2) + pow(new_c.x,2) - pow(radius,2);
    float a = pow(r.direction.x,2) + pow(r.direction.z,2);
    float d = pow(b,2) - (4*a*c);
    if(d<0){
        return false;
    }
    d = sqrt(d);
    float t0 = (-b-d)/(2*a);
    float t1 = (-b+d)/(2*a);
    if (t0<0){
        t = t1;
    }else{
        t = t0;
    }
    glm::vec3 intersection = r.origin + r.direction*t;
    glm::vec3 cd(0,1,0);
    float len = dot((intersection - bottom_center),cd);
    if(len<0||len>length){
        return false;
    }
    return true;
}


Ray Cylinder::getNormal(glm::vec3 intersection){
    glm::vec3 c(0,1,0);
    glm::vec3 normal = intersection - (bottom_center + (c*(dot((intersection - bottom_center),c))));
    normal = normalize(normal);
    Ray r(intersection,normal);
    return r;
}

void Cylinder::update_position(float angle){
    // bottom_center = initial_center;
    // bottom_center = rotate_point_yconstant(bottom_center,angle);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////Wall//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    
Wall::Wall(glm::vec3 cl,glm::vec3 n){
    type = 3;
    color = cl;
    normal = n;
}

bool Wall::intersect(Ray r,float& t){
    double directions_dot_prod = dot(r.direction,normal);
    if (directions_dot_prod == 0) {// the plane and ray are parallel
        // t = FLT_MAX; // no intersection, at 'infinity'
        return false;
    }
    t = dot(center - r.origin,normal) / directions_dot_prod;

    if (t < 1e-2) { // the plane is behind the ray
        // t = ;
        return false;
    }
    // glm::vec3 intersection = r.origin + r.direction*t;

    // if(intersection.x>2*R||intersection.y>2*R||intersection.z>2*R){
    //     return false;
    // }


    // if(intersection.x>2*R||intersection.y>2*R||intersection.z>2*R){
    //     return false;
    // }
    return true;


}

Ray Wall::getNormal(glm::vec3 intersection){
    return Ray(intersection,normal);
}
