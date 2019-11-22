#ifndef H_TETRAHEDRON
#define H_TETRAHEDRON

#include <glm/glm.hpp>
#include "SceneObject.h"

class Tetrahedron : public SceneObject
{
private:
    glm::vec3 a, b, c, d;  //The four vertices

public:	
	Tetrahedron(void);
	
    Tetrahedron(glm::vec3 pa, glm::vec3 pb, glm::vec3 pc, glm::vec3 pd, glm::vec3 col)
		: a(pa), b(pb), c(pc), d(pd)
	{
		color = col;
	};
	
	float intersect(glm::vec3 posn, glm::vec3 dir);
	
	glm::vec3 normal(glm::vec3 pt);

};

#endif
