// matrix test wally.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <sstream> 
#include <fstream>
#include <istream>
#include <string>
#include "Matrix.h"

using namespace std;

#pragma region Georges stuff
void WritePGM(char *filename, float *data, int sizeR, int sizeC, int Q)
{

	int i,j;
	unsigned char *image;
	ofstream myfile;

	image = (unsigned char *) new unsigned char [sizeR*sizeC];

	// convert the integer values to unsigned char

	for(i=0; i<sizeR*sizeC; i++)
		image[i]=(unsigned char)data[i];

	myfile.open(filename, ios::out|ios::binary|ios::trunc);

	if (!myfile) {
		cout << "Can't open file: " << filename << endl;
		exit(1);
	}

	myfile << "P5" << endl;
	myfile << sizeC << " " << sizeR << endl;
	myfile << Q << endl;

	myfile.write( reinterpret_cast<char *>(image), (sizeR*sizeC)*sizeof(unsigned char));

	if (myfile.fail()) {
		cout << "Can't write image " << filename << endl;
		exit(0);
	}

	myfile.close();

	delete [] image;

}

float* readTXT(string fileName, int sizeR, int sizeC)
{
	float* data = new float[sizeR*sizeC];
	int i=0;
	ifstream myfile (fileName);
	if (myfile.is_open())
	{

		while ( myfile.good())
		{
			if (i>sizeR*sizeC-1) break;
			myfile >> *(data+i);
			// cout << *(data+i) << ' '; // This line display the converted data on the screen, you may comment it out. 
			i++;                                                             
		}
		myfile.close();
	}

	else cout << "Unable to open file"; 
	//cout << i;

	return data;
}
#pragma endregion

#pragma region SSD search
float sumDifferenceSquared(Matrix& M1,Matrix& M2, int rows, int columns)
{
	float total = 0;
	//loop through every pixel
	for(int i = 0; i<rows;i++)
	{
		for( int j = 0; j<columns;j++)
		{
			if(M1.ReturnValue(i,j) != 255)
			{
				//sum up the difference between the pixels squared
				total += (M1.ReturnValue(i,j)-M2.ReturnValue(i,j));
			}
		}
	}
	//return the SSD
	return (float)pow(total,2);;
}

float ComparePositions(Matrix& scene, Matrix& wally, int wRows, int wCol, int sRows, int sCol)
{
	//create a pointer for transfering data
	float* data;
	//create variables to store the SSD and the best SSD, ste the best SSD to an arbitrary large number
	float total = 0,bestTotal = (float)INT_MAX;
	int posX, posY;
	//display some output to show that the code is working
	cout<<"working on row:"<<endl;
	//loop through every position in which the image of wally will fit
	for(int i = 0; i<sRows-wRows;i++)
	{
		for(int j = 0; j<sCol-wCol;j++)
		{
			//get the block from the image
			data = scene.GetBlock(i,i+49,j,j+36);
			//store the block in a matrix
			Matrix sceneBlock(49,36,data);
			delete []data;
			//recieve the positions and total for that comparison
			total = sumDifferenceSquared(wally,sceneBlock,wRows,wCol);
			if(total < bestTotal)
			{
				bestTotal = total;
				posX = j;
				posY = i;
			}
		}
		//if the row is divisable by 50 then tell the user the current row
		if(i%50 == 0)
		{
			cout<<"s: "<<i<<endl;
		}
	}
	//create a black matrix
	Matrix box(wRows,wCol,5);
	//place it over wally
	scene.AddBlock(box,posY,posX,posY+wRows,posX+wCol);
	WritePGM("BestMatchSSD.pgm",scene.GetData(),sRows,sCol,255);
	return 1;
}

#pragma endregion

#pragma region NCC search

