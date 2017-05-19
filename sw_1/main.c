#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define SIZE 64

typedef enum {false, true} bool;

int main(int argc, char** argv) {
  int i;
  int j;
  unsigned int buf;
  float matrix[SIZE][SIZE];
  float vector[SIZE];
  float partial;
  float result[SIZE];
  FILE *input_file;
  FILE *output_file;
  bool input = false;
  bool output = false;
  bool debug = false;

  for (i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "-input")) {
      ++i;
      if (i < argc) {
        input_file = fopen(argv[i], "r");
        if (input_file == NULL) {
          printf(ANSI_COLOR_RED "[ERROR] %s INVALID FILE NAME\n" ANSI_COLOR_RESET, argv[0]);
          return -1;
        }
        input = true;
      }
      else {
        printf(ANSI_COLOR_RED "[ERROR] CANNOT FIND FILE NAME\n" ANSI_COLOR_RESET);
        return -1;
      }
    }

    else if(!strcmp(argv[i], "-o") || !strcmp(argv[i], "-output")) {
      ++i;
      if (i < argc) {
        output_file = fopen(argv[i], "w");
        if (output_file == NULL) {
          printf(ANSI_COLOR_RED "[ERROR] INVALID FILE NAME\n" ANSI_COLOR_RESET);
          return -1;
        }
        output = true;
      }
      else {
        printf(ANSI_COLOR_RED "[ERROR] CANNOT FIND FILE NAME\n" ANSI_COLOR_RESET);
        return -1;
      }
    }

    else if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "-debug")) {
      debug = true;
    }

    else {
      printf(ANSI_COLOR_RED "[ERROR] INVALID COMMAND OPTION\n" ANSI_COLOR_RESET);
      return -1;
    }
  }
  if (input) {
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
  }
  else {
    for (i = 0; i < SIZE; ++i)
      for (j = 0; j < SIZE; ++j)
        matrix[i][j] = (float)rand()/(float)(RAND_MAX);

    for (i = 0; i < SIZE; ++i)
      vector[i] = (float)rand()/(float)(RAND_MAX);
  }

  if (debug) {
    printf(ANSI_COLOR_CYAN "\n##### HEXADECIMAL POINT VALUE #####\n" ANSI_COLOR_RESET);
    for (i = 0; i < SIZE; ++i) {
      printf("[ ");
      for (j = 0; j < SIZE; ++j) {
        printf("%08X ", *((int*)&matrix[i][j]));
      }
      printf("][ %08X ]\n", *((int*)&vector[i]));
    }

    printf(ANSI_COLOR_CYAN "\n##### FLOATING POINT VALUE #####\n" ANSI_COLOR_RESET);
    for (i = 0; i < SIZE; ++i) {
      printf("[ ");
      for (j = 0; j < SIZE; ++j) {
        printf("%.3f ", matrix[i][j]);
      }
      printf("][ %.3f ]\n", vector[i]);
    }

    printf(ANSI_COLOR_CYAN "\n##### CALCULATION #####\n" ANSI_COLOR_RESET);
    for (i = 0; i < SIZE; ++i) {
      result[i] = 0;
      for (j = 0; j < SIZE; ++j) {
        partial = matrix[i][j] * vector[j];
        printf("[ %.3f ] %c [ %.3f ] %.3f + %.3f * %.3f = %.3f + %.3f = "
          , matrix[i][j], j==SIZE/2?'.':' ', vector[j]
          , result[i], matrix[i][j]
          , vector[j], result[i], partial);
        result[i] += partial;
        printf("%.3f\n", result[i]);
      }
      printf("\n");
    }
  }
  else {
    for (i = 0; i < SIZE; ++i) {
      result[i] = 0;
      for (j = 0; j < SIZE; ++j) {
        result[i] += matrix[i][j] * vector[j];
      }
    }
  }
  
  printf(ANSI_COLOR_CYAN "\n##### RESULT VECTOR #####\n" ANSI_COLOR_RESET);
  for (i = 0; i < SIZE; ++i) {
    printf("[ %.3f ] %s [ %08X ]\n", result[i], i==SIZE/2?"=>":"  ",*((int*)&result[i]));
  }

  if (!input) {
    input_file = fopen("app.input.txt", "w");
    for (i = 0; i < SIZE; ++i) {
      for (j = 0; j < SIZE; ++j) {
        buf = *((int*)&matrix[i][j]);
        fprintf(input_file, "%08X\n", buf);
      }
    }
    for (i = 0; i < SIZE; ++i) {
      buf = *((int*)&vector[i]);
      fprintf(input_file, "%08X\n", buf);
    }
    fclose(input_file);
  }

  if (!output)
    output_file = fopen("app.output.txt", "w");
  
  if (output) {
  for (i = 0; i < SIZE; ++i) {
    buf = *((int*)&result[i]);
    fprintf(output_file, "%08X\n", buf);
  }
  fclose(output_file);
  }
  return 0;
}
