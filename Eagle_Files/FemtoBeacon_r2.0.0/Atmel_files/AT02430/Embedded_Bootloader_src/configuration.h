#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#define NO 0x01
#define YES 0x02

#define AT25F2048  0x01
#define AT45DB041  0x02
#define AT25DF041A 0x03

#define CRYSTAL_12MHz 0x01
#define CRYSTAL_16MHz 0x02
#define CRYSTAL_18d432MHz 0x03

// Use external memory
//#define EXTERNAL_MEMORY AT25F2048
#define EXTERNAL_MEMORY AT45DB041
//#define EXTERNAL_MEMORY AT25DF041A

// Rcb_bb RS-232 controller
#define RCB_BB_RS232_CONTROLLER NO
//#define RCB_BB_RS232_CONTROLLER YES

// Key remote RS-232 controller
#define KEY_REMOTE_RS232_CONTROLLER NO
//#define KEY_REMOTE_RS232_CONTROLLER YES

// MCU crystal frequency for Sam3s
#define CRYSTAL_FREQUENCY CRYSTAL_12MHz
//#define CRYSTAL_FREQUENCY CRYSTAL_16MHz
//#define CRYSTAL_FREQUENCY CRYSTAL_18d432MHz

//-----------------------------------------------
//ATMEGA128RFA1
//-----------------------------------------------
#ifdef ATMEGA128RFA1
  // Use USART0
  #define USE_USART0 0
  //#define USE_USART0 1
  
  // Use USART1
  #define USE_USART1 1
  //#define USE_USART1 0
  
  // Use USB_FIFO
  #define USE_USB_FIFO 0
  //#define USE_USB_FIFO 1

  // Use TWI
 // #define USE_TWI 0
  #define USE_TWI 1
#endif

//-----------------------------------------------
//AT91SAM3S4C
//-----------------------------------------------
#ifdef AT91SAM3S4C
  // Use USB DFU class
  #define USE_USB_DFU 1
  //#define USE_USB_DFU 0
  
  // Rcb_bb RS-232 controller
  #undef RCB_BB_RS232_CONTROLLER
  #define RCB_BB_RS232_CONTROLLER NO
  
  // Key remote RS-232 controller
  #undef KEY_REMOTE_RS232_CONTROLLER
  #define KEY_REMOTE_RS232_CONTROLLER NO
#endif

//-----------------------------------------------
//ATMEGA1281
//-----------------------------------------------
#ifdef ATMEGA1281
  // Use USART1
  #define USE_USART1 1
  //#define USE_USART1 0
  
  // Use USART0
  #define USE_USART0 0
  //#define USE_USART0 1
  
  // Use USB_FIFO
  #define USE_USB_FIFO 0
  //#define USE_USB_FIFO 1

  // Use TWI
  //#define USE_TWI 0
  #define USE_TWI 1

  // Used board is Mesh Bean Board 
  //#define MESHBEAN_BOARD NO
  #define MESHBEAN_BOARD YES
#endif

//-----------------------------------------------
//ATXMEGA256D3
//-----------------------------------------------
#ifdef ATXMEGA256D3
  // Use USARTD0
  #define USE_USARTD0 1
  //#define USE_USARTD0 0
  
  // Key remote RS-232 controller
  #undef KEY_REMOTE_RS232_CONTROLLER
  #define KEY_REMOTE_RS232_CONTROLLER NO
  
  // Rcb_bb RS-232 controller
  #undef RCB_BB_RS232_CONTROLLER
  #define RCB_BB_RS232_CONTROLLER NO

  // Use TWIC
  //#define USE_TWIC 0
  #define USE_TWIC 1

#endif

//-----------------------------------------------
//ATMEGA2561
//-----------------------------------------------
#ifdef ATMEGA2561
  // Use USART1
  #define USE_USART1 1
  //#define USE_USART1 0
  
  // Use USART0
  #define USE_USART0 0
  //#define USE_USART0 1

  // Use TWI
  //#define USE_TWI 0
  #define USE_TWI 1
#endif

//-----------------------------------------------
//ATXMEGA256A3
//-----------------------------------------------
#ifdef ATXMEGA256A3
  // Use USARTF0
  #define USE_USARTF0 1
  //#define USE_USARTF0 0
  
  // Use USE_SPIE
  #define USE_SPIE 0
  //#define USE_SPIE 1
  
  // Key remote RS-232 controller
  #undef KEY_REMOTE_RS232_CONTROLLER
  #define KEY_REMOTE_RS232_CONTROLLER NO
  
  // Use USARTD0
  #define USE_USARTD0 1
  //#define USE_USARTD0 0
  
  // Rcb_bb RS-232 controller
  #undef RCB_BB_RS232_CONTROLLER
  #define RCB_BB_RS232_CONTROLLER NO

  // Use TWIC
  #define USE_TWIC 0
  //#define USE_TWIC 1

  // Use TWIE
  //#define USE_TWIE 0
  #define USE_TWIE 1

  // Used board is REB-CBB Board 
  //#define REB_CBB_BOARD NO
  #define REB_CBB_BOARD YES
#endif

//-----------------------------------------------
//ATXMEGA128A1
//-----------------------------------------------
#ifdef ATXMEGA128A1
  // Key remote RS-232 controller
  #undef KEY_REMOTE_RS232_CONTROLLER
  #define KEY_REMOTE_RS232_CONTROLLER NO
  
  // Use USE_SPIE
  #define USE_SPIE 0
  //#define USE_SPIE 1
  
  // Use USARTD0
  #define USE_USARTD0 1
  //#define USE_USARTD0 0
  
  // Rcb_bb RS-232 controller
  #undef RCB_BB_RS232_CONTROLLER
  #define RCB_BB_RS232_CONTROLLER NO
  
  // Use USARTF0
  #define USE_USARTF0 1
  //#define USE_USARTF0 0

  // Use TWIC
  #define USE_TWIC 0
  //#define USE_TWIC 1

  // Use TWID
  #define USE_TWID 0
  //#define USE_TWID 1

  // Use TWIE
  #define USE_TWIE 0
  //#define USE_TWIE 1

  // Use TWIE
  //#define USE_TWIF 0
  #define USE_TWIF 1
#endif


#endif // _CONFIGURATION_H_
