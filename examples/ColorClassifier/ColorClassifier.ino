/*
  k-NN color classifier
  --------------------------
  
  This sketch classifies objects using a color sensor. 
  
  First you 'teach' the Arduino by putting an example of each object close to the color sensor. 
  After this the Arduino will guess the name of objects it is shown based on how similar 
  the color is to the previous examples it was shown.
    
  This example uses a simple case of k-Nearest Neighbour (k-NN) algorithm where k=1.

  HARDWARE: Arduino Nano BLE Sense

  USAGE: Follow prompts in serial console. Move object close to sample, then move it away

  This example code is in the public domain.
  
*/

#include <Arduino_KNN.h>
#include <Arduino_APDS9960.h>

// Create a new classifier with 3 dimensions for Red, Green and Blue levels
KNNClassifier myKNN(3);

// Create labels for examples to be classified
int classes = 3;
String label[3] = {"Apple", "Banana", "Orange"};

// The sketch starts by sampling examples
bool exampleMode = true;
int exampleClass = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!APDS.begin()) {
    Serial.println("Failled to initialized APDS!");
    while (1);
  }
}

void loop() {
  if (exampleMode) {
    Serial.print("Show me an example ");
    Serial.println(label[exampleClass]);
  }
  else {
    Serial.println("Let me guess your object");
  }

  // Read the color and proximity sensor
  int red, green, blue, proximity, colorTotal;
  while (1) {
    if (APDS.colorAvailable() && APDS.proximityAvailable()) {
      APDS.readColor(red, green, blue);
      colorTotal = (red + green + blue);
      proximity = APDS.readProximity();
      
      // Wait until an object is close and sufficiently illuminated
      if (proximity == 0 && colorTotal > 0) {
        break;
      }
    }
  }

  // Calculate the color ratios
  float redPercentage = (float)red / colorTotal;
  float greenPercentage = (float)green / colorTotal;
  float bluePercentage = (float)blue / colorTotal;

  // Print the red, green and blue percentage values
  Serial.print(redPercentage);
  Serial.print(",");
  Serial.print(greenPercentage);
  Serial.print(",");
  Serial.println(greenPercentage);

  // Put sample data into an array ready to pass to the k-NN classifier
  float input[] = {redPercentage, greenPercentage, bluePercentage};

  if (exampleMode) {
    // Add example color to the classifier
    myKNN.addExample(input, exampleClass);
    exampleClass++;
    
    // Once we have an example per class we switch to classifying objects
    if (exampleClass == classes) {
      exampleMode = false;
    }
  }
  else {
    // Classify the object
    int classification = myKNN.classify(input, 1); // classify input with K=1
    // print the classification
    Serial.println(label[classification]);
  }

  // Wait for the object to move away again
  while (!APDS.proximityAvailable() || APDS.readProximity()==0) {}

  Serial.println("");
}
