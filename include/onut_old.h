#pragma once
#include "onut/Async.h"
#include "onut/Font.h"
#include "onut/ContentManager.h"
#include "onut/ParticleSystem.h"

#include "crypto.h"
#include "DefineHelpers.h"
#include "EventManager.h"
#include "Input.h"
#include "GamePad.h"
#include "List.h"
#include "onut/Log.h"
#include "onutUI.h"
#include "ParticleSystemManager.h"
#include "PrimitiveBatch.h"
#include "RectUtils.h"
#include "Renderer.h"
#include "Settings.h"
#include "SpriteAnim.h"
#include "SpriteBatch.h"
#include "Synchronous.h"
#include "TimingUtils.h"
#include "Window.h"

// For quick stuff, we have shortcuts outside of the namespace
extern onut::Renderer*                  ORenderer;
extern onut::SpriteBatch*               OSpriteBatch;
extern onut::PrimitiveBatch*            OPrimitiveBatch;
extern onut::Settings*                  OSettings;
extern onut::EventManager*              OEvent;
extern onut::ParticleSystemManager<>*   OParticles;
extern Vector2                          OMousePos;
extern onut::Input*                     OInput;
extern onut::Window*                    OWindow;
extern onut::UIControl*                 OUI;
extern onut::UIContext*                 OUIContext;

using OAnimb = onut::Anim<bool, float, onut::lerpBool>;
using OAnimf = onut::Anim<float>;
using OAnimi = onut::Anim<int>;
using OAnim2 = onut::Anim<Vector2>;
using OAnim3 = onut::Anim<Vector3>;
using OAnim4 = onut::Anim<Vector4>;
using OAnimStr = onut::Anim<std::string>;
using OAnimc = onut::Anim<Color>;

#define OSB OSpriteBatch
#define OPB OPrimitiveBatch

namespace onut
{
    /**
    This should only be called once
    @param initCallback Called once after the engine has been initialized
    @param updateCallback Called at a fixed 120 times per seconds. Sometimes tho calls can be dropped and run slower if the game performs badly
    @param renderCallback Called once per frame. Draw your stuff in there
    @retun This function never returns until the game shutsdown.
    */
    void run(std::function<void()> initCallback, std::function<void()> updateCallback, std::function<void()> renderCallback);

    /**
    * Call run with a State object
    * Any object that defines an init, update and render call will work also
    */
    template<typename TstateType>
    void run(TstateType& stateManager)
    {
        run(std::bind(&TstateType::init, &stateManager),
            std::bind(&TstateType::update, &stateManager),
            std::bind(&TstateType::render, &stateManager));
    }

    /**
    Get a gamepad for index (0 to 3)
    */
    GamePad* getGamePad(int index);
}

#define ORun onut::run

//--- Resource shortcuts
/*
inline OFontRef OGetBMFont(const std::string& name)
{
    return oContentManager->getResourceAs<OFont>(name);
}

inline OSound* OGetSound(const std::string& name)
{
    return oContentManager->getResourceAs<OSound>(name);
}

inline OSoundCue* OGetSoundCue(const std::string& name)
{
    return oContentManager->getResourceAs<OSoundCue>(name);
}

inline OPfx* OGetPFX(const std::string& name)
{
    return oContentManager->getResourceAs<OPfx>(name);
}

inline void OLoadTexture(const char* pName)
{
    OContentManager->addResource<OTexture>(pName);
}

inline void OLoadBMFont(const char* pName)
{
    OContentManager->addResource<OFont>(pName);
}

inline void OLoadSound(const char* pName)
{
    OContentManager->addResource<OSound>(pName);
}

inline void OLoadSoundCue(const char* pName)
{
    OContentManager->addResource<OSoundCue>(pName);
}

inline void OLoadPFX(const char* pName)
{
    OContentManager->addResource<OPfx>(pName);
}

inline OSoundInstance* OCreateSoundInstance(const char* pName)
{
    auto pSound = OGetSound(pName);
    if (!pSound) return nullptr;
    return pSound->createInstance();
}
*/
inline auto OEmitPFX(const char* pName, const Vector3& position, const Vector3& dir = Vector3::UnitZ) -> decltype(OParticles->emit(nullptr, position, dir))
{
    auto pPfx = OGetParticleSystem(pName);
    return OParticles->emit(pPfx, position, dir);
}

