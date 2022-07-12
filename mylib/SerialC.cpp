#include <Arduino.h>
extern "C"
{
#include "SerialC.h"
}
void SerialPrint_Str(const char *s)
{
    Serial.print(s);
}
void SerialPrintln_Str(const char *s)
{
    Serial.println(s);
}
void SerialPrint_Int(int i)
{
    Serial.print(i);
}
void SerialPrintln_Int(int i)
{
    Serial.println(i);
}
void SerialPrint_Int_hex(int i)
{
    Serial.print(i, HEX);
}
void SerialPrintln_Int_hex(int i)
{
    Serial.println(i, HEX);
}
void SerialPrint_Double(double d)
{
    Serial.print(d);
}
void SerialPrintln_Double(double d)
{
    Serial.println(d);
}