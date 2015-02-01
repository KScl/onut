#include "DocumentView.h"
#include "styles.h"

static const Color g_panelBGColor = OColorHex(2d2d30);
static const Color g_groupOutlineColor = OColorHex(3f3f46);
static const Color g_panelTitleBGColor = OColorHex(3f3f46);
static const Color g_fontColor = OColorHex(f1f1f1);
static const Color g_fontColorFaded = OColorHex(f1f1f1) * .5f;
static const Color g_sizeHandleColor = OColorHex(999999);
static const Color g_btnStatesColors[4][2] = {
    {OColorHex(333337), OColorHex(3e3e42)},
    {OColorHex(999999), OColorHex(3e3e42)},
    {OColorHex(1c97ea), OColorHex(3e3e42)},
    {OColorHex(007acc), OColorHex(333337)}
};
static const Color g_toolBtnHoverColor = OColorHex(3e3e40);
static const Color g_toolBtnDownColor = OColorHex(007acc);
static const Color g_toolBtnDisabledColor = {.4f, .4f, .4f, .4f};
static const Color g_deepViewFill = OColorHex(252526);
static const Color g_deepViewOutline = OColorHex(3f3f46);
static const Color g_treeItemSelectedBGColor = OColorHex(3399ff);

static onut::BMFont* g_pFont;
static onut::Texture* g_pTexTreeOpen;
static onut::Texture* g_pTexTreeClose;

static onut::Texture* g_pTexIcoUIControl;
static onut::Texture* g_pTexIcoUIPanel;
static onut::Texture* g_pTexIcoUIButton;
static onut::Texture* g_pTexIcoUILabel;
static onut::Texture* g_pTexIcoUIImage;

extern DocumentView* g_pDocument;
OAnimf g_dottedLineAnim = 0.f;

