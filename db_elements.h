#ifndef DB_ELEMENTS_H
#define DB_ELEMENTS_H

#include <vector>
#include <opencv/cv.h>
#include <glm/glm.hpp>

using namespace cv;
using namespace std;

struct Colour
{
   float r;
   float g;
   float b;
};

struct Material
{
    Colour ambient;
    Colour diffuse;
    Colour specular;
    float shininess;
};

struct Element
{
    string name;
    vector<Point> contour;
    Material material;
    vector<complex<float>> fd;
    Mat img;
};


class DB_elements : public vector<Element>
{

public:
    DB_elements(const string& fileName, const int CANDIDATE_POINTS);
    DB_elements();

private:
    void loadContours();
    vector<Point> getContour(cv::Mat imageSRC, int index);
};

#endif // DB_ELEMENTS_H
