#include "db_elements.h"
#include <fstream>
#include <iostream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>



glm::vec3 ParseOBJVec3(const std::string& line);
static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token);
static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end);
Colour colourParser(string hexValue);
int candidate_points;

DB_elements::DB_elements(){}

DB_elements::DB_elements(const string& fileName, const int CANDIDATE_POINTS)
{
    ifstream file;
    string line, tmp;
    Element temp_element;
    int temp_colour;
    glm::vec3 temp_vec3;

    candidate_points = CANDIDATE_POINTS;

    file.open(fileName.c_str());
     if (file.is_open())
     {
         if(file.peek() == std::ifstream::traits_type::eof()){
            std::cout << "Error - DB file is empty" << std::endl;
         }

         while (file.good())
         {
             std::getline(file, line);

             unsigned int lineLength = line.length();

             if (lineLength < 2)
                 continue;

             const char* lineCStr = line.c_str();

             switch (lineCStr[0])
             {

             case 'N':
                 temp_element.name = line.substr(2,line.length()-2);

                 break;
             case 'C':
                 temp_element.material.ambient = colourParser(line.substr(2,line.length()-2));
                 break;
             case 'D':
                 temp_vec3 = ParseOBJVec3(line);
                 temp_element.material.diffuse.r = temp_vec3.x;
                 temp_element.material.diffuse.g = temp_vec3.y;
                 temp_element.material.diffuse.b = temp_vec3.z;
                 break;
             case 'S':
                 temp_vec3 = ParseOBJVec3(line);
                 temp_element.material.specular.r = temp_vec3.x;
                 temp_element.material.specular.g = temp_vec3.y;
                 temp_element.material.specular.b = temp_vec3.z;
                 break;
             case 's':
                 temp_element.material.shininess = stof(line.substr(2,line.length()-2));
             default:
                 this->push_back(temp_element);

             }
         }
     }
     else
     {
         std::cerr << "Unable to load DB file: " << fileName << std::endl;
     }

     loadContours();

}

void DB_elements::loadContours(){
    string file_name;
    Mat imageSRC;


    for (unsigned int n = 0; n < this->size(); n++){
        file_name = "/home/isa/Desktop/UIwithoutQT/DB/"+std::to_string(n)+".png";
        imageSRC = imread(file_name, -1);

        if (!imageSRC.data)
        {
            std::cerr << "Glew failed to initialize!" << std::endl;
        }

        this->at(n).contour = getContour(imageSRC, n);

    }
}

vector<Point2d> DB_elements::getContour(cv::Mat imageSRC, int index){

    vector<vector<Point>> contours;
    vector<Point2d> filt_contours;
    vector<Vec4i> hierarchy;
    Mat silhouette;
    vector<Mat> channels;

    ///padding
    int topbottom = (int) (0.05*imageSRC.rows);
    int rightleft = (int) (0.05*imageSRC.cols);
    copyMakeBorder( imageSRC, imageSRC, topbottom, topbottom, rightleft, rightleft, BORDER_CONSTANT);

    ///use alpha channel only
    split(imageSRC, channels);
    threshold(channels[3], silhouette, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    findContours( silhouette, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    this->at(index).img = silhouette;

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

glm::vec3 ParseOBJVec3(const std::string& line)
{
    unsigned int tokenLength = line.length();
    const char* tokenString = line.c_str();

    unsigned int vertIndexStart = 2;

    while (vertIndexStart < tokenLength)
    {
        if (tokenString[vertIndexStart] != ' ')
            break;
        vertIndexStart++;
    }

    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

    float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

    float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

    float z = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

    return glm::vec3(x, y, z);
}

static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token)
{
    unsigned int result = start;
    while (result < length)
    {
        result++;
        if (str[result] == token)
            break;
    }

    return result;
}

static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end)
{
    return atof(token.substr(start, end - start).c_str());
}

Colour colourParser(string hexValue)
{
  Colour rgbColour;

  rgbColour.r = (float)(stoul(hexValue.substr(0,2),nullptr,16))/255;

  rgbColour.g = (float)(stoul(hexValue.substr(2,2),nullptr,16))/255;

  rgbColour.b = (float)(stoul(hexValue.substr(4,2),nullptr,16))/255;

  return rgbColour;
}

