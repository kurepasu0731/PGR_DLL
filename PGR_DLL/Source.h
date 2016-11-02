#pragma once

#include "PGROpenCV.h"
#include <opencv2/opencv.hpp>


#define DLLExport __declspec (dllexport)

#define PGRWIDTH 1920
#define PGRHEIGHT 1200


//VideoWriter
cv::VideoWriter videoWriter;

extern "C" {
   DLLExport void* getPGR(int device);
   DLLExport void initPGR(void* camera, int device);
   DLLExport void releasePGR(void* camera, int device);
   DLLExport void getPGRTexture(void* camera, unsigned char* data, bool isRecord, bool isShowWin);
   DLLExport void showPixelData(unsigned char* data);
}