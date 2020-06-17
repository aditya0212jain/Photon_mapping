#include "scene.h"
#include "light.h"
#include<omp.h>

float getDistanceBetweenSpheres(float r1,float r2,float angle2){
    float b = -2 * r2 * cos(M_PI*angle2/180);
    float d = sqrt(pow(b,2)-(4*(pow(r2,2)-pow(r1,2))));
    float a1 = (-b + d)/2.0;
    float a2 = (-b - d)/2.0;
    if(a1<0){
        return a2;
    }
    if(a2<0){
        return a1;
    }
    return (a1<a2) ? a1:a2;
}

void fresnelcompute(const glm::vec3 I, const glm::vec3 N, const float ior, float &kr) 
{ 
    float cosi = dot(I,N);
    if(cosi<-1){
        cosi=-1;
    }else if(cosi>1){
        cosi = 1;
    }
    float etai = 1, etat = ior; 
    if (cosi > 0) { std::swap(etai, etat); } 
    // Compute sini using Snell's law
    float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi)); 
    // Total internal reflection
    if (sint >= 1) { 
        kr = 1; 
    } 
    else { 
        float cost = sqrtf(std::max(0.f, 1 - sint * sint)); 
        cosi = fabsf(cosi); 
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost)); 
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost)); 
        kr = (Rs * Rs + Rp * Rp) / 2; 
    } 
} 

Scene::Scene(){}

void Scene::make_scene(){
    // camera.position = glm::vec3(60*R,60*R,60*R);
    place_lights();
    // TODO: update this 
    place_all_objects_2();
}

void Scene::place_objects(){
    scene_objects.push_back(Sphere(R,glm::vec3(0,0,0)));
    Sphere s(R/4,glm::vec3(-R,0,0));
    s.color = ColorRGB(1,0,0);//orange
    s.reflection = 0.60;
    s.refraction = 0.3;
    s.diffuse = 0.3;
    s.specular = 0.70;
    s.id = 1;
    scene_objects.push_back(s);
    scene_spheres.push_back(s);
    scene_objects_ptr.push_back(&scene_spheres[scene_spheres.size()-1]);
    Sphere s1(R/4,glm::vec3(R,0,0));
    s1.color = ColorRGB(1,0,0);//orange
    s1.reflection = 0.60;
    s1.refraction = 0.3;
    s1.diffuse = 0.3;
    s1.specular = 0.70;
    s1.id = 2;
    scene_objects.push_back(s1);
    scene_spheres.push_back(s1);
    scene_objects_ptr.push_back(&scene_spheres[scene_spheres.size()-1]);
    place_snowman();
}

