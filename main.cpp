#include <iostream>
#include <GL/glew.h>
#include <opencv2/opencv.hpp>
#include <cv.h>
#include <highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <stdio.h>
#include <stdlib.h>


#include "display.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"
#include "transform.h"


static const int DISPLAY_WIDTH = 800;
static const int DISPLAY_HEIGHT = 600;
static const float ASPECT = (float)DISPLAY_WIDTH/(float)DISPLAY_HEIGHT;
static const int RESOLUTION = 100;
static float edgeSize;

Mesh* createOBB(float radius, glm::vec3 centre);
cv::Mat rotate(cv::Mat img);
void saveProjection(cv::Mat& img, int index);

int main(int argc, char *argv[])
{

    cv::Mat img(DISPLAY_HEIGHT, DISPLAY_WIDTH, CV_8UC3); //8byte unsigned int 3 channels

    Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "Mesh visualiser");

    Mesh mesh("/home/isa/Desktop/UIwithoutQT/Meshes/teapot.obj");
    Mesh *cubeMesh = createOBB(mesh.radius, mesh.centre);

    Shader mainShader("/home/isa/Desktop/UIwithoutQT/Shader/basicShader");
    Shader cubeShader("/home/isa/Desktop/UIwithoutQT/Shader/cubeShader");

    float view_distance = mesh.radius * 4.f;
    Camera camera(glm::vec3(mesh.centre.x, mesh.centre.y, -view_distance), mesh, 70.0f, ASPECT, 1.0f, 1000.0f, Camera::projectionType::PERSPECTIVE_P);
    Transform transform = Transform();

    float counter = 0.0f;

    while (!display.isClosed()) {

        //PERSPECTVE PROJECTION
        if(display.projection == Display::projectionType::PERSPECTIVE_P)
        {
            display.Clear(0.0f, 0.15f, 0.3f, 1.0f);

            float sinCounter = sinf(counter);
            float cosCounter = cosf(counter);

            /*transform.GetPos().x = sinCounter;*/
            //transform.GetRot().y = counter*50;
            //transform.SetScale(glm::vec3(cosCounter, cosCounter, cosCounter));

            mainShader.Bind();
            mainShader.Update(transform, camera);
            mesh.Draw();

            cubeShader.Bind();
            cubeShader.Update(transform, camera);
            cubeMesh->Draw();
        }
        //ORTHOGRAPHIC PROJECTION
        else
        {
            Shader mainShader("/home/isa/Desktop/UIwithoutQT/Shader/silhouetteShader");
            glm::vec3 pos;
            for (int i = 1; i<=3; i++){

                display.Clear(1.0f, 1.0f, 1.0f, 1.0f);

                switch(i){
                case 1:
                    pos = glm::vec3(mesh.centre.x, mesh.centre.y, -view_distance);
                    break;
                case 2:
                    pos = glm::vec3(view_distance, mesh.centre.y, mesh.centre.z);
                    break;
                case 3:
                    pos = glm::vec3(mesh.centre.x, ASPECT*view_distance, mesh.centre.z);
                    break;
                default:
                    return -1;
                }
                camera = Camera(pos, mesh, 70.0f, ASPECT, 1.0f, 1000.0f, Camera::projectionType::PERSPECTIVE_P);

                mainShader.Bind();
                mainShader.Update(transform, camera);
                mesh.Draw();
                saveProjection(img, i);

            }

            display.projection = Display::projectionType::PERSPECTIVE_P;

            mainShader = Shader("/home/isa/Desktop/UIwithoutQT/Shader/basicShader");
            camera = Camera(glm::vec3(mesh.centre.x, mesh.centre.y, -view_distance), mesh, 70.0f, ASPECT, 1.0f, 1000.0f, Camera::projectionType::PERSPECTIVE_P);

        }

        display.Update(transform);
        //counter += 0.00001;
    }

    return 0;
}

void saveProjection(cv::Mat& img, int index){
    std::string path = std::string("/home/isa/Desktop/UIwithoutQT/Silhouette/silhouette")+ std::to_string(index) + ".jpg";

    //3*sizeof(unsigned integer)
    glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize());
    glReadPixels(0, 0, DISPLAY_WIDTH ,DISPLAY_HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, img.data);

    img = rotate(img);
    imwrite(path, img);
}

