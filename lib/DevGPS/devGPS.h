#pragma once
#include "device.h"
#include "TinyGPS++.h"
#include "OTA.h"
#include "FHSS.h"
#include "common.h"
#include <functional>

#define KEY8    (uint8_t)0x8
#define KEY16   (uint16_t)0x16
#define KEY32   (uint32_t)0x0032
#define BOXID   (uint8_t)0x02

#define FKEY8   fkey8Encode()
#define FKEY16   fkey16Encode()
#define FKEY32   fkey32Encode()
#define FBOXID   fboxidEncode()

#define SERVICE_CHANNEL 1
#define SYNC_CHANNEL 5
#define TICK_CHANNEL 20 + BOXID
#define MAX_PERIOD_MILLIS 5000
#define MAX_BOXES 50
#define INTERVAL (MAX_PERIOD_MILLIS / MAX_BOXES)
#define START_TIME INTERVAL * BOXID
#define BUFFER (uint32_t)(INTERVAL * 0.10)

#define FHSSSYNC 5

extern device_t gpsDevice;

struct gpsPlus{
    TinyGPSPlus gps;
    Stream *SerialLogger = nullptr;

    bool isfix = false;
    bool allowPing = false;
     bool allowWakeUp = true;
     bool lockDouble = false;
     bool responceOneTime = false;
     bool WORD_ALIGNED_ATTR DMA_ATTR TXdoneBool = false;
     bool txTickDone = false;

     uint8_t fhss = 0;

    typedef struct {
        double lat;
        double lng;
        double alt;
        uint32_t time;
        uint32_t date;
        double speed;
    } gps_update;

      std::function<void()> sendRF;
    std::function<void()> TXCallBack2;

     uint8_t (*fkey8Encode)() = []() { return KEY8;}; 
     uint16_t (*fkey16Encode)() = []() { return KEY16;}; 
     uint32_t (*fkey32Encode)() = []() { return KEY32;};
     uint8_t (*fboxidEncode)() = []() { return BOXID;};

     gps_update last_upd;

     uint32_t lastMillis = 0;
     uint32_t pingRecvest = 0;

    

    inline void updateLast(void);
     int start(void){
//        setupFHSSChannel(SERVICE_CHANNEL);
        return DURATION_IMMEDIATELY;
    }

    void ICACHE_RAM_ATTR TXDoneFunc();
     bool checkid(uint8_t id) { return id == BOXID ? true : false;};            // TODO
     bool check8Key(uint8_t key) { return true;};         // TODO
     bool check16Key(uint16_t key) { return true;};        // TODO
     bool check32Key(uint32_t key)  { return true;};        // TODO
     bool checkWakeUpKey(uint32_t key)  { return true;};   // TODO
     void sendWakeUpResponce();           
     void sendServiceToSync();            
     void sendGPSResponce(uint8_t page);              
     void sendToPingResponce();           
     void ICACHE_RAM_ATTR sendPingResponce(); 
    void ICACHE_RAM_ATTR serviceToSyncCallback();            
     void ICACHE_RAM_ATTR sendTickResponce();             
     void ICACHE_RAM_ATTR packetProccess(OTA_Packet_s* packet);
     bool isNoSpeed(void);
     int loop(void);
     int gpsloop();
     bool allowWakeUpTime();

    
};
