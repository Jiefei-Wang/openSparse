#include "sparseMatrix.h"
#include "CommonHeader.h"
#include"Tools.h"
//#include "Tools.h"
using namespace af;


//not quite useful now
template<class T>
sparseMatrix<T>::sparseMatrix(T* dataVector, double * rowVector, double* colVector, double* size, double* offset) {
	
	cpyData((int *)rowVector, rowVector, (dim_t)size[1]);
	data = new array((dim_t)size[0], dataVector);
	rowInd = new array((dim_t)size[1], (int*)rowVector);
	colInd= new array((dim_t)size[2], colVector);
	rowNum = size[3];
	colNum = size[4];
	this->offset = *offset;

}

template<class T>
sparseMatrix<T>::sparseMatrix(T * dataVector,int * rowVector,double * colVector, double* size, double* offset)
{
	data = new array((dim_t)size[0], dataVector);
	rowInd = new array((dim_t)size[1],rowVector);
	colInd = new array((dim_t)size[2], colVector);
	rowNum = size[3];
	colNum = size[4];
	this->offset = *offset;
}

template<class T>
sparseMatrix<T>::~sparseMatrix()
{
	if (data != nullptr) {
		/*
		af::free(data->device<cl_mem>());
		af::free(rowInd->device<cl_mem>());
		af::free(colInd->device<cl_mem>());
		*/
		delete data;
		delete rowInd;
		delete colInd;
		deviceGC();
	}
}

template<class T>
void sparseMatrix<T>::unlock()
{
	if (data != nullptr) {
		data->unlock();
		rowInd->unlock();
		colInd->unlock();
	}
}

template<class T>
void sparseMatrix<T>::lock()
{
	if (data != nullptr) {
		data->lock();
		rowInd->lock();
		colInd->lock();
	}
}

template<class T>
cl_mem* sparseMatrix<T>::getDevData()
{
	return data->device<cl_mem>();
}

template<class T>
cl_mem * sparseMatrix<T>::getDevRow()
{
	return rowInd->device<cl_mem>();
}

template<class T>
cl_mem * sparseMatrix<T>::getDevCol()
{
	return colInd->device<cl_mem>();
}

template<class T>
void sparseMatrix<T>::getHostData(T * dataVector)
{
	if(dataVector==nullptr)
		dataVector = new T[data->dims(0)];
	data->host((void*)dataVector);
}

template<class T>
void sparseMatrix<T>::getHostRow(int * rowVector)
{
	if (rowVector == nullptr)
		rowVector = new int[rowInd->dims(0)];
	rowInd->host((void*)rowVector);
}

template<class T>
void sparseMatrix<T>::getHostCol(double * colVector)
{
	if (colVector == nullptr)
		colVector = new double[colInd->dims(0)];
	colInd->host((void*)colVector);
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
