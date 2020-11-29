// ArduinoVRGloveDriver.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include "fileapi.h"
#include <iostream>
using namespace std;
int main() {
	HANDLE hComm;
	LPCWSTR comPort = L"COM4"; // Arduino USB port name.
	hComm = CreateFileW(comPort, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, NULL);
	DCB config = { 0 };
	config.DCBlength = sizeof(config);

	config.BaudRate = 38400;
	config.StopBits = ONESTOPBIT;
	config.Parity = PARITY_NONE;
	config.ByteSize = DATABITS_8;
	config.fDtrControl = 0;
	config.fRtsControl = 0;
	char buffer[5];
	DWORD read;

	if (!SetCommState(hComm, &config))
	{ cout << "Failed to Set Comm State Reason: " <<  GetLastError() << endl; }
	cout << "Current Settings\n Baud Rate: " << config.BaudRate << "\n Parity: " << config.Parity << "\n Byte Size: " 
		<< config.ByteSize << "\n Stop Bits: " << config.StopBits << endl;
}



