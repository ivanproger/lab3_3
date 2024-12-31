#pragma once
#include <string>

class Person {
private:
    int ID;
    std::string LastName;
    std::string FirstName;
    int BirthYear;
    double Height;
    double Weight;

public:
    Person()
        : ID(0), LastName(""), FirstName(""), BirthYear(0), Height(0.0), Weight(0.0) {}

    Person(int id, const std::string& lastName, const std::string& firstName,
        int birthYear, double height, double weight)
        : ID(id), LastName(lastName), FirstName(firstName),
        BirthYear(birthYear), Height(height), Weight(weight) {}

    double getHeight() const { return Height; }
    double getWeight() const { return Weight; }
    int getBirthYear() const { return BirthYear; }
    // и т.д.

    // Могут быть операторы сравнения (если нужно), но здесь не обязательно
};