cv::Mat rotate(cv::Mat img)
{
    cv::Mat tempImg;
    cv::Point2f pt((float)DISPLAY_WIDTH/2,(float)DISPLAY_HEIGHT/2);
    cv::Mat r = getRotationMatrix2D(pt, 180, 1);
    warpAffine(img, tempImg, r, cv::Size(img.cols, img.rows));
    cv::flip(tempImg, tempImg, 1);
    return tempImg;
}

Mesh* createOBB(float radius, glm::vec3 centre) {
    edgeSize = sqrt(pow(((centre.x - radius)-(centre.x - radius)),2) + pow(((centre.y - radius)-(centre.y + radius)),2) + pow(((centre.z - radius)-(centre.z - radius)),2));

    Vertex vertices[] =
    {

        //back & front
        Vertex(glm::vec3(centre.x - radius, centre.y + radius, centre.z - radius), glm::vec3(0, 0, -1)),
        Vertex(glm::vec3(centre.x + radius, centre.y + radius, centre.z - radius), glm::vec3(0, 0, -1)),
        Vertex(glm::vec3(centre.x + radius, centre.y - radius, centre.z - radius), glm::vec3(0, 0, -1)),
        Vertex(glm::vec3(centre.x - radius, centre.y - radius, centre.z - radius), glm::vec3(0, 0, -1)),

        Vertex(glm::vec3(centre.x - radius, centre.y + radius, centre.z + radius), glm::vec3(0, 0, 1)),
        Vertex(glm::vec3(centre.x + radius, centre.y + radius, centre.z + radius), glm::vec3(0, 0, 1)),
        Vertex(glm::vec3(centre.x + radius, centre.y - radius, centre.z + radius), glm::vec3(0, 0, 1)),
        Vertex(glm::vec3(centre.x - radius, centre.y - radius, centre.z + radius), glm::vec3(0, 0, 1)),

        //bottom & top
        Vertex(glm::vec3(centre.x - radius, centre.y - radius, centre.z + radius), glm::vec3(0, -1, 0)),
        Vertex(glm::vec3(centre.x + radius, centre.y - radius, centre.z + radius), glm::vec3(0, -1, 0)),
        Vertex(glm::vec3(centre.x + radius, centre.y - radius, centre.z - radius), glm::vec3(0, -1, 0)),
        Vertex(glm::vec3(centre.x - radius, centre.y - radius, centre.z - radius), glm::vec3(0, -1, 0)),

        Vertex(glm::vec3(centre.x - radius, centre.y + radius, centre.z + radius), glm::vec3(0, 1, 0)),
        Vertex(glm::vec3(centre.x + radius, centre.y + radius, centre.z + radius), glm::vec3(0, 1, 0)),
        Vertex(glm::vec3(centre.x + radius, centre.y + radius, centre.z - radius), glm::vec3(0, 1, 0)),
        Vertex(glm::vec3(centre.x - radius, centre.y + radius, centre.z - radius), glm::vec3(0, 1, 0)),

        //right & left
        Vertex(glm::vec3(centre.x + radius, centre.y + radius, centre.z - radius), glm::vec3(1, 0, 0)),
        Vertex(glm::vec3(centre.x + radius, centre.y + radius, centre.z + radius), glm::vec3(1, 0, 0)),
        Vertex(glm::vec3(centre.x + radius, centre.y - radius, centre.z + radius), glm::vec3(1, 0, 0)),
        Vertex(glm::vec3(centre.x + radius, centre.y - radius, centre.z - radius), glm::vec3(1, 0, 0)),

        Vertex(glm::vec3(centre.x - radius, centre.y + radius, centre.z - radius), glm::vec3(-1, 0, 0)),
        Vertex(glm::vec3(centre.x - radius, centre.y + radius, centre.z + radius), glm::vec3(-1, 0, 0)),
        Vertex(glm::vec3(centre.x - radius, centre.y - radius, centre.z + radius), glm::vec3(-1, 0, 0)),
        Vertex(glm::vec3(centre.x - radius, centre.y - radius, centre.z - radius), glm::vec3(-1, 0, 0)),

    };

    unsigned int indices[] = { 0, 1, 2,
        0, 2, 3,

        6, 5, 4,
        7, 6, 4,

        10, 9, 8,
        11, 10, 8,

        12, 13, 14,
        12, 14, 15,

        16, 17, 18,
        16, 18, 19,

        22, 21, 20,
        23, 22, 20
    };
    return new Mesh(vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0]));
}
