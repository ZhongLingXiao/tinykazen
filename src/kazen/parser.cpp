#include <kazen/common.h>
#include <kazen/parser.h>
#include <kazen/proplist.h>
#include <pugixml.hpp>

NAMESPACE_BEGIN(kazen)

Object *loadFromXML(const std::string &filename) {

    /// Remove this: temp test code
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.c_str());

    pugi::xml_node scene = doc.child("scene");
     std::cout << scene.name() << std::endl;
    for (pugi::xml_node_iterator it = scene.begin(); it != scene.end(); ++it) {
        auto str = fmt::format("{}", string::indent(it->name(), 4));
        std::cout << str << std::endl;
        
        for (pugi::xml_attribute_iterator ait = it->attributes_begin(); ait != it->attributes_end(); ++ait) {
            std::cout << " : " << ait->name() << "=" << ait->value();
        }

        std::cout << std::endl;
    }

    return nullptr;
}

NAMESPACE_END(kazen)