using OEmitterInstance = onut::ParticleSystemManager<>::EmitterInstance;

inline onut::UIControl* OLoadUI(const std::string& name)
{
    std::string filename = "../../assets/ui/" + name;
    return new onut::UIControl(filename.c_str());
}
/*
inline void OPlaySound(const char* pName, float volume = 1.0f, float balance = 0.f, float pitch = 1.f)
{
    OGetSound(pName)->play(volume, balance, pitch);
}

inline void OPlayRandomSound(const std::vector<const char*>& sounds, 
                             float volume = 1.0f, float balance = 0.f, float pitch = 1.f)
{
    OPlaySound(onut::randv(sounds), volume, balance, pitch);
}

inline void OPlaySoundCue(const char* pName, float volume = 1.0f, float balance = 0.f, float pitch = 1.f)
{
    OGetSoundCue(pName)->play(volume, balance, pitch);
}
*/
//--- Game pads
inline onut::GamePad* OGamePad(int index)
{
    return onut::getGamePad(index);
}

inline bool OGamePadPressed(onut::GamePad::eGamePad button, int gamePadIndex = 0)
{
    return OGamePad(gamePadIndex)->isPressed(button);
}

inline bool OGamePadJustPressed(onut::GamePad::eGamePad button, int gamePadIndex = 0)
{
    return OGamePad(gamePadIndex)->isJustPressed(button);
}

inline bool OGamePadJustReleased(onut::GamePad::eGamePad button, int gamePadIndex = 0)
{
    return OGamePad(gamePadIndex)->isJustReleased(button);
}

inline const Vector2& OLThumb(int gamePadIndex = 0)
{
    return OGamePad(gamePadIndex)->getLeftThumb();
}

inline const Vector2& ORThumb(int gamePadIndex = 0)
{
    return OGamePad(gamePadIndex)->getRightThumb();
}

inline bool OPressed(int state)
{
    return OInput->isStateDown(state);
}

inline bool OJustPressed(int state)
{
    return OInput->isStateJustDown(state);
}

inline bool OJustReleased(int state)
{
    return OInput->isStateJustUp(state);
}

inline bool OReleased(int state)
{
    return OInput->isStateUp(state);
}

inline Vector4 ORectLocalToWorld(const Vector4& local, const Vector4& parent)
{
    auto ret = local;
    ret.x *= parent.z;
    ret.y *= parent.w;
    ret.x += parent.x;
    ret.y += parent.y;
    ret.z *= parent.z;
    ret.w *= parent.w;
    return std::move(ret);
}

inline Vector4 ORectWorldToLocal(const Vector4& world, const Vector4& parent)
{
    auto ret = world;
    ret.x -= parent.x;
    ret.y -= parent.y;
    ret.x /= parent.z;
    ret.y /= parent.w;
    ret.z /= parent.z;
    ret.w /= parent.w;
    return std::move(ret);
}

inline Vector4 OUVS(OTexture* pTexture, const Rect &rect)
{
    auto texDim = pTexture->getSizef();
    return{
        rect.x / texDim.x,
        rect.y / texDim.y,
        (rect.x + rect.z) / texDim.x,
        (rect.y + rect.w) / texDim.y
    };
}

inline void OQuit()
{
    PostQuitMessage(0);
}

inline onut::UIControl* OFindUI(const std::string& name)
{
    return OUI->getChild(name);
}

/**
Synchronize back to main thread. This can also be called from the main thread. It will just be delayed until the next frame.
@param callback Function or your usual lambda
@param args arguments
*/
template<typename Tfn,
    typename ... Targs>
    inline void OSync(Tfn callback, Targs... args)
{
    extern onut::Synchronous<onut::Pool<>> g_mainSync;
    g_mainSync.sync(callback, args...);
}
