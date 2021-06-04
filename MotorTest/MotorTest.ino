int motorPin = 3;

int ledPin2 = 4;

void setup()
{
    pinMode(motorPin, OUTPUT);
    digitalWrite(motorPin, HIGH);
    pinMode(ledPin2, OUTPUT);
    digitalWrite(ledPin2, HIGH);
}
void loop()
{
    digitalWrite(motorPin, LOW);
    delay(3000);
    digitalWrite(motorPin, HIGH);
    delay(3000);
    digitalWrite(ledPin2, LOW);
    delay(5000);
    digitalWrite(ledPin2, HIGH);
    delay(5000);
}
