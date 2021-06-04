// LDR pin connections
//  name  = analogpin;
int ldrlt = 2; //LDR top left
int ldrrt = 3; //LDR top rigt
int ldrld = 4; //LDR down left
int ldrrd = 5; //ldr down rigt

void setup()
{
    Serial.begin(9600);
}

void loop() 
{
    // Determine LDR values
    int lt = analogRead(ldrlt); // top left
    int rt = analogRead(ldrrt); // top right
    int ld = analogRead(ldrld); // down left
    int rd = analogRead(ldrrd); // down rigt

    // Calculate averages
    int avt = (lt + rt) / 2; // average value top
    int avd = (ld + rd) / 2; // average value down
    int avl = (lt + ld) / 2; // average value left
    int avr = (rt + rd) / 2; // average value right

    // LDR values
    String leftTextTop = "Top left: ";
    String topLeftText = leftTextTop + lt ;
    Serial.println(topLeftText);

    String rightTextTop = "Top right: ";
    String topRightText = rightTextTop + rt ;
    Serial.println(topRightText);

    String leftTextBottom = "Bottom left: ";
    String bottomLeftText = leftTextBottom + ld ;
    Serial.println(bottomLeftText);

    String rightTextBottom = "Bottom right: ";
    String bottomRightText = rightTextBottom + rd ;
    Serial.println(bottomRightText);

    Serial.println("------Average for each side------");

    // Average value for each side
    String topAverageText = "Top average: ";
    String topAverage = topAverageText + avt ;
    Serial.println(topAverage);

    String rightAverageText = "Right average: ";
    String rightAverage = rightAverageText + avr ;
    Serial.println(rightAverage);

    String bottomAverageText = "Bottom average: ";
    String bottomAverage = bottomAverageText + avd ;
    Serial.println(bottomAverage);
    
    String leftAverageText = "Left average: ";
    String leftAverage = leftAverageText + avl ;
    Serial.println(leftAverage);
    
    delay(10000);
}
