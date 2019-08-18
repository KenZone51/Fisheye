#include <iostream>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const int ESC = 27;

int main(int argc, char** argv){
    
    Mat fisheyeImage;
    VideoCapture capture(0);
    
    if(!capture.isOpened()){
        cout << "Erreur d'ouverture de la capture" << endl;
        return -1;
    }

    while(capture.read(fisheyeImage)){
        
        namedWindow("Webcam", WINDOW_NORMAL);
        imshow("Webcam", fisheyeImage);
        
        if (waitKey(1) == 27){
            break;
        }
    }
    
    return 0;
}
