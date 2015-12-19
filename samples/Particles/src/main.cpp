// Required for WinMain
#include <Windows.h>

// Oak Nut include
#include "onut.h"

void init();
void update();
void render();

OEmitterInstance emitter;
OAnim3 position;

// Main
int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
    OSettings->setGameName("Particles Sample");
    ORun(init, update, render);
}

void init()
{
}

void update()
{
    emitter.setTransform(position);

    if (OJustPressed(OINPUT_1))
    {
        OEmitPFX("test.pfx", Vector3{OScreenWf / 3, OScreenHf / 2, 0});
    }
    if (OJustPressed(OINPUT_2))
    {
        if (emitter.isPlaying())
        {
            emitter.stop();
        }
        else
        {
            emitter = OEmitPFX("test.pex", Vector3{OScreenWf * 2 / 3, OScreenHf / 2, 0});
            emitter.setRenderEnabled(false);

            // Animate it back and forth across the screen
            position.start(Vector3{OScreenWf - 100, OScreenHf / 2, 0}, Vector3{100, OScreenHf / 2, 0}, 2.f, OLinear, OPingPongLoop);
        }
    }
}

void render()
{
    // Clear to black
    ORenderer->clear({0, 0, 0, 1});

    OSB->begin();

    // Draw info
    auto pFont = OGetBMFont("font.fnt");
    pFont->draw("Press ^9901^999 to spawn particles from PFX file", {10, 10});
    pFont->draw("Press ^9902^999 to spawn particles from PFX file", {10, 30});

    // It is possible to manually call render an emitter,
    // so we can specify in which order it is renderer manually
    emitter.render();

    pFont->draw("FPS: " + std::to_string(onut::getTimeInfo().getFPS()), {10, 50});

    OSB->end();
}
