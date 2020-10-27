#include "DFRobot_CWWIRELESS.h"
sSlavemessage_t sSlavemessage;
//TX
uint8_t CWWIRELESS::slaveReadmajor(){
  String str;
  readReg(0x00,this->rxbuf,1);
  return this->rxbuf[0];
}

uint8_t CWWIRELESS::slaveReadminor(){
  String str;
  readReg(0x01,this->rxbuf,1);
  return this->rxbuf[0];
}

uint8_t CWWIRELESS::slaveStatus(){
  readReg(TX_STATUS_ADDR,this->rxbuf,1);
  switch(this->rxbuf[0]){
    case 0:
      return 1;
    case 1:              
      readReg(COIL_INDEX_ADDR,this->rxbuf,13);
      sSlavemessage.coil=rxbuf[0];
      sSlavemessage.Vin=((rxbuf[2]<< 8)&0xFF00 | rxbuf[1]);
      sSlavemessage.Iin=((rxbuf[4]<< 8)&0xFF00 | rxbuf[3]);
      sSlavemessage.RcvPwr=((rxbuf[6]<< 8)&0xFF00 | rxbuf[5]);
      sSlavemessage.Temp=(((rxbuf[2]<< 8)&0xFF00 | rxbuf[1])*((rxbuf[4]<< 8)&0xFF00 | rxbuf[3])/1000);
      sSlavemessage.Fre=(((rxbuf[10]<< 8)&0xFF00 | rxbuf[9])*100);
      sSlavemessage.Duty=((rxbuf[12]<< 8)&0xFF00 | rxbuf[11]);
      sSlavemessage.NTC=((rxbuf[8]<< 8)&0xFF00 | rxbuf[7]);
      return 0;
    case 2:
      return 2;
    default:
      return 3;
  }
}

bool CWWIRELESS::getSlavePPPPdata(uint8_t* buf,uint8_t size){
  uint8_t data_flow;
  uint8_t i = 0;
  String str="";
  readReg(TX_PPPP_DATA_STATE_ADDR,buf,1);
  data_flow = buf[0];
  if(data_flow &0x02){
    readReg(PPPP_RX_DATA_STRAT_ADDR,buf,size);
    this->txbuf[0]=data_flow & 0xFD;
    writeReg(TX_PPPP_DATA_STATE_ADDR,this->txbuf,1);
    return 0;
  }
  else
    return 1;
}

uint8_t CWWIRELESS::setSlavePPPPdata(uint8_t* buf,uint8_t size){
  uint8_t data_flow=0;
  uint8_t cmd_status = 0;
  uint8_t i = 0;
  String str="";
  while(1){
  readReg(CMD_FROM_MCU_ADDR,this->rxbuf,1);//get mcu cmd status
  cmd_status=this->rxbuf[0];
  if((cmd_status & 0x01) == 0)
	  break;
  }
  if((cmd_status & 0x01) == 0){
    readReg(TX_PPPP_DATA_STATE_ADDR,this->rxbuf,1);
    data_flow = this->rxbuf[0];//get tx_pppp data status
    uint8_t status=data_flow&0x01;
    writeReg(PPPP_TX_DATA_STRAT_ADDR,buf,size);//write data to cfs tx
    this->txbuf[0] = data_flow & 0xFE;
    writeReg(TX_PPPP_DATA_STATE_ADDR,this->txbuf,1);//clear tx_pppp_data status b0
    this->txbuf[0] = cmd_status | 0x01;
    writeReg(CMD_FROM_MCU_ADDR,this->txbuf,1);
	return 0;
  }else{
	  return 1;
  }
}

