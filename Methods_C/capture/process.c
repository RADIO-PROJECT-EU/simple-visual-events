
/* Algorithm parameters :
 *
 * COMMAND                  : Number of operation to be executed (default for RADIO algorithm: 5)
 * ALGO_?                   : If defined, algorithm No ? is enabled in command 5
 * SENSITIVITY              : How many changed pixels in each block will mean the block is chamged
 * STANDING_PERSON_HEIGHT   : If we detect changes above this line, the person is standing-up
 * OUTOFBED_LEFT / RIGHT    : If centre of activity is outside these columns, the person is off-bed
 * CUPX / CUPY              : Coordinates of the centre of the medication cup region
 * CUPR                     : Size (in pixels) of the region where the medication cup is placed
 * CUPTHRESHOLD             : Threshold in difference in pixel intensityto detect a chnage in the cup region
 * CUPTHRSCOUNT             : Count of changed pixels that mean a cup has been moved   
 *
 */ 
 
#define COMMAND 5
#define ALGO_1
#define ALGO_2
#define SENSITIVITY 30
#define STANDING_PERSON_HEIGHT 390
#define OUTOFBED_LEFT 20
#define OUTOFBED_RIGHT 500

#define CUPX 500
#define CUPY 200
#define CUPR 40
#define CUPTHRESHOLD 80
#define CUPTHRSCOUNT 30

unsigned int v_STANDING_PERSON_HEIGHT;
unsigned int v_OUTOFBED_LEFT;
unsigned int v_OUTOFBED_RIGHT;



/* Global variables
 *
 */
 
unsigned int cup[CUPR*CUPR*4];
unsigned char tempframe[640*480*3];

int placed;

unsigned int x1p,x2p,y1p,y2p,cxp, cyp;
unsigned int top, centerx, centery;
unsigned int gotup, cupmoved;

/* process_function(grb_a, rgb_b,command, showanno)
 *
 * processes two frames
 *
 * rgb_a     = the first frame  ( array[640*480*3] )
 * rgb_b     = the second frame ( array[640*480*3] )
 * command   = a specific operation to be applied on the frame
 * showanno  = 0 : no image annotations, 1 : show chnaged blocks, 2 : show all annotations
 *
 */
