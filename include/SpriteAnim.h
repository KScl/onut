#pragma once
#include "Anim.h"
#include "ContentManager.h"
#include "Texture.h"

#include <string>
#include <unordered_map>
#include <vector>

extern onut::ContentManager* OContentManager;

namespace onut
{
    class SpriteAnimDefinition
    {
    public:
        static SpriteAnimDefinition* createFromFile(const std::string& filename, onut::ContentManager* pContentManager);
        virtual ~SpriteAnimDefinition();

        struct Frame
        {
            Texture* pTexture = nullptr;
            Vector4 UVs = {0, 0, 1, 1};
            Vector2 origin = {.5f, .5f};
        };

        struct Anim
        {
            std::string name;
            std::vector<Frame> frames;
            float duration = 1.f;
            bool loop = false;
        };

        Anim* getAnim(const std::string& name);

    private:
        SpriteAnimDefinition();

        std::unordered_map<std::string, Anim> m_anims;
    };
}

namespace onut
{
    class SpriteAnim
    {
    public:
        SpriteAnim();
        SpriteAnim(SpriteAnimDefinition* pDefinition);
        SpriteAnim(const std::string& definitionFilename, ContentManager* pContentManager = OContentManager);

        void start(const std::string& animName);
        void startBackward(const std::string& animName);
        void stop(bool reset = false);
        bool isPlaying() const;

        int getFrameId() const;

        Texture* getTexture() const;
        const Vector4& getUVs() const;
        const Vector2& getOrigin() const;

    private:
        SpriteAnimDefinition* m_pDefinition = nullptr;
        SpriteAnimDefinition::Anim* m_pCurrentAnim = nullptr;
        Anim<float> m_frame = 0.f;
    };
};

using OSpriteAnim = onut::SpriteAnim;
