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
#include "cmshapecontext.h"


typedef vector<Point2d> Silhouette;

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
    int n_silhouettes;
    DB_elements dbElements;

    SimilarityEvaluator(DB_elements& elements, const int candidate_points, int n_projections);

    //Projections
    void LoadSilhouettesContour();

    //Fourier
    vector<int> CompareFourier();

    //Shape Context
    vector<int> CompareShapeContext();


private:
    vector<SilhouetteData> silhouettes;

    //Projections
    vector<Point2d> GetContour(cv::Mat gray);

    //Fourier
    vector<complex<float>> GetFourierDescriptors(Silhouette contour, Mat silImage);
    void GetDBFourierDescriptors();
    float ComputeFDDifference(vector<complex<float>> fd1, vector<complex<float>> fd2);

    vector<int> FindTop(vector<tuple<int, float>> scores);

};

#endif // SIMILARITYEVALUATOR_H
