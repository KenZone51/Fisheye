#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
String commandLine = "v4l2-ctl --list-devices 2>>/dev/null | awk '/MSK R4231: MSK R4231/ {getline;print}' | awk -F\"/dev/video\" '{ print $2 }' | awk 'NF > 0'|awk NF=NF RS= OFS=;";


const int ESC = 27;

string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    int return_code = -1;
    auto pclose_wrapper = [&return_code](FILE* cmd){ return_code = pclose(cmd); };
    { // scope is important, have to make sure the ptr goes out of scope first
    const unique_ptr<FILE, decltype(pclose_wrapper)> pipe(popen(cmd, "r"), pclose_wrapper);
    if (pipe) {
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }
    }
    return result;
}


Mat cropFisheyeImage(Mat fisheyeImage, int Wf, int Hf, int* Wfc, int* Hfc){
    
    Mat fisheyeImageCrop, mask;
    int radius = (int) Hf/2;
    Point2i center(Wf/2, Hf/2);
    
    mask.create(fisheyeImage.rows, fisheyeImage.cols, CV_8UC1); // nrows, ncols and type
    circle(mask, center, radius, Scalar(255, 255, 255), -1, 8, 0);
    
    fisheyeImage.copyTo(fisheyeImageCrop, mask);
    
    Rect rectangle((Wf/2)-radius, (Hf/2)-radius, (2*radius), (2*radius));
    
    fisheyeImageCrop = fisheyeImageCrop(rectangle);
    
    *Wfc = fisheyeImageCrop.size().width;
    *Hfc = fisheyeImageCrop.size().height;
    
    return fisheyeImageCrop;
    
}

int main(int argc, char** argv){
    
    Mat fisheyeImage0, fisheyeImage1, fisheyeImage0Crop, fisheyeImage1Crop, fisheyeImage;
    const char* command = commandLine.c_str();
    String idCamera = exec(command);
    int tabIdCamera[2];
    
    for(int i=0;i<2;i++){
        tabIdCamera[i] = idCamera.at(i) - '0';
    }
    
    VideoCapture camera0(tabIdCamera[0]);
    VideoCapture camera1(tabIdCamera[1]);

    while(true) {
        int Wf0, Hf0;
        int Wf1, Hf1;
        int Wfc0, Hfc0;
        int Wfc1, Hfc1;
        
        camera0 >> fisheyeImage0;
        camera1 >> fisheyeImage1;
        
        Wf0 = fisheyeImage0.size().width;
        Hf0 = fisheyeImage0.size().height;
        Wf1 = fisheyeImage1.size().width;
        Hf1 = fisheyeImage1.size().height;
        
        fisheyeImage0Crop = cropFisheyeImage(fisheyeImage0, Wf0, Hf0, &Wfc0, &Hfc0);
        fisheyeImage1Crop = cropFisheyeImage(fisheyeImage1, Wf1, Hf1, &Wfc1, &Hfc1);
        
        hconcat(fisheyeImage0Crop, fisheyeImage1Crop, fisheyeImage);
        imshow("Video", fisheyeImage);

        if (waitKey(1) == ESC) {
            break;
        }
    }
    
    return 0;
}
