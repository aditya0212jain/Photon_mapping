#ifndef HELPER_H
#define HELPER_H
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "./kdtree++/kdtree.hpp"

#define SNOWMAN 1
#define GLASS 2
#define WALL 3
#define LIGHT 4
#define CYLINDER 5
#define BALL 6

template<typename T>
class Vec3{
    public:
    T x, y, z;
    Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
    Vec3(T xx) : x(xx), y(xx), z(xx) {}
    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
    Vec3& normalize()
    {
        T nor2 = length2();
        if (nor2 > 0) {
            T invNor = 1 / sqrt(nor2);
            x *= invNor, y *= invNor, z *= invNor;
        }
        return *this;
    }
    Vec3<T> operator * (const T &f) const { return Vec3<T>(x * f, y * f, z * f); }
    Vec3<T> operator * (const Vec3<T> &v) const { return Vec3<T>(x * v.x, y * v.y, z * v.z); }
    T dot(const Vec3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3<T> operator - (const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
    Vec3<T> operator + (const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
    Vec3<T>& operator += (const Vec3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
    Vec3<T>& operator *= (const Vec3<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
    Vec3<T> operator - () const { return Vec3<T>(-x, -y, -z); }
    T length2() const { return x * x + y * y + z * z; }
    T length() const { return sqrt(length2()); }
    // friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v)
    // {
    //     os << "[" << v.x << " " << v.y << " " << v.z << "]";
    //     return os;
    // }
};

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
};

class Camera{
    public:
    glm::vec3 position;

    Camera(){}

    Camera(glm::vec3 p){
        position = p;
    }
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

class KDTreeNode
{
    public:
	typedef double value_type;
	Photon p;
	size_t index;

	value_type operator[](size_t n) const
	{
		return p.pos[n];
	}

	double distance( const KDTreeNode &node)
	{
		return glm::length(node.p.pos - p.pos);
	}
};

class Sphere;
class Cylinder;

class Scene{

    public:
    std::vector<Object> scene_objects;
    std::vector<Object *> scene_objects_ptr;
    std::vector<Object *> snow_man;
    std::vector<Sphere> scene_spheres;
    std::vector<Cylinder> scene_cylinders;
    std::vector<Object *> tubelights;
    std::vector<LightPoint> light_objects;
    std::vector<LightPoint *> lights;
    KDTree::KDTree<3, KDTreeNode> photon_map_;
    int toggle=2;
    bool anti_aliasing=false;


   Camera camera;

    Scene();

    void make_scene();

    void place_objects();

    void place_lights();

    void place_all_objects();

    void place_snowman();

    void update_camera(int type,int forward);

    void update_spheres_position(int type);

    void update_light_positions(int type);

    void trace_prep();

    void toggle_aliasing(){
        anti_aliasing = !anti_aliasing;
    }

    ColorRGB trace(int x,int y,int widht,int height);

    ColorRGB trace_ray(Ray ray,int depth,Object* exclude);

    void make_photon_map();

    glm::vec3 trace_photon(Photon p,int depth);

    ColorRGB shade(Object* obj,Ray normal,glm::vec3 intersection,Ray incident);

};

#endif