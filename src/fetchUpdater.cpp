#include "fetchUpdater.h"
#include "fetch.h"
#include "updater.h"

#include "LittleFS.h"
#include "ESP8266WiFi.h"


int fetchUpdater::checkUpdate(String url, String user, String pass, String version)
{
    fetch.begin(url, true);

    fetch.addHeader(F("Host"), url);
    
    // http.setUserAgent(F("ESP8266-http-Update"));
    fetch.addHeader(F("x-ESP8266-Chip-ID"), String(ESP.getChipId()));
    fetch.addHeader(F("x-ESP8266-STA-MAC"), WiFi.macAddress());
    fetch.addHeader(F("x-ESP8266-AP-MAC"), WiFi.softAPmacAddress());
    fetch.addHeader(F("x-ESP8266-free-space"), String(ESP.getFreeSketchSpace()));
    fetch.addHeader(F("x-ESP8266-sketch-size"), String(ESP.getSketchSize()));
    fetch.addHeader(F("x-ESP8266-sketch-md5"), String(ESP.getSketchMD5()));
    fetch.addHeader(F("x-ESP8266-chip-size"), String(ESP.getFlashChipRealSize()));
    fetch.addHeader(F("x-ESP8266-sdk-version"), ESP.getSdkVersion());
    
    if (version != "")
    {
        fetch.addHeader(F("x-ESP8266-version"), version);
    }

    if (user != "")
    {
        if (pass != "")
        {
            fetch.setAuthorization(user.c_str(), pass.c_str());
        }
        else
        {
            fetch.setAuthorization(user.c_str());
        }
    }

    int response = fetch.GET();
    switch(response)
    {
        case HTTP_CODE_OK:
            if (m_getBinary(BINARY_FILENAME))
            {
                updater.requestStart(BINARY_FILENAME);

                while (updater.getStatus() == 254)
                {
                    yield();
                    updater.loop();
                }
                return updater.getStatus();
            }
            break;
        
        default:
            return response;
    }

    return 0;
}


bool fetchUpdater::m_getBinary(String filename)
{
    File f = LittleFS.open(filename, "w");

    while (fetch.busy())
    {
        yield();
        if (fetch.available())
        {
            f.write(fetch.read());
        }
    }
    f.close();
    fetch.clean();
    
    return true;
}

fetchUpdater httpsUpdater;