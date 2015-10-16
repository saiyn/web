/** \file api_doc.h
*  Type definitions that describe the primitive application programming interface (API) *
*  The api definitions are created by DSP composer *
*/


#ifndef __COMPOSER_API_DOC_H__
#define __COMPOSER_API_DOC_H__



/** '1x8 PEQ'  (Preset)
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _ramp_1x8_PEQ_Preset 0x0001          /*signed 1.31 format*/
#define _a_11_1x8_PEQ_Preset 0x0002          /*signed 1.31 format*/
#define _a_12_1x8_PEQ_Preset 0x0003          /*signed 1.31 format*/
#define _DspSampleRate_1x8_PEQ_Preset 0x0004          /*READ-ONLY: unsigned integer*/
#define _a_14_1x8_PEQ_Preset 0x0005          /*signed 1.31 format*/
#define _a_15_1x8_PEQ_Preset 0x0006          /*signed 1.31 format*/
#define _a_16_1x8_PEQ_Preset 0x0007          /*signed 1.31 format*/
#define _a_17_1x8_PEQ_Preset 0x0008          /*signed 1.31 format*/
#define _a_18_1x8_PEQ_Preset 0x0009          /*signed 1.31 format*/
#define _b_11_1x8_PEQ_Preset 0x000a          /*signed 1.31 format*/
#define _b_12_1x8_PEQ_Preset 0x000b          /*signed 1.31 format*/
#define _b_13_1x8_PEQ_Preset 0x000c          /*signed 1.31 format*/
#define _b_14_1x8_PEQ_Preset 0x000d          /*signed 1.31 format*/
#define _b_15_1x8_PEQ_Preset 0x000e          /*signed 1.31 format*/
#define _b_16_1x8_PEQ_Preset 0x000f          /*signed 1.31 format*/
#define _b_17_1x8_PEQ_Preset 0x0010          /*signed 1.31 format*/
#define _b_18_1x8_PEQ_Preset 0x0011          /*signed 1.31 format*/
#define _g_11_1x8_PEQ_Preset 0x0012          /*signed 3.29 format*/
#define _g_12_1x8_PEQ_Preset 0x0013          /*signed 3.29 format*/
#define _g_13_1x8_PEQ_Preset 0x0014          /*signed 3.29 format*/
#define _g_14_1x8_PEQ_Preset 0x0015          /*signed 3.29 format*/
#define _g_15_1x8_PEQ_Preset 0x0016          /*signed 3.29 format*/
#define _g_16_1x8_PEQ_Preset 0x0017          /*signed 3.29 format*/
#define _g_17_1x8_PEQ_Preset 0x0018          /*signed 3.29 format*/
#define _g_18_1x8_PEQ_Preset 0x0019          /*signed 3.29 format*/
#define _minus_one_1x8_PEQ_Preset 0x0021          /*signed 1.31 format*/
#define _hold_count_1x8_PEQ_Preset 0x0022          /*signed integer*/
#define _clip_1x8_PEQ_Preset 0x0023          /*READ-ONLY: signed integer*/

/** '1x6 PEQ'  (Preset)
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _ramp_1x6_PEQ_Preset 0x0024          /*signed 1.31 format*/
#define _a_11_1x6_PEQ_Preset 0x0025          /*signed 1.31 format*/
#define _a_12_1x6_PEQ_Preset 0x0026          /*signed 1.31 format*/
#define _DspSampleRate_1x6_PEQ_Preset 0x0027          /*READ-ONLY: unsigned integer*/
#define _a_14_1x6_PEQ_Preset 0x0028          /*signed 1.31 format*/
#define _a_15_1x6_PEQ_Preset 0x0029          /*signed 1.31 format*/
#define _a_16_1x6_PEQ_Preset 0x002a          /*signed 1.31 format*/
#define _b_11_1x6_PEQ_Preset 0x002b          /*signed 1.31 format*/
#define _b_12_1x6_PEQ_Preset 0x002c          /*signed 1.31 format*/
#define _b_13_1x6_PEQ_Preset 0x002d          /*signed 1.31 format*/
#define _b_14_1x6_PEQ_Preset 0x002e          /*signed 1.31 format*/
#define _b_15_1x6_PEQ_Preset 0x002f          /*signed 1.31 format*/
#define _b_16_1x6_PEQ_Preset 0x0030          /*signed 1.31 format*/
#define _g_11_1x6_PEQ_Preset 0x0031          /*signed 3.29 format*/
#define _g_12_1x6_PEQ_Preset 0x0032          /*signed 3.29 format*/
#define _g_13_1x6_PEQ_Preset 0x0033          /*signed 3.29 format*/
#define _g_14_1x6_PEQ_Preset 0x0034          /*signed 3.29 format*/
#define _g_15_1x6_PEQ_Preset 0x0035          /*signed 3.29 format*/
#define _g_16_1x6_PEQ_Preset 0x0036          /*signed 3.29 format*/
#define _minus_one_1x6_PEQ_Preset 0x003c          /*signed 1.31 format*/
#define _hold_count_1x6_PEQ_Preset 0x003d          /*signed integer*/
#define _clip_1x6_PEQ_Preset 0x003e          /*READ-ONLY: signed integer*/

