#include <iostream>
#include <GL/glew.h>
#include <opencv2/opencv.hpp>
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

/**
 * @brief Screen initialization parameters
 */
static const int DISPLAY_WIDTH = 800;
static const int DISPLAY_HEIGHT = 600;
static const float ASPECT = (float)DISPLAY_WIDTH/(float)DISPLAY_HEIGHT;
static const int RESOLUTION = 100;

/**
 * @brief number of candidate points considered when downsapling silhouettes contours
 */
const int CANDIDATE_POINTS = 64;
static const int N_PROJECTIONS = 7;

Mesh* CreateOBB(float radius);
void Rotate(cv::Mat &img);
void SaveProjection(cv::Mat& img, int index);
void ExtractMaterials(const DB_elements& dbElements, vector<Material>& materials, vector<int>& bestMaterials);

int main()
{
    ////Prepare DB
//    DB_elements dbElements = DB_elements("/home/isa/Desktop/UIwithoutQT/DB/db.txt", CANDIDATE_POINTS);
//    SimilarityEvaluator evaluator = SimilarityEvaluator(dbElements, CANDIDATE_POINTS, N_PROJECTIONS);

    vector<Material> materials;
    Material basic_material;
    basic_material.shininess = -1.0f;
    vector<int> bestMaterials;

    cv::Mat imgProjection(DISPLAY_HEIGHT, DISPLAY_WIDTH, CV_8UC3); //8byte unsigned int 3 channels

    ////Init screen, mesh and shaders
    Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "Mesh visualiser");
    Mesh mesh("/home/isa/Desktop/UIwithoutQT/Meshes/teapot.obj");
    Mesh *cubeMesh = CreateOBB(mesh.radius);
    Shader mainShader("/home/isa/Desktop/UIwithoutQT/Shader/basicShader");
    Shader cubeShader("/home/isa/Desktop/UIwithoutQT/Shader/cubeShader");

    ////set camera
    Camera camera(mesh, 70.0f, ASPECT, 0.1f, 1000.0f);

    //transform for camera
    Transform transform_camera = Transform();
    //transform for OBB
    Transform transform_OBB = Transform();
    //transform for  Mesh
    Transform transform_mesh = Transform();
    //move mesh centre to (0,0) world space)
    transform_mesh.m_pos = (glm::vec3(-mesh.centre.x,-mesh.centre.y,-mesh.centre.z));

    while (!display.isClosed()) {

        ////PERSPECTVE PROJECTION
        if(display.projection == Display::projectionType::PERSPECTIVE_P)
        {
            display.Clear(0.0f, 0.15f, 0.3f, 1.0f);

            //if reset was seletced, reset best materials
            if(display.materialKey == -2){
                bestMaterials.clear();
                display.materialKey = -1;
            }

            //manage camera shader
            mainShader.Bind();
            if(display.materialKey != -1 && !bestMaterials.empty())
                mainShader.Update(transform_camera, camera, transform_mesh, materials.at(display.materialKey));
            else if (display.materialKey == -1 && !bestMaterials.empty())
                mainShader.Update(transform_camera, camera, transform_mesh, materials.at(0));
            else
                mainShader.Update(transform_camera, camera, transform_mesh, basic_material);
            mesh.Draw();

            //manage OBB shader
            if(display.materialKey == -1 && bestMaterials.empty()){
                cubeShader.Bind();
                cubeShader.UpdateWOBB(transform_camera, camera, transform_OBB);
                cubeMesh->Draw();
            }

        }

        ////ORTHOGRAPHIC PROJECTION
        else
        {
            Shader mainShader("/home/isa/Desktop/UIwithoutQT/Shader/silhouetteShader");
            glm::vec4 pos_transform, up_transform;
            glm::vec3 pos, up;
            float view_distance = mesh.radius + 1.f;


            for (int i = 1; i <= N_PROJECTIONS; i++){

                display.Clear(1.0f, 1.0f, 1.0f, 1.0f);

                switch(i){
                case 1:               
                    //front projection
                    pos_transform =  transform_OBB.GetModel() * glm::vec4(cubeMesh->centre.x, cubeMesh->centre.y, -view_distance, 1);
                    pos_transform /= pos_transform.w;
                    pos = glm::vec3(pos_transform.x, pos_transform.y, pos_transform.z);
                    up = glm::vec3(0.0f, 1.0f, 0.0f);
                    break;
                case 2:               
                    //left projection
                    pos_transform = transform_OBB.GetModel() * glm::vec4(view_distance, cubeMesh->centre.y, cubeMesh->centre.z, 1);
                    pos_transform /= pos_transform.w;
                    pos = glm::vec3(pos_transform.x,pos_transform.y,pos_transform.z);
                    up = glm::vec3(0.0f, 1.0f, 0.0f);
                    break;                
                case 3:                
                    //top projection
                    pos_transform = transform_OBB.GetModel() * glm::vec4(cubeMesh->centre.x, view_distance, cubeMesh->centre.z, 1);
                    pos_transform /= pos_transform.w;
                    pos = glm::vec3(pos_transform.x, pos_transform.y, pos_transform.z);
                    up = glm::vec3(0.0f, 0.0f, 1.0f);
                    break;                
                case 4:
                    //top-front-left projection
                    pos_transform = transform_OBB.GetModel() * glm::vec4(cubeMesh->centre.x - view_distance, cubeMesh->centre.y + view_distance, cubeMesh->centre.z - view_distance, 1);
                    pos_transform /= pos_transform.w;
                    pos = glm::vec3(pos_transform.x, pos_transform.y, pos_transform.z);
                    up = glm::vec3(0.0f, 1.0f, 0.0f);
                    break;
                case 5:
                    //top-back-left projection
                    pos_transform = transform_OBB.GetModel() * glm::vec4(cubeMesh->centre.x - view_distance, cubeMesh->centre.y + view_distance, cubeMesh->centre.z + view_distance, 1);
                    pos_transform /= pos_transform.w;
                    pos = glm::vec3(pos_transform.x, pos_transform.y, pos_transform.z);
                    up = glm::vec3(0.0f, 1.0f, 0.0f);
                    break;
                case 6:
                    //top-back-right projection
                    pos_transform = transform_OBB.GetModel() * glm::vec4(cubeMesh->centre.x + view_distance, cubeMesh->centre.y + view_distance, cubeMesh->centre.z + view_distance, 1);
                    pos_transform /= pos_transform.w;
                    pos = glm::vec3(pos_transform.x, pos_transform.y, pos_transform.z);
                    up = glm::vec3(0.0f, 1.0f, 0.0f);
                    break;
                case 7:
                    //top-front-right projection
                    pos_transform = transform_OBB.GetModel() * glm::vec4(cubeMesh->centre.x + view_distance, cubeMesh->centre.y + view_distance, cubeMesh->centre.z - view_distance, 1);
                    pos_transform /= pos_transform.w;
                    pos = glm::vec3(pos_transform.x, pos_transform.y, pos_transform.z);
                    up = glm::vec3(0.0f, 1.0f, 0.0f);
                    break;
                }
                camera = Camera(pos, mesh, ASPECT, up, 0.1f, 1000.0f);

                mainShader.Bind();
                mainShader.Update(transform_camera, camera, transform_mesh,basic_material);
                mesh.Draw();

                SaveProjection(imgProjection, i);

            }

            ////start similarity check
//            bestMaterials.clear();
//            evaluator.LoadSilhouettesContour();
//            //bestMaterials = evaluator.CompareFourier();
//            bestMaterials = evaluator.CompareShapeContext();
//            ExtractMaterials(dbElements, materials, bestMaterials);

            ////reset view
            display.projection = Display::projectionType::PERSPECTIVE_P;

            mainShader = Shader("/home/isa/Desktop/UIwithoutQT/Shader/basicShader");
            camera = Camera(mesh, 70.0f, ASPECT, 1.0f, 1000.0f);

        }

        display.Update(transform_camera, transform_OBB);
    }

    return 0;
}


