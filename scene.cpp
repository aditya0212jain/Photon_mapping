#include "scene.h"
#include "light.h"

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
    place_all_objects();
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
    float x[] = {2*R,-2*R};
    float y[] = {2*R,(float)2*R/(float)3.0,(float)(-2*R)/(float)3.0,-2*R};
    // float y[] = {2*R};
    for(int xt=0;xt<2;xt++){
        for(int yt=0;yt<4;yt++){
            for(int zt=0;zt<2;zt++){
                light_objects.push_back(LightPoint(glm::vec3(x[xt],-2*R+(yt*(R/3.0)),x[zt])));
                // light_objects.push_back(LightPoint(glm::vec3(x[xt],(yt*(R/3.0)),x[zt])));
                // lights.push_back(&light_objects[light_objects.size()-1]);
            }
        }
    }
    // light_objects.push_back(LightPoint(glm::vec3(2*R,2*R,2*R)));
}

void Scene::trace_prep(){
    camera.position = glm::vec3 (0,0,14*R);
    glm::vec3 dir = glm::vec3(0.0,0.0,0.0) - camera.position;
    dir = normalize(dir);
    glm::vec3 ref = camera.position + dir;
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
            traceColor += trace_ray(ray,4,nullptr);
        }

        traceColor = traceColor*(float)(1.0/((float)n));
        return traceColor;
    }



    glm::vec3 ray_dir = screenPoint - camera.position;
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
    return trace_ray(ray,4,nullptr);
}



ColorRGB Scene::trace_ray(Ray ray,int depth,Object* exclude){

    if (depth<=0){
        return ColorRGB(1,1,1);
    }
    //////
    /*
    shade
    reflection
    refraction
    TIR
    */
    Object* nearest_object=nullptr;
    float minD = INFINITY;
    float t;
    
    // std::cout<<scene_objects.size()<<std::endl;
    for(int i=0;i<scene_objects_ptr.size();i++){
        if(scene_objects_ptr[i]->intersect(ray,t)&&!(depth==4&&scene_objects_ptr[i]->type==3)){
            if(t<minD){
                minD = t;
                nearest_object = scene_objects_ptr[i];
            }
        }
    }

    //if no intersection
    if(nearest_object==nullptr){
        return ColorRGB(0,0,0);
    }
    
    glm::vec3 intersectionPoint = ray.origin + ray.direction*minD;
    Ray normal = nearest_object->getNormal(intersectionPoint);

    bool inside = false;
    if (dot(normal.direction,ray.direction)>0){
        inside = true;
        normal.direction = -normal.direction;
    }

    if(nearest_object->belong==SNOWMAN){
        ColorRGB I_shade = shade(nearest_object,normal,intersectionPoint,ray);
        return I_shade;
    }else if(nearest_object->belong==GLASS){
        glm::vec3 reflection_ray = ray.direction - (normal.direction * (dot(normal.direction,ray.direction)*2));
        reflection_ray = normalize(reflection_ray);
        float bias = 1e-2;

        // float facingratio = -ray.direction.dot(normal.direction);
        // float fresnel = 1*(0.1) + (1-0.1)*pow(1-facingratio,3);
        float fresnel;
        fresnelcompute(ray.direction,normal.direction,1.5,fresnel);
        ColorRGB I_reflection = trace_ray(Ray(intersectionPoint+normal.direction*bias,reflection_ray),depth -1,nearest_object);

        float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
        float cosi = dot(-normal.direction,ray.direction);
        float k = 1 - eta * eta * (1 - cosi * cosi);
        glm::vec3 refraction_dir = ray.direction * eta + normal.direction * (float)(eta *  cosi - sqrt(k));
        refraction_dir = normalize(refraction_dir);
        ColorRGB I_refraction;
        if(fresnel>1){
            I_refraction=glm::vec3(0);
        }else{
            I_refraction = trace_ray(Ray(intersectionPoint-normal.direction*bias,refraction_dir),depth -1,nearest_object);
        }
        
        ColorRGB fresnelcolor = (I_reflection*(fresnel/nearest_object->reflection) + I_refraction*((1-fresnel)/nearest_object->refraction));
        return fresnelcolor;

    }else if(nearest_object->belong==CYLINDER){
        return nearest_object->emissionColor;
    }else if(nearest_object->belong==WALL){
        glm::vec3 reflection_ray = ray.direction - (normal.direction * (dot(normal.direction,ray.direction)*2));
        reflection_ray = normalize(reflection_ray);
        float bias = 1e-2;

        float facingratio = dot(-ray.direction,normal.direction);
        float fresnel = 1*(0.1) + (1-0.1)*pow(1-facingratio,3);
        ColorRGB I_reflection = trace_ray(Ray(intersectionPoint+normal.direction*bias,reflection_ray),depth -1,nearest_object);

        ColorRGB fresnelcolor = (I_reflection*fresnel*nearest_object->reflection);
        // ColorRGB I_shade = shade(nearest_object,normal,intersectionPoint,ray);

        return fresnelcolor;
    }else if(nearest_object->belong==BALL){
        ColorRGB I_shade = shade(nearest_object,normal,intersectionPoint,ray);
        glm::vec3 reflection_ray = ray.direction - (normal.direction * (dot(normal.direction,ray.direction)*2));
        reflection_ray = normalize(reflection_ray);
        float bias = 1e-2;

        float facingratio = dot(-ray.direction,normal.direction);
        float fresnel = 1*(0.1) + (1-0.1)*pow(1-facingratio,3);
        ColorRGB I_reflection = trace_ray(Ray(intersectionPoint+normal.direction*bias,reflection_ray),depth -1,nearest_object);

        ColorRGB ballColor = I_reflection*(nearest_object->reflection)*fresnel*nearest_object->color;

        return ballColor+I_shade;
    }
    
}

