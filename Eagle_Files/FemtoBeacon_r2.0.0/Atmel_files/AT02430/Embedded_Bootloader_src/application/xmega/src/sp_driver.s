/**************************************************************************//**
  \file  sp_driver.s

  \brief Implementation of self programming functions.

  \author
      Atmel Corporation: http://www.atmel.com \n
      Support email: avr@atmel.com

    Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
    Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
    History:
      28/07/10 A. Khromykh - Created
*******************************************************************************/
/**************************************************************************//**
\internal
        Note on GCC calling convention:
        Scratch registers:   R18-R27, R30-R31
        Preserved registers: R2-R17, R28-R29
        Parameter registers: R8-R25 (2-,4-, or 8- byte alignment)
        Return registers:    R18-R25 (up to 64-bit)
*******************************************************************************/
/******************************************************************************
                   Define(s) section
******************************************************************************/
.set RAMPZ,                            0x003B         ; RAMPZ address
.set NVM_CMD_LOAD_FLASH_BUFFER_gc,     0x23           ; Load Flash page buffer
.set NVM_CMD_ERASE_WRITE_APP_PAGE_gc,  0x25           ; Erase-and-write Application Section page
.set NVM_CMD,                          0x01CA         ; Address of NVM CMD register
.set NVM_INTCTRL,                      0x01CD         ; Address of NVM INTCTRL register
.set NVM_STATUS,                       0x01CF         ; Address of NVM STATUS register
.set CCP_SPM_gc,                       0x9D           ; SPM Instruction Protection
.set CCP,                              0x0034         ; CCP address
.set NVM_NVMBUSY_bp,                   7              ; bit position BUSY into STATUS register
.set SLEEP_SEN_bm,                     0x01           ; sleep enable bit mask
.set SLEEP_CTRL,                       0x0048         ; sleep CTRL register
.set NVM_SPMLVL_HI_gc,                 0x0C           ; enable high level spm interrupt

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief This routine writes the word from R23:R22 into the Flash page buffer at
 address R25:R24.

\param
     R25:R24 - Byte address into Flash page.
     R23:R22 - Word to write.

\return
     Nothing.
******************************************************************************/
.section .text
.global SP_LoadFlashWord
SP_LoadFlashWord:
    push    r20
    push    r0
    push    r1
    push    r31
    push    r30
    movw    r0,  r22                           ; Prepare flash word in R1:R0.
    ldi     r20, NVM_CMD_LOAD_FLASH_BUFFER_gc  ; Prepare NVM command in R20.
    movw    r30, r24                           ; Load R25:R24 into Z(R31:R30).
    sts     NVM_CMD, r20                       ; Load prepared command into NVM Command register.
    ldi     r20, CCP_SPM_gc                    ; Prepare Protect SPM signature in R20
    sts     CCP, r20                           ; Enable SPM operation (this disables interrupts for 4 cycles).
    spm                                        ; Self-program.
  waitForSPM:
    lds     r20, NVM_STATUS                    ; Load the NVM Status register.
    sbrc    r20, NVM_NVMBUSY_bp                ; Check if bit is cleared.
    rjmp    waitForSPM                         ; Repeat check if bit is not cleared.
    clr     r20                                ; Clear r20
    sts     NVM_CMD, r20                       ; Clear up command register to NO_OPERATION.
    pop     r30
    pop     r31
    pop     r1
    pop     r0
    pop     r20
    ret

/**************************************************************************//**
\brief This routine erases first and then writes the page buffer to the
 Flash page at address R25:R24:R23:R22 in the application section. The address
 can point anywhere inside the page.

\param
     R25:R24:R23:R22 - Byte address into Flash page.

\retrun
     Nothing.
******************************************************************************/
.section .text
.global SP_EraseWriteApplicationPage
SP_EraseWriteApplicationPage:
    push    r20
    push    r19
    push    r18
    push    r31
    push    r30
    in      r19, RAMPZ                            ; Save RAMPZ, which is restored in SP_CommonSPM.
    out     RAMPZ, r24                            ; Load RAMPZ with the MSB of the address.
    movw    r30, r22                              ; Move low bytes of address to ZH:ZL from R23:R22
    ldi     r20, SLEEP_SEN_bm                     ; Prepare sleep enable & IDLE mode in R20.
    sts     SLEEP_CTRL, r20                       ; load sleep enable and sleep mode to CTRL
    ldi     r18, NVM_SPMLVL_HI_gc                 ; Prepare high level spm interrupt
    ldi     r20, NVM_CMD_ERASE_WRITE_APP_PAGE_gc  ; Prepare NVM command in R20.
    sts     NVM_CMD, r20                          ; Load prepared command into NVM Command register.
    ldi     r20, CCP_SPM_gc                       ; Prepare Protect SPM signature in R20
    sts     CCP, r20                              ; Enable SPM operation (this disables interrupts for 4 cycles).
    spm                                           ; Self-program.
    sts     NVM_INTCTRL, r18                      ; Enable spm interrupt
    sleep                                         ; Sleep
    clr     r18                                   ; Clear r18
    sts     SLEEP_CTRL, r18                       ; Clear sleep control register
    out     RAMPZ, r19                            ; Restore RAMPZ register.
    pop     r30
    pop     r31
    pop     r18
    pop     r19
    pop     r20
    ret

; END OF FILE

