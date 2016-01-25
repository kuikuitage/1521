/******************************************************************************/
/******************************************************************************/
#ifndef __CHARSTO_H__
#define __CHARSTO_H__

/*!
    Char-storage device's virtual sector size
  */
#define CHARSTO_SECTOR_SIZE (64 * 1024)

/*!
   The virtual sector size's bit offset, used for bit shift operation
  */
#define CHARSTO_SECTOR_SIZE_BO (16)

/*!
    SPI Flash read mode
  */
typedef enum spi_read_mode 
{
    /*!
        SPI Flash default mode,if the user don't know which to use.
      */ 
    SPI_READ_DEFAULT_MODE = 0,
    /*!
        SPI Flash normal read mode
      */    
    SPI_NR_MODE , 
    
    /*!
        SPI Flash fast read mode
      */
    SPI_FR_MODE,
    
    /*!
        SPI Flash dual output read mode
      */
    SPI_DOR_MODE, 
    
     /*!
        SPI Flash quad output read mode
      */
    SPI_QOR_MODE,
    
     /*!
        SPI Flash 2IO read mode, no mode phase
      */
    SPI_2IO_MODE,

     /*!
        SPI Flash 4IO read mode,have mode phase
      */
    SPI_4IO_QIOR_MODE,

    /*!
        SPI Flash dual I/O read mode, have mode phase
      */
    SPI_DIOR_MODE, 
   
}spi_read_mode_t;

/*!
    SPI Flash Program mode
  */
typedef enum spi_program_mode{
    /*!
    SPI Flash default program mode
      */
    SPI_PRGM_DEFAULT_MODE = 0,
 
    /*!
      SPI Flash page program mode
      */
    SPI_PP_MODE,

    /*!
      SPI Flash 4IO page program mode, the address on 4 I/O pins
      */
    SPI_4PP_MODE,

    /*!
      SPI Flash Quad page program read mode, the address on 1 I/O pin
      */
    SPI_QPP_MODE
}spi_pgm_mode_t;

/*!
    SPI Flash clock divider from the system clock
  */
typedef enum spi_clock
{
    /*!
    Fspi = Fsys / 2
      */
    FLASH_C_CLK_P2 = 1,
    /*!
    Fspi = Fsys / 4
      */    
    FLASH_C_CLK_P4,
    /*!
    Fspi = Fsys / 6
      */
    FLASH_C_CLK_P6,
    /*!
    Fspi = Fsys / 8
      */    
    FLASH_C_CLK_P8    
}spi_clock_t;

/*!
    SPI Flash Transfer Mode
  */
typedef enum spi_transfer_mode{
    /*!
    FLASH read/write by default control,used FLASH_CPU_RW_CACHE in default
      */  
    FLASH_DEFAULT_RW = 0,
    /*!
    FLASH read/write by software control
      */  
    FLASH_PIO_RW,
    /*!
    FLASH read/write by hardware cacheable control
      */  
    FLASH_CPU_RW_CACHE,
    /*!
    FLASH read/write by hardware nocacheable control
      */
    FLASH_CPU_RW_NOCACHE,
    /*!
    FLASH read/write by dma control
      */  
    FLASH_DMA_RW,

}spi_trs_mode_t;

/*!
    Char storage device structure
  */
typedef struct charsto_device
{
    /*!
    Device base control
      */
    void *p_base;
    /*!
    Device private handle
      */    
    void *p_priv;
} charsto_device_t;

/*!
    charsto sectors protect info
*/
typedef struct charsto_prot_secs
{
  /*!
      sector protection, start sector 
   */
  u32 protect_s; 
  /*!
      sector protection, end sector  
   */
  u32 protect_e; 
}charsto_prot_secs_t;

/*!
    Char storage I/O control cammand
  */
