#include <iostream>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const string PATH_IMAGE = "/Users/Kenza/Desktop/Xcode_cpp_opencv/fisheye2equirectangular/fisheye2equirectangular/Images/";
const int ESC = 27;

Point2f findCorrespondingFisheyePoint(int Xe, int Ye, int We, int He, float FOV){
    Point2f fisheyePoint;
    float theta, phi, r;
    Point3f sphericalPoint;
    
    theta = CV_PI * (Xe / ( (float) We ) - 0.5);
    phi = CV_PI * (Ye / ( (float) He ) - 0.5);
    
    sphericalPoint.x = cos(phi) * sin(theta);
    sphericalPoint.y = cos(phi) * cos(theta);
    sphericalPoint.z = sin(phi);
    
    theta = atan2(sphericalPoint.z, sphericalPoint.x);
    phi = atan2(sqrt(pow(sphericalPoint.x,2) + pow(sphericalPoint.z,2)), sphericalPoint.y);
    r = ( (float) We ) * phi / FOV;
    
    fisheyePoint.x = (int) ( 0.5 * ( (float) We ) + r * cos(theta) );
    fisheyePoint.y = (int) ( 0.5 * ( (float) He ) + r * sin(theta) );
    
    return fisheyePoint;
}

int main(int argc, char** argv){
    
    Mat fisheyeImage, equirectangularImage;
    int Wf, Hf;
    float FOV;
    int We, He;
    
    fisheyeImage = imread(PATH_IMAGE + "fisheyeImage.jpg", IMREAD_COLOR);
    namedWindow("Fisheye Image");
    imshow("Fisheye Image", fisheyeImage);
    
    Wf = fisheyeImage.size().width;
    Hf = fisheyeImage.size().height;
    FOV = (180 * CV_PI ) / 180;
    
    We = Wf;
    He = Hf;
    
    while (waitKey(0) != ESC){
        
    }
    
    equirectangularImage.create(He, We, CV_8UC3);
    
    for (int Xe = 0; Xe < We; Xe++){
        
        for (int Ye = 0; Ye < He; Ye++){
            
            Point2f fisheyePoint = findCorrespondingFisheyePoint(Xe, Ye, We, He, FOV);
            
            if (fisheyePoint.x >= We || fisheyePoint.y >= He)
                
                continue;
            
            if (fisheyePoint.x < 0 || fisheyePoint.y < 0)
                continue;
            
            equirectangularImage.at<Vec3b>(Point(Xe, Ye)) = fisheyeImage.at<Vec3b>(fisheyePoint);
            
        }
        
    }

    namedWindow("Equirectangular Image");
    imshow("Equirectangular Image", equirectangularImage);
    
    while (waitKey(0) != ESC){
        
    }
    
    imwrite("equirectangularImage.jpg", equirectangularImage);
    
}
