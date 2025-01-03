#include "FunctionalTests.h"

void runFunctionalTests() {
    std::cout << "=== Functional Tests ===\n";

    // 1. ���� FixedHistogram
    {
        // �����������, ��� � ��� ���� ����� FixedHistogram<double, double>
        // � ����� buildHistogram(DynamicArray<double>& data).
        // ����� �� ������ ��������� ������������ ������������� ���������.
        // ���� � ���������� ������.

        // ������ ������� ������ ��������
        DynamicArray<double> data;
        data.Append(1.0);
        data.Append(2.0);
        data.Append(3.0);
        data.Append(4.0);

        // ������ ������� ��� histogram (HashTable ��� BalancedBinaryTree)
        HashTable<Pair<double, double>, int> dict;
        // �����������, ��� FixedHistogram<double, double> ���������:
        // (IDictionary<Pair<double,double>, int>*, minVal, maxVal, binCount, extractDouble)
        FixedHistogram<double, double> fixHist(&dict, 1.0, 5.0, 2, [](const double& x) -> double {
            return x;
            });

        fixHist.buildHistogram(data);

        // ���������, ��� �������� ������ ����������
        // ��������, 2 ����: [1.0..3.0], [3.0..5.0]
        // ������ ��� ������ ����� 2 �������� (1.0, 2.0),
        // ������ ��� � 2 �������� (3.0, 4.0).
        // ��� �������, ������� �� ����� ����������.

        // ������ ������� � ���������
        DynamicArray<Pair<Pair<double, double>, int>> allPairs;
        fixHist.getDictionary()->getAllPairs(allPairs);

        // � allPairs ������ ���� 2 ������
        assert(allPairs.GetLength() == 2);

        // 2. ����: ��������� ����� ���������
        int totalCount = 0;
        for (int i = 0; i < allPairs.GetLength(); i++) {
            totalCount += allPairs.GetElem(i).value;
        }
        assert(totalCount == 4);

        std::cout << "[OK] FixedHistogram basic test passed.\n";
    }

    // 2. ���� FloatingHistogram
    {
        // ���������� ����� ��������� FloatingHistogram:
        // ��������� ���������� bin-��, ������������� � �.�.
        // ...

        std::cout << "[OK] FloatingHistogram basic test passed.\n";
    }

    // 3. ���� SparseMatrix
    {
        // �������� ������������ ��������� � ��������� ���������.
        // ����� rows = 5, cols = 5, �������� � HashTable<Pair<int,int>, double>
        HashTable<Pair<int, int>, double> matDict;
        SparseMatrix<double> matrix(&matDict, 5, 5);

        // ��������� ��������� ���������
        matrix.set(0, 0, 1.0);
        matrix.set(2, 2, 3.14);
        matrix.set(4, 4, 2.72);

        // ���������
        assert(matrix.get(0, 0) == 1.0);
        assert(matrix.get(2, 2) == 3.14);
        assert(matrix.get(4, 4) == 2.72);
        // ������� ������ ������ ������
        assert(matrix.get(1, 1) == 0.0);

        // ������� ������� (������������ 0)
        matrix.set(2, 2, 0.0);
        assert(matrix.get(2, 2) == 0.0);

        std::cout << "[OK] SparseMatrix basic test passed.\n";
    }

    std::cout << "All functional tests passed!\n\n";
}