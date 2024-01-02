#ifndef TemHum_H
#define TemHum_H

#include <Arduino.h>

class TemHum {
    public: 
        TemHum(int pin1, int pin2);
        void firebaseInitialized();
        void wifiConnect();
        void getData();
        void sendData(struct tm timeinfo);
        void initializePins();

    private:
        int pinPemanas;
        int pinPendingin;
};

#endif