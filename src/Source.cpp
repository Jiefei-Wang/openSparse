#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "R_ext/libextern.h"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include"Tools.h"
#include "kernelManager.h"

#include "sparseMatrix.h"
#include "openArray.h"

//using namespace std;



extern "C" LibExport
void upload(double* dataFrame, int * rowInd,double* colInd, double* size,double* offset, void** address) {
	sparseMatrix<double>* data=new sparseMatrix<double>(dataFrame, rowInd, colInd, size, offset);
	*address = (void*)data;
}
extern "C" LibExport
void download(double* dataFrame, int * rowInd, double* colInd, void** address) {
	sparseMatrix<double>* data = *(sparseMatrix<double>**)address;
	data->getHostData(dataFrame);
	data->getHostRow(rowInd);
	data->getHostCol(colInd);
}

extern "C" LibExport
void clear(void** address) {
	delete *(sparseMatrix<double>**)address;
}


extern "C" LibExport
void rowSum(double* rowResult,void** address,char** extCode) {
	sparseMatrix<double>* data = *(sparseMatrix<double>**)address;
	int nrow = data->rowNum;
	int ncol = data->colNum;
	size_t length = data->rowNum;
	openArray* result = openArray::constant(0, length, dtype::f32);

	cl_kernel kernel = kernelManager::createKernel("rowSum", *extCode);
	cl_command_queue af_queue = kernelManager::getQueue();
	// Set arguments and launch your kernels
	cl_int error = 0;
	error=clSetKernelArg(kernel, 0, sizeof(cl_mem), data->getDevData());
	error+=clSetKernelArg(kernel, 1, sizeof(cl_mem), data->getDevRow());
	error += clSetKernelArg(kernel, 2, sizeof(cl_mem), data->getDevCol());
	error += clSetKernelArg(kernel, 3, sizeof(int), &nrow);
	error += clSetKernelArg(kernel, 4, sizeof(int), &ncol);
	error += clSetKernelArg(kernel, 5, sizeof(cl_mem), result->getDeviceData());
	//	 data->getLength(1);
	size_t workNum = data->getLength(2) - 1;
	size_t localNum = 1;
	error += clEnqueueNDRangeKernel(af_queue, kernel, 1, NULL, &workNum, &localNum, 0, NULL, NULL);

	if (error != 0) throw("An error has occured in the kernel excution");
	cpyData(rowResult, (float*)result->getHostData(), length);
	
	//af_print(result);
}
extern "C" LibExport
void colSum(double* colResult, void** address, char** extCode) {
	sparseMatrix<double>* data = *(sparseMatrix<double>**)address;
	int nrow = data->rowNum;
	int ncol = data->colNum;
	size_t length = data->colNum;
	openArray* result= openArray::constant(0,length, dtype::f32);

	cl_kernel kernel = kernelManager::createKernel("colSum", *extCode);
	cl_command_queue af_queue = kernelManager::getQueue();
	// Set arguments and launch your kernels
	cl_int error = 0;
	error = clSetKernelArg(kernel, 0, sizeof(cl_mem), data->getDevData());
	error += clSetKernelArg(kernel, 1, sizeof(cl_mem), data->getDevRow());
	error += clSetKernelArg(kernel, 2, sizeof(cl_mem), data->getDevCol());
	error += clSetKernelArg(kernel, 3, sizeof(int), &nrow);
	error += clSetKernelArg(kernel, 4, sizeof(int), &ncol);
	error += clSetKernelArg(kernel, 5, sizeof(cl_mem), result->getDeviceData());
	//	 data->getLength(1);
	size_t workNum = data->getLength(2) - 1;
	size_t localNum = 1;
	error+= clEnqueueNDRangeKernel(af_queue, kernel, 1, NULL, &workNum, &localNum, 0, NULL, NULL);

	if (error != 0) throw("An error has occured in the kernel excution");
	cpyData(colResult, (float*)result->getHostData(), length);
	delete result;
}

extern "C" LibExport
void getDeviceList() {
	kernelManager::getAllDeviceName();
}
extern "C" LibExport
void getDeviceInfo(int * i) {
	kernelManager::getDeviceInfo( *i);
}
extern "C" LibExport
void getDeviceDetail(int * i) {
	kernelManager::getDeviceFullInfo(*i);
}
extern "C" LibExport
void setDevice(int * i) {
	kernelManager::setDevice(*i);
}

extern "C" LibExport
void getCurDevice() {
	kernelManager::getCurDevice();
}

extern "C" LibExport
void debug() {
	throw("test error");
}

