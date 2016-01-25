/******************************************************************************/
/******************************************************************************/
#ifndef __HAL_DMA_H__
#define __HAL_DMA_H__

/*!
  maximal dma channel's number 
  */
#define HAL_DMA_MAX_CHAN_NUM 8

/*!
  DMA transfer type
  */ 
typedef enum
{
  /*!
    The type of memory to memory
    */
  DMA_TYPE_MEM2MEM = 0x01,  
  /*!
    The type of memory to port
    */
  DMA_TYPE_MEM2PORT = 0x02,  
  /*!
    The type of port to memory
    */
  DMA_TYPE_PORT2MEM = 0x04,   
  /*!
    The type of memory to memory
    */
  DMA_TYPE_PORT2PORT = 0x08
}hal_dma_type_t;

/*!
  DMA transfer unit
  */
typedef enum 
{
  /*!
    DMA transfer in unit of byte
    */
  DMA_TRANS_UNIT_BYTE = 0x01,
  /*!
    DMA transfer in unit of half word
    */
  DMA_TRANS_UNIT_HALF = 0x02,
  /*!
    DMA transfer in unit of word
    */
  DMA_TRANS_UNIT_WORD = 0x04
}hal_trans_unit_t;

/*!
  DMA status used by HAL_DMA_XXXX_XX_CHECK() as return value
  */
typedef enum 
{
  /*!
    The DMA is in stop status
    */
  DMA_STATUS_STOP = 0,
  /*!
    The DMA is in running status
    */
  DMA_STATUS_RUNNING,
  /*!
    The DMA is in pause status
    */  
  DMA_STATUS_PAUSE,
  /*!
    The DMA is finish a transmit status
    */  
  DMA_STATUS_DONE,
  /*!
    driver has not been inited
    */  
  DMA_STATUS_LAST = DMA_STATUS_DONE
}dma_status_t;

/*!
  DMA event
  */
typedef enum 
{
  /*!
    driver has not been inited
    */
  DMA_EVENT_NOT_INIT = 0x01,
  /*!
    a timeout occurred
    */
  DMA_EVENT_TIMEOUT = 0x02,
  /*!
    DMA transfer error happend
    */  
  DMA_EVENT_ERROR = 0x04,
  /*!
    source interval buffer transfer complete
    */  
  DMA_EVENT_SRC_INTERVAL_INTERRUPT = 0x80,
  /*!
    destination interval buffer transfer complete
    */  
  DMA_EVENT_DST_INTERVAL_INTERRUPT = 0x10,
  /*!
    DMA transfer has been completed
    */  
  DMA_EVENT_COMPLETE = 0x20,
  /*!
    driver has not been inited
    */  
  DMA_EVENT_LAST = DMA_EVENT_COMPLETE,
  /*!
    DMA event mask
    */  
  DMA_EVENT_MASK = 0x3F
}dma_event_t;

/*!
  DMA mode used by hal_dma_1d_start() and struct hal_dma_2d_param
  */
typedef enum 
{
  /*!
    The dma 2d mode, else is 1d
    */
  DMA_MODE_2D = 0x0400,
  /*!
    ALU AND operation:   src & alu_data --> dst
    */
  DMA_MODE_ALU_AND = 0x010000,
  /*!
    ALU OR operation:    src | alu_data --> dst
    */
  DMA_MODE_ALU_OR = 0x020000,
  /*!
    ALU XOR operation:   src ^ alu_data --> dst
    */
  DMA_MODE_ALU_XOR = 0x040000,
  /*!
    ALU NOT operation:   ~src --> dst
    */
  DMA_MODE_ALU_NOT = 0x080000,
  /*!
    ALU FILL the user dword, used for the operation like memset
    */
  DMA_MODE_ALU_FILL = 0x100000,
  /*!
    ALU mask
    */
  DMA_MODE_ALU_MASK = 0x1F0000,  
}dma_mode_t;

/*!
  DMA transfer's parameters
  */
