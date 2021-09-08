#pragma once

#include <kazen/common.h>

NAMESPACE_BEGIN(kazen)

/**
 * \brief Intersection data structure
 *
 * This data structure records local information about a ray-triangle intersection.
 * This includes the position, traveled ray distance, uv coordinates, as well
 * as well as two local coordinate frames (one that corresponds to the true
 * geometry, and one that is used for shading computations).
 */
template <typename Float_>
struct Intersection {
    using Float     = Float_;
    using Mask      = mask_t<Float>;
    using Point3f   = Point<Float, 3>;
    using Vector3f  = Vector<Float, 3>;
    using MeshPtr   = replace_scalar_t<Float, const Mesh *>;

    /// Position of the surface intersection
    Point3f p;
    /// Unoccluded distance along the ray
    Float t = math::Infinity<Float>;
    /// UV coordinates, if any
    Point2f uv;
    /// Shading frame (based on the shading normal)
    Frame3f shFrame;
    /// Geometric frame (based on the true geometry)
    Frame3f geoFrame;
    /// Pointer to the associated mesh
    const MeshPtr *mesh;

    /// Create an uninitialized intersection record
    Intersection() : mesh(nullptr) { }

    /// Transform a direction vector into the local shading frame
    Vector3f toLocal(const Vector3f &d) const {
        return shFrame.toLocal(d);
    }

    /// Transform a direction vector from local to world coordinates
    Vector3f toWorld(const Vector3f &d) const {
        return shFrame.toWorld(d);
    }

    /// Return a human-readable summary of the intersection record
    std::string toString() const;


    ENOKI_STRUCT(Intersection, p, t, uv, shFrame, geoFrame);
};



class Mesh : Object {
public:
    using Float = enoki::Packet<float>;
    KAZEN_BASE_TYPES()
    using Intersection3f = Intersection<Float>;
    using ScalarIndex   = uint32_t;
    using ScalarSize    = uint32_t;
    using InputFloat    = float;
    using InputPoint3f  = Point<InputFloat, 3>;
    using InputVector2f = Vector<InputFloat, 2>;
    using InputVector3f = Vector<InputFloat, 3>;
    using InputNormal3f = Normal<InputFloat, 3>;
    using FloatStorage  = DynamicBuffer<replace_scalar_t<Float, InputFloat>>;

    /// Release all memory
    virtual ~Mesh();

    /// Initialize internal data structures (called once by the XML parser)
    virtual void activate();
    
    bool rayIntersect(const Ray3f &ray, float &u, float &v, float &t) const;

    /// Return the total number of face(current is triangles) in this shape
    ScalarSize getFaceCount() const { return m_faceCount; }

    /// Return the total number of vertices in this shape
    ScalarSize getVertexCount() const { return m_vertexCount; }

    /// Return vertex positions buffer
    const FloatStorage &getVertexPositions() const { return m_V; }

    ///  Return vertex normals buffer
    const FloatStorage &getVertexNormals() const { return m_N; }

    /// Return vertex texture coordinates buffer
    const MatrixXf &getVertexTexCoords() const { return m_UV; }

    /// Return a pointer to the triangle vertex index list
    const DynamicBuffer<UInt32> &getIndices() const { return m_F; }

    /// Return the surface area of the mesh
    ScalarFloat surfaceArea() const;

    /// Return an axis-aligned bounding box of the entire mesh
    const ScalarBoundingBox3f &bbox() const { return m_bbox; }

    /// Return an axis-aligned bounding box containing the given triangle
    ScalarBoundingBox3f getBoundingBox(ScalarIndex index) const;

    /// Is this mesh an area light?
    bool isLight() const { return m_light != nullptr; }

    /// Return a pointer to an attached area light instance
    Light *getLight(Mask /* unused */ = false) { return m_light; }

    /// Return a pointer to an attached area light instance (const version)
    const Light *getLight(Mask /* unused */ = false) const { return m_light; }

    /// Return a pointer to the BSDF associated with this mesh
    BSDF *getBSDF(Mask /* unused */ = false) { return m_bsdf; }

    /// Return a pointer to the BSDF associated with this mesh (const version)
    const BSDF *getBSDF(Mask /* unused */ = false) const { return m_bsdf; }

    /// Register a child object (e.g. a BSDF) with the mesh
    virtual void addChild(Object *child);

    /// Return the name of this mesh
    const std::string &getName() const { return m_name; }

    /// Return a human-readable summary of this instance
    std::string toString() const;

    /**
     * \brief Return the type of object (i.e. Mesh/BSDF/etc.)
     * provided by this instance
     * */
    EClassType getClassType() const { return EMesh; }

    // Supporting enoki scalar getter functions
    ENOKI_CALL_SUPPORT_FRIEND()
    ENOKI_PINNED_OPERATOR_NEW(Float)

protected:
    /// Create an empty mesh
    Mesh();

protected:
    std::string             m_name;                 ///< Identifying name
    ScalarBoundingBox3f     m_bbox;                 ///< Bounding box of the mesh
    ScalarSize              m_vertexCount = 0;      ///< Total number of vertices
    ScalarSize              m_faceCount = 0;        ///< Total number of faces

    FloatStorage            m_V;                    ///< Vertex positions
    FloatStorage            m_N;                    ///< Vertex normals
    FloatStorage            m_UV;                   ///< Vertex texture coordinates
    DynamicBuffer<UInt32>   m_F;                    ///< Faces
    BSDF                    *m_bsdf = nullptr;      ///< BSDF of the surface
    Light                   *m_light = nullptr;     ///< Associated light, if any
};

ENOKI_CALL_SUPPORT_TEMPLATE_BEGIN(kazen::Mesh)
    ENOKI_CALL_SUPPORT_GETTER_TYPE(getLight, m_light)
    ENOKI_CALL_SUPPORT_GETTER_TYPE(getBSDF, m_bsdf)
    auto isEmitter() const { return neq(getLight(), nullptr); }
ENOKI_CALL_SUPPORT_TEMPLATE_END(kazen::Mesh)

NAMESPACE_END(kazen)

