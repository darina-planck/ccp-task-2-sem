#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <ctime>
#include <limits>  // Для numeric_limits
#include <cctype>  // Для isdigit

using namespace std;

/**
 * @brief Проверяет, является ли ввод числовым и очищает буфер ввода
 * 
 * @param value Ссылка на переменную, куда будет сохранено значение
 * @param prompt Сообщение для пользователя
 * @return true если ввод числовой, false в противном случае
 */
bool getNumericInput(int& value, const string& prompt) { // проверка на ошибки
    string input;
    cout << prompt;
    
    // Если пользователь ввел не число
    if (!(cin >> value)) {
        cin.clear(); // Очищаем состояние ошибки
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Очищаем буфер
        cout << "Ошибка: Введите числовое значение." << endl;
        return false;
    }
    
    // Очищаем остаток буфера
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}

/**
 * @brief Объединяет два отсортированных подмассива в один отсортированный массив
 * 
 * Это часть алгоритма прямой сортировки слиянием. Функция принимает два соседних отсортированных
 * подмассива и объединяет их в один отсортированный массив.
 * 
 * @param arr Массив, содержащий оба подмассива для слияния
 * @param left Начальный индекс первого подмассива
 * @param mid Конечный индекс первого подмассива
 * @param right Конечный индекс второго подмассива
 */
void merge(vector<int>& arr, int left, int mid, int right) { // прямое слияние функция слияния
                                                            //функция объединяет два отсортированных подмассива в один отсортированный массив
    // Вычисляем размеры двух подмассивов для слияния
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Создаем временные массивы для хранения подмассивов
    vector<int> L(n1), R(n2);

    // Копируем данные во временные массивы L[] и R[]
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];


    int i = 0;    // Начальный индекс первого подмассива
    int j = 0;    // Начальный индекс второго подмассива
    int k = left; // Начальный индекс объединенного подмассива

    // Сравниваем элементы из обоих массивов и помещаем меньший в исходный массив
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Копируем оставшиеся элементы L[], если они есть
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Копируем оставшиеся элементы R[], если они есть
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

/**
 * @brief Реализует алгоритм прямой сортировки слиянием рекурсивно
 * 
 * Это классическая реализация сортировки слиянием (прямое слияние), которая следует
 * подходу "разделяй и властвуй":
 * 1. Делит массив на две равные половины
 * 2. Рекурсивно сортирует две половины
 * 3. Объединяет отсортированные половины
 * 
 * В отличие от естественной сортировки слиянием, которая находит и объединяет уже отсортированные
 * последовательности, прямая сортировка слиянием искусственно делит массив независимо от существующего порядка.
 * 
 * @param arr Массив для сортировки
 * @param left Начальный индекс массива или подмассива
 * @param right Конечный индекс массива или подмассива
 */
void mergeSort(vector<int>& arr, int left, int right) { 
    if (left < right) {
        // Находим среднюю точку для разделения массива на две половины
        int mid = left + (right - left) / 2;

        // Сортируем первую и вторую половины
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        // Объединяем отсортированные половины
        merge(arr, left, mid, right);
    }
}

/**
 * @brief Реализует алгоритм шейкерной (коктейльной) сортировки
 * 
 * Шейкерная сортировка - это вариация пузырьковой сортировки, которая сортирует в обоих направлениях.
 * Она работает путем:
 * 1. Перемещения наибольших элементов в конец (проход справа налево)
 * 2. Затем перемещения наименьших элементов в начало (проход слева направо)
 * 3. Повторения этих действий, пока массив не будет отсортирован
 * 
 * Этот двунаправленный подход может быть более эффективным, чем стандартная пузырьковая сортировка
 * для определенных распределений данных.
 * 
 * @param arr Массив для сортировки
 */
void shakerSort(vector<int>& arr) {
    bool swapped = true;
    int start = 0;
    int end = arr.size() - 1;

    while (swapped) {
        // Сбрасываем флаг обмена для первого прохода
        swapped = false;

        // Прямой проход (слева направо) - как при пузырьковой сортировке
        // Перемещаем наибольший элемент в конец
        for (int i = start; i < end; ++i) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                swapped = true;
            }
        }

        // Если ничего не переместилось, массив отсортирован
        if (!swapped)
            break;

        // Сбрасываем флаг обмена для обратного прохода
        swapped = false;
        
        // Перемещаем конечную точку на одну позицию назад
        // (наибольший элемент уже на своем месте)
        --end;

        // Обратный проход (справа налево)
        // Перемещаем наименьший элемент в начало
        for (int i = end - 1; i >= start; --i) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                swapped = true;
            }
        }

        // Перемещаем начальную точку на одну позицию вперед
        // (наименьший элемент уже на своем месте)
        ++start;
    }
}

/**
 * @brief Записывает массив целых чисел в файл
 * 
 * @param arr Массив для записи
 * @param filename Имя выходного файла
 * @return true если запись прошла успешно, false в противном случае
 */
bool writeToFile(const vector<int>& arr, const string& filename) { // запись в файлы
    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Ошибка при открытии файла " << filename << endl;
        return false;
    }

    for (const auto& num : arr) {
        outFile << num << " ";
    }
    outFile.close();
    return true;
}

