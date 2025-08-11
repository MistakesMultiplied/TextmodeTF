#include "../SDK/SDK.h"
#include <unordered_set>
#include <string>
#include "../Core/Core.h"

static bool ShouldBlockEntity(const char* cls)
{
	// Block visual/physics props that are not needed in textmode.
	if (!_stricmp(cls, "prop_dynamic") || !_stricmp(cls, "prop_dynamic_override") ||
		!_stricmp(cls, "prop_dynamic_glow") || !_stricmp(cls, "prop_physics") ||
		!_stricmp(cls, "prop_physics_multiplayer") || !_stricmp(cls, "prop_physics_override") ||
		!_stricmp(cls, "prop_ragdoll") || !_stricmp(cls, "func_breakable") ||
		!_stricmp(cls, "func_breakable_surf") || strstr(cls, "gib"))
		return true;

	return false;
}

// Hook for the client's entity factory.
MAKE_HOOK(Client_CreateEntityByName, G::Client_CreateEntityByNameAddr, void*,
		  void* rcx, const char* pszName)
{
	static std::unordered_set<std::string> s_logged;
	if (pszName && ShouldBlockEntity(pszName))
	{
		// Log once per classname to avoid spam
		if (s_logged.emplace(pszName).second)
			SDK::Output("Client_CreateEntityByName", std::format("Blocked client entity: {}", pszName).c_str());

		return nullptr; // Prevent spawn
	}
	return CALL_ORIGINAL(rcx, pszName);
}
