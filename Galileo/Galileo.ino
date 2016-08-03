void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(115200);
}

void loop() {
  SerialEvent();

}
void SerialEvent(){
  if(Serial1.available()){
    String str = Serial1.readStringUntil('\r');
    Serial1.flush();
    Serial.println(str);
    str.replace("\r","");
    str += "666";
    Serial1.println(str);
  }
}

