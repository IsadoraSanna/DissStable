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
    keys = SDL_GetKeyboardState(NULL);

    //enables z buffer
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //enables the cube to be translucent by editing alpha channel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    projection = PERSPECTIVE_P;
    materialKey = -1;
}

Display::~Display() {
    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

bool Display::isClosed() {
    return m_isClosed;
}

void Display::Update(Transform &transform_camera, Transform &transform_OBB)
{
    SDL_GL_SwapWindow(m_window);

    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_KEYDOWN:
            if (keys[SDL_SCANCODE_ESCAPE])
                m_isClosed = true;           
            else if(keys[SDL_SCANCODE_LEFT] && keys[SDL_SCANCODE_B])
                transform_OBB.m_rot += glm::vec3(0.f,-0.1f,0.f);
            else if(keys[SDL_SCANCODE_UP] && keys[SDL_SCANCODE_B])
                transform_OBB.m_rot += glm::vec3(0.1f,0.f,0.f);
            else if(keys[SDL_SCANCODE_RIGHT] && keys[SDL_SCANCODE_B])
                transform_OBB.m_rot += glm::vec3(0.f,0.1f,0.f);
            else if(keys[SDL_SCANCODE_DOWN] && keys[SDL_SCANCODE_B])
                transform_OBB.m_rot += glm::vec3(-0.1f,0.f,0.f);
            else if(keys[SDL_SCANCODE_LEFT])
                transform_camera.m_rot += glm::vec3(0.f,-0.1f,0.f);
            else if (keys[SDL_SCANCODE_UP])
               transform_camera.m_rot += glm::vec3(0.1f,0.f,0.f);
            else if (keys[SDL_SCANCODE_RIGHT])
                transform_camera.m_rot += glm::vec3(0.f,0.1f,0.f);
            else if (keys[SDL_SCANCODE_DOWN])
                transform_camera.m_rot += glm::vec3(-0.1f,0.f,0.f);
            else if (keys[SDL_SCANCODE_R]){
                transform_camera.Reset();
                transform_OBB.Reset();
                materialKey = -2;
            }
            else if (keys[SDL_SCANCODE_1])
                materialKey = 0;
            else if (keys[SDL_SCANCODE_2])
                materialKey = 1;
            else if (keys[SDL_SCANCODE_3])
                materialKey = 2;
            else if (keys[SDL_SCANCODE_RETURN])
                projection = ORTHOGRAPHIC_P;
            break;    
        //case SDL_MOUSEBUTTONDOWN:
        }
    }


}

void Display::Clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
