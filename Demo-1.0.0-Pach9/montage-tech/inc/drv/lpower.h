/******************************************************************************/
/******************************************************************************/

#ifndef __LPOWER_H__
#define __LPOWER_H__

/*!
    low power manager device structure
  */
typedef struct lpower_device
{
    /*!
        Device base control
      */
    void *p_base;
    /*!
        Device private handle
      */    
    void *p_priv;
} lpower_device_t;

/*!
    Power config
*/
typedef enum ctrl_cmd
{
    /*!
        key wake up
    */
    KEY_WAKE_UP = (0x1 << 0),
    /*!
        time wake up
    */
    TIME_WAKE_UP = (0x1 << 1),    
    /*!
        The front panel don't display 
      */
    FP_NO_DISPLAY = (0x1 << 2),
    /*!
        The front panel display standby time.
      */
    FP_DISPLAY_STANDBY_TIME = (0x1 << 3),
    /*!
        The front panel display local time.
      */
    FP_DISPLAY_LOCAL_TIME = (0x1 << 4),
    /*!
        Reserve 
    */
    FP_DISPLAY_MISC = (0x1 << 5),
    /*!
        Into low power mode but can't display standby time.
    */
    IN_LOW_POWER = (0x1 << 30),
    /*!
        Don't Into low power mode can display standby time.
    */
    NO_IN_LOW_POWER = (0x1 << 31),
        /*!
        set standby time
    */
    SET_STANDBY_TIME = (0x1 <<10),
     /*!
        LED_DIS_CHAR
    */
    LED_DIS_CHAR = (0x1 <<12),
         /*!
        LED_DIS_CHAR
    */
    LED_DIS_NO = (0x1 <<14),
         /*!
        LED_DIS_TIME
    */
    LED_DIS_TIME = (0x1 <<13),
        /*!
        get standby time
    */
    GET_STANDBY_TIME = (0x1 <<11)
} ctrl_cmd_t;

/*!
  User gpio config
*/
typedef struct lpower_gpio_cfg
{
  /*!
      Pin mux:set used as gpio
  */
  u8 pin_mux;
  /*!
      Pin number
  */
  u8 gpio_num;
  /*!
      High active(1) or low active(0)
  */
  BOOL gp_active;
  /*!
      Open(1) or close(0)
  */
  BOOL on_off;
} lpower_gpio_cfg_t;

/*!
  User gpio led config
*/
typedef struct lpower_led_cfg
{
  /*!
      Pin number
  */
  u8 gpio_num;
  /*!
      High active(1) or low active(0)
  */
  BOOL gp_active;
} lpower_led_cfg_t;

/*!
  front panel type
  */
enum fp_type {
    /*!
      CT1642
    */
  CT1642    = 0,
    /*!
      TM1635
    */
  TM1635    = 1,
      /*!
      FD620
    */
  FD620    = 2,
    /*!
      FD650
    */
  FD650    = 3 ,
      /*!
      FD650
    */
  FPGPIO    = 4 ,
    /*!
      type max
    */
  TYPE_MAX_CNT,
};

/*!
  front panel type
  */
enum fp_wakeup {
    /*!
      wake up by interrupt
    */
  WAKE_UP_INT    = 0,
    /*!
      wake up by normal
    */
  WAKE_UP_NOR    = 1,
      /*!
      type max
    */
 WAKE_MAX_CNT,
};

/*!
  int_edge for gpio
  */
enum int_edge {
    /*!
      rising up edge trigger
    */
  RISING_UP_INT    = 0,
    /*!
      falling down edge trigger
    */
  FALLING_DOWN_INT    = 1,
      /*!
      type max
    */
 EDGE_MAX_CNT,
};

/*!
  int_edge for gpio
  */
enum int_gpio {
    /*!
      pin 0
    */
  GPIO_INT_0    = 0,
    /*!
      pin 1
    */
  GPIO_INT_1    = 1,
      /*!
      type max
    */
 INT_MAX_CNT,
};

/*!
  standby cpu time struct
  */
typedef struct standby_time
{
  /*!
      get standby current  hour
    */   
  u8 cur_hour;
    /*!
      get standby current  min
    */   
  u8 cur_min;
      /*!
      get standby current  sec
    */   
  u8 cur_sec;
 /*!
      standby run pass day
    */   
  u8 pass_day;
}standby_time_t;


/*!
  standby cpu moudle 
*/
typedef struct lpower_standby_cfg
{
     /*!
      fp type   //default is TM1642 type = 0 ;
    */     
  enum fp_type type;
     /*!
      wake up mode,interrupt or normal
    */     
  enum fp_wakeup wake_type;
      /*!
      interrupt mode for gpio;
    */     
  enum int_edge int_edge_type;
      /*!
      interrupt pin for gpio ;
    */     
  enum int_gpio int_gpio_pin;
     /*!
      led src bitmap
    */
  u8 *p_raw_map;  
       /*!
      power down key   if the standby key is zero you must set pd_key0
    */   
  u8 pd_key0;
    /*!
      power down key
    */   
  u8 pd_key1;
      /*!
      power down key for front panel
    */  
  u8 pd_key_fp;    
       /*!
      used for gpio power pin
    */  
  u8 gpio_power_pin;   
       /*!
      used for fp clk pin
    */  
  u8 gpio_clk_pin;    
       /*!
      used for fp data pin
    */  
  u8 gpio_data_pin;   
       /*!
      time to wake up
    */  
  u32 time_wake_up;  
  
} lpower_standby_cfg_t;
/*!
    User config
  */
typedef struct lpower_cfg
{
    /*!
        Set if into low power mode.
      */    
  u32 low_power_flag;
  /*!
      enter standby param
  */
  utc_time_t *p_tim;
  /*!
      Key map, irda key, keyboard key and reserve
  */
  u16 key_map[3];
  /*!
      gpio cfg
  */
  lpower_gpio_cfg_t gpio_cfg[3];
  /*!
  frontpanel led num 
  */
  u8 led_num;
  /*!
  power polar
  */
  BOOL polar;
  /*!
  customer:oldman_biss....
  */
  u8 customer;
  /*!
  fp_type
  */
  u8 fp_type;
  /*!
    lp power code addr
    */
  u8 *p_code_addr; 
   /*!
    lp power code size
    */
  u32 code_size; 
   /*!
  the logo to display
  */
  u8 display_char[4]; 
  /*!
      lpower led num
  */
    u8 power_led_num; 
    /*!
      lpower led cfg
  */
    lpower_led_cfg_t led_gpio_cfg[3]; 
   /*!
      lpower in sram debug
  */
  u8 debug_flag;
  /*!
      lpower_standby_cfg 
  */
  lpower_standby_cfg_t standby_cfg;
} lpower_cfg_t;


/*!
   \return 0 for success and others for failure.
  */
RET_CODE ap_lpower_ioctl(lpower_device_t *p_dev, u32 cmd, u32 param);

/*!
   \enter into standby.
  */
void ap_lpower_enter(lpower_device_t *p_dev);

#endif
