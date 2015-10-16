#include "common_head.h"
#include "dsp_reg.h"
#include "dsp_app.h"
#include "api_doc.h"
#include "cs485xx.h"



#define FILTER_BYPASS(addr,offset) {\
	unsigned char j;\
	unsigned int  RegValue;\
	unsigned int RegAddr;\
	for(j=0; j< 12; j++)\
	{\
		if(j % 3 == 0)\
		{\
			 RegValue = 0;\
		}\
		else if(j % 3 == 2)\
		{\
			RegValue = 0xFFFFFFFF;\
		}\
		else \
		{ \
			continue;\
		} \
		RegAddr = (addr | 0xF0000000) + j;\
		Dsp0_WriteRegister(RegAddr,RegValue);\
		if(offset != 0){ \
		Dsp0_WriteRegister(RegAddr+offset,RegValue);\
		}\
	}\
}


void dsp_master_vol_set(int gain)
{
	uint8 index;
	uint32 RegAddr = 0;
  uint32 RegValue=0;
	
	index = MASTER_VOL_INDEX(gain);
	
	OSAL_ASSERT(index < MASTER_VOL_MAX_INDEX);
	
	RegAddr = (_g_1_Master_Volume | 0xF0000000);
	RegValue = c_master_gain[index];
	
	SYS_TRACE("RegValue = %x\r\n", RegValue);
	Dsp0_WriteRegister(RegAddr,RegValue);
	
	/*mute RCA*/
	if(index == 0){
		RegAddr = (_g_2_Output_level | 0xF0000000);
	  RegValue = c_master_gain[index];
		
		Dsp0_WriteRegister(RegAddr,RegValue);
		Dsp0_WriteRegister(RegAddr+1,RegValue);
	}else{
		RegAddr = (_g_2_Output_level | 0xF0000000);
	 // RegValue = 0x23165907;0.8
		RegValue = 0x211fe21d;//0.3
		Dsp0_WriteRegister(RegAddr,RegValue);
		Dsp0_WriteRegister(RegAddr+1,RegValue);
	}
	
}


void dsp_subLpFre_set(uint8 slope, uint16 freq, uint8 num, int offset1, int offset2)
{
	uint8 index, j;
	uint32  RegAddr;
	uint32  RegValue;
	
  for(index = 0; index < num; index++)
	{
		if(slope == FILTER_SLOPE_48DB)
		{
      RegValue = filter_48db[freq][index];
		}
	  else if(slope == FILTER_SLOPE_24DB)
	  {
			RegValue = filter_24db[freq][index];
		}
		else if(slope == FILTER_SLOPE_12DB)
		{
		  RegValue = filter_12db[freq][index];
		}
			 
		RegAddr = (_a_1_LP_Sub_48db | 0xf0000000) + index ;
		SYS_TRACE("will set reg0x[%x] and reg0x[%x] val=0x[%x]\n", RegAddr + offset1,RegAddr + offset2, RegValue);
		Dsp0_WriteRegister(RegAddr + offset1,RegValue);
		if(offset1 != offset2){
			Dsp0_WriteRegister(RegAddr + offset2,RegValue);
		}
	 }	
	 if(slope == FILTER_SLOPE_24DB){  
		    for(j = index; j < 2*index; j++)
				{
						if(j % 3 == 0)
						{
							  RegValue = 0;
						}
						else if(j % 3 == 1)
						{  
							 RegValue = filter_24db[freq][1];
						}
						else
						{
							 RegValue = 0xFFFFFFFF;  
						}
						
						RegAddr = (_a_1_LP_Sub_48db | 0xf0000000) + j ;
						SYS_TRACE("will set reg0x[%x] and reg0x[%x] val=0x[%x]\n", RegAddr + offset1,RegAddr + offset2, RegValue);
						Dsp0_WriteRegister(RegAddr + offset1,RegValue);
						if(offset1 != offset2){
							Dsp0_WriteRegister(RegAddr + offset2,RegValue);
						}
				}	
	 }else if(slope == FILTER_SLOPE_12DB){
		  for(j = index; j < 3*index; j++){
				if(j % 3 == 0)
				{
					RegValue = 0;
				}
				else if(j % 3 == 1)
				{  
					RegValue =   filter_12db[freq][1];
				}
				else
				{
					RegValue = 0xFFFFFFFF;  
				}	
				RegAddr = (_a_1_LP_Sub_48db | 0xf0000000) + j ;
				SYS_TRACE("will set reg0x[%x] and reg0x[%x] val=0x[%x]\n", RegAddr + offset1,RegAddr + offset2, RegValue);
				Dsp0_WriteRegister(RegAddr + offset1,RegValue);
				if(offset1 != offset2){
					Dsp0_WriteRegister(RegAddr + offset2,RegValue);
				}
				}
	 }
	
}

