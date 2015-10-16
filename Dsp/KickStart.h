/***********************************************
This file is created by cirrus dsp tool on 09/18/15 12:02:00
This tool was written by XuShoucai on 06/01/09
************************************************/
static const unsigned char KICKSTART_CFG [] = 
{
/* KickStart.cfg */
// ----------
// CS48520 (system) config
// ----------
// Kick Start with PLL enabled and Malloc-failure alert
  0x81, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x01, 
};
#define Bytes_of_KickStart_cfg  8   //(bytes)