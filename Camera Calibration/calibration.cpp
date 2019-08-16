#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

const int nbImage = 74; //Number of images for the calibration
const int nbHorizontalCorner = 8; //Number of internal width corners of the chessboard
const int nbVerticalCorner = 5; //Number of internal height corners of the chessboard
const int squareSize = 30; //Size of the square from the chessboard in mm.
const int nbCorner = nbHorizontalCorner * nbVerticalCorner;
Size boardSize = Size(nbHorizontalCorner,nbVerticalCorner); //Size of the chessboard pattern in corners

const string PATH_IMAGE_LOAD = "/Users/Kenza/Desktop/Xcode_cpp_opencv/calibration/calibration/ImageLoad/"; //Path to load the images from the calibration
const string PATH_IMAGE_SAVE = "/Users/Kenza/Desktop/Xcode_cpp_opencv/calibration/calibration/ImageSave/"; //Path to save the images from the calibration
const string PATH_PARAMETER = "/Users/Kenza/Desktop/Xcode_cpp_opencv/calibration/calibration/parameter.txt"; //Path to save the intrinsic matrix and distorsion coefficient from the calibration
const int ESC = 27; //ASCII value of the key ESC
ofstream stream(PATH_PARAMETER.c_str(), ios::app);

int findImageCoordinate(int nbImage,vector<vector<Point2f>>& cornerNImage, vector<Mat>& successImage){
    
    Mat fisheyeImage, fisheyeImageCopy,fisheyeImageGray;
    vector<Point2f> corner1Image;
    String name;
    
    int nbSuccessImage = 0;
    bool patternfound;
    
    for(int i = 0;  i < nbImage ; i++){
        
        name = "img" + to_string(i+1) + ".jpg";
        fisheyeImage = imread(PATH_IMAGE_LOAD + name, IMREAD_COLOR);
        
        patternfound = findChessboardCorners(fisheyeImage, boardSize, corner1Image,CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);
        
        if (patternfound){
            cout << "Corners found for image : " << name << endl;
            nbSuccessImage++;
            cvtColor(fisheyeImage, fisheyeImageGray , COLOR_RGB2GRAY);
            cornerSubPix(fisheyeImageGray, corner1Image, Size(11, 11), Size(-1, -1), TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 40, 0.1));
            fisheyeImageCopy = fisheyeImage.clone();
            drawChessboardCorners(fisheyeImageCopy, boardSize, corner1Image, patternfound);
            name = "imgCorner" + to_string(nbSuccessImage) + ".jpg";
            imshow(name, fisheyeImageCopy);
            //imwrite(PATH_IMAGE_SAVE + name, fisheyeImageCopy);
            
            while (waitKey(0) != ESC){
                //just wait
            }
            
            name = "img" + to_string(nbSuccessImage) + ".jpg";
            imwrite(PATH_IMAGE_SAVE + name, fisheyeImage);

            cornerNImage.push_back(corner1Image);
            successImage.push_back(fisheyeImage);
        }
        
        else {
            cout << "Corners not found for image " << name << endl;
            
        }
        
    }
    
    cout << endl;
    cout << "Corners found for " << nbSuccessImage << " images." << endl;
    
    return nbSuccessImage;
}

void findObjectCoordinate(int nbSuccessImage, vector<vector<Point3f>>& cornerNObject){
    
    for (int i = 0; i <nbSuccessImage; i++){
        vector<Point3f> corner1Object;
        for (int y = 0; y < nbVerticalCorner; y++){
            for (int x = 0; x <nbHorizontalCorner; x++){
                corner1Object.push_back(Point3f((float)y * squareSize, (float)x * squareSize, 0.f));
            }
        }
        cornerNObject.push_back(corner1Object);
    }
}

void saveParameter(Matx33d intrinsicMatrix, Vec4d distortionCoefficient){
    
    if(stream){
        cout << "Save the parameter from the calibration into a file" << endl;
        stream << endl;
        stream << "Intrinsic Matrix : " << intrinsicMatrix << endl;
        stream << endl;
        stream << "Distorsion Coefficient : " << distortionCoefficient << endl;
        
        cout << "Close the file" << endl;
        stream.close();
    }
    
    else{
        cout << "Error to open the file " << PATH_PARAMETER << "." << endl;
    }
}

void correctDistortion(int nbSuccessImage, Size imageSize, vector<Mat> successImage, Matx33d intrinsicMatrix, Vec4d distortionCoefficient){

    Mat undistortImage;
    Mat mapX = Mat(imageSize,CV_32FC1);
    Mat mapY = Mat(imageSize,CV_32FC1);
    Mat iD = Mat::eye(3,3,CV_32F);
    Matx33d K = intrinsicMatrix;
    K.operator()(0, 0) = K.operator()(0, 0)/2.0;
    K.operator()(1, 1) = K.operator()(1, 1)/2.0;
    String name;
    
    undistortImage = successImage[0].clone();
    
    for (int i = 0 ; i <nbSuccessImage ; i++){
        fisheye::initUndistortRectifyMap(intrinsicMatrix,distortionCoefficient,iD,K,imageSize,CV_32FC1,mapX,mapY);
        remap(successImage[i],undistortImage,mapX, mapY, INTER_LINEAR);
        name = "imgUndistorted" + to_string(i+1) + ".jpg";
        imwrite(PATH_IMAGE_SAVE + name,undistortImage);
        imshow(name, undistortImage);
        
        while (waitKey(0) != ESC){
            //just wait
        }
        
    }

}

