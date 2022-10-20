#pragma once
#include "device.h"
#include "TinyGPS++.h"



namespace gpsPlus{
    static TinyGPSPlus gps;
    static Stream *SerialLogger = nullptr;
    
    typedef struct {
        double lat;
        double lng;
        double alt;
        double time;
        double date;
        double speed;
    } gps_update;

    static gps_update last_upd;
    static void start(void){
        
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
    }

    static void loop(void){

        if (!SerialLogger)
            return;

        if (SerialLogger->available() > 0)
            gps.encode(SerialLogger->read());
        


        


        updateLast();
    }
}