/** 'HP_PT/48db' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _hold_count_HP_PT_48db 0x003f          /*signed integer*/
#define _ramp_HP_PT_48db 0x0040          /*signed 1.31 format*/
#define _k_HP_PT_48db 0x0041          /*signed 2.30 format*/
#define _a_1_HP_PT_48db 0x0042          /*signed 1.31 format*/
#define _b_1_HP_PT_48db 0x0043          /*signed 1.31 format*/
#define _c_1_HP_PT_48db 0x0044          /*signed integer*/
#define _a_2_HP_PT_48db 0x0045          /*signed 1.31 format*/
#define _b_2_HP_PT_48db 0x0046          /*signed 1.31 format*/
#define _c_2_HP_PT_48db 0x0047          /*signed integer*/
#define _a_3_HP_PT_48db 0x0048          /*signed 1.31 format*/
#define _b_3_HP_PT_48db 0x0049          /*signed 1.31 format*/
#define _c_3_HP_PT_48db 0x004a          /*signed integer*/
#define _a_4_HP_PT_48db 0x004b          /*signed 1.31 format*/
#define _b_4_HP_PT_48db 0x004c          /*signed 1.31 format*/
#define _c_4_HP_PT_48db 0x004d          /*signed integer*/
#define _clip_HP_PT_48db 0x004e          /*READ-ONLY: signed integer*/

/** 'HP_PT/48db' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _hold_count_HP_PT_48db_3447 0x004f          /*signed integer*/
#define _ramp_HP_PT_48db_3447 0x0050          /*signed 1.31 format*/
#define _k_HP_PT_48db_3447 0x0051          /*signed 2.30 format*/
#define _a_1_HP_PT_48db_3447 0x0052          /*signed 1.31 format*/
#define _b_1_HP_PT_48db_3447 0x0053          /*signed 1.31 format*/
#define _c_1_HP_PT_48db_3447 0x0054          /*signed integer*/
#define _a_2_HP_PT_48db_3447 0x0055          /*signed 1.31 format*/
#define _b_2_HP_PT_48db_3447 0x0056          /*signed 1.31 format*/
#define _c_2_HP_PT_48db_3447 0x0057          /*signed integer*/
#define _a_3_HP_PT_48db_3447 0x0058          /*signed 1.31 format*/
#define _b_3_HP_PT_48db_3447 0x0059          /*signed 1.31 format*/
#define _c_3_HP_PT_48db_3447 0x005a          /*signed integer*/
#define _a_4_HP_PT_48db_3447 0x005b          /*signed 1.31 format*/
#define _b_4_HP_PT_48db_3447 0x005c          /*signed 1.31 format*/
#define _c_4_HP_PT_48db_3447 0x005d          /*signed integer*/
#define _clip_HP_PT_48db_3447 0x005e          /*READ-ONLY: signed integer*/

/** 'HP_Sub/48db' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _hold_count_HP_Sub_48db 0x005f          /*signed integer*/
#define _ramp_HP_Sub_48db 0x0060          /*signed 1.31 format*/
#define _k_HP_Sub_48db 0x0061          /*signed 2.30 format*/
#define _a_1_HP_Sub_48db 0x0062          /*signed 1.31 format*/
#define _b_1_HP_Sub_48db 0x0063          /*signed 1.31 format*/
#define _c_1_HP_Sub_48db 0x0064          /*signed integer*/
#define _a_2_HP_Sub_48db 0x0065          /*signed 1.31 format*/
#define _b_2_HP_Sub_48db 0x0066          /*signed 1.31 format*/
#define _c_2_HP_Sub_48db 0x0067          /*signed integer*/
#define _a_3_HP_Sub_48db 0x0068          /*signed 1.31 format*/
#define _b_3_HP_Sub_48db 0x0069          /*signed 1.31 format*/
#define _c_3_HP_Sub_48db 0x006a          /*signed integer*/
#define _a_4_HP_Sub_48db 0x006b          /*signed 1.31 format*/
#define _b_4_HP_Sub_48db 0x006c          /*signed 1.31 format*/
#define _c_4_HP_Sub_48db 0x006d          /*signed integer*/
#define _clip_HP_Sub_48db 0x006e          /*READ-ONLY: signed integer*/

