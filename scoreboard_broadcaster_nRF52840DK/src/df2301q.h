/******************************************************************************
  * @file    df2301q.h
  * @author  Markel Robregado
  * @Date    May 8, 2024
  ******************************************************************************
  */

#ifndef DF2301Q_H_
#define DF2301Q_H_

#ifdef __cplusplus
 extern "C" {
#endif


/******************************************************************************/
/*************************** I2C REGISTER MAPPING  ****************************/
/******************************************************************************/
#define DF2301Q_I2C_ADDR                 0x64   //!< I2C address

#define DF2301Q_I2C_REG_CMDID            0x02   //!< Address of the register for requesting command word ID
#define DF2301Q_I2C_REG_PLAY_CMDID       0x03   //!< Address of the register for playing audio by command word ID
#define DF2301Q_I2C_REG_SET_MUTE         0x04   //!< Register for setting mute mode
#define DF2301Q_I2C_REG_SET_VOLUME       0x05   //!< Register for setting volume
#define DF2301Q_I2C_REG_WAKE_TIME        0x06   //!< Address of the register for wake-up time

/******************************************************************************/
/*************************** UART COMMUNICATION  ******************************/
/******************************************************************************/
#define DF2301Q_UART_BAUDRATE                       9600
#define DF2301Q_UART_MSG_DATA_MAX_SIZE              8            //!< Maximum data length of serial data frame
/*header*/
#define DF2301Q_UART_MSG_HEAD_LOW                   0xF4
#define DF2301Q_UART_MSG_HEAD_HIGH                  0xF5
#define DF2301Q_UART_MSG_HEAD                       ((DF2301Q_UART_MSG_HEAD_HIGH<<8)|DF2301Q_UART_MSG_HEAD_LOW)
/*tail*/
#define DF2301Q_UART_MSG_TAIL                       0xFB
/*msgType*/
#define DF2301Q_UART_MSG_TYPE_CMD_UP                0xA0
#define DF2301Q_UART_MSG_TYPE_CMD_DOWN              0xA1
#define DF2301Q_UART_MSG_TYPE_ACK                   0xA2
#define DF2301Q_UART_MSG_TYPE_NOTIFY                0xA3
/*msgCmd*/
#define DF2301Q_UART_MSG_CMD_ASR_RESULT             0x91         //Report voice recognition results
#define DF2301Q_UART_MSG_CMD_PLAY_VOICE             0x92         //Play local broadcast audio
#define DF2301Q_UART_MSG_CMD_GET_FLASHUID           0x93         //Read the serial number of FLASH
#define DF2301Q_UART_MSG_CMD_GET_VERSION            0x94         //Read version number
#define DF2301Q_UART_MSG_CMD_RESET_MODULE           0x95         //Reset the voice module
#define DF2301Q_UART_MSG_CMD_SET_CONFIG             0x96         //Settings
#define DF2301Q_UART_MSG_CMD_ENTER_OTA_MODE         0x97         //Enter update mode
#define DF2301Q_UART_MSG_CMD_NOTIFY_STATUS          0x9A         //Event notification
#define DF2301Q_UART_MSG_CMD_ACK_COMMON             0xAA
/* !!! if user want add please add form DF2301Q_UART_MSG_CMD_USER_START*/
#define DF2301Q_UART_MSG_CMD_USER_START             0xB0
/*msgData  msgCmd:DF2301Q_UART_MSG_CMD_PLAY_VOICE*/
#define DF2301Q_UART_MSG_DATA_PLAY_START            0x80
#define DF2301Q_UART_MSG_DATA_PLAY_PAUSE            0x81
#define DF2301Q_UART_MSG_DATA_PLAY_RESUME           0x82
#define DF2301Q_UART_MSG_DATA_PLAY_STOP             0x83
#define DF2301Q_UART_MSG_DATA_PLAY_BY_VOICEID       0x90
#define DF2301Q_UART_MSG_DATA_PLAY_BY_SEMANTIC_ID   0x91
#define DF2301Q_UART_MSG_DATA_PLAY_BY_CMD_ID        0x92
/*msgData  msg_cmd:DF2301Q_UART_MSG_CMD_GET_VERSION*/
#define DF2301Q_UART_MSG_DATA_VER_PROTOCOL          0x80         // Serial protocol version number
#define DF2301Q_UART_MSG_DATA_VER_SDK               0x81         // SDK version number
#define DF2301Q_UART_MSG_DATA_VER_ASR               0x82         // ASR component version number
#define DF2301Q_UART_MSG_DATA_VER_PREPROCESS        0x83         // Audio pre-processing algorithm version number
#define DF2301Q_UART_MSG_DATA_VER_PLAYER            0x84         // Player version number
#define DF2301Q_UART_MSG_DATA_VER_APP               0x8A         // App version number
/*msgData  msg_cmd:DF2301Q_UART_MSG_CMD_NOTIFY_STATUS*/
#define DF2301Q_UART_MSG_DATA_NOTIFY_POWERON        0xB0
#define DF2301Q_UART_MSG_DATA_NOTIFY_WAKEUPENTER    0xB1
#define DF2301Q_UART_MSG_DATA_NOTIFY_WAKEUPEXIT     0xB2
#define DF2301Q_UART_MSG_DATA_NOTIFY_PLAYSTART      0xB3
#define DF2301Q_UART_MSG_DATA_NOTIFY_PLAYEND        0xB4
/*msgData msg_cmd:DF2301Q_UART_MSG_CMD_SET_CONFIG*/
#define DF2301Q_UART_MSG_CMD_SET_VOLUME             0x80
#define DF2301Q_UART_MSG_CMD_SET_ENTERWAKEUP        0x81
#define DF2301Q_UART_MSG_CMD_SET_PRT_MID_RST        0x82
#define DF2301Q_UART_MSG_CMD_SET_MUTE               0x83
#define DF2301Q_UART_MSG_CMD_SET_WAKE_TIME          0x84
#define DF2301Q_UART_MSG_CMD_SET_NEEDACK            0x90
#define DF2301Q_UART_MSG_CMD_SET_NEEDSTRING         0x91
/*ACK error code*/
#define DF2301Q_UART_MSG_ACK_ERR_NONE               0x0
#define DF2301Q_UART_MSG_ACK_ERR_CHECKSUM           0xff
#define DF2301Q_UART_MSG_ACK_ERR_NOSUPPORT          0xfe

/******************************************************************************/
/*************************** COMMAND WORDS  ******************************/
/******************************************************************************/
#define TEAM_HOME_PLUS_ONE_POINT                    0x05
#define TEAM_HOME_MINUS_ONE_POINT                   0x06
#define TEAM_GUEST_PLUS_ONE_POINT                   0x07
#define TEAM_GUEST_MINUS_ONE_POINT                  0x08
#define TEAM_HOME_PLUS_ONE_SET                      0x09
#define TEAM_HOME_MINUS_ONE_SET                     0x0A
#define TEAM_GUEST_PLUS_ONE_SET                     0x0B
#define TEAM_GUEST_MINUS_ONE_SET                    0x0C
#define TEAM_HOME_SERVING                           0x0D 
#define TEAM_GUEST_SERVING                          0x0E 
#define SCORE_BOARD_RESET                           0x0F

#define TEAM_HOME_SERVING_BIT                       0  //1
#define TEAM_GUEST_SERVING_BIT                      1  //2


/**
* @struct sUartMsg_t
* @brief Serial data frame   struct
*/
typedef struct
{
    uint16_t header;
    uint16_t dataLength;
    uint8_t msgType;
    uint8_t msgCmd;
    uint8_t msgSeq;
    uint8_t msgData[DF2301Q_UART_MSG_DATA_MAX_SIZE];
    uint16_t chkSum;   /* send add auto */
    uint8_t tail;   /* send add auto */
}sUartMsg_t;

/**
* @enum eReceiveState_t
* @brief receive use state machine method, so two char can arrive different time
*/
typedef enum
{
    REV_STATE_HEAD0   = 0x00,
    REV_STATE_HEAD1   = 0x01,
    REV_STATE_LENGTH0 = 0x02,
    REV_STATE_LENGTH1 = 0x03,
    REV_STATE_TYPE    = 0x04,
    REV_STATE_CMD     = 0x05,
    REV_STATE_SEQ     = 0x06,
    REV_STATE_DATA    = 0x07,
    REV_STATE_CKSUM0  = 0x08,
    REV_STATE_CKSUM1  = 0x09,
    REV_STATE_TAIL    = 0x0a,
}eRecvState_t;


/**
  * @fn getCMDID
  * @brief Get the ID corresponding to the command word
  * @return Return the obtained command word ID, returning 0 means no valid ID is obtained
  */
uint8_t getCMDID(void);

 /**
  * @fn playByCMDID
  * @brief Play the corresponding reply audio according to the command word ID
  * @param play_id - Command word ID
  * @return None
  */
void playByCMDID(uint32_t play_id);

 /**
  * @fn resetModule
  * @brief Reset module
  * @return None
  */
void resetModule(void);

 /**
  * @fn settingCMD
  * @brief Set commands of the module
  * @param setType - Set type
  * @n       DF2301Q_UART_MSG_CMD_SET_VOLUME : Set volume, the set value range 1-7
  * @n       DF2301Q_UART_MSG_CMD_SET_ENTERWAKEUP : Enter wake-up state; set value 0
  * @n       DF2301Q_UART_MSG_CMD_SET_MUTE : Mute mode; set value 1: mute, 0: unmute
  * @n       DF2301Q_UART_MSG_CMD_SET_WAKE_TIME : Wake-up duration; the set value range 0-255s
  * @param setValue - Set value, refer to the set type above for the range
  * @return None
  */
void settingCMD(uint8_t setType, uint32_t setValue);

#ifdef __cplusplus
}
#endif

#endif /* DF2301Q_H_ */

 /*****END OF FILE****/
