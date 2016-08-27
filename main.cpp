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
#include "db_elements.h"
#include "similarityevaluator.h"


static const int DISPLAY_WIDTH = 800;
static const int DISPLAY_HEIGHT = 600;
static const float ASPECT = (float)DISPLAY_WIDTH/(float)DISPLAY_HEIGHT;
static const int RESOLUTION = 100;
static float edgeSize;

const int CANDIDATE_POINTS = 64;

Mesh* createOBB(float radius, glm::vec3 centrex);
cv::Mat rotate(cv::Mat img);
void saveProjection(cv::Mat& img, int index);

int main(int argc, char *argv[])
{
    //Prepare the DB
    DB_elements dbElements = DB_elements("/home/isa/Desktop/UIwithoutQT/DB/db.txt", CANDIDATE_POINTS);
    SimilarityEvaluator evaluator = SimilarityEvaluator(dbElements, CANDIDATE_POINTS);
    evaluator.getDBFourierDescriptors();

    //Init screen, mesh and shaders
    cv::Mat img(DISPLAY_HEIGHT, DISPLAY_WIDTH, CV_8UC3); //8byte unsigned int 3 channels

    Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "Mesh visualiser");

    Mesh mesh("/home/isa/Desktop/UIwithoutQT/Meshes/pillow.obj");
    Mesh *cubeMesh = createOBB(mesh.radius, mesh.centre);

    Shader mainShader("/home/isa/Desktop/UIwithoutQT/Shader/basicShader");
    Shader cubeShader("/home/isa/Desktop/UIwithoutQT/Shader/cubeShader");

    Camera camera(mesh, 70.0f, ASPECT, 1.0f, 1000.0f);
    //transform for the camera
    Transform transform_camera = Transform();
    //transform for the OBB
    Transform transform_OBB = Transform();
    //transform for the Mesh
    Transform transform_mesh = Transform();
    transform_mesh.m_pos = (glm::vec3(-mesh.centre.x,-mesh.centre.y,-mesh.centre.z));

    while (!display.isClosed()) {

        //PERSPECTVE PROJECTION
        if(display.projection == Display::projectionType::PERSPECTIVE_P)
        {
            display.Clear(0.0f, 0.15f, 0.3f, 1.0f);

            mainShader.Bind();
            mainShader.Update(transform_camera, camera, transform_mesh);
            mesh.Draw();

            cubeShader.Bind();
            cubeShader.UpdateWOBB(transform_camera, camera, transform_OBB);
            cubeMesh->Draw();

        }

        //ORTHOGRAPHIC PROJECTION
        else
        {
            Shader mainShader("/home/isa/Desktop/UIwithoutQT/Shader/silhouetteShader");
            glm::vec4 pos_transform, up_transform;
            glm::vec3 pos, up;
            float view_distance = mesh.radius + 1.f;


            for (int i = 1; i<=3; i++){

                display.Clear(1.0f, 1.0f, 1.0f, 1.0f);

                switch(i){
                case 1:
                {
                    //front projection
                    pos_transform = transform_mesh.GetModel()* transform_OBB.GetModel() * glm::vec4(cubeMesh->centre.x, cubeMesh->centre.y, -view_distance,1);
                    //pos_transform /= pos_transform.w;
                    pos = glm::vec3(pos_transform.x, pos_transform.y, pos_transform.z);
                    up_transform = transform_mesh.GetModel()*transform_OBB.GetModel() * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
                    //up_transform /= up_transform.w;
                    up = glm::vec3(up_transform.x, up_transform.y, up_transform.z);
                    break;
                }
                case 2:
                {
                    //left projection
                    pos_transform = transform_camera.GetModel() * transform_OBB.GetModel() * glm::vec4(view_distance, cubeMesh->centre.y, cubeMesh->centre.z, 1);
                    pos_transform /= pos_transform.w;
                    pos = glm::vec3(pos_transform.x,pos_transform.y,pos_transform.z);
                    up = glm::vec3(0.0f, 1.0f, 0.0f);
                    break;
                }
                case 3:
                {
                    //top projection
                    pos_transform = transform_camera.GetModel() * transform_OBB.GetModel() * glm::vec4(cubeMesh->centre.x, view_distance, cubeMesh->centre.z, 1);
                    pos_transform /= pos_transform.w;
                    pos = glm::vec3(pos_transform.x,pos_transform.y,pos_transform.z);
                    up = glm::vec3(0.0f, 1.0f, 0.0f);
                    break;
                }

                }
                camera = Camera(pos, mesh, ASPECT, up);

                mainShader.Bind();
                mainShader.Update(transform_camera, camera, transform_mesh);
                mesh.Draw();

                saveProjection(img, i);

            }

            //start similarity check

            evaluator.LoadSilhouettesContour();
            evaluator.compareFourier();

            //reset view
            display.projection = Display::projectionType::PERSPECTIVE_P;

            mainShader = Shader("/home/isa/Desktop/UIwithoutQT/Shader/basicShader");
            camera = Camera(mesh, 70.0f, ASPECT, 1.0f, 1000.0f);

        }

        display.Update(transform_camera, transform_OBB);
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

Mesh* createOBB(float radius, glm::vec3 centrex) {
    glm::vec3 centre = glm::vec3(0.f,0.0f,0.f);

    Vertex vertices[] =
    {
        //back & front
        Vertex(glm::vec3(centre.x - radius, centre.y + radius, centre.z - radius), glm::vec3(0, 0, 1)),
        Vertex(glm::vec3(centre.x + radius, centre.y + radius, centre.z - radius), glm::vec3(0, 0, 1)),
        Vertex(glm::vec3(centre.x + radius, centre.y - radius, centre.z - radius), glm::vec3(0, 0, 1)),
        Vertex(glm::vec3(centre.x - radius, centre.y - radius, centre.z - radius), glm::vec3(0, 0, 1)),

        Vertex(glm::vec3(centre.x - radius, centre.y + radius, centre.z + radius), glm::vec3(0, 0, -1)),
        Vertex(glm::vec3(centre.x + radius, centre.y + radius, centre.z + radius), glm::vec3(0, 0, -1)),
        Vertex(glm::vec3(centre.x + radius, centre.y - radius, centre.z + radius), glm::vec3(0, 0, -1)),
        Vertex(glm::vec3(centre.x - radius, centre.y - radius, centre.z + radius), glm::vec3(0, 0, -1)),
        
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


