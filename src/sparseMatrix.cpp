#include "sparseMatrix.h"
#include "CommonHeader.h"
#include"Tools.h"
//#include "Tools.h"

//not quite useful now
template<class T>
sparseMatrix<T>::sparseMatrix(T* dataVector, double * rowVector, double* colVector, double* size, double* offset) {
	
	data = new openArray(size[0], dataVector,dtype::autoDetect);
	rowInd = new openArray(size[1], rowVector, dtype::i32);
	colInd= new openArray(size[2], colVector, dtype::autoDetect);
	rowNum = size[3];
	colNum = size[4];
	this->offset = *offset;

}

template<class T>
sparseMatrix<T>::sparseMatrix(T * dataVector,int * rowVector,double * colVector, double* size, double* offset)
{
	data = new openArray(size[0], dataVector, dtype::autoDetect);
	rowInd = new openArray(size[1],rowVector, dtype::autoDetect);
	colInd = new openArray(size[2], colVector, dtype::autoDetect);
	rowNum = size[3];
	colNum = size[4];
	this->offset = *offset;
}

template<class T>
sparseMatrix<T>::~sparseMatrix()
{
	if (data != nullptr) {
		delete data;
		delete rowInd;
		delete colInd;
	}
}


template<class T>
cl_mem* sparseMatrix<T>::getDevData()
{
	return data->getDeviceData();
}

template<class T>
cl_mem * sparseMatrix<T>::getDevRow()
{
	return rowInd->getDeviceData();
}

template<class T>
cl_mem * sparseMatrix<T>::getDevCol()
{
	return colInd->getDeviceData();
}

template<class T>
void sparseMatrix<T>::getHostData(T * dataVector)
{
	if(dataVector==nullptr)
		dataVector = new T[data->dims(0)];
	data->getHostData((void*)dataVector);
}

template<class T>
void sparseMatrix<T>::getHostRow(int * rowVector)
{
	if (rowVector == nullptr)
		rowVector = new int[rowInd->dims(0)];
	rowInd->getHostData((void*)rowVector);
}

template<class T>
void sparseMatrix<T>::getHostCol(double * colVector)
{
	if (colVector == nullptr)
		colVector = new double[colInd->dims(0)];
	colInd->getHostData((void*)colVector);
}

template<class T>
size_t sparseMatrix<T>::getLength(int i)
{
	if (data == nullptr) {
		return 0;
	}
	switch (i) {
	case 0:
			return data->dims(0);
	case 1:
		return rowInd->dims(0);
	case 2:
		return colInd->dims(0);
	}
	return 0;
}
