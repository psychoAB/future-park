#include <pt.h>
#include <Servo.h>

// motor
#define AIA 6
#define AIB 5
#define BIA 11
#define BIB 9

#define rainPin A0
#define motionPin A1
#define buzzerPin 10
#define tempPin A2
#define LDRPin 15
#define LEDPin 7

struct pt ptSerialEvent;
struct pt ptMotorFW;
struct pt ptMotorBW;
struct pt ptRainSensor;
struct pt ptMotionSensor;
struct pt ptTempSensor;
struct pt ptLDR;
struct pt ptRunServo;
struct pt ptLED;

Servo servo;
String data;
int webData[4];
int rainRange, rainAnalog, motionDigital, temp, light, weather;

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
    static unsigned long t;
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
    if(String(webData[1]).indexOf("1") != -1)
    {
        motionDigital = digitalRead(motionPin);
        if(motionDigital == 1)
        {
            tone(buzzerPin, 800, 2000);
        }
        else if(motionDigital == 0)
        {
            noTone(buzzerPin);
        }
    }
    else if(String(webData[1]).indexOf("0") != -1)
    {
        motionDigital = 0;
    }
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
    static unsigned long t;
    PT_BEGIN(pt);
    light = analogRead(LDRPin);
    t = millis();
    PT_WAIT_WHILE(pt, millis() - t < 500);
    PT_END(pt);
}

PT_THREAD(runServo(struct pt *pt))
{
    static unsigned long t;
    PT_BEGIN(pt);
    if(String(webData[3]).indexOf("1") != -1)
    {
        servo.write(120);
        t = millis();
        PT_WAIT_WHILE(pt, millis() - t < 1000);
    }
    else if(String(webData[3]).indexOf("0") != -1)
    {
        servo.write(90);
        t = millis();
        PT_WAIT_WHILE(pt, millis() - t < 1000);
    }
    PT_END(pt);
}

PT_THREAD(LED(struct pt *pt))
{
    PT_BEGIN(pt);
    if(String(webData[0]).indexOf("1") != -1 || weather >= 1)
    {
        digitalWrite(LEDPin,HIGH);
    }
    else if(String(webData[0]).indexOf("0") != -1)
    {
        digitalWrite(LEDPin,LOW);
    }
    PT_END(pt);
}

PT_THREAD(serialEvent(struct pt *pt))
{
    static unsigned long t;
    PT_BEGIN(pt);
    if(Serial1.available())
    {
        String mesg = Serial1.readStringUntil('\r');
        Serial1.flush();
        if(mesg.indexOf("/") == -1 && mesg != "" && mesg != "\n")
        {
            static String str;
            static int element, index, lastIndex;
            lastIndex = 0;
            for(element = 0; element < 3; element++)
            {
                index = mesg.indexOf(",", lastIndex);
                str = mesg.substring(lastIndex, index);
                webData[element] = str.toInt();
                lastIndex = index + 1;
            }
            str = mesg.substring(lastIndex);
            webData[element] = str.toInt();
            str = "";
            for(element = 0; element < 4; element++)
            {
                str += String(webData[element]);
                str += " ";
            }
            Serial.println(str);
        }
    }
    if(rainRange == 0)
    {
        weather = 2;
    }
    else if(light < 100)
    {
        weather = 1;
    }
    else
    {
        weather = 0;
    }
    data = String("0," + String(weather) + "," + String(temp) + "," + "0" + "," + String(motionDigital) + "," + "0" + "," + "0" + "\r");
    Serial.println(String(millis()) + " " + data);
    Serial1.println(data);
    t = millis();
    PT_WAIT_WHILE(pt, millis() - t < 3000);
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
    pinMode(buzzerPin, OUTPUT);
    pinMode(tempPin,INPUT);
    pinMode(LDRPin, INPUT);
    pinMode(LEDPin, OUTPUT);

    PT_INIT(&ptMotorFW);
    PT_INIT(&ptMotorBW);
    PT_INIT(&ptRainSensor)
    PT_INIT(&ptMotionSensor);
    PT_INIT(&ptTempSensor);
    PT_INIT(&ptSerialEvent);
    PT_INIT(&ptRunServo);
    PT_INIT(&ptLED);

    servo.attach(3);
    data = "";
    webData[0] = 0;
    webData[1] = 1;
    webData[2] = 0;
    webData[3] = 0;
    rainRange = rainAnalog = motionDigital = temp = light = 0;

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
    runServo(&ptRunServo);
    LED(&ptLED);
    serialEvent(&ptSerialEvent);
}
