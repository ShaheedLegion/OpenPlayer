#ifndef DSP_H_INCLUDED
#define DSP_H_INCLUDED

#include <iostream>
using namespace std;

class DSP;

namespace ns_dsp
{
    DSP * g_dsp;
}

class DSP
{
  public:
    short * m_data;
    short * m_buffer;
    int data_len;
    DSP() : m_data(0), m_buffer(0), data_len(50)
    {
        ns_dsp::g_dsp = this;
        m_data = new short[data_len];
        m_buffer = new short[data_len];
    }
    ~DSP(){}

    static void ProcessAudio(int chan, void *stream, int len, void *udata)
    {
        int stepsize = (len / ns_dsp::g_dsp->data_len);
        int idx = 0;
        for (int i = 0; i < ns_dsp::g_dsp->data_len; i++)
        {
            ns_dsp::g_dsp->m_data[i] = ((short *)stream)[idx];
            idx += stepsize;
        }
        memcpy(ns_dsp::g_dsp->m_buffer, ns_dsp::g_dsp->m_data, ns_dsp::g_dsp->data_len);
    }
};
#endif // DSP_H_INCLUDED
