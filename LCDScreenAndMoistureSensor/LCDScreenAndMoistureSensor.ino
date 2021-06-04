// include the library code:
#include <LiquidCrystal.h>
#include <DHT.h>;

//Constants
#define DHTPIN 8     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int val = 0; //value for storing moisture value 
int soilPin = A0;//Declare a variable for the soil moisture sensor 
int soilPower = 7;//Variable for Soil moisture Power
int soilMoisture = 000;
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

void setup() 
{
    Serial.begin(115200);
    dht.begin();
    pinMode(soilPower, OUTPUT);
    digitalWrite(soilPower, LOW);//Set to LOW so no power is flowing through the sensor
    
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    // Print a message to the LCD.
    lcd.print("M:  H    T:");
}

void loop()
{
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    
    // print the soil moisture
    soilMoisture = readSoil();
    Serial.println(soilMoisture);

    // Print the humidity and temperature
    hum = dht.readHumidity();
    temp = dht.readTemperature();
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    
    // Print data
    lcd.print(soilMoisture);
    lcd.print("  ");
    lcd.print(hum);
    lcd.print("  ");
    lcd.print(temp);

    // Clear line
    lcd.print("   ");
    delay(1000);
}

//This is a function used to get the soil moisture content
int readSoil()
{
    digitalWrite(soilPower, HIGH);//turn D7 "On"
    delay(1000);//wait 10 milliseconds 
    val = analogRead(soilPin);//Read the SIG value form sensor 
    digitalWrite(soilPower, LOW);//turn D7 "Off"
    return val;//send current moisture value
}
