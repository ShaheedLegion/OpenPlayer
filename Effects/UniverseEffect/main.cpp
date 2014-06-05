#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "UniverseEffect.h"
extern "C"
{
    __declspec(dllexport) Effect * getObj(int w, int h);
    __declspec(dllexport) std::string getName(void);
}

Effect * getObj(int w, int h) {
    return new UniverseEffect(w, h);
}

std::string getName(void) {
    return "Universe";
}

extern "C" DLLAPI BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
