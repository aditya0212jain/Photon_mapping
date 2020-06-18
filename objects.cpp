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

Ray Sphere::b_box()
{
    float R = radius;
    glm::vec3 dir = glm::vec3(center.x-radius + (float(rand())/float((RAND_MAX)))*2*R,center.y-radius + (float(rand())/float((RAND_MAX)))*2*R,center.z-radius + (float(rand())/float((RAND_MAX)))*2*R);
    Ray r(glm::vec3(0,0,0),dir);
    // if(dir.y>0 && dir.y<1)
    // std::cout << dir.x << " " << dir.y << " " << dir.z << std::endl;
    return r;
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

Ray Cylinder::b_box()
{
    glm::vec3 center = bottom_center;
    float R = radius;
    glm::vec3 dir = glm::vec3(center.x-radius + (float(rand())/float((RAND_MAX)))*2*R,center.y+ (float(rand())/float((RAND_MAX)))*length,center.z-radius + (float(rand())/float((RAND_MAX)))*2*R);
    Ray r(glm::vec3(0,0,0),dir);
    return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////Wall//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    
Wall::Wall(glm::vec3 cl,glm::vec3 n){
    type = 3;
    center = cl;
    normal = n;
}

bool Wall::intersect(Ray r,float& t){
    double directions_dot_prod = dot(r.direction,normal);
    if (directions_dot_prod == 0) {// the plane and ray are parallel
        // t = FLT_MAX; // no intersection, at 'infinity'
        return false;
    }
    t = dot(center - r.origin,normal) / directions_dot_prod;

    if (t < 1e-6) { // the plane is behind the ray
        // t = ;
        return false;
    }
    glm::vec3 intersection = r.origin + r.direction*t;

    // if(intersection.x>2*R||intersection.y>2*R||intersection.z>2*R){
    //     return false;
    // }


    // // if(intersection.x>2*R||intersection.y>2*R||intersection.z>2*R){
    // //     return false;
    // // }
    return (t>=0);


}

Ray Wall::getNormal(glm::vec3 intersection){
    return Ray(intersection,normal);
}

Ray Wall::b_box()
{
    glm::vec3 dir = glm::vec3(float(rand())/float((RAND_MAX)),float(rand())/float((RAND_MAX)),float(rand())/float((RAND_MAX)));
    Ray r(glm::vec3(0,0,0),dir);
    return r;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////TRIANGLE//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Triangle::Triangle(glm::vec3 a0,glm::vec3 a1,glm::vec3 a2,glm::vec3 translate,glm::vec3 scale){
    // float scale = 2;
    // glm::vec3 translate(3,3,3);
    type = 4;
    v0 = a0*scale +translate;
    v1 = a1*scale +translate;
    v2 = a2*scale +translate;
    normal = glm::normalize(glm::cross(v2 - v0,v1 - v0));
    // std::cout<<"triangle\n";
    // std::cout<<v0.x<<" "<<v0.y<<" "<<v0.z<<"\n";
    // std::cout<<v1.x<<" "<<v1.y<<" "<<v1.z<<"\n";
    // std::cout<<v2.x<<" "<<v2.y<<" "<<v2.z<<"\n";
    // std::cout<<normal.x<<" "<<normal.y<<" "<<normal.z<<"\n";
}

Ray Triangle::getNormal(glm::vec3 intersection){
    Ray r;
    r.origin = intersection;
    r.direction = normal;
    return r;
}

Ray Triangle::b_box(){
    //TODO, dummy for now
    Ray r;
    r.origin = glm::vec3(0,0,0);
    r.direction = normal;
    return r;
}

bool Triangle::intersect(Ray r,float& t){
    
    // Indside out method
    // glm::vec3 n = -normal;
    // // n = glm::normalize(n);
    // // r.direction = glm::normalize(r.direction);
    // float eps=0.00001;
    // float d = glm::dot(n,v0);

    // // // check for parallel ray
    // if(abs(glm::dot(n,r.direction))<eps){
    //     return false;
    // }

    // t = (d - glm::dot(n,r.origin))/(glm::dot(n,r.direction));
    // // t = t/(glm::dot(n,r.direction));

    // // // return false if triangle is behind the ray i.e. t<0
    // if(t<0){
    //     return false;
    // }

    // glm::vec3 point = r.origin + r.direction*t;

    // // checking point is inside 

    // if(glm::dot(n,glm::cross(v1 - v0,point - v0))<0){
    //     return false;
    // }

    // if(glm::dot(n,glm::cross(v2 - v1,point - v1))<0){
    //     return false;
    // }

    // if(glm::dot(n,glm::cross(v0 - v2,point - v2))<0){
    //     return false;
    // }

    // // // t = t1;
    // return true;

    // every point inside the triangle can be expressed as (1-u-v)v0 + uv1+ vv2
    // u>0&&u<1&&v>0&&v<1&&(u+v)<1
    // area trianhle = cross product two edges starting vertex/2
    // Moller trumbore method
    float eps=0.00001;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;

    // det(M)
    glm::vec3 left_det_c = glm::cross(r.direction,edge2);
    float det = glm::dot(edge1,left_det_c);

    // when ray is in same plane as triangle
	if (det > -eps && det < eps)
		return false;

	float inv_det = 1/det;
    // calculating u using cramer's rul, det(Mu)/det(M)
    glm::vec3 s = r.origin - v0;
    float u = inv_det*glm::dot(s,left_det_c);

    // calculating v using cramer's rule, det(Mv)/det(M)
    glm::vec3 q = glm::cross(s,edge1);
    float v = inv_det* glm::dot(r.direction,q);

	if (u < 0.0 || u > 1.0){
        return false;
    }
	
	if (v < 0.0 || u + v > 1.0)
		return false;

    // calculating the third dimension t using cramer's rule
	t = inv_det * glm::dot(edge2,q);

	if (t > eps){
        return true;
    }
	else{
        return false;
    }

}

Triangle::Triangle(){
    v0 = glm::vec3(0,0,1);
    v1 = glm::vec3(0,1,0);
    v2 = glm::vec3(1,0,0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////MESH//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh(const char* filename,glm::vec3 translate,glm::vec3 scale){
    // Loading the mesh from filename
      
    type = 5;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normal;
    std::vector<glm::vec3> meshVertices;
    std::vector<glm::vec3> meshNormal;
    std::vector<int> faceIndex;
    std::vector<int> normalIndex;
    // Opening the file
    std::ifstream in(filename, std::ios::in);
    if (!in)
    {
        std::cerr << "Cannot open " << filename << std::endl;
        exit(1);

    }
    // std::cout << vertices.size() << std::endl;
    std::string line;
    int x=0;
    while (std::getline(in, line))
    {

        // std::cout << x << std::endl;
        x+=1;
        //check v for vertices
        if (line.substr(0,2)=="v "){
            

            std::istringstream v(line.substr(2));
            glm::vec3 vert;
            double x,y,z;
            v>>x;v>>y;v>>z;
            vert=glm::vec3(x,y,z);
            vertices.push_back(vert);
            // std::cout << vertices[vertices.size()-1].x<< std::endl;

        }
        //check for texture co-ordinate
        else if(line.substr(0,3)=="vt "){
            std::istringstream v(line.substr(3));
            glm::vec2 tex;
            int U,V;
            v>>U;v>>V;
            tex=glm::vec2(U,V);
            // Since we are not using texture for now
            // texture.push_back(tex);

        }

        else if(line.substr(0,3)=="vn "){
            std::istringstream v(line.substr(3));
            glm::vec3 norm;
            double x,y,z;
            v>>x;v>>y;v>>z;
            norm=glm::vec3(x,y,z);
            normal.push_back(norm);
            
        }
    //check for faces
        else if(line.substr(0,2)=="f "){
            int a,b,c; //to store mesh index
            int A,B,C; //to store texture index
            int a1,b1,c1;
            //std::istringstream v;
        //v.str(line.substr(2));
            const char* chh=line.c_str();
            sscanf (chh, "f %i/%i/%i %i/%i/%i %i/%i/%i",&a,&a1,&A,&b,&b1,&B,&c,&c1,&C); //here it read the line start with f and store the corresponding values in the variables
            // sscanf (chh, "f %i/%i/%i",&a,&b,&c);
            //v>>a;v>>b;v>>c;
            a--;b--;c--;
            // A--;B--;C--;
            //std::cout<<a<<b<<c<<A<<B<<C;
            triangle_list.push_back(new Triangle(vertices[a],vertices[b],vertices[c],translate,scale));
            boxAABB.update(vertices[a]*scale+translate);
            boxAABB.update(vertices[b]*scale+translate);
            boxAABB.update(vertices[c]*scale+translate);
            // faceIndex.push_back(a);normalIndex.push_back(A);
            // faceIndex.push_back(b);normalIndex.push_back(B);
            // faceIndex.push_back(c);normalIndex.push_back(C);
        }

    }
    std::cout<<"bbox:\n";
    std::cout<<boxAABB.max.x<<" "<<boxAABB.max.y<<" "<<boxAABB.max.z<<"\n";
    std::cout<<boxAABB.min.x<<" "<<boxAABB.min.y<<" "<<boxAABB.min.z<<"\n";
    std::cout<<"#triangles: "<<triangle_list.size()<<"\n";

}

// Mesh::Mesh(std::vector<Triangle> tl){
//     triangle_list = tl;
// }

bool Mesh::intersect(Ray r,float&t){

    // Add box check here first 

    // std::cout<<"Mesh intersectio is called\n";
    // Triangle* nearest_object=nullptr;
    float minD = INFINITY;
    // float t;
    bool intersected = false;
    // std::cout<<"triangles: "<<triangle_list.size()<<"\n";

    // if(b.intersect(r,t))
    // Finding intersection
    if(boxAABB.intersect(r)){
        for(int i=0;i<triangle_list.size();i++){
            if(triangle_list[i]->intersect(r,t)){
                intersected = true;
                if(t<minD){
                    minD = t;
                    intersected_triangle = triangle_list[i];
                }
            }
        }
    }

    return intersected;

}

Ray Mesh::getNormal(glm::vec3 intersection){
    // Ray r;
    // r.origin = intersection;
    // r.direction = intersected_triangle->normal;
    // return r;
    return intersected_triangle->getNormal(intersection);
}

Ray Mesh::b_box(){
    // TODO, dummy function as of now
    Ray r;
    r.origin = glm::vec3(0,0,0);
    r.direction = glm::vec3(0,0,0);
    return r;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////Box//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Box::Box(){
    max = glm::vec3(INT_MIN,INT_MIN,INT_MIN);
    min = glm::vec3(INT_MAX,INT_MAX,INT_MAX);
}

void Box::update(glm::vec3 point){
    max.x = point.x > max.x ? point.x : max.x ;
    max.y = point.y > max.y ? point.y : max.y ;
    max.z = point.z > max.z ? point.z : max.z ;
    min.x = point.x < min.x ? point.x : min.x ;
    min.y = point.y < min.y ? point.y : min.y ;
    min.z = point.z < min.z ? point.z : min.z ;
}

void swap(float& t1,float& t2){
    float t3 = t1;
    t1 = t2;
    t2 = t3;
    return;
}

bool Box::intersect(Ray r){
    float t_min = (min.x - r.origin.x) / r.direction.x; 
    float t_max = (max.x - r.origin.x) / r.direction.x; 

    float t_ymin = (min.y - r.origin.y) / r.direction.y; 
    float t_ymax = (max.y - r.origin.y) / r.direction.y; 

    float t_zmin = (min.z - r.origin.z) / r.direction.z; 
    float t_zmax = (max.z - r.origin.z) / r.direction.z; 
 
    if (t_min > t_max) swap(t_min, t_max); 

    if (t_ymin > t_ymax) swap(t_ymin, t_ymax); 

    if (t_zmin > t_zmax) swap(t_zmin, t_zmax); 
 
    // test similar for 2d box
    if ((t_min > t_ymax) || (t_ymin > t_max)) 
        return false; 
 
    if (t_ymax < t_max) 
        t_max = t_ymax; 

    if (t_ymin > t_min) 
        t_min = t_ymin; 
 
    // extending for 3d
    if ((t_min > t_zmax) || (t_zmin > t_max)) 
        return false; 

 
    return true; 
}