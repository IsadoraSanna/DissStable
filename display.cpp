#include "display.h"

#include <iostream>

#include <GL/glew.h>

Display::Display(int width, int height, const std::string& title) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    m_glContext = SDL_GL_CreateContext(m_window);

    //Initialize GLEW and openGL
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        std::cerr << "Glew failed to initialize!" << std::endl;
    }

    m_isClosed = false;
    buttonPressed = false;
    keys = SDL_GetKeyboardState(NULL);

    distanceY = 0;
    distanceX = 0;

    //enable the use of z buffer
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //enables the cube to be transparent by editing alpha channel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    projection = PERSPECTIVE_P;
}

Display::~Display() {
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

bool Display::isClosed() {
    return m_isClosed;
}

void Display::Update(Transform &transform)
{
    SDL_GL_SwapWindow(m_window);

    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_KEYDOWN:
            if (keys[SDL_SCANCODE_ESCAPE]) {
                std::cout << "ESC" << std::endl;
                m_isClosed = true;
            }
            else if(keys[SDL_SCANCODE_LEFT])
                transform.m_rot += glm::vec3(0.f,-0.1f,0.f);
            else if (keys[SDL_SCANCODE_UP])
               transform.m_rot += glm::vec3(0.1f,0.f,0.f);
            else if (keys[SDL_SCANCODE_RIGHT])
                transform.m_rot += glm::vec3(0.f,0.1f,0.f);
            else if (keys[SDL_SCANCODE_DOWN])
                transform.m_rot += glm::vec3(-0.1f,0.f,0.f);
            else if (keys[SDL_SCANCODE_RETURN])
                projection = ORTHOGRAPHIC_P;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) {
                //if it's the first time
                    firstX = e.motion.x;
                    firstY = e.motion.y;
                    buttonPressed = true;
                    std::cout << "prima pressione" << std::endl;

            }
            break;
        case SDL_MOUSEMOTION:
            if (buttonPressed) {
                std::cout << "trascino bottone premuto" << distanceY << ".. " << std::endl;
                distanceX = (e.motion.x - firstX);
                distanceY = (e.motion.y - firstY);
                transform.m_rot += glm::vec3(0.f,distanceY/100,0.f);
                //transform.m_rot += glm::vec3(distanceX/100,0.f,0.f);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            std::cout << "rilascio" << std::endl;
            buttonPressed = false;
            distanceX = 0;
            distanceY = 0;
            break;
        }
    }


}

void Display::Clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
