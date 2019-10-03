#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int ESC = 27;
String commandLine = "v4l2-ctl --list-devices 2>>/dev/null | awk '/MSK R4231: MSK R4231/ {getline;print}' | awk -F\"/dev/video\" '{ print $2 }' | awk 'NF > 0'|awk NF=NF RS= OFS=;";

namespace {
    
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
    
    class ParallelFor : public ParallelLoopBody{
        
    public:
        ParallelFor (Mat fisheyeImageCrop0, Mat fisheyeImageCrop1, Mat& equirectangularImage0, Mat& equirectangularImage1, int We, int He, float FOV)
        : m_fisheyeImageCrop0(fisheyeImageCrop0), m_fisheyeImageCrop1(fisheyeImageCrop1), m_equirectangularImage0(equirectangularImage0), m_equirectangularImage1(equirectangularImage1), m_We(We), m_He(He), m_FOV(FOV){}
        
        virtual void operator ()(const Range& range) const CV_OVERRIDE {
            for (int r = range.start; r < range.end; r++){
                int Xe = r / m_equirectangularImage0.cols;
                int Ye = r % m_equirectangularImage0.cols;
                
                Point2f fisheyePoint = findCorrespondingFisheyePoint(Xe, Ye, m_We, m_He, m_FOV);
                
                if (fisheyePoint.x >= m_We || fisheyePoint.y >= m_He)
                    
                    continue;
                
                if (fisheyePoint.x < 0 || fisheyePoint.y < 0)
                    continue;
                
                m_equirectangularImage0.at<Vec3b>(Point(Xe, Ye)) = m_fisheyeImageCrop0.at<Vec3b>(fisheyePoint);
                m_equirectangularImage1.at<Vec3b>(Point(Xe, Ye)) = m_fisheyeImageCrop1.at<Vec3b>(fisheyePoint);
            }
        }
        
        ParallelFor& operator=(const ParallelFor &) {
            return *this;
        };
        
    private:
        Mat m_fisheyeImageCrop0;
        Mat m_fisheyeImageCrop1;
        Mat& m_equirectangularImage0;
        Mat& m_equirectangularImage1;
        int m_We;
        int m_He;
        float m_FOV;
    };
}

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
    
    Mat fisheyeImage0, fisheyeImage1,fisheyeImageCrop0, fisheyeImageCrop1, equirectangularImage0, equirectangularImage1, equirectangularImage;
    
    int Wf, Hf;
    int Wfc, Hfc;
    int We, He;
    float FOV = (185 * CV_PI ) / 180;
    
    const char* command = commandLine.c_str();
    String idCamera = exec(command);
    int tabIdCamera[2];
    
    for(int i=0;i<2;i++){
        tabIdCamera[i] = idCamera.at(i) - '0';
    }
    
    VideoCapture camera0(tabIdCamera[0]);
    VideoCapture camera1(tabIdCamera[1]);
    
    if( (!camera0.isOpened()) || (!camera1.isOpened())){
        cout << "Erreur d'ouverture de la capture" << endl;
        return -1;
    }
    
    while(true){
        
        camera0 >> fisheyeImage0;
        camera1 >> fisheyeImage1;
        
        Wf = fisheyeImage0.size().width;
        Hf = fisheyeImage0.size().height;
        
        fisheyeImageCrop0 = cropFisheyeImage(fisheyeImage0, Wf, Hf, &Wfc, &Hfc);
        fisheyeImageCrop1 = cropFisheyeImage(fisheyeImage1, Wf, Hf, &Wfc, &Hfc);
        
        We = Wfc;
        He = Hfc;
        
        equirectangularImage0.create(He, We, CV_8UC3);
        equirectangularImage1.create(He, We, CV_8UC3);
        
        ParallelFor parallelFor(fisheyeImageCrop0, fisheyeImageCrop1, equirectangularImage0, equirectangularImage1, We, He, FOV);
        parallel_for_(Range(0, equirectangularImage0.rows*equirectangularImage0.cols), parallelFor);
    
        hconcat(equirectangularImage0, equirectangularImage1, equirectangularImage);
        imshow("Webcam", equirectangularImage);
        
        if(waitKey(1) == 27){
            break;
        }
    }
    
    return 0;
}



