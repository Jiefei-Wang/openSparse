#pragma once
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "arrayfire.h"

template<class T>
class sparseMatrix {

	af::array* data=nullptr;
	af::array* rowInd = nullptr;
	af::array* colInd = nullptr;
	double offset;
public:
	int rowNum;
	int colNum;

public:
	sparseMatrix(T* dataVector, double * rowVector, double* colVector, double* size, double* offset);
	sparseMatrix(T * dataVector, int * rowVector, double * colVector, double* size, double* offset);
	~sparseMatrix();

	void unlock();
	void lock();
	cl_mem* getDevData();
	cl_mem* getDevRow();
	cl_mem* getDevCol();

	void getHostData(T* dataVector);
	void getHostRow(int* rowVector);
	void getHostCol(double* colVector);
	//get the lenght of the data vector,
	//i:0=data,1=row index,2=column index
	size_t getLength(int i);
};


template class sparseMatrix<double>;
template class sparseMatrix<float>;
