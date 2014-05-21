#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501
#define WINVER 0x0501

#include <windows.h>
#include "includes\Application.h"
#include "includes\resource.h"

int w_w = 420;
int w_h = 340;
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);/*  Declare Windows procedure  */

int WINAPI WinMain (HINSTANCE hThisInstance,  HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    char szClassName[ ] = "FModCPPApp";
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);    /* Use default icon and mouse-pointer */
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(IDR_MYMENU);                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx (0, szClassName, "FModApp",
           WS_EX_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
           w_w, w_h, HWND_DESKTOP, NULL, hThisInstance, NULL );

    ShowWindow (hwnd, nCmdShow);
    fmapp::Initialize(hwnd, w_w, w_h);
    fmapp::CreateControls(hwnd);

/*
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) |WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0,0,0), (255 * 70)/100, LWA_ALPHA);    //set transparency for this window.
*/

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    fmapp::Destroy();
    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            break;
        case WM_COMMAND:
            fmapp::HandleCommand(hwnd, wParam, lParam);
            break;
        case WM_NOTIFY:
            fmapp::HandleNotify(hwnd, wParam, lParam);
            break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

