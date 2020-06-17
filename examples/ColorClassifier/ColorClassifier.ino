/*

  k-NN color classification
  -------------------------

  This sketch classifies objects using a color sensor.

  First you 'teach' the Arduino by putting an example of each object close to the color sensor.
  After this the Arduino will guess the name of objects it is shown based on how similar
  the color is to the examples it has seen.

  This example uses a case of k-Nearest Neighbour (k-NN) algorithm where k=5.

  HARDWARE: Arduino Nano BLE Sense

  USAGE: Follow prompts in serial console. Move object close to the board to sample its color, then move it away.

  Works best in a well lit area with objects of different colors.

 
  This example code is in the public domain.

*/

#include <Arduino_KNN.h>
#include <Arduino_APDS9960.h>

const int INPUTS = 3; // Classifier input is color sensor data; red, green and blue levels
const int CLASSES = 3; // Number of objects we will classify (e.g. Apple, Banana, Orange)
const int EXAMPLES_PER_CLASS = 30; // Number of times user needs to show examples for each object

const int K = 5;

// Create a new KNNClassifier
KNNClassifier myKNN(INPUTS);

// Names for each class (object type)
String      label[CLASSES] = {"Apple", "Lime", "Orange"};

// Array to store data to pass to the KNN library
float color[INPUTS];

// Threshold for color brightness
const int THRESHOLD = 5;

void setup() {

  Serial.begin(9600);
  while (!Serial);

  if (!APDS.begin()) {
    Serial.println("Failled to initialized APDS!");
    while (1);
  }

  Serial.println("Arduino k-NN color classifier");

  // Ask user for the name of each object
  for (int currentClass = 0; currentClass < CLASSES; currentClass++) {

  
    // Ask user to show examples of each object
    for (int currentExample = 0; currentExample < EXAMPLES_PER_CLASS; currentExample++) {

      Serial.print("Show me an example ");
      Serial.println(label[currentClass]);

      // Wait for an object then read its color
      readColor(color);

      // Add example color to the k-NN model
      myKNN.addExample(color, currentClass);

    }
      // Wait for the object to move away again
  while (!APDS.proximityAvailable() || APDS.readProximity() == 0) {}
  }
}


void loop() {

  int classification;

  // Wait for the object to move away again
  while (!APDS.proximityAvailable() || APDS.readProximity() == 0) {}

  Serial.println("Let me guess your object");

  // Wait for an object then read its color
  readColor(color);

  // Classify the object
  classification = myKNN.classify(color, K);

  // Print the classification
  Serial.print("You showed me ");
  Serial.println(label[classification]);
 
}


void readColor(float color[]) {
  int red, green, blue, proximity, colorTotal = 0;

 // Wait for the object to move close
 while (!APDS.proximityAvailable() || APDS.readProximity() > 0){}

  // Wait until we have a color bright enough
  while (colorTotal < THRESHOLD) {

    // Sample if color is available and object is close
    if (APDS.colorAvailable()) {

      // Read color and proximity
      APDS.readColor(red, green, blue);
      colorTotal = (red + green + blue);
    }
  }

  // Normalise the color sample data and put it in the classifier input array
  color[0] = (float)red / colorTotal;
  color[1] = (float)green / colorTotal;
  color[2] = (float)blue / colorTotal;

  // Print the red, green and blue percentage values
  Serial.print(color[0]);
  Serial.print(",");
  Serial.print(color[1]);
  Serial.print(",");
  Serial.println(color[2]);
}
