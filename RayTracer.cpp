/*========================================================================
* COSC 363  Computer Graphics (2019)
* Assignment 2
* Xuyang Cai
* Biud command: g++ -Wall -o "%e"  "%f" Cone.cpp Sphere.cpp SceneObject.cpp Ray.cpp Plane.cpp TextureBMP.cpp Cylinder.cpp -lm -lGL -lGLU -lglut
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "TextureBMP.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include <GL/glut.h>



using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene
TextureBMP texture;                 //texture picture for sphere
TextureBMP texture_floor;           //texture picture for floor

//---The first light source ----------------------------------
//   Generate first light source for the gragh
//----------------------------------------------------------------------------------
glm::vec3 first_light_source(Ray ray, glm::vec3 normalVector, glm::vec3 materialCol)
{
    glm::vec3 light(10, 100, 100); //position of first light
    glm::vec3 ambientCol(0.2);     //Ambient color of first light

    glm::vec3 lightVector = light - ray.xpt;
    glm::vec3 unitLightVector = glm::normalize(lightVector);
    float lDotn = glm::dot(normalVector, unitLightVector);
    glm::vec3 ambientColor = ambientCol * materialCol;
    glm::vec3 diffuseColor = lDotn * materialCol;

    glm::vec3 reflVector = glm::reflect(-unitLightVector, normalVector);
    float rDotv = glm::dot(reflVector, -ray.dir);
    float specularTerm;

    //The ray for the shadow
    Ray shadow(ray.xpt, unitLightVector);
    shadow.closestPt(sceneObjects);

    glm::vec3 colorSum;
    if(rDotv < 0 ){
        specularTerm = 0.0;
    } else {
        specularTerm = pow(rDotv, 10);
    }
    if(lDotn <= 0 || (shadow.xindex > -1 && shadow.xdist < glm::length(lightVector))){
        colorSum = ambientColor;

    } else {
        colorSum = ambientColor + diffuseColor + specularTerm;
    }

    return colorSum;
}

//---The second light source ----------------------------------
//   Generate second light source for the gragh
//----------------------------------------------------------------------------------

glm::vec3 second_light_source(Ray ray, glm::vec3 normalVector, glm::vec3 materialCol, glm::vec3 colorSum)
{
    glm::vec3 second_light(-50, 100, 150); //position of first light
    glm::vec3 ambientCol_second(0.1); //Ambient color of first light
    glm::vec3 lightVector_second = second_light - ray.xpt;
    glm::vec3 unitLightVector_second = glm::normalize(lightVector_second);
    float lDotn_second = glm::dot(normalVector, unitLightVector_second);
    glm::vec3 ambientColor_second = ambientCol_second * materialCol;
    glm::vec3 diffuseColor_second = lDotn_second * materialCol;
    glm::vec3 reflVector_second = glm::reflect(-unitLightVector_second, normalVector);
    float rDotv_second = glm::dot(reflVector_second, -ray.dir);
    float specularTerm_second;

    //The ray for the shadow
    Ray shadow_second(ray.xpt, unitLightVector_second);
    shadow_second.closestPt(sceneObjects);
     if(rDotv_second < 0 ){
        specularTerm_second = 0.0;
    } else {
        specularTerm_second = pow(rDotv_second, 10);
    }
    if(lDotn_second <= 0 || (shadow_second.xindex > -1 && shadow_second.xdist < glm::length(lightVector_second))){
        colorSum += ambientColor_second;

    } else {
        colorSum += ambientColor_second + diffuseColor_second + specularTerm_second;
    }
    return colorSum;


}

//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//   closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
    glm::vec3 backgroundCol(0);

    glm::vec3 white(1, 1, 1);
    ray.closestPt(sceneObjects);        //Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour
    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);

    glm::vec3 colorSum;
    colorSum = first_light_source(ray, normalVector, materialCol); //generate first shadow
    colorSum = second_light_source(ray, normalVector, materialCol, colorSum); //generate second shadow
    //generate reflection for the sphere1
    if(ray.xindex == 0 && step < MAX_STEPS)
    {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1);
        colorSum = colorSum + (0.8f * reflectedCol);
    }
    //sphere2 textured using "space.bmp"
    if(ray.xindex == 1){
        glm::vec3 d=glm::normalize(normalVector);
        float texcoords = d.x/(2 * M_PI) + 0.5;
        float texcoordt = d.y/(2 * M_PI) + 0.5;
        materialCol = texture.getColorAt(texcoords, texcoordt);
        colorSum += materialCol;
    }
    //apply transparency to sphere3
    if(ray.xindex == 2 && step < MAX_STEPS){
        Ray g(ray.xpt, ray.dir);
        glm::vec3 transparent_col = trace(g, step+1);
        colorSum = colorSum + 0.7f * transparent_col;
    }
    //apply refraction to sphere4
    if(ray.xindex == 3 && step < MAX_STEPS){
        float eta = 1/2.4;
        glm::vec3 g = glm::refract(ray.dir, normalVector, eta);
        Ray refrRay_g(ray.xpt, g);
        refrRay_g.closestPt(sceneObjects);
        glm::vec3 m = sceneObjects[refrRay_g.xindex]->normal(refrRay_g.xpt);
        glm::vec3 h = glm::refract(g, -m, 1/eta);
        Ray Ray_h(refrRay_g.xpt, h);
        glm::vec3 refractedCol = trace(Ray_h, step + 1);
        colorSum = colorSum + refractedCol;
    }
    //floor textured using "floor.bmp"
    if(ray.xindex == 4){
        float texcoords = (ray.xpt.x + 100) / 200;
        float texcoordt = (ray.xpt.z + 30) / -200;
        materialCol = texture_floor.getColorAt(texcoords, texcoordt);
        colorSum += materialCol;
    }
    return colorSum;


}

//---The anti-aliasing module -----------------------------------------------------------
// Generate anti-aliasing for the gragh.
//---------------------------------------------------------------------------------------

glm::vec3 anti_aliasing(float xp, float yp, float cellX, float cellY, glm::vec3 eye)
{
    //genarate four direction for four different rays
    glm::vec3 dir_buttom_left(xp +  0.25 * cellX, yp + 0.25 * cellY, -EDIST);
    glm::vec3 dir_buttom_right(xp +  0.75 * cellX, yp + 0.25 * cellY, -EDIST);
    glm::vec3 dir_top_right(xp +  0.75 * cellX, yp + 0.75 * cellY, -EDIST);
    glm::vec3 dir_top_left(xp +  0.25 * cellX, yp + 0.75 * cellY, -EDIST);

    //define four differnet rays
    Ray ray_buttom_left = Ray(eye, dir_buttom_left);
    ray_buttom_left.normalize();

    Ray ray_buttom_right = Ray(eye, dir_buttom_right);
    ray_buttom_right.normalize();

    Ray ray_top_right = Ray(eye, dir_top_right);
    ray_top_right.normalize();

    Ray ray_top_left = Ray(eye, dir_top_left);
    ray_top_left.normalize();

    glm::vec3 col_buttom_left = trace (ray_buttom_left, 1);
    glm::vec3 col_buttom_right = trace (ray_buttom_right, 1);
    glm::vec3 col_top_right = trace (ray_top_right, 1);
    glm::vec3 col_top_left = trace (ray_top_left, 1);

    //get average from the color
    glm::vec3 col;
    col.r = (col_buttom_left.r + col_buttom_right.r + col_top_right.r + col_top_left.r) / 4;
    col.g = (col_buttom_left.g + col_buttom_right.g + col_top_right.g + col_top_left.g) / 4;
    col.b = (col_buttom_left.b + col_buttom_right.b + col_top_right.b + col_top_left.b) / 4;
    return col;
}
//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
    float xp, yp;  //grid point
    float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
    float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

    glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);  //Each cell is a quad.

    for(int i = 0; i < NUMDIV; i++)     //For each grid point xp, yp
    {
        xp = XMIN + i*cellX;
        for(int j = 0; j < NUMDIV; j++)
        {
            yp = YMIN + j * cellY;
            glm::vec3 col = anti_aliasing(xp, yp, cellX, cellY, eye);
            glColor3f(col.r, col.g, col.b);

            //Draw each cell with its color value
            glVertex2f(xp, yp);
            glVertex2f(xp+cellX, yp);
            glVertex2f(xp+cellX, yp+cellY);
            glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}

//---The floor module -----------------------------------------------------------
// Define a floor plane for the gragh.
//---------------------------------------------------------------------------------------
void floor()
{
    Plane *floor = new Plane(glm::vec3(-100, -20, -30.0),
                              glm::vec3(100., -20, -30),
                              glm::vec3(100., -20, -230),
                              glm::vec3(-100., -20, -230),
                              glm::vec3(0.5,0.5,0.5));

    sceneObjects.push_back(floor);

}

//---The tetrahedron module -----------------------------------------------------------
// Define a tetrahedron for the gragh.
//---------------------------------------------------------------------------------------
void tetrahedron()
{
    Plane *tetrahedron_back = new Plane (glm::vec3(12,0,-67),
                              glm::vec3(16, 0, -67),
                              glm::vec3(14,4,-67),
                              glm::vec3(14,4,-67),
                              glm::vec3(0,0,1));
    sceneObjects.push_back(tetrahedron_back);

    Plane *tetrahedron_left = new Plane (glm::vec3(12,0,-67),
                              glm::vec3(14, 0, -63),
                              glm::vec3(14,4,-67),
                              glm::vec3(14,4,-67),
                              glm::vec3(0,0,1));
    sceneObjects.push_back(tetrahedron_left);

    Plane *tetrahedron_right = new Plane (glm::vec3(14,0,-67),
                              glm::vec3(16, 0, -63),
                              glm::vec3(14,4,-67),
                              glm::vec3(14,4,-67),
                              glm::vec3(0,0,0.5));
    sceneObjects.push_back(tetrahedron_right);

    Plane *tetrahedron_buttom = new Plane (glm::vec3(12,0,-67),
                              glm::vec3(16, 0, -67),
                              glm::vec3(14,0,-67),
                              glm::vec3(14,0,-67),
                              glm::vec3(0,0,1));
    sceneObjects.push_back(tetrahedron_buttom);
}

//---Shear box module -------------------------------------------
//   Create a shear box by six planes.
//----------------------------------------------------------------------------------
void shear_box()
{
    Plane *box_front = new Plane (glm::vec3(10,-15,-120),
                              glm::vec3(20, -15, -120),
                              glm::vec3(25,-5,-120),
                              glm::vec3(15,-5,-120),
                              glm::vec3(0.4157,0.3529,0.8039));
    sceneObjects.push_back(box_front);

    Plane *box_right = new Plane (glm::vec3(20,-15,-120),
                              glm::vec3(20, -15, -130),
                              glm::vec3(25,-5,-130),
                              glm::vec3(25,-5,-120),
                              glm::vec3(0.4157,0.3529,0.8039));
    sceneObjects.push_back(box_right);

    Plane *box_top = new Plane (glm::vec3(15,-5,-120),
                              glm::vec3(25, -5, -120),
                              glm::vec3(25,-5,-130),
                              glm::vec3(15,-5,-130),
                              glm::vec3(0.4157,0.3529,0.8039));
    sceneObjects.push_back(box_top);

    Plane *box_left = new Plane (glm::vec3(10,-15,-130),
                              glm::vec3(10, -15, -120),
                              glm::vec3(15,-5,-120),
                              glm::vec3(15,-5,-130),
                              glm::vec3(0.4157,0.3529,0.8039));
    sceneObjects.push_back(box_left);

    Plane *box_buttom = new Plane (glm::vec3(10,-15,-130),
                              glm::vec3(20, -15, -130),
                              glm::vec3(20,-15,-120),
                              glm::vec3(10,-15,-120),
                             glm::vec3(0.4157,0.3529,0.8039));
    sceneObjects.push_back(box_buttom);
}

//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);



    //-- texture name for sphere2
    char *texture_name = (char *)"space.bmp";
    texture = TextureBMP(texture_name);

    //-- texture name for floor
    char *texture_floor_name = (char *)"floor.bmp";
    texture_floor = TextureBMP(texture_floor_name);

    //-- Create four pointers to four sphere objects
    Sphere *sphere1 = new Sphere(glm::vec3(-5.0, 5.0, -90.0), 15.0, glm::vec3(0, 0, 1));
    sceneObjects.push_back(sphere1);

    Sphere *sphere2 = new Sphere(glm::vec3(4.0, 14.0, -70.0), 3.0, glm::vec3(0));
    sceneObjects.push_back(sphere2);

    Sphere *sphere3 = new Sphere(glm::vec3(0.0, -6.0, -70.0), 3.0, glm::vec3(0,0.5,0));
    sceneObjects.push_back(sphere3);

    Sphere *sphere4 = new Sphere(glm::vec3(10.0, -12.0, -70.0), 3.0, glm::vec3(0));
    sceneObjects.push_back(sphere4);

    floor();

    Cylinder *cylinder = new Cylinder(glm::vec3(-8.0, -15.0, -80.0), 3, 5, glm::vec3(0.3922, 0.5843, 0.9294));
    sceneObjects.push_back(cylinder);

    Cone *cone = new Cone(glm::vec3(0.0, -15.0, -70.0), 1, 5, glm::vec3(0.5412,0.1686,0.8863));
    sceneObjects.push_back(cone);

    tetrahedron();
    shear_box();


}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");
    glutDisplayFunc(display);
    initialize();
    glutMainLoop();
    return 0;
}
