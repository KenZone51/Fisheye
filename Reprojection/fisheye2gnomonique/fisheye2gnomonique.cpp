#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const string PATH_IMAGE = "/Users/Kenza/Desktop/Xcode_cpp_opencv/fisheye2gnomonique/fisheye2gnomonique/Images/";
const int ESC = 27;

//Find the corresponding fisheye outpout point corresponding to an input cartesian point
Point2f fisheye2gnomonique(int Xe, int Ye, double R, double Cfx, double Cfy, double He, double We){
    Point2f fisheyePoint;
    double theta, r, Xf, Yf; //Polar coordinates
    
    r = ( Ye / He ) * R;
    theta = ( Xe / We ) * 2.0 * (CV_PI);
    Xf = Cfx + r * sin(theta);
    Yf = Cfy + r * cos(theta);
    fisheyePoint.x = Xf;
    fisheyePoint.y = Yf;
    
    return fisheyePoint;
}

int main(int argc, char** argv){
    
    Mat fisheyeImage, gnomoniqueImage;
    
    fisheyeImage = imread(PATH_IMAGE + "fisheyeImage.jpg", IMREAD_COLOR);
    namedWindow("Fisheye Image", WINDOW_AUTOSIZE);
    imshow("Fisheye Image", fisheyeImage);
    
    
    while(waitKey(0) != ESC) {
        //wait until the key ESC is pressed
    }
    
    int Hf, Wf, He, We;
    double R, Cfx, Cfy; // Radius and center in x and y coordinates
    
    Hf = fisheyeImage.size().height;
    Wf = fisheyeImage.size().width;
    R = Hf/2;
    Cfx = Wf/2;
    Cfy = Hf/2;
    
    He = int (R);
    We = int (2.0 * CV_PI * R);
    
    gnomoniqueImage.create(He, We, fisheyeImage.type());
    
    for (int Xe = 0; Xe < gnomoniqueImage.size().width; Xe++){
        for (int Ye = 0; Ye < gnomoniqueImage.size().height; Ye++){
            
            gnomoniqueImage.at<Vec3b>(Point(Xe, Ye)) = fisheyeImage.at<Vec3b>(fisheye2gnomonique(Xe, Ye, R, Cfx, Cfy, He, We));
        }
    }
    
    namedWindow("Gnomonique Image", WINDOW_AUTOSIZE);
    imshow("Gnomonique Image",gnomoniqueImage);
    
    while(waitKey(0) != ESC) {
        //wait until the key ESC is pressed
    }
        
    imwrite("gnomoniqueImage.jpg", gnomoniqueImage);
    
    return 0;
}

