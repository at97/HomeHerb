#include "DHTesp.h"
#include <SimpleTimer.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

#define WATERING_INTERVAL 10000

// LCD
//Blue wire = first pin
//Brown wire = second pin
const int rst = 3, enable = 2, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rst, enable, d4, d5, d6, d7);
float percentMoisture = 0.0;

// GSM
// Green wire
int rxPin = 53;
//Blue wire
int txPin = 51;
char incomingByte[150]; // array to compare incoming SMS
SoftwareSerial MySerial(rxPin, txPin);

// DHT
// Red wire = 5V
// black wire = GND
// Blue wire (Green) = DHT pin
int DHT22Pin = 9;
DHTesp dht;

// Soil
SimpleTimer moistureTimer;
unsigned long previousTime = 0;
int moistureArray[10];
int soilMoisture = 0;
int moistureAverage = 0;
int moistureLimit = 700;
int sum = 0;
//Declare a variable for the soil moisture sensor (SIG)
int moistureSensorPin = A1;
//Variable for Soil moisture Power (VCC)
int moistureSensorPower = 8;
int counter = 0;
// Motor
int motorPin = 44;
// LEDs and LDRs
SimpleTimer LDRTimer;
int ldrTopLeft = 2; int topLeft = 0;
int ldrTopRight = 3; int topRight = 0;
int ldrDownLeft = 4; int downLeft = 0;
int ldrDownRight = 5; int downRight = 0;
int ldrAverage = 0;
int ledPin = 47;
int darknessLimit = 100;

unsigned long currentMillis = millis();
long previousMillis = 0;

/*----------Moisture Sensor functions----------*/
// Get soil moisture sensor value
int readSoilMoisture()
{
    delay(100);
    soilMoisture = analogRead(moistureSensorPin);
    return soilMoisture;
}

// Water plant for 10 seconds
void waterPlant()
{
    digitalWrite(motorPin, HIGH);
    delay(10000);
    digitalWrite(motorPin, LOW);
}

// Sample soil moisture sensor 10 times and water plant
void pollMoistureSensor()
{
    do
    {
        for (int i = 1; i < 11; i++)
        {
            moistureArray[i - 1] = readSoilMoisture();
            sum += moistureArray[i - 1];
            delay(1000);
        }
        moistureAverage = sum / 10;
        if (moistureAverage < moistureLimit)
        {
            // if moisture is less than 700, water plant
            waterPlant();
        }
    } while (moistureAverage < moistureLimit);
}

// Convert soil moisture value to percentage
float convertMoistureValue(int moistureVal)
{
    float percentOutput = 0.0;
    float temp = moistureVal - 870;
    if (temp < 0)
    {
        percentOutput = (temp / 870) * 100;
    }
    if (temp == 0)
    {
        percentOutput = 0.0;
    }
    if (temp > 0)
    {
        percentOutput = (temp / 154) * 100;
    }

    return percentOutput;
}

/*----------RHTemp sensor functions----------*/
// Get temperature value
float readTemp()
{
    float temperature = dht.getTemperature();
    return temperature;
}

// Get humidity value
float readHumid()
{
    float humidity = dht.getHumidity();
    return humidity;
}

/*----------LDR functions----------*/
// Get values from light-dependent resistors and turn on/off the LEDs
void pollLDR()
{
    topLeft = analogRead(ldrTopLeft); // top left
    topRight = analogRead(ldrTopRight); // top right
    downLeft = analogRead(ldrDownLeft); // down left
    downRight = analogRead(ldrDownRight); // down right

    // Average of LDRs
    ldrAverage = (topLeft + topRight + downLeft + downRight) / 4;

    // Decide whether to light LEDs
    if (ldrAverage < darknessLimit)
    {
        // Turn on LEDs if it's too dark
        digitalWrite(ledPin, HIGH);
    }
    else
    {
        // Turn off LEDs if it's bright enough
        digitalWrite(ledPin, LOW);
    }
}

/*----------GSM functions----------*/
// Initialize the GSM module to text the system
void GSMInitialization()
{
    MySerial.begin(9600);
    MySerial.println("AT");
    updateSerial();
    delay(1000);
    MySerial.println("AT+CMGF=1");
    updateSerial();
    MySerial.println("AT+CMGD=1,4");
    updateSerial();
    delay(700);
    MySerial.println("AT+CNMI=1,2,0,0,0");
    updateSerial();
    sendSMS("Hello, My name is HomeHerb! Enter \"Help\" to display commands.");
    delay(1000);
}

void updateSerial()
{
    delay(1000);
    while (Serial.available()) {
        MySerial.write(Serial.read());
    }
    while (MySerial.available()) {
        Serial.write(MySerial.read());
    }
}

// Send text message
void sendSMS(String msg)
{
    // Send text to the phone number (replace X with a 10-digit phone number)
    MySerial.println("AT+CMGS=\"+1XXXXXXXXXX\"");
    updateSerial();
    MySerial.println(msg);
    updateSerial();
    MySerial.write(26);
}

// Clear incoming byte
void clearIncomingByte(char byteArray[])
{
    for (int y = 0; y < 100; y++)
    {
        byteArray[y] = 0;
    }
}

