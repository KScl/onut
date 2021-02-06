#include <onut/Input.h>
#include <onut/Font.h>
#include <onut/Renderer.h>
#include <onut/SpriteBatch.h>
#include <onut/Window.h>
#include "GUIContext.h"
#include "globals.h"
#include "Theme.h"

void GUIContext::update()
{
    mouse = oInput->mousePosf;
    ctrl    = OInputPressed(OKeyLeftControl);
    shift   = OInputPressed(OKeyLeftShift);
    alt     = OInputPressed(OKeyLeftAlt);
    down    = OInputPressed(OMouse1);
    clicked = OInputJustReleased(OMouse1);

    if (OInputJustPressed(OMouse1)) down_pos = mouse;
}

void GUIContext::begin()
{
    theme               = g_theme;
    sb                  = oSpriteBatch.get();
    rect                = ORectFullScreen.Grow(-theme->panel_margin);
    r                   = oRenderer.get();
    rs                  = &r->renderStates;
    drag.type           = eUIDrag::None;
    saved_draw_point    = 0;
    cursor_type         = eUICursorType::Arrow;

    draw_calls.clear();
    rect_stack.clear();
}

void GUIContext::end()
{
    sb->begin();

    for (const auto& draw_call : draw_calls)
    {
        drawDrawCall(draw_call);
    }

    if (drag.type != eUIDrag::None)
    {
        auto diff = mouse - down_pos;
        if (drag.type == eUIDrag::HSplit) diff.y = 0.0f;
        if (drag.type == eUIDrag::VSplit) diff.x = 0.0f;
        for (auto& draw_call : drag.draw_calls)
        {
            draw_call.color *= 0.65f;
            draw_call.rect.x += diff.x;
            draw_call.rect.y += diff.y;
            draw_call.pos += diff;
            drawDrawCall(draw_call);
        }
    }

    sb->end();

    switch (cursor_type)
    {
        case eUICursorType::Arrow:      oInput->unsetMouseIcon();                           break;
        case eUICursorType::SizeEW:     oInput->setMouseIcon("cur_size_ew.png", {9, 9});    break;
        case eUICursorType::SizeNS:     oInput->setMouseIcon("cur_size_ns.png", {9, 9});    break;
        case eUICursorType::SizeAll:    oInput->setMouseIcon("cur_size_all.png", {9, 9});   break;
    }
}

void GUIContext::drawDrawCall(const UIDrawCall& draw_call)
{
    switch (draw_call.type)
    {
        case eUIDrawCall::Outline:
            sb->drawInnerOutlineRect(draw_call.rect, draw_call.thickness, draw_call.color);
            break;
        case eUIDrawCall::Rect:
            sb->drawRect(draw_call.texture, draw_call.rect, draw_call.color);
            break;
        case eUIDrawCall::Text:
            sb->drawText(draw_call.font, draw_call.text, draw_call.pos, draw_call.origin, draw_call.color);
            break;
        case eUIDrawCall::Sprite:
            sb->drawSprite(draw_call.texture, draw_call.pos, draw_call.color, draw_call.origin);
            break;
        case eUIDrawCall::Slice9:
            sb->drawRectScaled9(draw_call.texture, draw_call.rect, draw_call.padding, draw_call.color);
            break;
    }
}

void GUIContext::pushRect()
{
    rect_stack.push_back(rect);
}

Rect GUIContext::popRect()
{
    if (rect_stack.empty()) return rect;

    rect = rect_stack.back();
    rect_stack.pop_back();

    return rect;
}

void GUIContext::saveDrawPoint()
{
    saved_draw_point = draw_calls.size();
}

void GUIContext::rewindDrawCalls()
{
    draw_calls.resize(saved_draw_point);
}

UIDrawCall* GUIContext::getNextDrawCall()
{
    draw_calls.push_back({});
    return &draw_calls.back();
}

void GUIContext::drawOutline(const Rect& rect, float thickness, const Color& color)
{
    auto draw_call = getNextDrawCall();
    draw_call->type = eUIDrawCall::Outline;
    draw_call->thickness = thickness;
    draw_call->color = color;
    draw_call->rect = rect;
}

void GUIContext::drawRect(const OTextureRef& texture, const Rect& rect, const Color& color)
{
    auto draw_call = getNextDrawCall();
    draw_call->texture = texture;
    draw_call->type = eUIDrawCall::Rect;
    draw_call->rect = rect;
    draw_call->color = color;
}

void GUIContext::drawText(const OFontRef& font, const std::string& text, const Vector2& pos, const Vector2& origin, const Color& color)
{
    auto draw_call = getNextDrawCall();
    draw_call->type = eUIDrawCall::Text;
    draw_call->font = font;
    draw_call->text = text;
    draw_call->pos = pos;
    draw_call->origin = origin;
    draw_call->color = color;
}

void GUIContext::drawSprite(const OTextureRef& texture, const Vector2& pos, const Color& color, const Vector2& origin)
{
    auto draw_call = getNextDrawCall();
    draw_call->type = eUIDrawCall::Sprite;
    draw_call->texture = texture;
    draw_call->pos = pos;
    draw_call->color = color;
    draw_call->origin = origin;
}