/** 'LP_Sub/48db' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _hold_count_LP_Sub_48db 0x006f          /*signed integer*/
#define _ramp_LP_Sub_48db 0x0070          /*signed 1.31 format*/
#define _k_LP_Sub_48db 0x0071          /*signed 2.30 format*/
#define _a_1_LP_Sub_48db 0x0072          /*signed 1.31 format*/
#define _b_1_LP_Sub_48db 0x0073          /*signed 1.31 format*/
#define _c_1_LP_Sub_48db 0x0074          /*signed integer*/
#define _a_2_LP_Sub_48db 0x0075          /*signed 1.31 format*/
#define _b_2_LP_Sub_48db 0x0076          /*signed 1.31 format*/
#define _c_2_LP_Sub_48db 0x0077          /*signed integer*/
#define _a_3_LP_Sub_48db 0x0078          /*signed 1.31 format*/
#define _b_3_LP_Sub_48db 0x0079          /*signed 1.31 format*/
#define _c_3_LP_Sub_48db 0x007a          /*signed integer*/
#define _a_4_LP_Sub_48db 0x007b          /*signed 1.31 format*/
#define _b_4_LP_Sub_48db 0x007c          /*signed 1.31 format*/
#define _c_4_LP_Sub_48db 0x007d          /*signed integer*/
#define _clip_LP_Sub_48db 0x007e          /*READ-ONLY: signed integer*/

/** '24dB Gain'  (Sonance APP)
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _hold_count_24dB_Gain_Sonance_APP 0x007f          /*signed integer*/
#define _channel_clip_1_24dB_Gain_Sonance_APP 0x0080          /*READ-ONLY: signed integer*/
#define _channel_clip_2_24dB_Gain_Sonance_APP 0x0081          /*READ-ONLY: signed integer*/
#define _channel_clip_3_24dB_Gain_Sonance_APP 0x0082          /*READ-ONLY: signed integer*/
#define _channel_clip_4_24dB_Gain_Sonance_APP 0x0083          /*READ-ONLY: signed integer*/
#define _channel_clip_5_24dB_Gain_Sonance_APP 0x0084          /*READ-ONLY: signed integer*/
#define _channel_clip_6_24dB_Gain_Sonance_APP 0x0085          /*READ-ONLY: signed integer*/
#define _channel_clip_7_24dB_Gain_Sonance_APP 0x0086          /*READ-ONLY: signed integer*/
#define _channel_clip_8_24dB_Gain_Sonance_APP 0x0087          /*READ-ONLY: signed integer*/
#define _channel_clip_9_24dB_Gain_Sonance_APP 0x0088          /*READ-ONLY: signed integer*/
#define _channel_clip_10_24dB_Gain_Sonance_APP 0x0089          /*READ-ONLY: signed integer*/
#define _channel_clip_11_24dB_Gain_Sonance_APP 0x008a          /*READ-ONLY: signed integer*/
#define _channel_clip_12_24dB_Gain_Sonance_APP 0x008b          /*READ-ONLY: signed integer*/

/** '1x2 PEQ'  (Preset)
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _ramp_1x2_PEQ_Preset 0x008c          /*signed 1.31 format*/
#define _a_11_1x2_PEQ_Preset 0x008d          /*signed 1.31 format*/
#define _a_12_1x2_PEQ_Preset 0x008e          /*signed 1.31 format*/
#define _DspSampleRate_1x2_PEQ_Preset 0x008f          /*READ-ONLY: unsigned integer*/
#define _b_12_1x2_PEQ_Preset 0x0090          /*signed 1.31 format*/
#define _g_11_1x2_PEQ_Preset 0x0091          /*signed 3.29 format*/
#define _g_12_1x2_PEQ_Preset 0x0092          /*signed 3.29 format*/
#define _minus_one_1x2_PEQ_Preset 0x0094          /*signed 1.31 format*/
#define _hold_count_1x2_PEQ_Preset 0x0095          /*signed integer*/
#define _clip_1x2_PEQ_Preset 0x0096          /*READ-ONLY: signed integer*/

