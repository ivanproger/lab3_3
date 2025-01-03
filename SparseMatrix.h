#pragma once
#include "IDictionary.h"
#include "DynamicArray.h"
#include "Pair.h"
#include <stdexcept>

template <typename T>
class SparseMatrix {
private:
    IDictionary<Pair<int, int>, T>* dict;
    int rows;
    int cols;

public:
    SparseMatrix(IDictionary<Pair<int, int>, T>* dictionaryImpl, int rows, int cols)
        : dict(dictionaryImpl), rows(rows), cols(cols) {}

    void set(int row, int col, T value) {
        if (row < 0 || row > rows || col < 0 || col > cols) {
            throw std::out_of_range("Index out of range in SparseMatrix");
        }
        Pair<int, int> key(row, col);

        if (value == T()) {
            // при установке 0 удаляем из словаря
            dict->remove(key);
        }
        else {
            dict->insert(key, value);
        }
    }

    T get(int row, int col) const {
        if (row < 0 || row > rows || col < 0 || col > cols) {
            throw std::out_of_range("Index out of range in SparseMatrix");
        }
        Pair<int, int> key(row, col);

        if (dict->exist(key)) {
            return dict->get(key);
        }
        return T(); // 0 по умолчанию
    }

    int getNumRows() const { return rows; }
    int getNumCols() const { return cols; }

    void getNonZeroElements(DynamicArray<Pair<Pair<int, int>, T>>& arr) const {
        dict->getAllPairs(arr);
    }
};

