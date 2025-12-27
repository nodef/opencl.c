// Example 3: Image Processing (Grayscale Conversion) using OpenCL
#include <CL/opencl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define IMAGE_WIDTH 512
#define IMAGE_HEIGHT 512

const char *kernelSource =
"__kernel void grayscale(__read_only image2d_t inputImage, __write_only image2d_t outputImage) {\n" \
"    int2 coord = (int2)(get_global_id(0), get_global_id(1));\n" \
"    float4 pixel = read_imagef(inputImage, coord);\n" \
"    float gray = 0.299f * pixel.x + 0.587f * pixel.y + 0.114f * pixel.z;\n" \
"    write_imagef(outputImage, coord, (float4)(gray, gray, gray, 1.0f));\n" \
"}\n";

int main() {
    // Simulated input image (RGBA format)
    uint8_t *inputImage = (uint8_t *)malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 4);
    uint8_t *outputImage = (uint8_t *)malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 4);

    for (int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT * 4; i++) {
        inputImage[i] = rand() % 256;
    }

    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);

    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

    cl_image_format format = {CL_RGBA, CL_UNORM_INT8};
    cl_image_desc desc = {CL_MEM_OBJECT_IMAGE2D, IMAGE_WIDTH, IMAGE_HEIGHT, 0, 0, 0, 0, 0, 0, NULL};

    cl_mem inputImageBuffer = clCreateImage(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &format, &desc, inputImage, NULL);
    cl_mem outputImageBuffer = clCreateImage(context, CL_MEM_WRITE_ONLY, &format, &desc, NULL, NULL);

    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    cl_kernel kernel = clCreateKernel(program, "grayscale", NULL);
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputImageBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputImageBuffer);

    size_t globalSize[2] = {IMAGE_WIDTH, IMAGE_HEIGHT};
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, NULL, 0, NULL, NULL);

    size_t origin[3] = {0, 0, 0};
    size_t region[3] = {IMAGE_WIDTH, IMAGE_HEIGHT, 1};
    clEnqueueReadImage(queue, outputImageBuffer, CL_TRUE, origin, region, 0, 0, outputImage, 0, NULL, NULL);

    printf("Grayscale conversion completed.\n");

    clReleaseMemObject(inputImageBuffer);
    clReleaseMemObject(outputImageBuffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    free(inputImage);
    free(outputImage);

    return 0;
}