typedef enum charsto_iocrl_cmd
{
    /*!
        Check if it is a valid address in the storage device
      */
    CHARSTO_IOCTRL_IS_VALID_ADDR = DEV_IOCTRL_TYPE_UNLOCK + 0,
    /*!
        Check if the whole char-storage device is protected
      */    
    CHARSTO_IOCTRL_CHECK_PROTECT = DEV_IOCTRL_TYPE_UNLOCK + 1,
    /*!
        Get device's capacity size in bytes
      */    
    CHARSTO_IOCTRL_GET_CAPACITY = DEV_IOCTRL_TYPE_UNLOCK + 2,  
    /*!
        Check if the device is in busy status 
      */    
    CHARSTO_IOCTRL_CHECK_BUSY = DEV_IOCTRL_TYPE_UNLOCK + 3,
    /*!
        Enter sa 
      */    
    CHARSTO_IOCTRL_ENTER_SA = DEV_IOCTRL_TYPE_UNLOCK + 4,
    /*!
        Exit sa 
      */    
    CHARSTO_IOCTRL_EXIT_SA = DEV_IOCTRL_TYPE_UNLOCK + 5,
    /*!
        protect specified sectors
      */    
    CHARSTO_IOCTRL_PROTECT_SECTOR = DEV_IOCTRL_TYPE_UNLOCK + 6, 
    /*!
        protect specified sectors
      */    
    CHARSTO_IOCTRL_UNPROTECT_SECTOR = DEV_IOCTRL_TYPE_UNLOCK + 7, 
    /*!
        Some customers 's projects need this requirement
      */    
    CHARSTO_IOCTRL_ERASE_MIN_SECTOR = DEV_IOCTRL_TYPE_LOCK + 0 

} charsto_ioctrl_cmd_t;

/*!
    The structure used to configure the Char-storage driver
  */
typedef struct charsto_config
{
    /*!
    The valid memory size in the storage device
      */
    u32 size;
    /*!
    The SPI clock frequency, see enum spi_clock
      */    
    u8 spi_clk;
    /*!
    The specific reading mode in the SPI device, see enum spi_read_mode
      */    
    u8 rd_mode;
    /*!
    The write mode in the SPI device, see enum spi_program_mode
      */    
    u8 pgm_mode;
    /*!
    The data transfer mode, by software, hardware cpu , or DMA,see enum spi_trs_mode_t
      */
    u8 trs_modes;
    /*!
        Must describe the current charsto device mounted on which bus device
     */
    void *p_bus_handle;
} charsto_cfg_t;


/*!
   Read random size byte from valid random-access address

   \param[in] dev The device handle.
   \param[in] addr The relative offset address of this storage device 
   \param[in] buf The data buffer.
   \param[in] len The data length.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_read(charsto_device_t *p_dev, u32 addr, 
                    u8 *p_buf, u32 len);

/*!
   Write random size byte to valid random-access address. Note: only do write operation and do not include erasure.

   \param[in] dev The device handle.
   \param[in] addr The relative offset address of this storage device 
   \param[in] buf The data buffer.
   \param[in] len The data length.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_writeonly(charsto_device_t *p_dev, u32 addr, 
                    u8 *p_buf, u32 len);

/*!
   Erase specific virtual sectors in unit of 64K bytes

   \param[in] dev The device handle.
   \param[in] addr The relative offset address of this storage device. It can be the random address in the start sector to be erased.
   \param[in] sec_cnt The count of the virtual sectors to be erased.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_erase(charsto_device_t *p_dev, u32 addr, u32 sec_cnt);

/*!
   Protect the whole storage memory from programming(including writing and erasure).

   \param[in] dev The device handle.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_protect_all(charsto_device_t *p_dev);

/*!
   Un-protect the whole storage memory (i.e. allow the whole storage memory to be programmed).

   \param[in] dev The device handle.

   \return 0 for success and others for failure.
  */
RET_CODE charsto_unprotect_all(charsto_device_t *p_dev);

#endif //__CHARSTO_H__
