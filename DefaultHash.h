// DefaultHash.h
#pragma once

#include <functional>
#include "Pair.h"

// Общий шаблон хеш-функции
template <typename Key>
struct DefaultHash {
    size_t operator()(const Key& key) const {
        return std::hash<Key>()(key);
    }
};

// Специализация для Pair<int, int>
template <>
struct DefaultHash<Pair<int, int>> {
    size_t operator()(const Pair<int, int>& keyPair) const {
        // Простое комбинирование хешей двух целых чисел
        return std::hash<int>()(keyPair.key) ^ (std::hash<int>()(keyPair.value) << 1);
    }
};

// Специализация для Pair<double, double>
template <>
struct DefaultHash<Pair<double, double>> {
    size_t operator()(const Pair<double, double>& keyPair) const {
        // Комбинирование хешей двух вещественных чисел
        return std::hash<double>()(keyPair.key) ^ (std::hash<double>()(keyPair.value) << 1);
    }
};

