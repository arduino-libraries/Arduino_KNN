/*

  k-NN color classification
  -------------------------

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

const int  INPUTS = 3; // color sensor values; red, green and blue
const int  CLASSES = 3; // Number of objects we will classify (e.g. Apple, Banana, Orange)
const int  EXAMPLES_PER_CLASS = 1; // Number of times the users has to shown an example of an object

// Create a new KNNClassifier
KNNClassifier myKNN(INPUTS);

// K=1 means the classifier looks for the closest color example it's seen previously
const int K = 1;

// Names for each class (object type)
String label [CLASSES];

// Array to store data to pass to the KNN library
float color [INPUTS];

// Our sketch has three states
const int  GET_OBJECT_NAMES = 1;
const int  GET_EXAMPLES = 2;
const int  GUESSING = 3;

// We start by asking the use to type in their object names
int State = GET_OBJECT_NAMES;

int currentClass = 0;
int currentExample = 0;
int classification;


void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!APDS.begin()) {
    Serial.println("Failled to initialized APDS!");
    while (1);
  }

  Serial.println("Arduino k-NN color classifier");
}


void loop() {
  switch (State) {

    case GET_OBJECT_NAMES:
      Serial.print("Enter an object name:");
      label[currentClass] = readName();
      currentClass++;

      // Once we have a name for each class we switch to getting examples
      if (currentClass == CLASSES) {
        currentClass = 0;
        State = GET_EXAMPLES;
      }

      break;

    case GET_EXAMPLES:
      Serial.print("Show me an example ");
      Serial.println(label[currentClass]);

      // Wait for an object then read its color
      readColor(color);

      // Add example color to the k-NN model
      myKNN.addExample(color, currentClass);

      currentExample++;

      if (currentExample == EXAMPLES_PER_CLASS) {
        currentExample = 0;
        currentClass++;
      }

      // Once we have an example per class we switch to classifying objects
      if (currentClass == CLASSES) {
        State = GUESSING;
      }
      break;

    case GUESSING:
      Serial.println("Let me guess your object");
      // Wait for an object then read its color
      readColor(color);

      // Classify the object
      classification = myKNN.classify(color, K); // classify input with K=1
      // print the classification
      Serial.println(label[classification]);
      break;

    default:
      // statements
      break;
  }

  Serial.println("");
}


void readColor(float color[]) {
  // Read the color and proximity sensor
  int red, green, blue, proximity, total;
  while (1) {
    if (APDS.colorAvailable() && APDS.proximityAvailable()) {
      APDS.readColor(red, green, blue);
      total = (red + green + blue);
      proximity = APDS.readProximity();

      // Wait until an object is close and there's enough light
      if (proximity == 0 && total > 3) {
        break;
      }
    }
  }

  // Normalise the color sample data and put into color Input
  color[0] = (float)red / total;
  color[1] = (float)green / total;
  color[2] = (float)blue / total;

  // Print the red, green and blue percentage values
  Serial.print(color[0]);
  Serial.print(",");
  Serial.print(color[1]);
  Serial.print(",");
  Serial.println(color[2]);

  // Wait for the object to move away again
  while (!APDS.proximityAvailable() || APDS.readProximity() == 0) {}
}


// reads a name from the Serial Monitor
String readName() {
  String line;

  while (1) {
    if (Serial.available()) {
      char c = Serial.read();

      if (c == '\r') {
        // ignore
        continue;
      } else if (c == '\n') {
        break;
      }

      line += c;
    }
  }

  return line;
}
