
#include "stdafx.h"
#include<iostream>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <sstream> 
#include <fstream>
#include <istream>
#include<string>
#include "Matrix.h"
#include "BinaryMartix.h"

using namespace std;

#pragma region Georges stuff
void WritePGM(string filename, float *data, int sizeR, int sizeC, int Q)
{
	int i;
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
	delete [] data;

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

#pragma region Compare functions

int ComparePixel(float F1, float F2)
{
	//returns the squared difference of 2 floats
	return (int)pow(F1-F2,2);
}

int CompareBlock(BinaryMatrix& block, BinaryMatrix& Image, int StartCol, int EndCol, int StartRow, int EndRow)
{
	//create the variable that stors the SSD
	int SSD = 0;
	//loops through every pixel
	for(int i = StartRow;i<EndRow;i++)
	{
		for(int j = StartCol;j<EndCol;j++)
		{
			//accumulates the SSD
			SSD+= ComparePixel(block(i-StartRow,j-StartCol),Image(i,j));
			//cout<<block(i,j)<<endl;
		}
	}
	return SSD;
}

void CompareAgaingsAllBlocks(BinaryMatrix& block, BinaryMatrix& Image, int BlockRows,int BlockCol, int* Output)
{
	//create the return variables
	int Diff=0;
	//set BestDiff to the max so all other values will be smaller
	int BestX=0,BestY=0,BestDiff = INT_MAX;
	//loop through every block
	for(int i = 0;i<Image.GetRows();i+=BlockRows)
	{
		for(int j = 0;j<Image.GetColumns();j+=BlockCol)
		{
			//compare the block to all the blocks in the scrambled image
			Diff = CompareBlock(block,Image,i,i+BlockRows,j,j+BlockCol);
			//record the one with the lowest difference
			if(Diff<BestDiff)
			{
				BestDiff = Diff;
				BestX = i;
				BestY = j;
			}
			//diff of 0 means exact match, no need to continue looking
			if(Diff == 0)
			{
				break;
			}
		}
	}
	//store the results in the return variable
	*Output = BestDiff;
	*(Output +1) = BestX;
	*(Output+2) = BestY;
}

void CompareImages(BinaryMatrix& Scram, BinaryMatrix& Noise, int BlockRow, int BlockCol,int threshold, Matrix &Construct)
{
	//create an empty block
	BinaryMatrix Block(BlockRow,BlockCol,threshold);
	//create the matrix that will hold the final image

	//create the pointers for storing data and the outputs
	float* Data = new float[2];
	int* Output = new int[3];
	cout<<"Working: ";
	//begin looping through all the blocks
	for(int i = 0;i<Scram.GetRows();i+=BlockRow)
	{
		for(int j = 0;j<Scram.GetColumns();j+=BlockCol)
		{
			//Make sure data is empty
			delete []Data;
			//load the data from noise
			Data = Noise.GetBlock(i,i+BlockRow,j,j+BlockCol);
			//store the data
			Block.SetData(Data,BlockRow*BlockCol,true);
			//delete the data and output
			delete []Data;
			//collect the position of the block from scramble should have
			CompareAgaingsAllBlocks(Block,Scram,BlockRow,BlockCol,Output);
			//load the data from scrambled to data
			Data = Scram.GetBlock(*(Output+2),*(Output+2)+BlockRow,*(Output+1),*(Output+1)+BlockCol);
			//store the data
			Block.SetData(Data,BlockRow*BlockCol,true);
			//add the block from scrambled to the best possition in the image
			Construct.AddBlock(Block,i,j,i+BlockCol,j+BlockCol);
		}
		cout<<">";
	}
	cout<<endl;
	delete []Data;
	delete []Output;
}

int threshold()
{
	int input = 0;
	//the identifier prevents errors or the use for a loop to recieve correct input
tryAgain:
	cout<<"What do you want the threshold to be for converting the image to binary?"<<endl<<"(200 would reccomended)"<<endl;
	cin>>input;
	if(input >255 || input <0)
	{
		goto tryAgain;
	}
	return input;
}

void Unscramble(string Noise_dir, string Scram_dir,int BlockRow, int BlockCol, int threshold)
{
	//Ask if the user wants to clean the image with noise
	cout<<"Creating matrixes"<<endl;
	//readTXT returns a float* which needs to be deleted so it is loaded in to another pointer to be deleted when used
	float*Data;
	//load the data
	Data = readTXT(Scram_dir, 512,512);
	//create a matrix with the data
	BinaryMatrix Scrambled(512,512,threshold);
	Scrambled.SetData(Data,512*512,false);
	//delete the now copied data
	delete[]Data;
	//load the new data
	Data = readTXT(Noise_dir, 512,512);
	//create the second matrix
	BinaryMatrix Noise(512,512,threshold);
	Noise.SetData(Data,512*512,false);
	//delet the old data
	delete []Data;
	//clean if user requested

	//compare the images and return the unscrambled image
	BinaryMatrix Construct(Scrambled.GetRows(),Scrambled.GetColumns(),threshold);
	CompareImages(Scrambled,Noise,BlockRow,BlockRow,threshold, Construct);
	//write the image to file
	WritePGM("Output.pgm",Construct.GetData(),Scrambled.GetRows(),Scrambled.GetColumns(),1);
}

#pragma endregion

int main()
{
	//Everything is in this function
	Unscramble("logo_with_noise.txt","logo_shuffled.txt",32,32,threshold());
	//This checks if there has been a memory leak
	_CrtDumpMemoryLeaks();

	return 0;
}

