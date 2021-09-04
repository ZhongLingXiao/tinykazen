// #include <nori/integrator.h>
// #include <nori/scene.h>
#include <kazen/integrator.h>

NAMESPACE_BEGIN(kazen)

classTempIntegrator : public Integrator {
public:
    TempIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }

    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray, Mask active) const {
        /* temp return */
        return Color3f(.0f);
    }

    std::string toString() const {
        return "TestIntegrator[]";
    }
};

KAZEN_REGISTER_CLASS(TempIntegrator, "temp");

NAMESPACE_END(kazen)