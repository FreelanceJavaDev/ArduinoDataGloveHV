#include "ArduinoDriverClasses.h"
ArduinoControllerDriver::ArduinoControllerDriver()
{
	m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
	m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;

	m_sSerialNumber = "CTRL_TEST";

	m_sModelNumber = "ArduinoGlove";
}

EVRInitError ArduinoControllerDriver::Activate(vr::TrackedDeviceIndex_t unObjectId)
{
	m_unObjectId = unObjectId;
	m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, m_sModelNumber.c_str());
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, m_sModelNumber.c_str());

	// return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
	vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2);

	// avoid "not fullscreen" warnings from vrmonitor
	vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_IsOnDesktop_Bool, false);

	// our sample device isn't actually tracked, so set this property to avoid having the icon blink in the status window
	vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_NeverTracked_Bool, true);

	// even though we won't ever track we want to pretend to be the right hand so binding will work as expected
	vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, TrackedControllerRole_RightHand);

	// this file tells the UI what to show the user for binding this controller as well as what default bindings should
	// be for legacy or other apps
	vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_InputProfilePath_String, "ArduinoControllerDriver/input/Commtest_profile.json");

	// create Get test button
	vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/a/click", &m_compA);

	// create haptic component
	//vr::VRDriverInput()->CreateHapticComponent( m_ulPropertyContainer, "/output/haptic", &m_compHaptic );

	return VRInitError_None;
}

void *ArduinoControllerDriver::GetComponent(const char *pchComponentNameAndVersion)
{
	// override this to add a component to a driver
	return NULL;
}

void ArduinoControllerDriver::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1)
		pchResponseBuffer[0] = 0;
}

DriverPose_t ArduinoControllerDriver::GetPose()
{
	DriverPose_t pose = { 0 };
	pose.poseIsValid = false;
	pose.result = TrackingResult_Calibrating_OutOfRange;
	pose.deviceIsConnected = true;

	pose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
	pose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);

	return pose;
}

void ArduinoControllerDriver::RunFrame()
{
#if defined( _WINDOWS )
	// Your driver would read whatever hardware state is associated with its input components and pass that
	// in to UpdateBooleanComponent. This could happen in RunFrame or on a thread of your own that's reading USB
	// state. There's no need to update input state unless it changes, but it doesn't do any harm to do so.

	vr::VRDriverInput()->UpdateBooleanComponent(m_compA, (0x8000 & GetAsyncKeyState('A')) != 0, 0);
#endif
}


void ArduinoControllerDriver::ProcessEvent(const vr::VREvent_t & vrEvent)
{
	//special events such as piezos & haptic feedback
}
