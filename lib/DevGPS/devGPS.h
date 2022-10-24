#pragma once
#include "device.h"
#include "TinyGPS++.h"
#include "OTA.h"
#include "common.h"

#define KEY8    (uint8_t)0x8
#define KEY16   (uint16_t)0x16
#define KEY32   (uint32_t)0x0032
#define BOXID   (uint8_t)0x01

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



namespace gpsPlus{
    static TinyGPSPlus gps;
    static Stream *SerialLogger = nullptr;\

    static bool isfix = false;
    static bool allowPing = false;
    static bool allowWakeUp = true;
    static bool lockDouble = false;

    static uint8_t fhss = 0;

    typedef struct {
        double lat;
        double lng;
        double alt;
        uint32_t time;
        uint32_t date;
        double speed;
    } gps_update;

    static void (*sendRF)() = nullptr;
    static uint8_t (*fkey8Encode)() = []() { return KEY8;}; 
    static uint16_t (*fkey16Encode)() = []() { return KEY16;}; 
    static uint32_t (*fkey32Encode)() = []() { return KEY32;};
    static uint8_t (*fboxidEncode)() = []() { return BOXID;};

    static gps_update last_upd;

    static uint32_t lastMillis = 0;
    static uint32_t pingRecvest = 0;

    extern device_t gpsDevice;

    inline void updateLast(void);
    static int start(void){
//        setupFHSSChannel(SERVICE_CHANNEL);
        return DURATION_IMMEDIATELY;
    }
    static bool checkid(uint8_t id) { return id == BOXID ? true : false;};            // TODO
    static bool check8Key(uint8_t key) { return true;};         // TODO
    static bool check16Key(uint16_t key) { return true;};        // TODO
    static bool check32Key(uint32_t key)  { return true;};        // TODO
    static bool checkWakeUpKey(uint32_t key)  { return true;};   // TODO
    static void sendWakeUpResponce();           
    static void sendServiceToSync();            
    static void sendGPSResponce(uint8_t page) ;              // TODO
    static void sendToPingResponce() { return;};           // TODO
    static void sendPingResponce() { return;};             // TODO
    static void sendTickResponce() { return;};             // TODO

    ICACHE_RAM_ATTR void packetProccess(OTA_Packet_s* packet);
    static bool isNoSpeed(void);
    static int loop(void);

    
}
