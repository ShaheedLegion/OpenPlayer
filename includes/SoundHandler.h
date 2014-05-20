#ifndef SOUNDHANDLER_H_INCLUDED
#define SOUNDHANDLER_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include ".\..\bass\bass.h"


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <math.h>

#include "DSP.h"

#include "avi_utils.h"

#define AVI_REC 0

class SoundHandler
{
    public:
        HSTREAM m_stream;
        float m_spectrum[1024];
        HWND m_main;
        HBITMAP m_dib;
        HDC m_dc;
        int _bmw;
        int _bmh;
        void * m_bitmap;
        DSP * dsp;

#if (AVI_REC)
        HAVI m_avi;
#endif

    public:
    SoundHandler(HWND parent, int w, int h) : _bmw(w), _bmh(200)
    {
#if (AVI_REC)
        m_avi = NULL;
#endif
        dsp = new DSP(_bmw, _bmh);
        BASS_Init(-1,44100,0,parent,NULL);
        m_main = parent;

        BITMAPINFO bminfo;
        ZeroMemory(&bminfo, sizeof(BITMAPINFO));
        bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bminfo.bmiHeader.biWidth = _bmw;
        bminfo.bmiHeader.biHeight = _bmh;
        bminfo.bmiHeader.biPlanes = 1;
        bminfo.bmiHeader.biBitCount = 32;
        bminfo.bmiHeader.biCompression = BI_RGB;
        bminfo.bmiHeader.biSizeImage = (_bmw * _bmh * 4);

        HDC dc = GetDC(m_main);
        m_dc = CreateCompatibleDC(dc);
        m_dib = CreateDIBSection(dc, &bminfo, DIB_RGB_COLORS, (VOID **)&m_bitmap, NULL, 0);
        ReleaseDC(m_main, dc);

        SelectObject(m_dc, m_dib);
    }
    ~SoundHandler()
    {
        BASS_Free(); // close output
    }

    BOOL OpenFile(const char * filename, const char *filetitle)
    {
        StopSong();
        m_stream = BASS_StreamCreateFile(FALSE,filename,0,0,0);
        if (!m_stream)
        {
            MessageBox(NULL, filename, "Open failed!", MB_OK);
            return FALSE;
        }
        OpenFile(filetitle);
        BASS_ChannelPlay(m_stream,FALSE);
        return TRUE;
    }

    void StopSong()
    {
        CloseFile();
        try
        {
            BASS_ChannelStop(m_stream);
            BASS_StreamFree(m_stream);
            m_stream = 0;
        }
        catch (...){}
    }

    static void CALLBACK UpdateSpectrum(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
    {
        SoundHandler * handler = reinterpret_cast<SoundHandler *>(dwUser);
        BASS_ChannelGetData(handler->m_stream, handler->m_spectrum, BASS_DATA_FFT2048); // get the FFT data
        memset(handler->m_bitmap, 0, handler->_bmw * handler->_bmh * 4);
        //We will, unfortunately, paint the data right here as well onto the main dib section ...
        if (true)
        {
            int * bmdata = reinterpret_cast<int *>(handler->m_bitmap);
            handler->dsp->Process(handler->m_spectrum, bmdata, handler->_bmw, handler->_bmh);

            HDC dc = GetDC(handler->m_main);
            BitBlt(dc, 0, 0, handler->_bmw, handler->_bmh, handler->m_dc, 0, 0, SRCCOPY);
            ReleaseDC(handler->m_main, dc);

            handler->AddFrame();
        }
    }

    void SetVis(int idx)
    {
        dsp->SetVisIdx(idx);
    }

    void OpenFile(const char * ft)
    {
#if (AVI_REC)
        char buffer[256];
        memset(buffer, 0, 256);

        strcpy(buffer, ft);
        strcat(buffer, ".avi");
        m_avi = CreateAvi(buffer, 33, NULL);
#endif
    }
    void AddFrame()
    {
#if (AVI_REC)
        if (m_avi)
            AddAviFrame(m_avi, m_dib);
#endif
    }
    void CloseFile()
    {
#if (AVI_REC)
        try
        {
            CloseAvi(m_avi);
        }
        catch(...)
        {

        }
        m_avi = NULL;
#endif
    }
};
#endif // SOUNDHANDLER_H_INCLUDED
