// HashTable.h
#pragma once

#include "IDictionary.h"
#include "DynamicArray.h"
#include "Pair.h"
#include "DefaultHash.h" // Подключаем DefaultHash
#include <stdexcept>
#include <functional>
#include <iostream> // Для отладочных выводов

// Хеш-таблица с цепочками
template <typename Key, typename Value, typename HashFunc = DefaultHash<Key>>
class HashTable : public IDictionary<Key, Value> {
private:
    struct Bucket {
        DynamicArray<Pair<Key, Value>> chain; // Цепочка пар
    };

    DynamicArray<Bucket> buckets;
    int count;             // Количество реально элементов
    double loadFactor;     // Порог загрузочного фактора для рехеша
    HashFunc hashFunc;

    void rehash() {
        int oldCapacity = buckets.GetLength();
        DynamicArray<Bucket> oldBuckets = buckets;

        int newCapacity = oldCapacity * 2;
        if (newCapacity <= 0) {
            newCapacity = 1;
        }

        // Инициализируем новые бакеты
        buckets = DynamicArray<Bucket>(newCapacity);
        count = 0; // Мы заново будем вставлять все элементы

        for (int i = 0; i < newCapacity; ++i) {
            buckets.Append(Bucket());
        }

        // Переносим элементы из старых бакетов в новые
        for (int i = 0; i < oldCapacity; i++) {
            Bucket& oldBucket = oldBuckets.GetElem(i);
            for (int j = 0; j < oldBucket.chain.GetLength(); j++) {
                Pair<Key, Value>& p = oldBucket.chain.GetElem(j);
                insert(p.key, p.value);
            }
        }
    }

public:
    // Конструктор
    HashTable(int initialCapacity = 16, double loadFactor = 0.75)
        : buckets(initialCapacity), count(0), loadFactor(loadFactor), hashFunc(HashFunc()) {
        if (initialCapacity <= 0) {
            initialCapacity = 1;
        }

        // Инициализируем каждый Bucket (по умолчанию они пустые)
        for (int i = 0; i < initialCapacity; ++i) {
            buckets.Append(Bucket());
        }
    }

    // Конструктор копирования
    HashTable(const HashTable& other)
        : buckets(other.buckets), count(other.count), loadFactor(other.loadFactor), hashFunc(other.hashFunc) {
        // Копируются все бакеты и их цепочки благодаря реализации копирования в DynamicArray
    }

    // Оператор присваивания копированием
    HashTable& operator=(const HashTable& other) {
        if (this == &other) {
            return *this; // Защита от самоприсваивания
        }

        // Копируем все члены
        buckets = other.buckets;
        count = other.count;
        loadFactor = other.loadFactor;
        hashFunc = other.hashFunc;

        return *this;
    }

    // Деструктор
    ~HashTable() {
        // DynamicArray и Pair сами управляют памятью
    }

    // Вставка пары ключ-значение
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

        // Проверяем, есть ли уже ключ в цепочке
        DynamicArray<Pair<Key, Value>>& chain = buckets.GetElem(index).chain;
        for (int i = 0; i < chain.GetLength(); i++) {
            if (chain.GetElem(i).key == key) { // Используется оператор==
                // Обновляем значение
                chain.GetElem(i).value = value;
                std::cout << "Updated existing key." << std::endl;
                return;
            }
        }

        // Иначе добавляем новую пару
        chain.Append(Pair<Key, Value>(key, value));
        count++;
        std::cout << "Added new key." << std::endl;
    }

    // Проверка существования ключа
    bool exist(const Key& key) const override {
        if (buckets.GetLength() == 0) {
            return false;
        }

        size_t hashValue = hashFunc(key);
        int index = static_cast<int>(hashValue % buckets.GetLength());

        const DynamicArray<Pair<Key, Value>>& chain = buckets.GetElem(index).chain;
        for (int i = 0; i < chain.GetLength(); i++) {
            if (chain.GetElem(i).key == key) { // Используется оператор==
                return true;
            }
        }
        return false;
    }

    // Получение значения по ключу
    Value get(const Key& key) const override {
        if (buckets.GetLength() == 0) {
            throw std::runtime_error("HashTable has no buckets initialized.");
        }

        size_t hashValue = hashFunc(key);
        int index = static_cast<int>(hashValue % buckets.GetLength());

        const DynamicArray<Pair<Key, Value>>& chain = buckets.GetElem(index).chain;
        for (int i = 0; i < chain.GetLength(); i++) {
            if (chain.GetElem(i).key == key) { // Используется оператор==
                return chain.GetElem(i).value;
            }
        }
        throw std::runtime_error("Key not found in HashTable");
    }

    // Удаление пары по ключу
    bool remove(const Key& key) override {
        if (buckets.GetLength() == 0) {
            return false;
        }

        size_t hashValue = hashFunc(key);
        int index = static_cast<int>(hashValue % buckets.GetLength());

        DynamicArray<Pair<Key, Value>>& chain = buckets.GetElem(index).chain;
        for (int i = 0; i < chain.GetLength(); i++) {
            if (chain.GetElem(i).key == key) { // Используется оператор==
                chain.RemoveAt(i);
                count--;
                std::cout << "Removed key." << std::endl;
                return true;
            }
        }
        return false;
    }

    // Получение всех пар ключ-значение
    void getAllPairs(DynamicArray<Pair<Key, Value>>& arr) const override {
        for (int i = 0; i < buckets.GetLength(); i++) {
            const DynamicArray<Pair<Key, Value>>& chain = buckets.GetElem(i).chain;
            for (int j = 0; j < chain.GetLength(); j++) {
                arr.Append(chain.GetElem(j));
            }
        }
    }

    // Дополнительные методы, например size(), capacity(), можно добавить при необходимости
};
