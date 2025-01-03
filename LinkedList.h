// LinkedList.h
#pragma once
#include "Sequence.h"
#include "DynamicArray.h"
// Убедитесь, что Range определён

template <class T>
class LinkedList : public Sequence<T>
{
private:
    struct Node
    {
        T data;
        Node* next;

        Node(T data, Node* next = nullptr) : data(data), next(next) {}
    };

    Node* head;
    Node* tail;
    int length;

public:

    class LinkedListIterator : public Sequence<T>::Iterator {
    private:
        Node* current;

    public:
        LinkedListIterator(Node* start) : current(start) { }

        bool operator==(const typename Sequence<T>::Iterator& other) const override
        {
            const LinkedListIterator* otherIterator = dynamic_cast<const LinkedListIterator*>(&other);
            return otherIterator && current == otherIterator->current;
        }

        bool operator!=(const typename Sequence<T>::Iterator& other) const override
        {
            return !(*this == other);
        }

        T& operator*() override
        {
            return current->data;
        }

        typename Sequence<T>::Iterator& operator++() override
        {
            if (current)
            {
                current = current->next;
            }

            return *this;
        }
    };

    typename Sequence<T>::Iterator* ToBegin() override
    {
        return new LinkedListIterator(head);
    }

    typename Sequence<T>::Iterator* ToEnd() override
    {
        return new LinkedListIterator(nullptr);
    }

    LinkedList() : head(nullptr), tail(nullptr), length(0) {}

    LinkedList(T* items, int count) : head(nullptr), tail(nullptr), length(0)
    {
        for (int i = 0; i < count; ++i)
        {
            Append(items[i]);
        }
    }

    LinkedList(const LinkedList<T>& list) : head(nullptr), tail(nullptr), length(0)
    {
        Node* current = list.head;

        while (current != nullptr)
        {
            Append(current->data);
            current = current->next;
        }
    }

    LinkedList(DynamicArray<T>& dynamicArray) : head(nullptr), tail(nullptr), length(0)
    {
        for (int i = 0; i < dynamicArray.GetLength(); i++) // Исправлено: начинается с 0
        {
            Append(dynamicArray.GetElem(i));
        }
    }

    ~LinkedList()
    {
        Node* current = head;
        Node* next;

        while (current != nullptr)
        {
            next = current->next;
            delete current;
            current = next;
        }
    }

    // Константная версия GetElem
    const T& GetElem(int index) const override
    {
        return GetConstNode(index)->data;
    }

    // Неконстантная версия GetElem
    T& GetElem(int index) override
    {
        return GetNode(index)->data;
    }

    // Константная версия GetLength
    int GetLength() const override
    {
        return length;
    }

    // Неконстантная версия GetLength
    int GetLength() override
    {
        return length;
    }

    // Получение первого элемента
    T& GetFirstElem() override
    {
        if (head == nullptr) {
            throw std::out_of_range("List is empty");
        }
        return head->data;
    }

    // Получение последнего элемента
    T& GetLastElem() override
    {
        if (tail == nullptr) {
            throw std::out_of_range("List is empty");
        }
        return tail->data;
    }

    // Получение узла по индексу (неконстантный)
    Node* GetNode(int index)
    {
        if (index < 0 || index >= length) {
            throw std::out_of_range("Index out of range");
        }

        Node* current = head;

        for (int i = 0; i < index; i++)
        {
            current = current->next;
        }

        return current;
    }

    // Получение узла по индексу (константный)
    const Node* GetConstNode(int index) const
    {
        if (index < 0 || index >= length) {
            throw std::out_of_range("Index out of range");
        }

        Node* current = head;

        for (int i = 0; i < index; i++)
        {
            current = current->next;
        }

        return current;
    }

    void Swap(T& a, T& b) override
    {
        T temp = a;
        a = b;
        b = temp;
    }

    void Set(int index, T value) override
    {
        GetNode(index)->data = value;
    }

    LinkedList<T>* GetSubsequence(int startIndex, int endIndex) override
    {
        if (startIndex < 0 || endIndex >= length || startIndex > endIndex)
        {
            throw std::out_of_range("Invalid start or end index");
        }

        LinkedList<T>* sublist = new LinkedList<T>();
        Node* current = head;

        for (int i = 0; i <= endIndex; i++)
        {
            if (i >= startIndex)
            {
                sublist->Append(current->data);
            }

            current = current->next;
        }

        return sublist;
    }

    void Append(T item) override
    {
        Node* newNode = new Node(item);

        if (length == 0)
        {
            head = tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }

        length++;
    }

    void Prepend(T item) override
    {
        Node* newNode = new Node(item, head);

        if (length == 0)
        {
            head = tail = newNode;
        }
        else
        {
            head = newNode;
        }

        length++;
    }

    void Insert(T item, int index) override
    {
        if (index < 0 || index > length) {
            throw std::out_of_range("Index out of range");
        }

        if (index == 0)
        {
            Prepend(item);
        }
        else if (index == length)
        {
            Append(item);
        }
        else
        {
            Node* current = GetNode(index - 1);

            Node* newNode = new Node(item, current->next);
            current->next = newNode;
            length++;
        }
    }

    void Union(Sequence<T>* list) override
    {
        int listLength = list->GetLength();

        for (int i = 0; i < listLength; i++)
        {
            Append(list->GetElem(i));
        }
    }

    void Print() const {
        Node* current = head;
        std::cout << "Elements in the list: ";
        while (current) {
            std::cout << current->data << " "; // Убедитесь, что тип T поддерживает оператор <<
            current = current->next;
        }
        std::cout << std::endl;
    }

    void Clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = tail = nullptr;
        length = 0;
    }
};
