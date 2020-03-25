# Interpolation
## Description
Student project of Geometric Algebra, using OpenGL3 and C3ga.
<br/>
<br/>
The goal of the project is to make interpolation between two spheres.
# Dependencies
```SDL
OpenGL3
GLEW 
Eigen3
C3GA
```
# Build
Needs dependencies to work.
In the root project :
```shell script
cd cmake-build-debug
cmake ..
make
```
# How to run it
In cmake-build-debug
```shell script
./interpolation
```
# Notes
Currently, there is only a run as default.
<br/>`purple sphere` is the initial sphere
<br/>`yellow sphere` is the goal sphere
<br/>`red sphere` is the interpolation sphere
<br/> If you want change parameter, you could update Main.cpp at these lines :
```C++
/* Set the sphere 1 */
glm::vec<3, GLfloat> center1 = sphere.getCenter();
sphere.translator(center1.x - 0.8, center1.y - 0.7, center1.z);
sphere2.dilator(1.0);

/* Set the sphere 2 */
glm::vec<3, GLfloat> center2 = sphere2.getCenter();
sphere2.translator(center2.x + 0.5, center2.y + 0.6, center2.z + 0.5);
sphere2.dilator(1.5);
```
# Author
* Jérémie LE BASTARD
# Biblliography
* H. Hadfield & J. Lasenby : Direct Linear Interpolation of Geometric Objects in Conformal Geometric Algebra
  * https://link.springer.com/article/10.1007/s00006-019-1003-y
* Doran, C.: Circle and sphere blending with conformal geometric algebra.  (2003)
  * https://arxiv.org/pdf/cs/0310017.pdf

