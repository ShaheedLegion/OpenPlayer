#ifndef EFFECTS_H_INCLUDED
#define EFFECTS_H_INCLUDED

#include <string>
#include <memory>
#include <cstring>
#include <math.h>

#ifdef BUILD_DLL
    #define DLLAPI __declspec(dllexport)
#else
    #define DLLAPI
#endif // BUILD_DLL


typedef unsigned char uchar;

class __declspec(dllexport) Effect
{
    public:
    typedef struct tagStar
    {
        float x;
        float y;
        float z;
        int tx;
        int ty;
        float dx;
        float dy;
        float angle;
    } Star;
    typedef struct tagEmitter
    {
        public:
        float x;
        float y;
        float z;
        float dx;
        float dy;
        int tx;
        int ty;
        float life;
        Star * particles;
        int numstars;
        int state;

    }Emitter;
    int cdeltas[3];
    typedef struct tagEmptyColor
    {
    public:
        uchar u;
        uchar r;
        uchar g;
        uchar b;

    } EmptyColor;
    typedef struct tagColor
    {
        public:
        uchar b;
        uchar g;
        uchar r;
        uchar u;

        tagColor(uchar rd, uchar gr, uchar bl) : b(bl), g(gr), r(rd), u(0)
        {
        }
        tagColor() : b(0), g(0), r(0), u(0)
        {
        }
        void set(uchar _r, uchar _g, uchar _b)
        {
            r = _r;
            g = _g;
            b = _b;
        }
        uchar add(uchar val)
        {
            uchar ret = val + 1;
            if (ret > 254)
                ret = 255;
            return ret;
        }
        uchar subtract(uchar val)
        {
            uchar ret = val - 1;
            if (ret < 1)
                ret = 0;
            return ret;
        }
        uchar run(uchar val, int * cdeltas, int cidx, int min, int max)
        {
            uchar ret;
            int dt = cdeltas[cidx];

            if (dt == 0)
                return val;

            if (dt > 0)
                ret = add(val);
            else
                ret = subtract(val);

            if (ret == max)
                cdeltas[cidx] = -1;
            else if (ret == min)
                cdeltas[cidx] = 1;
            return ret;
        }
        void runcolor(int * deltas, int min, int max)
        {
            r = run(r, deltas, 0, min, max);
            g = run(g, deltas, 1, min, max);
            b = run(b, deltas, 2, min, max);
        }
        uchar averageChannel(uchar t, uchar b, uchar l, uchar r)
        {
            int re = (t + b + l + r) >> 2;
            return (uchar)re;
        }
        void avg4(tagColor * tp, tagColor * bm, tagColor * lt, tagColor * rt)
        {
            r = averageChannel(tp->r, bm->r, lt->r, rt->r);
            g = averageChannel(tp->g, bm->g, lt->g, rt->g);
            b = averageChannel(tp->b, bm->b, lt->b, rt->b);
        }
        int getInt()
        {
            int intrep = ((int)u << 24 | (int)r << 16 | (int)g << 8 | (int)b);
            return intrep;
        }
        void setInt(int col)
        {
            u = (char)((int)(col & 0xff000000) >> 24);
            r = (char)((int)(col & 0x00ff0000) >> 16);
            g = (char)((int)(col & 0x0000ff00) >> 8);
            b = (char)((int)(col & 0x000000ff));
        }
    } Color;
    int w;
    int h;
    float m_degrad;

    public:
    Effect(int ew, int eh) : w(ew), h(eh)
    {
        m_degrad = (float)((22.0f / 7.0f) / 180.0);
        memset(cdeltas, 0, 3 * sizeof(int));
    }
    virtual ~Effect(){}

    public:

    virtual void Process(float * spec, int * data, int color)
    {
        int y, y1 = 0;
        for (int x = 0; x < w; ++x)
        {
            y = sqrt(spec[x]) * 3 * h-4; // scale it (sqrt to make low values more visible)

            if (y > h) y = h - 1; // cap it
            if (x && (y1 = (y + y1) / 2)) // interpolate from previous to make the display smoother
            {
                if (y1 > h)
                    y1 = h - 1;   //cap it again.
                while (--y1>=0) data[y1 * w + x] = color << 16;
            }
            y1 = y;
            if (y > h)
                y = h - 1;
            while (--y>=0) data[y * w + x] = color + y; // draw level
        }
    }

    void TransformPoint(Star & s, int w, int h)
    {
        s.tx = (s.x / s.z) * w + (w >> 1);
        s.ty = (s.y / s.z) * h + (h >> 1);
    }
    void TransformPoint(Emitter & e, int w, int h)
    {
        e.tx = (e.x / e.z) * w + (w >> 1);
        e.ty = (e.y / e.z) * h + (h >> 1);
    }
};

/*
// Get an instance of the derived class
// contained in the DLL.
DLLAPI std::auto_ptr<Effect> getObj(int w, int h);

// Get the name of the plugin. This can
// be used in various associated messages.
DLLAPI std::string getName(void);
*/
#endif // EFFECTS_H_INCLUDED
