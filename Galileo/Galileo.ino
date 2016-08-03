#include <pt.h>

// motor
#define AIA 6
#define AIB 5
#define BIA 11
#define BIB 10

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

PT_THREAD(MoterFW(struct pt *pt))
{
    PT_BEGIN(pt);
    analogWrite(AIA, 255);
    analogWrite(AIB, 0);
    analogWrite(BIA, 255);
    analogWrite(BIB, 0);
    PT_END(pt);
}

PT_THREAD(MoterBW(struct pt *pt))
{
    PT_BEGIN(pt);
    analogWrite(AIA, 0);
    analogWrite(AIB, 255);
    analogWrite(BIA, 0);
    analogWrite(BIB, 255);
    PT_END(pt);
}

void init()
{
    Serial.begin(9600);
    Serial1.begin(115200);

    pinMode(AIA,OUTPUT);
    pinMode(AIB,OUTPUT);
    pinMode(BIA,OUTPUT);
    pinMode(BIB,OUTPUT);

    PT_INIT(&ptSerialEvent);

    Serial.flush();
    Serial.println("sys init");
    delay(500);
}

void setup()
{
    init();
}

void loop()
{
    serialEvent(&ptSerialEvent);
}
