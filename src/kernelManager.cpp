#include "kernelManager.h"

#include "arrayfire.h"
#include <af/opencl.h>
using namespace std;
int kernelManager::deviceIndex=0;
cl_context kernelManager::context=nullptr;
cl_device_id kernelManager::device_id = nullptr;
cl_command_queue kernelManager::command_queue;
std::map<std::string, cl_program> kernelManager::programTable;
std::map<std::string, cl_kernel> kernelManager::kernelTable;
char* kernelManager::kernelFile = "src/kernel.cl";
bool kernelManager::ready=false;





void kernelManager::getPlatformsInfo()
{
	int backends = af::getAvailableBackends();
	bool cpu = backends & AF_BACKEND_CPU;
	bool cuda = backends & AF_BACKEND_CUDA;
	bool opencl = backends & AF_BACKEND_OPENCL;
	cout << "Backend info:" << endl;
	cout << "CPU: " << cpu << endl;
	cout << "CUDA: " << cuda << endl;
	cout << "OPENCL: " << opencl << endl;
}

void kernelManager::getAllDeviceName() {
	cl_uint platform_num;
	clGetPlatformIDs(0, NULL, &platform_num);
	cl_platform_id* platform_id = new cl_platform_id[platform_num];
	clGetPlatformIDs(platform_num, platform_id, NULL);
	cl_uint device_num;
	cl_device_id* device_id;
	int device_count = 0;
	for (int i = 0; i < platform_num; i++) {
		clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_ALL, NULL, NULL, &device_num);
		device_id = new cl_device_id[device_num];
		clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_ALL, device_num, device_id, NULL);
		size_t name_size;
		char* device_name;
		for (int j = 0; j < device_num; j++) {
			clGetDeviceInfo(device_id[j], CL_DEVICE_NAME, NULL, NULL, &name_size);
			device_name = new char[name_size];
			clGetDeviceInfo(device_id[j], CL_DEVICE_NAME, name_size, device_name, NULL);
			std::cout << "Device " << device_count << ": " << device_name << std::endl;
			device_count++;
			delete[] device_name;
		}
		delete[] device_id;
	}
	delete[] platform_id;
}

void kernelManager::showDeviceInfo()
{
	char * 	d_name = new char[40 ];
	char * 	d_platform = new char[40];
	char * 	d_toolkit = new char[40];
	char * 	d_compute = new char[40];
	af::deviceInfo(d_name,
		d_platform,
		d_toolkit,
		d_compute
	);
	cout << "Device name: " << d_name << endl;
	cout << "Platform name: " << d_platform << endl;
	cout << "Toolkit: " << d_toolkit << endl;
	cout << "Compite capacity: " << d_compute << endl;
}

