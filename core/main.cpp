#include "../dependencies/common_includes.hpp"
#include "features/misc/misc.hpp"
#include "menu/config/config.hpp"

unsigned long __stdcall initial_thread(void* reserved) {

#ifdef debug_build
	AllocConsole();
	SetConsoleTitleA("Counter-Strike: Global Offensive");
	freopen_s((FILE * *)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE * *)stdout, "CONOUT$", "w", stdout);
#endif

	interfaces::initialize();
	hooks::initialize();
	utilities::material_setup();
	config_system.run("aristois");

	while (!GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	hooks::shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

#ifdef debug_build
	fclose((FILE*)stdin);
	fclose((FILE*)stdout);
	FreeConsole();
#endif

	FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(reserved), 0);
	return 0ul;
}

bool __stdcall DllMain(void* instance, unsigned long reason_to_call, void* reserved) {
	if (reason_to_call == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0, initial_thread, instance, 0, 0);
	}

	return true;
}