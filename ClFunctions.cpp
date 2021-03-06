#include "ClFunctions.h"

//Stavy z cviceni GMU
const char *CLErrorString(cl_int _err) {
    switch (_err) {
    case CL_SUCCESS:
        return "Success!";
    case CL_DEVICE_NOT_FOUND:
        return "Device not found.";
    case CL_DEVICE_NOT_AVAILABLE:
        return "Device not available";
    case CL_COMPILER_NOT_AVAILABLE:
        return "Compiler not available";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        return "Memory object allocation failure";
    case CL_OUT_OF_RESOURCES:
        return "Out of resources";
    case CL_OUT_OF_HOST_MEMORY:
        return "Out of host memory";
    case CL_PROFILING_INFO_NOT_AVAILABLE:
        return "Profiling information not available";
    case CL_MEM_COPY_OVERLAP:
        return "Memory copy overlap";
    case CL_IMAGE_FORMAT_MISMATCH:
        return "Image format mismatch";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        return "Image format not supported";
    case CL_BUILD_PROGRAM_FAILURE:
        return "Program build failure";
    case CL_MAP_FAILURE:
        return "Map failure";
    case CL_INVALID_VALUE:
        return "Invalid value";
    case CL_INVALID_DEVICE_TYPE:
        return "Invalid device type";
    case CL_INVALID_PLATFORM:
        return "Invalid platform";
    case CL_INVALID_DEVICE:
        return "Invalid device";
    case CL_INVALID_CONTEXT:
        return "Invalid context";
    case CL_INVALID_QUEUE_PROPERTIES:
        return "Invalid queue properties";
    case CL_INVALID_COMMAND_QUEUE:
        return "Invalid command queue";
    case CL_INVALID_HOST_PTR:
        return "Invalid host pointer";
    case CL_INVALID_MEM_OBJECT:
        return "Invalid memory object";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        return "Invalid image format descriptor";
    case CL_INVALID_IMAGE_SIZE:
        return "Invalid image size";
    case CL_INVALID_SAMPLER:
        return "Invalid sampler";
    case CL_INVALID_BINARY:
        return "Invalid binary";
    case CL_INVALID_BUILD_OPTIONS:
        return "Invalid build options";
    case CL_INVALID_PROGRAM:
        return "Invalid program";
    case CL_INVALID_PROGRAM_EXECUTABLE:
        return "Invalid program executable";
    case CL_INVALID_KERNEL_NAME:
        return "Invalid kernel name";
    case CL_INVALID_KERNEL_DEFINITION:
        return "Invalid kernel definition";
    case CL_INVALID_KERNEL:
        return "Invalid kernel";
    case CL_INVALID_ARG_INDEX:
        return "Invalid argument index";
    case CL_INVALID_ARG_VALUE:
        return "Invalid argument value";
    case CL_INVALID_ARG_SIZE:
        return "Invalid argument size";
    case CL_INVALID_KERNEL_ARGS:
        return "Invalid kernel arguments";
    case CL_INVALID_WORK_DIMENSION:
        return "Invalid work dimension";
    case CL_INVALID_WORK_GROUP_SIZE:
        return "Invalid work group size";
    case CL_INVALID_WORK_ITEM_SIZE:
        return "Invalid work item size";
    case CL_INVALID_GLOBAL_OFFSET:
        return "Invalid global offset";
    case CL_INVALID_EVENT_WAIT_LIST:
        return "Invalid event wait list";
    case CL_INVALID_EVENT:
        return "Invalid event";
    case CL_INVALID_OPERATION:
        return "Invalid operation";
    case CL_INVALID_GL_OBJECT:
        return "Invalid OpenGL object";
    case CL_INVALID_BUFFER_SIZE:
        return "Invalid buffer size";
    case CL_INVALID_MIP_LEVEL:
        return "Invalid mip-map level";
    default:
        return "Unknown";
    }
}

