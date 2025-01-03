// HashTable.h
#pragma once

#include "IDictionary.h"
#include "Pair.h"
#include "DefaultHash.h" // ��������������, ��� � ��� ���� ��������� ���-�������
#include <stdexcept>
#include <functional>
#include <iostream>

// ������� ����� ���-�������
enum class EntryStatus {
    EMPTY,
    OCCUPIED,
    DELETED
};

// ��������� ��� �������� ���� ����-�������� � �������
template <typename Key, typename Value>
struct HashEntry {
    Pair<Key, Value> pair;
    EntryStatus status;

    HashEntry() : pair(), status(EntryStatus::EMPTY) {}
    HashEntry(const Key& key, const Value& value) : pair(key, value), status(EntryStatus::OCCUPIED) {}
};

// ���-������� � �������� ���������� � ������� ������������
template <typename Key, typename Value, typename HashFunc = DefaultHash<Key>>
class HashTable : public IDictionary<Key, Value> {
private:
    HashEntry<Key, Value>* table; // ����� �������
    int capacity;                 // ������� ����������� �������
    int count;                    // ���������� ������� ���������
    double loadFactor;            // ����� ������������ ������� ��� �������������
    HashFunc hashFunc;            // ���-�������
    int R;                        // ������� ����� ��� ������ ���-�������

    // ������ ���-������� ��� �������� �����������
    size_t secondHash(const Key& key) const {
        if (capacity <= 1) return 1;
        return 1 + (hashFunc(key) % (capacity - 1));
    }

    // ������� ��� ���������� ����������� �������� ����� ������ n
    int previousPrime(int n) const {
        while (n > 1) {
            bool isPrime = true;
            for (int i = 2; i * i <= n; ++i) {
                if (n % i == 0) {
                    isPrime = false;
                    break;
                }
            }
            if (isPrime) return n;
            n--;
        }
        return 7; // ����������� �������� ������� �����
    }

    // ������������� ������� � ��������� ������������
    void rehash() {
        std::cout << "Rehashing: Old capacity = " << capacity << ", Old count = " << count << "\n";
        int oldCapacity = capacity;
        HashEntry<Key, Value>* oldTable = table;

        // ����������� ����������� � ��� ����
        capacity = oldCapacity * 2;
        if (capacity <= 0) {
            capacity = 1;
        }

        // ��������� R ��� ������� ����� ������ ����� �����������
        R = previousPrime(capacity / 2);

        // ������ ����� �������
        table = new HashEntry<Key, Value>[capacity];
        for (int i = 0; i < capacity; ++i) {
            table[i].status = EntryStatus::EMPTY;
        }
        count = 0; // ����� ��������

        // ��������� �������� �� ������ ������� � �����
        for (int i = 0; i < oldCapacity; i++) {
            if (oldTable[i].status == EntryStatus::OCCUPIED) {
                try {
                    insert(oldTable[i].pair.key, oldTable[i].pair.value);
                }
                catch (const std::exception& e) {
                    std::cerr << "Error during rehashing: " << e.what() << "\n";
                }
            }
        }

        // ����������� ������ ������ �������
        delete[] oldTable;
        std::cout << "Rehashing complete: New capacity = " << capacity << "\n";
    }

public:
    // �����������
    HashTable(int initialCapacity = 11, double loadFactor = 0.75)
        : capacity(initialCapacity), count(0), loadFactor(loadFactor), hashFunc(HashFunc()) {
        if (capacity <= 0) {
            capacity = 1;
        }
        table = new HashEntry<Key, Value>[capacity];
        for (int i = 0; i < capacity; ++i) {
            table[i].status = EntryStatus::EMPTY;
        }
        R = previousPrime(capacity / 2);
    }

    // ����������� �����������
    HashTable(const HashTable& other)
        : capacity(other.capacity), count(other.count), loadFactor(other.loadFactor), hashFunc(other.hashFunc), R(other.R) {
        table = new HashEntry<Key, Value>[capacity];
        for (int i = 0; i < capacity; i++) {
            table[i] = other.table[i];
        }
    }

    // �������� ������������ ������������
    HashTable& operator=(const HashTable& other) {
        if (this == &other) {
            return *this; // ������ �� ����������������
        }

        // ����������� ������� �������
        delete[] table;

        // �������� ������ �� ������ �������
        capacity = other.capacity;
        count = other.count;
        loadFactor = other.loadFactor;
        hashFunc = other.hashFunc;
        R = other.R;

        table = new HashEntry<Key, Value>[capacity];
        for (int i = 0; i < capacity; i++) {
            table[i] = other.table[i];
        }

        return *this;
    }

