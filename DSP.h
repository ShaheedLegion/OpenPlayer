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
        g_num_fills = 3;
        g_fills = new int[g_num_fills];
        g_effects = new Effect*[g_num_fills];
        g_fills[0] = 0xffff3226;
        g_fills[1] = 0xff3226ff;
        g_fills[2] = 0xff26ff32;

        g_effects[0] = new MilkyEffect(w, h);
        g_effects[1] = new UniverseEffect(w, h);
        g_effects[2] = new StarGateEffect(w, h);
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
        int y, y1 = 0;
        int color = g_fills[g_fill_idx];

        for (int x = 0;x < bw; x++)
        {
            y = sqrt(spectrum[x]) * 3 * bh-4; // scale it (sqrt to make low values more visible)

            if (y > bh) y = bh - 1; // cap it
            if (x && (y1 = (y + y1) / 2)) // interpolate from previous to make the display smoother
            {
                if (y1 > bh)
                    y1 = bh - 1;   //cap it again.
                while (--y1>=0) data[y1 * bw + x] = color << 16;
            }
            y1 = y;
            if (y > bh)
                y = bh - 1;
            while (--y>=0) data[y * bw + x] = color + y; // draw level
        }

        g_effects[g_fill_idx]->Process(spectrum, data);
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