void createUIStyles(onut::UIContext* pContext)
{
    g_pFont = OGetBMFont("segeo12.fnt");
    g_pTexTreeOpen = OGetTexture("viewItemOpen.png");
    g_pTexTreeClose = OGetTexture("viewItemClose.png");
    g_pTexIcoUIControl = OGetTexture("shape_handles.png");
    g_pTexIcoUIPanel = OGetTexture("shape_square.png");
    g_pTexIcoUIButton = OGetTexture("icoButton.png");
    g_pTexIcoUILabel = OGetTexture("text_dropcaps.png");
    g_pTexIcoUIImage = OGetTexture("picture.png");
    g_dottedLineAnim.start(0.f, -1.f, .5f, OLinear, OLoop);

    pContext->addStyle<onut::UIPanel>("", [](const onut::UIPanel* pPanel, const onut::sUIRect& rect)
    {
        OSB->drawRect(nullptr, onut::UI2Onut(rect), g_panelBGColor);
    });

    pContext->addStyle<onut::UIPanel>("group", [](const onut::UIPanel* pPanel, const onut::sUIRect& rect)
    {
        auto orect = onut::UI2Onut(rect);
        OSB->drawRect(nullptr, orect, g_groupOutlineColor);
        OSB->drawRect(nullptr, orect.Grow(-1), g_panelBGColor);
    });

    pContext->addStyle<onut::UITreeView>("", [](const onut::UITreeView* pPanel, const onut::sUIRect& rect)
    {
        auto orect = onut::UI2Onut(rect);
        OSB->drawRect(nullptr, orect.Grow(1), g_deepViewOutline);
        OSB->drawRect(nullptr, orect, g_deepViewFill);
    });

    pContext->addStyle<onut::UITreeViewItem>("", [pContext](const onut::UITreeViewItem* pItem, const onut::sUIRect& rect)
    {
        auto pTreeView = pItem->getTreeView();
        auto orect = onut::UI2Onut(rect);
        if (pItem->getIsSelected())
        {
            auto selectionRect = orect;
            auto treeViewWorldRect = pTreeView->getWorldRect(*pContext);
            selectionRect.x = treeViewWorldRect.position.x;
            selectionRect.z = treeViewWorldRect.size.x;
            OSB->drawRect(nullptr, selectionRect, g_treeItemSelectedBGColor);
        }
        auto expandClickWidth = pTreeView->getExpandClickWidth();

        // Expand icon
        if (!pItem->getItems().empty())
        {
            if (pItem->getIsExpanded())
            {
                OSB->drawSprite(g_pTexTreeOpen, orect.Left(expandClickWidth * .5f + 4));
            }
            else
            {
                OSB->drawSprite(g_pTexTreeClose, orect.Left(expandClickWidth * .5f + 4));
            }
        }

        const auto& text = pItem->getText();
        bool hasText = !text.empty();
        auto pControl = static_cast<onut::UIControl*>(pItem->getUserData());
        auto textPos = orect.Left(expandClickWidth + 24);
        auto textColor = g_fontColor;
        if (!pControl->isEnabled())
        {
            textColor = g_fontColorFaded;
        }

        // UI icon
        switch (pControl->getType())
        {
            case onut::eUIType::UI_CONTROL:
                OSB->drawSprite(g_pTexIcoUIControl, orect.Left(expandClickWidth + 12));
                if (!hasText)
                {
                    static const std::string uiText = "UIControl";
                    g_pFont->draw<OLeft>(uiText, textPos, textColor);
                }
                break;
            case onut::eUIType::UI_PANEL:
                OSB->drawSprite(g_pTexIcoUIPanel, orect.Left(expandClickWidth + 12));
                if (!hasText)
                {
                    static const std::string uiText = "UIPanel";
                    g_pFont->draw<OLeft>(uiText, textPos, textColor);
                }
                break;
            case onut::eUIType::UI_BUTTON:
                OSB->drawSprite(g_pTexIcoUIButton, orect.Left(expandClickWidth + 12));
                if (!hasText)
                {
                    static const std::string uiText = "UIButton";
                    g_pFont->draw<OLeft>(uiText, textPos, textColor);
                }
                break;
            case onut::eUIType::UI_LABEL:
                OSB->drawSprite(g_pTexIcoUILabel, orect.Left(expandClickWidth + 12));
                if (!hasText)
                {
                    static const std::string uiText = "UILabel";
                    g_pFont->draw<OLeft>(uiText, textPos, textColor);
                }
                break;
            case onut::eUIType::UI_IMAGE:
                OSB->drawSprite(g_pTexIcoUIImage, orect.Left(expandClickWidth + 12));
                if (!hasText)
                {
                    static const std::string uiText = "UIImage";
                    g_pFont->draw<OLeft>(uiText, textPos, textColor);
                }
                break;
        }

        // Label
        if (hasText)
        {
            g_pFont->draw<OLeft>(pItem->getText(), textPos, g_fontColor);
        }
    });

    pContext->addStyle<onut::UIPanel>("sizableRegion", [](const onut::UIPanel* pPanel, const onut::sUIRect& rect)
    {
        OSB->drawRect(nullptr, onut::UI2Onut(rect), Color::Black);
        OSB->end();

        // Render edited UIs
      //  ORenderer->set2DCamera({std::roundf(-rect.position.x), std::roundf(-rect.position.y)});
        ORenderer->set2DCamera({-rect.position.x, -rect.position.y});
        OSB->begin();
        g_pDocument->render();
        OSB->end();

        ORenderer->set2DCamera({0, 0});
        OSB->begin();
    });

    pContext->addStyle<onut::UIImage>("", [](const onut::UIImage* pImage, const onut::sUIRect& rect)
    {
        OSB->drawRect(OGetTexture(pImage->getImage().c_str()), onut::UI2Onut(rect));
    });

    pContext->addStyle<onut::UIImage>("toolBtn", [pContext](const onut::UIImage* pImage, const onut::sUIRect& rect)
    {
        auto orect = onut::UI2Onut(rect);
        auto state = pImage->getState(*pContext);
        auto pos = orect.Center();
        pos.x = std::roundf(pos.x);
        pos.y = std::roundf(pos.y);
        switch (state)
        {
            case onut::eUIState::NORMAL:
                OSB->drawSprite(OGetTexture(pImage->getImage().c_str()), pos);
                break;
            case onut::eUIState::HOVER:
                OSB->drawRect(nullptr, orect, g_toolBtnHoverColor);
                OSB->drawSprite(OGetTexture(pImage->getImage().c_str()), pos);
                break;
            case onut::eUIState::DOWN:
                OSB->drawRect(nullptr, orect, g_toolBtnDownColor);
                OSB->drawSprite(OGetTexture(pImage->getImage().c_str()), pos);
                break;
            case onut::eUIState::DISABLED:
                OSB->drawSprite(OGetTexture(pImage->getImage().c_str()), pos, g_toolBtnDisabledColor);
                break;
        }
    });
    
    pContext->addStyle<onut::UIPanel>("toolSeparator", [](const onut::UIPanel* pPanel, const onut::sUIRect& rect)
    {
        auto orect = onut::UI2Onut(rect);
        auto pos = orect.Center();
        pos.x = std::roundf(pos.x);
        pos.y = std::roundf(pos.y);
        OSB->drawSprite(OGetTexture("toolSeparator.png"), pos);
    });

    pContext->addStyle<onut::UIPanel>("view", [](const onut::UIPanel* pPanel, const onut::sUIRect& rect)
    {
    });

    pContext->addStyle<onut::UIPanel>("sizeHandle", [](const onut::UIPanel* pLabel, const onut::sUIRect& rect)
    {
        const auto orect = onut::UI2Onut(rect);
        OSB->drawRect(nullptr, orect, g_sizeHandleColor);
    });

    pContext->addStyle<onut::UILabel>("", [](const onut::UILabel* pLabel, const onut::sUIRect& rect)
    {
        g_pFont->draw<OLeft>(pLabel->getText(), onut::UI2Onut(rect).Left(), g_fontColor);
    });

    pContext->addStyle<onut::UILabel>("panelTitle", [](const onut::UILabel* pLabel, const onut::sUIRect& rect)
    {
        const auto orect = onut::UI2Onut(rect);
        OSB->drawRect(nullptr, orect, g_panelTitleBGColor);
        g_pFont->draw<OCenter>(pLabel->getText(), orect.Center(), g_fontColor);
    });

    pContext->addStyle<onut::UIButton>("", [pContext](const onut::UIButton* pButton, const onut::sUIRect& rect)
    {
        auto state = pButton->getState(*pContext);
        const auto rectOutter = onut::UI2Onut(rect);
        const auto rectInnuer = rectOutter.Grow(-1);
        switch (state)
        {
            case onut::eUIState::DISABLED:
                OSB->drawRect(nullptr, rectOutter, g_btnStatesColors[0][0]);
                OSB->drawRect(nullptr, rectInnuer, g_btnStatesColors[0][1]);
                g_pFont->draw<OCenter>(pButton->getCaption(), rectInnuer.Center(), g_fontColor * .5f);
                break;
            case onut::eUIState::NORMAL:
                OSB->drawRect(nullptr, rectOutter, g_btnStatesColors[1][0]);
                OSB->drawRect(nullptr, rectInnuer, g_btnStatesColors[1][1]);
                g_pFont->draw<OCenter>(pButton->getCaption(), rectInnuer.Center(), g_fontColor);
                break;
            case onut::eUIState::HOVER:
                OSB->drawRect(nullptr, rectOutter, g_btnStatesColors[2][0]);
                OSB->drawRect(nullptr, rectInnuer, g_btnStatesColors[2][1]);
                g_pFont->draw<OCenter>(pButton->getCaption(), rectInnuer.Center(), g_fontColor);
                break;
            case onut::eUIState::DOWN:
                OSB->drawRect(nullptr, rectOutter, g_btnStatesColors[3][0]);
                OSB->drawRect(nullptr, rectInnuer, g_btnStatesColors[3][1]);
                g_pFont->draw<OCenter>(pButton->getCaption(), rectInnuer.Center() + Vector2(1, 1), g_fontColor);
                break;
        }
    });

    pContext->addStyle<onut::UIButton>("imgButton", [pContext](const onut::UIButton* pButton, const onut::sUIRect& rect)
    {
        auto state = pButton->getState(*pContext);
        const auto rectOutter = onut::UI2Onut(rect);
        const auto rectInnuer = rectOutter.Grow(-1);
        auto textRect = rectInnuer;
        textRect.x += 24;
        textRect.z -= 24;
        switch (state)
        {
            case onut::eUIState::DISABLED:
                OSB->drawRect(nullptr, rectOutter, g_btnStatesColors[0][0]);
                OSB->drawRect(nullptr, rectInnuer, g_btnStatesColors[0][1]);
                g_pFont->draw<OLeft>(pButton->getCaption(), textRect.Left(), g_fontColor * .5f);
                break;
            case onut::eUIState::NORMAL:
                OSB->drawRect(nullptr, rectOutter, g_btnStatesColors[1][0]);
                OSB->drawRect(nullptr, rectInnuer, g_btnStatesColors[1][1]);
                g_pFont->draw<OLeft>(pButton->getCaption(), textRect.Left(), g_fontColor);
                break;
            case onut::eUIState::HOVER:
                OSB->drawRect(nullptr, rectOutter, g_btnStatesColors[2][0]);
                OSB->drawRect(nullptr, rectInnuer, g_btnStatesColors[2][1]);
                g_pFont->draw<OLeft>(pButton->getCaption(), textRect.Left(), g_fontColor);
                break;
            case onut::eUIState::DOWN:
                OSB->drawRect(nullptr, rectOutter, g_btnStatesColors[3][0]);
                OSB->drawRect(nullptr, rectInnuer, g_btnStatesColors[3][1]);
                g_pFont->draw<OLeft>(pButton->getCaption(), textRect.Left() + Vector2(1, 1), g_fontColor);
                break;
        }
    });

    pContext->addStyle<onut::UIPanel>("gizmo", [](const onut::UIPanel* pPanel, const onut::sUIRect& rect)
    {
        const Color DOTTED_LINE_COLOR = {1, 1, 1, .5f};

        auto pDottedLineTexture = OGetTexture("dottedLine.png");
        auto dottedLineScale = 1.f / pDottedLineTexture->getSizef().x;
        auto dottedLineOffset = g_dottedLineAnim.get();

        OSB->drawRectWithUVs(pDottedLineTexture,
        {rect.position.x, rect.position.y, 1, rect.size.y},
        {dottedLineOffset, dottedLineOffset, dottedLineOffset + dottedLineScale, dottedLineOffset + rect.size.y * dottedLineScale},
        DOTTED_LINE_COLOR);

        OSB->drawRectWithUVs(pDottedLineTexture,
        {rect.position.x + rect.size.x - 1, rect.position.y, 1, rect.size.y},
        {dottedLineOffset, dottedLineOffset, dottedLineOffset + dottedLineScale, dottedLineOffset + rect.size.y * dottedLineScale},
        DOTTED_LINE_COLOR);

        OSB->drawRectWithUVs(pDottedLineTexture,
        {rect.position.x, rect.position.y, rect.size.x, 1},
        {dottedLineOffset, dottedLineOffset, dottedLineOffset + rect.size.x * dottedLineScale, dottedLineOffset + dottedLineScale},
        DOTTED_LINE_COLOR);

        OSB->drawRectWithUVs(pDottedLineTexture,
        {rect.position.x, rect.position.y + rect.size.y - 1, rect.size.x, 1},
        {dottedLineOffset, dottedLineOffset, dottedLineOffset + rect.size.x * dottedLineScale, dottedLineOffset + dottedLineScale},
        DOTTED_LINE_COLOR);
    });

    pContext->addStyle<onut::UICheckBox>("", [pContext](const onut::UICheckBox* pCheckBox, const onut::sUIRect& rect)
    {
        auto state = pCheckBox->getState(*pContext);
        const auto rectOutter = onut::UI2Onut(rect);
        auto rectChk = rectOutter;
        rectChk.x += 4;
        rectChk.y = std::floorf(rectChk.y + rectChk.w / 2 - 6);
        rectChk.z = 12;
        rectChk.w = 12;
        switch (state)
        {
            case onut::eUIState::DISABLED:
                OSB->drawRect(nullptr, rectChk, g_btnStatesColors[0][0]);
                OSB->drawRect(nullptr, rectChk.Grow(-1), g_btnStatesColors[0][1]);
                g_pFont->draw<OLeft>(pCheckBox->getCaption(), rectOutter.Left(20), g_fontColor * .5f);
                break;
            case onut::eUIState::NORMAL:
                OSB->drawRect(nullptr, rectChk, g_btnStatesColors[1][0]);
                OSB->drawRect(nullptr, rectChk.Grow(-1), g_btnStatesColors[1][1]);
                g_pFont->draw<OLeft>(pCheckBox->getCaption(), rectOutter.Left(20), g_fontColor);
                break;
            case onut::eUIState::HOVER:
                OSB->drawRect(nullptr, rectChk, g_btnStatesColors[2][0]);
                OSB->drawRect(nullptr, rectChk.Grow(-1), g_btnStatesColors[2][1]);
                g_pFont->draw<OLeft>(pCheckBox->getCaption(), rectOutter.Left(20), g_fontColor);
                break;
            case onut::eUIState::DOWN:
                OSB->drawRect(nullptr, rectChk, g_btnStatesColors[3][0]);
                OSB->drawRect(nullptr, rectChk.Grow(-1), g_btnStatesColors[3][1]);
                g_pFont->draw<OLeft>(pCheckBox->getCaption(), rectOutter.Left(20) + Vector2(1, 1), g_fontColor);
                break;
        }
        if (pCheckBox->getIsChecked())
        {
            OSB->drawRect(nullptr, rectChk.Grow(-2), g_fontColor);
        }
    });

    pContext->addStyle<onut::UICheckBox>("align", [pContext](const onut::UICheckBox* pCheckBox, const onut::sUIRect& rect)
    {
        const auto orect = onut::UI2Onut(rect);
        auto texture = OGetTexture("align.png");
        Vector4 UVs;
        switch (pCheckBox->getAlign())
        {
            case onut::eUIAlign::TOP_LEFT:
                UVs.x = 0.f;
                UVs.z = 13.f / texture->getSizef().x;
                UVs.y = 0.f;
                UVs.w = 13.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::TOP:
                UVs.x = 13.f / texture->getSizef().x;
                UVs.z = 26.f / texture->getSizef().x;
                UVs.y = 0.f;
                UVs.w = 13.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::TOP_RIGHT:
                UVs.x = 26.f / texture->getSizef().x;
                UVs.z = 39.f / texture->getSizef().x;
                UVs.y = 0.f;
                UVs.w = 13.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::LEFT:
                UVs.x = 0.f;
                UVs.z = 13.f / texture->getSizef().x;
                UVs.y = 13.f / texture->getSizef().y;
                UVs.w = 26.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::CENTER:    
                UVs.x = 13.f / texture->getSizef().x;
                UVs.z = 26.f / texture->getSizef().x;
                UVs.y = 13.f / texture->getSizef().y;
                UVs.w = 26.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::RIGHT:
                UVs.x = 26.f / texture->getSizef().x;
                UVs.z = 39.f / texture->getSizef().x;
                UVs.y = 13.f / texture->getSizef().y;
                UVs.w = 26.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::BOTTOM_LEFT:
                UVs.x = 0.f;
                UVs.z = 13.f / texture->getSizef().x;
                UVs.y = 26.f / texture->getSizef().y;
                UVs.w = 39.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::BOTTOM:
                UVs.x = 13.f / texture->getSizef().x;
                UVs.z = 26.f / texture->getSizef().x;
                UVs.y = 26.f / texture->getSizef().y;
                UVs.w = 39.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::BOTTOM_RIGHT:
                UVs.x = 26.f / texture->getSizef().x;
                UVs.z = 39.f / texture->getSizef().x;
                UVs.y = 26.f / texture->getSizef().y;
                UVs.w = 39.f / texture->getSizef().y;
                break;
        }
        if (pCheckBox->getIsChecked())
        {
            UVs.x += 39.f / texture->getSizef().x;
            UVs.z += 39.f / texture->getSizef().x;
            UVs.y += 39.f / texture->getSizef().y;
            UVs.w += 39.f / texture->getSizef().y;
        }
        else
        {
            auto state = pCheckBox->getState(*pContext);
            switch (state)
            {
                case onut::eUIState::HOVER:
                    UVs.x += 39.f / texture->getSizef().x;
                    UVs.z += 39.f / texture->getSizef().x;
                    break;
                case onut::eUIState::DOWN:
                    UVs.y += 39.f / texture->getSizef().y;
                    UVs.w += 39.f / texture->getSizef().y;
                    break;
            }
        }
        OSB->drawRectWithUVs(texture, orect, UVs);
    });

    pContext->addStyle<onut::UIButton>("align", [pContext](const onut::UIButton* pButton, const onut::sUIRect& rect)
    {
        const auto orect = onut::UI2Onut(rect);
        auto texture = OGetTexture("align.png");
        Vector4 UVs;
        switch (pButton->getAlign())
        {
            case onut::eUIAlign::TOP_LEFT:
                UVs.x = 0.f;
                UVs.z = 13.f / texture->getSizef().x;
                UVs.y = 0.f;
                UVs.w = 13.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::TOP:
                UVs.x = 13.f / texture->getSizef().x;
                UVs.z = 26.f / texture->getSizef().x;
                UVs.y = 0.f;
                UVs.w = 13.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::TOP_RIGHT:
                UVs.x = 26.f / texture->getSizef().x;
                UVs.z = 39.f / texture->getSizef().x;
                UVs.y = 0.f;
                UVs.w = 13.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::LEFT:
                UVs.x = 0.f;
                UVs.z = 13.f / texture->getSizef().x;
                UVs.y = 13.f / texture->getSizef().y;
                UVs.w = 26.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::CENTER:    
                UVs.x = 13.f / texture->getSizef().x;
                UVs.z = 26.f / texture->getSizef().x;
                UVs.y = 13.f / texture->getSizef().y;
                UVs.w = 26.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::RIGHT:
                UVs.x = 26.f / texture->getSizef().x;
                UVs.z = 39.f / texture->getSizef().x;
                UVs.y = 13.f / texture->getSizef().y;
                UVs.w = 26.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::BOTTOM_LEFT:
                UVs.x = 0.f;
                UVs.z = 13.f / texture->getSizef().x;
                UVs.y = 26.f / texture->getSizef().y;
                UVs.w = 39.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::BOTTOM:
                UVs.x = 13.f / texture->getSizef().x;
                UVs.z = 26.f / texture->getSizef().x;
                UVs.y = 26.f / texture->getSizef().y;
                UVs.w = 39.f / texture->getSizef().y;
                break;
            case onut::eUIAlign::BOTTOM_RIGHT:
                UVs.x = 26.f / texture->getSizef().x;
                UVs.z = 39.f / texture->getSizef().x;
                UVs.y = 26.f / texture->getSizef().y;
                UVs.w = 39.f / texture->getSizef().y;
                break;
        }
        auto state = pButton->getState(*pContext);
        switch (state)
        {
            case onut::eUIState::HOVER:
                UVs.x += 39.f / texture->getSizef().x;
                UVs.z += 39.f / texture->getSizef().x;
                break;
            case onut::eUIState::DOWN:
                UVs.y += 39.f / texture->getSizef().y;
                UVs.w += 39.f / texture->getSizef().y;
                break;
        }
        OSB->drawRectWithUVs(texture, orect, UVs);
    });
}
