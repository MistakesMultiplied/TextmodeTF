#pragma once

#include "Globals.h"
#include "../Utils/Feature/Feature.h"
#include "Definitions/Interfaces.h"
#include "../Utils/Signatures/Signatures.h"
#include "../Utils/Interfaces/Interfaces.h"
#include "../Utils/Hooks/Hooks.h"
#include "../Utils/Memory/Memory.h"
#include <intrin.h>
#include <format>
#ifndef TEXTMODE
#include <exception>
#endif

namespace SDK
{
	void Output(const char* cFunction, const char* cLog = nullptr, bool bDebug = true, int iMessageBox = -1);
	bool BlacklistFile(const char* cFileName);
	double PlatFloatTime();
}