// Send responses when the user texts the system
void GSMResponse(char byteArray[])
{
    clearIncomingByte(byteArray);
    if (MySerial.available())
    {
        int n_char = MySerial.available();
        for (int x = 0; x < n_char; x++)
        {
            byteArray[x] = MySerial.read();
        }
        String C = byteArray;
        Serial.print(" ");
        Serial.println(C);
        Serial.print(" ");

        // Respond if the user texts "Update"
        if (strstr(byteArray, "Update"))
        {
            delay(1000);
            percentMoisture = convertMoistureValue(readSoilMoisture());
            if (percentMoisture < 0)
            {
                sendSMS("Moisture value: " + String(String(-percentMoisture) + "% - Below target" + "\nTemperature: " + String(readTemp()) + "C\nHumidity: " + String(readHumid()) + "%"));
                MySerial.println("AT+CMGD=1,4");
                updateSerial();
            }
            else if (percentMoisture == 0)
            {
                sendSMS("Moisture value: " + String(String(percentMoisture) + "% - Otimal target" + "\nTemperature: " + String(readTemp()) + "C\nHumidity: " + String(readHumid()) + "%"));
                MySerial.println("AT+CMGD=1,4");
                updateSerial();
            }
            else
            {
                sendSMS("Moisture value: " + String(percentMoisture) + "% - Above target" + "\nTemperature: " + String(readTemp()) + "C\nHumidity: " + String(readHumid()) + "%");
                MySerial.println("AT+CMGD=1,4");
                updateSerial();
            }
        }

        // Respond if the user texts "Help"
        if (strstr(byteArray, "Help"))
        {
            // Send commands
            delay(1000);
            sendSMS("Commands:\n1.Update: Plant status\n2.About: System info\n3.Help: Show commands");
            delay(1000);
            sendSMS("4.Water: Water the plant\n5.Lights: Turn LED strip on and off");
            MySerial.println("AT+CMGD=1,4");
            updateSerial();
        }

        // Respond if the user texts "About"
        if (strstr(byteArray, "About"))
        {
            // Explain features
            delay(1000);
            sendSMS("I'm a system that takes care of your indoor plant autonomously.");
            delay(1000);
            sendSMS("I use sensors to water a plant and provide lighting through an LED growth strip.");
            delay(1000);
            sendSMS("I can send this information to you by text using my GSM module.");
            MySerial.println("AT+CMGD=1,4");
            updateSerial();
        }

        // Respond if the user texts "Water"
        if (strstr(byteArray, "Water"))
        {
            // Water the plant
            delay(1000);
            waterPlant();
            sendSMS("Watering plant");
            MySerial.println("AT+CMGD=1,4");
            updateSerial();
            delay(2000);
        }

        // Respond if the user texts "Lights"
        if (strstr(byteArray, "Lights"))
        {
            lcd.clear();
            lcd.print("Activating LEDs");
            digitalWrite(ledPin, HIGH);
            sendSMS("Activating LEDs");
            MySerial.println("AT+CMGD=1,4");
            updateSerial();
            delay(5000);
            digitalWrite(ledPin, LOW);
            lcd.clear();
            lcd.print("Done");
            delay(3000);
        }
    }
}

/*----------LCD functions----------*/
// Print text on LCD
void lcdText()
{
    // Update screen every 3 seconds
    if (millis() % 3000 == 0)
    {
        if (counter == 0)
        {
            // Print humidity
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Humidity: ");
            lcd.print(readHumid(), 1);
            lcd.print("%");
            counter = counter + 1;
        }
        else if (counter == 1)
        {
            // Print temperature
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Temperature: ");
            lcd.setCursor(0, 1);
            lcd.print(readTemp(), 1);
            lcd.print((char)223);
            lcd.print(" Celsius");
            counter = counter + 1;
        }
        else if (counter == 2)
        {
            // Print moisture as a percentage + below/optimal/above target
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Moisture: ");
            percentMoisture = convertMoistureValue(readSoilMoisture());
            if (percentMoisture < 0.00)
            {
                lcd.print(-percentMoisture, 1);
                lcd.print("%");
                lcd.setCursor(0, 2);
                lcd.print("Below target");
            }
            if (percentMoisture == 0.00)
            {
                lcd.print(percentMoisture, 1);
                lcd.print("%");
                lcd.setCursor(0, 2);
                lcd.print("Optimal");
            }
            if (percentMoisture > 0.00)
            {
                lcd.print(percentMoisture, 1);
                lcd.print("%");
                lcd.setCursor(0, 2);
                lcd.print("Above target");
            }
            counter = 0;
        }
    }
}

// Initialize sensors + motor + LEDs
void setup()
{
    // open serial over USB
    Serial.begin(9600);

    // LCD Display
    // activate LCD module
    lcd.begin(16, 2);

    // Initialize GSM
    GSMInitialization();

    // Initialize soil moisture sensor and motor
    moistureTimer.setInterval(3600000, pollMoistureSensor);
    pinMode(moistureSensorPower, OUTPUT);
    digitalWrite(moistureSensorPower, HIGH);
    pinMode(motorPin, OUTPUT);
    digitalWrite(motorPin, LOW);

    // Initialize RH and Temperature sensor
    dht.setup(DHT22Pin, DHTesp::DHT22);

    // Initialize LDR timer and LED strip
    LDRTimer.setInterval(3600000, pollLDR);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
}

// Loop text on LCD screen
void loop()
{
    // Check for text
    GSMResponse(incomingByte);

    // Publish message on LCD
    lcdText();

    // Run LDRs
    LDRTimer.run();

    // Check sensors
    moistureTimer.run();
}