#ifndef DSP_H_INCLUDED
#define DSP_H_INCLUDED

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include "../Effects/Effects.h"

class DSP
{
WIN32_FIND_DATA fileData;

public:
    int g_fill_idx;
    std::vector<std::pair<Effect *, std::string> > g_effects;
    std::vector<HINSTANCE> modules;
    public:
    DSP(int w, int h, std::vector<std::string> & names)
    {
        g_fill_idx = -1;
        TCHAR lpFileName[MAX_PATH]; memset(lpFileName, 0, MAX_PATH * sizeof(TCHAR));
        TCHAR lpTemp[MAX_PATH]; memset(lpTemp, 0, MAX_PATH * sizeof(TCHAR));
        GetModuleFileName(GetModuleHandle(NULL), lpFileName, MAX_PATH);
        for (int i = MAX_PATH - 1; i > 0; --i)
        {
            if (lpFileName[i] == '\\')
                break;
            lpFileName[i] = '\0';
        }
        sprintf(lpTemp, "%s%s", lpFileName, "Effects\\*.dll");
        HANDLE fileHandle = FindFirstFile(lpTemp, &fileData);
        //DWORD err = GetLastError();
        if (fileHandle != (void*)-1)
        {
            do
            {
                sprintf(lpTemp, "%sEffects\\%s", lpFileName, fileData.cFileName);
                HINSTANCE temp = LoadLibrary(lpTemp);
                if (temp)
                    modules.push_back(temp);
            } while (FindNextFile(fileHandle, &fileData));
        }

        typedef Effect * (__cdecl *ObjProc)(int, int);
        typedef std::string (__cdecl *NameProc)(void);
        std::string cname;
        std::vector<HMODULE>::iterator iter = modules.begin(), ed = modules.end();
        for (; iter != ed; ++iter)
        {
            ObjProc objFunc = (ObjProc)GetProcAddress(*iter, "getObj");
            NameProc nameFunc = (NameProc)GetProcAddress(*iter, "getName");
            Effect * obj = objFunc(w, h);
            cname = nameFunc();
            g_effects.push_back(std::make_pair(obj, cname));
            names.push_back(cname);
        }
    }
    ~DSP()
    {
        std::vector<HMODULE>::iterator iter = modules.begin(), ed = modules.end();
        for (; iter != ed; ++iter)
            FreeLibrary(*iter);
    }

    void Process(float * spectrum, int * data, int bw, int bh)
    {
        if (g_fill_idx != -1)
            g_effects[g_fill_idx].first->Process(spectrum, data, 0);
    }

    void SetVisIdx(std::string & name)
    {
        for (unsigned int i = 0; i < g_effects.size(); ++i)
        {
            if (g_effects[i].second.compare(name) == 0)
            {
                g_fill_idx = i;
                break;
            }
        }
    }
};

#endif // DSP_H_INCLUDED
