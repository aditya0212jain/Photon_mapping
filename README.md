# Photon_mapping
Photon Mapping project for the course COL781 (2020).

# Compilation
```
g++ main.cpp light.cpp camera.cpp objects.cpp scene.cpp utils.cpp -lGL -lGLEW -lglfw -lgomp -fopenmp -o raytracer
```
Or use the make file in the repo.

# Running
```
./raytracer
```

# Dependencies
* GLFW
* OpenMP

# Features
1. Mesh files supported ( triangular faces only)
2. Parallelization by openMP
3. Ray tracing with soft shadows
4. GlobalIllumination
5. Caustics for glassy objects
6. Primitives supported: box, trianlge, cylinder, plane and sphere

# Examples
![Direct](/doc/direct.png)
![Soft_shadows](/doc/softshadows.png)
![Global_illumination_And_Caustics](/doc/photonMap/png)
![Global_illumination_And_Caustics_And_SoftShadows](/doc/softshadows_photonMap.png)

# Future Work
* Add GPU acceleration using CUDA
* Support XML file loading

# References
kdtree+ by Martin F. Krafft <libkdtree@pobox.madduck.net>
https://graphics.stanford.edu/courses/cs348b-00/course8.pdf
https://www.cs.drexel.edu/~david/Classes/Papers/egwr96.pdf
https://web.cs.wpi.edu/~emmanuel/courses/cs563/write_ups/zackw/photon_mapping/PhotonMapping.html
www.scratchpixel.com and lighthouse3d.com for ray intersection tutorial
https://github.com/kbladin/Monte_Carlo_Ray_Tracer for .obj files