int process_function (unsigned char *rgb_a, unsigned char *rgb_b,unsigned char command, unsigned int showanno) {


    unsigned int x,y;
                     
    unsigned long int cx, cy, x1,x2,y1,y2,n,nn;

	switch(command) {
		case 0 : // do nothing
			 break;		
		case 1 : // copy a to b
                {
                for (y=0;y<480;y++) {
                  for (x=0;x<640;x++) {
                    rgb_b[(x*480+y)*3+REDV  ] = rgb_a[(x*480+y)*3+REDV  ];
                    rgb_b[(x*480+y)*3+GREENV] = rgb_a[(x*480+y)*3+GREENV];
                    rgb_b[(x*480+y)*3+BLUEV ] = rgb_a[(x*480+y)*3+BLUEV ];
                  }
                }
                }
                break;
        case 2 : // diff with b and store in a
                {
                for (y=0;y<480;y++) {
                  for (x=0;x<640;x++) {
                    rgb_a[(x*480+y)*3+REDV  ] = abs(rgb_a[(x*480+y)*3+REDV  ] - rgb_b[(x*480+y)*3+REDV  ] );
                    rgb_a[(x*480+y)*3+GREENV] = abs(rgb_a[(x*480+y)*3+GREENV] - rgb_b[(x*480+y)*3+GREENV] );
                    rgb_a[(x*480+y)*3+BLUEV ] = abs(rgb_a[(x*480+y)*3+BLUEV ] - rgb_b[(x*480+y)*3+BLUEV ] );
                  }
                }
                }
                break;
        case 3 : // diff with b and store in a, store previous a in b
                 // copy a to tempframe
                 {
                for (y=0;y<480;y++) {
                  for (x=0;x<640;x++) {
                    tempframe[(x*480+y)*3+REDV  ] = rgb_a[(x*480+y)*3+REDV  ];
                    tempframe[(x*480+y)*3+GREENV] = rgb_a[(x*480+y)*3+GREENV];
                    tempframe[(x*480+y)*3+BLUEV ] = rgb_a[(x*480+y)*3+BLUEV ];
                  }
                }
                // copy diff(a,b) to a
                for (y=0;y<480;y++) {
                  for (x=0;x<640;x++) {
                    rgb_a[(x*480+y)*3+REDV  ] = abs(rgb_a[(x*480+y)*3+REDV  ] - rgb_b[(x*480+y)*3+REDV  ] );
                    rgb_a[(x*480+y)*3+GREENV] = abs(rgb_a[(x*480+y)*3+GREENV] - rgb_b[(x*480+y)*3+GREENV] );
                    rgb_a[(x*480+y)*3+BLUEV ] = abs(rgb_a[(x*480+y)*3+BLUEV ] - rgb_b[(x*480+y)*3+BLUEV ] );
                  }
                }
                // copy tempframe to b
                for (y=0;y<480;y++) {
                  for (x=0;x<640;x++) {
                    rgb_b[(x*480+y)*3+REDV  ]  = tempframe[(x*480+y)*3+REDV  ];
                    rgb_b[(x*480+y)*3+GREENV]  = tempframe[(x*480+y)*3+GREENV];
                    rgb_b[(x*480+y)*3+BLUEV ]  = tempframe[(x*480+y)*3+BLUEV ];
                  }
                }
                }
                break;       
        case 4 : // show centre of activity
                {
                
                cx=0;
                cy=0;     
                n = 0;
                 
                for (y=0;y<480;y++) {
                  for (x=0;x<640;x++) {
                    tempframe[(x*480+y)*3+REDV  ] = rgb_a[(x*480+y)*3+REDV  ];
                    tempframe[(x*480+y)*3+GREENV] = rgb_a[(x*480+y)*3+GREENV];
                    tempframe[(x*480+y)*3+BLUEV ] = rgb_a[(x*480+y)*3+BLUEV ];
                  }
                }
                // copy diff(a,b) to a
                for (y=0;y<480;y++) {
                  for (x=0;x<640;x++) {
                    if ((abs(rgb_a[(x*480+y)*3+REDV  ] - rgb_b[(x*480+y)*3+REDV  ] ))>40) { 
                        rgb_a[(x*480+y)*3+REDV  ] = 70; 
                        cx = cx + x;
                        cy = cy + y;
                        n = n + 1;
                    }
                    else 
                        rgb_a[(x*480+y)*3+REDV  ] = 0;
                    
                    if ((abs(rgb_a[(x*480+y)*3+GREENV] - rgb_b[(x*480+y)*3+GREENV] ))>40) {
                        rgb_a[(x*480+y)*3+GREENV] = 70; 
                        cx = cx + x;
                        cy = cy + y;
                        n = n + 1;
                    }
                    else 
                        rgb_a[(x*480+y)*3+GREENV] = 0;
                        
                    if ((abs(rgb_a[(x*480+y)*3+BLUEV ] - rgb_b[(x*480+y)*3+BLUEV ] ))>40) {
                        rgb_a[(x*480+y)*3+BLUEV ] = 70; 
                        cx = cx + x;
                        cy = cy + y;
                        n = n + 1;
                    }
                    else 
                        rgb_a[(x*480+y)*3+BLUEV ] = 0;
                  }
                }
                
                if (n>0) {
                    cx = cx/n;
                    cy = cy/n;
                }
                
                if ((cx<6)||(cx>634)||(cy<6)||(cy>474)) {
                // do nothing
                } else {
                     for (x=cx-5;x<cx+5;x++) {
                       for (y=cy-5;y<cy+5;y++) {
                          rgb_a[(x*480+y)*3+BLUEV ] = 255; 
                       }
                     }
                }
                
                // copy tempframe to b
                for (y=0;y<480;y++) {
                  for (x=0;x<640;x++) {
                    rgb_b[(x*480+y)*3+REDV  ]  = tempframe[(x*480+y)*3+REDV  ];
                    rgb_b[(x*480+y)*3+GREENV]  = tempframe[(x*480+y)*3+GREENV];
                    rgb_b[(x*480+y)*3+BLUEV ]  = tempframe[(x*480+y)*3+BLUEV ];
                  }
                }
                }
                break; 
        case 5 : // Main RADIO algorithm for events 1 and 2
                 // Event 1 : Time to get out of bed
                 // Event 2 : Medication cup chnage
                {
                
                                
                unsigned int xx,yy;   
                unsigned int s;
                int tmpi;
                
                x1=638; x2=1;
                y1=438; y2=1;
                 
                cx = 0; cy = 0; nn = 0;
                 
                // Store rgb in tempframe (make sure we do not loose it due to annotation)
                for (y=0;y<480;y++) {
                  for (x=0;x<640;x++) {
                    tempframe[(x*480+y)*3+REDV  ] = rgb_a[(x*480+y)*3+REDV  ];
                    tempframe[(x*480+y)*3+GREENV] = rgb_a[(x*480+y)*3+GREENV];
                    tempframe[(x*480+y)*3+BLUEV ] = rgb_a[(x*480+y)*3+BLUEV ];
                  }
                }

                #ifdef ALGO_1
                s=5; // size of each block
                
                // find diff in squares of size s, and find top,bot,right,left
                for (yy=s;yy<480-s;yy+=2*s) {
                  for (xx=s;xx<640-s;xx+=2*s) {
                                    
                    n=0;
                    
                    for (x=xx-s;x<xx+s;x++) {
                    for (y=yy-s;y<yy+s;y++) {

                
                    if ((abs(rgb_a[(x*480+y)*3+REDV  ] - rgb_b[(x*480+y)*3+REDV  ] ))>40) { 
                        if (showanno>0) rgb_a[(x*480+y)*3+REDV  ] = 70; 
                        n = n + abs(rgb_a[(x*480+y)*3+REDV  ] - rgb_b[(x*480+y)*3+REDV  ] );
                    }
                    else 
                        if (showanno>0) rgb_a[(x*480+y)*3+REDV  ] = rgb_a[(x*480+y)*3+REDV  ] /4;
                    
                    if ((abs(rgb_a[(x*480+y)*3+GREENV] - rgb_b[(x*480+y)*3+GREENV] ))>40) {
                        if (showanno>0) rgb_a[(x*480+y)*3+GREENV] = 70; 
                        n = n + abs(rgb_a[(x*480+y)*3+GREENV ] - rgb_b[(x*480+y)*3+GREENV ] );
                    }
                    else 
                        if (showanno>0) rgb_a[(x*480+y)*3+GREENV] = rgb_a[(x*480+y)*3+GREENV] /4;
                        
                    if ((abs(rgb_a[(x*480+y)*3+BLUEV ] - rgb_b[(x*480+y)*3+BLUEV ] ))>40) {
                        if (showanno>0) rgb_a[(x*480+y)*3+BLUEV ] = 70; 
                        n = n + abs(rgb_a[(x*480+y)*3+BLUEV  ] - rgb_b[(x*480+y)*3+BLUEV ] );
                    }
                    else 
                        if (showanno>0) rgb_a[(x*480+y)*3+BLUEV ] = rgb_a[(x*480+y)*3+BLUEV ] /4;
                        
                    }
                    }
                                    
                    n = n / (s*s*4);
                                    
                    if (n>SENSITIVITY) {
                        if (x<x1) x1=x;
                        if (x>x2) x2=x;
                        if (y<y1) y1=y;
                        if (y>y2) y2=y;
                        
                        cx = cx + x;
                        cy = cy + y;
                        nn = nn + 1;
                        
                        if (showanno>0) {
                            for (x=xx-s;x<xx+s;x++) {
                            for (y=yy-s;y<yy+s;y++) {
                                if (rgb_a[(x*480+y)*3+BLUEV ]<128)
                                    rgb_a[(x*480+y)*3+BLUEV ] = rgb_a[(x*480+y)*3+BLUEV ] + 128;
                                else
                                    rgb_a[(x*480+y)*3+BLUEV ] = 255;
                            }
                            }
                        }
                    }
                  }
                }

                
           
                // highlight bounding rectangle (top left , bot right)
                
                if ((x2>x1)&&(y2>y1)) //valid rectangle
                {
                    x1p=x1; y1p=y1; x2p=x2; y2p=y2;  top = y2p;
                    if (nn>0) { cxp=cx/nn; cyp=cy/nn; centerx=cxp; centery=cyp;}
                    
                }
                //else use previous

                //make sure we will not get crazy values for edges of rect
                if ((x1p<1)||(x2p>639)||(y1p<1)||(y2p>479))
                {
                    x1p=1;x2p=639;y1p=1;y2p=439; cxp= 320; cyp= 240;
                }
                    
                if (showanno>1) {
                    for (x=x1p;x<x2p;x++) rgb_a[(x*480+y1p)*3+REDV ] = 255; 
                    for (x=x1p;x<x2p;x++) rgb_a[(x*480+y2p)*3+REDV ] = 255; 
                    for (y=y1p;y<y2p;y++) rgb_a[(x1p*480+y)*3+REDV ] = 255; 
                    for (y=y1p;y<y2p;y++) rgb_a[(x2p*480+y)*3+REDV ] = 255; 
                
                    for (x=2;x<638;x++) {
                        rgb_a[(x*480+v_STANDING_PERSON_HEIGHT)*3+GREENV] = 255;
                        // rgb_a[(x*480+top                   )*3+BLUEV] = 255; // uncomment this to show hight in red
                    }
                    for (y=2;y<438;y++) {
                        rgb_a[(OUTOFBED_LEFT*480 +y)*3+GREENV] = 255;
                        rgb_a[(OUTOFBED_RIGHT*480+y)*3+GREENV] = 255;
                    }
                
                
                    if ((cxp<30)||(cxp>610)||(cyp<30)||(cyp>450)) {
                    // do nothing
                    } else {
                        for (x=cxp-30;x<cxp+30;x++) {
                            for (y=cyp-30;y<cyp+30;y++) {
                                tmpi = 255 - ((x-cxp)*(x-cxp)+(y-cyp)*(y-cyp))/3;
                                if (tmpi>0) rgb_a[(x*480+y)*3+GREENV ] = tmpi;
                            }
                        }
                    }
                    
                }
                 
                #else
                gotup = 0;
                top = 0;
                #endif
                
                #ifdef ALGO_2
                // Process the cup region
                int cupi=0;
                int cupd=0;
                int cupdiff=0;
                for (x=CUPX-CUPR; x<CUPX+CUPR; x++) {
                    for (y=CUPY-CUPR; y<CUPY+CUPR; y++) {
                        if (placed==0) { // the moment when the cup is placed
                            cup[cupi]=rgb_a[(x*480 + y)*3+REDV]+rgb_a[(x*480 + y)*3+GREENV]+rgb_a[(x*480 + y)*3+BLUEV];
                            cupi++;
                            cupd = 0;
                        }
                        else if (placed==2) { // when we want to check
                            cupd = rgb_a[(x*480 + y)*3+REDV]+rgb_a[(x*480 + y)*3+GREENV]+rgb_a[(x*480 + y)*3+BLUEV];
                            cupd = abs(cupd - cup[cupi]);
                            if ( cupd>CUPTHRESHOLD ) {
                                cupdiff++;
                                if (showanno>0) rgb_a[(x*480 + y)*3+GREENV] = 255;
                            }
                            cupi++;
                        }
                    }
                }
                if ((placed==2)&&(cupdiff>CUPTHRSCOUNT)) cupmoved=1; else cupmoved=0; 
                
                if (showanno==2) {
                   // Show region for cup
                    for (x=CUPX-CUPR; x<CUPX+CUPR; x++) {
                        rgb_a[(x*480 + CUPY + CUPR)*3+GREENV] = 255;
                        rgb_a[(x*480 + CUPY - CUPR)*3+GREENV] = 255;
                    }
                    for (y=CUPY-CUPR; y<CUPY+CUPR; y++) {
                        rgb_a[((CUPX+CUPR)*480 + y)*3+GREENV] = 255;
                        rgb_a[((CUPX-CUPR)*480 + y)*3+GREENV] = 255;
                    }
                    
                    
                }
                
                #else
                cupmoved=0;
                #endif
                
                // copy tempframe to b
                for (y=0;y<480;y++) {
                  for (x=0;x<640;x++) {
                    rgb_b[(x*480+y)*3+REDV  ]  = tempframe[(x*480+y)*3+REDV  ];
                    rgb_b[(x*480+y)*3+GREENV]  = tempframe[(x*480+y)*3+GREENV];
                    rgb_b[(x*480+y)*3+BLUEV ]  = tempframe[(x*480+y)*3+BLUEV ];
                  }
                }
                }
                break;                 
 	}

	return 0;
}


int process(unsigned char *rgb_a, unsigned char *rgb_b,unsigned int index, unsigned int showanno) {
    
    unsigned int height, isup, oob;

    if (index<10) gotup=0;
    
    process_function (rgb_a, rgb_b,COMMAND, showanno);

    height = top;

    #ifdef ALGO_1
    if (height>v_STANDING_PERSON_HEIGHT) isup=1; else isup=0;
    if ((centerx<OUTOFBED_LEFT)||(centerx>OUTOFBED_RIGHT)) oob=1; else oob=0;
    
    if ((isup==1)&&(gotup==0)&&(index>9))  { gotup=1; printf("Standing [after %05d Frames]\n",index); }
    
    if ((oob==1)&&(isup==1)) printf("Walking !!\n");
    
    if ((oob==1)&&(isup==0)) printf("* * * Warning : Out of Bed but not Standing up!  * * * \n");
    #endif
    
    #ifdef ALGO_2
    if (cupmoved==1) printf("The cup was moved!\n");
    #endif
}
