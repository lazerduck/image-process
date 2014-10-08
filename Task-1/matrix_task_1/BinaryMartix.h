
class BinaryMatrix : public Matrix
{
private:
	//the value to convert to binary around
	int Threshold;
public:
	BinaryMatrix(int RowSize, int ColumnSize, float* data,int threshold);
	BinaryMatrix(int RowSize, int ColumnSize,int threshold);
	BinaryMatrix(BinaryMatrix& M);
	BinaryMatrix();
	~BinaryMatrix();
	BinaryMatrix operator+ (BinaryMatrix& B);
	BinaryMatrix operator- (BinaryMatrix& B);
	BinaryMatrix operator* (BinaryMatrix& B);
	void SetData(float* data,int size, bool binary);
	void setValue(int i, int j, float value, bool binary);
	float operator()(int i, int j);
};

BinaryMatrix::BinaryMatrix(int RowSize, int ColumnSize, float* data, int threshold)
{
	Threshold = threshold;
	Rows = RowSize;
	Columns = ColumnSize;
	Size = Rows*Columns;
	Data = new float[Size];
	for(int i = 0;i<Size;i++) {
		*(Data+i) = *(data + i);
	}
}
BinaryMatrix::BinaryMatrix(int RowSize, int ColumnSize,int threshold)
{
	Threshold = threshold;
	Rows = RowSize;
	Columns = ColumnSize;
	Size = Rows*Columns;
	Data = new float[Size];
	for(int i = 0;i<Size;i++)
	{
		*(Data + i) = 0;
	}
}
BinaryMatrix::BinaryMatrix(BinaryMatrix& M)
{
	//copy constructor
	Rows = M.Rows;
	Columns = M.Columns;
	Threshold = M.Threshold;
	Size = M.Size;
	Data = new float[Size];
	for(int i = 0;i<Size;i++)
	{
		*(Data + i) = *(M.Data + i);
	}
}
BinaryMatrix::~BinaryMatrix()
{
	delete []Data;
	Data = 0;
}
void BinaryMatrix::SetData(float* data, int size, bool binary)
{
	//if the data is binary
	if(binary)
	{
		//delete the old data then set the new data
		delete Data;
		Data = new float[Size]; 
		for(int i = 0; i<Size;i++){ 
			*(Data+i) = *(data+i);
		} 
	}
	else
	{
		//delete the old data then set the new data
		delete Data;
		Data = new float[Size]; 
		for(int i = 0; i<Size;i++){ 
			if(*(data+i)>Threshold)
			{
				*(Data+i) = 1;
			}
			else
			{
				*(Data+i) = 0;
			}
		} 
	}
}
void BinaryMatrix::setValue(int i, int j, float Value, bool binary)
{
	//is the data binary
	if(binary)
	{
		// is it above the threshold
		if(Value<Threshold)
		{
			Value = 0;
		}
		else
		{
			Value = 1;
		}
	}
	//is it within the bounds
	if(Rows<i || Columns<j){
		*(Data + (i*Columns + j)) = Value;
	}
}
float BinaryMatrix::operator()(int i, int j)
{
	return *(Data + i*Columns+j);
}
BinaryMatrix BinaryMatrix::operator+ (BinaryMatrix& B)
{
	float*data = new float[Size];
	float*BData = B.GetData();
	for(int i = 0; i<Size;i++)
	{
		if(*(Data+i) == 1 && *(BData+i) ==1)
		{
			*(data+i) = 1;
		}
		else
		{
			*(data+i) = 0;
		}
	}
	BinaryMatrix c(Rows,Columns,data,Threshold);
	delete []BData;
	return c;
}
BinaryMatrix BinaryMatrix::operator- (BinaryMatrix& B)
{
	float*data = new float[Size];
	float*BData = B.GetData();
	for(int i = 0; i<Size;i++)
	{
		if(*(Data+i) != *(BData+i))
		{
			*(data+i) = 1;
		}
		else
		{
			*(data+i) = 0;
		}
	}
	BinaryMatrix c(Rows,Columns,data,Threshold);
	delete []BData;
	return c;
}
BinaryMatrix BinaryMatrix::operator* (BinaryMatrix& B)
{
	float*data = new float[Size];
	float*BData = B.GetData();
	for(int i = 0; i<Size;i++)
	{
		if(*(Data+i) == 1 && *(BData+i) ==1)
		{
			*(data+i) = 1;
		}
		else
		{
			*(data+i) = 0;
		}
	}
	BinaryMatrix c(Rows,Columns,data,Threshold);
	delete []BData;
	return c;
}

