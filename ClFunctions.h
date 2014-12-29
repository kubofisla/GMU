/* 
 * File:   ClFunctions.h
 * Author: jakub
 *
 * Created on December 28, 2014, 11:30 AM
 */

#ifndef CLFUNCTIONS_H
#define	CLFUNCTIONS_H
#endif	/* CLFUNCTIONS_H */

#define TMP_BUFFER_SIZE 4096

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <CL/opencl.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif //WIN32

void faultFormationCl(float a, float b, float c, float d, int MATRIX_W, int MATRIX_H, float** height);
void myClLoadDevice(cl_device_id *gpu_device, int MATRIX_W, int MATRIX_H, cl_int *err_msg);
void computeGpu(cl_device_id *gpu_device, int MATRIX_W, int MATRIX_H, float a, float b, float c, float displacement, cl_int *err_msg, cl_int *errNum, float** height);