void CWWIRELESS::transferSlavestring(char* str){
  uint8_t strlength=strlen(str);
  uint8_t memlength=freeMemory();
  if(memlength<=strlength)
    return ;
  uint8_t buf[4];
  uint8_t len=strlen(str);
  uint8_t packcount=len/VALID_DATA;
  uint8_t num=len%VALID_DATA;
  for(int i=0;i<packcount;i++){
    for(int j=0;j<VALID_DATA;j++){
      buf[j]=(int)(*str);
      str++;
    }
    cuappEnqueue(buf);
  }
  for(int i =0;i<4;i++){
    buf[i]=0;
  }
  for(int i =0;i<num;i++){
    buf[i]=(int)(*str);
    str++;
  }
  cuappEnqueue(buf);
}


static bool bufIsempty(uint8_t* buf){
  for(int i=0;i<5;i++){
    if(buf[i]!=0)
      return 1;
  }
  return 0;
}

void CWWIRELESS::slaveBegintransfer(){
  if((flag1==0)&&(flag3==1)){
    ifsend=1;
    count=1;    
  }
  if(MARK==1)
    ifsend=0;
  if(Ready_to_send_packets==NULL){
    if(cumsgBufHead==NULL){
      uint8_t buffer[4]={0};
      cuappEnqueue(buffer);
    }
    Ready_to_send_packets = cuappDequeue();
    Ready_to_send_packets->data[3]=proof_test_value++;
  }
  if((ifsend==1)&&(count<2)){
    setSlavePPPPdata(previous_packets[count],4);
    MARK=previous_packets[count][3];
    ifnow=0;
  }else{
    setSlavePPPPdata(Ready_to_send_packets->data,4);
    MARK=Ready_to_send_packets->data[3];
    ifnow=1;
  }
  flag1=0;
}

uint8_t CWWIRELESS::chackSlavestate(){
  uint8_t buf[1];
  readReg(TX_PPPP_DATA_STATE_ADDR,buf,1);
  return ((buf[0]>>1)&0x01);
}

String CWWIRELESS::receiveHoststring(){
  uint8_t buf[4];
  String str;
  if(chackSlavestate()){
    flag1=1;
    getSlavePPPPdata(buf,4);
    for(int i=0;i<3;i++){
      if((buf[i]!=0)&&(buf[i]!=1))
        str+=(char)buf[i];
    }
    DBG(buf[3]);
    DBG(MARK);
    uint8_t d=MARK-1;
    uint8_t r=buf[3]-1;
    uint8_t r1=buf[3]+2;
    if((buf[3]==MARK)||(buf[3]==d)){
      if(buf[3]==MARK)
        flag3=0;
      else
        flag3=1;
      if(ifnow==1){
        if(Ready_to_send_packets!=NULL){
          for(int i=0;i<5;i++){
            previous_packets[0][i]=previous_packets[1][i];
          }
          for(int i=0;i<5;i++){
            previous_packets[1][i]=Ready_to_send_packets->data[i];
          }
          free(Ready_to_send_packets);
          Ready_to_send_packets=NULL;
        }
      }else{
        if(count==1){
          count=0;
          ifsend=0;
        }else{
          count=1;
        }
      }
    }else if(abs(buf[3]-MARK)>2){
      proof_test_value=1;
      Ready_to_send_packets->data[3]=proof_test_value++;
      for(int i=0;i<5;i++){
        previous_packets[0][i]=1;
        previous_packets[1][i]=1;
        data1=0;
      }
    }else if(r1==MARK){
      ifsend=1;
    }else{
      //ifsend=0;
    }
    if(data1==r){
      data1=buf[3];
    }else{
      str="";
    }
    return str;
  }
}

//RX
uint16_t CWWIRELESS::hostChipID(void){
  uint16_t val;
  readReg(CHIPID_ADDR,this->rxbuf,2);
  val = (this->rxbuf[0]&0xff)+(this->rxbuf[1]<<8);
  return val;
}

uint16_t CWWIRELESS::hostVout(void){
  uint16_t val;
  readReg(VOUT_ADDR,this->rxbuf,2);
  val = (this->rxbuf[0]&0xff)+(this->rxbuf[1]<<8);
  return val;
}

uint16_t CWWIRELESS::hostFreq(void){
  uint16_t val;
  readReg(PERIOD_ADDR,this->rxbuf,2);
  val = (this->rxbuf[0]&0xff)|(this->rxbuf[1]<<8);
  return val;
}

