#include "Cone.h"
#include <math.h>


float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float value_a;
    float value_b;
    float value_c;

    value_a = dir.x * dir.x + dir.z * dir.z \
                - dir.y * dir.y * (radius/height) * (radius/height);
    value_b = 2 * (dir.x * (posn.x - center.x) + dir.z * (posn.z - center.z) + \
                    (radius/height) * (radius/height) * dir.y * (center.y + height - posn.y));
    value_c = pow(posn.x - center.x, 2) + pow(posn.z - center.z, 2) - pow(radius/height, 2) * pow(height - posn.y + center.y, 2);

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



    float cone_height = center.y + height;
    float t1_y = posn.y + dir.y * t1;
    float t2_y = posn.y + dir.y * t2;

    if(t1_y > cone_height && t2_y > cone_height){
        return -1;
    }
    if(t1_y < center.y && t2_y < center.y){
        return -1;
    }
    return t1;

}




glm::vec3 Cone::normal(glm::vec3 p)
{
    glm::vec3 n;
    n.x = (p.x - center.x);
    n.y = 0;
    n.z = (p.z - center.z);

    n = glm::normalize(n);
    return n;
}
