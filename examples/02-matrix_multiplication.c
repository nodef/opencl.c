// Example 2: Matrix Multiplication using OpenCL
#include <CL/opencl.h>
#include <stdio.h>
#include <stdlib.h>

#define MATRIX_SIZE 16

const char *kernelSource =
"__kernel void matrix_mul(__global const float *A, __global const float *B, __global float *C, int N) {\n" \
"    int row = get_global_id(0);\n" \
"    int col = get_global_id(1);\n" \
"    float sum = 0.0f;\n" \
"    for (int k = 0; k < N; k++) {\n" \
"        sum += A[row * N + k] * B[k * N + col];\n" \
"    }\n" \
"    C[row * N + col] = sum;\n" \
"}\n";

int main() {
    float *A = (float *)malloc(sizeof(float) * MATRIX_SIZE * MATRIX_SIZE);
    float *B = (float *)malloc(sizeof(float) * MATRIX_SIZE * MATRIX_SIZE);
    float *C = (float *)malloc(sizeof(float) * MATRIX_SIZE * MATRIX_SIZE);

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            A[i * MATRIX_SIZE + j] = i + j;
            B[i * MATRIX_SIZE + j] = i - j;
        }
    }

    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);

    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, MATRIX_SIZE * MATRIX_SIZE * sizeof(float), NULL, NULL);
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY, MATRIX_SIZE * MATRIX_SIZE * sizeof(float), NULL, NULL);
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, MATRIX_SIZE * MATRIX_SIZE * sizeof(float), NULL, NULL);

    clEnqueueWriteBuffer(queue, bufferA, CL_TRUE, 0, MATRIX_SIZE * MATRIX_SIZE * sizeof(float), A, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufferB, CL_TRUE, 0, MATRIX_SIZE * MATRIX_SIZE * sizeof(float), B, 0, NULL, NULL);

    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    cl_kernel kernel = clCreateKernel(program, "matrix_mul", NULL);
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
    clSetKernelArg(kernel, 3, sizeof(int), &MATRIX_SIZE);

    size_t globalSize[2] = {MATRIX_SIZE, MATRIX_SIZE};
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, NULL, 0, NULL, NULL);

    clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, MATRIX_SIZE * MATRIX_SIZE * sizeof(float), C, 0, NULL, NULL);

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%6.2f ", C[i * MATRIX_SIZE + j]);
        }
        printf("\n");
    }

    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    free(A);
    free(B);
    free(C);

    return 0;
}
