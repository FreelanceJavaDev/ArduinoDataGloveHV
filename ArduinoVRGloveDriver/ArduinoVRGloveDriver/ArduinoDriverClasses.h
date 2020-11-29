#ifndef ARDUINODRIVERCLASSES_H
#define ARDUINODRIVERCLASSES_H
#pragma once
#include "DriverConstants.h"


class WatchdogDriver_Arduino : public IVRWatchdogProvider
{
public:
	WatchdogDriver_Arduino() { m_pWatchdogThread = nullptr; }

	virtual EVRInitError Init(vr::IVRDriverContext *pDriverContext);
	virtual void Cleanup();

private:
	std::thread *m_pWatchdogThread;
};

WatchdogDriver_Arduino g_watchdogDriverNull;


class ArduinoDeviceDriver : public vr::ITrackedDeviceServerDriver, public vr::IVRDisplayComponent { 
public:
	ArduinoDeviceDriver();
	virtual ~ArduinoDeviceDriver() {}
	virtual EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId);
	virtual void Deactivate() { m_unObjectId = vr::k_unTrackedDeviceIndexInvalid; }
	virtual void EnterStandby() {}
	void *GetComponent(const char *pchComponentNameAndVersion);
	virtual void PowerOff() {}
	virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize);
	virtual void GetWindowBounds(int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight);
	virtual bool IsDisplayOnDesktop() //Just testing Glove for now
	{ return true; }

	virtual bool IsDisplayRealDisplay() //Just testing Glove for now, no need for HMD display
	{ return false; }

	virtual void GetRecommendedRenderTargetSize(uint32_t *pnWidth, uint32_t *pnHeight)
	{
		*pnWidth = m_nRenderWidth;
		*pnHeight = m_nRenderHeight;
	}

	virtual void GetEyeOutputViewport(EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight); 
	virtual void GetWindowBounds(int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight);
	virtual void GetProjectionRaw(EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom);

	virtual DistortionCoordinates_t ComputeDistortion(EVREye eEye, float fU, float fV);
	virtual DriverPose_t GetPose();


	void RunFrame();

	std::string GetSerialNumber() const { return m_sSerialNumber; }

private:
	vr::TrackedDeviceIndex_t m_unObjectId;
	vr::PropertyContainerHandle_t m_ulPropertyContainer;

	std::string m_sSerialNumber;
	std::string m_sModelNumber;

	int32_t m_nWindowX;
	int32_t m_nWindowY;
	int32_t m_nWindowWidth;
	int32_t m_nWindowHeight;
	int32_t m_nRenderWidth;
	int32_t m_nRenderHeight;
	float m_flSecondsFromVsyncToPhotons;
	float m_flDisplayFrequency;
	float m_flIPD;

};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class ArduinoControllerDriver : public vr::ITrackedDeviceServerDriver
{
public:
	ArduinoControllerDriver();
	virtual ~ArduinoControllerDriver() {}

	virtual EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId);

	virtual void Deactivate() { m_unObjectId = vr::k_unTrackedDeviceIndexInvalid; }

	virtual void EnterStandby() {}

	void *GetComponent( const char *pchComponentNameAndVersion )
	{
		// override this to add a component to a driver
		return NULL;
	}

	virtual void PowerOff() {}

	/** debug request from a client */
	virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize);
	virtual DriverPose_t GetPose();


	void RunFrame();

	void ProcessEvent(const vr::VREvent_t & vrEvent);


	std::string GetSerialNumber() const { return m_sSerialNumber; }

private:
	vr::TrackedDeviceIndex_t m_unObjectId;
	vr::PropertyContainerHandle_t m_ulPropertyContainer;

	vr::VRInputComponentHandle_t m_compA;
	//vr::VRInputComponentHandle_t m_compHaptic;

	std::string m_sSerialNumber;
	std::string m_sModelNumber;

};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class ServerDriver: public vr::IServerTrackedDeviceProvider
{
public:
	virtual EVRInitError Init( vr::IVRDriverContext *pDriverContext ) ;
	virtual void Cleanup() ;
	virtual const char * const *GetInterfaceVersions() { return vr::k_InterfaceVersions; }
	virtual void RunFrame() ;
	virtual bool ShouldBlockStandbyMode()  { return false; }
	virtual void EnterStandby()  {}
	virtual void LeaveStandby()  {}

private:
	ArduinoDeviceDriver *m_pNullHmdLatest = nullptr;
	ArduinoControllerDriver *m_pController = nullptr;
};

ServerDriver g_serverDriverNull;

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
HMD_DLL_EXPORT void *HmdDriverFactory( const char *pInterfaceName, int *pReturnCode )
{
	if( 0 == strcmp( IServerTrackedDeviceProvider_Version, pInterfaceName ) )
	{ return &g_serverDriverNull; }
	if( 0 == strcmp( IVRWatchdogProvider_Version, pInterfaceName ) )
	{ return &g_watchdogDriverNull; }

	if (pReturnCode) { *pReturnCode = VRInitError_Init_InterfaceNotFound; }
	return NULL;
}


#endif