#include <WiFi.h>
#include "APIServer.h"
#include "Configuration.h"

Configuration &config = Configuration::getInstance();
APIServer apiServer;

void handleOn(RequestArgs args)
{
    config.ledState = true;
    digitalWrite(config.ledPin, HIGH);
    args.server->send(200, "text/plain", "LED ON");
}

void handleOff(RequestArgs args)
{
    config.ledState = false;
    digitalWrite(config.ledPin, LOW);
    args.server->send(200, "text/plain", "LED OFF");
}

void handleRoot(RequestArgs args)
{
    String html = R"rawliteral(
    <html><body>
        <h1>ESP32 LED Control</h1>
        <p><button onclick="fetch('/on')">Turn ON</button></p>
        <p><button onclick="fetch('/off')">Turn OFF</button></p>
        <span>
            <label for="threshold">Threshold </label>
            <input id="threshold" value=")rawliteral" +
                  String(config.threshold) + R"rawliteral(">
            <button onclick="fetch(`/set-threshold?threshold=${document.getElementById('threshold').value}`)">
                Set Threshold
            </button>
        </span>
    </body></html>)rawliteral";
    args.server->send(200, "text/html", html);
}

void handleSetThreshold(RequestArgs args)
{
    if (args.contains("threshold"))
    {
        config.threshold = args["threshold"].toInt();
        config.resetCounter();
        args.server->send(200, "text/plain", "Threshold set to: " + String(config.threshold));
        return;
    }

    args.server->send(400, "text/plain", "Invalid request");
}

void configureEndpoints()
{
    apiServer.addEndpoint("/", HTTP_GET, handleRoot);
    apiServer.addEndpoint("/on", HTTP_GET, handleOn);
    apiServer.addEndpoint("/off", HTTP_GET, handleOff);
    apiServer.addEndpoint("/set-threshold", HTTP_GET, handleSetThreshold);
}

void setup()
{
    Serial.begin(115200);

    WiFi.begin(config.ssid, config.password);
    while (WiFi.status() != WL_CONNECTED)
        delay(500);

    Serial.println("\nConnected to: " + WiFi.localIP().toString());
    configureEndpoints();
    apiServer.begin();
}

void loop()
{
    config.updateButton();
    apiServer.handleClient();

    if (config.button.isPressed())
    {
        config.incrementPress();

        if (config.timesPressed >= config.threshold)
        {
            config.toggleLED();
            Serial.println("LED " + String(config.ledState ? "ON" : "OFF"));
            config.resetCounter();
        }
    }
}
