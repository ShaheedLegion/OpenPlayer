#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Mmsystem.h>
#include <Commdlg.h>
#include <Commctrl.h>

#include "SoundHandler.h"
#include "resource.h"

namespace fmapp
{
    void HandleVisChange(int vis);  //forward declare (c-style)
    typedef struct tagListData
    {
        char * name;
        int m_len;
        tagListData(const char *f) : name(0), m_len(0)
        {
            if (f)
            {
                m_len = strlen(f) + 1;
                name = new char[m_len];
                memset(name, 0, m_len);
                memcpy(name, f, m_len);
            }
        }
    }ListData;

    enum {OPEN = 5, EXIT, PLAYLIST};
    SoundHandler * g_sound = 0;
    char file_name[256];
    char file_title[256];
    DWORD timer;
    int wind_w;
    int wind_h;
    HWND _playlist;
    HMENU g_menu;

    BOOL GetFileName()
    {
        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.lpstrFile = file_name ;
        ofn.lpstrFile[0] = '\0';
        ofn.lpstrFileTitle = file_title;
        ofn.lpstrFileTitle[0] = '\0';
        ofn.nMaxFile = sizeof( file_name );
        ofn.nMaxFileTitle = sizeof(file_title);
        ofn.lpstrFilter = "MP3\0*.mp3\0Ogg\0*.ogg\0WMA\0*.wma\0Midi\0*.midi\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST ;
        return GetOpenFileName(&ofn);

    }
    void HandleFileOpen(const char * filename, const char * filetitle, bool additem)
    {
        if (timer) timeKillEvent(timer);
        timer = timeSetEvent(25, 25, (LPTIMECALLBACK)&g_sound->UpdateSpectrum, (DWORD_PTR)g_sound, TIME_PERIODIC);
        if (!g_sound->OpenFile(filename, filetitle))
                MessageBox(NULL, "Could not open song", "Error", MB_ICONERROR);
        else if (additem)
        {
            ListData * data = new ListData(filename);
            LVITEM lvI;
            lvI.pszText   = const_cast<CHAR*>(filetitle);
            lvI.mask      = LVIF_TEXT|LVIF_STATE|LVIF_PARAM;
            lvI.stateMask = LVIS_SELECTED;
            lvI.iSubItem  = 0;
            lvI.state     = 1;
            lvI.lParam    = reinterpret_cast<LPARAM>(data);

            if (ListView_InsertItem(_playlist, &lvI) == -1)
                MessageBox(NULL, "Could not add item to list view", "Error", MB_ICONERROR);
        }
    }
    void OpenFile()
    {
        if (GetFileName())
            HandleFileOpen(file_name, file_title, true);
    }
    void HandleCommand(HWND main, WPARAM wParam, LPARAM lParam)
    {
        int identifier = LOWORD(wParam);
        if (!g_sound)
            return;
        switch (identifier)
        {
            case ID_FILE_OPEN:
                OpenFile();
            break;
            case ID_FILE_EXIT:
                PostMessage(main, WM_DESTROY, 0, 0);
            break;
            case ID_STUFF_PP:
            break;
            case ID_STUFF_STOP:
                g_sound->StopSong();
            break;

            default:
                HandleVisChange(identifier);
                break;
        }
    }
    void HandleNotify(HWND hwnd, WPARAM wParam, LPARAM lParam)
    {
        LPNMHDR nmhdr = (LPNMHDR)lParam;
        if (nmhdr->code == (unsigned)NM_DBLCLK)
        {
            //the user double clicked on a list item...
            int ItemIndex = SendMessage(_playlist, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
            LVITEM SelectedItem;
            SelectedItem.iItem = ItemIndex;
            SelectedItem.mask = LVIF_PARAM;
            ListView_GetItem(_playlist, &SelectedItem);

            ListData * data = reinterpret_cast<ListData*>(SelectedItem.lParam);
            HandleFileOpen(data->name, 0, false);
        }
    }

    HWND CreateListView(HWND hwnd, LPCSTR text, int x, int y, int command)
    {
        HWND hwndList = CreateWindow(WC_LISTVIEW, text, WS_TABSTOP | WS_VISIBLE | WS_CHILD | LVS_REPORT |LVS_SHOWSELALWAYS,
        x, y, wind_w, 92, hwnd, (HMENU)command, (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);

        LVCOLUMN lvc;
        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
        lvc.pszText = const_cast<CHAR*>(text);
        lvc.cchTextMax = strlen(text) + 1;
        lvc.fmt = LVCFMT_LEFT;
        lvc.cx = wind_w;
        ListView_InsertColumn(hwndList, 0, &lvc);

        return hwndList;
    }

    void CreateControls(HWND hwnd)
    {
        _playlist = CreateListView(hwnd, "Playlist", 0, 200, PLAYLIST);

        //set up default check state for the visualizations ...
        g_menu = GetMenu(hwnd);
        HandleVisChange(0); //make sure the default is set to 0.
    }

    void HandleVisChange(int vis)
    {
        HMENU visMenu = GetSubMenu(g_menu, 2);
        if (visMenu == NULL)
            return;

        int nItems = GetMenuItemCount(visMenu);
        if (nItems == -1)   //should not happen, but just in case
            return;

        int itemId;
        for (int i = 0; i < nItems; ++i)
        {
            itemId = GetMenuItemID(visMenu, i);
            if (itemId != -1)
                CheckMenuItem(visMenu, itemId, MF_BYCOMMAND | MF_UNCHECKED);
        }

        CheckMenuItem(visMenu, vis, MF_BYCOMMAND | MF_CHECKED);

        MENUITEMINFO itemInfo;
        ZeroMemory(&itemInfo, sizeof(MENUITEMINFO));
        itemInfo.cbSize = sizeof(MENUITEMINFO);
        itemInfo.fMask = MIIM_STRING;
        itemInfo.fType = MFT_STRING;

        TCHAR buffer[MAX_PATH];
        itemInfo.cch = MAX_PATH;
        itemInfo.dwTypeData = buffer;

        if (GetMenuItemInfo(visMenu, vis, FALSE, &itemInfo))
        {
            std::string name(itemInfo.dwTypeData, itemInfo.cch);
            g_sound->SetVis(name);
        }
    }
    void Initialize(HWND hwnd, int w, int h)
    {
        wind_w = w;
        wind_h = h;
        std::vector<std::string> names;
        g_sound = new SoundHandler(hwnd, wind_w, wind_h, names);

        HMENU hMenu = GetMenu(hwnd);    //get the menu bar handle
        if (hMenu == NULL)
        {
            MessageBox(hwnd, "Error!", "Could not get the menu handle.", MB_OK);
            return;
        }

        HMENU visMenu = GetSubMenu(hMenu, 2);
        if (visMenu == NULL)
        {
            MessageBox(hwnd, "Error!", "Could not get the SubMenu handle.", MB_OK);
            return;
        }

        for (unsigned int i = 0; i < names.size(); ++i)
            AppendMenu(visMenu, MF_STRING, (WM_USER + i), names[i].c_str());
    }
    void Destroy(){ delete fmapp::g_sound; }
};

#endif // APPLICATION_H_INCLUDED
