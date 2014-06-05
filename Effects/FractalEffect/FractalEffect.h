#ifndef FRACTALEFFECT_H_INCLUDED
#define FRACTALEFFECT_H_INCLUDED

#include "../Effects.h"

class __declspec(dllexport) FractalEffect: public Effect
{
    float zoom;
    float czoom;
    float moveX;
    float moveY; //you can change these to zoom and change position
    int maxIterations; //after how much iterations the function should stop
    float frameTime;
    float frameRunner;
    float runVal;

    public:
    FractalEffect(int w, int h) : Effect(w, h), zoom(1.0f), moveX(0.0f), moveY(0.0f), maxIterations(64)
    {
        frameTime = 0.0f;
        czoom = zoom;
        frameRunner = 0.011f;
        runVal = 0.0001f;
    }
    ~FractalEffect()
    {
    }
    int HSVtoRGB(float hue, float s, float v)
    {
        Color col;
        int i;
        float f, p, q, t;
        if ( s == 0 )
        {
            uchar vt = (uchar)v;
            col.set(vt, vt, vt);
            return col.getInt();
        }
        hue /= 60;			// sector 0 to 5
        i = floor( hue );
        f = hue - i;			// factorial part of h
        p = v * ( 1 - s );
        q = v * ( 1 - s * f );
        t = v * ( 1 - s * ( 1 - f ) );

        switch( i )
        {
            case 0:
                col.set((uchar)v, (uchar)t, (uchar)p);
                break;
            case 1:
                col.set((uchar)q, (uchar)v, (uchar)p);
                break;
            case 2:
                col.set((uchar)p, (uchar)v, (uchar)t);
                break;
            case 3:
                col.set((uchar)p, (uchar)q, (uchar)v);
                break;
            case 4:
                col.set((uchar)t, (uchar)p, (uchar)v);
                break;
            default:		// case 5:
                col.set((uchar)v, (uchar)p, (uchar)q);
                break;
        }
        return col.getInt();
    }
    void Process(float * spec, int * data, int color)
    {
        float cRe, cIm;                   //real and imaginary part of the constant c, determines shape of the Julia Set
        float newRe, newIm, oldRe, oldIm;   //real and imaginary parts of new and old

        cRe = -0.7;
        cIm = 0.27015;
        frameTime += 0.01f;

        if (frameTime > 2.0f)
            frameTime = 0.0f;
        if (czoom < zoom)
            czoom += (cIm / 2.0f);
        else if (czoom > zoom)
            czoom -= (cIm / 2.0f);

        for(int x = 0; x < w; x++)
        for(int y = 0; y < h; y++)
        {
            newRe = 1.5 * (x - w / 2) / (0.5 * czoom * w) + moveX;
            newIm = (y - h / 2) / (0.5 * czoom * h) + moveY;
            int i;

            for (i = 0; i < maxIterations; i++)
            {
                oldRe = newRe;
                oldIm = newIm;
                newRe = oldRe * oldRe - oldIm * oldIm + cRe;
                newIm = 2 * oldRe * oldIm + cIm;
                if((newRe * newRe + newIm * newIm) > 4) break;
            }
            data[x + (y * w)] = HSVtoRGB((float)(i % 256), 255.0f, 255.0f * (i < maxIterations));
        }

        float specAvg = 1;
        for (int i = 0; i < 20; i+=2)
            specAvg += (spec[i] + spec[i + 1]);
        float dRot = 1.5;
        if (specAvg < 2.5f)
            dRot = 1;

        if (zoom > 90.0f)
            zoom = 1.0f;

        zoom *= pow(dRot, frameTime / 4);
        moveX = (int)(czoom / 20.0f);
        frameRunner += runVal;
        if (frameRunner > 0.1f)
            runVal *= -1.0f;
        if (frameRunner < 0.1f)
            runVal *= -1.0f;

        Effect::Process(spec, data, color >> 8);
    }
};


#endif // FRACTALEFFECT_H_INCLUDED