float calcMean(Matrix& block)
{
	float total = 0;
	for(int i = 0; i<block.GetRows();i++)
	{
		for(int j = 0;j<block.GetColumns();j++)
		{
			total += block.ReturnValue(i,j);
		}
	}
	//return the mean value of the matrix
	return total/block.ReturnSize();
}
Matrix meanSub(Matrix& block, float mean)
{
	float* data = block.GetData();
	for(int i = 0;i<block.ReturnSize();i++)
	{
		*(data+i) = *(data+i) - mean;
	}
	Matrix normalized(block.GetRows(),block.GetColumns(),data);
	delete []data;
	//return a matrix where each element represents how much that element used to deviate from the mean
	return normalized;
}
float calctop(Matrix& wally, Matrix& block)
{
	float total = 0;
	for(int i = 0;i<wally.GetRows();i++)
	{
		for(int j = 0;j<wally.GetColumns();j++)
		{
			total += wally.ReturnValue(i,j)*block.ReturnValue(i,j);
		}
	}
	//multiply the normalised matrices together for the top part of the equation
	return total;
}
float squareBlock(Matrix& block)
{
	float total = 0;
	float* data = block.GetData();
	for(int i = 0; i<block.ReturnSize();i++)
	{
		total += pow(*(data+i),2);
	}
	delete []data;
	//return the value of the entire marix squared and then totaled
	return total;
}
float multiplyBlocks(Matrix& block, Matrix& wally)
{
	float total = 0;
	for(int i = 0; i<wally.GetRows();i++)
	{
		for(int j = 0; j<wally.GetColumns();j++)
		{
			total += block.ReturnValue(i,j)*wally.ReturnValue(i,j);
		}
	}
	//multiply and then sum 2 matrices
	return total;
}
void computeNCC(Matrix& wally,Matrix& scene,int wRow,int wCol,int sRow,int sCol)
{
	float wallyMean = calcMean(wally);
	Matrix wallyNorm(meanSub(wally,wallyMean));
	float *data;
	float blockMean;
	float ncc = 0;
	float bestncc = -1;
	int x,y;
	cout<<"searching through row:";
	for(int i = 0; i<719;i++)
	{
		for(int j = 0; j<sCol-wCol;j++)
		{
			//perfom the ncc algorithm
			data = scene.GetBlock(i,i+wRow,j,j+wCol);
			Matrix sceneBlock(wRow,wCol,data);
			delete []data;
			blockMean = calcMean(sceneBlock);
			Matrix blockNorm = meanSub(sceneBlock,blockMean);
			float top = multiplyBlocks(wallyNorm,blockNorm);
			float botLeft = squareBlock(wallyNorm);
			float botRight = squareBlock(blockNorm);
			float bottom = sqrt(botLeft*botRight);
			ncc = top/bottom;
			if(ncc>bestncc)
			{
				bestncc = ncc;
				y = i;
				x = j;
			}
		}
		
		if(i%50==0)
		{
			cout<<i<<endl;
		}
	}
	cout<<endl;
	Matrix box(wRow,wCol,5);
	//place the box around wally in the scene
	scene.AddBlock(box,y,x,y+wRow,x+wCol);
	data= scene.GetData();
	cout<<"writing the result"<<endl;
	WritePGM("BestMatchNCC.pgm",data,sRow,sCol,255);
	delete []data;
}
#pragma endregion

int main()
{
	{
		//ask what algorith to use
		cout<<"enter n for NCC or s for SSD: ";
		char answer;
		cin>>answer;
		//load the matrices
		cout<<"loading wally"<<endl;
		float *data = readTXT("Wally_grey.txt",49,36);
		Matrix wally(49,36,data);
		
		delete []data;
		cout<<"loading the scene"<<endl;
		data = readTXT("Cluttered_scene.txt",768,1024);
		Matrix scene(768,1024,data);
		
		delete []data;
		cout<<"Begining search"<<endl;
		//perform the appropriate algorithm
		if(answer == 'n')
		{
			cout<<"begining NCC search"<<endl;
		computeNCC(wally,scene,49,36,768,1024);
		}
		else
		{
			cout<<"Begining SSD search"<<endl;
			ComparePositions(scene,wally,49,36,768,1024);
		}
	}
	//check for leaks
	_CrtDumpMemoryLeaks();

	return 0;
}

