#pragma once


#include <kazen/common.h>
NAMESPACE_BEGIN(kazen)

/**
 * \brief 
 * This is an associative container used to supply the constructors
 * of Object subclasses with parameter information.
 */
class PropertyList {
public:

    /// Supported types of properties
    enum class Type {
        Bool,               ///< Boolean value (true/false)
        Int32,              ///< 32-bit signed integer
        Float,              ///< Floating point value
        String,             ///< String
        Array3f,            ///< 3D array
        Color,              ///< Tristimulus color value
        Transform,          ///< 4x4 transform for homogeneous coordinates
    };

    using Float = float;
    using Array3f = Array<Float, 3>;
    KAZEN_BASE_TYPES()

    /// Construct an empty property container
    PropertyList();

    /// Release all memory
    ~PropertyList();    

    /// Verify if a value with the specified name exists
    bool hasProperty(const std::string &name) const;

    /// Returns the type of an existing property.
    Type type(const std::string &name) const;

    /// Return one of the parameters (converting it to a string if necessary)
    std::string toString(const std::string &name) const;

    /// Return one of the parameters (converting it to a string if necessary, with default value)
    std::string toString(const std::string &name, const std::string &def_val) const;

    friend std::ostream &operator<<(std::ostream &os, const PropertyList &p);

public:

    /// Set a boolean property
    void setBool(const std::string &name, const bool &value);
    
    /// Get a boolean property, and throw an exception if it does not exist
    const bool& getBool(const std::string &name) const;

    /// Get a boolean property, and use a default value if it does not exist
    const bool& getBool(const std::string &name, const bool &defaultValue) const;

    /// Set an integer property
    void setInt(const std::string &name, const int &value);
    
    /// Get an integer property, and throw an exception if it does not exist
    const int32_t& getInt(const std::string &name) const;

    /// Get am integer property, and use a default value if it does not exist
    const int32_t& getInt(const std::string &name, const int &defaultValue) const;

    /// Set a float property
    void setFloat(const std::string &name, const float &value);
    
    /// Get a float property, and throw an exception if it does not exist
    Float getFloat(const std::string &name) const;

    /// Get a float property, and use a default value if it does not exist
    Float getFloat(const std::string &name, const float &defaultValue) const;

    /// Set a string property
    void setString(const std::string &name, const std::string &value);

    /// Get a string property, and throw an exception if it does not exist
    const std::string& getString(const std::string &name) const;

    /// Get a string property, and use a default value if it does not exist
    const std::string& getString(const std::string &name, const std::string &defaultValue) const;

    /// Set a color property
    void setColor3f(const std::string &name, const Color3f &value);

    /// Get a color property, and throw an exception if it does not exist
    const Color3f& getColor3f(const std::string &name) const;

    /// Get a color property, and use a default value if it does not exist
    const Color3f& getColor3f(const std::string &name, const Color3f &defaultValue) const;

    // // TODO: add point3f & vector3f support
    // /// Set a point property
    // void setPoint(const std::string &name, const Point3f &value);

    // /// Get a point property, and throw an exception if it does not exist
    // Point3f getPoint(const std::string &name) const;

    // /// Get a point property, and use a default value if it does not exist
    // Point3f getPoint(const std::string &name, const Point3f &defaultValue) const;

    // /// Set a vector property
    // void setVector(const std::string &name, const Vector3f &value);

    // /// Get a vector property, and throw an exception if it does not exist
    // Vector3f getVector(const std::string &name) const;

    // /// Get a vector property, and use a default value if it does not exist
    // Vector3f getVector(const std::string &name, const Vector3f &defaultValue) const;

    /// Set a array3f property
    void setArray3f(const std::string &name, const Array3f &value);

    /// Get a point property, and throw an exception if it does not exist
    Array3f getArray3f(const std::string &name) const;

    /// Get a point property, and use a default value if it does not exist
    Array3f getArray3f(const std::string &name, const Array3f &defaultValue) const;


    /// Set a transform property
    void setTransform(const std::string &name, const Transform4f &value);

    /// Get a transform property, and throw an exception if it does not exist
    const Transform4f& getTransform(const std::string &name) const;

    /// Get a transform property, and use a default value if it does not exist
    const Transform4f& getTransform(const std::string &name, const Transform4f &defaultValue) const;


private:
    
    /// PImpl(Pointer to implementation): https://en.cppreference.com/w/cpp/language/pimpl 
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

NAMESPACE_END(kazen)