void kernelManager::getDeviceFullInfo(int device_index)
{
	char buffer[1024];
	cl_device_id device = getDeviceID(device_index);
	cl_device_type type;
	clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, NULL);
	if (type & CL_DEVICE_TYPE_DEFAULT) printf("CL_DEVICE_TYPE: %s\n", "CL_DEVICE_TYPE_DEFAULT");
	if (type & CL_DEVICE_TYPE_CPU) printf("CL_DEVICE_TYPE: %s\n", "CL_DEVICE_TYPE_CPU");
	if (type & CL_DEVICE_TYPE_GPU) printf("CL_DEVICE_TYPE: %s\n", "CL_DEVICE_TYPE_GPU");
	if (type & CL_DEVICE_TYPE_ACCELERATOR) printf("CL_DEVICE_TYPE: %s\n", "CL_DEVICE_TYPE_ACCELERATOR");
	if (type & CL_DEVICE_TYPE_CUSTOM) printf("CL_DEVICE_TYPE: %s\n", "CL_DEVICE_TYPE_CUSTOM");
	 (clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(buffer), buffer, NULL));
	printf("CL_DEVICE_NAME: %s\n", buffer);
	 (clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL));
	printf("CL_DEVICE_VENDOR: %s\n", buffer);
	cl_uint vendor_id;
	 (clGetDeviceInfo(device, CL_DEVICE_VENDOR_ID, sizeof(vendor_id), &vendor_id, NULL));
	printf("CL_DEVICE_VENDOR_ID: %d\n", vendor_id);
	 (clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL));
	printf("CL_DEVICE_VERSION: %s\n", buffer);
	 (clGetDeviceInfo(device, CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL));
	printf("CL_DRIVER_VERSION: %s\n", buffer);
	 (clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, sizeof(buffer), buffer, NULL));
	printf("CL_DEVICE_OPENCL_C_VERSION: %s\n", buffer);
	 (clGetDeviceInfo(device, CL_DEVICE_PROFILE, sizeof(buffer), buffer, NULL));
	printf("CL_DEVICE_PROFILE: %s\n", buffer);
	 (clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, sizeof(buffer), buffer, NULL));
	printf("CL_DEVICE_EXTENSIONS: %s\n", buffer);
	printf("CL_DEVICE_BUILT_IN_KERNELS: %s\n", clGetDeviceInfo(device, CL_DEVICE_BUILT_IN_KERNELS, sizeof(buffer), buffer, NULL) == CL_SUCCESS ? buffer : "UNSUPPORTED");
	cl_uint max_compute_units;
	 (clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units), &max_compute_units, NULL));
	printf("CL_DEVICE_MAX_COMPUTE_UNITS: %u\n", max_compute_units);
	cl_uint max_work_item_dimensions;
	 (clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(max_work_item_dimensions), &max_work_item_dimensions, NULL));
	printf("CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %u\n", max_work_item_dimensions);
	size_t* max_work_item_sizes = (size_t*)malloc(sizeof(size_t) * max_work_item_dimensions);
	 (clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * max_work_item_dimensions, max_work_item_sizes, NULL));
	printf("CL_DEVICE_MAX_WORK_ITEM_SIZES: "); for (size_t i = 0; i < max_work_item_dimensions; ++i) printf("%lu\t", max_work_item_sizes[i]); printf("\n");
	free(max_work_item_sizes);
	size_t max_work_group_size;
	 (clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, NULL));
	printf("CL_DEVICE_MAX_WORK_GROUP_SIZE: %lu\n", max_work_group_size);
	cl_uint preferred_vector_width_char;
	 (clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(preferred_vector_width_char), &preferred_vector_width_char, NULL));
	printf("CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR: %u\n", preferred_vector_width_char);
	cl_uint preferred_vector_width_short;
	 (clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, sizeof(preferred_vector_width_short), &preferred_vector_width_short, NULL));
	printf("CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT: %u\n", preferred_vector_width_short);
	cl_uint preferred_vector_width_int;
	 (clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(preferred_vector_width_int), &preferred_vector_width_int, NULL));
	printf("CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT: %u\n", preferred_vector_width_int);
	cl_uint preferred_vector_width_long;
	 (clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(preferred_vector_width_long), &preferred_vector_width_long, NULL));
	printf("CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG: %u\n", preferred_vector_width_long);
	cl_uint preferred_vector_width_float;
	 (clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(preferred_vector_width_float), &preferred_vector_width_float, NULL));
	printf("CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT: %u\n", preferred_vector_width_float);
	cl_uint preferred_vector_width_double;
	 (clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(preferred_vector_width_double), &preferred_vector_width_double, NULL));
	printf("CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE: %u\n", preferred_vector_width_double);
	cl_uint preferred_vector_width_half;
	 (clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, sizeof(preferred_vector_width_half), &preferred_vector_width_half, NULL));
	printf("CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF: %u\n", preferred_vector_width_half);
	cl_uint native_vector_width_char;
	 (clGetDeviceInfo(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, sizeof(native_vector_width_char), &native_vector_width_char, NULL));
	printf("CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR: %u\n", native_vector_width_char);
	cl_uint native_vector_width_short;
	 (clGetDeviceInfo(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, sizeof(native_vector_width_short), &native_vector_width_short, NULL));
	printf("CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT: %u\n", native_vector_width_short);
	cl_uint native_vector_width_int;
	 (clGetDeviceInfo(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, sizeof(native_vector_width_int), &native_vector_width_int, NULL));
	printf("CL_DEVICE_NATIVE_VECTOR_WIDTH_INT: %u\n", native_vector_width_int);
	cl_uint native_vector_width_long;
	 (clGetDeviceInfo(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, sizeof(native_vector_width_long), &native_vector_width_long, NULL));
	printf("CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG: %u\n", native_vector_width_long);
	cl_uint native_vector_width_float;
	 (clGetDeviceInfo(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, sizeof(native_vector_width_float), &native_vector_width_float, NULL));
	printf("CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT: %u\n", native_vector_width_float);
	cl_uint native_vector_width_double;
	 (clGetDeviceInfo(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, sizeof(native_vector_width_double), &native_vector_width_double, NULL));
	printf("CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE: %u\n", native_vector_width_double);
	cl_uint native_vector_width_half;
	 (clGetDeviceInfo(device, CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF, sizeof(native_vector_width_half), &native_vector_width_half, NULL));
	printf("CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF: %u\n", native_vector_width_half);
	cl_uint max_clock_frequency;
	 (clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(max_clock_frequency), &max_clock_frequency, NULL));
	printf("CL_DEVICE_MAX_CLOCK_FREQUENCY: %u MHz\n", max_clock_frequency);
	cl_uint address_bits;
	 (clGetDeviceInfo(device, CL_DEVICE_ADDRESS_BITS, sizeof(address_bits), &address_bits, NULL));
	printf("CL_DEVICE_ADDRESS_BITS: %u\n", address_bits);
	cl_ulong max_mem_alloc_size;
	 (clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, NULL));
	printf("CL_DEVICE_MAX_MEM_ALLOC_SIZE: %lu B = %lu MB\n", max_mem_alloc_size, max_mem_alloc_size / 1048576);
	cl_bool image_support;
	 (clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(image_support), &image_support, NULL));
	printf("CL_DEVICE_IMAGE_SUPPORT: %u\n", image_support);
	size_t max_parameter_size;
	 (clGetDeviceInfo(device, CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(max_parameter_size), &max_parameter_size, NULL));
	printf("CL_DEVICE_MAX_PARAMETER_SIZE: %lu B\n", max_parameter_size);
	cl_device_mem_cache_type global_mem_cache_type;
	 (clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, sizeof(global_mem_cache_type), &global_mem_cache_type, NULL));
	if (global_mem_cache_type == CL_NONE) printf("CL_DEVICE_GLOBAL_MEM_CACHE_TYPE: %s\n", "CL_NONE");
	if (global_mem_cache_type == CL_READ_ONLY_CACHE) printf("CL_DEVICE_GLOBAL_MEM_CACHE_TYPE: %s\n", "CL_READ_ONLY_CACHE");
	if (global_mem_cache_type == CL_READ_WRITE_CACHE) printf("CL_DEVICE_GLOBAL_MEM_CACHE_TYPE: %s\n", "CL_READ_WRITE_CACHE");
	cl_uint global_mem_cacheline_size;
	 (clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(global_mem_cacheline_size), &global_mem_cacheline_size, NULL));
	printf("CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE: %u B\n", global_mem_cacheline_size);
	cl_ulong global_mem_cache_size;
	 (clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(global_mem_cache_size), &global_mem_cache_size, NULL));
	printf("CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: %lu B = %lu KB\n", global_mem_cache_size, global_mem_cache_size / 1024);
	cl_ulong global_mem_size;
	 (clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(global_mem_size), &global_mem_size, NULL));
	printf("CL_DEVICE_GLOBAL_MEM_SIZE: %lu B = %lu MB\n", global_mem_size, global_mem_size / 1048576);
	cl_ulong max_constant_buffer_size;
	 (clGetDeviceInfo(device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(max_constant_buffer_size), &max_constant_buffer_size, NULL));
	printf("CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE: %lu B = %lu KB\n", max_constant_buffer_size, max_constant_buffer_size / 1024);
	cl_uint max_constant_args;
	 (clGetDeviceInfo(device, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(max_constant_args), &max_constant_args, NULL));
	printf("CL_DEVICE_MAX_CONSTANT_ARGS: %u\n", max_constant_args);
	cl_device_local_mem_type local_mem_type;
	 (clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(local_mem_type), &local_mem_type, NULL));
	if (local_mem_type == CL_NONE) printf("CL_DEVICE_LOCAL_MEM_TYPE: %s\n", "CL_NONE");
	if (local_mem_type == CL_LOCAL) printf("CL_DEVICE_LOCAL_MEM_TYPE: %s\n", "CL_LOCAL");
	if (local_mem_type == CL_GLOBAL) printf("CL_DEVICE_LOCAL_MEM_TYPE: %s\n", "CL_GLOBAL");
	cl_ulong local_mem_size;
	 (clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(local_mem_size), &local_mem_size, NULL));
	printf("CL_DEVICE_LOCAL_MEM_SIZE: %lu B = %lu KB\n", local_mem_size, local_mem_size / 1024);
	cl_bool error_correction_support;
	 (clGetDeviceInfo(device, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(error_correction_support), &error_correction_support, NULL));
	printf("CL_DEVICE_ERROR_CORRECTION_SUPPORT: %u\n", error_correction_support);
	cl_bool host_unified_memory;
	 (clGetDeviceInfo(device, CL_DEVICE_HOST_UNIFIED_MEMORY, sizeof(host_unified_memory), &host_unified_memory, NULL));
	printf("CL_DEVICE_HOST_UNIFIED_MEMORY: %u\n", host_unified_memory);
	size_t profiling_timer_resolution;
	 (clGetDeviceInfo(device, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(profiling_timer_resolution), &profiling_timer_resolution, NULL));
	printf("CL_DEVICE_PROFILING_TIMER_RESOLUTION: %lu ns\n", profiling_timer_resolution);
	cl_bool endian_little;
	 (clGetDeviceInfo(device, CL_DEVICE_ENDIAN_LITTLE, sizeof(endian_little), &endian_little, NULL));
	printf("CL_DEVICE_ENDIAN_LITTLE: %u\n", endian_little);
	cl_bool available;
	 (clGetDeviceInfo(device, CL_DEVICE_AVAILABLE, sizeof(available), &available, NULL));
	printf("CL_DEVICE_AVAILABLE: %u\n", available);
	cl_bool compier_available;
	 (clGetDeviceInfo(device, CL_DEVICE_COMPILER_AVAILABLE, sizeof(compier_available), &compier_available, NULL));
	printf("CL_DEVICE_COMPILER_AVAILABLE: %u\n", compier_available);
	cl_bool linker_available;
	 (clGetDeviceInfo(device, CL_DEVICE_LINKER_AVAILABLE, sizeof(linker_available), &linker_available, NULL));
	printf("CL_DEVICE_LINKER_AVAILABLE: %u\n", linker_available);
	cl_device_exec_capabilities exec_capabilities;
	 (clGetDeviceInfo(device, CL_DEVICE_EXECUTION_CAPABILITIES, sizeof(exec_capabilities), &exec_capabilities, NULL));
	if (exec_capabilities & CL_EXEC_KERNEL) printf("CL_DEVICE_EXECUTION_CAPABILITIES: %s\n", "CL_EXEC_KERNEL");
	if (exec_capabilities & CL_EXEC_NATIVE_KERNEL) printf("CL_DEVICE_EXECUTION_CAPABILITIES: %s\n", "CL_EXEC_NATIVE_KERNEL");
	cl_command_queue_properties queue_properties;
	 (clGetDeviceInfo(device, CL_DEVICE_QUEUE_PROPERTIES, sizeof(queue_properties), &queue_properties, NULL));
	if (queue_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE) printf("CL_DEVICE_QUEUE_PROPERTIES: %s\n", "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE");
	if (queue_properties & CL_QUEUE_PROFILING_ENABLE) printf("CL_DEVICE_QUEUE_PROPERTIES: %s\n", "CL_QUEUE_PROFILING_ENABLE");
	size_t printf_buffer_size;
	 (clGetDeviceInfo(device, CL_DEVICE_PRINTF_BUFFER_SIZE, sizeof(printf_buffer_size), &printf_buffer_size, NULL));
	printf("CL_DEVICE_PRINTF_BUFFER_SIZE: %lu B = %lu KB\n", printf_buffer_size, printf_buffer_size / 1024);
	cl_bool preferred_interop_user_sync;
	 (clGetDeviceInfo(device, CL_DEVICE_PREFERRED_INTEROP_USER_SYNC, sizeof(preferred_interop_user_sync), &preferred_interop_user_sync, NULL));
	printf("CL_DEVICE_PREFERRED_INTEROP_USER_SYNC: %u\n", preferred_interop_user_sync);
	//			cl_device_id parent_device;
	//			 (clGetDeviceInfo(device, CL_DEVICE_PARENT_DEVICE, sizeof(parent_device), &parent_device, NULL));
	//			printf("CL_DEVICE_PARENT_DEVICE: %u\n", parent_device);
	cl_uint reference_count;
	 (clGetDeviceInfo(device, CL_DEVICE_REFERENCE_COUNT, sizeof(reference_count), &reference_count, NULL));
	printf("CL_DEVICE_REFERENCE_COUNT: %u\n", reference_count);
	printf("\n");
}

