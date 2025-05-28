#include <iostream>
#include <vector>
#include <cmath>
#include <climits> 
using namespace std;

// Структура для представления английской денежной суммы
struct EnglishMoney {
    int pounds;   // Фунты стерлингов
    int shillings; // Шиллинги
    int pence;    // Пенсы
};

// Функция для проверки корректности значений и нормализации
EnglishMoney normalizeSum(EnglishMoney sum) {
    // Переводим всё в пенсы для упрощения
    int totalPence = sum.pence;
    totalPence += sum.shillings * 12; // 1 шиллинг = 12 пенсов
    totalPence += sum.pounds * 240;   // 1 фунт = 20 шиллингов = 240 пенсов
    
    // Нормализуем обратно
    EnglishMoney result;
    result.pounds = totalPence / 240;
    totalPence %= 240;
    result.shillings = totalPence / 12;
    result.pence = totalPence % 12;
    
    return result;
}

// Функция для увеличения денежной суммы
EnglishMoney increaseSum(EnglishMoney sum, int addPounds, int addShillings, int addPence) {
    sum.pounds += addPounds;
    sum.shillings += addShillings;
    sum.pence += addPence;
    
    // Важно нормализовать результат
    return normalizeSum(sum);
}

// Функция для сложения двух денежных сумм
EnglishMoney addSums(EnglishMoney sum1, EnglishMoney sum2) {
    EnglishMoney result;
    result.pounds = sum1.pounds + sum2.pounds;
    result.shillings = sum1.shillings + sum2.shillings;
    result.pence = sum1.pence + sum2.pence;
    
    // Нормализуем, чтобы значения были правильными
    return normalizeSum(result);
}

// Функция для вычитания денежных сумм
EnglishMoney subtractSums(EnglishMoney sum1, EnglishMoney sum2) {
    // Переведем всё в пенсы для упрощения
    int pence1 = sum1.pounds * 240 + sum1.shillings * 12 + sum1.pence;
    int pence2 = sum2.pounds * 240 + sum2.shillings * 12 + sum2.pence;
    
    int resultPence = pence1 - pence2;
    
    // Если результат отрицательный, просто вернем нули
    // (это упрощение, можно добавить отрицательные значения если нужно)
    if (resultPence < 0) {
        EnglishMoney result = {0, 0, 0};
        return result;
    }
    
    // Нормализуем обратно из пенсов
    EnglishMoney result;
    result.pounds = resultPence / 240;
    resultPence %= 240;
    result.shillings = resultPence / 12;
    result.pence = resultPence % 12;
    
    return result;
}

// Функция для перевода денежной суммы в пенсы
int convertToPence(EnglishMoney sum) {
    return sum.pounds * 240 + sum.shillings * 12 + sum.pence;
}

// Функция для вывода денежной суммы в формате "99-99-99"
void printMoney(EnglishMoney sum) {
    // Убедимся, что сумма нормализована
    sum = normalizeSum(sum);
    cout << sum.pounds << "-" << sum.shillings << "-" << sum.pence;
}

// Функция для ввода денежной суммы
EnglishMoney inputMoney() {
    EnglishMoney money;
    cout << "Введите количество фунтов: ";
    cin >> money.pounds;
    cout << "Введите количество шиллингов: ";
    cin >> money.shillings;
    cout << "Введите количество пенсов: ";
    cin >> money.pence;
    
    // Нормализуем введенную сумму
    return normalizeSum(money);
}

int main() {
    int N;
    cout << "Введите количество денежных сумм: ";
    cin >> N;
    if (N == 0) return 1;
    // Вектор для хранения всех денежных сумм
    vector<EnglishMoney> moneyList;
    
    // Ввод всех денежных сумм
    for (int i = 0; i < N; i++) {
        cout << "Денежная сумма #" << (i + 1) << ":" << endl;
        EnglishMoney money = inputMoney();
        moneyList.push_back(money);
    }
    
    // Расчет среднего значения
    int totalPence = 0;
    for (int i = 0; i < N; i++) {
        totalPence += convertToPence(moneyList[i]);
    }
    
    int averagePence = totalPence / N;
    
    // Конвертируем обратно в фунты-шиллинги-пенсы
    EnglishMoney averageMoney;
    averageMoney.pounds = averagePence / 240;
    averagePence %= 240;
    averageMoney.shillings = averagePence / 12;
    averageMoney.pence = averagePence % 12;
    
    cout << "Среднее значение: ";
    printMoney(averageMoney);
    cout << endl;
    
    // Находим наиболее близкие суммы
    int minDiff = INT_MAX;
    int pair1_min = 0, pair2_min = 0;
    
    // Находим наиболее далекие суммы
    int maxDiff = 0;
    int pair1_max = 0, pair2_max = 0;
    
    // Перебираем все пары сумм
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            int pence1 = convertToPence(moneyList[i]);
            int pence2 = convertToPence(moneyList[j]);
            
            // Разница между суммами в пенсах
            int diff = abs(pence1 - pence2);
            
            // Проверяем минимальную разницу
            if (diff < minDiff) {
                minDiff = diff;
                pair1_min = i;
                pair2_min = j;
            }
            
            // Проверяем максимальную разницу
            if (diff > maxDiff) {
                maxDiff = diff;
                pair1_max = i;
                pair2_max = j;
            }
        }
    }
    
    // Выводим наиболее близкие суммы
    cout << "Наиболее близкие суммы: " << endl;
    cout << "Сумма #" << (pair1_min + 1) << ": ";
    printMoney(moneyList[pair1_min]);
    cout << endl;
    cout << "Сумма #" << (pair2_min + 1) << ": ";
    printMoney(moneyList[pair2_min]);
    cout << endl;
    cout << "Разница: ";
    printMoney(normalizeSum({0, 0, minDiff}));
    cout << endl;

    
    // Выводим наиболее далекие суммы
    cout << "Наиболее далекие суммы: " << endl;
    cout << "Сумма #" << (pair1_max + 1) << ": ";
    printMoney(moneyList[pair1_max]);
    cout << endl;
    cout << "Сумма #" << (pair2_max + 1) << ": ";
    printMoney(moneyList[pair2_max]);
    cout << endl;
    cout << "Разница: ";
    printMoney(normalizeSum({0, 0, maxDiff}));
    cout << endl;

    
    return 0;
}