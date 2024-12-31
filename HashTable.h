// HashTable.h
#pragma once

#include "IDictionary.h"
#include "DynamicArray.h"
#include "Pair.h"
#include "DefaultHash.h" // ���������� DefaultHash
#include <stdexcept>
#include <functional>
#include <iostream> // ��� ���������� �������

// ���-������� � ���������
template <typename Key, typename Value, typename HashFunc = DefaultHash<Key>>
class HashTable : public IDictionary<Key, Value> {
private:
    struct Bucket {
        DynamicArray<Pair<Key, Value>> chain; // ������� ���
    };

    DynamicArray<Bucket> buckets;
    int count;             // ���������� ������� ���������
    double loadFactor;     // ����� ������������ ������� ��� ������
    HashFunc hashFunc;

    void rehash() {
        int oldCapacity = buckets.GetLength();
        DynamicArray<Bucket> oldBuckets = buckets;

        int newCapacity = oldCapacity * 2;
        if (newCapacity <= 0) {
            newCapacity = 1;
        }

        // �������������� ����� ������
        buckets = DynamicArray<Bucket>(newCapacity);
        count = 0; // �� ������ ����� ��������� ��� ��������

        for (int i = 0; i < newCapacity; ++i) {
            buckets.Append(Bucket());
        }

        // ��������� �������� �� ������ ������� � �����
        for (int i = 0; i < oldCapacity; i++) {
            Bucket& oldBucket = oldBuckets.GetElem(i);
            for (int j = 0; j < oldBucket.chain.GetLength(); j++) {
                Pair<Key, Value>& p = oldBucket.chain.GetElem(j);
                insert(p.key, p.value);
            }
        }
    }

public:
    // �����������
    HashTable(int initialCapacity = 16, double loadFactor = 0.75)
        : buckets(initialCapacity), count(0), loadFactor(loadFactor), hashFunc(HashFunc()) {
        if (initialCapacity <= 0) {
            initialCapacity = 1;
        }

        // �������������� ������ Bucket (�� ��������� ��� ������)
        for (int i = 0; i < initialCapacity; ++i) {
            buckets.Append(Bucket());
        }
    }

    // ����������� �����������
    HashTable(const HashTable& other)
        : buckets(other.buckets), count(other.count), loadFactor(other.loadFactor), hashFunc(other.hashFunc) {
        // ���������� ��� ������ � �� ������� ��������� ���������� ����������� � DynamicArray
    }

    // �������� ������������ ������������
    HashTable& operator=(const HashTable& other) {
        if (this == &other) {
            return *this; // ������ �� ����������������
        }

        // �������� ��� �����
        buckets = other.buckets;
        count = other.count;
        loadFactor = other.loadFactor;
        hashFunc = other.hashFunc;

        return *this;
    }

    // ����������
    ~HashTable() {
        // DynamicArray � Pair ���� ��������� �������
    }

    // ������� ���� ����-��������
    void insert(const Key& key, const Value& value) override {
        if (buckets.GetLength() == 0) {
            throw std::runtime_error("HashTable has no buckets initialized.");
        }

        double currentLoadFactor = static_cast<double>(count + 1) / buckets.GetLength();
        if (currentLoadFactor > loadFactor) {
            std::cout << "Load factor exceeded. Rehashing..." << std::endl;
            rehash();
        }

        size_t hashValue = hashFunc(key);
        int index = static_cast<int>(hashValue % buckets.GetLength());

        // ���������, ���� �� ��� ���� � �������
        DynamicArray<Pair<Key, Value>>& chain = buckets.GetElem(index).chain;
        for (int i = 0; i < chain.GetLength(); i++) {
            if (chain.GetElem(i).key == key) { // ������������ ��������==
                // ��������� ��������
                chain.GetElem(i).value = value;
                std::cout << "Updated existing key." << std::endl;
                return;
            }
        }

        // ����� ��������� ����� ����
        chain.Append(Pair<Key, Value>(key, value));
        count++;
        std::cout << "Added new key." << std::endl;
    }

    // �������� ������������� �����
    bool exist(const Key& key) const override {
        if (buckets.GetLength() == 0) {
            return false;
        }

        size_t hashValue = hashFunc(key);
        int index = static_cast<int>(hashValue % buckets.GetLength());

        const DynamicArray<Pair<Key, Value>>& chain = buckets.GetElem(index).chain;
        for (int i = 0; i < chain.GetLength(); i++) {
            if (chain.GetElem(i).key == key) { // ������������ ��������==
                return true;
            }
        }
        return false;
    }

    // ��������� �������� �� �����
    Value get(const Key& key) const override {
        if (buckets.GetLength() == 0) {
            throw std::runtime_error("HashTable has no buckets initialized.");
        }

        size_t hashValue = hashFunc(key);
        int index = static_cast<int>(hashValue % buckets.GetLength());

        const DynamicArray<Pair<Key, Value>>& chain = buckets.GetElem(index).chain;
        for (int i = 0; i < chain.GetLength(); i++) {
            if (chain.GetElem(i).key == key) { // ������������ ��������==
                return chain.GetElem(i).value;
            }
        }
        throw std::runtime_error("Key not found in HashTable");
    }

    // �������� ���� �� �����
    bool remove(const Key& key) override {
        if (buckets.GetLength() == 0) {
            return false;
        }

        size_t hashValue = hashFunc(key);
        int index = static_cast<int>(hashValue % buckets.GetLength());

        DynamicArray<Pair<Key, Value>>& chain = buckets.GetElem(index).chain;
        for (int i = 0; i < chain.GetLength(); i++) {
            if (chain.GetElem(i).key == key) { // ������������ ��������==
                chain.RemoveAt(i);
                count--;
                std::cout << "Removed key." << std::endl;
                return true;
            }
        }
        return false;
    }

    // ��������� ���� ��� ����-��������
    void getAllPairs(DynamicArray<Pair<Key, Value>>& arr) const override {
        for (int i = 0; i < buckets.GetLength(); i++) {
            const DynamicArray<Pair<Key, Value>>& chain = buckets.GetElem(i).chain;
            for (int j = 0; j < chain.GetLength(); j++) {
                arr.Append(chain.GetElem(j));
            }
        }
    }

    // �������������� ������, �������� size(), capacity(), ����� �������� ��� �������������
};
