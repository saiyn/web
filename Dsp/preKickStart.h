/***********************************************
This file is created by cirrus dsp tool on 09/18/15 12:02:00
This tool was written by XuShoucai on 06/01/09
************************************************/
static const unsigned char PREKICKSTART_CFG [] = 
{
/* preKickStart.cfg */
//include preKickStart\crd_adc.cfg
// ----------
// ADC (crd_adc) config
// ----------
// prekick for ADC
  0x81, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 
// Set port to detect and calculate sample rate
  0x81, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x00, 
//include preKickStart\dsp48500_dao.cfg
// ----------
// DAO (dsp48500_dao) config
// ----------
// Configure DAO
//    disable SPDIF output
//    DAO parameter A
//       A1 - master output clocks
  0x81, 0x80, 0x00, 0x1C, 0xFF, 0xFF, 0xDF, 0xFF, 
  0x81, 0x80, 0x00, 0x1D, 0xFF, 0xFF, 0xDF, 0xFF, 
//    DAO parameter C
//       C5
  0x81, 0x00, 0x00, 0x2D, 0x00, 0x00, 0x77, 0x13, 
  0x81, 0x00, 0x00, 0x2E, 0x00, 0x00, 0x77, 0x13, 
  0x81, 0x80, 0x00, 0x1C, 0xFF, 0xFF, 0xFF, 0x8F, 
  0x81, 0x40, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x20, 
//    DAO parameter D
//       D0 - I2S format (default)
//    DAO parameter E
//       E0 - low == left channel (default)
//    DAO parameter F
//       F0 - rising edge == valid data (default)
//include preKickStart\system.cfg
// ----------
// CS48520 (system) config
// ----------
//  Set PLL Speed 
  0x81, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x15, 
//include preKickStart\dsp48500_dai.cfg
// ----------
// DAI (dsp48500_dai) config
// ----------
//   DAI parameter A
//       A0 - I2S format (default)
//    DAI parameter B
//       B0 - rising edge (default)
//    DAI parameter C
//       C0 - low == left channel (default)
//    DAI pin and clock mapping
  0x81, 0x00, 0x00, 0x15, 
  0x00, 0x08, 0x00, 0x00, 
//include preKickStart\custom.cfg
};
#define Bytes_of_preKickStart_cfg  80   //(bytes)