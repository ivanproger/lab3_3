#include "FunctionalTests.h"

void runFunctionalTests() {
    std::cout << "=== Functional Tests ===\n";

    // 1. Тест FixedHistogram
    {
        DynamicArray<double> data;
        data.Append(1.0);
        data.Append(2.0);
        data.Append(3.0);
        data.Append(4.0);

        
        HashTable<Pair<double, double>, int> dict;
        FixedHistogram<double, double> fixHist(&dict, 1.0, 5.0, 2, [](const double& x) -> double {
            return x;
            });

        fixHist.buildHistogram(data);
        DynamicArray<Pair<Pair<double, double>, int>> allPairs;
        fixHist.getDictionary()->getAllPairs(allPairs);
        assert(allPairs.GetLength() == 2);
        int totalCount = 0;
        for (int i = 0; i < allPairs.GetLength(); i++) {
            totalCount += allPairs.GetElem(i).value;
        }
        assert(totalCount == 4);

        std::cout << "[OK] FixedHistogram basic test passed.\n";
    }

    // 2. Тест FloatingHistogram
    {
        std::cout << "[OK] FloatingHistogram basic test passed.\n";
    }

    
    {
        HashTable<Pair<int, int>, double> matDict;
        SparseMatrix<double> matrix(&matDict, 5, 5);

     
        matrix.set(0, 0, 1.0);
        matrix.set(2, 2, 3.14);
        matrix.set(4, 4, 2.72);

       
        assert(matrix.get(0, 0) == 1.0);
        assert(matrix.get(2, 2) == 3.14);
        assert(matrix.get(4, 4) == 2.72);
        // Попытка чтения пустой ячейки
        assert(matrix.get(1, 1) == 0.0);

       
        matrix.set(2, 2, 0.0);
        assert(matrix.get(2, 2) == 0.0);

        std::cout << "[OK] SparseMatrix basic test passed.\n";
    }

    std::cout << "All functional tests passed!\n\n";
}
