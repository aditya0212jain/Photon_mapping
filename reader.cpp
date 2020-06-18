
#include <vector>
#include <iostream>
#include<tuple>
#include<cmath>
#include <stdio.h>
// glewExperimental = GL_TRUE;
#include "glm/glm.hpp"
#include <math.h>
#include <cstdlib>
#include <stdlib.h>
// #include <iostream>
#include <fstream>
#include <sstream>





int main(int argc, char* args[])
{
  
std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normal;
std::vector<glm::vec3> meshVertices;
std::vector<glm::vec3> meshNormal;
std::vector<int> faceIndex;
std::vector<int> normalIndex;
  // std::cout << vertices.size() << std::endl;
  char *filename = "bunny.obj";
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
      texture.push_back(tex);

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

    //v>>a;v>>b;v>>c;
    a--;b--;c--;
    A--;B--;C--;
    //std::cout<<a<<b<<c<<A<<B<<C;
    faceIndex.push_back(a);normalIndex.push_back(A);
    faceIndex.push_back(b);normalIndex.push_back(B);
    faceIndex.push_back(c);normalIndex.push_back(C);
    // std::cout <<"   "  <<a  <<"   "  <<b <<"   "  <<c << std::endl;
    //  std::cout <<"   "  <<A  <<"   "  <<B <<"   "  <<C << std::endl;
  }

}
// std::cout << vertices.size() <<" " << normal.size()<< std::endl;

  // std::cout << faceIndex.size()<< std::endl;

//the mesh data is finally calculated here
for(int i=0;i<faceIndex.size();i++)
{
  // std::cout << faceIndex[i]<< std::endl;
  // std::cout <<"   "  <<vertices[faceIndex[i]].x  <<"   "  <<vertices[faceIndex[i]].y <<"   "  <<vertices[faceIndex[i]].z << std::endl;

    glm::vec3 meshData ;
    glm::vec3 normData ;
    // std::cout << faceIndex.size()<< std::endl;
    meshData=glm::vec3(vertices[faceIndex[i]].x,vertices[faceIndex[i]].y,vertices[faceIndex[i]].z);
    normData=glm::vec3(normal[normalIndex[i]].x,normal[normalIndex[i]].y,normal[normalIndex[i]].z);
    meshVertices.push_back(meshData);
    meshNormal.push_back(normData);
}

  std::cout << meshVertices.size() << " " << meshNormal.size()  << std::endl;
  // for(int i=0;i<normal.size();i++)
  // {
  //   std::cout << normal[i].x << " " << normal[i].y << " " << normal[i].z << " " << std::endl;
  // }
  return 0;
}