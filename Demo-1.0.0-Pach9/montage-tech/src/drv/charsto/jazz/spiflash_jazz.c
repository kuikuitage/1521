/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
//#include "sys_cfg.h"
#include <stdlib.h>
#include <string.h>
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mem_manager.h"
#include "sys_regs_jazz.h"
#include "hal_base.h"
#include "hal_dma.h"
#include "drv_dev.h"
#include "../../drvbase/drv_dev_priv.h"
#include "charsto.h"
#include "../charsto_priv.h"
#include "spi.h"

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SPI_FLASH_EON
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPI_FLASH_SST
#define CONFIG_SPI_FLASH_MACRONIX
#define CONFIG_SPI_FLASH_STMICRO
#define CONFIG_SPI_FLASH_ATMEL

//#define SF_DEBUG_ENABLE
#ifdef SF_DEBUG_ENABLE
#define SF_DEBUG_PRINTF OS_PRINTF
#else
#define SF_DEBUG_PRINTF DUMMY_PRINTF
#endif
/***************************************************************
*      Interface to SPI flash
****************************************************************/
typedef struct spi_flash {
  spi_bus_t *p_spi;

  const char  *p_name;

  /* Total flash size */
  u32 size;
  /* Write (page) size */
  u32 page_size;
  /* Erase (sector) size */
  u32 sector_size;

  s32 (*read)(struct spi_flash *p_flash, u32 offset,
          u32 len, void *p_buf);
  s32 (*write)(struct spi_flash *p_flash, u32 offset,
          u32 len, void *p_buf);
  s32 (*erase)(struct spi_flash *p_flash, u32 offset,
          u32 len);
  s32 (*power_down)(struct spi_flash *p_flash);
  s32 (*power_up)(struct spi_flash *p_flash);
  s32 (*enter_sa)(struct spi_flash *p_flash);
  s32 (*exit_sa)(struct spi_flash *p_flash);

} spi_flash_t;

spi_flash_t *spi_flash_probe(spi_bus_t * p_spi);

static inline s32 spi_flash_read(spi_flash_t *p_flash, u32 offset,
    u32 len, void *p_buf)
{
  return p_flash->read(p_flash, offset, len, p_buf);
}

static inline s32 spi_flash_write(spi_flash_t *p_flash, u32 offset,
    u32 len, void *p_buf)
{
  return p_flash->write(p_flash, offset, len, p_buf);
}

static inline s32 spi_flash_erase(spi_flash_t *p_flash, u32 offset,
    u32 len)
{
  return p_flash->erase(p_flash, offset, len);
}

static inline s32 spi_flash_power_down(spi_flash_t *p_flash)
{
    if(p_flash->power_down == NULL)
        return -1;
    return p_flash->power_down(p_flash);
}

static inline s32 spi_flash_power_up(spi_flash_t *p_flash)
{
    if(p_flash->power_up == NULL)
        return -1;
    return p_flash->power_up(p_flash);
}

/***************************************************************
*      SPI flash internal definitions
****************************************************************/

/* Common parameters -- kind of high, but they should only occur when there
 * is a problem (and well your system already is broken), so err on the side
 * of caution in case we're dealing with slower SPI buses and/or processors.
 */
#define SPI_FLASH_PROG_TIMEOUT    2000
#define SPI_FLASH_PAGE_ERASE_TIMEOUT    5000
#define SPI_FLASH_SECTOR_ERASE_TIMEOUT    10000

/* Common commands */
#define CMD_READ_ID   0x9f

#define CMD_READ_ARRAY_SLOW   0x03
#define CMD_READ_ARRAY_FAST   0x0b
#define CMD_READ_ARRAY_LEGACY   0xe8

#define CMD_PAGE_PROGRAM    0x02
#define CMD_WRITE_DISABLE   0x04
#define CMD_READ_STATUS   0x05
#define CMD_WRITE_ENABLE    0x06
#define CMD_POWER_DOWN  0xb9
#define CMD_POWER_UP 0xab

#define CMD_ENSA 0xb1
#define CMD_EXSA 0xc1


/* Common status */
#define STATUS_WIP    0x01

/* Send a single-byte command to the device and read the response */
s32 spi_flash_cmd(spi_bus_t *p_spi, u8 cmd, void *p_response, u32 len);

/*
 * Send a multi-byte command to the device and read the response. Used
 * for flash array reads, etc.
 */
s32 spi_flash_cmd_read(spi_bus_t *p_spi, u8 *p_cmd,
    u32 cmd_len, void *p_data, u32 data_len);

s32 spi_flash_cmd_read_fast(spi_flash_t *p_flash, u32 offset,
    u32 len, void *p_data);

/*
 * Send a multi-byte command to the device followed by (optional)
 * data. Used for programming the flash array, etc.
 */
s32 spi_flash_cmd_write(spi_bus_t *p_spi, u8 *p_cmd, u32 cmd_len,
    void *p_data, u32 data_len);

/*
 * Write the requested data out breaking it up into multiple write
 * commands as needed per the write size.
 */
s32 spi_flash_cmd_write_multi(spi_flash_t *p_flash, u32 offset,
    u32 len, void *p_buf);

/*
 * Enable writing on the SPI flash.
 */
static inline s32 spi_flash_cmd_write_enable(spi_flash_t *p_flash)
{
  return spi_flash_cmd(p_flash->p_spi, CMD_WRITE_ENABLE, NULL, 0);
}

/*
 * Disable writing on the SPI flash.
 */
static inline s32 spi_flash_cmd_write_disable(spi_flash_t *p_flash)
{
  return spi_flash_cmd(p_flash->p_spi, CMD_WRITE_DISABLE, NULL, 0);
}

/*
 * Same as spi_flash_cmd_read() except it also claims/releases the SPI
 * bus. Used as common part of the ->read() operation.
 */
s32 spi_flash_read_common(spi_flash_t *p_flash, u8 *p_cmd,
    u32 cmd_len, void *p_data, u32 data_len);

/*
 * Send the read status command to the device and wait for the wip
 * (write-in-progress) bit to clear itself.
 */
void spi_flash_cmd_wait_ready(spi_flash_t *p_flash, u32 timeout);

/* Erase sectors. */
s32 spi_flash_cmd_erase(spi_flash_t *p_flash, u8 erase_cmd,
      u32 offset, u32 len);

/* Manufacturer-specific probe functions */
spi_flash_t *spi_flash_probe_spansion(spi_bus_t *p_spi, u8 *p_idcode);
spi_flash_t *spi_flash_probe_atmel(spi_bus_t *p_spi, u8 *p_idcode);
spi_flash_t *spi_flash_probe_eon(spi_bus_t *p_spi, u8 *p_idcode);
spi_flash_t *spi_flash_probe_macronix(spi_bus_t *p_spi, u8 *p_idcode);
spi_flash_t *spi_flash_probe_sst(spi_bus_t *p_spi, u8 *p_idcode);
spi_flash_t *spi_flash_probe_stmicro(spi_bus_t *p_spi, u8 *p_idcode);
spi_flash_t *spi_flash_probe_winbond(spi_bus_t *p_spi, u8 *p_idcode);
spi_flash_t *spi_fram_probe_ramtron(spi_bus_t *p_spi, u8 *p_idcode);

/***************************************************************
*      SPI flash interface
****************************************************************/

static void spi_flash_addr(u32 addr, u8 *p_cmd)
{
  /* p_cmd[0] is actual command */
  p_cmd[1] = addr >> 16;
  p_cmd[2] = addr >> 8;
  p_cmd[3] = addr >> 0;
}

s32 spi_flash_cmd(spi_bus_t *p_spi, u8 cmd, void *p_response, u32 len)
{
  return spi_flash_cmd_read(p_spi, &cmd, 1, p_response, len);
}

s32 spi_flash_cmd_read(spi_bus_t *p_spi, u8 *p_cmd,
    u32 cmd_len, void *p_data, u32 data_len)
{
    if(spi_read(p_spi, p_cmd, cmd_len, NULL, p_data, data_len) != SUCCESS)
    {
        return -1;
    }
  else
    {
        return 0;
    }
}

s32 spi_flash_cmd_write(spi_bus_t *p_spi, u8 *p_cmd, u32 cmd_len,
    void *p_data, u32 data_len)
{
    if(spi_write(p_spi, p_cmd, cmd_len, NULL, p_data, data_len) != SUCCESS)
    {
        return -1;
    }
  else
    {
        return 0;
    }
}


