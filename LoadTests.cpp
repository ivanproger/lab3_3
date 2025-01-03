#include "LoadTests.h"

DynamicArray<double> generateRandomNumbers(double min, double max, int count) {
    DynamicArray<double> numbers;
    numbers.Clear();
    for (int i = 0; i < count; ++i) {
        double num = min + static_cast<double>(rand()) / RAND_MAX * (max - min);
        numbers.Append(num);
    }
    return numbers;
}

struct TestResult {
    std::string structure;
    std::string operation;
    int dataSize;
    long long timeMs;
};

struct Structure {
    std::string name;
    enum Type { BALANCED_BINARY_TREE, HASH_TABLE } type;
};

struct Operation {
    std::string name;
    enum Type { FIXED_HISTOGRAM, FLOATING_HISTOGRAM, SPARSE_MATRIX } type;
};

void displayResults(const DynamicArray<TestResult>& allResults, const int testSizes[], int numTestSizes,
    const Structure structures[], int numStructures,
    const Operation operations[], int numOperations) {
    std::cout << "\n=== Load Test Results ===\n";

    std::cout << std::left << std::setw(25) << "Structure"
        << std::left << std::setw(25) << "Operation";

    for (int i = 0; i < numTestSizes; ++i) {
        std::cout << std::left << std::setw(15) << ("Size " + std::to_string(testSizes[i]));
    }
    std::cout << "\n";

    std::cout << std::string(25 + 25 + 15 * numTestSizes, '-') << "\n";

    for (int s = 0; s < numStructures; ++s) {
        for (int o = 0; o < numOperations; ++o) {
            std::cout << std::left << std::setw(25) << structures[s].name
                << std::left << std::setw(25) << operations[o].name;

            for (int t = 0; t < numTestSizes; ++t) {
                bool found = false;
                for (int r = 0; r < allResults.GetLength(); ++r) {
                    const TestResult& tr = allResults.GetElem(r);
                    if (tr.structure == structures[s].name &&
                        tr.operation == operations[o].name &&
                        tr.dataSize == testSizes[t]) {
                        std::cout << std::left << std::setw(15) << tr.timeMs;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    std::cout << std::left << std::setw(15) << "-";
                }
            }
            std::cout << "\n";
        }
    }

    std::cout << "\nLoad tests completed.\n";
}

void runLoadTests() {
    std::cout << "=== Load Tests ===\n";

    srand(static_cast<unsigned int>(time(NULL)));

    const int testSizes[] = { 1000, 5000, 10000, 20000 };
    const int numTestSizes = sizeof(testSizes) / sizeof(testSizes[0]);

    Structure structures[] = {
        {"BalancedBinaryTree", Structure::BALANCED_BINARY_TREE},
        {"HashTable", Structure::HASH_TABLE}
    };
    const int numStructures = sizeof(structures) / sizeof(structures[0]);

    Operation operations[] = {
        {"FixedHistogram Build", Operation::FIXED_HISTOGRAM},
        {"FloatingHistogram Build", Operation::FLOATING_HISTOGRAM},
        {"SparseMatrix Build", Operation::SPARSE_MATRIX}
    };
    const int numOperations = sizeof(operations) / sizeof(operations[0]);

    DynamicArray<TestResult> allResults;

    for (int i = 0; i < numTestSizes; ++i) {
        int currentSize = testSizes[i];
        std::cout << "\n--- Testing with data size: " << currentSize << " ---\n";

        DynamicArray<double> bigData = generateRandomNumbers(0.0, 100.0, currentSize);

        for (int j = 0; j < numStructures; ++j) {
            Structure currentStructure = structures[j];
            std::cout << "\nStructure: " << currentStructure.name << "\n";

            for (int k = 0; k < numOperations; ++k) {
                Operation currentOperation = operations[k];
                std::cout << "Operation: " << currentOperation.name << " ... ";

                auto start = std::chrono::high_resolution_clock::now();

                if (currentOperation.type == Operation::FIXED_HISTOGRAM) {
                    if (currentStructure.type == Structure::BALANCED_BINARY_TREE) {
                        BalanceBinaryTree<Pair<double, double>, int> treeDict;
                        FixedHistogram<double, double> fixHistTree(&treeDict, 0.0, 100.0, 10,
                            [](const double& x) -> double { return x; });
                        fixHistTree.buildHistogram(bigData);
                    }
                    else if (currentStructure.type == Structure::HASH_TABLE) {
                        HashTable<Pair<double, double>, int> hashDict(10000, 0.5);
                        FixedHistogram<double, double> fixHistHash(&hashDict, 0.0, 100.0, 10,
                            [](const double& x) -> double { return x; });
                        fixHistHash.buildHistogram(bigData);
                    }
                }
                else if (currentOperation.type == Operation::FLOATING_HISTOGRAM) {
                    if (currentStructure.type == Structure::BALANCED_BINARY_TREE) {
                        BalanceBinaryTree<Pair<double, double>, int> treeDict;
                        FloatingHistogram<double, double> floatHistTree(&treeDict, 100.0,
                            [](const double& x) -> double { return x; });
                        floatHistTree.buildHistogram(bigData);
                    }
                    else if (currentStructure.type == Structure::HASH_TABLE) {
                        HashTable<Pair<double, double>, int> hashDict(10000, 0.5);
                        FloatingHistogram<double, double> floatHistHash(&hashDict, 100.0,
                            [](const double& x) -> double { return x; });
                        floatHistHash.buildHistogram(bigData);
                    }
                }
                else if (currentOperation.type == Operation::SPARSE_MATRIX) {
                    if (currentStructure.type == Structure::BALANCED_BINARY_TREE) {
                        BalanceBinaryTree<Pair<int, int>, double> matTree;
                        SparseMatrix<double> sparseMatTree(&matTree, 1000, 1000);
                        for (int m = 0; m < currentSize; m++) {
                            int r = rand() % 1000;
                            int c = rand() % 1000;
                            double val = static_cast<double>(rand() % 1000) / 10.0;
                            sparseMatTree.set(r, c, val);
                        }
                    }
                    else if (currentStructure.type == Structure::HASH_TABLE) {
                        HashTable<Pair<int, int>, double> matHash(200000, 0.5);
                        SparseMatrix<double> sparseMatHash(&matHash, 1000, 1000);
                        for (int m = 0; m < currentSize; m++) {
                            int r = rand() % 1000;
                            int c = rand() % 1000;
                            double val = static_cast<double>(rand() % 1000) / 10.0;
                            sparseMatHash.set(r, c, val);
                        }
                    }
                }

                auto end = std::chrono::high_resolution_clock::now();
                auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

                std::cout << dur << " ms\n";

                TestResult result;
                result.structure = currentStructure.name;
                result.operation = currentOperation.name;
                result.dataSize = currentSize;
                result.timeMs = dur;
                allResults.Append(result);
            }
        }
    }

    displayResults(allResults, testSizes, numTestSizes, structures, numStructures, operations, numOperations);
}
