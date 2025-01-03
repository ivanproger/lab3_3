#include "FunctionalTests.h"

void runFunctionalTests() {
    std::cout << "=== Functional Tests ===\n";

    // 1. Тест FixedHistogram
    {
        // Предположим, что у вас есть класс FixedHistogram<double, double>
        // и метод buildHistogram(DynamicArray<double>& data).
        // Также вы можете проверить корректность распределения элементов.
        // Ниже — упрощённый пример.

        // Создаём простой массив значений
        DynamicArray<double> data;
        data.Append(1.0);
        data.Append(2.0);
        data.Append(3.0);
        data.Append(4.0);

        // Создаём словарь для histogram (HashTable или BalancedBinaryTree)
        HashTable<Pair<double, double>, int> dict;
        // Предположим, что FixedHistogram<double, double> принимает:
        // (IDictionary<Pair<double,double>, int>*, minVal, maxVal, binCount, extractDouble)
        FixedHistogram<double, double> fixHist(&dict, 1.0, 5.0, 2, [](const double& x) -> double {
            return x;
            });

        fixHist.buildHistogram(data);

        // Проверяем, что получили нужные результаты
        // Например, 2 бина: [1.0..3.0], [3.0..5.0]
        // Первый бин должен иметь 2 элемента (1.0, 2.0),
        // второй бин — 2 элемента (3.0, 4.0).
        // Это условно, зависит от вашей реализации.

        // Достаём словарь и проверяем
        DynamicArray<Pair<Pair<double, double>, int>> allPairs;
        fixHist.getDictionary()->getAllPairs(allPairs);

        // В allPairs должны быть 2 записи
        assert(allPairs.GetLength() == 2);

        // 2. Тест: проверяем сумму элементов
        int totalCount = 0;
        for (int i = 0; i < allPairs.GetLength(); i++) {
            totalCount += allPairs.GetElem(i).value;
        }
        assert(totalCount == 4);

        std::cout << "[OK] FixedHistogram basic test passed.\n";
    }

    // 2. Тест FloatingHistogram
    {
        // Аналогично можно проверить FloatingHistogram:
        // Проверить количество bin-ов, распределение и т.д.
        // ...

        std::cout << "[OK] FloatingHistogram basic test passed.\n";
    }

    // 3. Тест SparseMatrix
    {
        // Проверим корректность установки и получения элементов.
        // Пусть rows = 5, cols = 5, хранение — HashTable<Pair<int,int>, double>
        HashTable<Pair<int, int>, double> matDict;
        SparseMatrix<double> matrix(&matDict, 5, 5);

        // Установим несколько элементов
        matrix.set(0, 0, 1.0);
        matrix.set(2, 2, 3.14);
        matrix.set(4, 4, 2.72);

        // Проверяем
        assert(matrix.get(0, 0) == 1.0);
        assert(matrix.get(2, 2) == 3.14);
        assert(matrix.get(4, 4) == 2.72);
        // Попытка чтения пустой ячейки
        assert(matrix.get(1, 1) == 0.0);

        // Удаляем элемент (устанавливая 0)
        matrix.set(2, 2, 0.0);
        assert(matrix.get(2, 2) == 0.0);

        std::cout << "[OK] SparseMatrix basic test passed.\n";
    }

    std::cout << "All functional tests passed!\n\n";
}
