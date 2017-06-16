#include "fpga_api.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

#define DATA_SIZE SIZE*(SIZE+1) // fpga bram data size

#define min(x,y) (((x)<(y))?(x):(y))

FPGA::FPGA(off_t data_addr, off_t api_addr)
{
    api_ = new unsigned int[SIZE];    // use api_ as tempolar output
    data_ = new float[DATA_SIZE];
}

FPGA::~FPGA()
{
    delete[] api_;
    delete[] data_;
}

float* FPGA::matrix(void)
{
	return data_ + SIZE;
}

float* FPGA::vector(void)
{
	return data_;
}

const float* FPGA::run()
{
    float* vec = this->vector();
    float* mat = this->matrix();
    float* out  = reinterpret_cast<float*>(api_);

    for(int i = 0 ; i < SIZE; ++i)
    {
        out[i] = 0;

        for(int j = 0 ; j < SIZE; ++j) {
           out[i] += vec[j] * mat[SIZE*i + j];
        }
    }

	for(int i = 0 ; i < SIZE; ++i)
	{
		data_[i] = out[i];
	}

    return data_;
}

void FPGA::largeMV(const float* large_mat, const float* input,
		float* output, int M, int N)
{
	float* vec = this->vector();
  float* mat = this->matrix();
  memset(output, 0, sizeof(float)*N);

  int i, j, k;
  for (i = 0; i < M / SIZE; ++i) {
    for (j = 0; j < N / SIZE; ++j) {
      memcpy(vec, input + i * SIZE, sizeof(float)*SIZE);
      for (k = 0; k < SIZE; ++k) {
        memcpy(mat + k*SIZE, large_mat + i * SIZE + j * M * SIZE + k * M, sizeof(float)*SIZE);
      }

      const float* output_temp = this->run();

      for (k = 0; k < SIZE; ++k) {
        *(output + j * SIZE + k) = *(output + j * SIZE + k) + *(output_temp + k);
      }
    }

    ///////////////////////// LAST_ROW
    memcpy(vec, input + i * SIZE, sizeof(float)*SIZE);
    int ROW_LAST_SIZE = N - j * SIZE;
    for(k = 0; k < ROW_LAST_SIZE; ++k) {
      memcpy(mat + k*SIZE, large_mat + i * SIZE + j*M*SIZE + k*M, sizeof(float)*SIZE);
    }
    for( ; k<SIZE ; ++k) {
      memset(mat + k*SIZE, 0, sizeof(float)*SIZE);
    }

    const float* output_temp = this->run();

    for (k = 0; k < ROW_LAST_SIZE; ++k) {
      *(output + j * SIZE + k) = *(output + j * SIZE + k) + *(output_temp + k);
    }
    //////////////////////////
  }

  int COLUMN_LAST_SIZE = M - i * SIZE;
  if(COLUMN_LAST_SIZE) {
    for (j = 0; j < N / SIZE; ++j) {
      memset(vec, 0, sizeof(float)*SIZE);
      memcpy(vec, input + i * SIZE, sizeof(float)*COLUMN_LAST_SIZE);

      for (k = 0; k < SIZE; ++k) {
        memset(mat + k * SIZE, 0, sizeof(float)*SIZE);
        memcpy(mat + k * SIZE, large_mat + i * SIZE + j * M * SIZE + k * M, sizeof(float)*COLUMN_LAST_SIZE);
      }

      const float* output_temp = this->run();

      for (k = 0; k < SIZE; ++k) {
        *(output + j * SIZE + k) = *(output + j * SIZE + k) + *(output_temp + k);
      }
    }

    /////////////////////// LAST_ROW
    memset(vec, 0, sizeof(float)*SIZE);
    memcpy(vec, input + i * SIZE, sizeof(float)*COLUMN_LAST_SIZE);
    int ROW_LAST_SIZE = N - j * SIZE;
    for(k = 0; k < ROW_LAST_SIZE; ++k) {
      memset(mat + k * SIZE, 0, sizeof(float)*SIZE);
      memcpy(mat + k*SIZE, large_mat + i * SIZE + j*M*SIZE + k*M, sizeof(float)*COLUMN_LAST_SIZE);
    }
    for( ; k < SIZE; ++k) {
      memset(mat + k *SIZE, 0, sizeof(float)*SIZE);
    }

    const float* output_temp = this->run();

    for (k = 0; k < ROW_LAST_SIZE; ++k) {
      *(output + j * SIZE + k) = *(output + j * SIZE + k) + *(output_temp + k);
    }
    /////////////////////////////////
  }
}