void SaveProjection(cv::Mat& img, int index){
    std::string path = std::string("/home/isa/Desktop/UIwithoutQT/Silhouette/silhouette")+ std::to_string(index) + ".jpg";

    glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize());
    glReadPixels(0, 0, DISPLAY_WIDTH ,DISPLAY_HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, img.data);

    Rotate(img);
    imwrite(path, img);
}

void Rotate(Mat &img)
{
    cv::Point2f pt((float)DISPLAY_WIDTH/2,(float)DISPLAY_HEIGHT/2);
    cv::Mat r = getRotationMatrix2D(pt, 180, 1);
    warpAffine(img, img, r, cv::Size(img.cols, img.rows));
    cv::flip(img, img, 1);
}

Mesh* CreateOBB(float radius) {
    //centre is (0,0,0)
    Vertex vertices[] =
    {
        //back & front
        Vertex(glm::vec3(- radius, radius,  - radius), glm::vec3(0, 0, 1)),
        Vertex(glm::vec3(radius, radius, - radius), glm::vec3(0, 0, 1)),
        Vertex(glm::vec3(radius, - radius, - radius), glm::vec3(0, 0, 1)),
        Vertex(glm::vec3(- radius, - radius, - radius), glm::vec3(0, 0, 1)),

        Vertex(glm::vec3(- radius, radius, radius), glm::vec3(0, 0, -1)),
        Vertex(glm::vec3(radius, radius, radius), glm::vec3(0, 0, -1)),
        Vertex(glm::vec3(radius,  - radius, radius), glm::vec3(0, 0, -1)),
        Vertex(glm::vec3(- radius, - radius, radius), glm::vec3(0, 0, -1)),
        
        //bottom & top
        Vertex(glm::vec3(- radius, - radius, radius), glm::vec3(0, -1, 0)),
        Vertex(glm::vec3(radius, - radius, radius), glm::vec3(0, -1, 0)),
        Vertex(glm::vec3(radius, - radius, - radius), glm::vec3(0, -1, 0)),
        Vertex(glm::vec3(- radius, - radius, - radius), glm::vec3(0, -1, 0)),

        Vertex(glm::vec3(- radius, radius, radius), glm::vec3(0, 1, 0)),
        Vertex(glm::vec3(radius, radius, radius), glm::vec3(0, 1, 0)),
        Vertex(glm::vec3(radius, radius, - radius), glm::vec3(0, 1, 0)),
        Vertex(glm::vec3(- radius, radius, - radius), glm::vec3(0, 1, 0)),

        //right & left
        Vertex(glm::vec3(radius, radius, - radius), glm::vec3(1, 0, 0)),
        Vertex(glm::vec3(radius, radius, radius), glm::vec3(1, 0, 0)),
        Vertex(glm::vec3(radius, - radius, radius), glm::vec3(1, 0, 0)),
        Vertex(glm::vec3(radius, - radius, - radius), glm::vec3(1, 0, 0)),

        Vertex(glm::vec3(- radius, radius, - radius), glm::vec3(-1, 0, 0)),
        Vertex(glm::vec3(- radius, radius, radius), glm::vec3(-1, 0, 0)),
        Vertex(glm::vec3(- radius, - radius,radius), glm::vec3(-1, 0, 0)),
        Vertex(glm::vec3(- radius, - radius, - radius), glm::vec3(-1, 0, 0)),
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


void ExtractMaterials(const DB_elements& dbElements, vector<Material> &materials, vector<int>& bestMaterials){
    materials.clear();
    for(unsigned int i = 0; i < bestMaterials.size(); i++)
        materials.push_back(dbElements.at(bestMaterials.at(i)).material);
}
