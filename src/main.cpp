#include "Arduino.h"
#include "mac.h"
#include "persistent.h"
#include "CAppServer.h"

//! Connection state
bool g_bConnected = false;

//! The server listening for connections
EthernetServer g_ethServer(80);

//! Tries to connect DHCP
//! @return True if success
static bool InitDHCP()
{
    static const unsigned long c_uTimeoutMs = 1000;

    // Attempt getting DHCP
    if (!Ethernet.begin(m_arrMacAddr, c_uTimeoutMs, c_uTimeoutMs))
    {
        Serial.println(F("Failed to configure Ethernet using DHCP"));

        if (Ethernet.hardwareStatus() == EthernetNoHardware)
        {
            Serial.println(F("Ethernet shield was not found"));
        } 
        else if (Ethernet.linkStatus() == LinkOFF) 
        {
            Serial.println(F("Ethernet cable is not connected"));
        }

        return false;
    }

    // Log the obtained IP
    Serial.print(F("Host: "));
    Serial.println(CHttpServer::GetHostName());

    return true;
}

//! Entry point of program
void setup()
{
    // Global interrupts
    sei();

    // Use serial for debug
    Serial.begin(9600);

    // Configure switch & set initial state
    ConfigureSwitch();

    // Factory reset pin
    static constexpr unsigned char PIN_FACTORY_RST = 7;

    pinMode(PIN_FACTORY_RST, INPUT);

    if (digitalRead(PIN_FACTORY_RST))
    {
        Persist_ResetDefaults();

        Serial.println(F("Factory reset"));
    }
}

//! Periodic loop
void loop()
{
    // Try to connect until success
    if (!g_bConnected)
    {
        g_bConnected = InitDHCP();
        return;
    }

    // Accept new connections
    EthernetClient ethClient = g_ethServer.available();
    if (ethClient) 
    {
        // Log connection
        Serial.print(F("New client: "));
        ethClient.remoteIP().printTo(Serial);
        Serial.println();

        // Create server
        CAppServer httpServer(ethClient);
        httpServer.Run();

        Serial.println(F("Client disconnected"));
    }
}