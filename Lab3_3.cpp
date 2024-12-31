// main.cpp
#include <iostream>
#include "BalanceBinaryTree.h"
#include "HashTable.h"
#include "DynamicArray.h"
#include "Person.h"
#include "Histogram.h"
#include "SparseVector.h"
#include "SparseMatrix.h"
#include "Pair.h"
#include "IDictionary.h"
#include "DefaultHash.h"

// Функции печати
#include "Utils.h" // Предположим, что здесь лежат PrintDictionary(...) и т.д.

// Функция для извлечения роста из объекта Person
double extractHeight(const Person& p) {
    return p.getHeight();
}

// Идентичная функция для double
double extractDouble(const double& d) {
    return d; // identity
}

int main() {
    try {
        // ------------------- 1. Пример гистограммы FixedHistogram с Person по росту -------------------
        BalanceBinaryTree<Pair<double, double>, int> treeDict;
        FixedHistogram<Person, double> fixedHist(&treeDict, 150.0, 210.0, 3, extractHeight);

        DynamicArray<Person> people;
        people.Append(Person(1, "Ivanov", "Ivan", 1990, 180.0, 75.0));
        people.Append(Person(2, "Petrov", "Petr", 1985, 165.0, 68.0));
        people.Append(Person(3, "Sidorov", "Sidr", 1970, 200.0, 90.0));
        people.Append(Person(4, "Fedorov", "Fedor", 2000, 155.0, 60.0));

        fixedHist.buildHistogram(people);

        std::cout << "FixedHistogram (Person by Height):" << std::endl;
        PrintDictionary(fixedHist.getDictionary());
        std::cout << std::endl;

        // ------------------- 2. Пример гистограммы FloatingHistogram c double -------------------
        HashTable<Pair<double, double>, int> hashDict;
        FloatingHistogram<double, double> floatHist(&hashDict, 3, extractDouble);

        DynamicArray<double> values;
        values.Append(1.1);
        values.Append(2.2);
        values.Append(2.3);
        values.Append(5.0);
        values.Append(1.05);
        values.Append(10.0);

        floatHist.buildHistogram(values);

        std::cout << "FloatingHistogram (double):" << std::endl;
        PrintDictionary(floatHist.getDictionary());
        std::cout << std::endl;

        // ------------------- 3. Пример SparseVector -------------------
        // Пусть будет HashTable<int, double> внутри
        HashTable<int, double> vecDict;
        SparseVector<double> sparseVec(&vecDict, 10);

        sparseVec.set(2, 3.14);
        sparseVec.set(5, 2.71);
        sparseVec.set(7, 0.0); // это удалит элемент (если был)

        std::cout << "SparseVector example (size=10):" << std::endl;
        for (int i = 0; i < 10; i++) {
            std::cout << "Index " << i << ": " << sparseVec.get(i) << std::endl;
        }
        std::cout << std::endl;

        // ------------------- 4. Пример SparseMatrix -------------------
        // Пусть будет BalanceBinaryTree<Pair<int,int>, double>
        BalanceBinaryTree<Pair<int, int>, double> matDict;
        SparseMatrix<double> sparseMat(&matDict, 5, 5); // 5x5 матрица

        sparseMat.set(0, 0, 1.0);
        sparseMat.set(1, 2, 2.5);
        sparseMat.set(4, 4, 10.0);

        std::cout << "SparseMatrix 5x5 example:" << std::endl;
        for (int r = 0; r < 5; r++) {
            for (int c = 0; c < 5; c++) {
                std::cout << sparseMat.get(r, c) << " ";
            }
            std::cout << std::endl;
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }

    return 0;
}
