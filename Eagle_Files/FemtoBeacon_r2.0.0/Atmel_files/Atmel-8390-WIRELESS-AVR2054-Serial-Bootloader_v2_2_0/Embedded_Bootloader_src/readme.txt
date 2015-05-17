Bootloader. Revision 2.1

Supported devices:
         MCU                                           external flash memory          RF4CE boot func  
atxmega128a1  (not tested):                                  nothing                         no 
atxmega256a3  (both compilers):                              at45db041                       no 
atmega1281    (both compilers):                              at25f2048, at45db041            yes
atmega2561    (both compilers):                              nothing                         no 
atmega128rfa1 (both compilers):                              at25f2048, at45db041            yes
atmega256rfr2 (both compilers):                              AT25DF041A                      yes
atxmega256d3  (both compilers):                              at45db041                       no

Fuse bytes for xmega:            (with and without OTAU)
FUSEBYTE0                                 0xFF
FUSEBYTE1                                 0x00
FUSEBYTE2                                 0xBF
FUSEBYTE4                                 0xFE
FUSEBYTE5                                 0xFF

Fuse bytes for atmega:       (without OTAU)     (with OTAU)     (with RF4CE boot functions)
EXTENDED                        0xFF              0xFF                    0xFF  
HIGH                            0x9C              0x9A                    0x9A 
LOW                             0x62              0x62                    0x62

Fuse bytes for atmega128rfa1/256rfr2:(without OTAU)     (with OTAU)     (with RF4CE boot functions)
EXTENDED                                  0xFE              0xFE                    0xFE   
HIGH                                      0x9C              0x9A                    0x9A
LOW                                       0x62              0x62                    0x62

Revision history:

1.0 - Initial revision.

1.1 - ATmega1281\2561 is supported.

1.2 - SPI serializer support is added for xMega, with the following parameters:
 - frequency is less or equal to 200kHz
 - start of SPI clock after slave select greater or equal to 100ns
 - delay between consecutive transfers greater or equal to 100us

spiFlasher is added to test the SPI part of the embedded bootloader (valid for the SAM7X evaluation board only).
Hardware sources used by spiFlasher:
 - IRQ1 - interrupt line for the slave data ready station (A15 on S3)
 - SPI1 - SPI interface (clock - A23, mosi - A24, miso - A25, ss - A26 on S3)

1.3 - Loading from external flash is supported for ATmega1281. The AT25f2048 external SPI flash is used. 
USART0 is used in MSPI mode; pin F3 is set as chip select. Size of boot section is 4 kbytes.

1.4 - Service area are increased (by 1 byte) for the external flash loader.

1.5 - ATmega128rfa1 is supported.

1.6 - ATxmega256d3 is supported. AT45db041 can be used for the bootloader with OTAU support.
xMega(ATxmega256a\d3) supports external flash. Both MCUs use SPI on port D to connect to external flash.
ATmega1281 and ATmega128rfa1 support AT25f2058 and AT45db041.
ATxmega256a3 and ATxmega256d3 support AT45db041.
Workaround is introduced for the following problem with xMega:
"Writing EEPROM or Flash while reading any of them will not work".

1.7 - Functions for RF4CE OTAU are added (only for ATmega1281 and ATmega128rfa1 and compilation with GCC)

1.8 - Start procedure was refactored: several interfaces are supported in a single
bootloader assembly; all interfaces scan for host's activity for 200ms by turn with 
one interface becoming main when host's activity is detected. 
IAR support is dropped.

2.0 - IAR projects are restored. AVR Studio 5 projects are added.

AT91SAM3S4C is supported, and the device firmware upgrade class of usb is supported.
Cortex bootloader is placed in the last 4kb of the internal flash memory (exclude vectors). Vectors are eliminated upon the
first image downloading (exclude reset vector). Bootloader stores the reset vector of the user's application and runs it 
by itself. A user's application can initiate a bootloading process by the use of the DFU application part from the BitCloud package 
or by writing the key byte 0x5A into the fifth byte to the end of the internal flash memory. Besides, the restoring mode 
can be initiated by connecting PB7 to the ground and turning it off\on. To load an image to a device the user can use
Bootloader PC Tool from the BitCloud package or dfu-util.exe (an open source utility, with max packet size equal to 64 bytes).

2.2 - ATmega256RFR2 device support,Atmel Studio 6 projects, Bootloading through TWI interface are added.

TWI interface is added for bootloading. The TWI boot loader allows loading of firmware images to target devices from Bootloader PC tool over the TWI interface.TWI cannot be used for direct communication with PC, an XMEGA-A1 Xplained kit is used in between for bridge functionality. The TWI module on ATxmega128A1 of XMEGA-A1 Xplained kit is configured in TWI master mode and the device running the TWI boot loader is configured in TWI slave mode. ATmega256RFR2 device support is added with projects in Atmel Studio 6 and IAR compiler. RF4CE, OTAU project support is also added for ATmega256RFR2 device. AVR Studio 4 and AVR Studio 5 projects are removed. Atmel Studio 6 projects are created for all supported devices    