#include "similarityevaluator.h"

using namespace std;

SimilarityEvaluator::SimilarityEvaluator( DB_elements& elements, const int CANDIDATE_POINTS, int n_projections)
{
    this->dbElements = elements;
    DB_SIZE = elements.size();
    this->candidate_points = CANDIDATE_POINTS;
    this->n_silhouettes = n_projections;
    GetDBFourierDescriptors();
}

////PROJECTIONS
void SimilarityEvaluator::LoadSilhouettesContour(){

    string silPath;
    Mat silSRC, gray;
    SilhouetteData temp_silhouette;
    silhouettes.clear();

    for(int i = 1; i <= n_silhouettes; i++){

       silPath = string("/home/isa/Desktop/UIwithoutQT/Silhouette/silhouette")+ to_string(i) + ".jpg";

       silSRC = imread(silPath, 1);

       if (silSRC.data){
           cvtColor(silSRC, gray, CV_BGR2GRAY);
           threshold(gray, gray, 1, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
           bitwise_not (gray, gray);


           temp_silhouette.silhouette = GetContour(gray);
           temp_silhouette.silIMG = gray;
           silhouettes.push_back(temp_silhouette);

           //security clear
           temp_silhouette.silhouette.clear();
       }
       else
           std::cerr << "Unable to load file: " << silPath << std::endl;

    }
}


vector<Point2d> SimilarityEvaluator::GetContour(cv::Mat gray){

    vector<vector<Point>> contours;
    vector<Point2d> filt_contours;

    findContours(gray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    int biggest = 0;
    for (unsigned int i = 1; i < contours.size(); i++){
        if(contours[i].size()>contours[biggest].size())
            biggest = i;
    }

    ///shape size normalization
    int step = (int)(ceil(contours[0].size()/candidate_points));
    for (unsigned int i = 0, j = 0; i < contours[biggest].size() && j < candidate_points; i += step, j++){
        //filt_contours.push_back(contours[biggest][i]);
        filt_contours.push_back(Point2d(contours[biggest][i].x, contours[biggest][i].y));
    }

    return filt_contours;
}

////FOURIER DESCRIPTORS
vector<int> SimilarityEvaluator::CompareFourier(){
    float difference = 0.0f;
    vector<float> scores;
    std::cout << "-------------FOURIER DESCRIPTORS-------------" << std::endl;
    for(int i = 0; i < n_silhouettes; i++){
           vector<complex<float>> silFD = GetFourierDescriptors(silhouettes.at(i).silhouette, silhouettes.at(i).silIMG);
           //confronta con gli fd di tutti i db elements
           for (int j = 0; j < DB_SIZE; j++){
               difference = ComputeFDDifference(silFD, dbElements.at(j).fd);
               std::cout << "Silhouette " << i+1 << " - DB element " << j << " -> Difference = " << difference << std::endl;
               //scores.push_back(difference);
               scores.push_back(difference);
           }
        std::cout << "--------" << std::endl;
    }
    return FindTop(scores);
}

vector<complex<float>> SimilarityEvaluator::GetFourierDescriptors(Silhouette contour, Mat silImage){
    int n_pixels_contour = contour.size();

    ///find the centroid
    Moments m = moments(silImage, true);
    Point center(m.m10/m.m00, m.m01/m.m00);

    ///create centroid distance function
    vector<float> cdf;
    for(int i = 0; i < n_pixels_contour; i++)
        cdf.push_back( sqrt(pow(contour[i].x-center.x,2) + pow(contour[i].y-center.y,2)) );

    ///create FDS coefficients
    complex<float> j;
    j = -1;
    j = sqrt(j);
    complex<float> result;
    vector<complex<float>> fd, fourier_coeff;
    float norm_factor;


    for (int n = 0; n < n_pixels_contour; ++n)
    {
        result = 0.0f;
        for (int t = 0; t < n_pixels_contour; ++t)
        {
            result += (cdf[t] * exp(-j*((float)M_PI)*((float)n)*((float)t) / ((float)n_pixels_contour)));

        }

        fourier_coeff.push_back((1.0f / n_pixels_contour) * result);
    }


    fourier_coeff.at(0) = complex<float>(0,0);

    norm_factor = abs(fourier_coeff.at(1));

    for( unsigned int i = 0; i < fourier_coeff.size(); i++) {
        fd.push_back(abs(fourier_coeff.at(i))/norm_factor);
    }

    return fd;
}

void SimilarityEvaluator::GetDBFourierDescriptors(){

    for(int i = 0; i < DB_SIZE; i++){
        dbElements.at(i).fd = GetFourierDescriptors(dbElements.at(i).contour, dbElements.at(i).img);
    }
}

float SimilarityEvaluator::ComputeFDDifference(vector<complex<float>> fd1, vector<complex<float>> fd2){
    float temp_diff = 0.0f;

    if (fd1.size() != fd2.size())
        return -1;

    for(unsigned int i = 0; i < fd1.size(); i++){
        temp_diff += pow(abs(fd1.at(i).real() - fd2.at(i).real()),2);
    }
    return sqrt(temp_diff);
}


////SHAPE CONTEXT
vector<int> SimilarityEvaluator::CompareShapeContext(){
    float difference = 0.0f;
    CmShapeContext shapeContext;
    vector<float> scores;

    std::cout << "---------SHAPE CONTEXT DESCRIPTORS---------" << std::endl;

    for(int i = 0; i < n_silhouettes; i++){

           for (int j = 0; j < DB_SIZE; j++){                        
               difference = shapeContext.matchCost(silhouettes.at(i).silhouette, dbElements.at(j).contour);

               std::cout << "Silhouette " << i+1 << " - DB element " << j << " - Difference = " << difference << std::endl;

               scores.push_back(difference);
           }

        std::cout << "--------" << std::endl;
    }

    return FindTop(scores);
}

////RESULTS
vector<int> SimilarityEvaluator::FindTop(vector<float> scores){
    float min = 1;
    int indexMin;
    int index = 0;
    vector<int> best;
    vector<float> temp_best;

    std::cout << "-------------Results-------------" << std::endl;

    for(int i = 0; i < n_silhouettes; i++)
    {
        for (unsigned int j = 0 ; j < DB_SIZE; j++)
        {
            if (i == 0)
                temp_best.push_back(scores.at(index));
            else
                if(scores.at(index) < temp_best.at(j))
                    temp_best.at(j) = scores.at(index);

            index++;
        }
    }

    for(int i = 0; i < N_BEST ; i++)
    {
        for (unsigned int j = 0 ; j < temp_best.size(); j++)
        {
            if(temp_best.at(j)<min){
                min = temp_best.at(j);
                indexMin = j;
            }
        }
        best.push_back(indexMin);

        std::cout << "Best #" << i+1 << " - index " <<indexMin << " - difference " << min << ". Material: " << dbElements.at(indexMin).name << std::endl;

        temp_best.at(indexMin) = 1;
        min = 1;
    }

    return best;
}
