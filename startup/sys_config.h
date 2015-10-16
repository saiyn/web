#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_


#define CONFIG_USE_CONSOLE
#define CONFIG_USE_CONSOLE

#define EPPROM_IIC_ADDRESS 0xA0



#define VOL_MENU_3S_TIMEOUT  (3 * 1000)
#define DIS_TIMEOUT          (20 * 1000)



#define SYS_NV_ADDRESS  0x00
#define SYS_LOW_POWER_FLAG_ADDRESS 0x200
#define SYS_NV_ADDRESS_SUB  0x400
#define SYS_NV_ADDRESS_EQ   0x800



#define CONFIG_SYS_MAXARGS  16

#define VOLUME_INDEX_DEFAULT   0
#define VOLUME_VALUE_MAX(x)     ((x)>=0?(11 - (x)):(11))
#define VOLUME_VALUE_MIN(x)     ((x)>=0?(-40):(-40 - (x)))
#define VOLUME_OFFSET_BETWEEN_VALUE_AND_GAIN(x)  (41 + (x))
#define VOLUME_HARDWARE_GAIN     29
#define VOLUME_GAIN_IN_1195_DEFAULT   6
#define VOLUME_GAIN_IN_DSP(x)   ((x) - VOLUME_HARDWARE_GAIN - VOLUME_GAIN_IN_1195_DEFAULT)
#define VOLUME_GAIN_IN_DSP_MAX  (6)

#define MASTER_VOL_MUTE_REG_VALUE (-41)
#define MASTER_VOL_VALUE_MIN  (-40)
#define MASTER_VOL_VALUE_MAX   (11)


#define INPUT_GAIN_MIN       (-10)
#define INPUT_GAIN_MAX       (10)

#define SENSE_LEVEL_MIN      3
#define SENSE_LEVEL_MAX      15
#define SENSE_LEVEL_DEFAULT  SENSE_6MV

#define FILTER_FREQ_MIN      20

#define SUB_LOWPASS_MIN      30
#define SUB_LOWPASS_MAX      300
/*special value to indicate LFE according to the webpage handler*/
#define SUB_LOWPASS_DEFAULT  330 
#define SUB_LOWPASS_NODE1    60
#define SUB_LOWPASS_NODE2    120
#define SUB_LP_NODE1_STEP     1
#define SUB_LP_NODE2_STEP     5
#define SUB_LP_NODE3_STEP     10

#define SUB_LP12DB_REG_NUM     3
#define SUB_LP24DB_REG_NUM     6
#define SUB_LP48DB_REG_NUM    12

#define SUB_LP_OFFSET_OF_FILTER  (SUB_LOWPASS_MIN - FILTER_FREQ_MIN)


#define SUB_HIGHPASS_MIN  20
#define SUB_HIGHPASS_MAX  100
#define SUB_HIGHPASS_DEFAULT 20
#define SUB_HIGHPASS_OFF  130
#define SUB_HIGHPASS_NODE1 60

#define SUB_HP_NODE1_STEP  1
#define SUB_HP_NODE2_STEP  5

#define LINEOUT_HIGHPASS_MIN 30
#define LINEOUT_HIGHPASS_MAX 300
/*special value to indicate Thru according to the webpage handler*/
#define LINEOUT_HIGHPASS_DEFAULT 330
#define LINEOUT_HIGHPASS_NODE1 60
#define LINEOUT_HIGHPASS_NODE2 120

#define SUB_DELAY_MAX 250

#define DEFAULTT_SOURCE        RCA 

#define POWER_ON_METHOD_DEFAULT   AUDIO

#define DSP_DELAY_DEFAULT_ROUTER  DELAY_OFF

#define LCD_BRIGHTNESS_DEFAULT BRI_MEDIUM

#define DSP_DEFAULT_PRESET PRE_NORMAL

#define SUB_DEFAULT_MODE   BRONZE_IWORIC

#define CODE_NUM 3

#define EQ_FREQ_MIN  20
#define EQ_FREQ_DEFAULT  100
#define EQ_FREQ_MAX  300
#define EQ_GAIN_MIN  (-18)
#define EQ_GAIN_MAX    12
#define EQ_Q_MIN     3
#define EQ_Q_MAX     120


#if 1
extern void s_printf(const char *fmt, ...);
#define SYS_TRACE s_printf
#else
#define SYS_TRACE(...)
#endif


#if 1
#define OSAL_ASSERT(x) \
if(!(x)){\
	SYS_TRACE("(%s) has assert failed at %s.\n", #x, __FUNCTION__);\
  while(1);\
}
#else

#define OSAL_ASSERT(...)

#endif



#endif


