# Introduction

Camera calibration is a process used to estimates the parameters of the lens and the sensor of a camera. This is a usefull process to correct the distorsion of a lens, to measure the size of an object in world units or to determine the location of the camera according to the image. Camera parameters include intrinsics, extrinsics, and distortion coefficients.

The intrinsics parameters are specifics for one cameras. They include :

  - f : the focal length of the lens.
  - ku and kv : the scaling factors of the image.
  - cu and cv : the coordinates of the optical center in the image. For a fisheye image, those coordinates are located at the center of the circle image.
  - suv : a value that reflect the non-orthogonality from the lines and colums of the photosensitive electronic cells from the camera sensor. Most of the time, this parameter is neglegted and take the value 0.

The extrinsics parameters which may vary according to the position of the camera in the 3D space. They include :
  
  - R3.3 : the rotation matrix that give the relation between the image coordinate system and the camera coordinate system.
  - Tx,Ty,Tz : the translation vector that give the relation between the image coordinate system and the camera coordinate system.

To estimate the camera parameters, you need to have 3-D world points and their corresponding 2-D image points.

# How to use this code
