/*****************************************************************************************************************************
* Copyright (c) 2022-2023 POLE
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
******************************************************************************************************************************/

#pragma once
#include <cmath>
#include <atomic>
#include "iospecs.hpp"
#include "containers.hpp"
#include "constants.hpp"

namespace cell {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// One pole LP parameter smooth filter ///////////////////////////////////////////////////////////////////////////
struct onepole 
{
    float a;
    float b;
    float z;

    void reset(float time) noexcept
    {
        a = expf(- tao / (time * 0.001f * settings::sample_rate));
        b = 1.0f - a;
        z = 0.0f;
    }
    constexpr float process(float in) noexcept
    {
        z = (in * b) + (z * a);
        return z;
    }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Allpass filter ////////////////////////////////////////////////////////////////////////////////////////////////
struct allpass
{
    float y = 0.0f;
    float a = 0.0f;
    constexpr float process(const float in) noexcept
    {
        float out = y + a * in;
        y = in - a * out;
        return out;
    }    
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Envelope Follower /////////////////////////////////////////////////////////////////////////////////////////////
struct envelope_follower
{
    float a;
    float r;
    float envelope = 0.0f;

    inline void init(float, float, float);
    inline void process(const float& in);
};

inline void envelope_follower::init( float aMs, float rMs, float sample_rate )
{
    a = pow( 0.01, 1.0 / ( aMs * sample_rate * 0.001 ) );
    r = pow( 0.01, 1.0 / ( rMs * sample_rate * 0.001 ) );
}

inline void envelope_follower::process(const float& in)
{
    float f = fabs(in);
    if (f > envelope) envelope = a * ( envelope - f ) + f;
    else              envelope = r * ( envelope - f ) + f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Limiter ///////////////////////////////////////////////////////////////////////////////////////////////////////
struct limiter_t
{
    envelope_follower e;
    float threshold = 0.01f;
    inline void  init(float, float , float);
    inline float process(const float& in);
    inline limiter_t();
};

inline limiter_t::limiter_t()
{
    init(1.0f, 10.0f, 48000.0f);
}

inline void limiter_t::init(float aMs, float rMs, float sample_rate)
{
    e.init(aMs, rMs, sample_rate);
}


inline float limiter_t::process(const float& in)
{
    float out = in;
    e.process(out);
    if(e.envelope>threshold)
    {
        out /= expf(e.envelope - threshold);
    }
    return out;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Product ///////////////////////////////////////////////////////////////////////////////////////////////////////
struct product
{
    std::atomic<float>*  in[3];
    std::atomic<float>  out[1];

    inline void process()
    {
        bool  o = false;
        float s = 1.0f;
        for(int i = 0; i < 3; ++i)
        {
            if(in[i] == &zero) continue;
            else 
            {
                s *= in[i]->load();
                o = true;
            }
        }
        out[0].store( o ? s : 0.0f);
    };

    inline void reset()
    {
        for(int i = 0; i < 3; ++i) in[i] = &zero;
        for(int i = 0; i < 1; ++i) out[i].store(0.0f);
    }

    product() { reset(); };
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Crossfader: f == 1? a = max; f==0? b = max ////////////////////////////////////////////////////////////////////
constexpr float xfade(const float& a, const float& b, const float& f) noexcept
{
    return a * f + b * (1.0f - f);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 3to2 //////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
constexpr l_r<T> c3c2(const lcr<T>& in, const float& a, const float& b) noexcept
{
    auto ac = a > 0.5f ? (1.0f - a) * 2.0f : a * 2.0f;
    auto bc = b > 0.5f ? (1.0f - b) * 2.0f : b * 2.0f;
    return l_r<T> { xfade(in.l, in.c, ac), xfade(in.c, in.r, bc) };
}

struct mixer
{
    std::atomic<float>* in[3];
    std::atomic<float>* angle[2];
    std::atomic<float>* cv[2];
    float  out[2];
    inline void process()
    {
        lcr<float> a { in[0]->load(), in[1]->load(), in[2]->load() };
        float lc = angle[0]->load() + cv[0]->load();
        float cr = angle[1]->load() + cv[1]->load();
        l_r<float> lr = c3c2(a, lc, cr);
        out[0] = lr.l;
        out[1] = lr.r;
    }

    inline void reset()
    {
        for(int i = 0; i < 3; ++i)
        {
            in[i] = &zero;
        }
        for(int i = 0; i < 2; ++i)
        {
            angle[i] = &zero;
            cv[i] = &zero;
            out[i] = 0.0f;
        }
    }

    mixer() { reset(); };
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Departed samples to milliseconds //////////////////////////////////////////////////////////////////////////////
constexpr unsigned DStoMS(unsigned samples, unsigned sample_rate) noexcept
{
    return samples * 1000 / sample_rate;
}

constexpr unsigned MStoDS(unsigned ms, unsigned sample_rate) noexcept
{
    return ms * sample_rate / 1000;
}


constexpr bool bool_from_range(float value, float min = 0.0f, float max = 1.0f) noexcept
{
    return value > ((max - min) * 0.5f);
}

inline int rand_in_range(int l, int r) noexcept
{ 
    return (rand() % (r - l + 1)) + l;
}

};

constexpr bool sign(float data) noexcept
{
    return data > 0.0f ? true : false;
}

constexpr bool sign(int data) noexcept
{
    return data > 0 ? true : false;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DC Block filter ///////////////////////////////////////////////////////////////////////////////////////////////
struct dcb_t
{
    float f = 0.9995;
    float eax = 0.0f, ebx = 0.0f;
    constexpr float process(const float& in) noexcept
    {
        ebx = in - eax + f * ebx;
        eax = in;
        return ebx;
    }
};
