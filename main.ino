#include <Arduino.h>
#include "./mylib/myUtil.h"
#include "myProgramer.h"

void setup()
{

    Serial.begin(250000); /*caution: BAUD 250k*/

    myprogramer_setup();
    myprogramer_start();
}

void loop()
{
}