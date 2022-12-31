#include <Arduino.h>
#include <ETH.h>

#include "ModbusClientTCP.h"

WiFiClient client;
ModbusClientTCP MB(client);

// Define an onData handler function to receive the regular responses
// Arguments are Modbus server ID, the function code requested, the message data and length of it,
// plus a user-supplied token to identify the causing request
void handleData(ModbusMessage response, uint32_t token)
{
    Serial.printf("Response: serverID=%d, FC=%d, Token=%08X, length=%d:\n", response.getServerID(), response.getFunctionCode(), token, response.size());
    for (auto &byte : response)
    {
        Serial.printf("%02X ", byte);
    }
    Serial.println("");
}

// Define an onError handler function to receive error responses
// Arguments are the error code returned and a user-supplied token to identify the causing request
void handleError(Error error)
{
    // ModbusError wraps the error code and provides a readable error message for it
    ModbusError me(error);
    Serial.printf("Error response: %02X - %s\n", (int)me, (const char *)me);
}

void testClient()
{
    // Set message timeout to 2000ms and interval between requests to the same host to 200ms
    MB.setTimeout(2000, 200);
    // Start ModbusTCP background task
    MB.begin();

    Serial.println("inside test client");

    // Issue a request
    // Set Modbus TCP server address and port number
    // (Fill in your data here!)
    MB.setTarget(IPAddress(192, 168, 0, 2), 502);

    uint16_t value = 0x00;
    while (true)
    {
        while (!Serial.available())
            ;
        String input = Serial.readStringUntil('\n');
        while (Serial.available())
            ;
        Serial.print("input is ");
        Serial.println(input);
        if (input == "yes")
        {
            value = 0xff00;
        }
        else
        {
            value = 0x00;
        }
        while (Serial.available())
            ;
        // Create request for
        // (Fill in your data here!)
        // - token to match the response with the request. We take the current millis() value for it.
        // - server ID = 4
        // - function code = 0x03 (read holding register)
        // - start address to read = word 2
        // - number of words to read = 6
        //
        // If something is missing or wrong with the call parameters, we will immediately get an error code
        // and the request will not be issued
        ModbusMessage message(1, WRITE_COIL, 0, value);
        uint32_t token = millis();
        ModbusMessage response = MB.syncRequest(message, token);
        Serial.println("after sending packet");
        Error status = response.getError();
        if (status != SUCCESS)
        {
            handleError(status);
            // return false;
        }
        else
        {
            handleData(response, token);
        }
        // return true;

        //   if (!client.connect(host, port))
        //   {
        //     Serial.println("connection failed");
        //     return;
        //   }
        //   client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
        //   while (client.connected() && !client.available())
        //     ;
        //   while (client.available())
        //   {
        //     Serial.write(client.read());
        //   }

        //   Serial.println("closing connection\n");
        //   client.stop();
    }
}
