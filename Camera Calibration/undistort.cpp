#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

const string PATH_IMAGE = "/Users/Kenza/Desktop/Xcode_cpp_opencv/undistort/undistort/Images/";
const int ESC = 27;

#define xScaleFactor 5;
#define yScaleFactor 5;

Mat CorrectDistortion(Mat fisheyeImage, Size imageSize, Matx33d intrinsicMatrix, Vec4d distortionCoefficient){
    
    Mat undistortImage;
    Mat mapX = Mat(imageSize,CV_32FC1);
    Mat mapY = Mat(imageSize,CV_32FC1);
    Mat iD = Mat::eye(3,3,CV_32F);
    
    undistortImage = fisheyeImage.clone();
    
    fisheye::initUndistortRectifyMap(intrinsicMatrix,distortionCoefficient,iD,intrinsicMatrix,imageSize,CV_32FC1,mapX,mapY);
    remap(fisheyeImage,undistortImage,mapX, mapY, INTER_LINEAR);
    
    return undistortImage;
    
}

int main(){
    
    Mat fisheyeImage, undistortImage;
    Size imageSize;
    Matx33d intrinsicMatrix;
    Vec4d distortionCoefficient;
    
    fisheyeImage = imread(PATH_IMAGE + "fisheyeImage.jpg", IMREAD_COLOR);
    imageSize = fisheyeImage.size();
    namedWindow("Fisheye Image");
    imshow("fisheye Image", fisheyeImage);
    
    while (waitKey(0) != ESC){
        //just wait
    }
    
    intrinsicMatrix.operator()(0, 0) = 164.8508765011963 * xScaleFactor; //x scale factor
    intrinsicMatrix.operator()(0, 1) = 0;
    intrinsicMatrix.operator()(0, 2) = (fisheyeImage.size().height/2); //x optical center
    intrinsicMatrix.operator()(1, 1) = 165.1687927442833 * yScaleFactor; //y scale factor
    intrinsicMatrix.operator()(1, 2) = (fisheyeImage.size().height/2); //y optical center
    intrinsicMatrix.operator()(2, 0) = 0;
    intrinsicMatrix.operator()(2, 1) = 0;
    intrinsicMatrix.operator()(2, 2) = 1;
    
    distortionCoefficient.operator()(0) = -0.085204;
    distortionCoefficient.operator()(1) = 0.119815;
    distortionCoefficient.operator()(2) = -0.079293;
    distortionCoefficient.operator()(3) = 0.0155262;
    
    undistortImage.create(fisheyeImage.size().height, fisheyeImage.size().width, fisheyeImage.type());
    
    const int64 start = cv::getTickCount();
    
    undistortImage = CorrectDistortion(fisheyeImage, imageSize, intrinsicMatrix, distortionCoefficient);
    
    double duration = (cv::getTickCount()-start)/cv::getTickFrequency();
    
    cout << duration << endl;
    
    imshow("undistortImage.jpg", undistortImage);
    imwrite(PATH_IMAGE + "undistortImage.jpg",undistortImage);
    
    while (waitKey(0) != ESC){
        //just wait
    }
    
    return 0;
    
    
}
