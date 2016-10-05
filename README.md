# CS430_Project2
### CS430: Computer Graphics
### Project 2: Basic Raycaster
### Marjorie Hahn
### 4 October 2016

This application takes a JSON file containing object data for spheres and
planes within a scene and uses raycasting to create a P3 ppm image that
corresponds to that object data. An example of an appropriate JSON file that
this program can work on can be found in valid.json.

Usage: raycast width height input.json output.ppm

Where "width" and "height" set the size in pixels of the output.ppm image.

In order to run the program, after you have downloaded the files off of Github,
make sure that you are sitting in the directory that holds all of the files and
run the command "make all". Then you will be able to run the program using the
usage command mentioned above. There is one JSON test file included that you can
use to test the functionality of the program: valid.json. The expected image for
this JSON file is output.ppm.
