#include <kazen/rfilter.h>

NAMESPACE_BEGIN(kazen)

/// Tent filter 
class TentFilter final : public ReconstructionFilter {
    using Float = enoki::Packet<float>;
    KAZEN_BASE_TYPES()

public:
    TentFilter(const PropertyList &props) {
        m_radius = 1.f;
        m_invRadius = 1.f / m_radius;
    }

    Float eval(Float x, mask_t<Float> /* active */) const {
        return enoki::max(0.f, 1.f - enoki::abs(x * m_invRadius));
    }
    
    std::string toString() const {
        return "TentFilter[]";
    }

private:
    ScalarFloat m_invRadius;
};
KAZEN_REGISTER_CLASS(TentFilter, "tent");

NAMESPACE_END(kazen)