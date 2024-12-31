#pragma once
#include "IDictionary.h"
#include "DynamicArray.h"
#include "Pair.h"
#include <stdexcept>

template <typename T>
class SparseVector {
private:
    IDictionary<int, T>* dict; // ключ - индекс, значение - элемент
    int size; // полна€ длина вектора

public:
    SparseVector(IDictionary<int, T>* dictionaryImpl, int size)
        : dict(dictionaryImpl), size(size) {}

    void set(int index, T value) {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range in SparseVector");
        }
        if (value == T()) {
            // значение 0 Ч удал€ем из словар€ (т.к. разреженный)
            dict->remove(index);
        }
        else {
            dict->insert(index, value);
        }
    }

    T get(int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range in SparseVector");
        }
        if (dict->exist(index)) {
            return dict->get(index);
        }
        return T(); // 0 по умолчанию
    }

    int getSize() const {
        return size;
    }

    // ћетод, позвол€ющий собрать все ненулевые элементы
    void getNonZeroElements(DynamicArray<Pair<int, T>>& arr) const {
        dict->getAllPairs(arr);
    }
};

