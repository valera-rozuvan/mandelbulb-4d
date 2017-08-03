#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "test_opencl.hpp"

#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)

int test_opencl(void)
{
  cl_device_id device_id = NULL;
  cl_context context = NULL;
  cl_command_queue command_queue = NULL;
  cl_mem memobj = NULL;
  cl_program program = NULL;
  cl_kernel kernel = NULL;
  cl_platform_id platform_id = NULL;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int ret;

  char string[MEM_SIZE];

  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    fprintf(stdout, "Current working dir: %s\n", cwd);
  } else {
    fprintf(stderr,"getcwd() failed!\n");
    return 1;
  }

  FILE *fp;
  char fileName[] = "hello.cl";

  #if defined(_WIN32) || defined(WIN32)
  char pathSeparator[] = "\\";
  #elif defined(__unix__) || defined(linux) || defined(__APPLE__)
  char pathSeparator[] = "/";
  #endif

  char * fullPath = (char*)malloc(sizeof(char) * (strlen(cwd) + strlen(pathSeparator) + strlen(fileName) + 1));
  if(fullPath != NULL) {
    fullPath[0] = '\0';   // ensures the memory is an empty string
    strcat(fullPath, cwd);
    strcat(fullPath, pathSeparator);
    strcat(fullPath, fileName);
  } else {
    fprintf(stderr, "malloc failed!\n");
    return 1;
  }

  char *source_str;
  size_t source_size;

  fprintf(stdout, "Full path: %s\n", fullPath);

  /* Load the source code containing the kernel*/
  fp = fopen(fullPath, "r");
  if (!fp) {
    fprintf(stderr, "Failed to load kernel.\n");
    return 1;
  }
  source_str = (char*)malloc(MAX_SOURCE_SIZE);
  source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);

  /* Get Platform and Device Info */
  ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);

  if (ret != CL_SUCCESS) {
    fprintf(stderr, "clGetPlatformIDs() failed!\n");
    return 1;
  }

  ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

  /* Create OpenCL context */
  context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

  /* Create Command Queue */
  command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

  /* Create Memory Buffer */
  memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, MEM_SIZE * sizeof(char), NULL, &ret);

  /* Create Kernel Program from the source */
  program = clCreateProgramWithSource(
    context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret
  );

  /* Build Kernel Program */
  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

  /* Create OpenCL Kernel */
  kernel = clCreateKernel(program, "hello", &ret);

  /* Set OpenCL Kernel Parameters */
  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);

  /* Execute OpenCL Kernel */
  ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);

  /* Copy results from the memory buffer */
  ret = clEnqueueReadBuffer(
    command_queue, memobj, CL_TRUE, 0, MEM_SIZE * sizeof(char), string, 0, NULL, NULL
  );

  /* Display Result */
  puts(string);

  /* Finalization */
  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(memobj);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);

  free(source_str);

  return 0;
}
