#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "GL/glew.h"
#include <GLFW/glfw3.h>
// #include "helper.cpp"
#include "scene.h"
#include "light.h"
// #include "objects.h"
// #include "utils.h"
// #include "light.h"
// #include "camera.h"
#include <iostream>
#include<omp.h>

#define QUIT(m,v)      { fprintf(stderr, "%s:%s\n", m, v); exit(1); }

Scene scene;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

class OpenGLdraw {
   public:
   void init(int W, int H) {
      unsigned int texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
  
      width = W; height = H;
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, W, H, 0,
             GL_RGB, GL_UNSIGNED_BYTE, NULL);
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
      glGenFramebuffers(1, &fbo);
      glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
      glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

      glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
   }

   void draw(GLubyte *image, GLuint W=0, GLuint H=0, int atx=0, int aty=0) {
      if(H == 0) H = height;
      if(W == 0) W = width;
      glTexSubImage2D(GL_TEXTURE_2D, 0, atx, aty, W, H, GL_RGB, GL_UNSIGNED_BYTE, image);

      glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
      glBlitFramebuffer(0, 0, W, H, atx, aty, W, H,
                     GL_COLOR_BUFFER_BIT, GL_NEAREST);
      glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

   }
   private:
	int width, height;
	GLuint fbo;
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
         if(scene.toggle==0){
            scene.update_camera(0,1);
         }else if(scene.toggle==1){
            scene.update_spheres_position(1);
         }else{
            scene.update_light_positions(1);
         }
         std::cout<<"Right\n";
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
         if(scene.toggle==0){
            scene.update_camera(0,0);
         }else if(scene.toggle==1){
            scene.update_spheres_position(0);
         }else{
            scene.update_light_positions(0);
         }
         std::cout<<"Left\n";
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS){
         if(scene.toggle==0){
            scene.update_camera(1,1);
         }
         std::cout<<"Up\n";
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
         if(scene.toggle==0){
            scene.update_camera(1,0);
         }
         std::cout<<"Down\n";
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS){
         scene.toggle = (scene.toggle +1)%3;
         /* toggle value what it represents:
            0                 Camera
            1                 Spheres
            2                 Lights
         */
         std::cout<<"toggled to: "<<scene.toggle<<"\n";
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS){
         scene.toggle_aliasing();
         std::cout<<"anti_aliasing: "<<scene.anti_aliasing<<"\n";
    }
}

int main( int argc, char* args[] )
{
   int M, m;

    if (!glfwInit())
        QUIT("gWindow_GLFW", "Could not Initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwSetErrorCallback(error_callback);

    bool coreProfile = true;
    if(coreProfile) {
       glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__ 
       glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
   }

    GLFWwindow *window = glfwCreateWindow(640, 480, "Simple", NULL, NULL);

    if (!window) {
        glfwTerminate();
        QUIT("gWindow_GLFW", "Could not create Window");
    }

    glfwMakeContextCurrent(window);

    if ( GLEW_OK != glewInit() ) {
        glfwTerminate();
        QUIT("gWindow_GLFW","glewInit failed");
    }

    glfwSwapInterval(1);

   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   printf("Width = %d, Height = %d\n", width, height);
   // width -= 5;

   OpenGLdraw opengl;
   opengl.init(width, height);
   

   GLubyte data[640*480*3];
   GLubyte color = 0;

   omp_set_num_threads(8);
   
   scene.make_scene();
   scene.trace_prep();
   scene.compute_photon_map();
   scene.compute_caustic_photon_map();
   ////////////////////////////////////////////////////////////////////
   ///////////////////////Changing Data//////////////////////////////////////

   glfwSetKeyCallback(window, key_callback);

   while (!glfwWindowShouldClose(window))
   {
      
      std::cout<<"1\n";
      float gamma = 1/2.2;
      int progress = 0;
      int max_pix = height*width;
      #pragma omp parallel num_threads(16)
      {
         std::cout<<"num threads: "<<omp_get_max_threads()<<"\n";
         #pragma omp for
         for(int i=0;i<height;i++){
            for(int j=0;j<width;j++){
               ColorRGB col = scene.trace(j,i,width,height);
               // col = glm::clamp(col,0.0f,1.0f);
               col.x = glm::clamp(glm::pow(col.x,gamma), 0.0f, 1.0f);
               col.y = glm::clamp(glm::pow(col.y,gamma), 0.0f, 1.0f);
               col.z = glm::clamp(glm::pow(col.z,gamma), 0.0f, 1.0f);
               int pixel = (j*3)+(i*3*width);
               data[pixel++] = col.x*255;
               data[pixel++] = col.y*255;
               data[pixel++] = col.z*255;
               progress++;
               std::cout<<"Redenring done: "<<(progress*100.0/max_pix)<<"%\n";
            }
         }
      }
      

      //////////////////////////////////////////////////////////////////////////
      glClearColor(0.5, 0.5, 0.5, 0.0);
      glClear(GL_COLOR_BUFFER_BIT);
      
      opengl.draw(data, 640, 480, 0, 0);
      glfwSwapBuffers(window);
      // glfwPollEvents();
      glfwWaitEvents();
   }
            
   glfwTerminate();
   return 0;
}

//g++ sample.cpp -lGL -lGLEW -lglfw
//g++ sample.cpp -lGL -lGLEW -lglfw -o raytracer