void calculateReprojectionError(int nbSuccessImage, vector<vector<Point3f>> cornerNObject, vector<vector<Point2f>> cornerNImage, vector<Vec3d> rotationVector,
                                vector<Vec3d> translationVector, Matx33d intrinsicMatrix,
                                Vec4d distortionCoefficient){
    
    vector<Point2f> corner1ImageReprojected;
    Mat ImageCorner1image, ImageCorner1ImageReprojected;
    double error1Image = 0.0;
    double errorNImage = 0.0;
    
    for (int i = 0;  i < nbSuccessImage;  i++){
        fisheye::projectPoints(cornerNObject[i], corner1ImageReprojected, rotationVector[i], translationVector[i], intrinsicMatrix, distortionCoefficient);
        
        ImageCorner1image = Mat(1,(cornerNImage[i]).size(),CV_32FC2);
        ImageCorner1ImageReprojected = Mat(1,corner1ImageReprojected.size(), CV_32FC2);
        
        for (size_t j = 0 ; j < (cornerNImage[i]).size(); j++){
            ImageCorner1ImageReprojected.at<Vec2f>(0,j) = Vec2f(corner1ImageReprojected[j].x, corner1ImageReprojected[j].y);
            ImageCorner1image.at<Vec2f>(0,j) = Vec2f((cornerNImage[i])[j].x, (cornerNImage[i])[j].y);
        }
        
        error1Image = norm(ImageCorner1ImageReprojected, ImageCorner1image, NORM_L2);
        errorNImage += error1Image /= nbCorner;
        cout << "The error for the image " << i+1 <<" is " << error1Image << "." << endl;
        
        if(stream){
            cout << "Save the error for the image " << i+1 << " into a file." << endl;
            stream << "error for the image " << i+1 << " : " << error1Image << endl;
        }
        
        else {
            cout << "Error to open the file " << PATH_PARAMETER << "." << endl;
        }
    }
    
    cout << "The total error for the images is " << errorNImage/nbSuccessImage << "." << endl;
    
    if(stream){
        stream << endl;
        cout << "Save the total error for the images into a file." << endl;
        stream << "Total error for the images : " << errorNImage/nbSuccessImage << endl;
        stream << endl;
    }
    
    else {
        cout << "Error to open the file " << PATH_PARAMETER << "." << endl;
    }
}

int main(){
    
    cout << "********** LOOKING FOR THE CORNER COORDINATES IN THE IMAGE COORDINATE SYSTEM **********"<< endl;
    vector<vector<Point2f>> cornerNImage;
    vector<Mat> successImage;
    int nbSuccessImage;
    
    nbSuccessImage = findImageCoordinate(nbImage, cornerNImage, successImage);
    
    cout << "********** LOOKING FOR THE CORNER COORDINATES IN THE REAL WORLD COORDINATE SYSTEM **********"<< endl;
    vector<vector<Point3f>> cornerNObject; //Tableau contenant les coordonnées des coins (3D) pour N objets
    findObjectCoordinate(nbSuccessImage, cornerNObject);
    
    cout << "********** CAMERA CALIBRATION **********"<< endl;
    Size imageSize = successImage[0].size();
    Matx33d intrinsicMatrix;
    Vec4d distortionCoefficient;
    vector<Vec3d> rotationVector;
    vector<Vec3d> translationVector;
    
    fisheye::calibrate(cornerNObject, cornerNImage, imageSize, intrinsicMatrix, distortionCoefficient, rotationVector, translationVector, fisheye::CALIB_FIX_SKEW | fisheye::CALIB_RECOMPUTE_EXTRINSIC | fisheye::CALIB_FIX_PRINCIPAL_POINT, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 200, 0.001));
    
    cout << "********** CALCULATE REPROJECTION ERRORS **********"<< endl;
    calculateReprojectionError(nbSuccessImage, cornerNObject, cornerNImage, rotationVector, translationVector, intrinsicMatrix,distortionCoefficient);
    
    cout << "********** SAVING INTRINSIC MATRIX AND DISTORTION COEFFICIENT PARAMETER **********"<< endl;
    saveParameter(intrinsicMatrix, distortionCoefficient);
    
    cout << "********** DISTORSION CORRECTION  **********"<< endl;
    correctDistortion(nbSuccessImage, imageSize, successImage, intrinsicMatrix, distortionCoefficient);
    
    return 0;
    
    
}
