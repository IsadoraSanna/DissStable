#ifndef DB_ELEMENTS_H
#define DB_ELEMENTS_H

#include <vector>
#include <opencv/cv.h>
#include <glm/glm.hpp>

#include "material.h"

using namespace cv;
using namespace std;

struct Element
{
    string name;
    vector<Point2d> contour;
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
    vector<Point2d> getContour(cv::Mat imageSRC, int index);
};

#endif // DB_ELEMENTS_H
