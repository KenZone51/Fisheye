#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int ESC = 27;

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
    
    Mat fisheyeImage, fisheyeImageCrop;
    VideoCapture capture(0);
    int Wf, Hf;
    int Wfc, Hfc;
    
    if(!capture.isOpened()){
        cout << "Erreur d'ouverture de la capture" << endl;
        return -1;
    }

    while(capture.read(fisheyeImage)){
        
        Wf = fisheyeImage.size().width;
        Hf = fisheyeImage.size().height;
        
        fisheyeImageCrop = cropFisheyeImage(fisheyeImage, Wf, Hf, &Wfc, &Hfc);
        namedWindow("Webcam", WINDOW_NORMAL);
        imshow("Webcam", fisheyeImageCrop);
        
        if (waitKey(1) == 27){
            break;
        }
    }
    
    return 0;
}