//vracia cas v s
double GetTime(void)
{
#if _WIN32  															/* toto jede na Windows */
    static int initialized = 0;
    static LARGE_INTEGER frequency;
    LARGE_INTEGER value;

    if (!initialized) {                         							/* prvni volani */
        initialized = 1;
        if (QueryPerformanceFrequency(&frequency) == 0) {                   /* pokud hi-res pocitadlo neni podporovano */
            //assert(0 && "HiRes timer is not available.");
            exit(-1);
        }
    }

    //assert(QueryPerformanceCounter(&value) != 0 && "This should never happen.");  /* osetreni chyby */
    QueryPerformanceCounter(&value);
    return (double)value.QuadPart / (double)frequency.QuadPart;  			/* vrat hodnotu v sekundach */

#else                                         							/* toto jede na Linux/Unixovych systemech */
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {        							/* vezmi cas */
        //assert(0 && "gettimeofday does not work.");  						/* osetri chyby */
        exit(-2);
    }
    return (double)tv.tv_sec + (double)tv.tv_usec/1000000.;  				/* vrat cas v sekundach */
#endif
}

//test navratovej hodnoty cl funkcii
void CheckOpenCLError(cl_int _ciErr, const char *_sMsg, ...)
{
    unsigned int uiDebug = 2;
    char buffer[1024];

    va_list arg;
    va_start (arg, _sMsg);
    vsprintf (buffer, _sMsg, arg);
    va_end (arg);
	if (_ciErr != CL_SUCCESS) {
		printf("%f: ERROR: %s: (%i)%s\n", GetTime(), buffer, _ciErr, CLErrorString(_ciErr));
		system("PAUSE");
		exit(1);
	}
	else {
		if (uiDebug > 1) {
			if (_DEBUG != 0)
				printf("%f:    OK: %s\n", GetTime(), buffer);
		}
	}
}

//nacitanie zdrojoveho kodu  
char* LoadProgSource(const char* cFilename)
{
    FILE* pFileStream = NULL;
    size_t szSourceLength;

    // otevri OpenCL zdrojak
#ifdef _WIN32   // Windows verzia
    if(fopen_s(&pFileStream, cFilename, "rb") != 0)
    {
        return NULL;
    }
#else           // Linux verzia
    pFileStream = fopen(cFilename, "rb");
    if(pFileStream == 0)
    {
        return NULL;
    }
#endif

    // ziskame velikost suboru
    fseek(pFileStream, 0, SEEK_END);
    szSourceLength = ftell(pFileStream);
    fseek(pFileStream, 0, SEEK_SET);

    // alokace
    char* cSourceString = (char *)malloc(szSourceLength + 1);
    if (fread(cSourceString, szSourceLength, 1, pFileStream) != 1)
    {
        fclose(pFileStream);
        free(cSourceString);
        return 0;
    }

    // finito
    fclose(pFileStream);
    cSourceString[szSourceLength] = '\0';

    return cSourceString;
}

//Vypocet fault formation na GPU
void faultFormationCl(int MATRIX_W, int MATRIX_H, float** height, int iter, float displacement){
    
    cl_int errNum;
    cl_int err_msg;
    cl_device_id gpu_device;
    
	if(myClLoadDevice(&gpu_device, MATRIX_W, MATRIX_H, &err_msg))
		computeFaultGpu(&gpu_device, MATRIX_W, MATRIX_H, displacement, &err_msg, &errNum, height, iter);
}

