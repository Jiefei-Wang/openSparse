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
void rowSum(double* rowResult,void** address) {
	sparseMatrix<double>* data = *(sparseMatrix<double>**)address;
	size_t length = data->rowNum;
	openArray* result = openArray::constant(0, length, dtype::f32);

	cl_kernel kernel = kernelManager::createKernel("rowSum");
	static cl_command_queue af_queue = kernelManager::getQueue();
	// Set arguments and launch your kernels
	cl_int error = 0;
	error=clSetKernelArg(kernel, 0, sizeof(cl_mem), data->getDevData());
	error+=clSetKernelArg(kernel, 1, sizeof(cl_mem), data->getDevRow());
	error += clSetKernelArg(kernel, 2, sizeof(cl_mem), data->getDevCol());
	error += clSetKernelArg(kernel, 3, sizeof(cl_mem), result->getDeviceData());
	//	 data->getLength(1);
	size_t workNum = data->getLength(2) - 1;
	size_t localNum = 1;
	clEnqueueNDRangeKernel(af_queue, kernel, 1, NULL, &workNum, &localNum, 0, NULL, NULL);
	cpyData(rowResult, (float*)result->getHostData(), length);
	
	//af_print(result);
}
extern "C" LibExport
void colSum(double* colResult, void** address) {
	sparseMatrix<double>* data = *(sparseMatrix<double>**)address;
	size_t length = data->colNum;
	openArray* result= openArray::constant(0,length, dtype::f32);
	cl_kernel kernel = kernelManager::createKernel("colSum");
	static cl_command_queue af_queue = kernelManager::getQueue();
	// Set arguments and launch your kernels
	cl_int error = 0;
	error = clSetKernelArg(kernel, 0, sizeof(cl_mem), data->getDevData());
	error += clSetKernelArg(kernel, 1, sizeof(cl_mem), data->getDevRow());
	error += clSetKernelArg(kernel, 2, sizeof(cl_mem), data->getDevCol());
	error += clSetKernelArg(kernel, 3, sizeof(cl_mem), result->getDeviceData());
	//	 data->getLength(1);
	size_t workNum = data->getLength(2) - 1;
	size_t localNum = 1;
	error+=clEnqueueNDRangeKernel(af_queue, kernel, 1, NULL, &workNum, &localNum, 0, NULL, NULL);

	cpyData(colResult, (float*)result->getHostData(), length);
	delete result;
	//af_print(result);
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
	std::cout << kernelManager::programTable.size() << std::endl;
	std::cout << kernelManager::kernelTable.size() << std::endl;
}

void test1();
void test2();
int main(void) {
	kernelManager::setDevice(1);
	test2();
	kernelManager::setDevice(2);
	cl_device_id* id = new cl_device_id[1];
	size_t size = 0;
	cl_int error = clGetContextInfo(kernelManager::context, CL_CONTEXT_DEVICES, NULL, NULL, &size);
	 error=clGetContextInfo(kernelManager::context, CL_CONTEXT_DEVICES, size, id,NULL);
	if (*id != kernelManager::device_id) std::cout << "not match" << std::endl;
	char buffer[1024];
	cl_device_id device = *id;
	if (device == NULL) throw("The selected device is not found!");
	(clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL));
	printf("Platform name: %s\n", buffer);
	(clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(buffer), buffer, NULL));
	printf("Device name: %s\n", buffer);
	(clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, sizeof(buffer), buffer, NULL));
	printf("Opencl version: %s\n", buffer);
	cl_ulong global_mem_size;
	(clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(global_mem_size), &global_mem_size, NULL));
	printf("Device memory size: %lu MB\n", global_mem_size / 1048576);

	test2();
	debug();
	//kernelManager::getAllDeviceName();
	//kernelManager::getPlatformsInfo();
	//kernelManager::showDeviceInfo();
	
	//std::cout<< afcl::get
	//kernelManager::getDeviceInfo(1);
}

void test1() {
#include "read_test_data"
	openArray oa(size[0], data, dtype::autoDetect);
	print_partial_matrix("data:", (double*)oa.getHostData(), oa.dims(1), oa.dims(0));


}

void test2() {
	//kernelManager::setKernelDirectory("C:/Users/Jeff/OneDrive/course material/work/Roswell park/openSparse/src/kernel.cl");
	kernelManager::setKernelDirectory("C:/Users/wangj/OneDrive/course material/work/Roswell park/openSparse/src/kernel.cl");
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
	colSum(colres, address);
	colSum(colres, address);
	print_partial_matrix("colSum: ", colsum, 1, 10);
	print_partial_matrix("colSum: ", colres, 1, 10);


	double* rowres = new double[10];
	rowSum(rowres, address);
	rowSum(colres, address);
	print_partial_matrix("rowSum: ", rowsum, 1, 10);
	print_partial_matrix("rowSum: ", rowres, 1, 10);
	clear(address);
}