#include "Interface.h"

FixedHistogram<double, double>* currentFixedHistogram = nullptr;
FloatingHistogram<double, double>* currentFloatingHistogram = nullptr;
SparseMatrix<double>* currentSparseMatrix = nullptr;

void createHistogram()
{
    while (true) {
        std::cout << "\nCreate Histogram:\n";
        std::cout << "1. Fixed Histogram\n";
        std::cout << "2. Floating Histogram\n";
        std::cout << "3. Back to Main Menu\n";
        std::cout << "Select: ";

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please try again.\n";
            continue;
        }

        if (choice == 1) { 
            double minVal, maxVal;
            int rangeCount;
            std::cout << "Enter minVal, maxVal, rangeCount: ";
            std::cin >> minVal >> maxVal >> rangeCount;
            if (std::cin.fail() || minVal >= maxVal || rangeCount <= 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid parameters. Please try again.\n";
                continue;
            }

            //(можно использовать HashTable или BalanceBinaryTree)
            HashTable<Pair<double, double>, int>* dict = new HashTable<Pair<double, double>, int>(rangeCount * 2, 0.75);

         
            currentFixedHistogram = new FixedHistogram<double, double>(dict, minVal, maxVal, rangeCount, [](const double& x) -> double {
                return x;
                });

            std::cout << "Fixed Histogram created successfully.\n";
            manageHistogram(dict, true);
            break;
        }
        else if (choice == 2) { 
            int elementsPerBin;
            std::cout << "Enter number of elements per bin: ";
            std::cin >> elementsPerBin;
            if (std::cin.fail() || elementsPerBin <= 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid elements per bin. Please try again.\n";
                continue;
            }

            HashTable<Pair<double, double>, int>* dict = new HashTable<Pair<double, double>, int>(elementsPerBin * 2, 0.75);

           
            currentFloatingHistogram = new FloatingHistogram<double, double>(dict, elementsPerBin, [](const double& x) -> double {
                return x;
                });

            std::cout << "Floating Histogram created successfully.\n";
            manageHistogram(dict, false);
            break;
        }
        else if (choice == 3) { 
            break;
        }
        else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

void manageHistogram(IDictionary<Pair<double, double>, int>* dict, bool isFixed) {
    while (true) {
        std::cout << "\nManage " << (isFixed ? "Fixed" : "Floating") << " Histogram:\n";
        std::cout << "1. Add Data\n";
        std::cout << "2. Remove Data\n";
        std::cout << "3. Display Histogram\n";
        std::cout << "4. Back to Histogram Menu\n";
        std::cout << "Select: ";

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please try again.\n";
            continue;
        }

        if (choice == 1) { 
            int n;
            std::cout << "Enter number of elements to add: ";
            std::cin >> n;
            if (std::cin.fail() || n <= 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid number. Please try again.\n";
                continue;
            }

            DynamicArray<double> data;
            std::cout << "Enter " << n << " numbers:\n";
            for (int i = 0; i < n; i++) {
                double x;
                std::cin >> x;
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid input. Data entry aborted.\n";
                    data.Clear();
                    break;
                }
                data.Append(x);
            }

            if (data.GetLength() != n) {
                std::cout << "Not all data was entered correctly.\n";
                continue;
            }

            if (isFixed && currentFixedHistogram != nullptr) {
                currentFixedHistogram->buildHistogram(data);
                std::cout << "Data added to Fixed Histogram.\n";
            }
            else if (!isFixed && currentFloatingHistogram != nullptr) {
                currentFloatingHistogram->buildHistogram(data);
                std::cout << "Data added to Floating Histogram.\n";
            }
            else {
                std::cout << "Histogram not initialized correctly.\n";
            }
        }
        else if (choice == 2) { // Remove Data
            double value;
            std::cout << "Enter value to remove: ";
            std::cin >> value;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please try again.\n";
                continue;
            }
            // Для удаления по значению нужно определить, как именно удалять.
            // Например, уменьшить счётчик в соответствующем бине.
            // Здесь пример для FixedHistogram.

            if (isFixed && currentFixedHistogram != nullptr) {
                // Найдём соответствующий бин
                double minVal;
                std::cout << "Enter min val: ";
                std::cin >> minVal;
                double maxVal;
                std::cout << "Enter max val: ";
                std::cin >> maxVal;
                int binCount;
                std::cout << "Enter bin count: ";
                std::cin >> binCount;
                if (binCount <= 0) {
                    std::cout << "Invalid bin count.\n";
                    continue;
                }
                double range = (maxVal - minVal) / binCount;
                int binIndex = static_cast<int>((value - minVal) / range);
                if (binIndex < 0 || binIndex >= binCount) {
                    std::cout << "Value out of range. Cannot remove.\n";
                    continue;
                }
                Pair<double, double> bin(minVal + binIndex * range, minVal + (binIndex + 1) * range);
                if (dict->exist(bin)) {
                    int currentCount = dict->get(bin);
                    if (currentCount > 0) {
                        dict->insert(bin, currentCount - 1);
                        std::cout << "Value removed from bin [" << bin.key << ", " << bin.value << ")\n";
                    }
                    else {
                        std::cout << "Bin is already empty.\n";
                    }
                }
                else {
                    std::cout << "Bin not found.\n";
                }
            }
            else if (!isFixed && currentFloatingHistogram != nullptr) {
                // Аналогично можно реализовать для FloatingHistogram
                std::cout << "Removal for FloatingHistogram not implemented.\n";
            }
            else {
                std::cout << "Histogram not initialized correctly.\n";
            }
        }
        else if (choice == 3) { // Display Histogram
            std::cout << "Histogram Contents:\n";
            PrintDictionary(dict);
        }
        else if (choice == 4) { 
            break;
        }
        else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

