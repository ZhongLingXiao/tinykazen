#include <kazen/accel.h>


NAMESPACE_BEGIN(kazen)

void Accel::addMesh(Mesh *mesh) {

}

void Accel::build() {
    /* Nothing to do here for now */
}

bool Accel::rayIntersect(const Ray3f &ray_, Intersection &its, bool shadowRay) const {
    bool foundIntersection = false;  // Was an intersection found so far?

    /* Do intersection test here */

    return foundIntersection;
}

NAMESPACE_END(kazen)

