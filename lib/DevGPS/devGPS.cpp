#include "devGPS.h"
#include "common.h"
#include "FHSS.h"
#include <unistd.h>

namespace gpsPlus{

    ICACHE_RAM_ATTR void packetProccess(OTA_Packet_s* packet){

        int index = packet->std.msp_ul.packageIndex;
        Pack_msg* pack = (Pack_msg*)(packet->std.msp_ul.payload);
        switch (pack->type){
            case TYPE_WAKE_UP:
                if (check32Key(pack->key32) && allowWakeUp)
                {
                    sendRF = sendWakeUpResponce;
                    allowWakeUp = false;
                }
                break;
            case TYPE_SERVICE_TO_SYNC:
                if (check8Key(pack->service_to_sync.key8) && checkid(pack->service_to_sync.id))
                {
                    
                    sendRF = sendServiceToSync;
                }
                    
                break;
            case TYPE_GPS_RECVEST:
                if (check8Key(pack->gps_recvest.key8) && check16Key(pack->gps_recvest.key16) && checkid(pack->gps_recvest.id))
                    sendGPSResponce(index);
                break;
            case TYPE_TO_PING_RECVEST:
                if (check8Key(pack->to_ping_recvest.key8) && check16Key(pack->to_ping_recvest.key16) && checkid(pack->to_ping_recvest.id)){
                    allowPing = true;
                    pingRecvest = millis();
                    responceOneTime = true;
                    sendRF = sendToPingResponce;
                }
                break;
            case TYPE_PING_RECVEST:
                if (allowPing)
                    sendPingResponce();
                break;
            case TYPE_TICK_RECVEST:
                if (check8Key(pack->tick_recvest.key8) && check16Key(pack->tick_recvest.key16) && checkid(pack->tick_recvest.id)){
                    setupFHSSChannel(20 + BOXID);
                    sendRF = sendTickResponce;
                }
                    
                break;


        }
    }

    static void sendTickResponce(){
        WORD_ALIGNED_ATTR OTA_Packet_s otaPkt = {0};
        otaPkt.msp.type = PACKET_TYPE_MSPDATA;
        otaPkt.msp.msp_ul.payload.type = TYPE_TICK_RESPONCE;
        OtaGeneratePacketCrc(&otaPkt);
        otaPkt.msp.msp_ul.payload.tick_responce.tick = ESP.getCycleCount();

        Radio.TXnb((uint8_t*)&otaPkt, ExpressLRS_currAirRate_Modparams->PayloadLength);

    }
    
    ICACHE_RAM_ATTR void sendPingResponce(){
        WORD_ALIGNED_ATTR OTA_Packet_s otaPkt = {0};
        otaPkt.msp.type = PACKET_TYPE_MSPDATA;
        otaPkt.msp.msp_ul.payload.type = TYPE_PONG_RESPONCE;

        OtaGeneratePacketCrc(&otaPkt);
        Radio.TXnb((uint8_t*)&otaPkt, ExpressLRS_currAirRate_Modparams->PayloadLength);
    }

    static void sendToPingResponce(){
        WORD_ALIGNED_ATTR OTA_Packet_s otaPkt = {0};
        otaPkt.msp.type = PACKET_TYPE_MSPDATA;
        otaPkt.msp.msp_ul.packageIndex = 0;
        otaPkt.msp.msp_ul.payload.to_ping_responce.id = FBOXID;
        otaPkt.msp.msp_ul.payload.to_ping_responce.key16 = FKEY16;
        otaPkt.msp.msp_ul.payload.to_ping_responce.key8 = FKEY8;

        otaPkt.msp.msp_ul.payload.type = TYPE_TO_PING_RESPONCE;

        if (responceOneTime)
        {
            OtaGeneratePacketCrc(&otaPkt);
            Radio.TXnb((uint8_t*)&otaPkt, ExpressLRS_currAirRate_Modparams->PayloadLength);
            responceOneTime = false;
        }

        if (millis() - pingRecvest < 30000){
            allowPing = false;
            sendRF = nullptr;
        }
    }

    inline bool allowWakeUpTime(){
        // uint32_t centiseconds = (last_upd.time & 0x000000FF) * 10;
        uint32_t centiseconds = (last_upd.time % 100) * 10;
       // uint32_t seconds = ((last_upd.time & 0x0000FF00) >> 8) * 1000;
        uint32_t seconds = ((last_upd.time / 100) % 100) * 1000;
        uint32_t time = seconds + centiseconds + gps.time.age();

        if ((time % 5000) > (START_TIME + BUFFER) && (time % 5000) < (START_TIME + INTERVAL - BUFFER))
            return true;
        else 
            return false;
    }

