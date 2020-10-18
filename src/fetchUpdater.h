#pragma once

#include <Arduino.h>

#ifndef BINARY_FILENAME
    #define BINARY_FILENAME "IoTF_otafirmware"
#endif

class fetchUpdater
{
private:
    bool m_getBinary(String filename);


public:
    int checkUpdate(String url, String user="", String pass="", String version="");
    
};

extern fetchUpdater httpsUpdater;