void GUIContext::drawSlice9(const OTextureRef& texture, const Rect& rect, const Vector4& padding, const Color& color)
{
    auto draw_call = getNextDrawCall();
    draw_call->type = eUIDrawCall::Slice9;
    draw_call->texture = texture;
    draw_call->rect = rect;
    draw_call->padding = padding;
    draw_call->color = color;
}

void GUIContext::drawPanel()
{
    drawOutline(rect, theme->border_size, theme->panel_border_color);
    drawRect(nullptr, rect.Grow(-theme->border_size), theme->panel_color);
}

eUIState GUIContext::drawHSplitHandle()
{
    auto state = getState(rect);
    if (state != eUIState::None)
    {
        drawSlice9(theme->scrollbar, Rect(rect.x, rect.y + rect.w * 0.4f, rect.z, rect.w * 0.2f), Vector4(4), theme->disabled_text_color);
        cursor_type = eUICursorType::SizeEW;
    }

    if (state == eUIState::Drag)
    {
        drag.type = eUIDrag::HSplit;
        drag.draw_calls.clear();
    }

    return state;
}

eUIState GUIContext::drawVSplitHandle()
{
    auto state = getState(rect);
    if (state != eUIState::None)
    {
        drawSlice9(theme->scrollbar, Rect(rect.x + rect.z * 0.4f, rect.y, rect.z * 0.2f, rect.w), Vector4(4), theme->disabled_text_color);
        cursor_type = eUICursorType::SizeNS;
    }

    if (state == eUIState::Drag)
    {
        drag.type = eUIDrag::VSplit;
        drag.draw_calls.clear();
    }

    return state;
}

eUIState GUIContext::drawTab(const std::string& name, float& offset, 
                             const Color& color, const Color& border_color,
                             const Color& text_colors, bool close_btn)
{
    auto tab_draw_point = draw_calls.size();

    auto text_size = theme->font->measure(name);
    auto width = text_size.x + theme->tab_padding * 2.0f;
    auto full_width = width + (close_btn ? (16.0f + theme->tab_padding) : 0.0f);

    auto tab_rect = rect;
    tab_rect.x += offset;
    tab_rect.z = full_width;
    tab_rect.w++;
    drawRect(nullptr, tab_rect, border_color);

    tab_rect = tab_rect.Grow(-theme->border_size);
    tab_rect.w += theme->border_size;
    drawRect(nullptr, tab_rect, color);

    auto state = getState(tab_rect);
    auto text_rect = tab_rect;
    text_rect.z = width - theme->border_size * 2.0f;
    drawText(theme->font, name, text_rect.Center(), OCenter, text_colors);

    offset += full_width + theme->tab_spacing;

    if (close_btn)
    {
        if (drawToolButton(theme->x_icon, tab_rect.Right() - Vector2(16.0f + theme->tab_padding, 7.0f)))
        {
            clicked = false;
            return eUIState::Close;
        }
    }

    if (state == eUIState::Drag)
    {
        drag.type = eUIDrag::Tab;
        drag.draw_calls.assign(draw_calls.begin() + tab_draw_point, draw_calls.end());
    }

    return state;
}

eUIState GUIContext::drawActiveTab(const std::string& name, float& offset, bool close_btn)
{
    return drawTab(name, offset, theme->panel_color, theme->panel_border_color, theme->text_color, close_btn);
}

eUIState GUIContext::drawInactiveTab(const std::string& name, float& offset, bool close_btn)
{
    return drawTab(name, offset, theme->inactive_tab_color, theme->window_color, theme->disabled_text_color, close_btn);
}

void GUIContext::drawArea()
{
    drawOutline(rect, theme->border_size, theme->area_border_color);
    drawRect(nullptr, rect.Grow(-theme->border_size), theme->area_color);
}

eUIState GUIContext::getState(const Rect& rect)
{
    eUIState ret = eUIState::None;
    bool is_mouse_hover = rect.Contains(mouse);

    if (clicked && rect.Contains(down_pos))
    {
        if (is_mouse_hover)
        {
            ret = eUIState::Clicked;
        }
        else
        {
            ret = eUIState::Drop;
        }
    }
    else if (down)
    {
        if (rect.Contains(down_pos))
        {
            if (is_mouse_hover)
            {
                ret = eUIState::Down;
            }
            else if (Vector2::Distance(down_pos, mouse) > 3.0f)
            {
                ret = eUIState::Drag;
            }
            else
            {
                ret = eUIState::Hover;
            }
        }
    }
    else if (is_mouse_hover)
    {
        ret = eUIState::Hover;
    }

    return ret;
}

const Color& GUIContext::colorForState(eUIState state, Color** colors)
{
    switch (state)
    {
        case eUIState::Hover: return *colors[1]; 
        case eUIState::Down:  return *colors[2]; 
        default:              return *colors[0];
    }
}

bool GUIContext::drawToolButton(const OTextureRef& icon, const Vector2& pos)
{
    Rect btn_rect(pos, theme->tool_button_size, theme->tool_button_size);
    auto state = getState(btn_rect);

    drawSprite(icon, pos, colorForState(state, theme->tool_button_colors), OTopLeft);

    return state == eUIState::Clicked;
}
