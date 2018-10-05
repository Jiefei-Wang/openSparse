
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




double element_trans(double m) {
#ifndef OP_element_trans
	return m;
#else
	MagicCodeHere
	return m;
#endif
}

__kernel void rowSum(__global double* data, __global int* rowVec, __global double* colVec,
	 int nrow, int ncol,
	__global float* result) {
	int colID = get_global_id(0);
	size_t rowID;
	size_t dataID;
	size_t lastRowID = -1;
	double curData;
	unsigned int nonzero = colVec[colID + 1] - colVec[colID];
	for (size_t i = 0; i < nonzero; i++) {
		dataID = (size_t)colVec[colID] + i;
		rowID = rowVec[dataID];
#ifdef OP_element_trans
		if (rowID - lastRowID > 1) {
			for (size_t j = lastRowID + 1; j < rowID; j++) {
				curData = element_trans(0);
				atomicAdd_g_f(result + j, curData);
			}
		}
		lastRowID = rowID;
#endif
		curData = element_trans(data[dataID]);
		atomicAdd_g_f(result + rowID, curData);
	}
#ifdef OP_element_trans
	if (nrow - lastRowID > 1) {
		for (size_t j = lastRowID + 1; j < nrow; j++) {
			curData = element_trans(0);
			atomicAdd_g_f(result + j, curData);
		}
	}
#endif
}


__kernel void colSum(__global double* data, __global int* rowVec, __global double* colVec,
	 int nrow,  int ncol,
	__global float* result) {
	int colID = get_global_id(0);
	size_t dataID;
	unsigned int nonzero = colVec[colID + 1] - colVec[colID];
	for (size_t i = 0; i < nonzero; i++) {
		dataID = (size_t)colVec[colID] + i;
		double curData = element_trans(data[dataID]);
		result[colID] = result[colID] + curData;
	}
#ifdef OP_element_trans
	result[colID]= result[colID]+(nrow- nonzero)* element_trans(0);
#endif
}
