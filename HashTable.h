// HashTable.h
#pragma once

#include "IDictionary.h"
#include "Pair.h"
#include "DefaultHash.h" // Предполагается, что у вас есть дефолтная хеш-функция
#include <stdexcept>
#include <functional>
#include <iostream>

// Статусы ячеек хеш-таблицы
enum class EntryStatus {
    EMPTY,
    OCCUPIED,
    DELETED
};

// Структура для хранения пары ключ-значение и статуса
template <typename Key, typename Value>
struct HashEntry {
    Pair<Key, Value> pair;
    EntryStatus status;

    HashEntry() : pair(), status(EntryStatus::EMPTY) {}
    HashEntry(const Key& key, const Value& value) : pair(key, value), status(EntryStatus::OCCUPIED) {}
};

// Хеш-таблица с открытой адресацией и двойным хешированием
template <typename Key, typename Value, typename HashFunc = DefaultHash<Key>>
class HashTable : public IDictionary<Key, Value> {
private:
    HashEntry<Key, Value>* table; // Сырые массивы
    int capacity;                 // Текущая вместимость таблицы
    int count;                    // Количество занятых элементов
    double loadFactor;            // Порог загрузочного фактора для рехеширования
    HashFunc hashFunc;            // Хеш-функция
    int R;                        // Простое число для второй хеш-функции

    // Вторая хеш-функция для двойного хеширования
    size_t secondHash(const Key& key) const {
        if (capacity <= 1) return 1;
        return 1 + (hashFunc(key) % (capacity - 1));
    }

    // Функция для нахождения предыдущего простого числа меньше n
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
        return 7; // Минимальное разумное простое число
    }

    // Рехеширование таблицы с удвоенной вместимостью
    void rehash() {
        std::cout << "Rehashing: Old capacity = " << capacity << ", Old count = " << count << "\n";
        int oldCapacity = capacity;
        HashEntry<Key, Value>* oldTable = table;

        // Увеличиваем вместимость в два раза
        capacity = oldCapacity * 2;
        if (capacity <= 0) {
            capacity = 1;
        }

        // Обновляем R как простое число меньше новой вместимости
        R = previousPrime(capacity / 2);

        // Создаём новую таблицу
        table = new HashEntry<Key, Value>[capacity];
        for (int i = 0; i < capacity; ++i) {
            table[i].status = EntryStatus::EMPTY;
        }
        count = 0; // Сброс счётчика

        // Переносим элементы из старой таблицы в новую
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

        // Освобождаем память старой таблицы
        delete[] oldTable;
        std::cout << "Rehashing complete: New capacity = " << capacity << "\n";
    }

public:
    // Конструктор
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

    // Конструктор копирования
    HashTable(const HashTable& other)
        : capacity(other.capacity), count(other.count), loadFactor(other.loadFactor), hashFunc(other.hashFunc), R(other.R) {
        table = new HashEntry<Key, Value>[capacity];
        for (int i = 0; i < capacity; i++) {
            table[i] = other.table[i];
        }
    }

    // Оператор присваивания копированием
    HashTable& operator=(const HashTable& other) {
        if (this == &other) {
            return *this; // Защита от самоприсваивания
        }

        // Освобождаем текущую таблицу
        delete[] table;

        // Копируем данные из другой таблицы
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

    // Деструктор
    ~HashTable() {
        delete[] table;
    }

    // Вставка пары ключ-значение
    void insert(const Key& key, const Value& value) override {
        // Проверяем, не превышен ли загрузочный фактор
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
                // Если раньше нашли удалённую ячейку, используем её
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
                // Обновляем значение, если ключ уже существует
                table[index].pair.value = value;
                return;
            }
        }

        // Если нашли удалённую ячейку ранее и не вставили
        if (firstDeletedIndex != -1) {
            table[firstDeletedIndex].pair = Pair<Key, Value>(key, value);
            table[firstDeletedIndex].status = EntryStatus::OCCUPIED;
            count++;
            return;
        }

        // Если таблица полностью заполнена
        throw std::runtime_error("HashTable is full, cannot insert new key.");
    }

    // Проверка существования ключа
    bool exist(const Key& key) const override {
        size_t hash1 = hashFunc(key) % capacity;
        size_t hash2 = secondHash(key);

        for (int i = 0; i < capacity; i++) {
            size_t index = (hash1 + i * hash2) % capacity;
            if (table[index].status == EntryStatus::EMPTY) {
                // Ключа точно нет
                return false;
            }
            else if (table[index].status == EntryStatus::OCCUPIED && table[index].pair.key == key) {
                return true;
            }
            // Если ячейка DELETED или OCCUPIED с другим ключом, продолжаем искать
        }
        return false;
    }

    // Получение значения по ключу
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
            // Если DELETED, продолжаем
        }
        throw std::runtime_error("Key not found in HashTable.");
    }

    // Удаление пары по ключу
    bool remove(const Key& key) override {
        size_t hash1 = hashFunc(key) % capacity;
        size_t hash2 = secondHash(key);

        for (int i = 0; i < capacity; i++) {
            size_t index = (hash1 + i * hash2) % capacity;
            if (table[index].status == EntryStatus::EMPTY) {
                return false; // Ключ не найден
            }
            else if (table[index].status == EntryStatus::OCCUPIED && table[index].pair.key == key) {
                table[index].status = EntryStatus::DELETED;
                count--;
                return true;
            }
            // Если DELETED, продолжаем искать
        }
        return false; // Ключ не найден
    }

    // Получение всех пар ключ-значение
    void getAllPairs(DynamicArray<Pair<Key, Value>>& arr) const override {
        for (int i = 0; i < capacity; i++) {
            if (table[i].status == EntryStatus::OCCUPIED) {
                arr.Append(table[i].pair);
            }
        }
    }

    // Количество элементов
    int size() const {
        return count;
    }

    // Вместимость таблицы
    int getCapacity() const {
        return capacity;
    }

    // Для демонстрации: вывод всех элементов
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
