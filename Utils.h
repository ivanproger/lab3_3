// Utils.h
#pragma once
#include <iostream>
#include "IDictionary.h"
#include "DynamicArray.h"
#include "Pair.h"

// Печать словаря (K, V) — универсальный случай
template <typename K, typename V>
void PrintDictionary(const IDictionary<K, V>* dict) {
    DynamicArray<Pair<K, V>> pairs;
    dict->getAllPairs(pairs);

    for (int i = 0; i < pairs.GetLength(); i++) {
        const Pair<K, V>& p = pairs.GetElem(i);
        std::cout << "[" << p.key << " -> " << p.value << "]" << std::endl;
    }
}

// Перегрузка для ключа Pair<double, double> и значения int
inline void PrintDictionary(const IDictionary<Pair<double, double>, int>* dict) {
    DynamicArray<Pair<Pair<double, double>, int>> pairs;
    dict->getAllPairs(pairs);

    for (int i = 0; i < pairs.GetLength(); i++) {
        const Pair<Pair<double, double>, int>& p = pairs.GetElem(i);
        std::cout << "[(" << p.key.key << ", " << p.key.value << ") -> "
            << p.value << "]" << std::endl;
    }
}

// Можно аналогично сделать перегрузки для других типов ключей и значений
