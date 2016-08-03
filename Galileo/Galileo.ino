#include <pt.h>
#include <Servo.h>
#include <LiquidCrystal.h>

// motor
#define AIA 6
#define AIB 5
#define BIA 11
#define BIB 10

#define rainPin A0
#define motionPin A1
#define tempPin A2
#define LDRPin 15

struct pt ptSerialEvent;
struct pt ptMotorFW;
struct pt ptMotorBW;
struct pt ptRainSensor;
struct pt ptMotionSensor;
struct pt ptTempSensor;
struct pt ptLDR;
struct pt ptServo;

Servo servo;
String data;
int rainRange, rainAnalog, motionAnalog, temp, light;

PT_THREAD(serialEvent(struct pt *pt))
{
    unsigned long t;
    PT_BEGIN(pt);
    if(Serial1.available())
    {
        String mesg = Serial1.readStringUntil('\r');
        Serial1.flush();
        Serial.println(mesg);
    }
    data = String(String(rainAnalog) + "," + String(motionAnalog) + "," + String(temp) + "," + String(light) + "\r");
    Serial.println(data);
    Serial1.println(data);
    t = millis();
    PT_WAIT_WHILE(pt, millis() - t < 500);
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

PT_THREAD(rainSensor(struct pt *pt))
{
    unsigned long t;
    PT_BEGIN(pt);
    rainAnalog = analogRead(rainPin);
    rainRange = map(rainAnalog, 0, 1024, 0, 3);
    t = millis();
    PT_WAIT_WHILE(pt, millis() - t < 200);
    PT_END(pt);
}

PT_THREAD(motionSensor(struct pt *pt))
{
    PT_BEGIN(pt);
    motionAnalog = digitalRead(motionPin);
    PT_END(pt);
}

PT_THREAD(tempSensor(struct pt *pt))
{
    PT_BEGIN(pt);
    temp = (25*analogRead(tempPin) - 2050)/100;
    PT_END(pt);
}

PT_THREAD(LDRSensor(struct pt *pt))
{
    unsigned long t;
    PT_BEGIN(pt);
    light = analogRead(LDRPin);
    t = millis();
    PT_WAIT_WHILE(pt, millis() - t < 500);
    PT_END(pt);
}

PT_THREAD(openServo(struct pt *pt))
{
    unsigned long t;
    PT_BEGIN(pt);
    servo.write(45);
    t = millis();
    PT_WAIT_WHILE(pt, millis() - t < 1000);
    PT_END(pt);
}

PT_THREAD(closeServo(struct pt *pt))
{
    unsigned long t;
    PT_BEGIN(pt);
    servo.write(0);
    t = millis();
    PT_WAIT_WHILE(pt, millis() - t < 1000);
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

    pinMode(rainPin,INPUT);
    pinMode(motionPin, INPUT);
    pinMode(tempPin,INPUT);
    pinMode(LDRPin, INPUT);

    PT_INIT(&ptMotorFW);
    PT_INIT(&ptMotorBW);
    PT_INIT(&ptRainSensor)
    PT_INIT(&ptMotionSensor);
    PT_INIT(&ptTempSensor);
    PT_INIT(&ptSerialEvent);
    PT_INIT(&ptServo);

    servo.attach(9);

    data = "";
    rainRange = rainAnalog = motionAnalog = temp = light = 0;

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
    rainSensor(&ptRainSensor);
    motionSensor(&ptMotionSensor);
    tempSensor(&ptTempSensor);
    LDRSensor(&ptLDR);
    serialEvent(&ptSerialEvent);
}