void CWWIRELESS::sendHostpacket(uint8_t* buf,uint8_t size){
  writeReg(PPP_HEADER_ADDR,buf,size);
  this->txbuf[0]=SYS_CMD_SEND_PPP;
  writeReg(SYS_CMD_ADDR,this->txbuf,1);
}

void CWWIRELESS::transferHoststring(char* str){
  uint8_t strlength=strlen(str);
  uint8_t memlength=freeMemory();
  if(memlength<=strlength)
    return ;
  uint8_t buf[5]={0x48,0};
  uint8_t len=strlen(str);
  uint8_t packcount=len/VALID_DATA;
  uint8_t num=len%VALID_DATA;
  
  for(int i=0;i<packcount;i++){
    for(int j=1;j<VALID_DATA+1;j++)
    {
      buf[j]=(int)(*str);
      str++;
    }
    cuappEnqueue(buf);
  }
  
  for(int i =1;i<6;i++){
    buf[i]=0;
  }
  
  for(int i =1;i<num+1;i++){
    buf[i]=(int)(*str);
    str++;
  }
  cuappEnqueue(buf);
}

void CWWIRELESS::hostBegintransfer(){
  if((flag1==0)&&(flag3==1)){
    ifsend=1;
    count=1;    
  }
  if(MARK==1)
    ifsend=0;
  if(Ready_to_send_packets==NULL){
    if(cumsgBufHead==NULL){
      uint8_t buffer[5]={0x48,0};
      cuappEnqueue(buffer);
    }
    Ready_to_send_packets=cuappDequeue();
    Ready_to_send_packets->data[4]=proof_test_value++;
  }
  if((ifsend==1)&&(count<2)){
    previous_packets[count][0]=0x48;
    sendHostpacket(previous_packets[count],5);
    MARK=previous_packets[count][4];
    ifnow=0;
  }else{
    sendHostpacket(Ready_to_send_packets->data,5);
    MARK=Ready_to_send_packets->data[4];
    ifnow=1;
  }
  flag1=0;
}

void CWWIRELESS::setCallback(void (*call)()){
  this->callback = call;
}

void CWWIRELESS::loop(){
  if(this->callback!=NULL){
    callback();
  }
}

uint8_t CWWIRELESS::getHostflag(){
  readReg(INTR_FLAG_ADDR,this->rxbuf,2);
  return ((rxbuf[0] >> INT_BIT4_FSK_RECV)&0x01);
}

static int pktSize(uint8_t header){
  if (header < 0x20){
    return 1;
  }else if(header < 0x80){
    return (2 + ((header - 0x20) >> 4));
  }else if(header < 0xe0){
    return (8 + ((header - 0x80) >> 3));
  }else{
    return (20 + ((header - 0xe0) >> 2));
  }
}

void CWWIRELESS::clearIntrflag(uint8_t flag){
  uint16_t clear_flag = 0;
  if(flag > 15)
    clear_flag = 0;
  else
    clear_flag = 1 << flag;
  this->txbuf[0] = (uint8_t)(clear_flag & 0x00ff);
  this->txbuf[1] = (uint8_t)((clear_flag >> 8 )& 0x00ff);
  writeReg(INTR_BIT_CLEAR_ADDR,this->txbuf,2);
  this->txbuf[0] = SYS_CMD_CLEAR_INT;
  writeReg(SYS_CMD_ADDR,this->txbuf,1);
}

uint8_t CWWIRELESS::receiveHostpacket(uint8_t* buf){
  uint8_t header,size;
  readReg(BC_HEADER_ADDR,this->rxbuf,1);
  header = this->rxbuf[0];
  buf[0]=header;
  size = pktSize(header);
  readReg(BC_HEADER_ADDR,buf+1,size+1);
  clearIntrflag(INT_BIT4_FSK_RECV);
  return size;
}

