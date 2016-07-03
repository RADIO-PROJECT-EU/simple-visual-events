      /**
       * Display video from webcam
       *
       * Author Nash
       * License GPL
       * Website http://nashruddin.com
       */



#include <stdio.h>
#include "cv.h"
#include "highgui.h"

#include "capture_v41.h"

#include "process_rgb.h"
#include "hw_model.h"
#include "command.h"


int delay;
     
struct imageRGB im_rgb;

IplImage* Capture(struct imageRGB* im_rgb);
//IplImage* ReadBMP(IplImage *frame, char *fname_head,unsigned int findex,struct imageRGB* im_rgb);

void CloseCamera();
void InitCamera();


int udp_open();

// Create memory for calculations
static CvMemStorage* storage = 0;

int command = 0;
int repeat = 0;

int main( int argc, char **argv )
{

      unsigned char *data;
      int step;
      char file_title[100];
	  char file_count_s[10];
	  int file_count;
      int findex;
      int i,j;


	  int line_pos;

      if (argc==2) {
          strcpy(file_title,argv[1]);
		  file_count=100;
      }
      else if (argc==3) {
		  strcpy(file_title,argv[1]);
		  strcpy(file_count_s,argv[2]);
		  sscanf(file_count_s,"%d",&file_count);
	  }
	  else
		file_title[0]=0;
	

	  printf("STARTED\n");
      
      IplImage *frame;
	  IplImage *zoomframe = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3); 
    
       
      /* initialize camera */
      InitCamera();
      InitProcess();
       
      /* create a window for the video */
      cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );

      storage = cvCreateMemStorage(0);

      udp_open();

      printf("\nPress 'H' for help....\n\n");



      findex = 0;
      
      while( command != 'q' ) {

          
        if ((file_title[0]!=0)&&(file_title[0]!='E'))
        {
            findex++;
            if (findex>file_count-1)
            {
                repeat++;
                printf("Repeat:%d\n",repeat);
                findex=0;
            }
	    //frame=ReadBMP(frame,file_title,findex,&im_rgb);
	     
            delay=50000000; while (delay>0) delay--;
        }
        else if (file_title[0]=='E') // 'E' means that the frame comes via ethernet from the DMV camera
        {
			
			// default initialisation values. should be updated with actual at the start of each packet
    
			eth_rows=512;
			eth_row_size=512;
			eth_rows_per_packet=1;  
    
			while(1) {
    
				line_pos = eth_rx_packet();

				if ( line_pos == 9999 ) {
					break;
				}
			}

		}
        else
        {
            frame=Capture(&im_rgb);
        }


        #if 1
            ProcessFrame(&im_rgb);
        #else
            for (i=1; i<im_rgb.rows; i++) {
                for (j=0; j<im_rgb.cols; j++) {
                    if (im_rgb.p[j][i][0] > im_rgb.p[j][i][1]) {
                        im_rgb.p[j][i][0] = 255; // red
                    }
                } // end of line
            }
      
        #endif


        // end of frame processing


        data= (unsigned char *)(frame->imageData);
        step = frame->widthStep;

        //printf("rows: %d   cols:%d  step:%d\n",frame.height, frame.width, step);
      
        for (i=1; i<im_rgb.rows; i++) {
            for (j=0; j<im_rgb.cols*3; j+= frame->nChannels) {
                  data[j]   = im_rgb.p[j/3][i][2];  // blue
                  data[j+1] = im_rgb.p[j/3][i][1]; // green
                  data[j+2] = im_rgb.p[j/3][i][0]; // red
            } // end of line
            data+= step;  // next line
        }
      
		#define ZOOM
		#ifdef ZOOM
		
		//zoomframe->width     = frame->width*2;
		//zoomframe->height    = frame->height*2;
		//zoomframe->nChannels = frame->nChannels;
		//zoomframe->widthStep = frame->widthStep;
		
		data= (unsigned char *)(zoomframe->imageData);
        step = zoomframe->widthStep;
		
		for (i=0;i<2*im_rgb.rows;i++) {
			for (j=0; j<6*im_rgb.cols;j+=zoomframe->nChannels)
			{
				  data[j]   = im_rgb.p[j/6][im_rgb.rows-i/2][2]; // blue
                  data[j+1] = im_rgb.p[j/6][im_rgb.rows-i/2][1]; // green
                  data[j+2] = im_rgb.p[j/6][im_rgb.rows-i/2][0]; // red
			}
			data+=step;
		}
			  
		/* display current frame */
        cvShowImage( "result", zoomframe );	  
			  
		#else
		
		
        /* display current frame */
        cvShowImage( "result", frame );
		
        #endif

	   
        command = GetUserCommand();
      
      }
       
      /* free memory */
      cvDestroyWindow( "result" );

      CloseCamera();
       
      return 0;
}
      


