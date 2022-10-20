#pragma once
#include "device.h"
#include "TinyGPS++.h"
#include "OTA.h"
#include "common.h"

#define KEY8    0x8
#define KEY16   0x16
#define KEY32   0x0032
#define BOXID   0x01

#define SERVICE_CHANNEL 1
#define SYNC_CHANNEL 5
#define TICK_CHANNEL 20 + BOXID

namespace gpsPlus{
    static TinyGPSPlus gps;
    static Stream *SerialLogger = nullptr;\

    static bool isfix = false;
    static bool allowPing = false;
    static bool allowWakeUp = true;

    static uint8_t fhss = 0;

    typedef struct {
        double lat;
        double lng;
        double alt;
        double time;
        double date;
        double speed;
    } gps_update;

    static void (*sendRF)() = nullptr;

    static gps_update last_upd;

    static uint32_t lastMillis = 0;
    static uint32_t pingRecvest = 0;

    inline void updateLast(void);
    static int start(void){
//        setupFHSSChannel(SERVICE_CHANNEL);
        return DURATION_IMMEDIATELY;
    }
    static bool checkid(uint8_t id);
    static bool check8Key(uint8_t key);
    static bool check16Key(uint8_t key);
    static bool check32Key(uint8_t key);
    static bool checkWakeUpKey(uint32_t key);
    static void sendWakeUpResponce();
    static void sendServiceToSync();
    static void sendGPSResponce();
    static void sendToPingResponce();
    static void sendPingResponce();
    static void sendTickResponce();

    inline void packetProccess(OTA_Packet_s* packet);
    static bool isNoSpeed(void);
    static int loop(void);

    
}
