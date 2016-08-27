#ifndef SIMILARITYEVALUATOR_H
#define SIMILARITYEVALUATOR_H

#include "db_elements.h"
#include <string>
#include <iostream>
#include <fstream>
#include <tuple>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "db_elements.h"


typedef vector<Point> Silhouette;

struct SilhouetteData{
    Silhouette silhouette;
    cv::Mat silIMG;
};

class SimilarityEvaluator
{
public:
    int candidate_points;
    int DB_SIZE;
    const int N_BEST = 3;
    const int N_SILHOUETTES = 3;
    DB_elements dbElements;

    SimilarityEvaluator( DB_elements& elements, const int candidate_points);

    //Projections
    void LoadSilhouettesContour();
    vector<Point> getContour(cv::Mat gray);

    void getDBFourierDescriptors();
    vector<complex<float>> getFourierDescriptors(Silhouette contour, Mat silImage);
    float computeFDDifference(vector<complex<float>> fd1, vector<complex<float>> fd2);
    void compareFourier();


    void findTop(vector<tuple<int, float>> scores);
private:
    vector<SilhouetteData> silhouettes;


};

#endif // SIMILARITYEVALUATOR_H
