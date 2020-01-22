/*
  KNN - Board Orientation

  This example presents the user with the option to either
  classify or train the boards current orientation as one of 
  6 categories using a KNN classifier. The IMU's acceleration sensor
  is used as input.

  NOTE: Make sure Serial Monitor's line ending setting
        is configured for "Newline" or "Both NL & CR".

  The circuit:
  - Arduino Nano 33 BLE or Arduino Nano 33 BLE Sensor

  This example code is in the public domain.
*/

#include <Arduino_KNN.h>
#include <Arduino_LSM9DS1.h>

const String BOARD_ORIENTATIONS[] = {
  "unknown",
  "up",               // flat, processor facing up
  "down",             // flat, processor facing down
  "digital pins up",  // landscape, analog pins down
  "analog pins up",   // landscape, analog pins up
  "connector up",     // portrait, USB connector up
  "connector down"    // portrait, USB connector down
};

// create KNN classifier, input will be array of 3 floats
KNNClassifier myKNN(3);

// used to store sensor reading from IMU's acceleration sensor
float acceleration[3];

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Board Orientation KNN");
  Serial.println();

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void loop() {
  // prompt the user
  Serial.println("Select an option: ");
  Serial.println();
  Serial.println("1) Classify board orientation");
  Serial.println("2) Train board orientation");
  Serial.println();

  int option = readNumber();

  // process option
  if (option == 1) {
    classifyBoardOrientation();
  } else if (option == 2) {
    trainBoardOrientation();
  }
}

void classifyBoardOrientation() {
  // read acceleration into array
  readAcceleration(acceleration);

  // use the KNN classifier to classify acceleration data
  int boardOrientation = myKNN.classify(acceleration, 1); // K = 1

  // print the classification out
  if (boardOrientation >= 1 && boardOrientation <= 6) {
    Serial.print("Predicted board orientation is: ");

    // use BOARD_ORIENTATIONS array to map board orientation from int to String
    Serial.println(BOARD_ORIENTATIONS[boardOrientation]);
  } else {
    Serial.println("Unknown board orientation! Did you train me?");
  }
  Serial.println();
}

void trainBoardOrientation() {
  // prompt the user
  Serial.println("Select an orientation to train: ");
  Serial.println();

  // print all board orientation options
  for (int i = 1; i <= 6; i++) { // start at 1, skip "unknown"
    Serial.print(i);
    Serial.print(") ");
    Serial.println(BOARD_ORIENTATIONS[i]);
  }
  Serial.println();

  // read option
  int boardOrientation = readNumber();

  if (boardOrientation >= 1 && boardOrientation <= 6) {
    // read acceleration into array
    readAcceleration(acceleration);

    // add acceleration as example to KNN
    myKNN.addExample(acceleration, boardOrientation);

    Serial.print("Learned current board orientation is: ");
    Serial.println(BOARD_ORIENTATIONS[boardOrientation]);
    Serial.println();
  }
}

// reads a number from the Serial Monitor
// expects new line
int readNumber() {
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

  return line.toInt();
}

// read acceleration data from the IMU
void readAcceleration(float acceleration[]) {
  // wait for new acceleration data to be available();
  while (!IMU.accelerationAvailable());

  // read acceleration data
  IMU.readAcceleration(acceleration[0], acceleration[1], acceleration[2]);
}
