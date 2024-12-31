#pragma once
#include "IDictionary.h"
#include "DynamicArray.h"
#include "Pair.h"
#include <stdexcept>

// Тип функции, которая из объекта T (например, Person) возвращает double:
template <typename T>
using ExtractDoubleFunc = double(*)(const T&);

/////////////////////////////////////////////////////////
// FixedHistogram
/////////////////////////////////////////////////////////
template <typename T, typename KeyType = double>
class FixedHistogram {
private:
    // Будем хранить результат в IDictionary<KeyType, int>.
    // KeyType здесь может быть, например, пара (minRange, maxRange) или просто double,
    // но удобнее хранить каждый «диапазон» как пару (нижняя_граница, верхняя_граница).
    IDictionary<Pair<KeyType, KeyType>, int>* dictionary;

    KeyType minVal;
    KeyType maxVal;
    int numBins; // число «коробок» (диапазонов)

    // Функция, извлекающая числовое значение из T
    ExtractDoubleFunc<T> extractor;

public:
    // Конструктор принимает:
    // 1) словарь (BalanceBinaryTree или HashTable)
    // 2) минимальное значение, максимальное значение
    // 3) число диапазонов
    // 4) функцию извлечения double из T (если T - простой double, можно передать лямбду identity)
    FixedHistogram(IDictionary<Pair<KeyType, KeyType>, int>* dict,
        KeyType minVal, KeyType maxVal, int numBins,
        ExtractDoubleFunc<T> extractorFunc)
        : dictionary(dict), minVal(minVal), maxVal(maxVal), numBins(numBins), extractor(extractorFunc) {}

    void buildHistogram(const DynamicArray<T>& data) {
        // Сначала очистим словарь (при желании) — в зависимости от реализации
        // но словарь может быть новым, тогда он уже пуст.

        // Шаг
        double range = (maxVal - minVal) / numBins;

        // Сформируем все диапазоны в словаре, инициализируя счётчики 0
        for (int i = 0; i < numBins; i++) {
            Pair<KeyType, KeyType> bin;
            bin.key = minVal + i * range;        // нижняя граница
            bin.value = (i == numBins - 1)
                ? maxVal   // последний бин до maxVal включительно
                : (minVal + (i + 1) * range);
            dictionary->insert(bin, 0);
        }

        // Распределяем элементы
        for (int i = 0; i < data.GetLength(); i++) {
            double val = extractor(data.GetElem(i));
            // Ищем, в какой диапазон попадает val
            // Упрощённо пробежимся по бинам
            // (если бин — [low, high), кроме последнего, можно корректировать)
            for (int b = 0; b < numBins; b++) {
                Pair<KeyType, KeyType> bin;
                bin.key = minVal + b * range;
                bin.value = (b == numBins - 1)
                    ? maxVal
                    : (minVal + (b + 1) * range);

                if (val >= bin.key && val <= bin.value) {
                    int currentCount = dictionary->get(bin);
                    dictionary->insert(bin, currentCount + 1);
                    break;
                }
            }
        }
    }

    // Getter словаря (чтобы можно было вывести результат)
    IDictionary<Pair<KeyType, KeyType>, int>* getDictionary() const {
        return dictionary;
    }
};


/////////////////////////////////////////////////////////
// FloatingHistogram
/////////////////////////////////////////////////////////
template <typename T, typename KeyType = double>
class FloatingHistogram {
private:
    IDictionary<Pair<KeyType, KeyType>, int>* dictionary;
    int numBins;
    ExtractDoubleFunc<T> extractor;

public:
    FloatingHistogram(IDictionary<Pair<KeyType, KeyType>, int>* dict,
        int numBins,
        ExtractDoubleFunc<T> extractorFunc)
        : dictionary(dict), numBins(numBins), extractor(extractorFunc) {}

    void buildHistogram(const DynamicArray<T>& data) {
        if (numBins <= 0) {
            throw std::runtime_error("Number of bins must be positive");
        }
        if (data.GetLength() == 0) {
            return;
        }

        // Сортируем копию данных по значению extractor, чтобы «плавающие» бины
        // распределялись равномерно по числу элементов
        DynamicArray<T> sortedData(data);
        // Простейший пузырёк или любой другой метод сортировки
        for (int i = 0; i < sortedData.GetLength(); i++) {
            for (int j = 0; j < sortedData.GetLength() - i - 1; j++) {
                if (extractor(sortedData.GetElem(j)) > extractor(sortedData.GetElem(j + 1))) {
                    sortedData.Swap(sortedData.GetElem(j), sortedData.GetElem(j + 1));
                }
            }
        }

        // Сколько элементов должен вмещать один бин (в идеале)
        int binSize = data.GetLength() / numBins;
        int remainder = data.GetLength() % numBins;

        int startIndex = 0;
        for (int b = 0; b < numBins; b++) {
            int currentBinCount = binSize;
            if (b == numBins - 1) {
                // Добавим остаток в последний
                currentBinCount += remainder;
            }
            if (currentBinCount == 0) {
                // Если binSize < 1 и т.д. — защита
                continue;
            }

            // Нижняя граница
            double lowVal = extractor(sortedData.GetElem(startIndex));
            // Верхняя граница
            double highVal = extractor(sortedData.GetElem(startIndex + currentBinCount - 1));

            // Вставляем диапазон [lowVal, highVal]
            Pair<KeyType, KeyType> bin(lowVal, highVal);
            dictionary->insert(bin, currentBinCount);

            startIndex += currentBinCount;
        }
    }

    IDictionary<Pair<KeyType, KeyType>, int>* getDictionary() const {
        return dictionary;
    }
};