/**
 * @brief Основная функция программы
 * 
 * Обрабатывает пользовательский ввод, генерацию массива, сортировку и вывод в файл.
 */
int main() {
    vector<int> numbers;
    char choice;
    
    // Настраиваем вывод на русском языке
    setlocale(LC_ALL, "Russian");
    
    // Создаем имена файлов с временной меткой для уникальности
    time_t now = time(0);
    string timestamp = to_string(now);
    string initialFileName = "original_data_" + timestamp + ".txt";
    string mergeSortFileName = "sorted_by_straight_merge_" + timestamp + ".txt";
    string shakerSortFileName = "sorted_by_shaker_" + timestamp + ".txt";

    bool validChoice = false;
    while (!validChoice) {
        cout << "Как вы хотите заполнить список чисел?" << endl;
        cout << "1. Ввести самостоятельно" << endl;
        cout << "2. Сгенерировать случайно" << endl;
        cout << "Ваш выбор (1/2): ";
        
        string input;
        getline(cin, input);
        
        if (input.length() == 1 && (input[0] == '1' || input[0] == '2')) {
            choice = input[0];
            validChoice = true;
        } else {
            cout << "Некорректный выбор. Пожалуйста, выберите 1 или 2." << endl;
        }
    }

    // Обрабатываем пользовательский ввод в зависимости от выбора
    if (choice == '1') {
        // Опция ручного ввода
        int n;
        bool validInput = false;
        
        while (!validInput) {
            validInput = getNumericInput(n, "Введите количество чисел: ");
            if (validInput && (n <= 0 || n > 1000000)) {
                cout << "Количество должно быть положительным и не превышать 1,000,000." << endl;
                validInput = false;
            }
        }
        
        cout << "Введите " << n << " чисел:" << endl;
        for (int i = 0; i < n; i++) {
            int num;
            bool validNum = false;
            
            while (!validNum) {
                string prompt = "Число " + to_string(i+1) + ": ";
                validNum = getNumericInput(num, prompt);
            }
            
            numbers.push_back(num);
        }


        
    } else if (choice == '2') {
        // Опция случайной генерации
        int count, min_value, max_value;
        bool validInput = false;
        
        while (!validInput) {
            validInput = getNumericInput(count, "Введите количество чисел для генерации: ");
            if (validInput && (count <= 0 || count > 1000000)) {
                cout << "Количество должно быть положительным и не превышать 1,000,000." << endl;
                validInput = false;
            }
        }
        
        validInput = false;
        while (!validInput) {
            validInput = getNumericInput(min_value, "Введите минимальное значение: ");
        }
        
        validInput = false;
        while (!validInput) {
            validInput = getNumericInput(max_value, "Введите максимальное значение: ");
            if (validInput && max_value < min_value) {
                cout << "Максимальное значение должно быть больше или равно минимальному." << endl;
                validInput = false;
            }
        }

        try {
            // Генерируем случайные числа с использованием генератора Mersenne Twister
            random_device rd;  // Используется для получения семени для генератора случайных чисел
            mt19937 gen(rd()); // Стандартный генератор mersenne_twister_engine
            uniform_int_distribution<> distrib(min_value, max_value);

            for (int i = 0; i < count; i++) {
                numbers.push_back(distrib(gen));
            }
        } catch (const exception& e) {
            cerr << "Ошибка при генерации случайных чисел: " << e.what() << endl;
            return 1;
        }
    }

    // Проверяем, что массив не пустой
    if (numbers.empty()) {
        cout << "Список чисел пуст. Программа будет завершена." << endl;
        return 1;
    }

    // Записываем исходный список в файл
    if (!writeToFile(numbers, initialFileName)) {
        cout << "Не удалось сохранить исходный список. Программа будет завершена." << endl;
        return 1;
    }
    cout << "Исходный список сохранен в файл: " << initialFileName << endl;

    // Создаем копии для двух методов сортировки
    vector<int> mergeSortNumbers = numbers;
    vector<int> shakerSortNumbers = numbers;

    // Выполняем прямую сортировку слиянием
    try {
        mergeSort(mergeSortNumbers, 0, mergeSortNumbers.size() - 1);
    } catch (const exception& e) {
        cerr << "Ошибка при выполнении сортировки слиянием: " << e.what() << endl;
        return 1;
    }
    
    // Выполняем шейкерную сортировку
    try {
        shakerSort(shakerSortNumbers);
    } catch (const exception& e) {
        cerr << "Ошибка при выполнении шейкерной сортировки: " << e.what() << endl;
        return 1;
    }

    // Записываем отсортированные списки в файлы
    bool mergeSortSuccess = writeToFile(mergeSortNumbers, mergeSortFileName);
    bool shakerSortSuccess = writeToFile(shakerSortNumbers, shakerSortFileName);

    if (mergeSortSuccess) {
        cout << "Список, отсортированный методом прямого слияния, сохранен в файл: " << mergeSortFileName << endl;
    } else {
        cout << "Не удалось сохранить список, отсортированный методом прямого слияния." << endl;
    }

    if (shakerSortSuccess) {
        cout << "Список, отсортированный шейкерным методом, сохранен в файл: " << shakerSortFileName << endl;
    } else {
        cout << "Не удалось сохранить список, отсортированный шейкерным методом." << endl;
    }

    return 0;
}