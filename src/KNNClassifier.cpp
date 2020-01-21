/*
  This file is part of the Arduino_KNN library.
  Copyright (c) 2020 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <limits.h>
#include <float.h>

#include "utility/KNNNode.h"

#include "KNNClassifier.h"

KNNClassifier::KNNClassifier(int inputLength) :
  _inputLength(inputLength),
  _examples(NULL)
{
}

KNNClassifier::~KNNClassifier()
{
  clearAllClasses();
}

void KNNClassifier::addExample(const float input[], int class_)
{
  // create new node
  KNNNode* newNode = new KNNNode(input, _inputLength, class_);

  // add new node to examples list
  if (_examples == NULL) {
    _examples = newNode;
  } else {
    KNNNode* node = _examples;

    while (1) {
      KNNNode* next = node->next();

      if (next == NULL) {
        node->setNext(newNode);

        break;
      }

      node = next;
    }
  }
}

int KNNClassifier::classify(const float input[], int k)
{
  if (k == 0 || k > getCount()) {
    _confidence = NAN;

    return 0;
  }

  struct {
    float distance;
    int class_;
  } nearestNeighbors[k];

  // default values
  for (int i = 0; i < k; i++) {
    nearestNeighbors[i].distance = FLT_MAX;
    nearestNeighbors[i].class_ = INT_MIN;
  }

  // find the K nearest neighbours
  KNNNode* node = _examples;

  while (node != NULL) {
    float distance = node->distance(input, _inputLength);

    for (int i = 0; i < k; i++) {
      if (distance < nearestNeighbors[i].distance) {
        // shift current k's by one
        for (int j = k - 1; j > i; j--) {
          nearestNeighbors[j] = nearestNeighbors[j - 1];
        }

        // insert new k
        nearestNeighbors[i].distance = distance;
        nearestNeighbors[i].class_ = node->class_();

        break;
      }
    }

    node = node->next();
  }

  // count the class numbers
  struct {
    int class_;
    int count;
  } classCounts[k];

  // default values
  for (int i = 0; i < k; i++) {
    classCounts[i].count = 0;
    classCounts[i].class_ = 0;
  }

  for (int i = 0; i < k; i++) {
    int class_ = nearestNeighbors[i].class_;
    bool classFound = false;

    // try to find an existing slot
    for (int j = 0; j < k; j++) {
      if (classCounts[j].class_ == class_) {
        classCounts[j].count++;

        classFound = true;
        break;
      }
    }

    if (classFound) {
      // done this nearest neighbor
      continue;
    }

    // insert class in the first free slot
    for (int j = 0; j < k; j++) {
      if (classCounts[j].count == 0) {
        classCounts[j].count = 1;
        classCounts[j].class_ = class_;
        break;
      }
    }
  }

  int maxCount = 0;
  int class_ = 0;

  for (int i = 0; i < k; i++) {
    int count = classCounts[i].count;

    if (maxCount < count) {
      maxCount = count;
      class_ = classCounts[i].class_;
    }
  }

  _confidence = (float)maxCount / (float)k;

  return class_;
}

float KNNClassifier::confidence()
{
  return _confidence;
}

void KNNClassifier::clearClass(int class_)
{
  KNNNode* previous = NULL;
  KNNNode* node = _examples;

  while (node != NULL) {
    KNNNode* next = node->next();

    if (node->class_() == class_) {
      if (previous == NULL) {
        _examples = next;
      } else {
        previous->setNext(next);
      }

      delete node;
    } else {
      previous = node;
    }

    node = next;
  }
}

void KNNClassifier::clearAllClasses()
{
  KNNNode* node = _examples;

  while (node != NULL) {
    KNNNode* next = node->next();

    delete node;

    node = next;
  }

  _examples = NULL;
}

int KNNClassifier::getCountByClass(int class_)
{
  int count = 0;

  KNNNode* node = _examples;

  while (node != NULL) {
    if (node->class_() == class_) {
      count++;
    }

    node = node->next();
  }

  return count;
}

int KNNClassifier::getCount()
{
  int count = 0;

  KNNNode* node = _examples;

  while (node != NULL) {
    count++;

    node = node->next();
  }

  return count;
}

// int KNNClassifier::save(Print& out)
// {
// }

// int KNNClassifier::load(Stream& in)
// {
// }
