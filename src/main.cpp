#include "Arduino.h"
#include "mac.h"
#include "CHttpServer.h"

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
        Serial.println("Failed to configure Ethernet using DHCP");

        if (Ethernet.hardwareStatus() == EthernetNoHardware)
        {
            Serial.println("Ethernet shield was not found.");
        } 
        else if (Ethernet.linkStatus() == LinkOFF) 
        {
            Serial.println("Ethernet cable is not connected.");
        }

        return false;
    }

    // Log the obtained IP
    Serial.write("IP: ");
    Ethernet.localIP().printTo(Serial);
    Serial.println();

    return true;
}

//! Entry point of program
void setup()
{
    // Use serial for debug
    Serial.begin(9600);

    // User built-in LED for status
	pinMode(LED_BUILTIN, OUTPUT);
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
        Serial.print("New client: ");
        ethClient.remoteIP().printTo(Serial);
        Serial.println();

        // Create server
        CHttpServer httpServer(ethClient);
        httpServer.Run();

        Serial.println("Client disconnected");
    }
}