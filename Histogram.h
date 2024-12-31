#pragma once
#include "IDictionary.h"
#include "DynamicArray.h"
#include "Pair.h"
#include <stdexcept>

// ��� �������, ������� �� ������� T (��������, Person) ���������� double:
template <typename T>
using ExtractDoubleFunc = double(*)(const T&);

/////////////////////////////////////////////////////////
// FixedHistogram
/////////////////////////////////////////////////////////
template <typename T, typename KeyType = double>
class FixedHistogram {
private:
    // ����� ������� ��������� � IDictionary<KeyType, int>.
    // KeyType ����� ����� ����, ��������, ���� (minRange, maxRange) ��� ������ double,
    // �� ������� ������� ������ ��������� ��� ���� (������_�������, �������_�������).
    IDictionary<Pair<KeyType, KeyType>, int>* dictionary;

    KeyType minVal;
    KeyType maxVal;
    int numBins; // ����� �������� (����������)

    // �������, ����������� �������� �������� �� T
    ExtractDoubleFunc<T> extractor;

public:
    // ����������� ���������:
    // 1) ������� (BalanceBinaryTree ��� HashTable)
    // 2) ����������� ��������, ������������ ��������
    // 3) ����� ����������
    // 4) ������� ���������� double �� T (���� T - ������� double, ����� �������� ������ identity)
    FixedHistogram(IDictionary<Pair<KeyType, KeyType>, int>* dict,
        KeyType minVal, KeyType maxVal, int numBins,
        ExtractDoubleFunc<T> extractorFunc)
        : dictionary(dict), minVal(minVal), maxVal(maxVal), numBins(numBins), extractor(extractorFunc) {}

    void buildHistogram(const DynamicArray<T>& data) {
        // ������� ������� ������� (��� �������) � � ����������� �� ����������
        // �� ������� ����� ���� �����, ����� �� ��� ����.

        // ���
        double range = (maxVal - minVal) / numBins;

        // ���������� ��� ��������� � �������, ������������� �������� 0
        for (int i = 0; i < numBins; i++) {
            Pair<KeyType, KeyType> bin;
            bin.key = minVal + i * range;        // ������ �������
            bin.value = (i == numBins - 1)
                ? maxVal   // ��������� ��� �� maxVal ������������
                : (minVal + (i + 1) * range);
            dictionary->insert(bin, 0);
        }

        // ������������ ��������
        for (int i = 0; i < data.GetLength(); i++) {
            double val = extractor(data.GetElem(i));
            // ����, � ����� �������� �������� val
            // ��������� ���������� �� �����
            // (���� ��� � [low, high), ����� ����������, ����� ��������������)
            for (int b = 0; b < numBins; b++) {
                Pair<KeyType, KeyType> bin;
                bin.key = minVal + b * range;
                bin.value = (b == numBins - 1)
                    ? maxVal
                    : (minVal + (b + 1) * range);

                if (val >= bin.key && val <= bin.value) {
                    int currentCount = dictionary->get(bin);
                    dictionary->insert(bin, currentCount + 1);
                    break;
                }
            }
        }
    }

    // Getter ������� (����� ����� ���� ������� ���������)
    IDictionary<Pair<KeyType, KeyType>, int>* getDictionary() const {
        return dictionary;
    }
};


/////////////////////////////////////////////////////////
// FloatingHistogram
/////////////////////////////////////////////////////////
template <typename T, typename KeyType = double>
class FloatingHistogram {
private:
    IDictionary<Pair<KeyType, KeyType>, int>* dictionary;
    int numBins;
    ExtractDoubleFunc<T> extractor;

public:
    FloatingHistogram(IDictionary<Pair<KeyType, KeyType>, int>* dict,
        int numBins,
        ExtractDoubleFunc<T> extractorFunc)
        : dictionary(dict), numBins(numBins), extractor(extractorFunc) {}

    void buildHistogram(const DynamicArray<T>& data) {
        if (numBins <= 0) {
            throw std::runtime_error("Number of bins must be positive");
        }
        if (data.GetLength() == 0) {
            return;
        }

        // ��������� ����� ������ �� �������� extractor, ����� ���������� ����
        // �������������� ���������� �� ����� ���������
        DynamicArray<T> sortedData(data);
        // ���������� ������ ��� ����� ������ ����� ����������
        for (int i = 0; i < sortedData.GetLength(); i++) {
            for (int j = 0; j < sortedData.GetLength() - i - 1; j++) {
                if (extractor(sortedData.GetElem(j)) > extractor(sortedData.GetElem(j + 1))) {
                    sortedData.Swap(sortedData.GetElem(j), sortedData.GetElem(j + 1));
                }
            }
        }

        // ������� ��������� ������ ������� ���� ��� (� ������)
        int binSize = data.GetLength() / numBins;
        int remainder = data.GetLength() % numBins;

        int startIndex = 0;
        for (int b = 0; b < numBins; b++) {
            int currentBinCount = binSize;
            if (b == numBins - 1) {
                // ������� ������� � ���������
                currentBinCount += remainder;
            }
            if (currentBinCount == 0) {
                // ���� binSize < 1 � �.�. � ������
                continue;
            }

            // ������ �������
            double lowVal = extractor(sortedData.GetElem(startIndex));
            // ������� �������
            double highVal = extractor(sortedData.GetElem(startIndex + currentBinCount - 1));

            // ��������� �������� [lowVal, highVal]
            Pair<KeyType, KeyType> bin(lowVal, highVal);
            dictionary->insert(bin, currentBinCount);

            startIndex += currentBinCount;
        }
    }

    IDictionary<Pair<KeyType, KeyType>, int>* getDictionary() const {
        return dictionary;
    }
};