typedef struct hal_dma_param
{
  /*!
    DMA mode, see dma_mode_t
    */
  u32 mode;                     
  /*!
    Total data length in byte
    */  
  u32 len;
  /*!
    filled data if use alu fill mode
    */   
  u32 alu_fill_data;
  /*!
    filled data width in byte, see hal_trans_unit_t
    */   
  u32 alu_fill_width;  
  /*!
    Source address
    */   
  u32 src_addr;
  /*!
    Destin address
    */   
  u32 dst_addr;
  /*!
    Source region width in byte, used only in 2d mode
    */   
  u32 src_width;
  /*!
    Destin region width in byte, used only in 2d mode
    */   
  u32 dst_width;
  /*!
    Source region leap size in byte, used only in 2d mode
    */   
  u32 src_leap;
  /*!
    Destin region leap size in byte, used only in 2d mode
    */   
  u32 dst_leap;
  /*!
      next node
    */
  struct hal_dma_param *p_next;
}hal_dma_param_t;

/*!
  DMA transfer's parameters
  */

/*!
  DMA channel's capacity
  */
typedef struct
{
  /*!
    DMA transfer unit size, see hal_trans_unit_t
    */  
  u8 trans_unit;
  /*!
    DMA transfer type, see hal_dma_type_t
    */  
  u8 type;  
  /*!
    DMA support 2d if 1, else not support 2d 
    */    
  u8 support_2d : 1;
  /*!
    DMA support link transfer if 1, else not support
    */    
  u8 support_link : 1;
  /*!
    DMA support alu transfer if 1, else not support
    */   
  u8 support_alu : 1;
  /*!
    DMA support big endian mode if 1, else not support
    */    
  u8 support_be : 1;
  /*!
    DMA address align word mode
    */    
  u8 addr_align_word : 1; 
  /*!
    DMA address align half-word mode
    */    
  u8 addr_align_half : 1;  
  /*!
    DMA address align byte mode
    */    
  u8 addr_align_byte : 1;   
  /*!
    DMA max transfer size in one request
    */    
  u32 max_size_one_req;  
  /*!
    DMA max transfer size in width of one line, only used for 2D
    */    
  u32 max_size_one_width;   
  /*!
    DMA max line number in height, only used for 2D
    */    
  u32 max_size_one_height;  
  /*!
    DMA max transfer size of leap, only used for 2D
    */    
  u32 max_size_one_leap;  
}hal_dma_chan_cap_t;

/*!
  DMA's capacity
  */
typedef struct 
{
  /*!
    DMA channel's number
    */
  u8 chan_num;
  /*!
    DMA channel's capacity
    */
  hal_dma_chan_cap_t chan_cap[HAL_DMA_MAX_CHAN_NUM];
}hal_dma_capacity_t;

/*!
  DMA notify callback function
  */
typedef void (*HAL_DMA_NOTIFY) (
    /*!
    The channel's id
    */
    int id,
    /*!
    The event mask
    */    
    dma_event_t event,
    /*!
    The 1st parameter
    */    
    u32 param1,
    /*!
    The 2nd parameter
    */    
    u32 param2);

/*!
  DMA notify callback structure
  */
typedef struct
{
  /*!
    The pointer of callback function
    */
  HAL_DMA_NOTIFY fn_cb;
  /*!
    The mask of event fot this notify, see dma_event_t
    */  
  u32 event_mask;
  /*!
    The 1st parameter of callback function
    */  
  u32 param1;
  /*!
    The 2nd parameter of callback function
    */  
  u32 param2;
}hal_dma_notify_t;
/*!
  aes mode
  */
typedef enum aes_mode_sel
{
  /*!
    ecb
    */
  AES_MODE_ECB = 0,
  /*!
    cbc : not support
    */
  AES_MODE_CBC
}aes_mode_sel_t;
/*!
  aes key select
  */
typedef enum aes_key_sel
{
  /*!
    default use
    */
  AES_KEY_SEL_128 = 0,
  /*!
    not support
    */
  AES_KEY_SEL_192,
  /*!
    not support
    */
  AES_KEY_SEL_256,
  /*!
    not support
    */
  AES_KEY_SEL_FORBID
}aes_key_sel_t;
/*!
  enable
  */