void kernelManager::setKernelDirectory(char *dir)
{
	kernelFile = dir;
}

void kernelManager::setDevice(int device)
{
	cl_device_id device_id = getDeviceID(device);
	afcl::setDevice(device_id,afcl::getContext());
	initializeManager();

}


cl_kernel kernelManager::createKernel(const char * filename, const char * kernel)
{
	if (programTable.find(string(filename)) == programTable.end())
		loadProgram(filename);
	if (kernelTable.find(string(kernel)) != kernelTable.end())
		return kernelTable[string(kernel)];
	cl_int error;
	cl_kernel dev_kernel = clCreateKernel(programTable[string(filename)], kernel, &error);
	
	switch (error) {
	case 0:
		break;
	
	default:
		cout << "Fail to create kernel, error code: " << error << endl;
	}
		
	kernelTable.insert(make_pair(string(kernel), dev_kernel));
	return dev_kernel;
}

cl_kernel kernelManager::createKernel(const char * kernel)
{
	return createKernel(kernelFile, kernel);
}



void kernelManager::loadProgram(const char* filename)
{
	if (!ready)
		initializeManager();
	if (programTable.find(string(filename)) != programTable.end())
		return;
	
	ifstream in(filename, std::ios_base::binary);
	if (!in.good()) {
		return;
	}

	// get file length
	in.seekg(0, std::ios_base::end);
	size_t length = in.tellg();
	in.seekg(0, std::ios_base::beg);

	// read program source
	std::vector<char> data(length + 1);
	in.read(&data[0], length);
	data[length] = 0;


	// create and build program 
	cl_int error = 0;
	const char* source = &data[0];
	cl_program program = clCreateProgramWithSource(context, 1, &source, 0, &error);
	if (error != CL_SUCCESS) {
		cout << "Fail to read program, error code: " << error << endl;
		return;
	}
	error = clBuildProgram(program, 1, &device_id, 0, 0, 0);
	switch (error) {
	case CL_BUILD_PROGRAM_FAILURE:
		cout << "Fail to build program, build info: " << endl;
		size_t log_size;
		clGetProgramBuildInfo(program, afcl::getDeviceId(), CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char *log = (char *)malloc(log_size);
		// Get the log
		clGetProgramBuildInfo(program, afcl::getDeviceId(), CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
		// Print the log
		printf("%s\n", log);
		delete[] log;
		break;
	}
	programTable.insert(make_pair(string(filename), program));
}



void kernelManager::initializeManager()
{
	context = afcl::getContext();
	device_id = afcl::getDeviceId();

	for (std::map<std::string, cl_kernel>::iterator it = kernelTable.begin(); it != kernelTable.end(); ++it) {
		clReleaseKernel(it->second);
	}
	for (std::map<std::string, cl_program>::iterator it = programTable.begin(); it != programTable.end(); ++it) {
		clReleaseProgram(it->second);
	}
	programTable.clear();
	kernelTable.clear();
	ready = true;
}

cl_device_id kernelManager::getDeviceID(int k)
{
	cl_uint platform_num;
	clGetPlatformIDs(0, NULL, &platform_num);
	cl_platform_id* platform_id = new cl_platform_id[platform_num];
	clGetPlatformIDs(platform_num, platform_id, NULL);
	cl_uint device_num;
	cl_device_id* device_id;
	int device_count = 0;
	for (int i = 0; i < platform_num; i++) {
		clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_ALL, NULL, NULL, &device_num);
		device_id = new cl_device_id[device_num];
		clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_ALL, device_num, device_id, NULL);
		size_t name_size;
		char* device_name;
		if (k - device_count >= device_num)
			device_count += device_num;
		else
			return device_id[k - device_count];
		delete[] device_id;
	}
	delete[] platform_id;
	return nullptr;
}


