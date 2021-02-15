#include <onut/Renderer.h>
#include <onut/SpriteBatch.h>
#include <onut/PrimitiveBatch.h>
#include <onut/TiledMap.h>
#include "TiledMapRendererComponent.h"
#include "SceneManager.h"
#include "Entity.h"
#include "TransformHelper.h"
#if BARK_EDITOR
#include "Gizmo2DContext.h"
#endif

#if BARK_EDITOR
void TiledMapRendererComponent::onEnable()
{
    _2DRendererComponent::onEnable();

    enableGizmo(entity);
}
#endif

#if BARK_EDITOR
void TiledMapRendererComponent::onDisable()
{
    _2DRendererComponent::onDisable();

    disableGizmo(entity);
}
#endif

#if BARK_EDITOR
void TiledMapRendererComponent::renderGizmo(Gizmo2DContext* ctx)
{
    if (!entity->selected) return;

    const auto& transform = entity->getWorldTransform();
    Vector2 size((float)tiledMap->getWidth() * (float)tiledMap->getTileSize(),
                 (float)tiledMap->getHeight() * (float)tiledMap->getTileSize());

    ctx->drawDottedRect(transform, size, Vector2::Zero);
}
#endif

Rect TiledMapRendererComponent::getWorldRect()
{
    const auto& transform = entity->getWorldTransform();
    Vector2 size((float)tiledMap->getWidth() * (float)tiledMap->getTileSize(),
                 (float)tiledMap->getHeight() * (float)tiledMap->getTileSize());

    return TransformHelper::getWorldRect(transform, size, Vector2::Zero);
}

void TiledMapRendererComponent::render(onut::RenderStates& rs, OSpriteBatch* sb)
{
    if (!tiledMap) return;

    sb->flush();

    rs.sampleFiltering = smooth ? OFilterLinear : OFilterNearest;
    rs.blendMode = (onut::BlendMode)blendMode;

    tiledMap->setTransform(entity->getWorldTransform());
    tiledMap->render();
}
