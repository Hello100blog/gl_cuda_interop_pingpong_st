
__constant__ unsigned int  volumeDim;
__constant__ float  cellWidth;
__constant__ float  volumeCoeff;
__constant__ float  alpha;

surface<void, 3> volumeTexOut;
texture<float, 3, cudaReadModeElementType> volumeTexIn;

extern "C"  __global__ void heatEquationStep(float dt)
{
	int i = 1 + blockIdx.x * blockDim.x + threadIdx.x ;
	int j = 1 + blockIdx.y * blockDim.y + threadIdx.y ;
	int k = 1 + blockIdx.z * blockDim.z + threadIdx.z ;
	
	float val = tex3D(  volumeTexIn, i,     j,   k   );
	float valU = tex3D( volumeTexIn, (i+1), j,   k   );
	float valD = tex3D( volumeTexIn, (i-1), j,   k   );
	float valN = tex3D( volumeTexIn, i,     j+1, k   );
	float valS = tex3D( volumeTexIn, i,     j-1, k   );
	float valE = tex3D( volumeTexIn, i,     j,   k+1 );
	float valW = tex3D( volumeTexIn, i,     j,   k-1 );
	
	float negSixVal = -6.0f*val;

	float result = val + dt * alpha * (negSixVal + valU + valD + valN + valS + valE + valW) / (2.0f * cellWidth * cellWidth);

    surf3Dwrite(result, volumeTexOut, i * sizeof(float), j, k);
}