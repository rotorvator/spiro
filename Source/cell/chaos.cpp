// ISC License
//
// Copyright (c) 2023 POLE
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "chaos.hpp"

namespace cell {

void map_t::process()
{
    int f = (int)ctrl[static_cast<int>(interface::map::ctrl::form)]->load();

    if(_reset)
    {
        (this->*reset[0])();
        _reset = false;
    }
    (this->*form[0])();
}

void map_t::switch_wave(const int& w)
{
    _reset = true;
}

map_t::map_t()
{
    init(interface::map::ctrls, interface::map::ins, interface::map::outs);
}

void map_t::sprott_reset()
{
    f[0] = 0.8f;     // a
    f[1] = 0.5f;     // b
    f[2] = 0.1f;     // c
    f[3] = 1.0f;     // d
    f[4] = 0.01f;    // t

    f[5] = 0.1f;     // x
    f[6] = 0.1f;     // y
    f[7] = 0.1f;     // z
}

void map_t::sprott() 
{
    f[4] = (ctrl[static_cast<int>(interface::map::ctrl::tune)]->load() + ctrl[static_cast<int>(interface::map::in::fm)]->load() * 0.1f ) * 0.2f + 0.0001f;
    if(ctrl[static_cast<int>(interface::map::in::warp)] == &zero) f[2] = 0.1f + ctrl[static_cast<int>(interface::map::ctrl::warp)]->load();
    else f[2] = 0.1f + ctrl[static_cast<int>(interface::map::ctrl::warp)]->load() * ctrl[static_cast<int>(interface::map::in::warp)]->load();

    f[5] += f[4] * f[6] * f[0];
    f[6] += f[4] * (- f[6] * f[7] - f[5]);
    f[7] += f[4] * (f[1] * f[6] * f[6] - f[2] * f[5] - f[3]);

    if((f[5] > std::numeric_limits<float>::max()) && (f[6] > std::numeric_limits<float>::max()) && (f[7] > std::numeric_limits<float>::max())) 
    {
        f[5] = 0.1f; 
        f[6] = 0.1f;
        f[7] = 0.1f;
    }
    else
    {
        out[static_cast<int>(interface::map::out::x)].store(f[5] * ctrl[static_cast<int>(interface::map::ctrl::amp)]->load() * 0.4f);
        out[static_cast<int>(interface::map::out::y)].store(f[6] * ctrl[static_cast<int>(interface::map::ctrl::amp)]->load() * 0.4f);
        out[static_cast<int>(interface::map::out::z)].store(f[7] * ctrl[static_cast<int>(interface::map::ctrl::amp)]->load() * 0.4f);
    }
}

void map_t::helmholz_reset()
{
    f[5] = 0.1f;     // x
    f[6] = 0.1f;     // y
    f[7] = 0.1f;     // z

    f[0] = 5.11f;    // Gamma
    f[1] = 0.55f;    // Delta
    f[2] = 0.01;     // t
}

void map_t::helmholz() 
{
    f[2] = (ctrl[static_cast<int>(interface::map::ctrl::tune)]->load() + ctrl[static_cast<int>(interface::map::in::fm)]->load() * 0.1f ) * 0.2f + 0.01f;
    if(ctrl[static_cast<int>(interface::map::in::warp)] == &zero) f[1] = ((ctrl[static_cast<int>(interface::map::ctrl::warp)]->load() - 0.5f) * 0.03f) + 0.55f;
    else f[1] = ((ctrl[static_cast<int>(interface::map::ctrl::warp)]->load() * fabsf(ctrl[static_cast<int>(interface::map::in::warp)]->load()) - 0.5f) * 0.03f) + 0.55f;

        f[5] += f[2] * f[6];
        f[6] += f[2] * f[0] * f[7];
        f[7] += f[2] * ( -f[7] - f[1] * f[6] - f[5] - f[5] * f[5] );

    if((f[5] > std::numeric_limits<float>::max()) && (f[6] > std::numeric_limits<float>::max()) && (f[7] > std::numeric_limits<float>::max())) 
    {
        f[5] = 0.1f; 
        f[6] = 0.1f;
        f[7] = 0.1f;
    }
    else
    {     
        out[static_cast<int>(interface::map::out::x)].store(f[5] * ctrl[static_cast<int>(interface::map::ctrl::amp)]->load() * 3.0f);
        out[static_cast<int>(interface::map::out::y)].store(f[6] * ctrl[static_cast<int>(interface::map::ctrl::amp)]->load() * 3.0f);
        out[static_cast<int>(interface::map::out::z)].store(f[7] * ctrl[static_cast<int>(interface::map::ctrl::amp)]->load() * 3.0f);
    }
}

void map_t::halvorsen_reset()
{
    f[0] = 1.4f;     // a
    f[1] = 0.01f;    // t

    f[5] = 0.1f;     // x
    f[6] = 0.0f;     // y
    f[7] = 0.0f;     // z
};


void map_t::halvorsen()
{
    f[1] = (ctrl[static_cast<int>(interface::map::ctrl::tune)]->load() + ctrl[static_cast<int>(interface::map::in::fm)]->load() * 0.01f) * 0.02f + 0.00001f;
    if(ctrl[static_cast<int>(interface::map::in::warp)] == &zero) f[0] = 1.4f + ctrl[static_cast<int>(interface::map::ctrl::warp)]->load();
    else f[0] = 1.4f + ctrl[static_cast<int>(interface::map::ctrl::warp)]->load() * fabsf(ctrl[static_cast<int>(interface::map::in::warp)]->load());

    f[5] += f[1] * ( - f[0] * f[5] - 4.0f * f[6] - 4.0f * f[7] - f[6] * f[6]);
    f[6] += f[1] * ( - f[0] * f[6] - 4.0f * f[7] - 4.0f * f[5] - f[7] * f[7]);
    f[7] += f[1] * ( - f[0] * f[7] - 4.0f * f[5] - 4.0f * f[6] - f[5] * f[5]);

    if((f[5] > std::numeric_limits<float>::max()) && (f[6] > std::numeric_limits<float>::max()) && (f[7] > std::numeric_limits<float>::max())) 
    {
        f[5] = 0.1f; 
        f[6] = 0.0f;
        f[7] = 0.0f;
    }
    else
    {
        out[static_cast<int>(interface::map::out::x)].store(f[5] * ctrl[static_cast<int>(interface::map::ctrl::amp)]->load() * 0.5f);
        out[static_cast<int>(interface::map::out::y)].store(f[6] * ctrl[static_cast<int>(interface::map::ctrl::amp)]->load() * 0.5f);
        out[static_cast<int>(interface::map::out::z)].store(f[7] * ctrl[static_cast<int>(interface::map::ctrl::amp)]->load() * 0.5f);
    }

}

void map_t::tsucs_reset()
{
    f[0] = 1.0f;     // x
    f[1] = 1.0f;     // y
    f[2] = 1.0f;     // z

    f[3] = 40.00f;   // a
    f[4] = 0.500f;   // b
    f[5] = 20.00f;   // c
    f[6] = 0.833f;   // d
    f[7] = 0.650f;   // e

    f[8] = 0.001f;   // t
};

void map_t::tsucs()
{
    if(ctrl[static_cast<int>(interface::map::in::warp)] == &zero) f[7] = ctrl[static_cast<int>(interface::map::ctrl::warp)]->load() / 8.0f + 0.55f;
    else f[7] = ctrl[static_cast<int>(interface::map::ctrl::warp)]->load() * fabsf(ctrl[static_cast<int>(interface::map::in::warp)]->load())  / 8.0f + 0.55f;

    f[8] = (ctrl[static_cast<int>(interface::map::ctrl::tune)]->load() + ctrl[static_cast<int>(interface::map::in::fm)]->load() * 0.01f) / 100.0f + 0.00001f;

        f[0] += f[8] * (f[3] * (f[1] - f[0]) + f[4] * f[0] * f[2]);
        f[1] += f[8] * (f[5] *  f[1] - f[0]  * f[2]);
        f[2] += f[8] * (f[6] *  f[2] + f[0]  * f[1] - f[7] * f[0] * f[0]);
 
    if((f[0] > std::numeric_limits<float>::max()) && (f[1] > std::numeric_limits<float>::max()) && (f[2] > std::numeric_limits<float>::max())) 
    {
        f[0] = 1.0f; 
        f[1] = 1.0f;
        f[2] = 1.0f;
    }
    else
    {
        out[static_cast<int>(interface::map::out::x)].store((f[0]) * ctrl[static_cast<int>(interface::map::ctrl::amp)]->load() * 0.05f);
        out[static_cast<int>(interface::map::out::y)].store((f[1]) * ctrl[static_cast<int>(interface::map::ctrl::amp)]->load() * 0.05f);
        out[static_cast<int>(interface::map::out::z)].store((f[2] - 45.0f) * ctrl[static_cast<int>(interface::map::ctrl::amp)]->load() * 0.05f);
    }
}

}
