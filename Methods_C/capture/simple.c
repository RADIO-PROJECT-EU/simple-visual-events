/* Capture video input from AVI or camera, public domain. */

#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <ctype.h>

IplImage *image = 0;

#define WINDOWNAME "RADIO Camera Capture Application"

typedef unsigned char byte;

int main(int argc, char** argv) {
  CvCapture *capture = 0;
  int x,y;
   
  capture = cvCaptureFromCAM(0);

  cvNamedWindow(WINDOWNAME, 1);

  for(;;) {

    image  = cvQueryFrame(capture);

    cvShowImage(WINDOWNAME, image);

    char c = cvWaitKey(30);
    if (c =='q') { break; }
  }

  cvReleaseCapture(&capture);
  cvDestroyWindow(WINDOWNAME);

  return 0;
}
