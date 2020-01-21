/*
  SimpleKNN

  This example demonstrates how to use the Arduino KNN library.

  It creates a KNN classifier that expects an input array of 2 (floats).
  Then adds 4 example inputs with their respective classifications.
  After this it demonstrates how to classify an input value and get
  the classifications confidence.

  This example code is in the public domain.
*/

#include <Arduino_KNN.h>

// Create a new KNNClassifier, input values are array of 2 (floats),
// change if you need a bigger input size
KNNClassifier myKNN(2);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Simple KNN");
  Serial.println();

  Serial.print("Adding examples to myKNN ... ");
  Serial.println();

  // add examples to KNN
  float example1[] = { 7.0, 7.0 };
  float example2[] = { 5.0, 5.0 };
  float example3[] = { 9.0, 9.0 };
  float example4[] = { 5.0, 5.0 };

  myKNN.addExample(example1, 7); // add example for class 7
  myKNN.addExample(example2, 5); // add example for class 5
  myKNN.addExample(example3, 9); // add example for class 9
  myKNN.addExample(example4, 5); // add example for class 5 (again)

  // get and print out the KNN count
  Serial.print("\tmyKNN.getCount() = ");
  Serial.println(myKNN.getCount());
  Serial.println();

  // you can also print the counts by class
  //  Serial.print("\tmyKNN.getCountByClass(5) = ");
  //  Serial.println(myKNN.getCountByClass(5)); // expect 2

  // classify the input
  Serial.println("Classifying input ...");

  float input[] = { 5.0, 5.0 };

  int classification = myKNN.classify(input, 3); // classify input with K=3
  float confidence = myKNN.confidence();

  // print the classification and confidence
  Serial.print("\tclassification = ");
  Serial.println(classification);

  // since there are 2 examples close to the input and K = 3,
  // expect the confidence to be: 2/3 = ~0.67
  Serial.print("\tconfidence     = ");
  Serial.println(confidence);
}

void loop() {
  // do nothing
}