//Nacitanie zariadeni
int myClLoadDevice(cl_device_id *gpu_device, int MATRIX_W, int MATRIX_H, cl_int *err_msg){
    char tmp_buf[TMP_BUFFER_SIZE];

    cl_platform_id *platforms;
    cl_device_id *tmp_devices;
    cl_uint platforms_count;
    
    
    // Get Platforms count
    CheckOpenCLError(clGetPlatformIDs(0, NULL, &platforms_count),"clGetPlatformIDs");
    // Allocate space for platforms
    platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id) * platforms_count);
    // Get Platforms
    CheckOpenCLError(clGetPlatformIDs(platforms_count, platforms, NULL),"clGetPlatformIDs");
	if (_DEBUG != 0)
		printf("Platforms:\n");
    for(int i = 0; i < platforms_count; i++)
            {
                    cl_uint tmp_devices_count;
                    // Get platform name
                    CheckOpenCLError(clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, TMP_BUFFER_SIZE, tmp_buf, NULL),"clGetPlatformInfo");
					if (_DEBUG != 0)
						printf(" %d. platform name: %s.\n", i, tmp_buf);
                    // Get platform devices count
                    if((*err_msg = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &tmp_devices_count)) == CL_DEVICE_NOT_FOUND) continue;
                    CheckOpenCLError(*err_msg,"clGetDeviceIDs");
                    // Allocate space for devices
                    tmp_devices = (cl_device_id *)malloc(sizeof(cl_device_id) * tmp_devices_count);
                    // Get all platform devices
                    if((*err_msg = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, tmp_devices_count, tmp_devices, 0)) == CL_DEVICE_NOT_FOUND) continue;
                    CheckOpenCLError(*err_msg,"clGetDeviceIDs");
                    for(int j = 0; j < tmp_devices_count; j++)
                            {
                                    // Get device name
                                    CheckOpenCLError(clGetDeviceInfo(tmp_devices[j], CL_DEVICE_NAME, TMP_BUFFER_SIZE, tmp_buf, NULL),"clGetDeviceInfo");
									if (_DEBUG != 0)
										printf("  %d. device name: %s.\n", j, tmp_buf);
                            }
                    free(tmp_devices);
            }

    *gpu_device = NULL;
    cl_device_type device_type1;    

    for(int i = 0; i < platforms_count; i++)
    {
            cl_uint tmp_devices_count;
            // Get platform devices count
            if((*err_msg = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &tmp_devices_count)) == CL_DEVICE_NOT_FOUND) continue;
            CheckOpenCLError(*err_msg,"clGetDeviceIDs");
            // Allocate space for devices
            tmp_devices = (cl_device_id *)malloc(sizeof(cl_device_id) * tmp_devices_count);
            // Get all platform devices
            if((*err_msg = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, tmp_devices_count, tmp_devices, 0)) == CL_DEVICE_NOT_FOUND) continue;
            CheckOpenCLError(*err_msg,"clGetDeviceIDs");
            for(int j = 0; j < tmp_devices_count; j++)
                    {
                            // Get device type
                            CheckOpenCLError(clGetDeviceInfo(tmp_devices[j], CL_DEVICE_TYPE, sizeof(cl_device_type), &device_type1, NULL),"clGetDeviceInfo");
                            if (device_type1 == CL_DEVICE_TYPE_GPU)
                            {
                                    *gpu_device = tmp_devices[j];
                            }
                    }
            //free(tmp_devices);
    }

    // check if device is correct
    cl_device_type device_type;
    CheckOpenCLError(clGetDeviceInfo(*gpu_device, CL_DEVICE_NAME, TMP_BUFFER_SIZE, tmp_buf, NULL), "clGetDeviceInfo");
    CheckOpenCLError(clGetDeviceInfo(*gpu_device, CL_DEVICE_TYPE, sizeof(cl_device_type), &device_type, NULL), "clGetDeviceInfo");
	
	if (device_type == CL_DEVICE_TYPE_GPU)
	{
		if (_DEBUG != 0)
			printf("\nSelected device type: Correct\n");
	}
	else
	{
		printf("\nSelected device type: Incorrect\n Comupting skipped.\n");
		return 0;
	}
	if (_DEBUG != 0)
		printf("Selected device name: %s.\n", tmp_buf);


    free(platforms);
	return 1;
}

void random_save(int *rand_array, float *rand_array2, int size){
	for (int ik = 0; ik < size; ik++) {
		rand_array[ik] = rand();
		rand_array2[ik] = (float)rand() / (float)RAND_MAX;
	}
}

