#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define SIZE 64
#define MAX_COMMAND_LENGTH 64
#define hex2float(x) (*((float*)&(x)))
#define float2hex(x) (*((unsigned int*)&(x)))

typedef enum {false, true} bool;

int main(int argc, char** argv) {
  int i;
  int j;
  int len;
  int mem;
  unsigned int buf;
  float matrix[SIZE][SIZE];
  float vector[SIZE];
  float result_cpu[SIZE];
  float result_fpga[SIZE];
  int mem_descriptor;
  float *fpga_bram;
  unsigned int *fpga_ip;
  FILE *input_file;

  // Initiailize prototype
  input_file = fopen("./app.input.txt", "r");
  if (input_file == NULL) {
    printf(ANSI_COLOR_RED "[ERROR] THERE IS NO INPUT FILE\n" ANSI_COLOR_RESET);
    return -1;
  }
  for (i = 0; i < SIZE; ++i) {
    for (j = 0; j < SIZE; ++j) {
      fscanf(input_file, "%x", &buf);
      matrix[i][j] = *((float*)&buf);
    }
  }
  for (i = 0; i < SIZE; ++i) {
    fscanf(input_file, "%x", &buf);
    vector[i] = *((float*)&buf);
  }
  fclose(input_file);

  for (i = 0; i < SIZE; ++i) {
    result_cpu[i] = 0;
    for (j = 0; j < SIZE; ++j) {
      result_cpu[i] += matrix[i][j] * vector[j];
    }
  }

  mem_descriptor = open("/dev/mem", O_RDWR | O_NONBLOCK);
  if (mem_descriptor < 0) {
    printf(ANSI_COLOR_RED "[ERROR] ERRORS ERUPT IN OPENNING MEM FILE\n" ANSI_COLOR_RESET);
    return -1;
  }
  fpga_bram = mmap(NULL, (SIZE*(SIZE + 1))* sizeof(float), PROT_WRITE, MAP_SHARED, mem_descriptor, 0x40000000);
  fpga_ip = mmap(NULL, sizeof(float), PROT_WRITE, MAP_SHARED, mem_descriptor, 0x43c00000);
  for (i = 0; i < SIZE; ++i) {
    for (j = 0; j < SIZE; ++j) {
      *(fpga_bram + i * SIZE + j) = matrix[i][j];
    }
  }
  for (i = 0; i < SIZE; ++i) {
    *(fpga_bram + i + SIZE * SIZE) = vector[i];
  }
  *fpga_ip = 0x5555;
  while (*fpga_ip == 0x5555);
  for (i = 0; i < SIZE; ++i) {
    result_fpga[i] = fpga_bram[i];
  }
  printf("%-10s%-10s%-10s%-10s\n", "index", "CPU", "FPGA", "FPGA(hex)");
  for (i = 0; i < SIZE; ++i) {
    printf("%-10d%-10f%-10f%-10X\n", i, result_cpu[i], result_fpga[i], float2hex(result_fpga[i]));
  }
  close(mem_descriptor);

  return 0;
}
