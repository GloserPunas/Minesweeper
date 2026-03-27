#include "Button.h"

Button::Button(SDL_Rect rect,
    std::string label,
    SDL_Color bgColor,
    SDL_Color textColor)
    : rect(rect)
    , label(std::move(label))
    , bgColor(bgColor)
    , textColor(textColor)
    , state(ButtonState::NORMAL)
{}

void Button::draw(SDL_Renderer* renderer, TTF_Font* font) const {
    if (!renderer) return;

    SDL_Color bg = currentBgColor();
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_Color border = isDisabled() ? Color::BORDER : Color::BTN_BORDER;
    SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a);
    SDL_RenderDrawRect(renderer, &rect);

    if (font) drawLabel(renderer, font);
}

bool Button::handleEvent(const SDL_Event& e) {
    if (isDisabled()) return false;

    if (e.type == SDL_MOUSEMOTION) {
        bool inside = contains(e.motion.x, e.motion.y);
        if (state != ButtonState::PRESSED)
            state = inside ? ButtonState::HOVERED : ButtonState::NORMAL;
        return false;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        if (contains(e.button.x, e.button.y)) {
            state = ButtonState::PRESSED;
        }
        return false;
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        bool wasPressed = (state == ButtonState::PRESSED);
        bool inside     = contains(e.button.x, e.button.y);
        state = inside ? ButtonState::HOVERED : ButtonState::NORMAL;
        return wasPressed && inside;
    }

    return false;
}

bool Button::contains(int px, int py) const {
    return px >= rect.x && px <= rect.x + rect.w
        && py >= rect.y && py <= rect.y + rect.h;
}

void Button::setDisabled(bool disabled) {
    state = disabled ? ButtonState::DISABLED : ButtonState::NORMAL;
}

SDL_Color Button::currentBgColor() const {
    switch (state) {
        case ButtonState::HOVERED:  return Color::BTN_HOVER;
        case ButtonState::PRESSED:  return Color::BTN_BORDER;
        case ButtonState::DISABLED: {
            return SDL_Color{
                static_cast<Uint8>(bgColor.r / 2),
                static_cast<Uint8>(bgColor.g / 2),
                static_cast<Uint8>(bgColor.b / 2),
                180
            };
        }
        default: return bgColor;
    }
}

// ============================================================
// drawLabel() — private
// ============================================================

void Button::drawLabel(SDL_Renderer* renderer, TTF_Font* font) const {
    if (label.empty()) return;

    // Màu text mờ hơn khi disabled
    SDL_Color col = isDisabled()
        ? SDL_Color{ textColor.r, textColor.g, textColor.b, 120 }
        : textColor;

    SDL_Surface* surf = TTF_RenderText_Blended(font, label.c_str(), col);
    if (!surf) return;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (!tex) return;

    int tw, th;
    SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);

    SDL_Rect dst = {
        rect.x + (rect.w - tw) / 2,
        rect.y + (rect.h - th) / 2,
        tw, th
    };
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}
