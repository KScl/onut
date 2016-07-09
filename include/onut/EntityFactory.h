// Onut include
#include <onut/Entity.h>
#include <onut/Maths.h>
#include <onut/Texture.h>

OEntityRef OCreateSoundEntity(const std::string& sound, const Vector2& position = Vector2::Zero, float volume = 1);
OEntityRef OCreateSpriteEntity(const std::string& filename, const Vector2& position = Vector2::Zero);
OEntityRef OCreateTextEntity(const std::string& font, const std::string& text, const Vector2& position = Vector2::Zero);
OEntityRef OCreateTiledMapEntity(const std::string& filename);
