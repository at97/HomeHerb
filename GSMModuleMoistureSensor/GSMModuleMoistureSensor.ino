//If sms send = TEST then you get a SMS reply to your own phone

#include <SoftwareSerial.h>
// Green wire
int rxPin = 53;
//Blue wire
int txPin = 51;
SoftwareSerial MySerial(rxPin,txPin);
char incomingbyte[150];   // array to compare incoming SMS 

int index=0; // array to compare incoming SMS - index
int count=5; // counting to 5 connecting network (Setup setting)
int i=0;  // Setup setting
int y=0; // condition to read incoming values
int c=0;

// Soil
// Moisture value
int moistureValue = 0;
//Declare a variable for the soil moisture sensor 
int soilPin = A1;
//Variable for Soil moisture Power
int soilPower = 8;

/*The setup will start in 19200 baud both Serial and My Serial, later count until 5 to connect to network and start the AT commands parameters, so 
SMS messages and parameters*/

void updateSerial()
{
    delay(2000);
    while(Serial.available()){
      MySerial.write(Serial.read());
    }
    while(MySerial.available()){
      Serial.write(MySerial.read());
    }
  }
void sendSMS(String msg)
{
    MySerial.println("AT+CMGS=\"1XXXXXXXXXX\"");
    updateSerial();
    MySerial.println(msg);
    updateSerial();
    MySerial.write(26);
    Serial.println();  
}
void setup()
{
    Serial.begin(9600);
    MySerial.begin(9600);

    pinMode(soilPower, OUTPUT);//Set D7 as an OUTPUT
    digitalWrite(soilPower, LOW);//Set to LOW so no power is flowing through the sensor
    
    MySerial.println("AT");
    updateSerial();
    delay(1000);

    MySerial.println("AT+CMGF=1");
    updateSerial();
    delay(200);
    MySerial.println("AT+CMGD=1,4");
    updateSerial();
    delay(700);
    MySerial.println("AT+CNMI=1,2,0,0,0");
    updateSerial();
    delay(200);

    sendSMS("Hello, My name is HomeHerb");
  }
  
void loop()
{
    //readSoil();
    delay (1000);
    for(int y=0; y < 150; y++)
    {
        incomingbyte[y] = 0;
    }
    if(MySerial.available())
    {
        int n_char = MySerial.available();
        for (int x = 0; x < n_char; x++)
        {
          incomingbyte[x]=MySerial.read();
        }
         
        String C = incomingbyte;
        //Serial.println("setup ended");
        Serial.print("  ");
        Serial.println(C);
        Serial.print("  ");
        if(strstr(incomingbyte,"Update"))
        {
            //String moistureValueString = (String)moistureValue;   
            sendSMS("Moisture String: ");
            //sendSMS(moistureValue);
            MySerial.println("AT+CMGD=1,4");
            updateSerial();      
        } 
    }
}


//This is a function used to get the soil moisture content
int readSoil()
{
    digitalWrite(soilPower, HIGH);//turn D7 "On"
    delay(10);//wait 10 milliseconds 
    //Read the SIG value form sensor 
    moistureValue = analogRead(soilPin);
    //turn D7 "Off"
    digitalWrite(soilPower, LOW);
    return moistureValue;
}


   
    
