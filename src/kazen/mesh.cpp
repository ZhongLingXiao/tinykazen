#include <kazen/mesh.h>

NAMESPACE_BEGIN(kazen)

Mesh::Mesh() { }

Mesh::~Mesh() {
    delete m_bsdf;
    delete m_light;
}

void Mesh::activate() {
    if (!m_bsdf) {
        /* If no material was assigned, instantiate a diffuse BRDF */
        m_bsdf = static_cast<BSDF *>(ObjectFactory::createInstance("diffuse", PropertyList()));
    }
}


NAMESPACE_END(kazen)