    static void sendGPSResponce(uint8_t page){
        WORD_ALIGNED_ATTR OTA_Packet_s otaPkt = {0};
        uint32_t* msg;

        if (!lockDouble)
            lockDouble = true;

        otaPkt.msp.type = PACKET_TYPE_MSPDATA;
        otaPkt.msp.msp_ul.packageIndex = page; 
        switch (page){
            case 0:
                msg = (uint32_t*)&(last_upd.lat);
                otaPkt.msp.msp_ul.payload.gps_responce.responce = msg[0];
                break;
            case 1:
                msg = (uint32_t*)&(last_upd.lat);
                otaPkt.msp.msp_ul.payload.gps_responce.responce = msg[1];
                break;
            case 2:
                msg = (uint32_t*)&(last_upd.lng);
                otaPkt.msp.msp_ul.payload.gps_responce.responce = msg[0];
                break;
            case 3:
                msg = (uint32_t*)&(last_upd.lng);
                otaPkt.msp.msp_ul.payload.gps_responce.responce = msg[1];
                break;
            case 4:
                msg = (uint32_t*)&(last_upd.alt);
                otaPkt.msp.msp_ul.payload.gps_responce.responce = msg[0];
                break;
            case 5:
                msg = (uint32_t*)&(last_upd.alt);
                otaPkt.msp.msp_ul.payload.gps_responce.responce = msg[1];
                break;
        }
        otaPkt.msp.msp_ul.payload.type = TYPE_GPS_RESPONCE;

        OtaGeneratePacketCrc(&otaPkt);
        Radio.TXnb((uint8_t*)&otaPkt, ExpressLRS_currAirRate_Modparams->PayloadLength);
    }

    static void sendWakeUpResponce(void){

        if (!allowWakeUpTime())
            return;

        WORD_ALIGNED_ATTR OTA_Packet_s otaPkt = {0};
        otaPkt.msp.type = PACKET_TYPE_MSPDATA;
        otaPkt.msp.msp_ul.packageIndex = 0;
        otaPkt.msp.msp_ul.payload.wake_up_responce.id = FBOXID;
        otaPkt.msp.msp_ul.payload.wake_up_responce.key16 = FKEY16;
        otaPkt.msp.msp_ul.payload.wake_up_responce.key8 = FKEY8;
        otaPkt.msp.msp_ul.payload.type = TYPE_WAKE_UP_RESPONCE;

        OtaGeneratePacketCrc(&otaPkt);
        Radio.TXnb((uint8_t*)&otaPkt, ExpressLRS_currAirRate_Modparams->PayloadLength);
        uint32_t now = millis();
        while (millis() - now < 5)
        {}
    }

    static void serviceToSyncCallback(){
        setupFHSSChannel(FHSSSYNC);
        TXCallBack = nullptr;
    }


    static void sendServiceToSync(){
        
        WORD_ALIGNED_ATTR OTA_Packet_s otaPkt = {0};
        otaPkt.msp.type = PACKET_TYPE_MSPDATA;
        otaPkt.msp.msp_ul.packageIndex = 0;
        otaPkt.msp.msp_ul.payload.service_to_sync_responce.id = FBOXID;
        otaPkt.msp.msp_ul.payload.service_to_sync_responce.key8 = FKEY8;
        otaPkt.msp.msp_ul.payload.service_to_sync_responce.key16 = FKEY16;
        otaPkt.msp.msp_ul.payload.type = TYPE_SERVICE_TO_SYNC_RESPONCE;

        TXCallBack = serviceToSyncCallback;

        OtaGeneratePacketCrc(&otaPkt);
        Radio.TXnb((uint8_t*)&otaPkt, ExpressLRS_currAirRate_Modparams->PayloadLength);

        
    
        sendRF = nullptr;
        
    }

    inline void updateLast(void){
        if (gps.location.isUpdated() && !lockDouble)
        {
            last_upd.lat = gps.location.lat();
            last_upd.lng = gps.location.lng();
        }
            
        if (gps.altitude.isUpdated() && !lockDouble)
            last_upd.alt = gps.altitude.meters();
        if (gps.time.isUpdated())
            last_upd.time = gps.time.value();
        if (gps.speed.isUpdated())
            last_upd.speed = gps.speed.mps();
        if (gps.date.isUpdated())
            last_upd.date = gps.date.value();
    }

    static bool isNoSpeed(void){
        uint32_t now = millis();
        static double lastSpeed;
        if (last_upd.speed - lastSpeed > 0)
        {
            lastSpeed = last_upd.speed;
            lastMillis = now;
            return false;
        }
        if (now - lastMillis > 10000)
            return true;
        else 
            return false;
    }

    static int gpsloop(void){

        

        // if (!SerialLogger)
        //     return DURATION_IMMEDIATELY;

        if (Serial.available() > 0)
            gps << Serial.read();
        
        if (!isfix)
        {
            if (gps.location.isValid())
            {
                isfix = true;
                lastMillis = millis();
            }
            // TODO
        }
        else{
            if (isNoSpeed())
            {
                // TODO
            }
        }

        
        // char str[50];
        // int l = sprintf(str, "millis = %u", millis());    
        
        // SerialLogger->write(str, l);
        if (sendRF)
            sendRF();
        updateLast();
        return DURATION_IMMEDIATELY;
    }


    device_t gpsDevice = {
        .initialize = nullptr,
        .start = start,
        .event = start,
        .timeout = gpsloop,
    };
}