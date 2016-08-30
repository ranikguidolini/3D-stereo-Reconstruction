/* *************** License:**************************
   Created by Ranik Guidolini
     
   Given a list of chessboard images and the number of corners (nx, ny)
   on the chessboards, saves the instrinsics and extrinsics parameter
   Give a video on the OpenCv suported format extracts 
   the frames and numerate each one with the prefix
   For example:
   ./frame video.mp4 l
   l0001.jpeg
   l0002.jpeg
   ...
   ************************************************** */

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
	int frameNumber = atoi (argv[3]);
	
	if (argc != 4)
	{
		cout << "-----------------------------------------------------------------------------------------------\n";
		cout << argv[0] << " <videoFileNmae> <prefix> <Number of Frames to Skip>\n";
		cout << "example:" << argv[0] << " Left.mp4 l 1 \"If you dont want to skip any frame\"\n";
		cout << "example:" << argv[0] << " Left.mp4 l 5 \"If you want to take one at each five frames\"\n";
		cout << "-----------------------------------------------------------------------------------------------\n";
		return -1;
	}
	
	
	//Escolher inicio numeraçao
	//Formato saida
	
	char name[15];
	IplImage* frame;
	int num=0, i=0;
	char s [20];
	//cvNamedWindow( "DisplayVideo", CV_WINDOW_AUTOSIZE );
	CvCapture* capture = cvCreateFileCapture( argv[1] );
	if (!capture)
	{
		cout << "não foi possivel abrir o arquiavo!" << endl;
		return -1;
	}
	
	while(1) 
	{
		num++;
		frame = cvQueryFrame( capture );
		
		if( !frame ) 
                	break;
		
		char c = cvWaitKey(33);
		if( c == 27 ) 
			break;
		
		if ((num % frameNumber) == 0)                  //descarta uma parte dos frames
		{
			//cvShowImage( "DisplayVideo", frame );
			sprintf(name, "%s%04d.jpeg", argv[2], i);
			cvSaveImage(name, frame);
			i++;
		}
	}
	cvReleaseCapture( &capture );
	//cvDestroyWindow("DisplayVideo" );
}