String CWWIRELESS::reciveSlavestring(){
  uint8_t rbuf[6]={0};
  int i=0;
  String newstr;
  if(getHostflag()){
    flag1=1;
    uint8_t rsize=receiveHostpacket(rbuf);
    if(rsize+1 >= 1 ){
      for(i = 2;i < rsize+1;i++){
        if((rbuf[i]!=0)&&(rbuf[i]!=1))
          newstr+=(char)rbuf[i];
      }
    }
    uint8_t d=MARK-1;
    uint8_t r=rbuf[5]-1;
    uint8_t r1=rbuf[5]+2;
    DBG(rbuf[5]);
    DBG(MARK);
    if((rbuf[5]==MARK)||(rbuf[5]==d)){
      if(rbuf[5]==MARK)
        flag3=0;
      else
        flag3=1;
      if(ifnow==1){
        if(Ready_to_send_packets!=NULL){
          for(int i=0;i<5;i++){
            previous_packets[0][i]=previous_packets[1][i];
          }
          for(int i=0;i<5;i++){
            previous_packets[1][i]=Ready_to_send_packets->data[i];
          }
          free(Ready_to_send_packets);
          Ready_to_send_packets=NULL;
        }
      }else{
        if(count==1){
          count=0;
          ifsend=0;
        }else{
          count=1;
        }
      }
    }else if(abs(rbuf[5]-MARK)>2){
      proof_test_value=1;
      Ready_to_send_packets->data[4]=proof_test_value++;
      for(int i=3;i<5;i++){
        previous_packets[0][i]=1;
        previous_packets[1][i]=1;
      }
      data1=0;
    }else if(MARK==r1){
      ifsend=1;
    }else{
      //ifsend=0;
    }
    if(data1==r){
      data1=rbuf[5];
    }else{
      newstr="";
    }
    return newstr;
  }
}

void CWWIRELESS::setItp(){
  uint8_t tx=0x10;
  writeReg(0x24,&tx,1);
}

uint8_t CWWIRELESS::getCoupling(){
  readReg(0x80,this->rxbuf,1);
  return this->rxbuf[0];
}

//queue
bool CWWIRELESS::cuappEnqueue(uint8_t* pbuf){
  struct sQueueData *p;
  p = (struct sQueueData*)malloc(sizeof(struct sQueueData));
  if(p == NULL){
    free(p);
    p=NULL;
    return 0;
  }
  p->next = NULL;
  if(cumsgBufHead==NULL){
    cumsgBufHead=p;
    cumsgBufTail=p;
  }else{
    cumsgBufTail->next = p;
    cumsgBufTail = p;
  }
  memset(p->data,0x00,5);
  memcpy(p->data,pbuf,5);
  return 1;
}

struct sQueueData* CWWIRELESS::cuappDequeue(void){
  struct sQueueData *p;
  p = cumsgBufHead;
  if(cumsgBufHead != NULL){
    cumsgBufHead = p->next;
  }else{
    return NULL;
  }
  return p;
}

//communication
CWWIRELESS_IIC::CWWIRELESS_IIC(uint8_t addr,uint8_t type)
:IIC_addr(addr){
  this->type=type;
  Wire.begin();
}

void CWWIRELESS_IIC::readReg(uint8_t reg,uint8_t* buf,uint8_t len){
  Wire.beginTransmission(this->IIC_addr);
  Wire.write(reg);
  if(this->type)
    Wire.write(0);
  uint8_t data=Wire.endTransmission();  
  if(0==Wire.requestFrom(IIC_addr,len))
    return ;
  for(int i=0;i<len;i++){
    buf[i]=Wire.read();
  }
  
}

void CWWIRELESS_IIC::writeReg(uint8_t reg,uint8_t* buf,uint8_t len){
  Wire.beginTransmission(this->IIC_addr);
  Wire.write(reg);
  if(this->type)
    Wire.write(0);
  for (uint8_t i=0; i<len; i++){
      Wire.write(buf[i]);
  }
  Wire.endTransmission();  
}