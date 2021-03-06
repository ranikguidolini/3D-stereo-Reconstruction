#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sstream>

using namespace std;

static void
stereo_rectfy (const char* imageList, int savePairs, int x, int y, int w, int h)
{
    char nome[150];
    bool isVerticalStereo = false;//OpenCV can handle left-right or up-down camera arrangements
    const int maxScale = 1;
    FILE* f = fopen(imageList, "rt");
    int i, j, lr, nframes, /*n = nx*ny,*/ N = 0;
    vector<string> imageNames[2];
    vector<CvPoint3D32f> objectPoints;
    vector<CvPoint2D32f> points[2];
    vector<int> npoints;
    vector<uchar> active[2];
    //vector<CvPoint2D32f> temp(n);
    CvSize imageSize = {0,0};
    // ARRAY AND VECTOR STORAGE:
    double M1[3][3], M2[3][3], D1[5], D2[5];
    double R[3][3], T[3], E[3][3], F[3][3];
    double Q[4][4];
    CvMat _M1 = cvMat(3, 3, CV_64F, M1 );
    CvMat _M2 = cvMat(3, 3, CV_64F, M2 );
    CvMat _D1 = cvMat(1, 5, CV_64F, D1 );
    CvMat _D2 = cvMat(1, 5, CV_64F, D2 );
    CvMat _R = cvMat(3, 3, CV_64F, R );
    CvMat _T = cvMat(3, 1, CV_64F, T );
    CvMat _E = cvMat(3, 3, CV_64F, E );
    CvMat _F = cvMat(3, 3, CV_64F, F );
    CvMat _Q = cvMat(4,4, CV_64F, Q);
     
// READ IN THE LIST OF CHESSBOARDS:
    if( !f )
    {
        fprintf(stderr, "Can not open the list of images file.\n\n");
        return;
    }


	
    for(i=0;;i++)
    {
        char buf[1024];
        int count = 0, result=0;
        lr = i % 2;
        vector<CvPoint2D32f>& pts = points[lr];
        if( !fgets( buf, sizeof(buf)-3, f ))
            break;
        size_t len = strlen(buf);
        while( len > 0 && isspace(buf[len-1]))
            buf[--len] = '\0';
        if( buf[0] == '#')
            continue;
        IplImage* img = cvLoadImage( buf, 0 );
        if( !img )
            break;
	printf("%s\n", buf);
        imageSize = cvGetSize(img);
        imageNames[lr].push_back(buf);

        cvReleaseImage( &img );
    }
    fclose(f);
    nframes = i/2;

    CvMat* aux;
    

    
    aux = (CvMat*) cvLoad("M1.xml");
    if (!aux)
	cout << "Could not read matrix M1\n";
    _M1 = *aux;    
    aux = (CvMat*) cvLoad("D1.xml");
    if (!aux)
	cout << "Could not read matrix D1\n";
    _D1 = *aux;    
    aux = (CvMat*) cvLoad("M2.xml");
    if (!aux)
	cout << "Could not read matrix M2\n";
    _M2 = *aux;    
    aux = (CvMat*) cvLoad("D2.xml");
    if (!aux)
	cout << "Could not read matrix D2\n";
    _D2 = *aux;   
    aux = (CvMat*) cvLoad("Q.xml");
    if (!aux)
	cout << "Could not read matrix Q\n";
    _Q = *aux;    
    aux = (CvMat*) cvLoad("R.xml");
    if (!aux)
	cout << "Could not read matrix R\n";
    _R = *aux;    
    aux = (CvMat*) cvLoad("T.xml");
    if (!aux)
	cout << "Could not read matrix T\n";
    _T = *aux;

//COMPUTE AND DISPLAY RECTIFICATION
        CvMat* mx1 = cvCreateMat( imageSize.height,
            imageSize.width, CV_32F );
        CvMat* my1 = cvCreateMat( imageSize.height,
            imageSize.width, CV_32F );
        CvMat* mx2 = cvCreateMat( imageSize.height,
            imageSize.width, CV_32F );
        CvMat* my2 = cvCreateMat( imageSize.height,
            imageSize.width, CV_32F );
        CvMat* img1r = cvCreateMat( imageSize.height,
            imageSize.width, CV_8U );
        CvMat* img2r = cvCreateMat( imageSize.height,
            imageSize.width, CV_8U );
        CvMat* disp = cvCreateMat( imageSize.height,
            imageSize.width, CV_16S );
        CvMat* vdisp = cvCreateMat( imageSize.height,
            imageSize.width, CV_8U );
        CvMat* pair;
        double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
        CvMat _R1 = cvMat(3, 3, CV_64F, R1);
        CvMat _R2 = cvMat(3, 3, CV_64F, R2);
	
        // IF BY CALIBRATED (BOUGUET'S METHOD)
	aux = (CvMat*) cvLoad("R1.xml");
	_R1 = *aux;   
	aux = (CvMat*) cvLoad("R2.xml");
	_R2 = *aux;
	
        CvMat _P1 = cvMat(3, 4, CV_64F, P1);
        CvMat _P2 = cvMat(3, 4, CV_64F, P2);
	
	aux = (CvMat*) cvLoad("P1.xml");
	_P1 = *aux;
	aux = (CvMat*) cvLoad("P2.xml");
	_P2 = *aux;
	
	cvStereoRectify (&_M1, &_M2, &_D1, &_D2, imageSize, &_R, &_T, &_R1, &_R2, &_P1, &_P2, &_Q, 0/*CV_CALIB_ZERO_DISPARITY*/);
	//isVerticalStereo = fabs(P2[1][3]) > fabs(P2[0][3]);
        //Precompute maps for cvRemap()
        cvInitUndistortRectifyMap(&_M1,&_D1,&_R1,&_P1,mx1,my1);
        cvInitUndistortRectifyMap(&_M2,&_D2,&_R2,&_P2,mx2,my2);

// RECTIFY THE IMAGES AND FIND DISPARITY MAPS
        if( !isVerticalStereo )
            pair = cvCreateMat( imageSize.height, imageSize.width*2, CV_8UC3 );
        else
            pair = cvCreateMat( imageSize.height*2, imageSize.width, CV_8UC3 );
	
	cout << "Saving rectified images...\n";
	for (i = 0; i < nframes; i++)
        {
            IplImage* img1 = cvLoadImage(imageNames[0][i].c_str(),0);
            IplImage* img2 = cvLoadImage(imageNames[1][i].c_str(),0);
	    
            if (img1 && img2)
            {
                CvMat part;
                cvRemap( img1, img1r, mx1, my1 );
                cvRemap( img2, img2r, mx2, my2 );
		
		if (x > 0)   //x > 0 means the image must be cropped
		{
			// Transform it into the C++ cv::Mat format
			cv::Mat imagel(img1r); 
			cv::Mat imager(img2r); 

			// Setup a rectangle to define your region of interest
			cv::Rect myROI(x, y, w, h);

			// Crop the full image to that image contained by the rectangle myROI
			// Note that this doesn't copy the data
			imagel = imagel(myROI);
			imager = imager(myROI);
			
			sprintf(nome, "recL%d.png", i);
			imwrite(nome, imagel);
			sprintf(nome, "recR%d.png", i);
			imwrite(nome, imager);
		}
		else
		{
			sprintf(nome, "recL%d.png", i);
			cvSaveImage(nome, img1r);
			sprintf(nome, "recR%d.png", i);
			cvSaveImage(nome, img2r);
		}
		
		if (savePairs != 0)
		{
			if (i < 1)
				cout << "Saving rectified pairs with epipolar lines...\n";   //Only print message on the first loop
			
			if( !isVerticalStereo )
			{
				cvGetCols( pair, &part, 0, imageSize.width );
				cvCvtColor( img1r, &part, CV_GRAY2BGR );
				cvGetCols( pair, &part, imageSize.width, imageSize.width*2 );
				cvCvtColor( img2r, &part, CV_GRAY2BGR );
				for( j = 0; j < imageSize.height; j += 16 )
				cvLine( pair, cvPoint(0,j),
				cvPoint(imageSize.width*2,j),
				CV_RGB(0,255,0));
			}
			else
			{
				cvGetRows( pair, &part, 0, imageSize.height );
				cvCvtColor( img1r, &part, CV_GRAY2BGR );
				cvGetRows( pair, &part, imageSize.height, imageSize.height*2 );
				cvCvtColor( img2r, &part, CV_GRAY2BGR );
				for( j = 0; j < imageSize.width; j += 16 )
				cvLine( pair, cvPoint(j,0),
				cvPoint(j,imageSize.height*2),
				CV_RGB(0,255,0));
			}
			sprintf (nome, "par%d.jpeg", i);
			cvSaveImage (nome, pair);
		}
	   }
		cvReleaseImage (&img1);
		cvReleaseImage (&img2);
	}
        cvReleaseMat( &mx1 );
        cvReleaseMat( &my1 );
        cvReleaseMat( &mx2 );
        cvReleaseMat( &my2 );
        cvReleaseMat( &img1r );
        cvReleaseMat( &img2r );
        cvReleaseMat( &disp );
	cout << "Done!" << endl;
}

int 
main(int argc, char *argv[])
{
	int i=0, savePairs=0, x=0, y=0, w=0, h=0;
	if (argc < 2 || argc > 8)
	{
		fprintf (stderr,"USAGE: %s imageList -sp -c <x> <y> <w> <h>\n",argv[0]);
		fprintf (stderr,"\t -sp : Optional, if added it will save the pairs of rectified images with epipolar lines\n");
		fprintf (stderr,"\t -c : Optional, if added it will save the pairs of rectified images with epipolar lines\n");
		return 1;
	} 
	
	if (argc >= 3)
	{
		if (!strcmp (argv[2], "-sp"))
			savePairs = 1;
		
		if (!strcmp (argv[2], "-c"))
			i = 3;
		if (argc > 3 && !strcmp (argv[3], "-c"))
			i = 4;
		
		if (i > 0)
		{
			
			x = atoi (argv[i]);
			y = atoi (argv[i+1]);
			w = atoi (argv[i+2]);
			h = atoi (argv[i+3]);
			
			cout << x << endl<< y << endl<< w << endl<< h << endl;
		}
	}
	cout << savePairs << endl;
	stereo_rectfy(argv[1], savePairs, x, y, w, h);
	return 0;
}