/** 'Compressor'  (Limiter)
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _threshold_Compressor_Limiter 0x0097          /*signed 9.23 format*/
#define _soft_knee_Compressor_Limiter 0x0098          /*signed 9.23 format*/
#define _dsp_ratio_Compressor_Limiter 0x0099          /*signed 1.31 format*/
#define _dsp_attack_Compressor_Limiter 0x009a          /*signed 1.31 format*/
#define _dsp_release_Compressor_Limiter 0x009b          /*signed 1.31 format*/
#define _dsp_bypass_Compressor_Limiter 0x009c          /*signed integer*/
#define _region_Compressor_Limiter 0x009d          /*READ-ONLY: signed integer signal indicator, 0 - below; 1 - knee; 2 - above*/
#define _dsp_reduction_Compressor_Limiter 0x009e          /*READ-ONLY: signed 1.31 format*/

/** 'Output level' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _hold_count_Output_level 0x009f          /*signed integer*/
#define _ramp_Output_level 0x00a0          /*signed 1.31 format*/
#define _g_1_Output_level 0x00a1          /*signed 3.29 format*/
#define _g_2_Output_level 0x00a2          /*signed 3.29 format*/
#define _g_3_Output_level 0x00a3          /*signed 3.29 format*/
#define _channel_clip_1_Output_level 0x00a4          /*READ-ONLY: signed integer*/
#define _channel_clip_2_Output_level 0x00a5          /*READ-ONLY: signed integer*/
#define _channel_clip_3_Output_level 0x00a6          /*READ-ONLY: signed integer*/

/** 'Mixer 2x1' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _bypass_Mixer_2x1 0x00a7          /*signed integer*/
#define _enable_Mixer_2x1 0x00a8          /*signed integer*/
#define _a_coeff_Mixer_2x1 0x00a9          /*signed 1.31 format*/
#define _b_coeff_Mixer_2x1 0x00aa          /*signed 1.31 format*/

/** 'Master Volume' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _hold_count_Master_Volume 0x00ab          /*signed integer*/
#define _ramp_Master_Volume 0x00ac          /*signed 1.31 format*/
#define _g_1_Master_Volume 0x00ad          /*signed 3.29 format*/
#define _channel_clip_1_Master_Volume 0x00ae          /*READ-ONLY: signed integer*/

/** 'Peak Square Detector'  (Limiter)
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _input_disable_1_Peak_Square_Detector_Limiter 0x00af          /*signed integer*/
#define _tc_Peak_Square_Detector_Limiter 0x00b0          /*signed 1.31 format*/
#define _max_peak_Peak_Square_Detector_Limiter 0x00b1          /*READ-ONLY: signed 1.31 format*/

/** 'SUB Delay' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _bypass_SUB_Delay 0x00b2          /*signed integer*/
#define _enable_SUB_Delay 0x00b3          /*signed integer*/
#define _delay_value_SUB_Delay 0x00b4          /*signed 16.16 format*/

/** 'SUB Delay' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _bypass_SUB_Delay_3413 0x00b5          /*signed integer*/
#define _enable_SUB_Delay_3413 0x00b6          /*signed integer*/
#define _delay_value_SUB_Delay_3413 0x00b7          /*signed 16.16 format*/

/** '2x1 Router' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _ramp_2x1_Router 0x00b8          /*signed 1.31 format*/
#define _output_select_1_2x1_Router 0x00b9          /*signed integer*/

/** 'Dealay Router_Line' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _ramp_Dealay_Router_Line 0x00ba          /*signed 1.31 format*/
#define _output_select_1_Dealay_Router_Line 0x00bb          /*signed integer*/

/** 'Phase Router' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _ramp_Phase_Router 0x00bc          /*signed 1.31 format*/
#define _output_select_1_Phase_Router 0x00bd          /*signed integer*/

/** 'Dealay Router_Line' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _ramp_Dealay_Router_Line_3455 0x00be          /*signed 1.31 format*/
#define _output_select_1_Dealay_Router_Line_3455 0x00bf          /*signed integer*/

/** 'Dealay Router_Sub' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _ramp_Dealay_Router_Sub 0x00c0          /*signed 1.31 format*/
#define _output_select_1_Dealay_Router_Sub 0x00c1          /*signed integer*/

/** 'BW LP' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _bypass_BW_LP 0x00c2          /*signed integer*/
#define _Cutoff_Frequency__BW_LP 0x00c3          /*signed integer*/

/** 'On/Off'  (Preset)
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _value_On_Off_Preset 0x00c4          /*signed integer*/

/** '-(X)' 
*
* Write Message = 0xf000HHHH 0xhhhhhhhh
* Read Request Message = 0xf0C0HHHH
* Read ResponseMessage = 0x70C0HHHH 0xhhhhhhhh
* 0xHHHH = index. 0xhhhhhhhh = Data Value
*/
#define _polarity___X_ 0x00c5          /*signed integer*/
#endif /*__COMPOSER_API_DOC_H__*/