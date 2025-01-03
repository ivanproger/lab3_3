#pragma once
#include <iostream>
#include <limits>
#include <cassert>
#include <chrono>
#include <iomanip>

#include "BalanceBinaryTree.h"
#include "HashTable.h"
#include "DynamicArray.h"
#include "Person.h"
#include "Histogram.h"
#include "SparseMatrix.h"
#include "Pair.h"
#include "IDictionary.h"
#include "DefaultHash.h"
#include "Utils.h"
#include "FunctionalTests.h"
#include "LoadTests.h"

void createHistogram();
void manageHistogram(IDictionary<Pair<double, double>, int>* dict, bool isFixed);
void createSparseMatrix();
void manageSparseMatrix(SparseMatrix<double>& matrix);
void displayMenu();
void runInterface();