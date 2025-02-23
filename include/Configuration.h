#pragma once
#include <ezButton.h>

class Configuration
{
public:
    static Configuration &getInstance()
    {
        static Configuration instance;
        return instance;
    }

    // Hardware configuration
    const char *ssid = "bathroom_cam_3";
    const char *password = "4wordsallcaps";
    const int ledPin = 2;
    ezButton button{35};

    // System state
    bool ledState = false;
    int threshold = 1;
    int timesPressed = 0;

    void updateButton() { button.loop(); }
    void incrementPress() { timesPressed++; }
    void resetCounter() { timesPressed = 0; }
    void toggleLED()
    {
        ledState = !ledState;
        digitalWrite(ledPin, ledState ? HIGH : LOW);
    }

private:
    Configuration()
    {
        pinMode(ledPin, OUTPUT);
        button.setDebounceTime(50);
    }
};
