#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <EEPROM.h>


// define hardware pins for this controller
// sticks
#define Left_Hori         PC0
#define Left_Verti        PC1
#define Right_Hori        PC2
#define Right_Verti       PC3
// encoders
#define Left_Trim_Data    PD0
#define Right_Trim_Data   PD1
#define Left_Trim_Clk     PD2
#define Right_Trim_Clk    PC4
#define Left_Trim_Clr     PB0
#define Right_Trim_Clr    PB1
// buttons
#define SW0               PD4
#define SW1               PD5
#define SW2               PD6
#define SW3               PD7

// SPI pins
#define Chip_Select       PB2
#define Chip_Enable       PC5
#define MOSI              PB3
#define MISO              PB4
#define SCK               PB5
#define NRF_INT           PD3

// Masks
#define Button_Mask       0xF0


// check the mode that the user picked
#ifndef Controller_Mode
#define Controller_Mode 2     // mode 2 controller as default
#endif
#if Controller_Mode < 1 || Controller_Mode > 4
#undef Controller_Mode
#define Controller_Mode 2     // if invalid mode, use mode 2
#endif

// also the buttons
#ifndef Exponential
#define Exponential     SW3
#endif
#ifndef Halve
#define Halve           SW0
#endif
#ifndef Magic0
#define Magic0          SW2
#endif
#ifndef Magic1
#define Magic1          SW1
#endif


// configure the pins according to specific mode
#if Controller_Mode==1
#define Throttle        Right_Verti
#define Rudder          Left_Hori
#define Elevator        Left_Verti
#define Aileron         Right_Hori
#define Rudder_Trim_Data  Left_Trim_Data
#define Rudder_Trim_Clk   Left_Trim_Clk
#define Rudder_Trim_Clr   Left_Trim_Clr
#define Aileron_Trim_Data Right_Trim_Data
#define Aileron_Trim_Clk  Right_Trim_Clk
#define Aileron_Trim_Clr  Right_Trim_Clr

#elif Controller_Mode==2
#define Throttle        Left_Verti
#define Rudder          Left_Hori
#define Elevator        Right_Verti
#define Aileron         Right_Hori
#define Rudder_Trim_Data  Left_Trim_Data
#define Rudder_Trim_Clk   Left_Trim_Clk
#define Rudder_Trim_Clr   Left_Trim_Clr
#define Aileron_Trim_Data Right_Trim_Data
#define Aileron_Trim_Clk  Right_Trim_Clk
#define Aileron_Trim_Clr  Right_Trim_Clr

#elif Controller_Mode==3
#define Throttle        Right_Verti
#define Rudder          Right_Hori
#define Elevator        Left_Verti
#define Aileron         Left_Hori
#define Rudder_Trim_Data  Right_Trim_Data
#define Rudder_Trim_Clk   Right_Trim_Clk
#define Rudder_Trim_Clr   Right_Trim_Clr
#define Aileron_Trim_Data Left_Trim_Data
#define Aileron_Trim_Clk  Left_Trim_Clk
#define Aileron_Trim_Clr  Left_Trim_Clr

#elif Controller_Mode==4
#define Throttle        Left_Verti
#define Rudder          Right_Hori
#define Elevator        Right_Verti
#define Aileron         Left_Hori
#define Rudder_Trim_Data  Right_Trim_Data
#define Rudder_Trim_Clk   Right_Trim_Clk
#define Rudder_Trim_Clr   Right_Trim_Clr
#define Aileron_Trim_Data Left_Trim_Data
#define Aileron_Trim_Clk  Left_Trim_Clk
#define Aileron_Trim_Clr  Left_Trim_Clr

#endif


// EEPROM Addresses
#define Address_Rudder      0x10
#define Address_Elevator    0x12

struct Data_Package {
  uint16_t mode;
  uint16_t throttle;
  int16_t aileron;
  int16_t rudder;
  int16_t elevator;
  uint8_t magic_code0;
  uint8_t magic_code1;
};

extern Data_Package package;

extern volatile bool doExp, doHalve;

extern volatile int8_t offset_aileron, offset_throttle, offset_rudder, offset_elevator;
extern volatile uint8_t button_status;   // [7:4]={SW3,SW2,SW1,SW0}