void test1();
void test2(char* );
void test3();
void test4();
int main(void) {
	//kernelManager::setKernelDirectory("C:/Users/Jeff/OneDrive/course material/work/Roswell park/openSparse/src/kernel.cl");
	kernelManager::setKernelDirectory("C:/Users/wangj/OneDrive/course material/work/Roswell park/openSparse/src/kernel.cl");

	//kernelManager::setDevice(0);
	test2("m=m+2;");
	test2("m=m+2;m=m*4;");
	//kernelManager::setDevice(10);
	
	//test2();
	/*
	kernelManager::setDevice(0);
	test3();
	kernelManager::setDevice(0);
	test3();

	kernelManager::setDevice(0);
	test3();
	kernelManager::setDevice(0);
	test3();*/
	//kernelManager::setDevice(0);
	//test2();
	//kernelManager::getAllDeviceName();
	//kernelManager::getPlatformsInfo();
	
	//std::cout<< afcl::get
	//kernelManager::getDeviceInfo(1);
	//kernelManager::getDeviceFullInfo(1);
}

void test1() {
#include "read_test_data"
	openArray oa(size[0], data, dtype::autoDetect);
	print_partial_matrix("data:", (double*)oa.getHostData(), oa.dims(1), oa.dims(0));


}

void test2(char* extCode) {
	char** extCodePtr = &extCode;

#include "read_test_data"
	void** address = new void*;
	double offset = 0;
	upload(data, rowInd, colInd, size, &offset, address);
	double* download_data = new double[(int)size[0]];
	int* download_rowInd = new int[(int)size[0]];
	double* download_colInd = new double[(int)size[0]];
	download(download_data, download_rowInd, download_colInd, address);
	print_partial_matrix("Data: ", download_data, 1, size[0]);
	print_partial_matrix("row: ", download_rowInd, 1, size[1]);
	print_partial_matrix("col: ", download_colInd, 1, size[2]);

	double* colres = new double[10];
	colSum(colres, address, extCodePtr);
	print_partial_matrix("colSum: ", colsum, 1, 10);
	print_partial_matrix("colSum: ", colres, 1, 10);


	double* rowres = new double[10];
	rowSum(rowres, address, extCodePtr);
	print_partial_matrix("rowSum: ", rowsum, 1, 10);
	print_partial_matrix("rowSum: ", rowres, 1, 10);
	clear(address);
}


void test3() {
	kernelManager::setKernelDirectory("vector_add_kernel.cl");
	cl_int error;
	//Data preparation
	const int LIST_SIZE = 1024;
	openArray* A = openArray::constant(1, LIST_SIZE, dtype::i32);
	openArray* B = openArray::constant(2, LIST_SIZE, dtype::i32);
	openArray* C = openArray::constant(0, LIST_SIZE, dtype::i32);

	//prepare the kernel function
	cl_kernel kernel= kernelManager::createKernel("vector_add");
	cl_command_queue queue = kernelManager::getQueue();
	//set argument
	error = clSetKernelArg(kernel, 0, sizeof(cl_mem), A->getDeviceData());
	error += clSetKernelArg(kernel, 1, sizeof(cl_mem), B->getDeviceData());
	error += clSetKernelArg(kernel, 2, sizeof(cl_mem), C->getDeviceData());

	//launch kernel
	size_t global_item_size = LIST_SIZE; // Process the entire lists
	size_t local_item_size = 64;
	if (error != CL_SUCCESS)throw(error);
	error =clEnqueueNDRangeKernel(queue, kernel, 1, NULL,
		&global_item_size, &local_item_size, 0, NULL, NULL);
}


void test4() {
	kernelManager::setKernelDirectory("vector_add_kernel.cl");
	cl_int error;
	//Data preparation
	const int LIST_SIZE = 1024;
	openArray* A = openArray::constant(1, LIST_SIZE, dtype::i32);
	openArray* B = openArray::constant(2, LIST_SIZE, dtype::i32);
	openArray* C = openArray::constant(0, LIST_SIZE, dtype::i32);
	//An empty kernel function
	cl_kernel kernel = kernelManager::createKernel("vector_add");
	cl_command_queue queue = kernelManager::getQueue();

	error = clSetKernelArg(kernel, 0, sizeof(cl_mem), A->getDeviceData());
	error += clSetKernelArg(kernel, 1, sizeof(cl_mem), B->getDeviceData());
	error += clSetKernelArg(kernel, 2, sizeof(cl_mem), C->getDeviceData());
	//launch kernel
	size_t global_item_size = LIST_SIZE; // Process the entire lists
	size_t local_item_size = 64;
	if (error != CL_SUCCESS)throw(error);
	error = clEnqueueNDRangeKernel(queue, kernel, 1, NULL,
		&global_item_size, &local_item_size, 0, NULL, NULL);
}