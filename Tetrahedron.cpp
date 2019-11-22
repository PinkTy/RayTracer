#include "Tetrahedron.h"
#include <math.h>

float Tetrahedron::intersect(glm::vec3 posn, glm::vec3 dir)
{


    glm::vec3 n = normal(posn);
    glm::vec3 vdif = a - posn;
    float vdotn = glm::dot(dir, n);
    if(fabs(vdotn) < 1.e-4) return -1;
    float t = glm::dot(vdif, n)/vdotn;
    if(fabs(t) < 0.0001) return -1;
    glm::vec3 q = posn + dir*t;
    else return -1;
}
