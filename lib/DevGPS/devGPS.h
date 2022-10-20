#pragma once
#include "device.h"
#include "TinyGPS++.h"



namespace gpsPlus{
    static TinyGPSPlus gps;
    static Stream *SerialLogger = nullptr;
    static bool isfix = false;
    typedef struct {
        double lat;
        double lng;
        double alt;
        double time;
        double date;
        double speed;
    } gps_update;

    static gps_update last_upd;

    static uint32_t lastMillis = 0;

    static int start(void){
        return DURATION_IMMEDIATELY;
    }

    inline void updateLast(void){
        if (gps.location.isUpdated())
        {
            last_upd.lat = gps.location.lat();
            last_upd.lng = gps.location.lng();
        }
            
        if (gps.altitude.isUpdated())
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

    static int loop(void){

        if (!SerialLogger)
            return DURATION_IMMEDIATELY;

        if (SerialLogger->available() > 0)
            gps.encode(SerialLogger->read());
        
        if (!isfix)
        {
            if (gps.location.isValid())
            {
                isfix = true;
                lastMillis = millis();
            }  
        }
        else{
            if (isNoSpeed())
        }

        

        
        updateLast();
        return DURATION_IMMEDIATELY;
    }

    device_t gpsDevice = {
        .initialize = nullptr,
        .start = start,
        .event = start,
        .timeout = loop,
    };
}
