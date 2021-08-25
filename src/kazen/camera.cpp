/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include <kazen/camera.h>
#include <kazen/rfilter.h>
#include <kazen/warp.h>

NAMESPACE_BEGIN(kazen)

/**
 * \brief Perspective camera with depth of field
 *
 * This class implements a simple perspective camera model. It uses an
 * infinitesimally small aperture, creating an infinite depth of field.
 */
class PerspectiveCamera final : public Camera {
public:
    using Float = enoki::Packet<float>;
    KAZEN_BASE_TYPES()

    PerspectiveCamera(const PropertyList &propList) {
        /* Width and height in pixels. Default: 720p */
        m_outputSize.x() = propList.getInt("width", 1280);
        m_outputSize.y() = propList.getInt("height", 720);
        m_invOutputSize = m_outputSize.cast<float>().cwiseInverse();

        /* Specifies an optional camera-to-world transformation. Default: none */
        m_cameraToWorld = propList.getTransform("toWorld", ScalarTransform4f());

        /* Horizontal field of view in degrees */
        m_fov = propList.getFloat("fov", 30.0f);

        /* Near and far clipping planes in world-space units */
        m_nearClip = propList.getFloat("nearClip", 1e-4f);
        m_farClip = propList.getFloat("farClip", 1e4f);

        m_rfilter = nullptr;
    }

    void activate() {
        ScalarFloat aspect = m_outputSize.x() / (ScalarFloat) m_outputSize.y();

        /* Project vectors in camera space onto a plane at z=1:
         *
         *  xProj = cot * x / z
         *  yProj = cot * y / z
         *  zProj = (far * (z - near)) / (z * (far-near))
         *  The cotangent factor ensures that the field of view is 
         *  mapped to the interval [-1, 1].
         */

        /**
         * Translation and scaling to shift the clip coordinates into the
         * range from zero to one. Also takes the aspect ratio into account.
         */
        m_sampleToCamera = 
            ScalarTransform4f::scale(Vector3f(-0.5f, -0.5f * aspect, 1.f)) *
            ScalarTransform4f::translate(Vector3f(-1.f, -1.f / aspect, 0.f)) * 
            ScalarTransform4f::perspective(m_fov, m_nearClip, m_farClip);

        /* If no reconstruction filter was assigned, instantiate a Gaussian filter */
        if (!m_rfilter)
            // m_rfilter = static_cast<ReconstructionFilter *>(ObjectFactory::createInstance("gaussian", PropertyList()));
            m_rfilter = static_cast<ReconstructionFilter *>(ObjectFactory::createInstance("tent", PropertyList()));
    }

    Color3f sampleRay(Ray3f &ray,
            const Point2f &samplePosition,
            const Point2f &apertureSample) const {
        // /* Compute the corresponding position on the 
        //    near plane (in local camera space) */
        // Point3f nearP = m_sampleToCamera * Point3f(
        //     samplePosition.x() * m_invOutputSize.x(),
        //     samplePosition.y() * m_invOutputSize.y(), 0.0f);

        // /* Turn into a normalized ray direction, and
        //    adjust the ray interval accordingly */
        // Vector3f d = nearP.normalized();
        // float invZ = 1.0f / d.z();

        // ray.o = m_cameraToWorld * Point3f(0, 0, 0);
        // ray.d = m_cameraToWorld * d;
        // ray.mint = m_nearClip * invZ;
        // ray.maxt = m_farClip * invZ;
        // ray.update();

        // return Color3f(1.0f);
    }

    void addChild(Object *obj) {
        switch (obj->getClassType()) {
            case EReconstructionFilter:
                if (m_rfilter)
                    throw Exception("Camera: tried to register multiple reconstruction filters!");
                m_rfilter = static_cast<ReconstructionFilter *>(obj);
                break;
            default:
                throw Exception("Camera::addChild(<{}>) is not supported!", classTypeName(obj->getClassType()));
        }
    }

    /// Return a human-readable summary
    std::string toString() const {
        using string::indent;
        oss << "PerspectiveCamera[]" << '\n'
            << "]";
        return oss.str();
    }
private:
    ScalarVector2f m_invOutputSize;
    ScalarTransform4f m_sampleToCamera;
    ScalarTransform4f m_cameraToWorld;
    ScalarFloat m_fov;
    ScalarFloat m_nearClip;
    ScalarFloat m_farClip;
};

KAZEN_REGISTER_CLASS(PerspectiveCamera, "perspective");
NAMESPACE_END(kazen)