void Scene::place_all_objects(){
    // placing the 12 balls 
    ColorRGB cyan(0,1,1);
    ColorRGB orange(1,0.647,0);
    ColorRGB magenta(1,0,1);
    for(int i=0;i<12;i++){
        float angle = i*30;
        float x = 1.5*R*cos(M_PI*angle/180);
        float z = 1.5*R*sin(M_PI*angle/180);
        // Sphere s(R/4,glm::vec3(x,-2*R+R/4,z));
        float refle = (i%2==0) ? 0.30:0.9;
        float refra = (i%2==0) ? 0.60:0.0;
        ColorRGB coltemp;
        switch (i%3)
        {
        case 0:
            coltemp = cyan;
            break;
        case 1:
            coltemp = orange;
            break;
        case 2:
            coltemp = magenta;
        default:
            break;
        }
        if(i%2==0){
            coltemp=glm::vec3(1,1,1);
        }
        //reflection, refraction, color, diffuse, spec
        scene_objects_ptr.push_back(new Sphere(R/4,glm::vec3(x,-2*R+R/4,z),i%2));
        scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(refle,refra,coltemp,0.25,0.75);
        if(i%2==0){
            scene_objects_ptr[scene_objects_ptr.size()-1]->belong=GLASS;
        }else{
            scene_objects_ptr[scene_objects_ptr.size()-1]->belong =BALL;
        }
    }

    //placing snowman
    place_snowman();

    //Placing lights
    float x_r[]= {2*R,-2*R};
    for(int i=0;i<2;i++){
        for(int j=0;j<2;j++){
            scene_objects_ptr.push_back(new Cylinder(R/25,R,glm::vec3(x_r[i],-2*R,x_r[j])));
            scene_objects_ptr[scene_objects_ptr.size()-1]->belong= CYLINDER;
        }
    }

    //placing walls

    scene_objects_ptr.push_back(new Wall(glm::vec3(2*R,0,0),glm::vec3(-1,0,0)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(1,0.0,ColorRGB(1,0,0),1.0,0.0);
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = WALL;

    scene_objects_ptr.push_back(new Wall(glm::vec3(-2*R,0,0),glm::vec3(1,0,0)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(1,0.0,ColorRGB(1,0,0),1.0,0.0);
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = WALL;

    scene_objects_ptr.push_back(new Wall(glm::vec3(0,0,-2*R),glm::vec3(0,0,1)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(1,0.0,ColorRGB(1,0,0),0.0,1.0);
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = WALL;

    scene_objects_ptr.push_back(new Wall(glm::vec3(0,0,2*R),glm::vec3(0,0,-1)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(1,0.0,ColorRGB(1,0,0),1.0,0.0);
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = WALL;

    // up
    scene_objects_ptr.push_back(new Wall(glm::vec3(0,2*R,0),glm::vec3(0,-1,0)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(1,0.0,ColorRGB(0,1,0),1.0,0.0);
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = WALL;

    // down
    scene_objects_ptr.push_back(new Wall(glm::vec3(0,-2*R,0),glm::vec3(0,1,0)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(1,0.0,ColorRGB(1,1,1),1.0,0.0);
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = WALL;

}

void Scene::place_all_objects_2(){
    // placing the 12 balls 
    ColorRGB cyan(0,1,1);
    ColorRGB orange(1,0.647,0);
    ColorRGB magenta(1,0,1);
    for(int i=0;i<1;i++){
        float angle = i*30;
        float x = 1.5*R*cos(M_PI*angle/180);
        float z = 1.5*R*sin(M_PI*angle/180);
        // Sphere s(R/4,glm::vec3(x,-2*R+R/4,z));
        float refle = ((i+1)%2==0) ? 0.30:0.9;
        float refra = ((i+1)%2==0) ? 0.60:0.0;
        ColorRGB coltemp;
        switch ((i+1)%3)
        {
        case 0:
            coltemp = cyan;
            break;
        case 1:
            coltemp = orange;
            break;
        case 2:
            coltemp = magenta;
        default:
            break;
        }
        if((i+1)%2==0){
            coltemp=glm::vec3(1,1,1);
        }
        //reflection, refraction, color, diffuse, spec
        scene_objects_ptr.push_back(new Sphere(R/4,glm::vec3(x,-2*R+R/4,z),i%2));
        scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(refle,refra,coltemp,0.25,0.75);
        if((i+1)%2==0){
            scene_objects_ptr[scene_objects_ptr.size()-1]->belong=GLASS;
        }else{
            scene_objects_ptr[scene_objects_ptr.size()-1]->belong =BALL;
        }
    }
    // placing the 12 balls 
    // ColorRGB cyan(0,1,1);
    // ColorRGB orange(1,0.647,0);
    // ColorRGB magenta(1,0,1);
    for(int i=0;i<1;i++){
        float angle = 0;
        float x = 0;
        float z = 0;
        // Sphere s(R/4,glm::vec3(x,-2*R+R/4,z));
        float refle = (i%2==0) ? 0.30:0.9;
        float refra = (i%2==0) ? 0.60:0.0;
        ColorRGB coltemp;
        switch (i%3)
        {
        case 0:
            coltemp = ColorRGB(1,1,1);
            break;
        case 1:
            coltemp = orange;
            break;
        case 2:
            coltemp = magenta;
        default:
            break;
        }
        if(i%2==0){
            coltemp=glm::vec3(1,1,1);
        }
        //reflection, refraction, color, diffuse, spec
        scene_objects_ptr.push_back(new Sphere(R,glm::vec3(x,0,z),i%2));
        scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(refle,refra,coltemp,0.25,0.75);
        if(i%2==0){
            scene_objects_ptr[scene_objects_ptr.size()-1]->belong=GLASS;
            glassy_scene_objects_ptr.push_back(scene_objects_ptr[scene_objects_ptr.size()-1]);
        }else{
            scene_objects_ptr[scene_objects_ptr.size()-1]->belong =BALL;
        }
    }

    //placing snowman
    // place_snowman();

    //Placing lights
    // float x_r[]= {2*R,-2*R};
    // for(int i=0;i<2;i++){
    //     for(int j=0;j<2;j++){
    //         scene_objects_ptr.push_back(new Cylinder(R/25,R,glm::vec3(x_r[i],-2*R,x_r[j])));
    //         scene_objects_ptr[scene_objects_ptr.size()-1]->belong= CYLINDER;
    //     }
    // }

    //placing walls

    scene_objects_ptr.push_back(new Wall(glm::vec3(2*R,R,0),glm::vec3(-1,0,0)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(0.9,0.0,ColorRGB(0,0,1),1,0);
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = WALL;

    scene_objects_ptr.push_back(new Wall(glm::vec3(-2*R,R,0),glm::vec3(1,0,0)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(0.9,0.0,ColorRGB(1,0,0),1,0);
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = WALL;

    scene_objects_ptr.push_back(new Wall(glm::vec3(0,R,-2*R),glm::vec3(0,0,1)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(0.9,0.0,ColorRGB(1,1,1),1,0);
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = WALL;

    // scene_objects_ptr.push_back(new Wall(glm::vec3(0,R,2*R),glm::vec3(0,0,-1)));
    // scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(0.9,0.0,ColorRGB(1,0,0),0.65,0.35);
    // scene_objects_ptr[scene_objects_ptr.size()-1]->belong = WALL;

    // // up
    scene_objects_ptr.push_back(new Wall(glm::vec3(0,2*R,0),glm::vec3(0,-1,0)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(0.9,0.0,ColorRGB(1,1,1),1,0);
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = WALL;

    // // down
    scene_objects_ptr.push_back(new Wall(glm::vec3(0,-2*R,0),glm::vec3(0,1,0)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(0.9,0.0,ColorRGB(1,1,1),1,0);
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = WALL;

}

void Scene::place_snowman(){
    //reflection, refraction, color, diffuse, spec
    scene_objects_ptr.push_back(new Sphere(R,glm::vec3(0,-R,0)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(0.9,0.0,ColorRGB(1,1,1),0.65,0.35);
    scene_objects_ptr[scene_objects_ptr.size()-1]->isSnowman = true;
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = SNOWMAN;
    float d1 = getDistanceBetweenSpheres(R,R/2,60);

    // Sphere s1(R/2,glm::vec3(0,-R+d1,0));
    //reflection, refraction, color, diffuse, spec
    scene_objects_ptr.push_back(new Sphere(R/2,glm::vec3(0,-R+d1,0)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(0.9,0.0,ColorRGB(1,1,1),0.65,0.35);
    scene_objects_ptr[scene_objects_ptr.size()-1]->isSnowman = true;
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = SNOWMAN;
    float d2 = getDistanceBetweenSpheres(R/2,R/4,60);

    Sphere s2(R/4,glm::vec3(0,-R+d1+d2,0));
    //reflection, refraction, color, diffuse, spec
    scene_objects_ptr.push_back(new Sphere(R/4,glm::vec3(0,-R+d1+d2,0)));
    scene_objects_ptr[scene_objects_ptr.size()-1]->set_properties(0.9,0.0,ColorRGB(1,1,1),0.65,0.35);
    scene_objects_ptr[scene_objects_ptr.size()-1]->isSnowman = true;
    scene_objects_ptr[scene_objects_ptr.size()-1]->belong = SNOWMAN;
}

void Scene::place_lights(){
    // float x[] = {2*R,-2*R};
    // float y[] = {2*R,(float)2*R/(float)3.0,(float)(-2*R)/(float)3.0,-2*R};
    // // float y[] = {2*R};
    // for(int xt=0;xt<2;xt++){
    //     for(int yt=0;yt<4;yt++){
    //         for(int zt=0;zt<2;zt++){
    //             light_objects.push_back(LightPoint(glm::vec3(x[xt],-2*R+(yt*(R/3.0)),x[zt])));
    //             // light_objects.push_back(LightPoint(glm::vec3(x[xt],(yt*(R/3.0)),x[zt])));
    //             // lights.push_back(&light_objects[light_objects.size()-1]);
    //         }
    //     }
    // }

    // float z[] = {R,-R};
    // float y[] = {2*R,(float)2*R/(float)3.0,(float)(-2*R)/(float)3.0,-2*R};
    // float y[] = {2*R};
    // for(int xt=0;xt<4;xt++){
    //     for(int yt=0;yt<1;yt++){
    //         for(int zt=0;zt<2;zt++){
    //             light_objects.push_back(LightPoint(glm::vec3(R+xt/3,2*R,z[zt])));
    //             light_objects.push_back(LightPoint(glm::vec3(x[xt],(yt*(R/3.0)),x[zt])));
    //             lights.push_back(&light_objects[light_objects.size()-1]);
    //         }
    //     }
    // }
    light_objects.push_back(LightPoint(glm::vec3(0,2*R,-R)));
    // light_objects.push_back(LightPoint(glm::vec3(0,-2*R,R)));

}

void Scene::trace_prep(){
    camera.position = glm::vec3 (0,0,14*R);
    glm::vec3 dir = glm::vec3(0.0,0.0,0.0) - camera.position;
    dir = normalize(dir);
    glm::vec3 ref = camera.position + dir;
}

/// Function for creating the caustic and global photon maps

void Scene::trace_caustic_photon(Ray r,int depth,int& n_photon,int glass,glm::vec3 color){
    if (depth<=0){
        return ;
    }

    Object* nearest_object=nullptr;
    float minD = INFINITY;
    float t;
    
    Ray ray = r;
    // Finding intersection
    for(int i=0;i<scene_objects_ptr.size();i++){
        if(scene_objects_ptr[i]->intersect(ray,t)){
            if(t<minD){
                minD = t;
                nearest_object = scene_objects_ptr[i];
            }
        }
    }

    //if no intersection
    if(nearest_object==nullptr){
        return ;
    }

    glm::vec3 intersectionPoint = ray.origin + ray.direction*minD;
    if(nearest_object->belong == GLASS || glass==1)
    {
    glass = 1;
    // std::cout<< depth << " "<< n_photon << " " << nearest_object->type  <<"\n";
    
    //     KDTreeNode node;
    //     node.p.dir = -r.direction;
    //     node.p.pos = intersectionPoint;
    //     //TODO: check the spectral power 
    //     node.p.spectral = glm::vec3(1,1,1);
    //     caustic_photon_map.insert(node);
    //     n_photon++;
    //     // trace_caustic_photon(reflection_ray,depth-1,n_photon,glass);
    // std::cout << intersectionPoint.x<< " "<< intersectionPoint.y << " "<< intersectionPoint.z<<"\n";
        
        }
        // return;
    // std::cout << " "<< intersectionPoint.y<<"\n";
    Ray normal = nearest_object->getNormal(intersectionPoint);
    float bias = 1e-3;
    
    bool inside = false;
    if (dot(normal.direction,ray.direction)>0){
        inside = true;
        normal.direction = -normal.direction;
    }
    intersectionPoint = intersectionPoint + (normal.direction)*bias;
    float r1 = float(rand())/float((RAND_MAX));
    // Russian roullete
    if(r1<nearest_object->reflection){
        // photon is reflected
        return;
        glm::vec3 reflection_ray_dir = ray.direction - (normal.direction * (2* dot(normal.direction,ray.direction)));
        reflection_ray_dir = normalize(reflection_ray_dir);
        Ray reflection_ray;
        reflection_ray.direction = reflection_ray_dir;
        reflection_ray.origin = intersectionPoint;
        // if(glass!=1)
        // return;
        // KDTreeNode node;
        // node.p.dir = -r.direction;
        // node.p.pos = intersectionPoint;
        // //TODO: check the spectral power 
        // node.p.spectral = glm::vec3(1,1,1);
        // caustic_photon_map.insert(node);
        // n_photon++;
        trace_caustic_photon(reflection_ray,depth-1,n_photon,glass,color/(nearest_object->reflection));
    }else if (r1<(nearest_object->reflection+nearest_object->refraction)){
        // photon is refracted
        bias = 2*bias;
        intersectionPoint = intersectionPoint -(normal.direction)*bias;
        // std::cout <<" "<<depth<<" ***************************************\n";
        float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
        float cosi = -dot(ray.direction,normal.direction);
        float k = 1 - eta * eta * (1 - cosi * cosi);
        glm::vec3 refraction_dir = ray.direction * eta + normal.direction * (float) (eta *  cosi - sqrt(k));
        refraction_dir = normalize(refraction_dir);
        Ray refraction_ray;
        refraction_ray.direction = refraction_dir;
        refraction_ray.origin = intersectionPoint;
        // if(glass!=1)
        // return;
        // KDTreeNode node;
        // node.p.dir = -r.direction;
        // node.p.pos = intersectionPoint;
        // //TODO: check the spectral power 
        // node.p.spectral = glm::vec3(1,1,1);
        // caustic_photon_map.insert(node);
        // n_photon++;
        trace_caustic_photon(refraction_ray,depth-1,n_photon,glass,color/(nearest_object->refraction));
    }else{
        // photon is absorbed
        if(glass!=1 || nearest_object->type!=WALL)
        return;
        KDTreeNode node;
        node.p.dir = -r.direction;
        node.p.pos = intersectionPoint;
        //TODO: check the spectral power 
        float inv = (float)2/(float)N_PHOTONS_CAUSTIC;
        float inv1 = (float)1/(3.14*CAUSTIC_PHOTON_RADIUS*CAUSTIC_PHOTON_RADIUS);
        node.p.spectral = color*inv/(float)(1 - nearest_object->reflection - nearest_object->refraction);
        caustic_photon_map.insert(node);
        n_photon++;
        // std::cout<<"photon is added "<<inv <<" "<<inv1<< "\n";
    }

    return ;

}

void Scene::trace_photon(Ray r,int depth,int& n_photon,glm::vec3 color){
    if (depth<=0){
        return ;
    }
    Object* nearest_object=nullptr;
    float minD = INFINITY;
    float t;
    
    Ray ray = r;
    // Finding intersection
    for(int i=0;i<scene_objects_ptr.size();i++){
        if(scene_objects_ptr[i]->intersect(ray,t)){
            if(t<minD){
                minD = t;
                nearest_object = scene_objects_ptr[i];
            }
        }
    }

    // std::cout<<"trace_photon called\n";

    //if no intersection
    if(nearest_object==nullptr){
        return ;
    }
    // std::cout<<"intersection is not null\n";
    float bias = 1e-3;
    glm::vec3 intersectionPoint = ray.origin + ray.direction*minD;
    Ray normal = nearest_object->getNormal(intersectionPoint);

    bool inside = false;
    if (dot(normal.direction,ray.direction)>0){
        inside = true;
        normal.direction = -normal.direction;
    }
    intersectionPoint = intersectionPoint + (normal.direction)*bias;
    float r1 = float(rand())/float((RAND_MAX));
    // Russian roullete
    if(r1<nearest_object->reflection){
        // photon is reflected
        glm::vec3 reflection_ray_dir = ray.direction - (normal.direction * (2* dot(normal.direction,ray.direction)));
        reflection_ray_dir = normalize(reflection_ray_dir);
        Ray reflection_ray;
        reflection_ray.direction = reflection_ray_dir;
        reflection_ray.origin = intersectionPoint;
        // KDTreeNode node;
        // node.p.dir = -r.direction;
        // node.p.pos = intersectionPoint;
        // //TODO: check the spectral power 
        // node.p.spectral = glm::vec3(nearest_object->color.x,nearest_object->color.y,nearest_object->color.z);
        // photon_map.insert(node);
        // n_photon++;
        trace_photon(reflection_ray,depth-1,n_photon,(color)/nearest_object->reflection);
    }else if (r1<(nearest_object->reflection+nearest_object->refraction)){
        return;
        // photon is refracted
        bias = -2*bias;
        intersectionPoint = intersectionPoint + (normal.direction)*bias;
        float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
        float cosi = -dot(ray.direction,normal.direction);
        float k = 1 - eta * eta * (1 - cosi * cosi);
        glm::vec3 refraction_dir = ray.direction * eta + normal.direction * (float) (eta *  cosi - sqrt(k));
        refraction_dir = normalize(refraction_dir);
        Ray refraction_ray;
        refraction_ray.direction = refraction_dir;
        refraction_ray.origin = intersectionPoint;
        // KDTreeNode node;
        // node.p.dir = -r.direction;
        // node.p.pos = intersectionPoint;
        // //TODO: check the spectral power 
        // node.p.spectral = glm::vec3(nearest_object->color.x,nearest_object->color.y,nearest_object->color.z);
        // photon_map.insert(node);
        // n_photon++;
        trace_photon(refraction_ray,depth-1,n_photon,color/(nearest_object->refraction));
    }else{
        // photon is absorbed
        KDTreeNode node;
        node.p.dir = -r.direction;
        node.p.pos = intersectionPoint;
        //TODO: check the spectral power 
        float inv = (float)2/(float)N_PHOTONS_GLOBAL;
        float inv1 = (float)1/(3.14*PHOTON_RADIUS*PHOTON_RADIUS);
        node.p.spectral = color*(glm::dot(normal.direction,node.p.dir))*inv/(float)(1 - nearest_object->reflection - nearest_object->refraction);
        // node.p.spectral = (glm::vec3(1,1,1))*inv*inv1*(float)10.0*inv2*(glm::dot(normal.direction,node.p.dir))*((float)1/(float)(1-(nearest_object->reflection+nearest_object->refraction)));
        // node.p.spectral = glm::vec3(nearest_object->color.x,nearest_object->color.y,nearest_object->color.z);
        photon_map.insert(node);
        n_photon++;
        // std::cout<<"photon is added "<<inv2.x<<"\n";
    }

    return ;

}

void Scene::compute_photon_map(){
    // Adding for PhotonMapping
    // this function will update the photonmap 
    // should be called everytime before tracing rays
    
    // Below is the pseudocode
    // use the trace_photon function for tracing
    // trace_photon automatically updates the photon_map
    // make sure to empty the photon_map each time

    // Generates Global Photon Map
    #pragma omp parallel
    {
        int num_threads = omp_get_max_threads();
        std::cout<<"num global photon map threads: "<<num_threads<<"\n";
        int n_photon = 0;
        float power = 15.0;
        // std::cout<<"lights: "<<light_objects.size()<<"\n";
        for(int i=0;i<light_objects.size();i++){

            while(n_photon<(i+1)*((N_PHOTONS_GLOBAL/light_objects.size())/num_threads)){

                glm::vec3 d(-1.0+(float(rand())/float((RAND_MAX)))*(float)2,-1.0+(float(rand())/float((RAND_MAX)))*(float)2,-1.0+(float(rand())/float((RAND_MAX)))*(float)2);
                Ray r;
                // std::cout<<"d: "<<d.x<<" "<<d.y<<" "<<d.z<<"\n";
                r.origin = light_objects[i].origin;
                r.direction = normalize(d);
                trace_photon(r,TRACE_DEPTH_PHOTON,n_photon,glm::vec3(1.0,1.0,1.0)*power);
                // break;
                // std::cout<<"photons: "<<n_photon<<"\n";
            }
        }
    }

    photon_map.optimize();


}

void Scene::compute_caustic_photon_map(){
    // Adding for PhotonMapping
    // this function will update the photonmap 
    // should be called everytime before tracing rays
    
    // Below is the pseudocode
    // use the trace_photon function for tracing
    // trace_photon automatically updates the photon_map
    // make sure to empty the photon_map each time

    // Generates Global Photon Map
    if(glassy_scene_objects_ptr.size()==0)
        return;

    #pragma omp parallel
    {
        int num_threads = omp_get_max_threads();
        std::cout<<"num caustic photon map threads: "<<num_threads<<"\n";

    
        int n_photon = 0;
        // std::cout<<"lights: "<<light_objects.size()<<"\n";
        for(int i=0;i<light_objects.size();i++){

            while(n_photon<(N_PHOTONS_CAUSTIC/light_objects.size())/num_threads){
                int randomx = ( std::rand() % ( glassy_scene_objects_ptr.size()));
                Object* obj = glassy_scene_objects_ptr[randomx];
                
                
                // Object* obj = glassy_scene_objects_ptr[randomx];

                // d = obj->b_box;



                Ray r;
                
                r.origin = light_objects[i].origin;
                Ray c = glassy_scene_objects_ptr[randomx]->b_box();

                r.direction = normalize(c.direction-r.origin);


                // if(c.direction.y>2 && c.direction.y<3)                
                // std::cout <<" "<<r.direction.y<<" "<<"\n";
                // r.direction = glm::vec3(obj->b_box);
                trace_caustic_photon(r,TRACE_DEPTH_CAUSTIC_PHOTON,n_photon,0,glm::vec3(1.0,1.0,1.0));
                // break;
                // std::cout<<"photons: "<<n_photon<<"\n";
            }
        }

    }

    caustic_photon_map.optimize();


}

ColorRGB Scene::trace(int x,int y,int width,int height){
    
    glm::vec3 dir = glm::vec3(0.0,0.0,0.0) - camera.position;
    dir = normalize(dir);
    glm::vec3 ref = camera.position + dir;
    //First need to generate ray
    // int width = 640;
    // int height = 480;
    float fov = 30;
    float tanV = tan(M_PI*(fov/2)/180.0);
    float wby2 = width/2.0;
    float hby2 = height/2.0;
    float xt = (x - wby2)*tanV/wby2;
    float yt = (y - hby2)*tanV/hby2;
    glm::vec3 screenPoint(xt+ref.x,yt+ref.y,ref.z);

    // Distributed Ray Tracing
    // Anti Aliasing
    int n = 5;
    float x1 = x+1;
    float y1 = y+1;
    float xt1 = (x1 - wby2)*tanV/wby2;
    float yt1 = (y1 - hby2)*tanV/hby2;
    glm::vec3 screenPoint2(xt1+ref.x,yt1+ref.y,ref.z);
    glm::vec3 diff = screenPoint2 - screenPoint;


    if(anti_aliasing==true){
        ColorRGB traceColor(0,0,0);

        for(int i=0;i<n;i++){
            float rx = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float ry = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            glm::vec3 pointTemp = screenPoint +glm::vec3(rx*diff.x,ry*diff.y,0);
            if(i==0){
                pointTemp = screenPoint;
            }
            glm::vec3 ray_dir = pointTemp - camera.position;
            ray_dir = normalize(ray_dir);
            Ray ray(camera.position, ray_dir);
            glm::vec3 point2 = ray.origin+ray.direction;
            ray.origin = rotate_point_yconstant(ray.origin,angle_y);
            ray.origin = rotate_point_xconstant(ray.origin,angle_x);
            point2 = rotate_point_yconstant(point2,angle_y);
            point2 = rotate_point_xconstant(point2,angle_x);
            ray.direction = point2 - ray.origin;
            ray.direction = normalize(ray.direction);
            // std::cout<<"x,y,z: "<<ray.direction.x<<" "<<ray.direction.y<<" "<<ray.direction.z<<"\n";
            // traceColor += trace_global_illumination(ray,TRACE_DEPTH,nullptr);
            // traceColor += indirect_illumination(ray);
        }

        traceColor = traceColor*(float)(1.0/((float)n));
        return traceColor;
    }



    glm::vec3 ray_dir = screenPoint - camera.position;
    ray_dir = normalize(ray_dir);
    Ray ray(camera.position, ray_dir);
    glm::vec3 point2 = ray.origin+ray.direction;
    ray.direction = point2 - ray.origin;
    ray.direction = normalize(ray.direction);

    // std::cout<<"x,y,z: "<<ray.direction.x<<" "<<ray.direction.y<<" "<<ray.direction.z<<"\n";
    // return trace_global_illumination(ray,TRACE_DEPTH,nullptr);
    // return indirect_illumination(ray);
    return trace_ray(ray,TRACE_DEPTH,nullptr);
    // return trace_ray(ray,TRACE_DEPTH,nullptr);
}

ColorRGB Scene::trace_ray(Ray ray,int depth,Object* exclude){

    if (depth<=0){
        return ColorRGB(1,1,1);
    }

    Object* nearest_object=nullptr;
    float minD = INFINITY;
    float t;
    
    // std::cout<<scene_objects.size()<<std::endl;
    for(int i=0;i<scene_objects_ptr.size();i++){
        if(scene_objects_ptr[i]->intersect(ray,t)){
            if(t<minD){
                minD = t;
                nearest_object = scene_objects_ptr[i];
            }
        }
    }

    //if no intersection
    if(nearest_object==nullptr){
        return ColorRGB(0,0,0);
    }else if(nearest_object->belong==LIGHT){
        return nearest_object->emissionColor;
    }
    
    glm::vec3 intersectionPoint = ray.origin + ray.direction*(minD);
    Ray normal = nearest_object->getNormal(intersectionPoint);

    bool inside = false;
    if (dot(normal.direction,ray.direction)>0){
        inside = true;
        normal.direction = -normal.direction;
    }

    ColorRGB caustic_component = caustic_illumination(intersectionPoint,nearest_object);
    ColorRGB indirect_component = indirect_illumination(intersectionPoint,nearest_object);
    ColorRGB directlight_component = direct_illumination(nearest_object,normal,intersectionPoint,ray);
    // ColorRGB montecarlotrace_component = montecarlotrace_illumination(nearest_object,normal,intersectionPoint,ray,depth);

    ColorRGB total;
    // total = caustic_component +  directlight_component + montecarlotrace_component;
    total = indirect_component + caustic_component + directlight_component;
    total = total * nearest_object->color;
    return total;
    // if(nearest_object->belong==SNOWMAN){
    //     ColorRGB I_shade = shade(nearest_object,normal,intersectionPoint,ray);
    //     return I_shade;
    // }else if(nearest_object->belong==GLASS){
    //     glm::vec3 reflection_ray = ray.direction - (normal.direction * (dot(normal.direction,ray.direction)*2));
    //     reflection_ray = normalize(reflection_ray);
    //     float bias = 1e-2;

    //     // float facingratio = -ray.direction.dot(normal.direction);
    //     // float fresnel = 1*(0.1) + (1-0.1)*pow(1-facingratio,3);
    //     float fresnel;
    //     fresnelcompute(ray.direction,normal.direction,1.5,fresnel);
    //     ColorRGB I_reflection = trace_ray(Ray(intersectionPoint+normal.direction*bias,reflection_ray),depth -1,nearest_object);

    //     float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
    //     float cosi = dot(-normal.direction,ray.direction);
    //     float k = 1 - eta * eta * (1 - cosi * cosi);
    //     glm::vec3 refraction_dir = ray.direction * eta + normal.direction * (float)(eta *  cosi - sqrt(k));
    //     refraction_dir = normalize(refraction_dir);
    //     ColorRGB I_refraction;
    //     if(fresnel>1){
    //         I_refraction=glm::vec3(0);
    //     }else{
    //         I_refraction = trace_ray(Ray(intersectionPoint-normal.direction*bias,refraction_dir),depth -1,nearest_object);
    //     }
        
    //     ColorRGB fresnelcolor = (I_reflection*(fresnel/nearest_object->reflection) + I_refraction*((1-fresnel)/nearest_object->refraction));
    //     return fresnelcolor;

    // }else if(nearest_object->belong==CYLINDER){
    //     return nearest_object->emissionColor;
    // }else if(nearest_object->belong==WALL){

    //     // TODO: Check the following
    //     ColorRGB I_shade = shade(nearest_object,normal,intersectionPoint,ray);
    //     return I_shade;

    //     // glm::vec3 reflection_ray = ray.direction - (normal.direction * (dot(normal.direction,ray.direction)*2));
    //     // reflection_ray = normalize(reflection_ray);
    //     // float bias = 1e-2;

    //     // float facingratio = dot(-ray.direction,normal.direction);
    //     // float fresnel = 1*(0.1) + (1-0.1)*pow(1-facingratio,3);
    //     // ColorRGB I_reflection = trace_ray(Ray(intersectionPoint+normal.direction*bias,reflection_ray),depth -1,nearest_object);

    //     // ColorRGB fresnelcolor = (I_reflection*fresnel*nearest_object->reflection);
    //     // ColorRGB I_shade = shade(nearest_object,normal,intersectionPoint,ray);

    //     // return fresnelcolor;
    // }else if(nearest_object->belong==BALL){
    //     ColorRGB I_shade = shade(nearest_object,normal,intersectionPoint,ray);
    //     glm::vec3 reflection_ray = ray.direction - (normal.direction * (dot(normal.direction,ray.direction)*2));
    //     reflection_ray = normalize(reflection_ray);
    //     float bias = 1e-2;

    //     float facingratio = dot(-ray.direction,normal.direction);
    //     float fresnel = 1*(0.1) + (1-0.1)*pow(1-facingratio,3);
    //     ColorRGB I_reflection = trace_ray(Ray(intersectionPoint+normal.direction*bias,reflection_ray),depth -1,nearest_object);

    //     ColorRGB ballColor = I_reflection*(nearest_object->reflection)*fresnel*nearest_object->color;

    //     return ballColor+I_shade;
    // }
    
}

// Below functions are for computing the integral components

ColorRGB Scene::indirect_illumination(glm::vec3 intersectionPoint,Object* nearest_object){
    // this function is the tracer used after creating 
    // the photon map ( in place of the old trace_ray)
    
    // uses global_photon_map


    // Steps:
    // 1) get the intersection object 
    // 2) get the closest photons from photonmap
    // 3) add the contribution of each photon 
    // Return the contribution

    KDTreeNode intersection_node;
    intersection_node.p.pos = intersectionPoint;
    
    std::vector<KDTreeNode> contributing_photons;
    photon_map.find_within_range(intersection_node,PHOTON_RADIUS,std::back_insert_iterator<std::vector<KDTreeNode> >(contributing_photons));

    ColorRGB total(0,0,0);
    // std::cout<<"n: "<<contributing_photons.size()<<"\n";
    float brdf = nearest_object->reflection*nearest_object->diffuse;
    float alpha = 0.918;
    float beta = 1.953;
    float rsquare = (float)PHOTON_RADIUS*PHOTON_RADIUS;
    float e1 = glm::exp(-beta);

    for(int i=0;i<contributing_photons.size();i++){
        //TODO: check the relation, needs to be updated *nearest_object->diffuse*nearest_object->reflection
        float distanceSquare = glm::pow(glm::distance(contributing_photons[i].p.pos,intersectionPoint),2);
        float e2 = glm::exp(-beta*distanceSquare/(2*rsquare));
        float filter = alpha*(1 - ((1 - e2)/(1 - e1)));

        total += contributing_photons[i].p.spectral*brdf*filter;
    }
    total = total/(float)(M_PI*PHOTON_RADIUS*PHOTON_RADIUS);

    return total;

}

ColorRGB Scene::caustic_illumination(glm::vec3 intersectionPoint, Object* nearest_object){

    KDTreeNode intersection_node;
    intersection_node.p.pos = intersectionPoint;

    std::vector<KDTreeNode> contributing_photons;
    caustic_photon_map.find_within_range(intersection_node,CAUSTIC_PHOTON_RADIUS,std::back_insert_iterator<std::vector<KDTreeNode> >(contributing_photons));

    ColorRGB total(0,0,0);
    // std::cout<<"n: "<<contributing_photons.size()<<"\n";
    float brdf = nearest_object->reflection*nearest_object->diffuse;
    float alpha = 0.918;
    float beta = 1.953;
    float rsquare = (float)CAUSTIC_PHOTON_RADIUS*CAUSTIC_PHOTON_RADIUS;
    float e1 = glm::exp(-beta);

    for(int i=0;i<contributing_photons.size();i++){
        //TODO: check the relation, needs to be updated *nearest_object->diffuse*nearest_object->reflection
        float distanceSquare = glm::pow(glm::distance(contributing_photons[i].p.pos,intersectionPoint),2);
        float e2 = glm::exp(-beta*distanceSquare/(2*rsquare));
        float filter = alpha*(1 - ((1 - e2)/(1 - e1)));

        total += contributing_photons[i].p.spectral*brdf*filter;
        
    }
    total = total/(float)(M_PI*CAUSTIC_PHOTON_RADIUS*CAUSTIC_PHOTON_RADIUS);

    return total;

}

ColorRGB Scene::direct_illumination(Object* obj,Ray normal,glm::vec3 intersection,Ray incident){
    ColorRGB iShade(0,0,0);
    // ambient color, will remove later when indirect illumination is added
    iShade = ColorRGB(0.0,0.0,0.0);
    for(int i=0;i<light_objects.size();i++){
        float attenuation = light_objects[i].DistanceDrop(intersection);
        //// shadow attenuation required
        float shadow_att = light_objects[i].ShadowDrop(this,intersection);
        ColorRGB light_color;
        Ray l = light_objects[i].getL(intersection);
        glm::vec3 h = incident.direction + l.direction;
        h = normalize(h);
        float temp = dot(h,normal.direction);
        temp = pow(temp,5);
        float combined = obj->diffuse*(dot(l.direction,normal.direction)) +
                            obj->specular*(temp);

        light_color = light_objects[i].color*combined;
        iShade += light_color*attenuation*shadow_att*std::max(0.0f,dot(l.direction,normal.direction));

    }
    // return iShade;
    return iShade;
}

ColorRGB Scene::montecarlotrace_illumination(Object* nearest_object,Ray normal,glm::vec3 intersectionPoint,Ray ray,int depth){
    bool inside = false;
    if (dot(normal.direction,ray.direction)>0){
        inside = true;
        normal.direction = -normal.direction;
    }

    if(nearest_object->belong==GLASS){
        glm::vec3 reflection_ray = ray.direction - (normal.direction * (dot(normal.direction,ray.direction)*2));
        reflection_ray = normalize(reflection_ray);
        float bias = 1e-4;

        // float facingratio = -ray.direction.dot(normal.direction);
        // float fresnel = 1*(0.1) + (1-0.1)*pow(1-facingratio,3);
        float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
        float cosi = dot(-normal.direction,ray.direction);
        float k = 1 - eta * eta * (1 - cosi * cosi);
        glm::vec3 refraction_dir = ray.direction * eta + normal.direction * (float)(eta *  cosi - sqrt(k));
        refraction_dir = normalize(refraction_dir);

        
        float fresnel;
        fresnelcompute(ray.direction,normal.direction,1.5,fresnel);

        ColorRGB I_refraction;
        if(fresnel>1){
            I_refraction=glm::vec3(0);
        }else{
            I_refraction = trace_ray(Ray(intersectionPoint-normal.direction*bias,refraction_dir),depth -1,nearest_object);
        }

        ColorRGB I_reflection = trace_ray(Ray(intersectionPoint+normal.direction*bias,reflection_ray),depth -1,nearest_object);
        
        // ColorRGB fresnelcolor = (I_reflection*(fresnel/nearest_object->reflection) + I_refraction*((1-fresnel)/nearest_object->refraction));
        ColorRGB ans = I_reflection*nearest_object->reflection*nearest_object->specular + I_refraction*nearest_object->refraction;
        return ans;
        // return fresnelcolor;

    }else if(nearest_object->belong==CYLINDER){
        return nearest_object->emissionColor;
    }else if(nearest_object->belong==WALL){

        // TODO: Check the following
        return ColorRGB(0,0,0);
        
    }else if(nearest_object->belong==BALL){
        glm::vec3 reflection_ray = ray.direction - (normal.direction * (dot(normal.direction,ray.direction)*2));
        reflection_ray = normalize(reflection_ray);
        float bias = 1e-4;

        ColorRGB I_reflection = trace_ray(Ray(intersectionPoint+normal.direction*bias,reflection_ray),depth -1,nearest_object);
        
        // ColorRGB fresnelcolor = (I_reflection*(fresnel/nearest_object->reflection) + I_refraction*((1-fresnel)/nearest_object->refraction));
        ColorRGB ans = I_reflection*nearest_object->reflection*nearest_object->specular;// + I_refraction*nearest_object->refraction;
        return ans;
    }

    return ColorRGB(0,0,0);
}

// Below functions are Not in use
ColorRGB Scene::shade(Object* obj,Ray normal,glm::vec3 intersection,Ray incident){
    ColorRGB iShade(0,0,0);
    iShade = ColorRGB(0.1,0.1,0.1);
    for(int i=0;i<light_objects.size();i++){
        float attenuation = light_objects[i].DistanceDrop(intersection);
        //// shadow attenuation required
        float shadow_att = light_objects[i].ShadowDrop(this,intersection);
        ColorRGB light_color;
        Ray l = light_objects[i].getL(intersection);
        glm::vec3 h = incident.direction + l.direction;
        h = normalize(h);
        float temp = dot(h,normal.direction);
        temp = pow(temp,5);
        // temp *= temp;
        // t
        float combined = obj->diffuse*(dot(l.direction,normal.direction)) +
                            obj->specular*(temp);

        light_color = light_objects[i].color*combined;
        iShade += light_color*attenuation*shadow_att*std::max(0.0f,dot(l.direction,normal.direction));

    }
    // return iShade;
    return iShade*obj->color;
}

glm::vec3 Scene::rotate_point_yconstant(glm::vec3 point,float angle){
    float xnew = point.x * cos(M_PI*angle/180.0) - point.z * sin(M_PI*angle/180.0);
    float znew = point.x * sin(M_PI*angle/180.0) + point.z * cos(M_PI*angle/180.0);
    point.x = xnew;
    point.z = znew;
    return point;
}

glm::vec3 Scene::rotate_point_xconstant(glm::vec3 point,float angle){
    float ynew = point.y * cos(M_PI*angle/180.0) - point.z * sin(M_PI*angle/180.0);
    float znew = point.y * sin(M_PI*angle/180.0) + point.z * cos(M_PI*angle/180.0);
    point.y = ynew;
    point.z = znew;
    return point;
}

void Scene::update_camera(int type,int forward){
    
    if(type==0){
        // horizontally angle_y
        if(forward==1){
            angle_y +=15;
        }else{
            angle_y -=15;
        }
        return;
    }else{
        if(forward==1){
            angle_x -=15;
        }else{
            angle_x +=15;
        }
    }
    return;
    // float xnew = camera.position.x * cos(M_PI*angle/180) - camera.position.z * sin(M_PI*angle/100);
    // float znew = camera.position.x * sin(M_PI*angle/180) + camera.position.z * cos(M_PI*angle/180);
    // camera.position.x = xnew;
    // camera.position.z = znew;
}

void Scene::update_spheres_position(int type){
    if(type==0){
        angle_sphere=-15;
    }else{
        angle_sphere=15;
    }
    for(int i=0;i<scene_objects_ptr.size();i++){
        if(scene_objects_ptr[i]->type==1){
            scene_objects_ptr[i]->update_position(angle_sphere);
        }
    }
}

void Scene::update_light_positions(int type){
    if(type==0){
        angle_lights = -15;
    }else
    {
        angle_lights = 15;
    } 
    std::cout<<"angle_lights: "<<angle_lights<<"\n";
    for(int i=0;i<scene_objects_ptr.size();i++){
        if(scene_objects_ptr[i]->type==2){
            scene_objects_ptr[i]->update_position(angle_lights);
        }
    }
    for(int i=0;i<light_objects.size();i++){
        light_objects[i].update_position(angle_lights);
    }
}
