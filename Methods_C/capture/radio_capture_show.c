/* Capture video input from camera and display on screen */

#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <ctype.h>


IplImage *image = 0;
IplImage *show  = 0;

#define WINDOWNAME "RADIO Camera Capture/Show Application"

#define CAPTURE_WEBCAM
//#define CAPTURE_BMP (230)


#define REDV   0
#define GREENV 1
#define BLUEV  2


typedef unsigned char byte;


#include "process.c"


void show_help() {
  printf("---------------------------\n");
  printf("Key-based user controls:   \n");
  printf("---------------------------\n");
  printf("q = Quit                   \n");
  printf("a = Annotation select      \n");
  printf("r = Reset iteration counter\n");
  printf("p = Cup detection mode     \n");
  printf("    0 -> Wait to place cup \n");
  printf("    0 -> Cup is placed     \n");
  printf("    0 -> Check if moved    \n");
  printf("h =  Set shorter height    \n");
  printf("H =  Set taller height     \n");
  printf("? =  Display this help msg.\n");
  printf("---------------------------\n");
}

int main(int argc, char** argv) {
  
  CvCapture *capture = 0;
 
  int x,y;
  unsigned char RGB[640][480][3];
  unsigned char Bac[640][480][3]; 
   
  capture = cvCaptureFromCAM(0);

  cvNamedWindow(WINDOWNAME, 1);
  
  
  unsigned char ip[4];
  ip[0]=192;
  ip[1]=168;
  ip[2]=0;
  ip[3]=30;
  //udp_open(&ip);
  
  unsigned int iteration, index , showanno;
  
  
  v_STANDING_PERSON_HEIGHT = STANDING_PERSON_HEIGHT;
  
  show_help();
  
  showanno = 2;
  iteration = 1;
  
  for(;;) {
  
    if (iteration>16383) iteration=0; else iteration++;
    
    
    #ifdef CAPTURE_WEBCAM
        image  = cvQueryFrame(capture);
        index = iteration;
    #endif

    #ifdef CAPTURE_BMP
    	char fnam[16];
	    sprintf(fnam,"./bmp/frame%04d.bmp",(iteration%CAPTURE_BMP)+1);
	    printf("Reading: %s\n",fnam);
        image  = cvLoadImage(fnam,CV_LOAD_IMAGE_COLOR);
        index = (iteration%5)+1;

        
    #endif
    
    int rows    = image->height;
    int columns = image->width;
    int step    = image->widthStep;
    int channels= image->nChannels;
    
    //printf("Rows = %d   Columns = %d  channels = %d\n",rows,columns,channels);
    
    unsigned char *image_data = (unsigned char*)(image->imageData);
    
    for (y=0;y<rows;y++) {
        for (x=0;x<columns;x++) {
		RGB[639-x][y][REDV  ] = image_data[step*y+x*3+0];
		RGB[639-x][y][GREENV] = image_data[step*y+x*3+1];
		RGB[639-x][y][BLUEV ] = image_data[step*y+x*3+2];
		//printf("x=%d, y=%d, i=%d\n",x,y,step*y+x*3);
        }
    }
    
    process((unsigned char *)RGB,(unsigned char *)Bac,index,showanno);
    
    
    for (y=0;y<rows;y++) {
      for (x=0;x<columns;x++) {
		image_data[step*y+x*3+0] = RGB[x][y][REDV  ];
		image_data[step*y+x*3+1] = RGB[x][y][GREENV];
		image_data[step*y+x*3+2] = RGB[x][y][BLUEV ];
      }
    }
    
    cvShowImage(WINDOWNAME, image);

    char c = cvWaitKey(30);
    
    if (c =='?') show_help();
    if (c =='q') { break; }
    if (c =='r') { iteration=0; } // Trigger reset of the counter
    if (c =='a') {
        if (showanno>1) showanno=0; else showanno++; // 0..1..2..0..1..2..0..1..2 etc
    }
    if (c =='p') {
        if (placed>1) placed=0; else placed=placed+1;
        printf("Placed = %d\n",placed);
    }
    if (c == 'h') v_STANDING_PERSON_HEIGHT = v_STANDING_PERSON_HEIGHT - 2;
    if (c == 'H') v_STANDING_PERSON_HEIGHT = v_STANDING_PERSON_HEIGHT + 2;
    
  }

  cvReleaseCapture(&capture);
  cvDestroyWindow(WINDOWNAME);

  return 0;
}

