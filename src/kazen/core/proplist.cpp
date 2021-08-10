#include <cstdlib>
#include <map>

#include <kazen/proplist.h>
#include <kazen/transform.h>

NAMESPACE_BEGIN(kazen)
using Float       = typename PropertyList::Float;
using Color3f     = typename PropertyList::Color3f;
using Array3f     = typename PropertyList::Array3f;
using Transform4f = typename PropertyList::Transform4f;

using Variant = variant<
    bool,
    int32_t,
    Float,
    std::string,
    Array3f,
    Color3f,
    Transform4f
>;


struct PropertyList::Impl {
    std::map<std::string, Variant> properties;
};


#define DEFINE_PROPERTY_ACCESSOR(Type, TagName, SetterName, GetterName)                                 \
    void PropertyList::SetterName(const std::string &name, Type const &value) {                         \
        if (hasProperty(name))                                                                          \
            std::cerr << "Property \"" << name <<  "\" was specified multiple times!" << std::endl;     \
        pImpl->properties[name] = (Type) value;                                                         \
    }                                                                                                   \
                                                                                                        \
    Type const & PropertyList::GetterName(const std::string &name) const {                              \
        const auto it = pImpl->properties.find(name);                                                   \
        if (it == pImpl->properties.end())                                                              \
            throw Exception("Property {} has not been specified!", name);                               \
        if (!it->second.is<Type>())                                                                     \
            throw Exception("The property {} has the wrong type (expected <" #TagName ">).", name);     \
        return (Type const &) it->second;                                                               \
    }                                                                                                   \
                                                                                                        \
    Type const & PropertyList::GetterName(const std::string &name, Type const &defaultValue) const {    \
        const auto it = pImpl->properties.find(name);                                                   \
        if (it == pImpl->properties.end())                                                              \
            return defaultValue;                                                                        \
        if (!it->second.is<Type>())                                                                     \
            throw Exception("The property {} has the wrong type (expected <" #TagName ">).", name);     \
        return (Type const &) it->second;                                                               \
    }


DEFINE_PROPERTY_ACCESSOR(bool,          boolean,    setBool,        getBool)
DEFINE_PROPERTY_ACCESSOR(int32_t,       integer,    setInt,         getInt)
DEFINE_PROPERTY_ACCESSOR(std::string,   string,     setString,      getString)
DEFINE_PROPERTY_ACCESSOR(Color3f,       color,      setColor3f,     getColor3f)
DEFINE_PROPERTY_ACCESSOR(Transform4f,   transform,  setTransform,   getTransform)


PropertyList::PropertyList() : pImpl(new Impl()) { }


PropertyList::~PropertyList() { }


bool PropertyList::hasProperty(const std::string &name) const {
    return pImpl->properties.find(name) != pImpl->properties.end();
}


NAMESPACE_BEGIN()
    struct TypeVisitor {
        using Type = PropertyList::Type;
        Type operator()(const std::nullptr_t &) { throw std::runtime_error("Internal error"); }
        Type operator()(const bool &) { return Type::Bool; }
        Type operator()(const int32_t &) { return Type::Int32; }
        Type operator()(const Float &) { return Type::Float; }
        Type operator()(const std::string &) { return Type::String; }
        Type operator()(const Array3f &) { return Type::Array3f; }
        Type operator()(const Color3f &) { return Type::Color; }
        Type operator()(const Transform4f &) { return Type::Transform; }
    };

    struct StreamVisitor {
        std::ostream &os;
        StreamVisitor(std::ostream &os) : os(os) { }
        void operator()(const std::nullptr_t &) { throw std::runtime_error("Internal error"); }
        void operator()(const bool &b) { os << (b ? "true" : "false"); }
        void operator()(const int32_t &i) { os << i; }
        void operator()(const Float &f) { os << f; }
        void operator()(const std::string &s) { os << "\"" << s << "\""; }
        void operator()(const Array3f &t) { os << t; }
        void operator()(const Color3f &t) { os << t; }
        void operator()(const Transform4f &t) { os << t; }
    };
NAMESPACE_END()

PropertyList::Type PropertyList::type(const std::string &name) const {
    const auto it = pImpl->properties.find(name);
    if (it == pImpl->properties.end())
        throw Exception("type(): Could not find property named {}!", name);

    return it->second.visit(TypeVisitor());
}

std::string PropertyList::toString(const std::string &name) const {
    std::ostringstream oss;
    bool found = false;
    for (auto &p : pImpl->properties) {
        if (p.first != name)
            continue;
        p.second.visit(StreamVisitor(oss));
        found = true;
        break;
    }
    if (!found)
        throw Exception("Property {} has not been specified!", name);
    return oss.str();
}

std::string PropertyList::toString(const std::string &name, const std::string &defaultValue) const {
    std::ostringstream oss;
    bool found = false;
    for (auto &p : pImpl->properties) {
        if (p.first != name)
            continue;
        p.second.visit(StreamVisitor(oss));
        found = true;
        break;
    }
    if (!found)
        return defaultValue;
    return oss.str();
}

std::ostream &operator<<(std::ostream &os, const PropertyList &list) {
    auto it = list.pImpl->properties.begin();

    os << "PropertyList[" << std::endl;
    while (it != list.pImpl->properties.end()) {
        os << "  \"" << it->first << "\" -> ";
        it->second.visit(StreamVisitor(os));
        if (++it != list.pImpl->properties.end()) os << ",";
        os << std::endl;
    }
    os << "]" << std::endl;

    return os;
}


/// Setter && Getter ///

/// Float
void PropertyList::setFloat(const std::string &name, const float &value) {
    if (hasProperty(name))
        std::cerr << "Property \"" << name <<  "\" was specified multiple times!" << std::endl;
    pImpl->properties[name] = (Float) value;
}

Float PropertyList::getFloat(const std::string &name) const {
    const auto it = pImpl->properties.find(name);
    if (it == pImpl->properties.end())
        throw Exception("Property {} has not been specified!", name);
    if (!(it->second.is<Float>() || it->second.is<int64_t>()))
        throw Exception("The property {} has the wrong type (expected <float>).", name);
    if (it->second.is<int64_t>())
        return (int64_t) it->second;
    return (Float) it->second;
}

Float PropertyList::getFloat(const std::string &name, const float &defaultValue) const {
    const auto it = pImpl->properties.find(name);
    if (it == pImpl->properties.end())
        return defaultValue;
    if (!(it->second.is<Float>() || it->second.is<int64_t>()))
        throw Exception("The property {} has the wrong type (expected <float>).", name);
    if (it->second.is<int64_t>())
        return (int64_t) it->second;
    return (Float) it->second;
}


/// Array3f
void PropertyList::setArray3f(const std::string &name, const Array3f &value) {
    if (hasProperty(name))
        std::cerr << "Property \"" << name <<  "\" was specified multiple times!" << std::endl;
    pImpl->properties[name] = (Array3f) value;
}

Array3f PropertyList::getArray3f(const std::string &name) const {
    const auto it = pImpl->properties.find(name);
    if (it == pImpl->properties.end())
        throw Exception("Property {} has not been specified!", name);
    if (!it->second.is<Array3f>())
        throw Exception("The property {} has the wrong type (expected <vector> or <point>).", name);
    return it->second.operator Array3f&();
}

Array3f PropertyList::getArray3f(const std::string &name, const Array3f &defaultValue) const {
    const auto it = pImpl->properties.find(name);
    if (it == pImpl->properties.end())
        return defaultValue;
    if (!it->second.is<Array3f>())
        throw Exception("The property {} has the wrong type (expected <vector> or <point>).", name);
    return it->second.operator Array3f&();
}


NAMESPACE_END(kazen)