s32 spi_flash_cmd_write_multi(spi_flash_t *p_flash, u32 offset,
    u32 len, void *p_buf)
{
  u32 page_addr = 0, byte_addr = 0, page_size = 0;
  u32 chunk_len = 0, actual = 0;
  s32 ret = 0;
  u8 cmd[4];

  page_size = p_flash->page_size;
  page_addr = offset / page_size;
  byte_addr = offset % page_size;

  if (p_flash->p_spi == NULL) {
    SF_DEBUG_PRINTF("SF: unable to claim SPI bus %s %d %s\n",
                      __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  cmd[0] = CMD_PAGE_PROGRAM;
  spi_soft_reset(p_flash->p_spi);
  for (actual = 0; actual < len; actual += chunk_len) {
    chunk_len = MIN(len - actual, page_size - byte_addr);

    cmd[1] = page_addr >> 8;
    cmd[2] = page_addr;
    cmd[3] = byte_addr;

    SF_DEBUG_PRINTF("PP: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x } chunk_len = %d\n",
          (const u8 *)p_buf + actual, cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);

    ret = spi_flash_cmd_write_enable(p_flash);
    if (ret < 0) {
      SF_DEBUG_PRINTF("SF: enabling write failed %s %d %s\n",
                      __FUNCTION__, __LINE__, __FILE__);
      break;
    }

    ret = spi_flash_cmd_write(p_flash->p_spi, cmd, 4,
            (u8 *)p_buf + actual, chunk_len);
    if (ret < 0) {
      SF_DEBUG_PRINTF("SF: write failed %s %d %s\n",__FUNCTION__, __LINE__, __FILE__);
      break;
    }

    spi_flash_cmd_wait_ready(p_flash, SPI_FLASH_PROG_TIMEOUT);


    page_addr++;
    byte_addr = 0;
  }

  SF_DEBUG_PRINTF("SF: program %s %d bytes @ %#x\n",
        ret ? "failure" : "success", len, offset);
  return ret;
}
static u8 flash_cmd_4erase = 0xff;
static u8 flash_cmd_4read = 0xff;
static u8 flash_cmd_4write = 0xff;
s32 spi_flash_cmd_4erase(spi_flash_t *p_flash, u32 offset, u32 len)
{
    u32 start = 0, end = 0, erase_size = 0;
    s32 ret = 0;
    u8 cmd[5];
    cmd[0] = flash_cmd_4erase;

    erase_size = p_flash->sector_size;
    if (offset % erase_size || len % erase_size) {
    SF_DEBUG_PRINTF("SF: Erase offset/length not multiple of erase size %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
    return -1;
    }

    if (p_flash->p_spi == NULL) {
    SF_DEBUG_PRINTF("SF: unable to claim SPI bus %s %d %s\n",
                      __FUNCTION__, __LINE__, __FILE__);
    return -1;
    }

    start = offset;
    end = start + len;
    while(offset < end)
    {
        cmd[1] = offset >> 24;
        cmd[2] = offset >> 16;
        cmd[3] = offset >> 8;
        cmd[4] = offset;

        offset += erase_size;

        SF_DEBUG_PRINTF("SF: erase %2x %2x %2x %2x %2x (%x)\n", cmd[0], cmd[1],
          cmd[2], cmd[3], cmd[4], offset);

        ret = spi_flash_cmd_write_enable(p_flash);
        if (ret)
            return ret;

        ret = spi_flash_cmd_write(p_flash->p_spi, cmd, sizeof(cmd), NULL, 0);
        if (ret)
            return ret;

        spi_flash_cmd_wait_ready(p_flash, SPI_FLASH_PAGE_ERASE_TIMEOUT);
    }
    SF_DEBUG_PRINTF("SF: Successfully 4Byte_erased %d bytes @ %#x\n", len, start);

    return ret;
}

s32 spi_flash_cmd_4read(struct spi_flash *p_flash, u32 offset,
    u32 len, void *p_data)
{
    u8 cmd[6];
    cmd[0] = flash_cmd_4read;
    cmd[1] = offset >> 24;
    cmd[2] = offset >> 16;
    cmd[3] = offset >> 8;
    cmd[4] = offset;
    cmd[5] = 0x00;
    return spi_flash_read_common(p_flash, cmd, sizeof(cmd), p_data, len);
}

s32 spi_flash_cmd_4write(spi_flash_t *p_flash, u32 offset,
    u32 len, void *p_buf)
{
  u32 page_addr = 0, byte_addr = 0, page_size = 0;
  u32 chunk_len = 0, actual = 0;
  s32 ret = 0;
  u8 cmd[5];

  page_size = p_flash->page_size;
  page_addr = offset / page_size;
  byte_addr = offset % page_size;

  if (p_flash->p_spi == NULL) {
    SF_DEBUG_PRINTF("SF: unable to claim SPI bus %s %d %s\n",
                      __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  cmd[0] = flash_cmd_4write;
  spi_soft_reset(p_flash->p_spi);
  for (actual = 0; actual < len; actual += chunk_len) {
    chunk_len = MIN(len - actual, page_size - byte_addr);

    cmd[1] = page_addr >> 16;
    cmd[2] = page_addr >> 8;
    cmd[3] = page_addr;
    cmd[4] = byte_addr;

    SF_DEBUG_PRINTF("PP: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x%02x } chunk_len = %d\n",
          (const u8 *)p_buf + actual, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], chunk_len);

    ret = spi_flash_cmd_write_enable(p_flash);
    if (ret < 0) {
      SF_DEBUG_PRINTF("SF: enabling write failed %s %d %s\n",
                      __FUNCTION__, __LINE__, __FILE__);
      break;
    }

    ret = spi_flash_cmd_write(p_flash->p_spi, cmd, 5,
            (u8 *)p_buf + actual, chunk_len);
    if (ret < 0) {
      SF_DEBUG_PRINTF("SF: write failed %s %d %s\n",__FUNCTION__, __LINE__, __FILE__);
      break;
    }

    spi_flash_cmd_wait_ready(p_flash, SPI_FLASH_PROG_TIMEOUT);


    page_addr++;
    byte_addr = 0;
  }

  SF_DEBUG_PRINTF("SF: program %s %d bytes @ %#x\n",
        ret ? "failure" : "success", len, offset);
  return ret;
}

/*
s32 spi_flash_cmd_write_multi(spi_flash_t *p_flash, u32 offset,
    u32 len, const void *p_buf)
{
  u32 page_addr = 0, byte_addr = 0, page_size = 0;
  u32 start_tick = 0, stop_tick = 0;
  u32 chunk_len = 0, actual = 0;
  s32 ret = 0;
  u8 cmd[4];

  page_size = p_flash->page_size;
  page_addr = offset / page_size;
  byte_addr = offset % page_size;

  if (p_flash->p_spi == NULL) {
    SF_DEBUG_PRINTF("SF: unable to claim SPI bus %s %d %s\n",
                      __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  cmd[0] = CMD_PAGE_PROGRAM;

//  spi_jazz_pinmux(p_flash->spi_id);

  spi_soft_reset(p_flash->p_spi);

  for (actual = 0; actual < len; actual += chunk_len) {
    chunk_len = MIN(len - actual, page_size - byte_addr);

    cmd[1] = page_addr >> 8;
    cmd[2] = page_addr;
    cmd[3] = byte_addr;

    SF_DEBUG_PRINTF("PP: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x } chunk_len = %d\n",
          (const u8 *)p_buf + actual, cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);
    #ifdef SF_DEBUG_ENABLE
    start_tick = mtos_ticks_get();
    #endif
    ret = spi_flash_cmd_write_enable(p_flash);
    if (ret < 0) {
      SF_DEBUG_PRINTF("SF: enabling write failed %s %d %s\n",
                      __FUNCTION__, __LINE__, __FILE__);
      break;
    }
    #ifdef SF_DEBUG_ENABLE
    stop_tick = mtos_ticks_get();
    #endif
     SF_DEBUG_PRINTF("\n-----fly:tick = %d", stop_tick-start_tick);
    ret = spi_flash_cmd_write(p_flash->p_spi, cmd, 4,
            (const u8 *)p_buf + actual, chunk_len);
    if (ret < 0) {
      SF_DEBUG_PRINTF("SF: write failed %s %d %s\n",__FUNCTION__, __LINE__, __FILE__);
      break;
    }
    #ifdef SF_DEBUG_ENABLE
    stop_tick = mtos_ticks_get();
    #endif
    SF_DEBUG_PRINTF("\n-----fly:tick = %d", stop_tick-start_tick);
    spi_flash_cmd_wait_ready(p_flash, SPI_FLASH_PROG_TIMEOUT);
    #ifdef SF_DEBUG_ENABLE
    stop_tick = mtos_ticks_get();
    #endif
    SF_DEBUG_PRINTF("\n-----fly:tick = %d", stop_tick-start_tick);

    page_addr++;
    byte_addr = 0;
  }

  SF_DEBUG_PRINTF("SF: program %s %d bytes @ %#x\n",
        ret ? "failure" : "success", len, offset);
  return ret;
}
*/

s32 spi_flash_read_common(struct spi_flash *p_flash, u8 *p_cmd,
    u32 cmd_len, void *p_data, u32 data_len)
{
  s32 ret = 0;

  if (p_flash->p_spi == NULL) {
    SF_DEBUG_PRINTF("SF: unable to claim SPI bus %s %d %s\n",
                      __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  ret = spi_flash_cmd_read(p_flash->p_spi, p_cmd, cmd_len, p_data, data_len);

  return ret;
}

s32 spi_flash_cmd_read_fast(struct spi_flash *p_flash, u32 offset,
    u32 len, void *p_data)
{
  u8 cmd[5] = {0,0,0,0,0};

  cmd[0] = CMD_READ_ARRAY_FAST;
  spi_flash_addr(offset, cmd);
  cmd[4] = 0x00;

  return spi_flash_read_common(p_flash, cmd, sizeof(cmd), p_data, len);
}


void spi_flash_cmd_wait_ready(spi_flash_t *p_flash, u32 timeout)
{
    u8 resp = 0;
    int ret = 0;
    u32 status = 0;

    status = 1;

    while(status && timeout)
    {
        ret = spi_flash_cmd(p_flash->p_spi, CMD_READ_STATUS, &resp, 1);
        if(ret < 0)
        {
          SF_DEBUG_PRINTF("read flash status failed! %s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
          timeout --;
          continue;
        }
        if((resp&0x01) == 1)
          status = 1;
        else
        {
          status = 0;
          timeout --;
        }
    }

    if(timeout == 0)
    {
        SF_DEBUG_PRINTF("spi_flash_cmd_wait_ready: time out! %s %d %s\n",
                __FUNCTION__, __LINE__, __FILE__);
    }
}

s32 spi_flash_cmd_erase(spi_flash_t *p_flash, u8 erase_cmd,
      u32 offset, u32 len)
{
  u32 start = 0, end = 0, erase_size = 0;
  s32 ret = 0;
  u8 cmd[4];

  erase_size = p_flash->sector_size;
  if (offset % erase_size || len % erase_size) {
    SF_DEBUG_PRINTF("SF: Erase offset/length not multiple of erase size %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  if (p_flash->p_spi == NULL) {
    SF_DEBUG_PRINTF("SF: unable to claim SPI bus %s %d %s\n",
                      __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  cmd[0] = erase_cmd;
  start = offset;
  end = start + len;

  while (offset < end) {
    spi_flash_addr(offset, cmd);
    offset += erase_size;

    SF_DEBUG_PRINTF("SF: erase %2x %2x %2x %2x (%x)\n", cmd[0], cmd[1],
          cmd[2], cmd[3], offset);

    ret = spi_flash_cmd_write_enable(p_flash);
    if (ret)
      return ret;

    ret = spi_flash_cmd_write(p_flash->p_spi, cmd, sizeof(cmd), NULL, 0);
    if (ret)
      return ret;

      spi_flash_cmd_wait_ready(p_flash, SPI_FLASH_PAGE_ERASE_TIMEOUT);

  }

  SF_DEBUG_PRINTF("SF: Successfully erased %d bytes @ %#x\n", len, start);

  return ret;
}

s32 spi_flash_cmd_power_down(spi_flash_t *p_flash)
{
    return spi_flash_cmd(p_flash->p_spi, CMD_POWER_DOWN, NULL, 0);
}

s32 spi_flash_cmd_power_up(spi_flash_t *p_flash)
{
    return spi_flash_cmd(p_flash->p_spi, CMD_POWER_UP, NULL, 0);
}

s32 spi_flash_cmd_enter_sa(spi_flash_t *p_flash)
{
    SF_DEBUG_PRINTF("spi flash enter sa\n");
    return spi_flash_cmd(p_flash->p_spi, CMD_ENSA, NULL, 0);
}

s32 spi_flash_cmd_exit_sa(spi_flash_t *p_flash)
{
    SF_DEBUG_PRINTF("spi flash exit sa\n");
    return spi_flash_cmd(p_flash->p_spi, CMD_EXSA, NULL, 0);
}

/*
 * The following table holds all device probe functions
 *
 * shift:  number of continuation bytes before the ID
 * idcode: the expected IDCODE or 0xff for non JEDEC devices
 * probe:  the function to call
 *
 * Non JEDEC devices should be ordered in the table such that
 * the probe functions with best detection algorithms come first.
 *
 * Several matching entries are permitted, they will be tried
 * in sequence until a probe function returns non NULL.
 *
 * IDCODE_CONT_LEN may be redefined if a device needs to declare a
 * larger "shift" value.  IDCODE_PART_LEN generally shouldn't be
 * changed.  This is the max number of bytes probe functions may
 * examine when looking up part-specific identification info.
 *
 * Probe functions will be given the idcode buffer starting at their
 * manu id byte (the "idcode" in the table below).  In other words,
 * all of the continuation bytes will be skipped (the "shift" below).
 */
#define IDCODE_CONT_LEN 0
#define IDCODE_PART_LEN 5

typedef struct flash_probe_info
{
  const u8 shift;
  const u8 idcode;
  spi_flash_t * (*probe) (spi_bus_t *p_spi, u8 *p_idcode);
} flash_probe_info_t;

const flash_probe_info_t  flashes[] = {
  /* Keep it sorted by define name */
#ifdef CONFIG_SPI_FLASH_ATMEL
  { 0, 0x1f, spi_flash_probe_atmel, },
#endif
#ifdef CONFIG_SPI_FLASH_EON
  { 0, 0x1c, spi_flash_probe_eon, },
  { 0, 0xc8, spi_flash_probe_eon, },
  { 0, 0x8c, spi_flash_probe_eon, },
#endif
#ifdef CONFIG_SPI_FLASH_MACRONIX
  { 0, 0xc2, spi_flash_probe_macronix, },
#endif
#ifdef CONFIG_SPI_FLASH_SPANSION
  { 0, 0x01, spi_flash_probe_spansion, },
#endif
#ifdef CONFIG_SPI_FLASH_SST
  { 0, 0xbf, spi_flash_probe_sst, },
#endif
#ifdef CONFIG_SPI_FLASH_STMICRO
  { 0, 0x20, spi_flash_probe_stmicro, },
#endif
#ifdef CONFIG_SPI_FLASH_WINBOND
  { 0, 0xef, spi_flash_probe_winbond, },
#endif
#ifdef CONFIG_SPI_FRAM_RAMTRON
  { 6, 0xc2, spi_fram_probe_ramtron, },
#undef IDCODE_CONT_LEN
#define IDCODE_CONT_LEN 6
#endif
  /* Keep it sorted by best detection */
#ifdef CONFIG_SPI_FLASH_STMICRO
  { 0, 0xff, spi_flash_probe_stmicro, },
#endif
#ifdef CONFIG_SPI_FRAM_RAMTRON_NON_JEDEC
  { 0, 0xff, spi_fram_probe_ramtron, },
#endif
};
#define IDCODE_LEN (IDCODE_CONT_LEN + IDCODE_PART_LEN)

spi_flash_t *spi_flash_probe(spi_bus_t * p_spi)
{
  spi_flash_t *p_flash = NULL;
  s32 ret = 0, i = 0, shift = 0;
  u8 idcode[IDCODE_LEN] = {0}, *p_idp = NULL;

  if (!p_spi) {
    SF_DEBUG_PRINTF("SF: Failed to claim SPI bus! %s %d %s\n",
                __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

  /* Read the ID codes */
  ret = spi_flash_cmd(p_spi, CMD_READ_ID, idcode, sizeof(idcode));
  if (ret)
    return NULL;

  /* count the number of continuation bytes */
  for (shift = 0, p_idp = idcode;
       shift < IDCODE_CONT_LEN && *p_idp == 0x7f;
       ++shift, ++p_idp)
    continue;

  /* search the table for matches in shift and id */
  for (i = 0; i < ARRAY_SIZE(flashes); ++i)
    if (flashes[i].shift == shift && flashes[i].idcode == *p_idp) {
      /* we have a match, call probe */
      p_flash = flashes[i].probe(p_spi, p_idp);
      if (p_flash)
        break;
    }

  if (!p_flash) {
    SF_DEBUG_PRINTF("SF: Unsupported manufacturer %02x\n", *p_idp);
    return p_flash;
  }
    p_flash->p_spi = p_spi;
  SF_DEBUG_PRINTF("SF: Detected %s with page size %d, total %d\n",
            p_flash->p_name, p_flash->sector_size, p_flash->size);

  return p_flash;
}

/************************************************************
*                                   Winbond
************************************************************/
/* M25Pxx-specific commands */
#define CMD_W25_WREN    0x06  /* Write Enable */
#define CMD_W25_WRDI    0x04  /* Write Disable */
#define CMD_W25_RDSR    0x05  /* Read Status Register */
#define CMD_W25_WRSR    0x01  /* Write Status Register */
#define CMD_W25_READ    0x03  /* Read Data Bytes */
#define CMD_W25_FAST_READ  0x0b  /* Read Data Bytes at Higher Speed */
#define CMD_W25_PP    0x02  /* Page Program */
#define CMD_W25_SE    0x20  /* Sector (4K) Erase */
#define CMD_W25_BE    0xd8  /* Block (64K) Erase */
#define CMD_W25_CE    0xc7  /* Chip Erase */
#define CMD_W25_DP    0xb9  /* Deep Power-down */
#define CMD_W25_RES    0xab  /* Release from DP, and Read Signature */

typedef struct winbond_spi_flash_params {
  u16  id;
  /* Log2 of page size in power-of-two mode */
  u8    l2_page_size;
  u16  pages_per_sector;
  u16  sectors_per_block;
  u16  nr_blocks;
  const char  *p_name;
} winbond_spi_flash_params_t;

static const winbond_spi_flash_params_t winbond_spi_flash_table[] = {
  {
    /*.id      = */0x3013,
    /*.l2_page_size    = */8,
    /*.pages_per_sector  = */16,
   /* .sectors_per_block  = */16,
    /*.nr_blocks    = */8,
   /* .p_name      = */"W25X40"
  },
  {
     /*.id      = */0x3015,
     /*.l2_page_size    = */8,
     /*.pages_per_sector  = */16,
     /*.sectors_per_block  = */16,
     /*.nr_blocks    = */32,
     /*.p_name      = */"W25X16"
  },
  {
    /*.id      = */0x3016,
    /*.l2_page_size    = */8,
    /*.pages_per_sector  = */16,
    /*.sectors_per_block  = */16,
    /*.nr_blocks    = */64,
    /*.p_name      = */"W25X32"
  },
  {
    /*.id      = */0x3017,
    /*.l2_page_size    = */8,
    /*.pages_per_sector  = */16,
    /*.sectors_per_block  = */16,
    /*.nr_blocks    = */128,
    /*.p_name      = */"W25X64"
  },
  {
    /*.id      = */0x4015,
    /*.l2_page_size    = */8,
    /*.pages_per_sector  = */16,
    /*.sectors_per_block  = */16,
    /*.nr_blocks    = */32,
    /*.p_name      = */"W25Q16"
  },
  {
    /*.id      = */0x4016,
    /*.l2_page_size    = */8,
    /*.pages_per_sector  = */16,
    /*.sectors_per_block  = */16,
    /*.nr_blocks    = */64,
    /*.p_name      = */"W25Q32"
  },
  {
    /*.id      = */0x4017,
    /*.l2_page_size    = */8,
    /*.pages_per_sector  = */16,
    /*.sectors_per_block  = */16,
    /*.nr_blocks    = */128,
   /*.p_name      = */"W25Q64"
  },
  {
    /*.id      = */0x4018,
    /*.l2_page_size    = */8,
    /*.pages_per_sector  = */16,
    /*.sectors_per_block  = */16,
    /*.nr_blocks    = */256,
    /*.p_name      = */"W25Q128"
  }
};

static s32 winbond_erase(spi_flash_t *p_flash, u32 offset, u32 len)
{
  return spi_flash_cmd_erase(p_flash, CMD_W25_SE, offset, len);
}

static spi_flash_t block_winbond_flash;
spi_flash_t *spi_flash_probe_winbond(spi_bus_t *p_spi, u8 *p_idcode)
{
  const winbond_spi_flash_params_t *p_params = NULL;
  spi_flash_t *p_flash = NULL;
  u32 i = 0;
  u32 page_size = 0;

  for (i = 0; i < ARRAY_SIZE(winbond_spi_flash_table); i++) {
    p_params = &winbond_spi_flash_table[i];
    if (p_params->id == ((p_idcode[1] << 8) | p_idcode[2]))
      break;
  }

  if (i == ARRAY_SIZE(winbond_spi_flash_table)) {
    SF_DEBUG_PRINTF("SF: Unsupported Winbond ID %02x%02x   %s %d %s\n",
        p_idcode[1], p_idcode[2],
              __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

//  p_flash = mtos_malloc(sizeof(*p_flash));
  p_flash = &block_winbond_flash;
  if (!p_flash) {
    SF_DEBUG_PRINTF("SF: Failed to allocate memory!  %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

  p_flash->p_spi = p_spi;
  p_flash->p_name = p_params->p_name;

  /* Assuming power-of-two page size initially. */
  page_size = 1 << p_params->l2_page_size;

    p_flash->write = spi_flash_cmd_write_multi;
    p_flash->erase = winbond_erase;
    p_flash->read = spi_flash_cmd_read_fast;
    p_flash->power_down = spi_flash_cmd_power_down;
    p_flash->power_up = spi_flash_cmd_power_up;
    p_flash->page_size = page_size;
    p_flash->sector_size = page_size * p_params->pages_per_sector;
    p_flash->size = page_size * p_params->pages_per_sector
        * p_params->sectors_per_block
        * p_params->nr_blocks;

  return p_flash;
}
/*********************************************************
*                     Spansion
*********************************************************/
/* S25FLxx-specific commands */
#define CMD_S25FLXX_READ  0x03  /* Read Data Bytes */
#define CMD_S25FLXX_FAST_READ  0x0b  /* Read Data Bytes at Higher Speed */
#define CMD_S25FLXX_READID  0x90  /* Read Manufacture ID and Device ID */
#define CMD_S25FLXX_WREN  0x06  /* Write Enable */
#define CMD_S25FLXX_WRDI  0x04  /* Write Disable */
#define CMD_S25FLXX_RDSR  0x05  /* Read Status Register */
#define CMD_S25FLXX_WRSR  0x01  /* Write Status Register */
#define CMD_S25FLXX_PP    0x02  /* Page Program */
#define CMD_S25FLXX_SE    0xd8  /* Sector Erase */
#define CMD_S25FLXX_BE    0xc7  /* Bulk Erase */
#define CMD_S25FLXX_DP    0xb9  /* Deep Power-down */
#define CMD_S25FLXX_RES    0xab  /* Release from DP, and Read Signature */

#define SPSN_ID_S25FL008A  0x0213
#define SPSN_ID_S25FL016A  0x0214
#define SPSN_ID_S25FL032A  0x0215
#define SPSN_ID_S25FL064A  0x0216
#define SPSN_ID_S25FL128P  0x2018
#define SPSN_ID_S25FL256S  0x0219
#define SPSN_EXT_ID_S25FL128P_256KB  0x0300
#define SPSN_EXT_ID_S25FL128P_64KB  0x0301
#define SPSN_EXT_ID_S25FL256S_256KB  0x4d00
#define SPSN_EXT_ID_S25FL256S_64KB  0x4d01
#define SPSN_EXT_ID_S25FL032P    0x4d00
#define SPSN_EXT_ID_S25FL129P    0x4d01

typedef struct spansion_spi_flash_params {
  u16 idcode1;
  u16 idcode2;
  u16 page_size;
  u16 pages_per_sector;
  u16 nr_sectors;
  const char *p_name;
} spansion_spi_flash_params_t;

static const spansion_spi_flash_params_t spansion_spi_flash_table[] = {
  {
    /*.idcode1 = */SPSN_ID_S25FL008A,
    /*.idcode2 = */0,
    /*.page_size = */256,
    /*.pages_per_sector = */256,
    /*.nr_sectors = */16,
    /*.p_name = */"S25FL008A"
  },
  {
    /*.idcode1 = */SPSN_ID_S25FL016A,
    /*.idcode2 = */0,
    /*.page_size = */256,
    /*.pages_per_sector = */256,
    /*.nr_sectors = */32,
    /*.p_name = */"S25FL016A"
  },
  {
    /*.idcode1 = */SPSN_ID_S25FL032A,
    /*.idcode2 = */0,
    /*.page_size = */256,
    /*.pages_per_sector = */256,
    /*.nr_sectors = */64,
    /*.p_name = */"S25FL032A"
  },
  {
    /*.idcode1 = */SPSN_ID_S25FL064A,
    /*.idcode2 = */0,
   /* .page_size = */256,
    /*.pages_per_sector = */256,
   /* .nr_sectors = */128,
    /*.p_name = */"S25FL064A"
  },
  {
    /*.idcode1 = */SPSN_ID_S25FL064A,
    /*.idcode2 = */0x4d00,
   /* .page_size = */256,
    /*.pages_per_sector = */256,
   /* .nr_sectors = */128,
    /*.p_name = */"S25FL064P"
  },
  {
    /*.idcode1 = */SPSN_ID_S25FL128P,
    /*.idcode2 = */SPSN_EXT_ID_S25FL128P_64KB,
    /*.page_size = */256,
    /*.pages_per_sector = */256,
    /*.nr_sectors = */256,
    /*.p_name = */"S25FL128P_64K"
  },
  {
    /*.idcode1 = */SPSN_ID_S25FL128P,
    /*.idcode2 = */SPSN_EXT_ID_S25FL128P_256KB,
    /*.page_size = */256,
    /*.pages_per_sector = */1024,
    /*.nr_sectors = */64,
    /*.p_name = */"S25FL128P_256K"
  },
  {
    /*.idcode1 = */SPSN_ID_S25FL032A,
    /*.idcode2 = */SPSN_EXT_ID_S25FL032P,
    /*.page_size = */256,
    /*.pages_per_sector = */256,
    /*.nr_sectors = */64,
    /*.p_name = */"S25FL032P"
  },
  {
    /*.idcode1 = */SPSN_ID_S25FL128P,
    /*.idcode2 = */SPSN_EXT_ID_S25FL129P,
    /*.page_size = */256,
    /*.pages_per_sector = */256,
    /*.nr_sectors = */256,
    /*.p_name = */"S25FL129P_64K"
  },
  {
    /*.idcode1 = */SPSN_ID_S25FL256S,
    /*.idcode2 = */SPSN_EXT_ID_S25FL256S_256KB,
    /*.page_size = */256,
    /*.pages_per_sector = */1024,
    /*.nr_sectors = */128,
    /*.p_name = */"S25FL256S_256K"
  },
  {
    /*.idcode1 = */SPSN_ID_S25FL256S,
    /*.idcode2 = */SPSN_EXT_ID_S25FL256S_64KB,
    /*.page_size = */256,
    /*.pages_per_sector = */256,
    /*.nr_sectors = */512,
    /*.p_name = */"S25FL256S_64K"
  }
};

static s32 spansion_erase(spi_flash_t *p_flash, u32 offset, u32 len)
{
  return spi_flash_cmd_erase(p_flash, CMD_S25FLXX_SE, offset, len);
}

static spi_flash_t block_spansion_flash;
spi_flash_t *spi_flash_probe_spansion(spi_bus_t *p_spi, u8 *p_idcode)
{
  const spansion_spi_flash_params_t *p_params = NULL;
  spi_flash_t *p_flash = NULL;
  u32 i = 0;
  u16 jedec = 0, ext_jedec = 0;

  jedec = p_idcode[1] << 8 | p_idcode[2];
  ext_jedec = p_idcode[3] << 8 | p_idcode[4];

  for (i = 0; i < ARRAY_SIZE(spansion_spi_flash_table); i++) {
    p_params = &spansion_spi_flash_table[i];
    if (p_params->idcode1 == jedec) {
      if (p_params->idcode2 == ext_jedec)
        break;
    }
  }

  if (i == ARRAY_SIZE(spansion_spi_flash_table)) {
    SF_DEBUG_PRINTF("SF: Unsupported SPANSION ID %04x %04x  %s %d %s\n",
                  jedec, ext_jedec,__FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

 // p_flash = mtos_malloc(sizeof(*p_flash));
  p_flash = &block_spansion_flash;
  if (!p_flash) {
    SF_DEBUG_PRINTF("SF: Failed to allocate memory  %s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

    p_flash->p_spi = p_spi;
    p_flash->p_name = p_params->p_name;

    p_flash->page_size = p_params->page_size;
    p_flash->sector_size = p_params->page_size * p_params->pages_per_sector;
    p_flash->size = p_flash->sector_size * p_params->nr_sectors;

    if(p_flash->size > 0x1000000)
    {
      flash_cmd_4write = 0x12;
      flash_cmd_4erase = 0xdc;
      flash_cmd_4read = 0x0c;
      p_flash->write = spi_flash_cmd_4write;
      p_flash->erase = spi_flash_cmd_4erase;
      p_flash->read = spi_flash_cmd_4read;
    }
    else
    {
      p_flash->write = spi_flash_cmd_write_multi;
      p_flash->erase = spansion_erase;
      p_flash->read = spi_flash_cmd_read_fast;
    }
    p_flash->power_down = spi_flash_cmd_power_down;
    p_flash->power_up = spi_flash_cmd_power_up;

  return p_flash;
}

/*********************************************************
*                     EON
*********************************************************/
/* EN25Q128-specific commands */
#define CMD_EN25Q128_WREN  0x06    /* Write Enable */
#define CMD_EN25Q128_WRDI  0x04    /* Write Disable */
#define CMD_EN25Q128_RDSR  0x05    /* Read Status Register */
#define CMD_EN25Q128_WRSR  0x01    /* Write Status Register */
#define CMD_EN25Q128_READ  0x03    /* Read Data Bytes */
#define CMD_EN25Q128_FAST_READ  0x0b    /* Read Data Bytes at Higher Speed */
#define CMD_EN25Q128_PP    0x02    /* Page Program */
#define CMD_EN25Q128_SE    0x20    /* Sector Erase */
#define CMD_EN25Q128_BE    0xd8    /* Block Erase */
#define CMD_EN25Q128_DP    0xb9    /* Deep Power-down */
#define CMD_EN25Q128_RES  0xab    /* Release from DP, and Read Signature */

typedef struct eon_spi_flash_params {
  u8 idcode1;
  u16 page_size;
  u16 pages_per_sector;
  u16 sectors_per_block;
  u16 nr_sectors;
  const char *p_name;
} eon_spi_flash_params_t;

static const eon_spi_flash_params_t eon_spi_flash_table[] = {
  {
    /*.idcode1 = */0x15,
    /*.page_size = */256,
    /*.pages_per_sector = */16,
    /*.sectors_per_block = */16,
    /*.nr_sectors = */512,
    /*.p_name = */"EN25Q16"
  },
  {
    /*.idcode1 = */0x16,
    /*.page_size = */256,
    /*.pages_per_sector = */16,
    /*.sectors_per_block = */16,
    /*.nr_sectors = */1024,
    /*.p_name = */"EN25Q32B"
  },
  {
    /*.idcode1 = */0x17,
    /*.page_size = */256,
    /*.pages_per_sector = */16,
    /*.sectors_per_block = */16,
    /*.nr_sectors = */2048,
    /*.p_name = */"EN25Q64"
  },
  {
    /*.idcode1 = */0x18,
    /*.page_size = */256,
    /*.pages_per_sector = */16,
    /*.sectors_per_block = */16,
    /*.nr_sectors = */4096,
    /*.p_name = */"EN25Q128"
  }
};

static s32 eon_erase(spi_flash_t *p_flash, u32 offset, u32 len)
{
  return spi_flash_cmd_erase(p_flash, CMD_EN25Q128_BE, offset, len);
}

static spi_flash_t block_eon_flash;
spi_flash_t *spi_flash_probe_eon(spi_bus_t *p_spi, u8 *p_idcode)
{
  const  eon_spi_flash_params_t *p_params = NULL;
  spi_flash_t *p_flash = NULL;
  u32 i = 0;

  for (i = 0; i < ARRAY_SIZE(eon_spi_flash_table); ++i) {
    p_params = &eon_spi_flash_table[i];
    if (p_params->idcode1 == p_idcode[2])
      break;
  }

  if (i == ARRAY_SIZE(eon_spi_flash_table)) {
    SF_DEBUG_PRINTF("SF: Unsupported EON ID %02x %s %d %s\n", p_idcode[1],
                  __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

 // p_flash = mtos_malloc(sizeof(*p_flash));
  p_flash = &block_eon_flash;
  if (!p_flash) {
    SF_DEBUG_PRINTF("SF: Failed to allocate memory %s %d %s\n",
                  __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

  p_flash->p_spi = p_spi;
  p_flash->p_name = p_params->p_name;

  p_flash->write = spi_flash_cmd_write_multi;
  p_flash->erase = eon_erase;
  p_flash->read = spi_flash_cmd_read_fast;
  p_flash->power_down = spi_flash_cmd_power_down;
  p_flash->power_up = spi_flash_cmd_power_up;
  p_flash->page_size = p_params->page_size;
  p_flash->sector_size = p_params->page_size * p_params->pages_per_sector
      * p_params->sectors_per_block;
  p_flash->size = p_params->page_size * p_params->pages_per_sector
      * p_params->nr_sectors;

  return p_flash;
}

/********************************************************
*                       SST
********************************************************/
#define CMD_SST_WREN    0x06  /* Write Enable */
#define CMD_SST_WRDI    0x04  /* Write Disable */
#define CMD_SST_RDSR    0x05  /* Read Status Register */
#define CMD_SST_WRSR    0x01  /* Write Status Register */
#define CMD_SST_READ    0x03  /* Read Data Bytes */
#define CMD_SST_FAST_READ  0x0b  /* Read Data Bytes at Higher Speed */
#define CMD_SST_BP    0x02  /* Byte Program */
#define CMD_SST_AAI_WP    0xAD  /* Auto Address Increment Word Program */
#define CMD_SST_SE    0x20  /* Sector Erase */

#define SST_SR_WIP    (1 << 0)  /* Write-in-Progress */
#define SST_SR_WEL    (1 << 1)  /* Write enable */
#define SST_SR_BP0    (1 << 2)  /* Block Protection 0 */
#define SST_SR_BP1    (1 << 3)  /* Block Protection 1 */
#define SST_SR_BP2    (1 << 4)  /* Block Protection 2 */
#define SST_SR_AAI    (1 << 6)  /* Addressing mode */
#define SST_SR_BPL    (1 << 7)  /* BP bits lock */

#define SST_FEAT_WP    (1 << 0)  /* Supports AAI word program */
#define SST_FEAT_MBP    (1 << 1)  /* Supports multibyte program */

typedef struct sst_spi_flash_params {
  u8 idcode1;
  u8 flags;
  u16 nr_sectors;
  const char *p_name;
} sst_spi_flash_params_t;

#define SST_SECTOR_SIZE (4 * 1024)
#define SST_PAGE_SIZE   256
static const struct sst_spi_flash_params sst_spi_flash_table[] = {
  {
    /*.idcode1 = */0x8d,
    /*.flags = */SST_FEAT_WP,
    /*.nr_sectors = */128,
    /*.p_name = */"SST25VF040B"
  },{
    /*.idcode1 = */0x8e,
    /*.flags = */SST_FEAT_WP,
    /*.nr_sectors = */256,
    /*.p_name = */"SST25VF080B"
  },{
    /*.idcode1 = */0x41,
    /*.flags = */SST_FEAT_WP,
    /*.nr_sectors = */512,
    /*.p_name = */"SST25VF016B"
  },{
    /*.idcode1 = */0x4a,
    /*.flags = */SST_FEAT_WP,
    /*.nr_sectors = */1024,
    /*.p_name = */"SST25VF032B"
  },{
    /*.idcode1 = */0x4b,
    /*.flags = */SST_FEAT_MBP,
    /*.nr_sectors = */2048,
    /*.p_name = */"SST25VF064C"
  },{
    /*.idcode1 = */0x01,
    /*.flags = */SST_FEAT_WP,
    /*.nr_sectors = */16,
    /*.p_name = */"SST25WF512"
  },{
    /*.idcode1 = */0x02,
    /*.flags = */SST_FEAT_WP,
    /*.nr_sectors = */32,
    /*.p_name = */"SST25WF010"
  },{
    /*.idcode1 = */0x03,
    /*.flags = */SST_FEAT_WP,
    /*.nr_sectors = */64,
    /*.p_name = */"SST25WF020"
  },{
    /*.idcode1 = */0x04,
    /*.flags = */SST_FEAT_WP,
    /*.nr_sectors = */128,
    /*.p_name = */"SST25WF040"
  }
};

static s32
sst_enable_writing(spi_flash_t *p_flash)
{
  s32 ret = spi_flash_cmd_write_enable(p_flash);
  if (ret)
    SF_DEBUG_PRINTF("SF: Enabling Write failed %s %d %s\n",
              __FUNCTION__, __LINE__, __FILE__);
  return ret;
}

static s32
sst_disable_writing(spi_flash_t *p_flash)
{
  int ret = spi_flash_cmd_write_disable(p_flash);
  if (ret)
    SF_DEBUG_PRINTF("SF: Disabling Write failed %s %d %s\n",
                __FUNCTION__, __LINE__, __FILE__);
  return ret;
}

static s32
sst_byte_write(spi_flash_t *p_flash, u32 offset, void *p_buf)
{
  s32 ret = 0;
  u8 cmd[4] = {
    CMD_SST_BP,
    offset >> 16,
    offset >> 8,
    offset,
  };

  SF_DEBUG_PRINTF("BP: 0x%p => cmd = { 0x%02x 0x%06x }\n",
                 p_buf, cmd[0], offset);

  ret = sst_enable_writing(p_flash);
  if (ret)
    return ret;

  ret = spi_flash_cmd_write(p_flash->p_spi, cmd, sizeof(cmd), p_buf, 1);
  if (ret)
    return ret;

  spi_flash_cmd_wait_ready(p_flash, SPI_FLASH_PROG_TIMEOUT);
    return ret;
}

static s32
sst_write_wp(spi_flash_t *p_flash, u32 offset, u32 len, void *p_buf)
{
  u32 actual = 0, cmd_len = 0;
  s32 ret = 0;
  u8 cmd[4];

  if (p_flash->p_spi == NULL) {
    SF_DEBUG_PRINTF("SF: Unable to claim SPI bus %s %d %s\n",
              __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  /* If the data is not word aligned, write out leading single byte */
  actual = offset % 2;
  if (actual) {
    ret = sst_byte_write(p_flash, offset, p_buf);
    if (ret)
    {
            SF_DEBUG_PRINTF("SF: sst: program %s %u bytes @ 0x%x\n",
                  ret ? "failure" : "success", len, offset - actual);
          return ret;
       }
  }
  offset += actual;

  ret = sst_enable_writing(p_flash);
  if (ret)
  {
          SF_DEBUG_PRINTF("SF: sst: program %s %u bytes @ 0x%x\n",
                ret ? "failure" : "success", len, offset - actual);
        return ret;
     }

  cmd_len = 4;
  cmd[0] = CMD_SST_AAI_WP;
  cmd[1] = offset >> 16;
  cmd[2] = offset >> 8;
  cmd[3] = offset;

  for (; actual < len - 1; actual += 2) {
    SF_DEBUG_PRINTF("WP: 0x%p => cmd = { 0x%02x 0x%06x }\n",
          (const u8 *)p_buf + actual, cmd[0],offset);

    ret = spi_flash_cmd_write(p_flash->p_spi, cmd, cmd_len,
                              (u8 *)p_buf + actual, 2);
    if (ret) {
      SF_DEBUG_PRINTF("SF: sst word program failed %s %d %s\n",
                  __FUNCTION__, __LINE__,__FILE__);
      break;
    }

    spi_flash_cmd_wait_ready(p_flash, SPI_FLASH_PROG_TIMEOUT);

    cmd_len = 1;
    offset += 2;
  }

  if (!ret)
    ret = sst_disable_writing(p_flash);

  /* If there is a single trailing byte, write it out */
  if (!ret && actual != len)
    ret = sst_byte_write(p_flash, offset,  (u8 *)p_buf + actual);

    SF_DEBUG_PRINTF("SF: sst: program %s %u bytes @ 0x%x\n",
          ret ? "failure" : "success", len, offset - actual);
  return ret;
}

static s32 sst_erase(spi_flash_t *p_flash, u32 offset, u32 len)
{
  return spi_flash_cmd_erase(p_flash, CMD_SST_SE, offset, len);
}

static s32
sst_unlock(spi_flash_t *p_flash)
{
  s32 ret = 0;
  u8 cmd = 0, status = 0;

  ret = sst_enable_writing(p_flash);
  if (ret)
    return ret;

  cmd = CMD_SST_WRSR;
  status = 0;
  ret = spi_flash_cmd_write(p_flash->p_spi, &cmd, 1, &status, 1);
  if (ret)
    SF_DEBUG_PRINTF("SF: Unable to set status byte %s %d %s\n",
              __FUNCTION__, __LINE__, __FILE__);

  return ret;
}

static spi_flash_t block_sst_flash;
spi_flash_t *
spi_flash_probe_sst(spi_bus_t *p_spi, u8 *p_idcode)
{
  const sst_spi_flash_params_t *p_params = NULL;
    spi_flash_t *p_flash = NULL;
    u32 i = 0;

  for (i = 0; i < ARRAY_SIZE(sst_spi_flash_table); ++i) {
    p_params = &sst_spi_flash_table[i];
    if (p_params->idcode1 == p_idcode[2])
      break;
  }

  if (i == ARRAY_SIZE(sst_spi_flash_table)) {
    SF_DEBUG_PRINTF("SF: Unsupported SST ID %02x   %s %d %s\n", p_idcode[1],
                  __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

  //p_flash = mtos_malloc(sizeof(*p_flash));
  p_flash = &block_sst_flash;
  if (!p_flash) {
    SF_DEBUG_PRINTF("SF: Failed to allocate memory %s %d %s\n",
              __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

  p_flash->p_spi = p_spi;
    p_flash->p_name = p_params->p_name;

  if (p_params->flags & SST_FEAT_WP)
    p_flash->write = sst_write_wp;
  else
    p_flash->write = spi_flash_cmd_write_multi;
  p_flash->erase = sst_erase;
  p_flash->read = spi_flash_cmd_read_fast;
  p_flash->power_down = NULL;
  p_flash->power_up = NULL;
  p_flash->page_size = SST_PAGE_SIZE;
  p_flash->sector_size = SST_SECTOR_SIZE;
  p_flash->size = p_flash->sector_size * p_params->nr_sectors;

  /* Flash powers up read-only, so clear BP# bits */
  sst_unlock(p_flash);

  return p_flash;
}
/********************************************************
*                       MACRONIX
********************************************************/
/* MX25xx-specific commands */
#define CMD_MX25XX_WREN    0x06  /* Write Enable */
#define CMD_MX25XX_WRDI    0x04  /* Write Disable */
#define CMD_MX25XX_RDSR    0x05  /* Read Status Register */
#define CMD_MX25XX_WRSR    0x01  /* Write Status Register */
#define CMD_MX25XX_READ    0x03  /* Read Data Bytes */
#define CMD_MX25XX_FAST_READ  0x0b  /* Read Data Bytes at Higher Speed */
#define CMD_MX25XX_PP    0x02  /* Page Program */
#define CMD_MX25XX_SE    0x20  /* Sector Erase */
#define CMD_MX25XX_BE    0xD8  /* Block Erase */
#define CMD_MX25XX_CE    0xc7  /* Chip Erase */
#define CMD_MX25XX_DP    0xb9  /* Deep Power-down */
#define CMD_MX25XX_RES    0xab  /* Release from DP, and Read Signature */

typedef struct macronix_spi_flash_params {
  u16 idcode;
  u16 page_size;
  u16 pages_per_sector;
  u16 sectors_per_block;
  u16 nr_blocks;
  const char *p_name;
} macronix_spi_flash_params_t;

static const macronix_spi_flash_params_t macronix_spi_flash_table[] = {
  {
    /*.idcode = */0x2013,
    /*.page_size = */256,
    /*.pages_per_sector = */16,
    /*.sectors_per_block = */16,
    /*.nr_blocks = */8,
    /*.p_name = */"MX25L4005"
  },
  {
    /*.idcode = */0x2014,
    /*.page_size = */256,
    /*.pages_per_sector = */16,
    /*.sectors_per_block = */16,
    /*.nr_blocks = */16,
    /*.p_name = */"MX25L8005"
  },
  {
    /*.idcode = */0x2015,
    /*.page_size = */256,
    /*.pages_per_sector = */16,
    /*.sectors_per_block = */16,
    /*.nr_blocks = */32,
    /*.p_name = */"MX25L1605D"
  },
  {
    /*.idcode = */0x2016,
    /*.page_size = */256,
    /*.pages_per_sector = */16,
    /*.sectors_per_block = */16,
    /*.nr_blocks = */64,
    /*.p_name = */"MX25L3205D"
  },
  {
    /*.idcode = */0x2017,
    /*.page_size = */256,
    /*.pages_per_sector = */16,
    /*.sectors_per_block = */16,
    /*.nr_blocks = */128,
    /*.p_name = */"MX25L6405D"
  },
  {
    /*.idcode = */0x2018,
    /*.page_size = */256,
    /*.pages_per_sector = */16,
    /*.sectors_per_block = */16,
    /*.nr_blocks = */256,
    /*.p_name = */"MX25L12805D"
  },
  {
    /*.idcode = */0x2019,
    /*.page_size = */256,
    /*.pages_per_sector = */16,
    /*.sectors_per_block = */16,
    /*.nr_blocks = */512,
    /*.p_name = */"MX25L25635E"
  },
  {
    /*.idcode = */0x2618,
    /*.page_size = */256,
    /*.pages_per_sector = */16,
    /*.sectors_per_block = */16,
    /*.nr_blocks = */256,
    /*.p_name = */"MX25L12855E"
  },
};

static s32 macronix_write_status(spi_flash_t *p_flash, u8 sr)
{
  u8 cmd = 0;
  s32 ret = 0;

  ret = spi_flash_cmd_write_enable(p_flash);
  if (ret < 0) {
    SF_DEBUG_PRINTF("SF: enabling write failed  %s %d %s\n",
            __FUNCTION__, __LINE__,__FILE__);
    return ret;
  }

  cmd = CMD_MX25XX_WRSR;
  ret = spi_flash_cmd_write(p_flash->p_spi, &cmd, 1, &sr, 1);
  if (ret) {
    SF_DEBUG_PRINTF("SF: fail to write status register  %s %d %s\n",
              __FUNCTION__, __LINE__, __FILE__);
    return ret;
  }

  spi_flash_cmd_wait_ready(p_flash, SPI_FLASH_PROG_TIMEOUT);

  return 0;
}

static s32 macronix_unlock(spi_flash_t *p_flash)
{
  s32 ret = 0;

  /* Enable status register writing and clear BP# bits */
  ret = macronix_write_status(p_flash, 0);
  if (ret)
    SF_DEBUG_PRINTF("SF: fail to disable write protection  %s %d %s\n",
             __FUNCTION__, __LINE__, __FILE__);

  return ret;
}

static s32 macronix_erase(spi_flash_t *p_flash, u32 offset, u32 len)
{
  return spi_flash_cmd_erase(p_flash, CMD_MX25XX_BE, offset, len);
}

static spi_flash_t block_macronix_flash;
spi_flash_t *spi_flash_probe_macronix(spi_bus_t *p_spi, u8 *p_idcode)
{
  const macronix_spi_flash_params_t *p_params = NULL;
  spi_flash_t *p_flash = NULL;
  u32 i = 0;
  u16 id = p_idcode[2] | p_idcode[1] << 8;

  for (i = 0; i < ARRAY_SIZE(macronix_spi_flash_table); i++) {
    p_params = &macronix_spi_flash_table[i];
    if (p_params->idcode == id)
      break;
  }

  if (i == ARRAY_SIZE(macronix_spi_flash_table)) {
    SF_DEBUG_PRINTF("SF: Unsupported Macronix ID %04x   %s %d %s\n", id,
            __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

 // p_flash = mtos_malloc(sizeof(*p_flash));
  p_flash = &block_macronix_flash;
  if (!p_flash) {
    SF_DEBUG_PRINTF("SF: Failed to allocate memory    %s %d %s\n",
              __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

  p_flash->p_spi = p_spi;
  p_flash->p_name = p_params->p_name;

  p_flash->write = spi_flash_cmd_write_multi;
  p_flash->erase = macronix_erase;
  p_flash->read = spi_flash_cmd_read_fast;
  p_flash->power_down = spi_flash_cmd_power_down;
  p_flash->power_up = spi_flash_cmd_power_up;
  p_flash->enter_sa = spi_flash_cmd_enter_sa;
  p_flash->exit_sa = spi_flash_cmd_exit_sa;
  p_flash->page_size = p_params->page_size;
  p_flash->sector_size = p_params->page_size * p_params->pages_per_sector
    * p_params->sectors_per_block;
  p_flash->size = p_flash->sector_size * p_params->nr_blocks;

  /* Clear BP# bits for read-only flash */
  macronix_unlock(p_flash);

  return p_flash;
}
/********************************************************
*                       STMICRO
********************************************************/
/* M25Pxx-specific commands */
#define CMD_M25PXX_WREN    0x06  /* Write Enable */
#define CMD_M25PXX_WRDI    0x04  /* Write Disable */
#define CMD_M25PXX_RDSR    0x05  /* Read Status Register */
#define CMD_M25PXX_WRSR    0x01  /* Write Status Register */
#define CMD_M25PXX_READ    0x03  /* Read Data Bytes */
#define CMD_M25PXX_FAST_READ  0x0b  /* Read Data Bytes at Higher Speed */
#define CMD_M25PXX_PP    0x02  /* Page Program */
#define CMD_M25PXX_SE    0xd8  /* Sector Erase */
#define CMD_M25PXX_BE    0xc7  /* Bulk Erase */
#define CMD_M25PXX_DP    0xb9  /* Deep Power-down */
#define CMD_M25PXX_RES    0xab  /* Release from DP, and Read Signature */

typedef struct stmicro_spi_flash_params {
  u8 idcode1;
  u16 page_size;
  u16 pages_per_sector;
  u16 nr_sectors;
  const char *p_name;
} stmicro_spi_flash_params_t;

static const stmicro_spi_flash_params_t stmicro_spi_flash_table[] = {
  {
    /*.idcode1 = */0x11,
    /*.page_size = */256,
    /*.pages_per_sector = */128,
    /*.nr_sectors = */4,
    /*.p_name = */"M25P10"
  },
  {
    /*.idcode1 = */0x15,
    /*.page_size = */256,
    /*.pages_per_sector = */256,
    /*.nr_sectors = */32,
    /*.p_name = */"M25P16"
  },
  {
    /*.idcode1 = */0x12,
    /*.page_size = */256,
    /*.pages_per_sector = */256,
    /*.nr_sectors = */4,
    /*.p_name = */"M25P20"
  },
  {
    /*.idcode1 = */0x16,
    /*.page_size = */256,
    /*.pages_per_sector = */256,
    /*.nr_sectors = */64,
    /*.p_name = */"M25P32"
  },
  {
    /*.idcode1 = */0x13,
    /*.page_size = */256,
    /*.pages_per_sector = */256,
    /*.nr_sectors = */8,
    /*.p_name = */"M25P40"
  },
  {
    /*.idcode1 = */0x17,
    /*.page_size = */256,
    /*.pages_per_sector =*/ 256,
    /*.nr_sectors = */128,
    /*.p_name = */"M25P64"
  },
  {
    /*.idcode1 = */0x14,
    /*.page_size = */256,
    /*.pages_per_sector = */256,
    /*.nr_sectors = */16,
    /*.p_name = */"M25P80"
  },
  {
    /*.idcode1 = */0x18,
    /*.page_size = */256,
    /*.pages_per_sector = */1024,
    /*.nr_sectors = */64,
    /*.p_name = */"M25P128"
  }
};

static s32 stmicro_erase(spi_flash_t *p_flash, u32 offset, u32 len)
{
  return spi_flash_cmd_erase(p_flash, CMD_M25PXX_SE, offset, len);
}

static spi_flash_t block_stmicro_flash;

spi_flash_t *spi_flash_probe_stmicro(spi_bus_t *p_spi, u8 * p_idcode)
{
  const stmicro_spi_flash_params_t *p_params = NULL;
  spi_flash_t *p_flash = NULL;
  s32 i = 0;

  if (p_idcode[0] == 0xff) {
    i = spi_flash_cmd(p_spi, CMD_M25PXX_RES,
          p_idcode, 4);
    if (i)
      return NULL;
    if ((p_idcode[3] & 0xf0) == 0x10) {
      p_idcode[0] = 0x20;
      p_idcode[1] = 0x20;
      p_idcode[2] = p_idcode[3] + 1;
    } else
      return NULL;
  }

  for (i = 0; i < ARRAY_SIZE(stmicro_spi_flash_table); i++) {
    p_params = &stmicro_spi_flash_table[i];
    if (p_params->idcode1 == p_idcode[2]) {
      break;
    }
  }

  if (i == ARRAY_SIZE(stmicro_spi_flash_table)) {
    SF_DEBUG_PRINTF("SF: Unsupported STMicro ID %02x   %s %d %s\n", p_idcode[1],
            __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

//  p_flash = mtos_malloc(sizeof(*p_flash));
  p_flash = &block_stmicro_flash;
  if (!p_flash) {
    SF_DEBUG_PRINTF("SF: Failed to allocate memory  %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

  p_flash->p_spi = p_spi;
  p_flash->p_name = p_params->p_name;

  p_flash->write = spi_flash_cmd_write_multi;
  p_flash->erase = stmicro_erase;
  p_flash->read = spi_flash_cmd_read_fast;
  p_flash->page_size = p_params->page_size;
  p_flash->sector_size = p_params->page_size * p_params->pages_per_sector;
  p_flash->size = p_flash->sector_size * p_params->nr_sectors;

  return p_flash;
}

/********************************************************
*                       ATMEL
********************************************************/
/* AT45-specific commands */
#define CMD_AT45_READ_STATUS    0xd7
#define CMD_AT45_ERASE_PAGE    0x81
#define CMD_AT45_LOAD_PROG_BUF1    0x82
#define CMD_AT45_LOAD_BUF1    0x84
#define CMD_AT45_LOAD_PROG_BUF2    0x85
#define CMD_AT45_LOAD_BUF2    0x87
#define CMD_AT45_PROG_BUF1    0x88
#define CMD_AT45_PROG_BUF2    0x89

/* AT45 status register bits */
#define AT45_STATUS_P2_PAGE_SIZE  (1 << 0)
#define AT45_STATUS_READY    (1 << 7)

/* DataFlash family IDs, as obtained from the second idcode byte */
#define DF_FAMILY_AT26F      0
#define DF_FAMILY_AT45      1
#define DF_FAMILY_AT26DF    2  /* AT25DF and AT26DF */

typedef struct atmel_spi_flash_params {
  u8    idcode1;
  /* Log2 of page size in power-of-two mode */
  u8    l2_page_size;
  u8    pages_per_block;
  u8    blocks_per_sector;
  u8    nr_sectors;
  const char  *p_name;
} atmel_spi_flash_params_t;

/* spi_flash needs to be first so upper layers can free() it */
typedef struct atmel_spi_flash {
  spi_flash_t flash;
  const atmel_spi_flash_params_t *p_params;
} atmel_spi_flash_t;

static inline atmel_spi_flash_t *
to_atmel_spi_flash(spi_flash_t *p_flash)
{
    u32 offset = 0;
    void *p = NULL;
   offset = (u32)& (((atmel_spi_flash_t *) 0)->flash);
   p = p_flash - offset;
  return (atmel_spi_flash_t *)p;
}

static const atmel_spi_flash_params_t atmel_spi_flash_table[] = {
  {
    /*.idcode1    = */0x22,
    /*.l2_page_size    = */8,
    /*.pages_per_block  = */8,
    /*.blocks_per_sector  = */16,
    /*.nr_sectors    = */4,
    /*.p_name      = */"AT45DB011D"
  },
  {
    /*.idcode1    = */0x23,
    /*.l2_page_size    = */8,
    /*.pages_per_block  = */8,
    /*.blocks_per_sector  = */16,
    /*.nr_sectors    = */8,
    /*.p_name      = */"AT45DB021D"
  },
  {
    /*.idcode1    = */0x24,
    /*.l2_page_size    = */8,
    /*.pages_per_block  = */8,
    /*.blocks_per_sector  = */32,
    /*.nr_sectors    = */8,
    /*.p_name      = */"AT45DB041D"
  },
  {
    /*.idcode1    = */0x25,
    /*.l2_page_size    = */8,
    /*.pages_per_block  = */8,
    /*.blocks_per_sector  = */32,
    /*.nr_sectors    = */16,
    /*.p_name      = */"AT45DB081D"
  },
  {
    /*.idcode1    = */0x26,
    /*.l2_page_size    = */9,
    /*.pages_per_block  = */8,
    /*.blocks_per_sector  = */32,
    /*.nr_sectors    = */16,
    /*.p_name      = */"AT45DB161D"
  },
  {
    /*.idcode1    = */0x27,
    /*.l2_page_size    = */9,
    /*.pages_per_block  = */8,
    /*.blocks_per_sector  = */64,
    /*.nr_sectors    = */64,
    /*.p_name      = */"AT45DB321D"
  },
  {
    /*.idcode1    = */0x28,
    /*.l2_page_size    = */10,
    /*.pages_per_block  = */8,
    /*.blocks_per_sector  = */32,
    /*.nr_sectors    = */32,
    /*.p_name      = */"AT45DB642D"
  }
};

static void at45_wait_ready(spi_flash_t *p_flash, u32 timeout)
{
    u8 resp = 0;
    s32 ret = 0;
    u32 status = 0;

    status = 1;

    while(status && timeout)
    {
        ret = spi_flash_cmd(p_flash->p_spi, CMD_AT45_READ_STATUS, &resp, 1);
        if(ret < 0)
        {
          SF_DEBUG_PRINTF("read flash status failed! %s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
          timeout --;
          continue;
        }
        if((resp&AT45_STATUS_READY) == 1)
          status = 1;
        else
        {
          status = 0;
          timeout --;
        }
    }

    if(timeout == 0)
    {
        SF_DEBUG_PRINTF("spi_flash_cmd_wait_ready: time out! %s %d %s\n",
                __FUNCTION__, __LINE__, __FILE__);
    }
}

/*
 * Assemble the address part of a command for AT45 devices in
 * non-power-of-two page size mode.
 */
static void at45_build_address(atmel_spi_flash_t *p_asf, u8 *p_cmd, u32 offset)
{
  u32 page_addr = 0;
  u32 byte_addr = 0;
  u32 page_size = 0;
  u32 page_shift = 0;

  /*
   * The "extra" space per page is the power-of-two page size
   * divided by 32.
   */
  page_shift = p_asf->p_params->l2_page_size;
  page_size = (1 << page_shift) + (1 << (page_shift - 5));
  page_shift++;
  page_addr = offset / page_size;
  byte_addr = offset % page_size;

  p_cmd[0] = page_addr >> (16 - page_shift);
  p_cmd[1] = page_addr << (page_shift - 8) | (byte_addr >> 8);
  p_cmd[2] = byte_addr;
}

static s32 dataflash_read_fast_at45(spi_flash_t *p_flash,
    u32 offset, u32 len, void *p_buf)
{
  atmel_spi_flash_t *p_asf = to_atmel_spi_flash(p_flash);
  u8 cmd[5] = {0};

  cmd[0] = CMD_READ_ARRAY_FAST;
  at45_build_address(p_asf, cmd + 1, offset);
  cmd[4] = 0x00;

  return spi_flash_read_common(p_flash, cmd, sizeof(cmd), p_buf, len);
}

/*
 * TODO: the two write funcs (_p2/_at45) should get unified ...
 */
static s32 dataflash_write_p2(spi_flash_t *p_flash,
    u32 offset, u32 len, void *p_buf)
{
  atmel_spi_flash_t *p_asf = to_atmel_spi_flash(p_flash);
  u32 page_size = 0;
  u32 addr = offset;
  u32 chunk_len = 0;
  u32 actual = 0;
  s32 ret = 0;
  u8 cmd[4] = {0};

  /*
   * TODO: This function currently uses only page buffer #1.  We can
   * speed this up by using both buffers and loading one buffer while
   * the other is being programmed into main memory.
   */

  page_size = (1 << p_asf->p_params->l2_page_size);

  if (p_flash->p_spi == NULL) {
    SF_DEBUG_PRINTF("SF: Unable to claim SPI bus %s %d %s\n",
              __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  for (actual = 0; actual < len; actual += chunk_len) {
    chunk_len = MIN(len - actual, page_size - (addr % page_size));

    /* Use the same address bits for both commands */
    cmd[0] = CMD_AT45_LOAD_BUF1;
    cmd[1] = addr >> 16;
    cmd[2] = addr >> 8;
    cmd[3] = addr;

    ret = spi_flash_cmd_write(p_flash->p_spi, cmd, 4,
         (u8 *)p_buf + actual, chunk_len);
    if (ret < 0) {
      SF_DEBUG_PRINTF("SF: Loading AT45 buffer failed  %s %d %s\n",
                  __FUNCTION__, __LINE__, __FILE__);
      return ret;
    }

    cmd[0] = CMD_AT45_PROG_BUF1;
    ret = spi_flash_cmd_write(p_flash->p_spi, cmd, 4, NULL, 0);
    if (ret < 0) {
      SF_DEBUG_PRINTF("SF: AT45 page programming failed  %s %d %s\n",
                  __FUNCTION__, __LINE__, __FILE__);
      return ret;
    }

    at45_wait_ready(p_flash, SPI_FLASH_PROG_TIMEOUT);

    addr += chunk_len;
  }

  SF_DEBUG_PRINTF("SF: AT45: Successfully programmed %u bytes @ 0x%x\n",
      len, offset);
  ret = 0;

  return ret;
}

static s32 dataflash_write_at45(spi_flash_t *p_flash,
    u32 offset, u32 len, void *p_buf)
{
  atmel_spi_flash_t *p_asf = to_atmel_spi_flash(p_flash);
  u32 page_addr = 0;
  u32 byte_addr = 0;
  u32 page_size = 0;
  u32 page_shift = 0;
  u32 chunk_len = 0;
  u32 actual = 0;
  s32 ret = 0;
  u8 cmd[4] = {0};

  /*
   * TODO: This function currently uses only page buffer #1.  We can
   * speed this up by using both buffers and loading one buffer while
   * the other is being programmed into main memory.
   */

  page_shift = p_asf->p_params->l2_page_size;
  page_size = (1 << page_shift) + (1 << (page_shift - 5));
  page_shift++;
  page_addr = offset / page_size;
  byte_addr = offset % page_size;

  if (p_flash->p_spi == NULL) {
    SF_DEBUG_PRINTF("SF: Unable to claim SPI bus  %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  for (actual = 0; actual < len; actual += chunk_len) {
    chunk_len = MIN(len - actual, page_size - byte_addr);

    /* Use the same address bits for both commands */
    cmd[0] = CMD_AT45_LOAD_BUF1;
    cmd[1] = page_addr >> (16 - page_shift);
    cmd[2] = page_addr << (page_shift - 8) | (byte_addr >> 8);
    cmd[3] = byte_addr;

    ret = spi_flash_cmd_write(p_flash->p_spi, cmd, 4,
         (u8 *)p_buf + actual, chunk_len);
    if (ret < 0) {
      SF_DEBUG_PRINTF("SF: Loading AT45 buffer failed\n  %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
      return ret;
    }

    cmd[0] = CMD_AT45_PROG_BUF1;
    ret = spi_flash_cmd_write(p_flash->p_spi, cmd, 4, NULL, 0);
    if (ret < 0) {
      SF_DEBUG_PRINTF("SF: AT45 page programming failed  %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
      return ret;
    }

    at45_wait_ready(p_flash, SPI_FLASH_PROG_TIMEOUT);

    page_addr++;
    byte_addr = 0;
  }

  SF_DEBUG_PRINTF("SF: AT45: Successfully programmed %u bytes @ 0x%x\n",
      len, offset);
  ret = 0;
  return ret;
}

/*
 * TODO: the two erase funcs (_p2/_at45) should get unified ...
 */
static s32 dataflash_erase_p2(spi_flash_t *p_flash, u32 offset, u32 len)
{
  atmel_spi_flash_t *p_asf = to_atmel_spi_flash(p_flash);
  u32 page_size = 0;

  u32 actual = 0;
  s32 ret = 0;
  u8 cmd[4] = {0};

  /*
   * TODO: This function currently uses page erase only. We can
   * probably speed things up by using block and/or sector erase
   * when possible.
   */

  page_size = (1 << p_asf->p_params->l2_page_size);

  if (offset % page_size || len % page_size) {
    SF_DEBUG_PRINTF("SF: Erase offset/length not multiple of page size  %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  cmd[0] = CMD_AT45_ERASE_PAGE;
  cmd[3] = 0x00;

  if (p_flash->p_spi == NULL) {
    SF_DEBUG_PRINTF("SF: Unable to claim SPI bus  %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  for (actual = 0; actual < len; actual += page_size) {
    cmd[1] = offset >> 16;
    cmd[2] = offset >> 8;

    ret = spi_flash_cmd_write(p_flash->p_spi, cmd, 4, NULL, 0);
    if (ret < 0) {
      SF_DEBUG_PRINTF("SF: AT45 page erase failed  %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
      return ret;
    }

    at45_wait_ready(p_flash, SPI_FLASH_PAGE_ERASE_TIMEOUT);

    offset += page_size;
  }

  SF_DEBUG_PRINTF("SF: AT45: Successfully erased %u bytes @ 0x%x\n",
      len, offset);
  ret = 0;
  return ret;
}

static s32 dataflash_erase_at45(spi_flash_t *p_flash, u32 offset, u32 len)
{
  atmel_spi_flash_t *p_asf = to_atmel_spi_flash(p_flash);
  u32 page_addr = 0;
  u32 page_size = 0;
  u32 page_shift = 0;
  u32 actual = 0;
  s32 ret = 0;
  u8 cmd[4] = {0};

  /*
   * TODO: This function currently uses page erase only. We can
   * probably speed things up by using block and/or sector erase
   * when possible.
   */

  page_shift = p_asf->p_params->l2_page_size;
  page_size = (1 << page_shift) + (1 << (page_shift - 5));
  page_shift++;
  page_addr = offset / page_size;

  if (offset % page_size || len % page_size) {
    SF_DEBUG_PRINTF("SF: Erase offset/length not multiple of page size  %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  cmd[0] = CMD_AT45_ERASE_PAGE;
  cmd[3] = 0x00;

  if (p_flash->p_spi == NULL) {
    SF_DEBUG_PRINTF("SF: Unable to claim SPI bus  %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
    return -1;
  }

  for (actual = 0; actual < len; actual += page_size) {
    cmd[1] = page_addr >> (16 - page_shift);
    cmd[2] = page_addr << (page_shift - 8);

    ret = spi_flash_cmd_write(p_flash->p_spi, cmd, 4, NULL, 0);
    if (ret < 0) {
      SF_DEBUG_PRINTF("SF: AT45 page erase failed  %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
      return ret;
    }

    at45_wait_ready(p_flash, SPI_FLASH_PAGE_ERASE_TIMEOUT);

    page_addr++;
  }

  SF_DEBUG_PRINTF("SF: AT45: Successfully erased %u bytes @ 0x%x\n",
      len, offset);
  ret = 0;
  return ret;
}

static atmel_spi_flash_t block_atmel_flash;

spi_flash_t *spi_flash_probe_atmel(spi_bus_t *p_spi, u8 *p_idcode)
{
  const atmel_spi_flash_params_t *p_params = NULL;
  u32 page_size = 0;
  u32 family = 0;
  atmel_spi_flash_t *p_asf = NULL;
  u32 i = 0;
  s32 ret = 0;
  u8 status = 0;

  for (i = 0; i < ARRAY_SIZE(atmel_spi_flash_table); i++) {
    p_params = &atmel_spi_flash_table[i];
    if (p_params->idcode1 == p_idcode[1])
      break;
  }

  if (i == ARRAY_SIZE(atmel_spi_flash_table)) {
    SF_DEBUG_PRINTF("SF: Unsupported DataFlash ID %02x %s %d %s\n",
        p_idcode[1], __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

  //p_asf = mtos_malloc(sizeof(struct atmel_spi_flash));
  p_asf = &block_atmel_flash;
  if (!p_asf) {
    SF_DEBUG_PRINTF("SF: Failed to allocate memory  %s %d %s\n",
            __FUNCTION__, __LINE__, __FILE__);
    return NULL;
  }

  p_asf->p_params = p_params;
  p_asf->flash.p_spi = p_spi;
  p_asf->flash.p_name = p_params->p_name;

  /* Assuming power-of-two page size initially. */
  page_size = 1 << p_params->l2_page_size;

  family = p_idcode[1] >> 5;

  switch (family) {
  case DF_FAMILY_AT45:
    /*
     * AT45 chips have configurable page size. The status
     * register indicates which configuration is active.
     */
    ret = spi_flash_cmd(p_spi, CMD_AT45_READ_STATUS, &status, 1);
    if (ret)
    {
         return NULL;
       }

    SF_DEBUG_PRINTF("SF: AT45 status register: %02x\n", status);

    if (!(status & AT45_STATUS_P2_PAGE_SIZE)) {
      p_asf->flash.read = dataflash_read_fast_at45;
      p_asf->flash.write = dataflash_write_at45;
      p_asf->flash.erase = dataflash_erase_at45;
      page_size += 1 << (p_params->l2_page_size - 5);
    } else {
      p_asf->flash.read = spi_flash_cmd_read_fast;
      p_asf->flash.write = dataflash_write_p2;
      p_asf->flash.erase = dataflash_erase_p2;
    }

    break;

  case DF_FAMILY_AT26F:
  case DF_FAMILY_AT26DF:
    p_asf->flash.read = spi_flash_cmd_read_fast;
    break;

  default:
    SF_DEBUG_PRINTF("SF: Unsupported DataFlash family %u  %s %d %s\n", family,
              __FUNCTION__, __LINE__, __FILE__);
       return NULL;
  }

  p_asf->flash.sector_size = page_size;
  p_asf->flash.size = page_size * p_params->pages_per_block
        * p_params->blocks_per_sector
        * p_params->nr_sectors;

  return &p_asf->flash;

}

/********************************************************
*                       END
********************************************************/
typedef struct spi_jazz_priv
{
  u8 protect;
  spi_flash_t *p_flash;
} spi_jazz_priv_t;

#define FLASH_BLOCK_SIZE (1024 * 64)

static RET_CODE spiflash_jazz_protect_all(lld_charsto_t *p_lld)
{
  spi_jazz_priv_t *p_priv = (spi_jazz_priv_t *)p_lld->p_priv;
  p_priv->protect = 1;

  return SUCCESS;
}


static RET_CODE spiflash_jazz_unprotect_all(lld_charsto_t *p_lld)
{
  spi_jazz_priv_t *p_priv = (spi_jazz_priv_t *)p_lld->p_priv;
  p_priv->protect = 0;
  return SUCCESS;
}

static RET_CODE spiflash_jazz_open(lld_charsto_t *p_lld, void *p_cfg)
{
  spi_jazz_priv_t *p_priv = (spi_jazz_priv_t *)p_lld->p_priv;
  charsto_cfg_t *p_config = (charsto_cfg_t *)p_cfg;

  /* config flash controller to common setting */
  p_priv = p_lld->p_priv = mtos_malloc(sizeof(spi_jazz_priv_t));
  if(p_priv == NULL)
  {
    SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
    return ERR_NO_MEM;
  }
  memset(p_priv, 0x00, sizeof(spi_jazz_priv_t));

  /* customer config */
  if(NULL != p_config)
  {
    if(p_config->p_bus_handle == NULL)
    {
      mtos_free(p_lld->p_priv);
      p_lld->p_priv = NULL;
      SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
      return ERR_FAILURE;
    }
    p_priv->p_flash = spi_flash_probe((spi_bus_t *)p_config->p_bus_handle);
    if(p_priv->p_flash == NULL)
    {
      mtos_free(p_lld->p_priv);
      p_lld->p_priv = NULL;
      SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
      return ERR_FAILURE;
    }
  }
  else
  {
    mtos_free(p_lld->p_priv);
    p_lld->p_priv = NULL;
    SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
    return ERR_FAILURE;
  }

  /* unprotect the whole chip no matter what state it is currently */
  p_priv->protect = 1;
  spiflash_jazz_unprotect_all(p_lld);

  return SUCCESS;
}

static RET_CODE spiflash_jazz_close(lld_charsto_t *p_lld)
{
  spi_jazz_priv_t *p_priv = NULL;
  if(p_lld->p_priv != NULL)
  {
    p_priv = (spi_jazz_priv_t *)p_lld->p_priv;
    if(p_priv->p_flash != NULL)
    {
      mtos_free(p_priv->p_flash);
    }
    mtos_free(p_lld->p_priv);
    p_lld->p_priv = NULL;
  }
  return SUCCESS;
}

static RET_CODE spiflash_jazz_io_ctrl(lld_charsto_t *p_lld, u32 cmd, u32 param)
{
  spi_jazz_priv_t *p_priv = (spi_jazz_priv_t *)p_lld->p_priv;

  switch(cmd)
  {
    case CHARSTO_IOCTRL_IS_VALID_ADDR:
      if(param >= FLASH_BASE_ADDR
        && param < FLASH_BASE_ADDR + p_priv->p_flash->size)
      {
        return SUCCESS;
      }
      break;

    case CHARSTO_IOCTRL_CHECK_PROTECT:
      *((u8 *)param) = (p_priv->protect == 1) ? 1 : 0;
      break;
    case DEV_IOCTRL_POWER:
      if(DEV_POWER_SLEEP == param)
      {
        spi_flash_power_down(p_priv->p_flash);
      }
      else if(DEV_POWER_FULLSPEED == param)
      {
        spi_flash_power_up(p_priv->p_flash);
      }
      break;
    case CHARSTO_IOCTRL_ENTER_SA:
    SF_DEBUG_PRINTF("ioctl CHARSTO_IOCTRL_ENTER_SA\n");
    if(p_priv->p_flash->enter_sa)
    p_priv->p_flash->enter_sa(p_priv->p_flash);
      break;

    case CHARSTO_IOCTRL_EXIT_SA:
    SF_DEBUG_PRINTF("ioctl CHARSTO_IOCTRL_EXIT_SA\n");
    if(p_priv->p_flash->exit_sa)
    p_priv->p_flash->exit_sa(p_priv->p_flash);
      break;

    default:
      break;
  }

  return ERR_FAILURE;
}

/*!
   Reading Flash appointed length(in bytes) of data to the appointed memory buffer.
 */
static RET_CODE spiflash_jazz_read(lld_charsto_t *p_lld, u32 addr, u8 *p_buf, u32 len)
{
  spi_jazz_priv_t *p_priv = NULL;
  spi_flash_t *p_flash = NULL;
  s32 ret = 0;

  p_priv = (spi_jazz_priv_t *)p_lld->p_priv;
  p_flash = p_priv->p_flash;
  if(p_priv == NULL || p_buf == NULL || (addr + len) >  p_flash->size)
  {
    SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
    return ERR_FAILURE;
  }

  if(len == 0)
  {
    SF_DEBUG_PRINTF("!!len=0   %s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
    return SUCCESS;
  }
  ret = spi_flash_read(p_flash, addr, len, (void *)p_buf);
  if(ret < 0)
  {
    SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
    return ERR_FAILURE;
  }
  else
  {
    return SUCCESS;
  }
}

static RET_CODE spiflash_jazz_writeonly(lld_charsto_t *p_lld,u32 addr,u8 *p_buf,u32 len)
{
  spi_jazz_priv_t *p_priv = (spi_jazz_priv_t *)p_lld->p_priv;
  spi_flash_t *p_flash = NULL;
  s32 ret = 0;

  p_flash = p_priv->p_flash;

  if(p_priv == NULL || p_buf == NULL || (addr + len) >  p_flash->size)
  {
    SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
    return ERR_FAILURE;
  }

  if(p_priv->protect == 1)
  {
    return ERR_FAILURE;
  }

  if(len == 0)
  {
    SF_DEBUG_PRINTF("!!len=0   %s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
    return SUCCESS;
  }

  ret = spi_flash_write(p_flash, addr, len, (void *)p_buf);
  if(ret < 0)
  {
    SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
    return ERR_FAILURE;
  }
  else
  {
    return SUCCESS;
  }

}

/*!
   Erase flash the appointed number of sectors from appointed start sector address.
 */
static RET_CODE spiflash_jazz_erase(lld_charsto_t *p_lld, u32 addr, u32 sec_cnt)
{
  spi_jazz_priv_t *p_priv = (spi_jazz_priv_t *)p_lld->p_priv;
  spi_flash_t *p_flash = NULL;
  s32 ret = 0;
  u32 block_num = 0;
  u32 erase_size = 0;
  u32 erase_block_cnt = 0;
  u32 remain = 0;
  u8 *p_sector_buf = NULL;

  p_flash = p_priv->p_flash;
  if(p_priv == NULL)
  {
    SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
    return ERR_FAILURE;
  }
  if(p_priv->protect == 1)
  {
    return ERR_FAILURE;
  }
  block_num = addr / FLASH_BLOCK_SIZE;
  addr = block_num * FLASH_BLOCK_SIZE;

  if((addr + sec_cnt * FLASH_BLOCK_SIZE) > p_flash->size)
  {
    SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
    return ERR_FAILURE;
  }

  erase_size = sec_cnt * FLASH_BLOCK_SIZE;
  erase_block_cnt = erase_size /p_flash->sector_size;
  remain = erase_size % p_flash->sector_size;

  while(erase_block_cnt)
  {
      ret = spi_flash_erase(p_flash, addr,p_flash->sector_size);
      if(ret < 0)
      {
        SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
        return ERR_FAILURE;
      }
      erase_block_cnt --;
      addr += p_flash->sector_size;
  }

  if(remain != 0)
  {
       p_sector_buf = mtos_malloc(p_flash->sector_size);
       if(p_sector_buf == NULL)
      {
          SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
          return ERR_FAILURE;
      }
      ret = spi_flash_read(p_flash, addr, p_flash->sector_size, p_sector_buf);
      if(ret < 0)
      {
        SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
        return ERR_FAILURE;
      }
      ret = spi_flash_erase(p_flash, addr,p_flash->sector_size);
      if(ret < 0)
      {
        SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
        return ERR_FAILURE;
      }
      ret = spi_flash_write(p_flash, addr + remain,p_flash->sector_size - remain,
                        p_sector_buf + remain);
      if(ret < 0)
      {
        SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
        return ERR_FAILURE;
      }
      mtos_free(p_sector_buf);
  }

  return SUCCESS;
}

RET_CODE spiflash_jazz_get_capacity(struct lld_charsto *p_lld, u32 *p_cap)
{
  spi_jazz_priv_t *p_priv = (spi_jazz_priv_t *)p_lld->p_priv;
  spi_flash_t *p_flash = NULL;

  if((p_priv == NULL) || (p_priv->p_flash == NULL))
  {
    SF_DEBUG_PRINTF("%s %d %s\n", __FUNCTION__, __LINE__, __FILE__);
    return ERR_FAILURE;
  }

  p_flash = p_priv->p_flash;
  if(p_cap)
  {
    *p_cap = p_flash->size;
  }
  return SUCCESS;
}

static void spiflash_jazz_detach(lld_charsto_t *p_lld)
{
}

RET_CODE spiflash_jazz_attach(char *p_name)
{
  charsto_device_t *p_dev = NULL;
  device_base_t *p_base = NULL;
  lld_charsto_t *p_lld = NULL;

  if((p_dev = dev_allocate((char *)p_name, SYS_DEV_TYPE_CHARSTO,
                           sizeof(struct charsto_device),
                           sizeof(struct lld_charsto))) == NULL)
  {
    return ERR_FAILURE;
  }

  /* link base */
  p_base = (device_base_t *)p_dev->p_base;
  p_base->open = (RET_CODE (*)(void *, void *))spiflash_jazz_open;
  p_base->close = (RET_CODE (*)(void *))spiflash_jazz_close;
  p_base->detach = (void (*)(void *))spiflash_jazz_detach;
  p_base->io_ctrl = (RET_CODE (*)(void *, u32, u32))spiflash_jazz_io_ctrl;

  /* attach lld */
  p_lld = (lld_charsto_t *)p_dev->p_priv;
  p_lld->read = spiflash_jazz_read;
  p_lld->writeonly = spiflash_jazz_writeonly;
  p_lld->erase = spiflash_jazz_erase;
  p_lld->protect_all = spiflash_jazz_protect_all;
  p_lld->unprotect_all = spiflash_jazz_unprotect_all;
  p_lld->get_capacity = spiflash_jazz_get_capacity;

  return SUCCESS;
}
