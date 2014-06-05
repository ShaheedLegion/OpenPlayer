#ifndef STARGATEEFFECT_H_INCLUDED
#define STARGATEEFFECT_H_INCLUDED

#include "../Effects.h"

class __declspec(dllexport) StarGateEffect : public Effect
{
    int * distanceTable;
    int * angleTable;
    int * m_bitmap;
    float m_animation;
    public:
    StarGateEffect(int w, int h) : Effect(w, h), m_animation(0.0f)
    {
        int len = w * h;
        m_bitmap = new int[len];
        memset(m_bitmap, 0, len * sizeof(int));

        distanceTable = new int[len];
        angleTable = new int[len];

        int angle, distance;
        float ratio = 32.0;
        for(int x = 0; x < w; x++)
        for(int y = 0; y < h; y++)
        {
            distance = (int)(ratio * h / sqrt((x - w / 2.0) * (x - w / 2.0) + (y - h / 2.0) * (y - h / 2.0))) % h;
            angle = (unsigned int)(0.5 * w * atan2(y - h / 2.0, x - w / 2.0) / 3.1416);
            distanceTable[x + (y * w)] = distance;
            angleTable[x + (y * w)] = angle;
        }
    }
    ~StarGateEffect()
    {
        delete [] m_bitmap;
        delete [] distanceTable;
        delete [] angleTable;
    }
    void memblur(int * dst, int len)
    {
        EmptyColor * tp;
        EmptyColor * lt;
        EmptyColor * bm;
        EmptyColor * rt;

        Color col;

        int ypos, idx;
        for (int y = 1; y < h-1; ++y)
        {
            ypos = (y * w);
            for (int x = 1; x < w-1; ++x)
            {
                idx = ypos + x;

                lt = reinterpret_cast<EmptyColor *>(&dst[idx - 1]);
                rt = reinterpret_cast<EmptyColor *>(&dst[idx + 1]);
                tp = reinterpret_cast<EmptyColor *>(&dst[idx - w]);
                bm = reinterpret_cast<EmptyColor *>(&dst[idx + w]);

                col.r = col.averageChannel(tp->r, bm->r, lt->r, rt->r);
                col.g = col.averageChannel(tp->g, bm->g, lt->g, rt->g);
                col.b = col.averageChannel(tp->b, bm->b, lt->b, rt->b);

                dst[idx - w] = col.getInt();
            }
        }
    }
    void Process(float * spec, int * data, int color)
    {
        Effect::Process(spec, data, color);
        int len = w * h;
        m_animation += 0.01f;

        int shiftX = (int)(w * 1.0 * m_animation);
        int shiftY = (int)(w * 0.25 * m_animation);

        int idx = 0;
        int col;

        unsigned int lkx;
        unsigned int lky;
        for(int x = 0; x < w; x++)
        for(int y = 0; y < h; y++)
        {
            idx = x + (y * w);
            //get the texel from the texture by using the tables, shifted with the animation values
            lkx = (unsigned int)(distanceTable[idx] + shiftX) % w;
            lky = (unsigned int)(angleTable[idx] + shiftY) % h;
            col = data[lkx + (lky * w)];
            m_bitmap[idx] = col;
        }
        memblur(m_bitmap, len);
        memcpy(data, m_bitmap, len * sizeof(int));
    }
};

#endif // STARGATEEFFECT_H_INCLUDED
