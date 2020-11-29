#include "ArduinoDriverClasses.h"
EVRInitError ServerDriver::Init(vr::IVRDriverContext *pDriverContext)
{
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
	InitDriverLog(vr::VRDriverLog());

	m_pNullHmdLatest = new ArduinoDeviceDriver();
	vr::VRServerDriverHost()->TrackedDeviceAdded(m_pNullHmdLatest->GetSerialNumber().c_str(), vr::TrackedDeviceClass_HMD, m_pNullHmdLatest);

	m_pController = new ArduinoControllerDriver();
	vr::VRServerDriverHost()->TrackedDeviceAdded(m_pController->GetSerialNumber().c_str(), vr::TrackedDeviceClass_Controller, m_pController);

	return VRInitError_None;
}

void ServerDriver::Cleanup()
{
	CleanupDriverLog();
	delete m_pNullHmdLatest;
	m_pNullHmdLatest = NULL;
	delete m_pController;
	m_pController = NULL;
}


void ServerDriver::RunFrame()
{
	if (m_pNullHmdLatest)
	{
		m_pNullHmdLatest->RunFrame();
	}
	if (m_pController)
	{
		m_pController->RunFrame();
	}

	vr::VREvent_t vrEvent;
	while (vr::VRServerDriverHost()->PollNextEvent(&vrEvent, sizeof(vrEvent)))
	{
		if (m_pController)
		{
			m_pController->ProcessEvent(vrEvent);
		}
	}
}