//Vypocetna cast fault algoritmu
void computeFaultGpu(cl_device_id *gpu_device, int MATRIX_W, int MATRIX_H, float displacement, cl_int *err_msg, cl_int *errNum, float** height, int iteration)
{
    cl_float *device_data = (cl_float *)malloc(sizeof(cl_float) * MATRIX_W * MATRIX_H);
	memset(device_data, 0, MATRIX_H*MATRIX_W*sizeof(cl_float));

    
    cl_context context = NULL;
    cl_command_queue queue = NULL;

    context = clCreateContext(NULL, 1, gpu_device, NULL, NULL, errNum);

    queue = clCreateCommandQueue(context, *gpu_device, CL_QUEUE_PROFILING_ENABLE, errNum);

    char *program_source = LoadProgSource("faultFormation.cl");

    // get program
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&program_source, NULL, err_msg);
    CheckOpenCLError(*err_msg,"clCreateProgramWithSource");

    // build program
    //CheckOpenCLError(clBuildProgram(program, 1, gpu_device, "", NULL, NULL),"clBuildProgram");
	clBuildProgram(program, 1, gpu_device, "", NULL, NULL), "clBuildProgram";
	char *build_log;

	size_t ret_val_size;

	(clGetProgramBuildInfo(program, *gpu_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size));

	build_log = new char[ret_val_size + 1];

	(clGetProgramBuildInfo(program, *gpu_device, CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL));

	// create kernel
	cl_kernel kernel = clCreateKernel(program, "faultFormation", err_msg);
	CheckOpenCLError(*err_msg, "clCreateKernel");

	double dtStart = GetTime();

	int *randomArray1 = (int*)malloc(iteration*sizeof(int));
	float *randomArray2 = (float*)malloc(iteration*sizeof(float));

	random_save(randomArray1, randomArray2, iteration);

	/*Buffers*/
	cl_mem h_buffer;
	cl_mem r1_buffer;
	cl_mem r2_buffer;

	h_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, MATRIX_H*MATRIX_W*sizeof(cl_float), device_data, errNum);
	r1_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, iteration*sizeof(cl_int), randomArray1, errNum);
	r2_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, iteration*sizeof(cl_float), randomArray2, errNum);

	cl_int matrix_width = MATRIX_W;
	//cl_int matrix_height = MATRIX_H;

	cl_float i_param = iteration;
	cl_float disp_param = displacement;

	/*Parametry*/
	clSetKernelArg(kernel, 0, sizeof(h_buffer), &h_buffer);
	clSetKernelArg(kernel, 1, sizeof(r1_buffer), &r1_buffer);
	clSetKernelArg(kernel, 2, sizeof(r2_buffer), &r2_buffer);
	clSetKernelArg(kernel, 3, sizeof(i_param), &i_param);
	clSetKernelArg(kernel, 4, sizeof(disp_param), &disp_param);
	clSetKernelArg(kernel, 5, sizeof(matrix_width), &matrix_width);
	//clSetKernelArg(kernel, 6, sizeof(matrix_height), &matrix_height);

	size_t local[2] = { 16, 16 };
	size_t global[2] = { MATRIX_W, MATRIX_H };

	clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, NULL);

	clEnqueueReadBuffer(queue, h_buffer, CL_FALSE, 0, MATRIX_W*MATRIX_H*sizeof(cl_int), device_data, 0, NULL, NULL);

	// synchronize queue
	CheckOpenCLError(clFinish(queue), "clFinish");

	clReleaseMemObject(h_buffer);

	//copy result
	for (int z = 0; z < MATRIX_H; z++){
		for (int x = 0; x < MATRIX_W; x++)
		{
			height[z][x] = device_data[z*MATRIX_W + x];
		}
	}

	double dtStop = GetTime();
	printf("Compute GPU time: %f\n", dtStop - dtStart);

	CheckOpenCLError(clReleaseKernel(kernel), "clReleaseKernel");
	CheckOpenCLError(clReleaseProgram(program), "clReleaseProgram");

	CheckOpenCLError(clReleaseCommandQueue(queue), "clReleaseCommandQueue");
	CheckOpenCLError(clReleaseContext(context), "clReleaseContext");

    // deallocate host data
    free(device_data);
}

