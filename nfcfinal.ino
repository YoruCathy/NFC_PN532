#include <SoftwareSerial.h>
#include <PN532_SWHSU.h>
#include <PN532.h>

#define NOTE 882                      //蜂鸣器所发声音

int state = 1;
int LedPin = 3;                       //发光二极管连接到3号引脚
int BeeperPin = 5;                    //蜂鸣器连接到5号引脚
int relayPin = 7;                   //继电器连接到7号引脚

SoftwareSerial SWSerial( 10, 11 ); // Arduino与NFC读取模块的连接串口RX, TX

PN532_SWHSU pn532swhsu( SWSerial );
PN532 nfc( pn532swhsu );

void setup(void)
{
  //设置发光二级管初始状态
  pinMode(LedPin, OUTPUT);                

  //设置蜂鸣器初始状态
  pinMode(BeeperPin, OUTPUT);
  //digitalWrite(BeeperPin, LOW);

  //设置继电器初始状态
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  
  Serial.begin(115200);
  Serial.println("Hello, User!");
  //delay(3000);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();

  //检验NFC读卡器模块是否正常连接
  if (! versiondata) {
    Serial.print("Sorry, I didn't find the NFC reading board! ");
    Serial.print("Please check the wires and press the RESET button,\n then try again! ");
    while (1); // halt
  }
  
  //显示NFC读卡器模块信息
  Serial.print("Yes! The NFC reading board has been connected correctly! ");
  Serial.println("\nHere comes its information: ");
  Serial.print("The chip is PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("And the Firmware version is "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);
  //delay(3000);
  Serial.print("OK, everything has been ready! Let's start right now!");
  //delay(3000);

  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("\nI'm waiting for a school card ...");
  Serial.println("Please put it near ...");
  Serial.println("");
}

void loop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0 };     // 储存所读取的卡的UID
  uint8_t uidLength;                        // 所读取的卡UID长度
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    uint8_t keya[4] = { 0x83, 0x5A, 0x2E, 0x44,};
    bool success=0;
    for(int i=0;i<=uidLength-1;++i){   
        if(uid[i]!=keya[i]){success=0;break;}
        else success=1;
    }
    if (success){   
        digitalWrite(LedPin,HIGH);delay(100);
        digitalWrite(LedPin,LOW);delay(100);
        digitalWrite(LedPin,HIGH);delay(100);
        digitalWrite(LedPin,LOW);delay(100);
        digitalWrite(LedPin,LOW);//控制蜂鸣器
        tone(BeeperPin, NOTE);delay(200);
        noTone(BeeperPin);
        tone(BeeperPin, NOTE);delay(200);
        noTone(BeeperPin);//控制继电器
        digitalWrite(relayPin, HIGH);delay(50);
        digitalWrite(relayPin, LOW);delay(50);
        delay(10000);
    }
    else{delay(5000);}//校验失败
    }
}