// Pair.h
#pragma once

template <typename Key, typename Value>
struct Pair {
    Key key;
    Value value;

    Pair() : key(Key()), value(Value()) {}
    Pair(const Key& k, const Value& v) : key(k), value(v) {}

    // Оператор равенства
    bool operator==(const Pair<Key, Value>& other) const {
        return (key == other.key) && (value == other.value);
    }

    // Оператор неравенства
    bool operator!=(const Pair<Key, Value>& other) const {
        return !(*this == other);
    }

    // Оператор меньше
    bool operator<(const Pair<Key, Value>& other) const {
        if (key < other.key) {
            return true;
        }
        else if (key == other.key) {
            return value < other.value;
        }
        return false;
    }

    // Оператор больше
    bool operator>(const Pair<Key, Value>& other) const {
        return other < *this;
    }

    // Оператор меньше или равно
    bool operator<=(const Pair<Key, Value>& other) const {
        return !(other < *this);
    }

    // Оператор больше или равно
    bool operator>=(const Pair<Key, Value>& other) const {
        return !(*this < other);
    }
};