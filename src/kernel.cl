
 void atomicAdd_g_f(volatile __global float *addr, float val)
{
	union {
		unsigned int u32;
		float f32;
	} next, expected, current;
	current.f32 = *addr;
	do {
		expected.f32 = current.f32;
		next.f32 = expected.f32 + val;
		current.u32 = atomic_cmpxchg((volatile __global unsigned int *)addr,
			expected.u32, next.u32);
	} while (current.u32 != expected.u32);
}


__kernel void rowSum(__global double* data, __global int* rowVec, __global double* colVec, __global float* result) {
	int colID = get_global_id(0);
	int rowID;
	size_t dataID;
	unsigned int nonzero = colVec[colID + 1] - colVec[colID];
	for (unsigned int i = 0; i < nonzero; i++) {
		dataID = (size_t)colVec[colID] + i;
		rowID = rowVec[dataID];
		//float test = result[rowID];
		atomicAdd_g_f(result + rowID, data[dataID]);
	}
}


__kernel void colSum(__global double* data, __global int* rowVec, __global double* colVec, __global float* result) {
	int colID = get_global_id(0);
	size_t dataID;
	unsigned int nonzero = colVec[colID + 1] - colVec[colID];
	for (unsigned int i = 0; i < nonzero; i++) {
		dataID = (size_t)colVec[colID] + i;
		//rowID = rowVec[dataID];
		//float test = result[rowID];
		result[colID] = result[colID] + data[dataID];
	}
}