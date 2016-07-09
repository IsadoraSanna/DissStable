#ifndef DISPLAY_H
#define DISPLAY_H


#include "transform.h"
#include <string>
#include <SDL2/SDL.h>
#undef main

class Display
{
public:
    Display(int width, int height, const std::string& title);
    virtual ~Display();
    void Clear(float r, float g, float b, float a);
    void Update(Transform &transform);
    bool isClosed();
    bool projection;


    enum projectionType{
        PERSPECTIVE_P,
        ORTHOGRAPHIC_P
    };

protected:
private:
    const Uint8 *keys;
    void operator=(const Display& display) {}
    Display(const Display& display) {}

    SDL_Window* m_window;
    SDL_GLContext m_glContext;
    bool m_isClosed;
    bool buttonPressed;
    float firstX;
    float firstY;
    float distanceY;
    float distanceX;
};

#endif // DISPLAY_H
