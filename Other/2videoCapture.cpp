#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int ESC = 27;

int main(int argc, char** argv){
    
    Mat fisheyeImage, fisheyeImage0, fisheyeImage1;
    
    VideoCapture camera0(2); //Rpi
    VideoCapture camera1(0); //Usb

    while(true) {
        camera0 >> fisheyeImage0;
        camera1 >> fisheyeImage1;
        
        hconcat(fisheyeImage0, fisheyeImage1, fisheyeImage);
        imshow("Video", fisheyeImage);
            
        if (waitKey(1) == ESC) {
            break;
        }
    }
    
    return 0;
}
