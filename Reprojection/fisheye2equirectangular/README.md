This source code allows to project a fisheye image into a equirectangular projection. This projection map a portion of the surface of a sphere to a flat image. The horizontal coordinate of the flat image correspond to the longitude of the sphere and the vertical coordinate correspond to the latitude of the sphere.

The images used in the source code can be found in the "Images" folder. To use this code, the fisheye image needs to be contain in a square (i.e the size and the height of the input image need to be the same). Also you need to need to change the variable FOV according to the FOV of your camera. In my case, the camera have a 200° FOV.

For a given fisheye image as following :

![fisheyeImage](https://user-images.githubusercontent.com/54168090/63164547-5f017a00-c05b-11e9-9324-135184d68a66.jpg)

The result equirectangular image is the following :

![equirectangularImage](https://user-images.githubusercontent.com/54168090/63164688-bd2e5d00-c05b-11e9-942f-85443c25c96c.jpg)

