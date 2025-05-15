#pragma once
#include "../Utils/Feature/Feature.h"

#include <sstream>
#include <fstream>

class CCore
{
public:
	void Load();
	void Loop();
	void Unload();

	void AppendFailText(const char* sMessage, bool bCritical = false);

	bool m_bUnload = false;
	bool m_bTimeout = false;
private:
	bool m_bFilesystemLoaded = false, m_bEngineLoaded = false, m_bMatSysLoaded = false, m_bClientLoaded = false;

	int LoadFilesystem();
	int LoadEngine();
	int LoadMatSys();
	int LoadClient();

	std::stringstream ssFailStream;
};

ADD_FEATURE_CUSTOM(CCore, Core, U);