#include "Wire.h"
#include "Arduino.h"
#include <MemoryFree.h>
//Aregister belonging to TX
#define FW_VER_ADDR              0x00
#define TX_STATUS_ADDR           0x02
#define COIL_INDEX_ADDR          0x03
#define TX_VIN_ADDR              0x04
#define TX_CURRENT_IN_ADDR       0x06
#define RECEIVED_RX_POWER_ADDR   0x08
#define NTC_ADC_VAL_ADDR         0x0a
#define BRIDGE_FREQ_ADDR         0x0c
#define BRIDGE_DUTY_ADDR         0x0e
#define INPUT_POWER_LIMIT_ADDR   0x10
#define CMD_FROM_MCU_ADDR        0x20
#define TX_PPPP_DATA_STATE_ADDR  0x21
#define PPPP_TX_DATA_STRAT_ADDR  0x22
#define PPPP_RX_DATA_STRAT_ADDR  0x26 

//Aregister belonging to RX
#define CHIPID_ADDR              0x00
#define HW_VER_ADDR              0x04
#define FW_VER_ADDR              0x08
#define SYS_CMD_ADDR             0x20
#define INTR_FLAG_ADDR           0x26
#define INTR_BIT_CLEAR_ADDR      0x28
#define PERIOD_ADDR              0x2a
#define IOUT_ADDR                0x30
#define VOUT_ADDR                0x32
#define VRECT_ADDR               0x34
#define VOUTSET_ADDR             0x36
#define ILIM_ADDR                0x3a
#define VLIM_ADDR                0x3c
#define DIE_TEMP_ADDR            0x50
#define PPP_HEADER_ADDR          0x60
#define BC_HEADER_ADDR           0x76
#define BC_MSG_ADDR              0x77 
#define TRIM_VERSION_ADDR        0XEF


#define INT_BIT0_OCP             0
#define INT_BIT1_OVP             1
#define INT_BIT2_PLDO            2
#define INT_BIT3_OTP             3
#define INT_BIT4_FSK_RECV        4
#define INT_BIT5_EPP             5
#define INT_BIT6_LDO_ON          6
#define INT_BIT7_LDO_OFF         7
#define INT_BIT15_POWER_ON       15


#define SYS_CMD_CLEAR_INT        0x01
#define SYS_CMD_SEND_PPP         0x02
#define SYS_CMD_OVP_CHANGE       0x10
#define SYS_CMD_OCP_CHANGE       0x20
#define SYS_CMD_VOUT_CHANGE      0x40

#define VALID_DATA               3
#define SLAVE_CHEAK_DATA         3
#define HOST_CHEAK_DATA          4

#define PACKET_INTERVAL          1000

//#define ENABLEDBG

#ifdef ENABLEDBG
#define DBG(sth)   Serial.print("DBG:");Serial.print(__LINE__);Serial.print("        ");Serial.println(sth)
#else
#define DBG(sth)
#endif

typedef struct
{
  uint8_t coil;
  uint16_t Vin;
  uint16_t Iin;
  uint16_t RcvPwr;
  uint16_t Temp;
  uint16_t Fre;
  uint16_t Duty;
  uint16_t NTC;
}sSlavemessage_t;
extern sSlavemessage_t sSlavemessage;

struct sQueueData{
  struct sQueueData *next;
  uint8_t data[5];
};

class CWWIRELESS
{
  public:
    bool flag1=1;//处理丢包
    bool flag2=0;//
    bool flag3=0;//处理收发包序号相同丢包问题
    bool flag4=0;
    uint8_t count = 0;
    uint8_t ifnow=1;
    uint8_t ifsend=0;
    uint8_t proof_test_value=1;
    uint8_t MARK=1;
    uint8_t data1=0;
    struct sQueueData * Ready_to_send_packets;
    uint8_t previous_packets[2][5]={{0,0,0,1,1},{0,0,0,1,1}};
    //TX
    uint8_t type;
    CWWIRELESS(){};
    ~CWWIRELESS(){};
    uint8_t slaveReadmajor();
    uint8_t slaveReadminor();
    void transferSlavestring(char* str);
    uint8_t slaveStatus();
    String receiveHoststring();
    bool getSlavePPPPdata(uint8_t* buf,uint8_t size);
    uint8_t setSlavePPPPdata(uint8_t* buf,uint8_t size);
    uint8_t chackSlavestate();
    void slaveBegintransfer();
    String respondHost();
    //RX
    uint16_t hostChipID(void);
    uint16_t hostVout(void);
    uint16_t hostFreq(void);
    void sendHostpacket(uint8_t* buf,uint8_t size);
    void transferHoststring(char* str);
    uint8_t getHostflag();
    void clearIntrflag(uint8_t flag);
    uint8_t receiveHostpacket(uint8_t* buf);
    //String reciveSlavestring(uint16_t str);
    void setItp();
    String reciveSlavestring();
    void hostBegintransfer();
    uint8_t getCoupling();
    //queue
    void (*callback)();
    struct sQueueData *cumsgBufHead=NULL;
    struct sQueueData *cumsgBufTail=NULL;
    bool cuappEnqueue(uint8_t *pbuf);
    struct sQueueData* cuappDequeue();
    void setCallback(void (*call)());
    void loop();
    //communication 
    virtual void readReg(uint8_t reg,uint8_t* buf,uint8_t len)=0;
    virtual void writeReg(uint8_t reg,uint8_t* buf,uint8_t len)=0;
    String oldstr="";
    uint8_t flag=1;
  private:
    uint8_t rxbuf[6];
    uint8_t txbuf[6];
    int Ttime;
    int p;
    void (*txCallback)();
    void (*rxCallback)();
};

class CWWIRELESS_IIC:public CWWIRELESS
{
  public:
    CWWIRELESS_IIC(uint8_t addr,uint8_t type);
    ~CWWIRELESS_IIC(){};
    void readReg(uint8_t reg,uint8_t* buf,uint8_t len);
    void writeReg(uint8_t reg,uint8_t* buf,uint8_t len);
  private:
    uint8_t IIC_addr;
};