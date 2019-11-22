#include "Cylinder.h"
#include <math.h>


float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float value_a;
    float value_b;
    float value_c;

    value_a = dir.x * dir.x + dir.z * dir.z \
                + 0.25 * dir.y * dir.y - dir.x * dir.y;
    value_b = 2 * (dir.x * (posn.x - center.x) + dir.z * (posn.z - center.z) \
                    + 0.25 * posn.y * dir.y + 0.5 * center.x * dir.y - 0.5 * posn.x*dir.y - 0.5 * dir.x * posn.y);
    value_c = (posn.x - center.x) * (posn.x - center.x) + \
              (posn.z - center.z) * (posn.z - center.z) - radius * radius\
              + 0.25 * posn.y * posn.y+ center.x * posn.y - posn.x * posn.y;

    float delta = value_b * value_b - 4* value_a * value_c;

    if(fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;

    float t1 = (-value_b - sqrt(delta)) / (2 * value_a);
    float t2 = (-value_b + sqrt(delta)) / (2 * value_a);

    if(fabs(t1) < 0.001 )
    {
        if (t2 > 0){
            return t2;
        } else {
            t1 = -1.0;
        }
    }
    if(fabs(t2) < 0.001 ) {
        t2 = -1.0;
    }

    if(t1 > t2){
        float switch_t1 = t1;
        t1 = t2;
        t2 = switch_t1;
    }



    float cylinder_height = center.y + height;
    float t1_y = posn.y + dir.y * t1;
    float t2_y = posn.y + dir.y * t2;

    if(t1_y > cylinder_height && t2_y > cylinder_height){
        return -1;
    }

     if(t1_y < center.y && t2_y < center.y){
        return -1;
    }

    if(t1_y > cylinder_height && t2_y < cylinder_height){
        return t2;
    }
    return t1;

}



glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n;
    n.x = (p.x - center.x);
    n.y = 0;
    n.z = (p.z - center.z);

    n = glm::normalize(n);
    return n;
}
