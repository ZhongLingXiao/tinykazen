#pragma once


#include <kazen/common.h>

NAMESPACE_BEGIN(kazen)

class Renderer {
public:
    using Float = enoki::Packet<float>;
    KAZEN_BASE_TYPES()

    void renderSample(const Scene *scene, Sampler *sampler, ImageBlock &block, const Vector2f &pos, Mask active);
    void renderBlock(const Scene *scene, Sampler *sampler, ImageBlock &block);
    void render(Scene *scene, const std::string &filename);

};

NAMESPACE_END(kazen)
