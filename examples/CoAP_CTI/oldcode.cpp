//uint8_t heaterPin = 10;
//uint8_t ledPin = 13;
//uint8_t methanePin = A2;
//uint8_t carbonPin = A3;
//int methaneValue=0;
//int carbonValue=0;
//
//void setup()
//{
//
//  if(sensorsExist)
//  {
//    pinMode(heaterPin, OUTPUT);
////    uber.blinkLED(1, 500);
//  } 
//
//}
//
//void loop()
//{
//  static unsigned long ledTimestamp = 0;
//  if(millis() - ledTimestamp > 5000)
//  {
////    uber.blinkLED(1,100);
//    ledTimestamp = millis();
//  }
//}
//
////void checkSensors(void)
////{
////  checkMethane();
////}
//
//void checkMethane(void)
//{
//  // for temp sensor
//
//  static unsigned long methaneTimestamp = 0;
//  if(millis() - methaneTimestamp > 3 * 60000)
//  {
//    methaneValue = analogRead(methanePin);  // read the value from the sensor
//    uber.sendValue("ch4", methaneValue);    
//    methaneTimestamp = millis();
//  }
//}
//