glm::vec3 Scene::trace_photon(Photon p,int depth){
    if (depth<=0){
        KDTreeNode node;
        node.p = p;
        photon_map_.insert(node);
    }
    //////
    /*
    shade
    reflection
    refraction
    TIR
    */
    Object* nearest_object=nullptr;
    float minD = INFINITY;
    float t;
    
    Ray ray;
    ray.origin = p.pos;
    ray.direction = p.dir;
    for(int i=0;i<scene_objects_ptr.size();i++){
        if(scene_objects_ptr[i]->intersect(ray,t)&&!(depth==4&&scene_objects_ptr[i]->type==3)){
            if(t<minD){
                minD = t;
                nearest_object = scene_objects_ptr[i];
            }
        }
    }

    //if no intersection
    if(nearest_object==nullptr){
        return glm::vec3(0);
    }
    
    glm::vec3 intersectionPoint = ray.origin + ray.direction*minD;
    Ray normal = nearest_object->getNormal(intersectionPoint);

    bool inside = false;
    if (dot(normal.direction,ray.direction)>0){
        inside = true;
        normal.direction = -normal.direction;
    }

    float r = float(rand())/float((RAND_MAX));
    if(r<nearest_object->reflection){
        
        glm::vec3 reflection_ray = ray.direction - (normal.direction * (2* dot(normal.direction,ray.direction)));
        reflection_ray = normalize(reflection_ray);
        p.pos = intersectionPoint;
        p.dir = reflection_ray;
        p.pos += 1e-5;
        trace_photon(p,depth-1);
    }else if (r<nearest_object->reflection+nearest_object->refraction){
        float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
        float cosi = -dot(ray.direction,normal.direction);
        float k = 1 - eta * eta * (1 - cosi * cosi);
        glm::vec3 refraction_dir = ray.direction * eta + normal.direction * (float) (eta *  cosi - sqrt(k));
        refraction_dir = normalize(refraction_dir);
        p.pos = intersectionPoint;
        p.dir = refraction_dir;
        trace_photon(p,depth-1);
    }else{
        KDTreeNode node;
        node.p = p;
        photon_map_.insert(node);
    }

}

ColorRGB Scene::shade(Object* obj,Ray normal,glm::vec3 intersection,Ray incident){
    ColorRGB iShade(0,0,0);
    iShade = ColorRGB(0.3,0.3,0.3)*obj->diffuse;
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