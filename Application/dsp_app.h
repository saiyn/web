#ifndef _DSP_APP_H_
#define _DSP_APP_H_





void dsp_master_vol_set(int);


void dsp_sub_hp_bypass(void);

void dsp_sub_lp_bypass(void);

void dsp_lineout_hp_bypass(void);

void dsp_subLpFre_set(uint8 slope, uint16 freq, uint8 num, int offset1, int offset2);

void dsp_delay_switch_to_sub(void);

void dsp_delay_time_set(uint8 time);

void dsp_delay_switch_off(void);

void dsp_delay_switch_to_line(void);

void dsp_delay_switch_to_sub(void);

void dsp_eq_set_freq(uint32 freq, uint8 offset, uint8 type);

void dsp_eq_set_gain(uint32 gain, uint8 offset, uint8 type);

void dsp_eq_set_width(uint32 q, uint8 offset, uint8 type);

void dsp_eq_off(void);

void dsp_eq_on(void);

void dsp_phrase_off(void);

void dsp_phrase_on(void);

#endif



