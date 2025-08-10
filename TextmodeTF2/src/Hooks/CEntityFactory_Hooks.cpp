#include "../SDK/SDK.h"
#include <unordered_set>
#include <string>
#include <cstring>
#include <Windows.h>
#include "../Core/Core.h"

// Validate that `p` points to a readable, NUL-terminated string of length < maxLen.
// If valid, copies into `out` and returns true.
static bool GetSafeString(const char* p, std::string& out, size_t maxLen = 128)
{
    out.clear();
    if (!p) return false;

    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(p, &mbi, sizeof(mbi))) return false;
    if (mbi.State != MEM_COMMIT) return false;
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;

    const DWORD prot = mbi.Protect & 0xFF;
    const bool readable =
        prot == PAGE_READONLY || prot == PAGE_READWRITE || prot == PAGE_WRITECOPY ||
        prot == PAGE_EXECUTE_READ || prot == PAGE_EXECUTE_READWRITE || prot == PAGE_EXECUTE_WRITECOPY;
    if (!readable) return false;

    // Clamp probe length to remaining bytes in this committed region to avoid crossing page boundaries
    const auto base   = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
    const auto start  = reinterpret_cast<uintptr_t>(p);
    const auto end    = base + static_cast<uintptr_t>(mbi.RegionSize);
    size_t regionAvail = 0;
    if (start >= base && start < end)
    {
        regionAvail = static_cast<size_t>(end - start);
    }
    if (regionAvail == 0) return false;
    const size_t limit = (maxLen < regionAvail) ? maxLen : regionAvail;

    size_t len = strnlen_s(p, limit);
    if (len == 0 || len >= limit) return false; // empty or unterminated within this region

    out.assign(p, len);
    return true;
}

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
	std::string safeName;
	if (!GetSafeString(pszName, safeName))
	{
		return CALL_ORIGINAL(rcx, pszName);
	}

	if (ShouldBlockEntity(safeName.c_str()))
	{
		if (s_logged.emplace(safeName).second)
			SDK::Output("Client_CreateEntityByName", std::format("Blocked client entity: {}", safeName).c_str());

		return nullptr; // Prevent spawn
	}
	return CALL_ORIGINAL(rcx, pszName);
}
