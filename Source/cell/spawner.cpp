/*****************************************************************************************************************************
* ISC License
*
* Copyright (c) 2023 POLE
*
* Permission to use, copy, modify, and/or distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
* REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
* AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
* INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
* OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
******************************************************************************************************************************/

#include "spawner.hpp"

using namespace cell::interface;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destination buffer setter //////////////////////////////////////////////////////////////////////////////////////////////////
void cell::spawner::process()
{
    for(int e = 0; e < settings::env_n; ++e)
    {
        if(env[e].stage == OFF) env[e].start(); 
        env[e].iterate();
    }

    for(int e = 0; e < rack.size(); ++e) rack.process(e);

    sum[0].process();
    sum[1].process();
    pct.process();
    mix.process();

    l_r<float> lr { mix.out[0] * *bus.pot[potentiometer_list::volume], 
                    mix.out[1] * *bus.pot[potentiometer_list::volume] };

    out[0] = dcb[0].process(lr.l);
    out[1] = dcb[1].process(lr.r);
}


void cell::spawner::connect_bus()
{
    // Oscillators ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vco[0].ctrl[static_cast<int>(vco::ctrl::octave)] = bus.prm[parameter_list::vco_a_oct];
    vco[1].ctrl[static_cast<int>(vco::ctrl::octave)] = bus.prm[parameter_list::vco_b_oct];
    vco[2].ctrl[static_cast<int>(vco::ctrl::octave)] = bus.prm[parameter_list::vco_c_oct];
    vco[3].ctrl[static_cast<int>(vco::ctrl::octave)] = bus.prm[parameter_list::vco_d_oct];

    vco[0].ctrl[static_cast<int>(vco::ctrl::detune)] = bus.pot[potentiometer_list::vco_a_fine];
    vco[1].ctrl[static_cast<int>(vco::ctrl::detune)] = bus.pot[potentiometer_list::vco_b_fine];
    vco[2].ctrl[static_cast<int>(vco::ctrl::detune)] = bus.pot[potentiometer_list::vco_c_fine];
    vco[3].ctrl[static_cast<int>(vco::ctrl::detune)] = bus.pot[potentiometer_list::vco_d_fine];

    vco[0].ctrl[static_cast<int>(vco::ctrl::pll)]    = bus.pot[potentiometer_list::vco_a_warp];
    vco[1].ctrl[static_cast<int>(vco::ctrl::pll)]    = bus.pot[potentiometer_list::vco_b_warp];
    vco[2].ctrl[static_cast<int>(vco::ctrl::pll)]    = bus.pot[potentiometer_list::vco_c_warp];
    vco[3].ctrl[static_cast<int>(vco::ctrl::pll)]    = bus.pot[potentiometer_list::vco_d_warp];

    vco[0].ctrl[static_cast<int>(vco::ctrl::am)]     = bus.pot[potentiometer_list::vco_a_am];
    vco[1].ctrl[static_cast<int>(vco::ctrl::am)]     = bus.pot[potentiometer_list::vco_b_am];
    vco[2].ctrl[static_cast<int>(vco::ctrl::am)]     = bus.pot[potentiometer_list::vco_c_am];
    vco[3].ctrl[static_cast<int>(vco::ctrl::am)]     = bus.pot[potentiometer_list::vco_d_am];

    vco[0].ctrl[static_cast<int>(vco::ctrl::pwm)]    = bus.pot[potentiometer_list::vco_a_pw];
    vco[1].ctrl[static_cast<int>(vco::ctrl::pwm)]    = bus.pot[potentiometer_list::vco_b_pw];
    vco[2].ctrl[static_cast<int>(vco::ctrl::pwm)]    = bus.pot[potentiometer_list::vco_c_pw];
    vco[3].ctrl[static_cast<int>(vco::ctrl::pwm)]    = bus.pot[potentiometer_list::vco_d_pw];

    vco[0].ctrl[static_cast<int>(vco::ctrl::fm)]     = bus.pot[potentiometer_list::vco_a_fm];
    vco[1].ctrl[static_cast<int>(vco::ctrl::fm)]     = bus.pot[potentiometer_list::vco_b_fm];
    vco[2].ctrl[static_cast<int>(vco::ctrl::fm)]     = bus.pot[potentiometer_list::vco_c_fm];
    vco[3].ctrl[static_cast<int>(vco::ctrl::fm)]     = bus.pot[potentiometer_list::vco_d_fm];

    vco[0].ctrl[static_cast<int>(vco::ctrl::amp)]    = bus.pot[potentiometer_list::vco_a_amp];
    vco[1].ctrl[static_cast<int>(vco::ctrl::amp)]    = bus.pot[potentiometer_list::vco_b_amp];
    vco[2].ctrl[static_cast<int>(vco::ctrl::amp)]    = bus.pot[potentiometer_list::vco_c_amp];
    vco[3].ctrl[static_cast<int>(vco::ctrl::amp)]    = bus.pot[potentiometer_list::vco_d_amp];

    vco[0].ctrl[static_cast<int>(vco::ctrl::form)]   = bus.prm[parameter_list::vco_a_form];
    vco[1].ctrl[static_cast<int>(vco::ctrl::form)]   = bus.prm[parameter_list::vco_b_form];
    vco[2].ctrl[static_cast<int>(vco::ctrl::form)]   = bus.prm[parameter_list::vco_c_form];
    vco[3].ctrl[static_cast<int>(vco::ctrl::form)]   = bus.prm[parameter_list::vco_d_form];

    // Envelopes //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Filters ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vcf[0].ctrl[static_cast<int>(vcf::ctrl::cutoff)] = bus.pot[potentiometer_list::vcf_a_cut];
    vcf[0].ctrl[static_cast<int>(vcf::ctrl::Q)]      = bus.pot[potentiometer_list::vcf_a_res];

    vcf[1].ctrl[static_cast<int>(vcf::ctrl::cutoff)] = bus.pot[potentiometer_list::vcf_b_cut];
    vcf[1].ctrl[static_cast<int>(vcf::ctrl::Q)]      = bus.pot[potentiometer_list::vcf_b_res];

    // VCAs ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vca[0].ctrl[static_cast<int>(vca::ctrl::amp)]    = bus.pot[potentiometer_list::vca_a];
    vca[1].ctrl[static_cast<int>(vca::ctrl::amp)]    = bus.pot[potentiometer_list::vca_b];
    
    // Rotor //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    rot[0].ctrl[static_cast<int>(rtr::ctrl::x)]      = bus.pot[potentiometer_list::angle_x];
    rot[0].ctrl[static_cast<int>(rtr::ctrl::y)]      = bus.pot[potentiometer_list::angle_y];
    rot[0].ctrl[static_cast<int>(rtr::ctrl::z)]      = bus.pot[potentiometer_list::angle_z];

    // Mixer //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    mix.angle[0]                                     = bus.pot[potentiometer_list::lc_to_l];
    mix.angle[1]                                     = bus.pot[potentiometer_list::cr_to_r];

    // Chaos //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    chs[0].ctrl[static_cast<int>(map::ctrl::tune)]   = bus.pot[potentiometer_list::chs_a_tune];
    chs[1].ctrl[static_cast<int>(map::ctrl::tune)]   = bus.pot[potentiometer_list::chs_b_tune];

    chs[0].ctrl[static_cast<int>(map::ctrl::warp)]   = bus.pot[potentiometer_list::chs_a_warp];
    chs[1].ctrl[static_cast<int>(map::ctrl::warp)]   = bus.pot[potentiometer_list::chs_b_warp];

    chs[0].ctrl[static_cast<int>(map::ctrl::amp)]    = bus.pot[potentiometer_list::chs_a_amp];
    chs[1].ctrl[static_cast<int>(map::ctrl::amp)]    = bus.pot[potentiometer_list::chs_b_amp];

    chs[0].ctrl[static_cast<int>(map::ctrl::form)]   = bus.prm[parameter_list::chs_a_form];
    chs[1].ctrl[static_cast<int>(map::ctrl::form)]   = bus.prm[parameter_list::chs_b_form];

    lfo[0].ctrl[static_cast<int>(lfo::ctrl::form)]   = bus.prm[parameter_list::lfo_a_form];
    lfo[1].ctrl[static_cast<int>(lfo::ctrl::form)]   = bus.prm[parameter_list::lfo_b_form];

    // Delay //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vcd[0].ctrl[static_cast<int>(vcd::ctrl::time)]   = bus.pot[potentiometer_list::vcd_time];
    vcd[0].ctrl[static_cast<int>(vcd::ctrl::feed)]   = bus.pot[potentiometer_list::vcd_feed];

    // SNH ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    snh[0].ctrl[static_cast<int>(snh::ctrl::time)]   = bus.pot[potentiometer_list::snh_a];
    snh[1].ctrl[static_cast<int>(snh::ctrl::time)]   = bus.pot[potentiometer_list::snh_b];

    lfo[0].ctrl[static_cast<int>(lfo::ctrl::delta)]  = bus.pot[potentiometer_list::lfo_a_freq];
    lfo[1].ctrl[static_cast<int>(lfo::ctrl::delta)]  = bus.pot[potentiometer_list::lfo_b_freq];
    lfo[0].ctrl[static_cast<int>(lfo::ctrl::amp)]    = bus.pot[potentiometer_list::lfo_a_am];
    lfo[1].ctrl[static_cast<int>(lfo::ctrl::amp)]    = bus.pot[potentiometer_list::lfo_b_am];

    /**************************************************************************************************************************
    * 
    *  INPUT Sockets
    * 
    **************************************************************************************************************************/
    // VCO ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bay->io[socket_list::vco_a_in_fm].com    = &vco[0].in[static_cast<int>(vco::in::fm)];
    bay->io[socket_list::vco_b_in_fm].com    = &vco[1].in[static_cast<int>(vco::in::fm)];
    bay->io[socket_list::vco_c_in_fm].com    = &vco[2].in[static_cast<int>(vco::in::fm)];
    bay->io[socket_list::vco_d_in_fm].com    = &vco[3].in[static_cast<int>(vco::in::fm)];

    bay->io[socket_list::vco_a_in_pw].com    = &vco[0].in[static_cast<int>(vco::in::pwm)];
    bay->io[socket_list::vco_b_in_pw].com    = &vco[1].in[static_cast<int>(vco::in::pwm)];
    bay->io[socket_list::vco_c_in_pw].com    = &vco[2].in[static_cast<int>(vco::in::pwm)];
    bay->io[socket_list::vco_d_in_pw].com    = &vco[3].in[static_cast<int>(vco::in::pwm)];

    bay->io[socket_list::vco_a_in_sh].com    = &vco[0].in[static_cast<int>(vco::in::pll)];
    bay->io[socket_list::vco_b_in_sh].com    = &vco[1].in[static_cast<int>(vco::in::pll)];
    bay->io[socket_list::vco_c_in_sh].com    = &vco[2].in[static_cast<int>(vco::in::pll)];
    bay->io[socket_list::vco_d_in_sh].com    = &vco[3].in[static_cast<int>(vco::in::pll)];

    bay->io[socket_list::vco_a_in_am].com    = &vco[0].in[static_cast<int>(vco::in::am)];
    bay->io[socket_list::vco_b_in_am].com    = &vco[1].in[static_cast<int>(vco::in::am)];
    bay->io[socket_list::vco_c_in_am].com    = &vco[2].in[static_cast<int>(vco::in::am)];
    bay->io[socket_list::vco_d_in_am].com    = &vco[3].in[static_cast<int>(vco::in::am)];

    bay->io[socket_list::vco_a_in_ec].com    = &vco[0].in[static_cast<int>(vco::in::detune)];
    bay->io[socket_list::vco_b_in_ec].com    = &vco[1].in[static_cast<int>(vco::in::detune)];
    bay->io[socket_list::vco_c_in_ec].com    = &vco[2].in[static_cast<int>(vco::in::detune)];
    bay->io[socket_list::vco_d_in_ec].com    = &vco[3].in[static_cast<int>(vco::in::detune)];

    // Delay //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bay->io[socket_list::vcd_in_a].com       = &vcd[0].in[static_cast<int>(vcd::in::a)];
    bay->io[socket_list::vcd_in_b].com       = &vcd[0].in[static_cast<int>(vcd::in::b)];
    bay->io[socket_list::vcd_in_c].com       = &vcd[0].in[static_cast<int>(vcd::in::c)];
    bay->io[socket_list::vcd_in_d].com       = &vcd[0].in[static_cast<int>(vcd::in::d)];

    bay->io[socket_list::vcd_in_time].com    = &vcd[0].in[static_cast<int>(vcd::in::time)];
    bay->io[socket_list::vcd_in_feed].com    = &vcd[0].in[static_cast<int>(vcd::in::feed)];

    // VCF ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bay->io[socket_list::vcf_a_in_a].com     = &vcf[0].in[static_cast<int>(vcf::in::a)];
    bay->io[socket_list::vcf_b_in_a].com     = &vcf[1].in[static_cast<int>(vcf::in::a)];

    bay->io[socket_list::vcf_a_in_b].com     = &vcf[0].in[static_cast<int>(vcf::in::b)];
    bay->io[socket_list::vcf_b_in_b].com     = &vcf[1].in[static_cast<int>(vcf::in::b)];

    bay->io[socket_list::vcf_a_in_c].com     = &vcf[0].in[static_cast<int>(vcf::in::c)];
    bay->io[socket_list::vcf_b_in_c].com     = &vcf[1].in[static_cast<int>(vcf::in::c)];


    bay->io[socket_list::vcf_a_cut_in].com   = &vcf[0].in[static_cast<int>(vcf::in::cutoff)];
    bay->io[socket_list::vcf_b_cut_in].com   = &vcf[1].in[static_cast<int>(vcf::in::cutoff)];

    bay->io[socket_list::vcf_a_q_in].com     = &vcf[0].in[static_cast<int>(vcf::in::Q)];
    bay->io[socket_list::vcf_b_q_in].com     = &vcf[1].in[static_cast<int>(vcf::in::Q)];

    // VCA
    bay->io[socket_list::vca_a_in_a].com     = &vca[0].in[0];
    bay->io[socket_list::vca_b_in_a].com     = &vca[1].in[0];

    bay->io[socket_list::vca_a_in_b].com     = &vca[0].in[1];
    bay->io[socket_list::vca_b_in_b].com     = &vca[1].in[1];

    bay->io[socket_list::vca_a_in_cv].com    = &vca[0].in[static_cast<int>(vca::in::amp)];
    bay->io[socket_list::vca_b_in_cv].com    = &vca[1].in[static_cast<int>(vca::in::amp)];

    // Rotor
    bay->io[socket_list::lcr_cv_l].com       = &rot[0].in[static_cast<int>(rtr::in::cvx)];
    bay->io[socket_list::lcr_cv_c].com       = &rot[0].in[static_cast<int>(rtr::in::cvy)];
    bay->io[socket_list::lcr_cv_r].com       = &rot[0].in[static_cast<int>(rtr::in::cvx)];

    bay->io[socket_list::lcr_in_l_a].com     = &rot[0].in[static_cast<int>(rtr::in::ax)];
    bay->io[socket_list::lcr_in_c_a].com     = &rot[0].in[static_cast<int>(rtr::in::ay)];
    bay->io[socket_list::lcr_in_r_a].com     = &rot[0].in[static_cast<int>(rtr::in::az)];

    bay->io[socket_list::lcr_in_l_b].com     = &rot[0].in[static_cast<int>(rtr::in::bx)];
    bay->io[socket_list::lcr_in_c_b].com     = &rot[0].in[static_cast<int>(rtr::in::by)];
    bay->io[socket_list::lcr_in_r_b].com     = &rot[0].in[static_cast<int>(rtr::in::bz)];

    // Mixer
    bay->io[socket_list::mix_lc_cv].com      = &mix.cv[0];
    bay->io[socket_list::mix_cr_cv].com      = &mix.cv[1];

    bay->io[socket_list::mix_in_l].com       = &mix.in[0];
    bay->io[socket_list::mix_in_c].com       = &mix.in[1];
    bay->io[socket_list::mix_in_r].com       = &mix.in[2];


    bay->io[socket_list::sum_a_in_a].com     = &sum[0].in[0];
    bay->io[socket_list::sum_a_in_b].com     = &sum[0].in[1];
    bay->io[socket_list::sum_b_in_a].com     = &sum[1].in[0];
    bay->io[socket_list::sum_b_in_b].com     = &sum[1].in[1];

    bay->io[socket_list::sum_a_out_a].data   = &sum[0].out[0]; 
    bay->io[socket_list::sum_a_out_b].data   = &sum[0].out[1]; 
    bay->io[socket_list::sum_b_out_a].data   = &sum[1].out[0]; 
    bay->io[socket_list::sum_b_out_b].data   = &sum[1].out[1]; 

    bay->io[socket_list::pct_in_a].com       = &pct.in[0];
    bay->io[socket_list::pct_in_b].com       = &pct.in[1];
    bay->io[socket_list::pct_in_c].com       = &pct.in[2];

    bay->io[socket_list::pct_out_a].data     = &pct.out[0]; 

    // DSystem
    bay->io[socket_list::chs_a_in_wm].com    = &chs[0].ctrl[static_cast<int>(map::in::warp)];
    bay->io[socket_list::chs_b_in_wm].com    = &chs[1].ctrl[static_cast<int>(map::in::warp)];

    bay->io[socket_list::chs_a_in_ec].com    = &chs[0].ctrl[static_cast<int>(map::in::fm)];
    bay->io[socket_list::chs_b_in_ec].com    = &chs[1].ctrl[static_cast<int>(map::in::fm)];

    // Sample n Hold
    bay->io[socket_list::snh_a_in_a].com     = &snh[0].in[static_cast<int>(snh::in::a)];
    bay->io[socket_list::snh_a_in_b].com     = &snh[0].in[static_cast<int>(snh::in::b)];
    bay->io[socket_list::snh_b_in_a].com     = &snh[1].in[static_cast<int>(snh::in::a)];
    bay->io[socket_list::snh_b_in_b].com     = &snh[1].in[static_cast<int>(snh::in::b)];

    bay->io[socket_list::snh_a_time].com     = &snh[0].in[static_cast<int>(snh::in::time)];
    bay->io[socket_list::snh_b_time].com     = &snh[1].in[static_cast<int>(snh::in::time)];;

    bay->io[socket_list::lfo_a_in_freq].com  = &lfo[0].in[static_cast<int>(lfo::in::fm)];
    bay->io[socket_list::lfo_b_in_freq].com  = &lfo[1].in[static_cast<int>(lfo::in::fm)];

    bay->io[socket_list::lfo_a_in_am].com    = &lfo[0].in[static_cast<int>(lfo::in::am)];
    bay->io[socket_list::lfo_b_in_am].com    = &lfo[1].in[static_cast<int>(lfo::in::am)];


    /**************************************************************************************************************************
    * 
    *  OUTPUT Sockets
    * 
    **************************************************************************************************************************/
    bay->io[socket_list::vco_a_out].data           = &vco[0].out[static_cast<int>(vco::out::main)];
    bay->io[socket_list::vco_b_out].data           = &vco[1].out[static_cast<int>(vco::out::main)];
    bay->io[socket_list::vco_c_out].data           = &vco[2].out[static_cast<int>(vco::out::main)];
    bay->io[socket_list::vco_d_out].data           = &vco[3].out[static_cast<int>(vco::out::main)];

    bay->io[socket_list::env_a_out].data           = &env[0].out; 
    bay->io[socket_list::env_b_out].data           = &env[1].out;
    bay->io[socket_list::env_c_out].data           = &env[2].out;
    bay->io[socket_list::env_d_out].data           = &env[3].out;


    bay->io[socket_list::chs_a_out_x].data         = &chs[0].out[0];
    bay->io[socket_list::chs_a_out_y].data         = &chs[0].out[1];
    bay->io[socket_list::chs_a_out_z].data         = &chs[0].out[2];

    bay->io[socket_list::chs_b_out_x].data         = &chs[1].out[0];
    bay->io[socket_list::chs_b_out_y].data         = &chs[1].out[1];
    bay->io[socket_list::chs_b_out_z].data         = &chs[1].out[2];

    bay->io[socket_list::vcf_a_out_lp].data        = &vcf[0].out[static_cast<int>(vcf::out::lp)];
    bay->io[socket_list::vcf_a_out_bp].data        = &vcf[0].out[static_cast<int>(vcf::out::bp)];
    bay->io[socket_list::vcf_a_out_hp].data        = &vcf[0].out[static_cast<int>(vcf::out::hp)];

    bay->io[socket_list::vcf_b_out_lp].data        = &vcf[1].out[static_cast<int>(vcf::out::lp)];
    bay->io[socket_list::vcf_b_out_bs].data        = &vcf[1].out[static_cast<int>(vcf::out::bp)];
    bay->io[socket_list::vcf_b_out_hp].data        = &vcf[1].out[static_cast<int>(vcf::out::hp)];

    bay->io[socket_list::vca_a_out_a].data         = &vca[0].out[0];
    bay->io[socket_list::vca_b_out_a].data         = &vca[1].out[0];

    bay->io[socket_list::vca_a_out_b].data         = &vca[0].out[1];
    bay->io[socket_list::vca_b_out_b].data         = &vca[1].out[1];

    bay->io[socket_list::lcr_out_l_a].data         = &rot[0].out[static_cast<int>(rtr::out::ax)];
    bay->io[socket_list::lcr_out_c_a].data         = &rot[0].out[static_cast<int>(rtr::out::ay)];
    bay->io[socket_list::lcr_out_r_a].data         = &rot[0].out[static_cast<int>(rtr::out::az)];

    bay->io[socket_list::lcr_out_l_b].data         = &rot[0].out[static_cast<int>(rtr::out::bx)];
    bay->io[socket_list::lcr_out_c_b].data         = &rot[0].out[static_cast<int>(rtr::out::by)];
    bay->io[socket_list::lcr_out_r_b].data         = &rot[0].out[static_cast<int>(rtr::out::bz)];

    bay->io[socket_list::snh_a_out].data           = &snh[0].out[static_cast<int>(snh::out::a)];
    bay->io[socket_list::snh_b_out].data           = &snh[1].out[static_cast<int>(snh::out::a)];

    bay->io[socket_list::lfo_a_out_a].data         = &lfo[0].out[static_cast<int>(lfo::out::a)];
    bay->io[socket_list::lfo_a_out_b].data         = &lfo[0].out[static_cast<int>(lfo::out::b)];
    bay->io[socket_list::lfo_b_out_a].data         = &lfo[1].out[static_cast<int>(lfo::out::a)];
    bay->io[socket_list::lfo_b_out_b].data         = &lfo[1].out[static_cast<int>(lfo::out::b)];

    bay->io[socket_list::vcd_out_a].data           = &vcd[0].out[static_cast<int>(vcd::out::a)];
    bay->io[socket_list::vcd_out_b].data           = &vcd[0].out[static_cast<int>(vcd::out::b)];
    bay->io[socket_list::vcd_out_c].data           = &vcd[0].out[static_cast<int>(vcd::out::c)];
    bay->io[socket_list::vcd_out_d].data           = &vcd[0].out[static_cast<int>(vcd::out::d)];




    /**************************************************************************************************************************
    * 
    *  Envelopes 
    * 
    **************************************************************************************************************************/
    env[0].node[1].time = bus.prm[parameter_list::env_a_time_a];
    env[0].node[2].time = bus.prm[parameter_list::env_a_time_h];
    env[0].node[3].time = bus.prm[parameter_list::env_a_time_d];
    env[0].node[4].time = bus.prm[parameter_list::env_a_time_s];
    env[0].node[5].time = bus.prm[parameter_list::env_a_time_r];

    env[1].node[1].time = bus.prm[parameter_list::env_b_time_a];
    env[1].node[2].time = bus.prm[parameter_list::env_b_time_h];
    env[1].node[3].time = bus.prm[parameter_list::env_b_time_d];
    env[1].node[4].time = bus.prm[parameter_list::env_b_time_s];
    env[1].node[5].time = bus.prm[parameter_list::env_b_time_r];

    env[2].node[1].time = bus.prm[parameter_list::env_c_time_a];
    env[2].node[2].time = bus.prm[parameter_list::env_c_time_h];
    env[2].node[3].time = bus.prm[parameter_list::env_c_time_d];
    env[2].node[4].time = bus.prm[parameter_list::env_c_time_s];
    env[2].node[5].time = bus.prm[parameter_list::env_c_time_r];

    env[3].node[1].time = bus.prm[parameter_list::env_d_time_a];
    env[3].node[2].time = bus.prm[parameter_list::env_d_time_h];
    env[3].node[3].time = bus.prm[parameter_list::env_d_time_d];
    env[3].node[4].time = bus.prm[parameter_list::env_d_time_s];
    env[3].node[5].time = bus.prm[parameter_list::env_d_time_r];


    env[0].node[1].value = bus.prm[parameter_list::env_a_value_a];
    env[0].node[2].value = bus.prm[parameter_list::env_a_value_h];
    env[0].node[3].value = bus.prm[parameter_list::env_a_value_d];
    env[0].node[4].value = bus.prm[parameter_list::env_a_value_s];
    env[0].node[5].value = bus.prm[parameter_list::env_a_value_r];

    env[1].node[1].value = bus.prm[parameter_list::env_b_value_a];
    env[1].node[2].value = bus.prm[parameter_list::env_b_value_h];
    env[1].node[3].value = bus.prm[parameter_list::env_b_value_d];
    env[1].node[4].value = bus.prm[parameter_list::env_b_value_s];
    env[1].node[5].value = bus.prm[parameter_list::env_b_value_r];

    env[2].node[1].value = bus.prm[parameter_list::env_c_value_a];
    env[2].node[2].value = bus.prm[parameter_list::env_c_value_h];
    env[2].node[3].value = bus.prm[parameter_list::env_c_value_d];
    env[2].node[4].value = bus.prm[parameter_list::env_c_value_s];
    env[2].node[5].value = bus.prm[parameter_list::env_c_value_r];

    env[3].node[1].value = bus.prm[parameter_list::env_d_value_a];
    env[3].node[2].value = bus.prm[parameter_list::env_d_value_h];
    env[3].node[3].value = bus.prm[parameter_list::env_d_value_d];
    env[3].node[4].value = bus.prm[parameter_list::env_d_value_s];
    env[3].node[5].value = bus.prm[parameter_list::env_d_value_r];

    env[0].node[1].curve = bus.prm[parameter_list::env_a_form_a];
    env[0].node[2].curve = bus.prm[parameter_list::env_a_form_h];
    env[0].node[3].curve = bus.prm[parameter_list::env_a_form_d];
    env[0].node[4].curve = bus.prm[parameter_list::env_a_form_s];
    env[0].node[5].curve = bus.prm[parameter_list::env_a_form_r];

    env[1].node[1].curve = bus.prm[parameter_list::env_b_form_a];
    env[1].node[2].curve = bus.prm[parameter_list::env_b_form_h];
    env[1].node[3].curve = bus.prm[parameter_list::env_b_form_d];
    env[1].node[4].curve = bus.prm[parameter_list::env_b_form_s];
    env[1].node[5].curve = bus.prm[parameter_list::env_b_form_r];

    env[2].node[1].curve = bus.prm[parameter_list::env_c_form_a];
    env[2].node[2].curve = bus.prm[parameter_list::env_c_form_h];
    env[2].node[3].curve = bus.prm[parameter_list::env_c_form_d];
    env[2].node[4].curve = bus.prm[parameter_list::env_c_form_s];
    env[2].node[5].curve = bus.prm[parameter_list::env_c_form_r];

    env[3].node[1].curve = bus.prm[parameter_list::env_d_form_a];
    env[3].node[2].curve = bus.prm[parameter_list::env_d_form_h];
    env[3].node[3].curve = bus.prm[parameter_list::env_d_form_d];
    env[3].node[4].curve = bus.prm[parameter_list::env_d_form_s];
    env[3].node[5].curve = bus.prm[parameter_list::env_d_form_r];

    env[0].time_scale    = bus.prm[parameter_list::env_a_scale];
    env[1].time_scale    = bus.prm[parameter_list::env_b_scale];
    env[2].time_scale    = bus.prm[parameter_list::env_c_scale];
    env[3].time_scale    = bus.prm[parameter_list::env_d_scale];

    vco[0].ctrl[static_cast<int>(vco::ctrl::freerun)]   = bus.prm[parameter_list::vco_a_freerun];
    vco[1].ctrl[static_cast<int>(vco::ctrl::freerun)]   = bus.prm[parameter_list::vco_b_freerun];
    vco[2].ctrl[static_cast<int>(vco::ctrl::freerun)]   = bus.prm[parameter_list::vco_c_freerun];
    vco[3].ctrl[static_cast<int>(vco::ctrl::freerun)]   = bus.prm[parameter_list::vco_d_freerun];


   
    for(int i = 0; i < settings::osc_n; ++i)
    {
        for(int j = 0; j < settings::poly; ++j)
        {
            for(int e = 0; e < SEGMENTS; ++e)
            {
                vco[i].env[j].node[e].time     = env[i].node[e].time;
                vco[i].env[j].node[e].value    = env[i].node[e].value;
                vco[i].env[j].node[e].curve    = env[i].node[e].curve;
                vco[i].env[j].time_scale       = env[i].time_scale;
            }
        }
    }

    int p = 0;
    for(int e = 0; e < settings::lfo_n; ++e) rack.bind(&lfo[e], p++);
    for(int e = 0; e < settings::snh_n; ++e) rack.bind(&snh[e], p++);
    for(int e = 0; e < settings::vca_n; ++e) rack.bind(&vca[e], p++);
    for(int e = 0; e < settings::map_n; ++e) rack.bind(&chs[e], p++);
    for(int e = 0; e < settings::osc_n; ++e) rack.bind(&vco[e], p++);
    for(int e = 0; e < settings::vcf_n; ++e) rack.bind(&vcf[e], p++);
    for(int e = 0; e < settings::vcd_n; ++e) rack.bind(&vcd[e], p++);
    for(int e = 0; e < settings::rtr_n; ++e) rack.bind(&rot[e], p++);






}

cell::spawner::spawner()
{
    int modules_n = 
        settings::osc_n +
        settings::lfo_n +
        settings::map_n +
        settings::snh_n +
        settings::vca_n +
        settings::vcf_n +
        settings::vcd_n +
        settings::rtr_n;

    rack.allocate(modules_n);
}

cell::spawner::~spawner()
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
