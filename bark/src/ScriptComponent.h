#pragma once

#include <onut/ForwardDeclaration.h>
#include "Component.h"

ForwardDeclare(Script)

class ScriptComponent final : public Component, public std::enable_shared_from_this<ScriptComponent>
{
public:
    ScriptRef script;

    void addScriptPropertiesToJSObject();

    void onCreate() override;
    void onEnable() override;
    void onUpdate(float dt) override;
    void onDisable() override;
    void onDestroy() override;

public:
#if BARK_EDITOR
    Json::Value serialize() override
    {
        auto json = Component::serialize();
        return std::move(json);
    }
#endif

    void deserialize(const Json::Value& json) override
    {
        Component::deserialize(json);
    }

    void* getJSPrototype() override { return nullptr; }; // Unused
};
