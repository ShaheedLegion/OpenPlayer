#ifndef DSP_H_INCLUDED
#define DSP_H_INCLUDED

#include "Effects.h"

class DSP
{
public:
    int * g_fills;
    int g_num_fills;
    int g_fill_idx;
    Effect ** g_effects;
    public:
    DSP(int w, int h)
    {
        g_fill_idx = 0;
        g_num_fills = 5;
        g_fills = new int[g_num_fills];
        g_effects = new Effect*[g_num_fills];
        g_fills[0] = 0xffff3226;
        g_fills[1] = 0xff3226ff;
        g_fills[2] = 0xff26ff32;
        g_fills[3] = 0xff26f26f;
        g_fills[4] = 0xffff3264;

        g_effects[0] = new MilkyEffect(w, h);
        g_effects[1] = new UniverseEffect(w, h);
        g_effects[2] = new StarGateEffect(w, h);
        g_effects[3] = new FractalEffect(w, h);
        g_effects[4] = new FlameEffect(w, h);
    }
    ~DSP()
    {
        delete [] g_fills;
        for (int i = 0; i < g_num_fills; ++i)
            delete g_effects[i];
        delete [] g_effects;
    }

    void Process(float * spectrum, int * data, int bw, int bh)
    {
        int color = g_fills[g_fill_idx];
        g_effects[g_fill_idx]->Process(spectrum, data, color);
  }

  void SetVisIdx(int idx)
  {
      if (idx >= 0 && idx < g_num_fills)
      {
          g_fill_idx = idx;
      }
  }
};

#endif // DSP_H_INCLUDED
