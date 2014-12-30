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


#define _DEBUG 1

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <CL/opencl.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif //WIN32

double GetTime(void);
void faultFormationCl(int MATRIX_W, int MATRIX_H, float** height, int iter, float displacement);
int myClLoadDevice(cl_device_id *gpu_device, int MATRIX_W, int MATRIX_H, cl_int *err_msg);
void computeFaultGpu(cl_device_id *gpu_device, int MATRIX_W, int MATRIX_H, float displacement, cl_int *err_msg, cl_int *errNum, float** height, int iteration);

void computePerlinGpu(cl_device_id *gpu_device, int MATRIX_W, int MATRIX_H, float persistence, int octaves, cl_int *err_msg, cl_int *errNum, float** height);
void perlinNoiseCl(float persistence, int octaves, int MATRIX_W, int MATRIX_H, float** height);

