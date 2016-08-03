#include <pt.h>

struct pt ptSerialEvent;

PT_THREAD(serialEvent(struct pt *pt))
{
    unsigned long t;
    PT_BEGIN(pt);
    if(Serial.available())
    {
        String mesg = Serial.readStringUntil('\n');
        Serial.flush();
        Serial1.println(mesg);
    }
    if(Serial1.available())
    {
        String mesg = Serial1.readStringUntil('\r');
        Serial1.flush();
        Serial.println(mesg);
    }
    t = millis();
    PT_WAIT_WHILE(pt, millis() - t < 200);
    PT_END(pt);
}

void setup()
{
    Serial.begin(9600);
    Serial1.begin(115200);
    Serial.flush();
    PT_INIT(&ptSerialEvent);
    Serial.println("sys init");
}

void loop()
{
    serialEvent(&ptSerialEvent);
}
