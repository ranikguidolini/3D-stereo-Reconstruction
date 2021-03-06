/* *************** License:**************************
   Created by Ranik Guidolini
     
   Given a list of chessboard images and the number of corners (nx, ny)
   on the chessboards, saves the instrinsics and extrinsics parameter
   Give a video on the OpenCv suported format extracts 
   the frames and numerate each one with the prefix
   Create a .txt file with a list of images name ending
   in the number passed.
   For example:
   ./crialista 2
   ./crialista 34 -tp 3D_L4 504 3D_R4 425 3 .JPG
   ************************************************** */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

using namespace std;

int main ( int argc, char** argv ) 
{	
	int i=0, j=0, k=0, flag=1, set=4;
	string left="l", right="r", format = ".jpeg";
	
	if (argc < 2)
	{
		cout << argv[0] << " <maxNumber>\n";
		return -1;
	}
	if (argc > 2 && !strcmp(argv[2], "-tp"))
	{
		left = argv[3];
		j = atoi(argv[4]);
		right = argv[5];
		k = atoi(argv[6]);
		set = atoi(argv[7]);
		format = argv[8];
	} 
	
	ofstream lista;
	lista.open ("list.txt");
	int max = atoi(argv[1]) + 1;
	
	for (; i < max; i++, j++, k++)
	{
		//if (flag)
		//{
			lista << left << setfill('0') << setw(set) << j << format << endl;
			lista << right << setfill('0') << setw(set) << k << format << endl;
		//}
		//else
		//{
		//	lista << left  << j << ".jpeg" << endl;
		//	lista << right << k << ".jpeg" << endl;
		//}
		/*lista << i << "-left.bmp" << endl;
		lista << i << "-right.bmp" << endl;*/
	}
	lista.close();
	return 0;
}