typedef enum enable_sel
{
  /*!
   disable aes or des
    */
  DISABLE = 0,
  /*!
    enable aes or des
    */
  ENABLE
}enable_sel_t;
/*!
  decrpt select
  */
typedef enum decrpt_enable
{
  /*!
    disbale decrpt
    */
  DECRPT_ENABLE = 0,
  /*!
    use decrpt
    */
  ENCRPT_ENABLE
}decrpt_enable_t;
/*!
  des core select
  */
typedef enum des_core_sel
{
  /*!
    des
    */
  ONE_DES_64 = 0,
  /*!
    3 des : not support
    */
  THREE_DES_64
}des_core_sel_t;
/*!
  aes des config
  */
typedef struct dma_aes_des_cfg
{
  /*!
   enable
    */
  u8 aes_enable : 1;
  /*!
    mode
    */
  u8 aes_mode : 1;
  /*!
    XX
    */
  u8 aes_key : 2;
  /*!
     XX
     */
  u8 aes_key_decrypt : 1;
  /*!
     XX
     */
  u8 des_enable : 1;
  /*!
     XX
     */
  u8 des_core : 1;
  /*!
     XX
     */
  u8 des_decrpt : 1;
  /*!
     XX
     */
  u8 aes_status ; //reserved
  /*!
     XX
     */
  u32 aes_des_key[8];
  /*!
     XX
     */
  u32 aes_vector[4];

}dma_aes_des_cfg_t;
/*!
  initiate the DMA
  
  \param[in] None

  \return SUCCESS if OK, else fail
  */
extern RET_CODE hal_dma_init(void);

/*!
  Get DMA's capacity
  
  \param[in] p_capacity the capacity of DMA

  \return SUCCESS if OK, else fail
  */
extern RET_CODE hal_dma_capacity_get(hal_dma_capacity_t *p_capacity);

/*!
  Interface used to start a DMA channel's operation
  
  \param[in] id channel id
  \param[in] p_param this thansfer's operation
  \param[in] p_notify this thansfer's notify 

  \return SUCCESS if OK, else fail
*/
extern RET_CODE hal_dma_start(int id, hal_dma_param_t *p_param, 
                                             hal_dma_notify_t *p_notify);

/*!
  Interface used to check a DMA channel's status.
  
  \param[in] id channel id

  \return SUCCESS if OK, else fail
*/
extern s32 hal_dma_check(int id);

/*!
  Interface used to stop a DMA channel's operation
  
  \param[in] id channel id

  \return see dma_status_t
*/
extern RET_CODE hal_dma_stop(int id);


/*!
  Interface used to start a link list DMA operation
  
  \param[in] id channel id
  \param[in] mode transfer mode
  \param[in] link link descriptor

  \return SUCCESS if OK, else fail
*/
extern s32 hal_dma_link_start(int id, u32 mode, void *p_link);

/*!
  Interface used to start a link list DMA operation
  
  \param[in] id channel id
  \param[in] en 1: pause, 0: resume

  \return SUCCESS if OK, else fail
*/
extern s32 hal_dma_link_pause(int id, int en);

/*!
  Interface used to stop a link list DMA operation
  
  \param[in] id channel id

  \return SUCCESS if OK, else fail
*/
extern s32 hal_dma_link_stop (int id);

/*!
  Interface used to check a link list DMA status
  
  \param[in] id channel id
  \param[in] link_item one item of a link list

  \return SUCCESS if OK, else fail
*/
extern s32 hal_dma_link_check(int id, void *p_link_item);


/*!
  Interface used to check a link list DMA status

  \param[in] id channel id
  \param[in] link_item one item of a link list

  \return SUCCESS if OK, else fail
*/
RET_CODE hal_dma_aes_des_config(u32 *param);

/*!
  Interface used to check a link list DMA status

  \param[in] id channel id
  \param[in] link_item one item of a link list

  \return SUCCESS if OK, else fail
*/
void hal_dma_aes_enable(int set);

/*!
  Interface used to check a link list DMA status

  \param[in] id channel id
  \param[in] link_item one item of a link list

  \return SUCCESS if OK, else fail
*/
void hal_dma_des_enable(int set);



#endif //__HAL_DMA_H__