// vypocet perlin noise na GPU
void perlinNoiseCl(float persistence, int octaves, int MATRIX_W, int MATRIX_H, float** height){

	cl_int errNum;
	cl_int err_msg;
	cl_device_id gpu_device;

	if(myClLoadDevice(&gpu_device, MATRIX_W, MATRIX_H, &err_msg))
		computePerlinGpu(&gpu_device, MATRIX_W, MATRIX_H, persistence, octaves, &err_msg, &errNum, height);
}

//samotny vypocet perlin noise pomocou GPU
void computePerlinGpu(cl_device_id *gpu_device, int MATRIX_W, int MATRIX_H, float persistence, int octaves, cl_int *err_msg, cl_int *errNum, float** height)
{
	cl_float *device_data = (cl_float *)malloc(sizeof(cl_float) * MATRIX_W * MATRIX_H);
	memset(device_data, 0, MATRIX_H*MATRIX_W*sizeof(cl_float));

	cl_context context = NULL;
	cl_command_queue queue = NULL;

	context = clCreateContext(NULL, 1, gpu_device, NULL, NULL, errNum);

	queue = clCreateCommandQueue(context, *gpu_device, CL_QUEUE_PROFILING_ENABLE, errNum);

	char *program_source = LoadProgSource("perlinNoise.cl");

	// get program
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&program_source, NULL, err_msg);
	CheckOpenCLError(*err_msg, "clCreateProgramWithSource");

	if (_DEBUG != 0) {
		clBuildProgram(program, 1, gpu_device, "", NULL, NULL);
	}
	else{
		// build program
		CheckOpenCLError(clBuildProgram(program, 1, gpu_device, "", NULL, NULL), "clBuildProgram");
	}

	char *build_log;

	size_t ret_val_size;

	(clGetProgramBuildInfo(program, *gpu_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size));

	build_log = new char[ret_val_size + 1];

	(clGetProgramBuildInfo(program, *gpu_device, CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL));

	// create kernel
	cl_kernel kernel = clCreateKernel(program, "perlinNoise", err_msg);
	CheckOpenCLError(*err_msg, "clCreateKernel");

	double dtStart = GetTime();

	/*Buffers*/
	cl_mem h_buffer;

	h_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, MATRIX_H*MATRIX_W*sizeof(cl_float), device_data, errNum);

	cl_int matrix_width = MATRIX_W;
	cl_int matrix_height = MATRIX_H;

	cl_float persistence_param = persistence;
	cl_int octaves_param = octaves;

	/*Parametry*/
	clSetKernelArg(kernel, 0, sizeof(h_buffer), &h_buffer);
	clSetKernelArg(kernel, 1, sizeof(persistence_param), &persistence_param);
	clSetKernelArg(kernel, 2, sizeof(octaves_param), &octaves_param);
	clSetKernelArg(kernel, 3, sizeof(matrix_width), &matrix_width);
	clSetKernelArg(kernel, 4, sizeof(matrix_height), &matrix_height);

	size_t local[2] = { 16, 16 };
	size_t global[2] = { MATRIX_W, MATRIX_H };

	clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, NULL);

	clEnqueueReadBuffer(queue, h_buffer, CL_FALSE, 0, MATRIX_W*MATRIX_H*sizeof(cl_int), device_data, 0, NULL, NULL);

	// synchronize queue
	CheckOpenCLError(clFinish(queue), "clFinish");

	clReleaseMemObject(h_buffer);

	//copy result
	for (int z = 0; z < MATRIX_H; z++){
		for (int x = 0; x < MATRIX_W; x++)
		{
			height[z][x] = device_data[z*MATRIX_W + x];
		}
	}

	double dtStop = GetTime();
	printf("Compute GPU time: %f\n", dtStop - dtStart);

	CheckOpenCLError(clReleaseKernel(kernel), "clReleaseKernel");
	CheckOpenCLError(clReleaseProgram(program), "clReleaseProgram");

	CheckOpenCLError(clReleaseCommandQueue(queue), "clReleaseCommandQueue");
	CheckOpenCLError(clReleaseContext(context), "clReleaseContext");

	// deallocate host data
	free(device_data);
}
