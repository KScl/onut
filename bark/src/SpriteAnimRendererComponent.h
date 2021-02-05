#pragma once

#include <onut/BlendMode.h>
#include <onut/ForwardDeclaration.h>
#include "_2DRendererComponent.h"

OForwardDeclare(SpriteAnim);
OForwardDeclare(SpriteAnimInstance);
ForwardDeclare(SpriteAnimRendererComponent);

class SpriteAnimRendererComponent final : public _2DRendererComponent
{
public:
    COMPONENT_PROPERTY(OSpriteAnimRef, spriteAnim, nullptr, PROP_CPP_SETTER);
    COMPONENT_PROPERTY(std::string, anim, "", PROP_CPP_SETTER);
    COMPONENT_PROPERTY(Color, color, Color::White);
    COMPONENT_PROPERTY(bool, smooth, true);
    COMPONENT_PROPERTY(int, blendMode, (int)OBlendPreMultiplied);

    void onCreate() override;
    void onUpdate(float dt) override {};
    void onDestroy() override {};
    
    void render(onut::RenderStates& rs, OSpriteBatch* sb) override;

    void set_spriteAnim(const OSpriteAnimRef& value);
    void set_anim(const std::string& value);

    OSpriteAnimInstanceRef spriteAnimInstance;
    
    COMPONENT_DECLARATION(SpriteAnimRendererComponent, SpriteAnimRenderer)
    // [GENERATED COMPONENT DECLARATION BEGIN]
public:
#if BARK_EDITOR
    Json::Value serialize() override
    {
        auto json = Component::serialize();

        setJson_OSpriteAnimRef(json, "spriteAnim", spriteAnim);
        setJson_std_string(json, "anim", anim);
        setJson_Color(json, "color", color);
        setJson_bool(json, "smooth", smooth);
        setJson_int(json, "blendMode", blendMode);

        return std::move(json);
    }
#endif

    void deserialize(const Json::Value& json) override
    {
        Component::deserialize(json);

        spriteAnim = getJson_OSpriteAnimRef(json, "spriteAnim", nullptr);
        anim = getJson_std_string(json, "anim", "");
        color = getJson_Color(json, "color", Color::White);
        smooth = getJson_bool(json, "smooth", true);
        blendMode = getJson_int(json, "blendMode", (int)OBlendPreMultiplied);
    }

    static void* js_prototype;
    void* getJSPrototype() override { return js_prototype; };
    // [GENERATED COMPONENT DECLARATION END]
};