void createSparseMatrix() {
    std::cout << "\nCreate Sparse Matrix:\n";
    int rows, cols;
    std::cout << "Enter number of rows and columns: ";
    std::cin >> rows >> cols;
    if (std::cin.fail() || rows <= 0 || cols <= 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid dimensions. Returning to main menu.\n";
        return;
    }

    std::cout << "Select storage type:\n";
    std::cout << "1. HashTable\n";
    std::cout << "2. BalancedBinaryTree\n";
    std::cout << "Select: ";
    int storageChoice;
    std::cin >> storageChoice;

    IDictionary<Pair<int, int>, double>* dict = nullptr;
    if (storageChoice == 1) {
        dict = new HashTable<Pair<int, int>, double>(rows * cols + 1, 0.75);
    }
    else if (storageChoice == 2) {
        dict = new BalanceBinaryTree<Pair<int, int>, double>();
    }
    else {
        std::cout << "Invalid storage type. Returning to main menu.\n";
        return;
    }

    currentSparseMatrix = new SparseMatrix<double>(dict, rows, cols);
    std::cout << "Sparse Matrix created successfully.\n";

    
    manageSparseMatrix(*currentSparseMatrix);
}

void manageSparseMatrix(SparseMatrix<double>& matrix) {
    while (true) {
        std::cout << "\nManage Sparse Matrix:\n";
        std::cout << "START WITH 0 index!!!\n";
        std::cout << "1. Add/Update Element\n";
        std::cout << "2. Remove Element\n";
        std::cout << "3. Display Matrix\n";
        std::cout << "4. Back to Main Menu\n";
        std::cout << "Select: ";

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please try again.\n";
            continue;
        }

        if (choice == 1) { // Add/Update Element
            int r, c;
            double val;
            std::cout << "Enter row, column, and value: ";
            std::cin >> r >> c >> val;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please try again.\n";
                continue;
            }
            try {
                matrix.set(r, c, val);
                std::cout << "Element set successfully.\n";
            }
            catch (const std::out_of_range& e) {
                std::cout << e.what() << "\n";
            }
        }
        else if (choice == 2) { // Remove Element
            int r, c;
            std::cout << "Enter row and column to remove: ";
            std::cin >> r >> c;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please try again.\n";
                continue;
            }
            try {
                matrix.set(r, c, 0.0); // Устанавливаем значение 0 для удаления
                std::cout << "Element removed successfully.\n";
            }
            catch (const std::out_of_range& e) {
                std::cout << e.what() << "\n";
            }
        }
        else if (choice == 3) { // Display Matrix
            std::cout << "SparseMatrix 5x5 example:" << std::endl;
            for (int r = 0; r < 5; r++) {
                for (int c = 0; c < 5; c++) {
                    std::cout << matrix.get(r, c) << " ";
                }
                std::cout << std::endl;
            }
        }
        else if (choice == 4) { // Back
            break;
        }
        else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

void runInterface() {
    while (true) {
        std::cout << "\nMenu:\n";
        std::cout << "1. Create Histogram\n";
        std::cout << "2. Create Sparse Matrix\n";
        std::cout << "3. Run Functional Tests\n";
        std::cout << "4. Run Load Tests\n";
        std::cout << "5. Exit\n";
        std::cout << "Select: ";

        int choice;
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please try again.\n";
            continue;
        }

        if (choice == 1) { // Create Histogram
            createHistogram();
        }
        else if (choice == 2) { // Create Sparse Matrix
            createSparseMatrix();
        }
        else if (choice == 3) { // Run functional tests
            runFunctionalTests();
        }
        else if (choice == 4) { // Run load tests
            runLoadTests();
        }
        else if (choice == 5) { // Exit
            std::cout << "Exiting...\n";
            break;
        }
        else {
            std::cout << "Invalid choice. Please select from the menu.\n";
        }
    }
}


