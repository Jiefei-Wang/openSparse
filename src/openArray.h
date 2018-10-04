#pragma once
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "CommonHeader.h"
#include <vector>
#include "Tools.h"
#include <type_traits>
enum dtype {autoDetect,f32,f64,i32,i64};


class openArray {
private:
	cl_mem data=nullptr;
	LARGEINDEX* dimension=new LARGEINDEX[2];
	dtype dataType;
	std::vector<void*> hostptr;
public:
	openArray(LARGEINDEX size1, dtype type);
	openArray(LARGEINDEX size1, LARGEINDEX size2, dtype type);
	template<class T>
	openArray(LARGEINDEX size1,T* src, dtype type);
	template<class T>
	openArray(LARGEINDEX size1, LARGEINDEX size2, T* src, dtype type);
	~openArray();
	//set and get data
	static openArray* constant(double number, LARGEINDEX size1, dtype type);
	static openArray* constant(double number, LARGEINDEX size1, LARGEINDEX size2, dtype type);
	cl_mem* getDeviceData();
	void getHostData(void *source);
	void* getHostData();

	//Dimension
	LARGEINDEX dims(int i);

private:
	template<class T>
	void* dataConvert(LARGEINDEX size, T data, dtype type);
	void gpuAlloc(LARGEINDEX size, dtype type);
	void gpuAlloc(LARGEINDEX size,void* hostData, dtype type);
	static int typesize(dtype type);
	static void* transferData(double data, dtype type);

};



template<class T>
inline openArray::openArray(LARGEINDEX size1, T * src, dtype type)
{
	if (type == dtype::autoDetect) {
		if (std::is_same<T, int>::value)	type = dtype::i32;
		if (std::is_same<T, long long>::value)type = dtype::i64;
		if (std::is_same<T, float>::value)type = dtype::f32;
		if (std::is_same<T, double>::value)type = dtype::f64;
		gpuAlloc(size1, src, type);
	}
	else {
		void* host_data = dataConvert(size1, src, type);
		gpuAlloc(size1, host_data, type);
	}
	dimension[0] = size1;
	dimension[1] = 1;
	dataType = type;
}

template<class T>
inline openArray::openArray(LARGEINDEX size1, LARGEINDEX size2, T * src, dtype type)
{
	if (type == dtype::autoDetect) {
		if (std::is_same<T, int>::value)	type = dtype::i32;
		if (std::is_same<T, long long>::value)type = dtype::i64;
		if (std::is_same<T, float>::value)type = dtype::f32;
		if (std::is_same<T, double>::value)type = dtype::f64;
		gpuAlloc(size1*size2, src, type);
	}
	else {
		void* host_data = dataConvert(size1*size2, src, type);
		gpuAlloc(size1*size2, host_data, type);
	}
	dimension[0] = size1;
	dimension[1] = size2;
	dataType = type;
}

template<class T>
void * openArray::dataConvert(LARGEINDEX size, T data, dtype type)
{
	float* tmp_data1;
	double* tmp_data2;
	int* tmp_data3;
	long long* tmp_data4;
	switch (type) {
	case f32:
		tmp_data1 = (float*)malloc(size*typesize(type));
		cpyData(tmp_data1, data, size);
		return tmp_data1;
	case f64:
		tmp_data2 = (double*)malloc(size*typesize(type));
		cpyData(tmp_data2, data, size);
		return tmp_data2;
	case i32:
		tmp_data3 = (int*)malloc(size*typesize(type));
		cpyData(tmp_data3, data, size);
		return tmp_data3;
	case i64:
		tmp_data4 = (long long*)malloc(size*typesize(type));
		cpyData(tmp_data4, data, size);
		return tmp_data4;
	};
}