void dsp_subHpFreq_set(uint8 slope, uint16 freq, uint8 num, uint8 offset)
{
	 uint8 index, j;
	 uint32  RegAddr;
	 uint32  RegValue; 
	
	 for(index = 0; index < num; index++){
		if(slope == FILTER_SLOPE_48DB)
		{
			RegValue = filter_48db[freq][index];
		}
		else if(slope == FILTER_SLOPE_24DB)
		{
		  RegValue = filter_24db[freq][index]; 
		}
		else if(slope == FILTER_SLOPE_12DB)
		{
			RegValue = filter_12db[freq][index];   
		}
		 
		RegAddr = (_a_1_HP_Sub_48db | 0xf0000000) + index;
		SYS_TRACE("will set reg0x[%x] val=0x[%x]\n", RegAddr, RegValue);
		Dsp0_WriteRegister(RegAddr,RegValue);
	//	Dsp0_WriteRegister(RegAddr + offset,RegValue);
	 } 
	 
	 if(slope == FILTER_SLOPE_24DB)
	 {
		   for(j = index; j < 2*index; j++)
		 {
			   if(j % 3 == 0)
				 {
					  RegValue = 0; 
				 }
				 else if(j % 3 == 1)
				 {
					 RegValue = filter_24db[freq][1]; 
				 }
				 else
				 {
					   RegValue = 0xFFFFFFFF;
				 }
				 
				  RegAddr = (_a_1_HP_Sub_48db | 0xf0000000) + j;
		      SYS_TRACE("will set reg0x[%x] val=0x[%x]\n", RegAddr, RegValue);
		      Dsp0_WriteRegister(RegAddr,RegValue);
		    //  Dsp0_WriteRegister(RegAddr + offset,RegValue);
		 }
	 }
	 else if(slope == FILTER_SLOPE_12DB)
	 {
		 for(j = index; j < 3*index; j++)
		 {
			   if(j % 3 == 0)
				 {
					  RegValue = 0; 
				 }
				 else if(j % 3 == 1)
				 {
					 RegValue = filter_12db[freq][1]; 
				 }
				 else
				 {
					   RegValue = 0xFFFFFFFF;
				 }
				 
				  RegAddr = (_a_1_HP_Sub_48db | 0xf0000000) + j;
		      SYS_TRACE("will set reg0x[%x] val=0x[%x]\n", RegAddr, RegValue);
		      Dsp0_WriteRegister(RegAddr,RegValue);
		     // Dsp0_WriteRegister(RegAddr + offset,RegValue);
		 } 
	 }
	
}


void dsp_sub_hp_bypass(void)
{
	 FILTER_BYPASS(_a_1_HP_Sub_48db, 0);
}

void dsp_sub_lp_bypass(void)
{
	 FILTER_BYPASS(_a_1_LP_Sub_48db, 0);
}

void dsp_lineout_hp_bypass(void)
{
	 FILTER_BYPASS(_a_1_HP_PT_48db,0)
	 FILTER_BYPASS(_a_1_HP_PT_48db,0x10)
}

void dsp_delay_time_set(uint8 time)
{
	  uint8 value[4] = {0};
    uint8 temp;
    uint32  RegAddr;
	  uint32  RegValue;

    temp = time % 10;
    if(temp) value[1] = 0x80;
    value[2] = time / 10;
    
    RegAddr = (0xF0000000 | _delay_value_SUB_Delay);
    RegValue = *(unsigned int *)value;

    Dsp0_WriteRegister(RegAddr,RegValue);
    Dsp0_WriteRegister(RegAddr + 0x03,RegValue);
}

void dsp_delay_switch_to_sub(void)
{
	uint32  RegAddr = (0xF0000000 | _output_select_1_2x1_Router);
	uint32  RegValue = 1;
	
	Dsp0_WriteRegister(RegAddr,RegValue);
	
	RegAddr = (0xF0000000 | _output_select_1_Dealay_Router_Sub);
	RegValue = 2;
	Dsp0_WriteRegister(RegAddr,RegValue);
	
	/*cut off line*/
	RegAddr = (0xF0000000 | _output_select_1_Dealay_Router_Line);
	RegValue = 2;
	Dsp0_WriteRegister(RegAddr,RegValue);
	Dsp0_WriteRegister(RegAddr + 0x04,RegValue);
}


