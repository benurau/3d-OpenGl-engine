#ifndef COLISSIONS_H
#define COLISSIONS_H

#include "HitBox.h"

std::vector<glm::vec3> computeVertexNormals(const std::vector<glm::vec3>& positions, const std::vector<unsigned int>& indices);
bool point_Box_Colission(HitBox& box, glm::vec3 position, glm::vec3& Movement);


#endif