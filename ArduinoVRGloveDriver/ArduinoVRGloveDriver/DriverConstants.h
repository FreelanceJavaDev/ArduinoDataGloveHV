#ifndef DRIVERCONSTANTS_H
#define DRIVERCONSTANTS_H
#include "stdafx.h"
#include "driverlog.h"

#include <vector>
#include <thread>
#include <chrono>

#if defined( _WINDOWS )
#include <windows.h>
#endif

using namespace vr;


#if defined(_WIN32)
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#define HMD_DLL_IMPORT extern "C" __declspec( dllimport )
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define HMD_DLL_IMPORT extern "C" 
#else
#error "Unsupported Platform."
#endif

inline HmdQuaternion_t HmdQuaternion_Init(double w, double x, double y, double z)
{
	HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
};

inline void HmdMatrix_SetIdentity(HmdMatrix34_t *pMatrix)
{
	pMatrix->m[0][0] = 1.f;
	pMatrix->m[0][1] = 0.f;
	pMatrix->m[0][2] = 0.f;
	pMatrix->m[0][3] = 0.f;
	pMatrix->m[1][0] = 0.f;
	pMatrix->m[1][1] = 1.f;
	pMatrix->m[1][2] = 0.f;
	pMatrix->m[1][3] = 0.f;
	pMatrix->m[2][0] = 0.f;
	pMatrix->m[2][1] = 0.f;
	pMatrix->m[2][2] = 1.f;
	pMatrix->m[2][3] = 0.f;
};

// keys for use with the settings API
static const char * const k_pch_Test_Section = "driver_test";
static const char * const k_pch_Test_SerialNumber_String = "SerialTest";
static const char * const k_pch_Test_ModelNumber_String = "devDriver";
static const char * const k_pch_Test_WindowX_Int32 = "windowX";
static const char * const k_pch_Test_WindowY_Int32 = "windowY";
static const char * const k_pch_Test_WindowWidth_Int32 = "windowWidth";
static const char * const k_pch_Test_WindowHeight_Int32 = "windowHeight";
static const char * const k_pch_Test_RenderWidth_Int32 = "renderWidth";
static const char * const k_pch_Test_RenderHeight_Int32 = "renderHeight";
static const char * const k_pch_Test_SecondsFromVsyncToPhotons_Float = "secondsFromVsyncToPhotons";
static const char * const k_pch_Test_DisplayFrequency_Float = "displayFrequency";

#endif
