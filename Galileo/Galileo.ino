#include <pt.h>

// motor
#define AIA 6
#define AIB 5
#define BIA 11
#define BIB 10

// rain
#define RN A0

struct pt ptSerialEvent;
struct pt ptMotorFW;
struct pt ptMotorBW;
struct pt ptRainSensor;

int range, sensorReading;

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

PT_THREAD(MotorFW(struct pt *pt))
{
    PT_BEGIN(pt);
    analogWrite(AIA, 255);
    analogWrite(AIB, 0);
    analogWrite(BIA, 255);
    analogWrite(BIB, 0);
    PT_END(pt);
}

PT_THREAD(MotorBW(struct pt *pt))
{
    PT_BEGIN(pt);
    analogWrite(AIA, 0);
    analogWrite(AIB, 255);
    analogWrite(BIA, 0);
    analogWrite(BIB, 255);
    PT_END(pt);
}

PT_THREAD(RainSensor(struct pt *pt))
{
    unsigned long t;
    PT_BEGIN(pt);
    sensorReading = analogRead(RN);
    range = map(sensorReading, 0, 1024, 0, 3);
    switch (range)
    {
        case 0:
            Serial.println("Flood");
            break;
        case 1:
            Serial.println("Rain Warning");
            break;
        case 2:
            Serial.println("Not Raining");
            break;
    }
    t = millis();
    PT_WAIT_WHILE(pt, millis() - t < 200);
    PT_END(pt);
}

void init()
{
    Serial.begin(9600);
    Serial1.begin(115200);

    // motor
    pinMode(AIA,OUTPUT);
    pinMode(AIB,OUTPUT);
    pinMode(BIA,OUTPUT);
    pinMode(BIB,OUTPUT);

    // rain
    pinMode(RN,INPUT);

    PT_INIT(&ptSerialEvent);
    PT_INIT(&ptMotorFW);
    PT_INIT(&ptMotorBW);
    PT_INIT(&ptRainSensor)

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
    RainSensor(&ptRainSensor);
}