void dsp_delay_switch_to_line(void)
{
	uint32  RegAddr = (0xF0000000 | _output_select_1_2x1_Router);
	uint32  RegValue = 2;
	
	Dsp0_WriteRegister(RegAddr,RegValue);
	
	RegAddr = (0xF0000000 | _output_select_1_Dealay_Router_Line);
	RegValue = 1;
	Dsp0_WriteRegister(RegAddr,RegValue);
	Dsp0_WriteRegister(RegAddr + 0x04,RegValue);
	
	/*cut off sub*/
	RegAddr = (0xF0000000 | _output_select_1_Dealay_Router_Sub);
	RegValue = 1;
	
	Dsp0_WriteRegister(RegAddr,RegValue);
}


void dsp_delay_switch_off(void)
{
  uint32 RegAddr = (0xF0000000 | _output_select_1_Dealay_Router_Sub);
	uint32 RegValue = 1;
	
	Dsp0_WriteRegister(RegAddr,RegValue);
	
	RegAddr = (0xF0000000 | _output_select_1_Dealay_Router_Line);
	RegValue = 2;
	Dsp0_WriteRegister(RegAddr,RegValue);
	Dsp0_WriteRegister(RegAddr + 0x04,RegValue);
}

void dsp_eq_set_freq(uint32 a, uint8 offset, uint8 type)
{
	 uint32 RegAddr;
	 uint32 RegValue = a;
	
	 if(TYPE_ROOM_EQ == type){
		 RegAddr = (0xF0000000 | _a_11_1x6_PEQ_Preset) + offset;
	 }else if(TYPE_SUB_EQ ==type){
		 RegAddr = (0xF0000000 | _a_11_1x8_PEQ_Preset) + offset;
	 }else{
		 RegAddr = (0xF0000000 | _a_11_1x2_PEQ_Preset) + offset;
	 }
	
	 SYS_TRACE("dsp_eq_set_freq:[%x]-[%x]\r\n", RegAddr, RegValue);
	 Dsp0_WriteRegister(RegAddr, RegValue);
}

void dsp_eq_set_gain(uint32 g, uint8 offset, uint8 type)
{
	uint32 RegAddr;
	uint32 RegValue = g;
	
	if(TYPE_ROOM_EQ == type){
		 RegAddr = (0xF0000000 | _g_11_1x6_PEQ_Preset) + offset;
	}else if(TYPE_SUB_EQ == type){
		 RegAddr = (0xF0000000 | _g_11_1x8_PEQ_Preset) + offset;
	 }else{
		 RegAddr = (0xF0000000 | _g_11_1x2_PEQ_Preset) + offset;
	 }

	SYS_TRACE("dsp_eq_set_gain:[%x]-[%x]\r\n", RegAddr, RegValue);
	Dsp0_WriteRegister(RegAddr, RegValue);
}


static void do_eq_width_filter(uint8 *q)
{
	 uint8 table[] = {1,2,4,6,8,10,15,20,40,60,80,120};
   uint8 index;
   uint8 temp;

   for(index = 0; index < sizeof(table); index++)
  {
	    if(*q == table[index]) return;
		  else
			{
				 if(*q > table[index])
 				 {
					  temp = table[index];
				 }					 
			}
				
  }

   *q = temp;
}

void dsp_eq_set_width(uint32 b, uint8 offset, uint8 type)
{
	uint32 RegAddr;
  uint32 RegValue = b;
	
	if(TYPE_ROOM_EQ == type){
		 RegAddr = (0xF0000000 | _b_11_1x6_PEQ_Preset) + offset;
	}else if(TYPE_SUB_EQ == type){
		 RegAddr = (0xF0000000 | _b_11_1x8_PEQ_Preset) + offset;
	 }else{
		 RegAddr = (0xF0000000 | _DspSampleRate_1x2_PEQ_Preset) + offset;
	 }

  SYS_TRACE("dsp_eq_set_width:[%x]-[%x]\r\n", RegAddr, RegValue);
	Dsp0_WriteRegister(RegAddr, RegValue);
}


void dsp_eq_off(void)
{
	uint32 RegAddr = (0xF0000000 | _value_On_Off_Preset);
  uint32 RegValue = 0;
	
	Dsp0_WriteRegister(RegAddr, RegValue);
}

void dsp_eq_on(void)
{
	uint32 RegAddr = (0xF0000000 | _value_On_Off_Preset);
  uint32 RegValue = 1;
	
	Dsp0_WriteRegister(RegAddr, RegValue); 
}

void dsp_phrase_off(void)
{
	uint32 RegAddr = (0xF0000000 | _output_select_1_Phase_Router);
  uint32 RegValue = 1;

  Dsp0_WriteRegister(RegAddr, RegValue); 
}

void dsp_phrase_on(void)
{
	uint32 RegAddr = (0xF0000000 | _output_select_1_Phase_Router);
  uint32 RegValue = 2;

  Dsp0_WriteRegister(RegAddr, RegValue);  
}

