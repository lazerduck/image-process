class Matrix
{

public:
	Matrix(int RowSize, int ColumnSize, float* data);
	Matrix(int RowSize, int ColumnSize);
	Matrix(int RowSize, int ColumnSize,float val);
	Matrix(Matrix& M);
	Matrix();
	~Matrix();
	const int ReturnSize(){ return Size; }
	const int GetColumns(){ return Columns; }
	const int GetRows(){ return Rows; }
	virtual float* GetData(){ 
		//create a copy of Data and return it
		float*temp = new float[Size];
		for(int i = 0;i<Size;i++)
		{
			*(temp+i) = *(Data+i);
		}

		return temp;
	}
	void SetColumns( int ColumnSize ) {
		//set collumns and size if possible
		Columns = ColumnSize; 
		if(Rows >0){
			Size = Rows*Columns;
		} 
	}
	void SetRows(int RowSize) {
		//set the rows and size if possible
		Rows = RowSize;
		if(Columns >0){
			Size = Rows*Columns; 
		} 
	}
	virtual void SetData(float* InputData, int Size) {
		//delete the old data then set the new data
		delete Data;
		Data = new float[Size]; 
		for(int i = 0; i<Size;i++){ 
			*(Data+i) = *(InputData+i);
		} 
	} 
	const float ReturnValue(int i, int j) { 
		//check if within range then return the value
		float Output = 0;
		if(Rows<i || Columns<j){
			return -1;
		}
		Output =  *(Data + (i*Columns + j)); 
		return Output; 
	}
	virtual void SetValue(int i, int j, float Value) { 
		//check if in range then set the value
		if(Rows>i && Columns>j){
			*(Data + (i*Columns + j)) = Value;
		}
	}
	virtual void AddBlock(Matrix& Block,int StartRow, int StartCol, int EndRow, int EndCol);
	virtual float* GetBlock(int StartRow, int EndRow, int StartColumn, int EndColumn);
	virtual Matrix operator+ (Matrix& B);
	virtual Matrix operator- (Matrix& B);
	virtual Matrix operator/ (Matrix& B);
	virtual Matrix operator* (Matrix& B);
	virtual Matrix operator++ ();
	virtual Matrix operator++ (int);
	virtual float operator()(int i, int j);
	//protected data members
protected:
	int Rows;
	int Columns;
	int Size;
	float* Data;
};
Matrix::Matrix(int RowSize, int ColumnSize, float* data) {
	//constructor with data passed by pointer
	Rows = RowSize;
	Columns = ColumnSize;
	Size = Rows*Columns;
	//create a new float and do a deep copy
	Data = new float[Size];
	for(int i = 0;i<Size;i++) {
		*(Data + i) = *(data + i);
	}
}
Matrix::Matrix(int RowSize, int ColumnSize) {
	//constructor with no data
	Rows = RowSize;
	Columns = ColumnSize;
	Size = Rows*Columns;
}
Matrix::Matrix(int RowSize, int ColumnSize, float val) {
	//constructor to set all of the elements to val
	Rows = RowSize;
	Columns = ColumnSize;
	Size = Rows*Columns;
	Data = new float[Size];
	for(int i = 0; i<Size;i++)
	{
		*(Data+i) = val;
	}
}
Matrix::Matrix() {
	//constructor for an empty matrix class
	Rows = 0;
	Columns = 0;
	Size = 0;
	Data = 0;
}
Matrix::Matrix(Matrix&M)
{
	//copy constructor
	Rows = M.Rows;
	Columns = M.Columns;
	Size = M.Size;
	Data = new float[Size];
	for(int i = 0;i<Size;i++)
	{
		*(Data + i) = *(M.Data + i);
	}

}
Matrix::~Matrix() {
	//destructor
	delete []Data;
	Data = 0;
}
Matrix Matrix::operator+(Matrix &B){
	//add all of the elements in matrices together
	float*data = new float[Size];
	float*BData = B.GetData();
	for(int i = 0; i<Size;i++)
	{
		*(data+i) = (*(Data+i) + *(BData+i));
	}
	Matrix C (Rows,Columns,data);
	delete []BData;
	return C;
}
Matrix Matrix::operator-(Matrix &B){
	//subtract the elements
	float*data = new float[Size];
	float*BData = B.GetData();
	for(int i = 0; i<Size;i++)
	{
		*(data+i) = (*(Data+i) - *(BData+i));
	}
	Matrix C (Rows,Columns,data);
	delete []BData;
	return C;
}
Matrix Matrix::operator/(Matrix &B){
	//divide corresponding elements
	float*data = new float[Size];
	float*BData = B.GetData();
	for(int i = 0; i<Size;i++)
	{
		*(data+i) = (*(Data+i) / *(BData+i));
	}
	Matrix C (Rows,Columns,data);
	delete []BData;
	return C;
}
Matrix Matrix::operator*(Matrix &B){
	//multiply corresponding elements
	float*data = new float[Size];
	float*BData = B.GetData();
	for(int i = 0; i<Size;i++)
	{
		*(data+i) = (*(Data+i) * *(BData+i));
	}
	Matrix C (Rows,Columns,data);
	delete []BData;
	return C;
}
Matrix Matrix::operator++(int){
	//increment all of the values
	float*data = new float[Size];
	for(int i = 0; i<Size;i++)
	{
		*(data+i) = *(Data+i) +1;
		*(Data+i) +=1;
	}
	Matrix C (Rows,Columns,data);
	return C;
}
Matrix Matrix::operator++(){
	//increment all values
	float*data = new float[Size];
	for(int i = 0; i<Size;i++)
	{
		*(data+i) = *(Data+i) +1;
	}
	Matrix C (Rows,Columns,data);
	return C;
}
float Matrix::operator()(int i, int j)
{
	return *(Data + i*Columns + j);
}

void Matrix::AddBlock(Matrix& Block,int StartRow, int StartCol, int EndRow, int EndCol)
{
	//stores data between the 4 positions provided
	for(int i =StartRow;i<EndRow;i++)
	{
		for(int j= StartCol;j<EndCol;j++)
		{
			this->SetValue(i,j,Block(i-StartRow,j-StartCol));
		}
	}
}
float* Matrix::GetBlock(int StartRow, int EndRow, int StartColumn, int EndColumn)
{
	//returns the data between the 4 positions provided
	float* data = new float[(EndRow-StartRow)*(EndColumn-StartColumn)];
	for(int i = StartRow;i<EndRow;i++)
	{
		for(int j = StartColumn; j< EndColumn;j++)
		{
			*(data+ ((i-StartRow)*(EndRow-StartRow))+(j-StartColumn)) = *(Data + (i*Rows) + j);
		}
	}
	return data;
}
