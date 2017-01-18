/****************************************************************************//**
  \file rf4ceFlash.h

  \brief Declaration RF4CE request primitives.

  \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2011, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

  \internal
  History:
    31.01.11 A. Khromykh - Created
*******************************************************************************/
#ifndef _RF4CEFLASH_H
#define _RF4CEFLASH_H

/******************************************************************************
                   Types section
******************************************************************************/
/* The necessary list of functions ID for rf4ce */
typedef enum
{
  FILL_PAGE_BUFFER,
  PROGRAM_PAGE,
  SWAP_IMAGE,
  ERASE_PAGE,
  WRITE_PAGE,
  MAX_FUNCTION_NUMBER
} rf4ce_func_identity_t;

/* Parameters for filling page buffer */
typedef struct
{
  uint16_t start_offset;
  uint16_t length;
  uint8_t *data;
} fill_page_buffer_req_t;

/* Parameters for programming page */
typedef struct
{
  uint32_t page_start_addr;
} program_page_req_t;

/* Parameters for swapping image */
typedef struct
{
  uint32_t image_start_addr;
  uint32_t image_size;
} swap_images_req_t;

/* Parameters for erasing page */
typedef struct
{
  uint32_t page_start_addr;
} erase_page_req_t;

/* Parameters for writing page */
typedef struct
{
  uint32_t page_start_addr;
} write_page_req_t;

/* Common memory for all requests */
typedef union
{
  fill_page_buffer_req_t fill_page_buffer_req;
  program_page_req_t     program_page_req;
  swap_images_req_t      swap_images_req;
  erase_page_req_t       erase_page_req;
  write_page_req_t       write_page_req;
} common_boot_req_t;

/* Request structure for boot functionality */
typedef struct
{
  rf4ce_func_identity_t request_type;
  common_boot_req_t     common_boot_req;
} rf4ce_boot_req_t;

/* Prototype of the entry point to bootloader. */
typedef void (* boot_entry_point_t)(void);

#endif /* _RF4CEFLASH_H */
