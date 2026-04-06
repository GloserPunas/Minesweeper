#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "Constants.h"

enum class ButtonState {
    NORMAL,
    HOVERED,
    PRESSED,
    DISABLED,
};

class Button {
public:
    Button() = default;

    Button(SDL_Rect rect,
        std::string label,
        SDL_Color bgColor = Color::BTN_NORMAL,
        SDL_Color textColor = Color::BLACK);

    void draw(SDL_Renderer* renderer, TTF_Font* font) const;

    bool handleEvent(const SDL_Event& e);

    bool contains(int px, int py) const;

    void setLabel (const std::string& label) { this->label = label; }
    void setDisabled(bool disabled);
    void setRect (SDL_Rect r) { rect = r; }

    const std::string& getLabel() const { return label; }
    SDL_Rect getRect () const { return rect;  }
    ButtonState getState() const { return state; }
    bool isDisabled() const { return state == ButtonState::DISABLED; }

private:
    SDL_Rect rect = {0, 0, 0, 0};
    std::string label;
    SDL_Color bgColor = Color::BTN_NORMAL;
    SDL_Color textColor = Color::BLACK;
    ButtonState state = ButtonState::NORMAL;

    SDL_Color currentBgColor() const;

    void drawLabel(SDL_Renderer* renderer, TTF_Font* font) const;
};