    // ����������
    ~HashTable() {
        delete[] table;
    }

    // ������� ���� ����-��������
    void insert(const Key& key, const Value& value) override {
        // ���������, �� �������� �� ����������� ������
        if ((double)(count + 1) / capacity > loadFactor) {
            std::cout << "Load factor exceeded. Initiating rehash.\n";
            rehash();
        }

        size_t hash1 = hashFunc(key) % capacity;
        size_t hash2 = secondHash(key);

        int firstDeletedIndex = -1;

        for (int i = 0; i < capacity; i++) {
            size_t index = (hash1 + i * hash2) % capacity;

            if (table[index].status == EntryStatus::EMPTY) {
                // ���� ������ ����� �������� ������, ���������� �
                if (firstDeletedIndex != -1) {
                    table[firstDeletedIndex].pair = Pair<Key, Value>(key, value);
                    table[firstDeletedIndex].status = EntryStatus::OCCUPIED;
                }
                else {
                    table[index] = HashEntry<Key, Value>(key, value);
                }
                count++;
                return;
            }
            else if (table[index].status == EntryStatus::DELETED) {
                if (firstDeletedIndex == -1) {
                    firstDeletedIndex = (int)index;
                }
            }
            else if (table[index].status == EntryStatus::OCCUPIED && table[index].pair.key == key) {
                // ��������� ��������, ���� ���� ��� ����������
                table[index].pair.value = value;
                return;
            }
        }

        // ���� ����� �������� ������ ����� � �� ��������
        if (firstDeletedIndex != -1) {
            table[firstDeletedIndex].pair = Pair<Key, Value>(key, value);
            table[firstDeletedIndex].status = EntryStatus::OCCUPIED;
            count++;
            return;
        }

        // ���� ������� ��������� ���������
        throw std::runtime_error("HashTable is full, cannot insert new key.");
    }

    // �������� ������������� �����
    bool exist(const Key& key) const override {
        size_t hash1 = hashFunc(key) % capacity;
        size_t hash2 = secondHash(key);

        for (int i = 0; i < capacity; i++) {
            size_t index = (hash1 + i * hash2) % capacity;
            if (table[index].status == EntryStatus::EMPTY) {
                // ����� ����� ���
                return false;
            }
            else if (table[index].status == EntryStatus::OCCUPIED && table[index].pair.key == key) {
                return true;
            }
            // ���� ������ DELETED ��� OCCUPIED � ������ ������, ���������� ������
        }
        return false;
    }

    // ��������� �������� �� �����
    Value get(const Key& key) const override {
        size_t hash1 = hashFunc(key) % capacity;
        size_t hash2 = secondHash(key);

        for (int i = 0; i < capacity; i++) {
            size_t index = (hash1 + i * hash2) % capacity;
            if (table[index].status == EntryStatus::EMPTY) {
                throw std::runtime_error("Key not found in HashTable.");
            }
            else if (table[index].status == EntryStatus::OCCUPIED && table[index].pair.key == key) {
                return table[index].pair.value;
            }
            // ���� DELETED, ����������
        }
        throw std::runtime_error("Key not found in HashTable.");
    }

    // �������� ���� �� �����
    bool remove(const Key& key) override {
        size_t hash1 = hashFunc(key) % capacity;
        size_t hash2 = secondHash(key);

        for (int i = 0; i < capacity; i++) {
            size_t index = (hash1 + i * hash2) % capacity;
            if (table[index].status == EntryStatus::EMPTY) {
                return false; // ���� �� ������
            }
            else if (table[index].status == EntryStatus::OCCUPIED && table[index].pair.key == key) {
                table[index].status = EntryStatus::DELETED;
                count--;
                return true;
            }
            // ���� DELETED, ���������� ������
        }
        return false; // ���� �� ������
    }

    // ��������� ���� ��� ����-��������
    void getAllPairs(DynamicArray<Pair<Key, Value>>& arr) const override {
        for (int i = 0; i < capacity; i++) {
            if (table[i].status == EntryStatus::OCCUPIED) {
                arr.Append(table[i].pair);
            }
        }
    }

    // ���������� ���������
    int size() const {
        return count;
    }

    // ����������� �������
    int getCapacity() const {
        return capacity;
    }

    // ��� ������������: ����� ���� ���������
    void display() const {
        std::cout << "HashTable Contents:\n";
        for (int i = 0; i < capacity; ++i) {
            if (table[i].status == EntryStatus::OCCUPIED) {
                std::cout << "Index " << i << ": Key = (" << table[i].pair.key.key << ", " << table[i].pair.key.value
                    << "), Value = " << table[i].pair.value << "\n";
            }
        }
    }
};
