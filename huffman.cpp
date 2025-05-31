#include <iostream> 
#include <map>       // Для ассоциативного контейнера map (частоты символов и коды)
#include <queue>     // Для приоритетной очереди (priority_queue) при построении дерева
#include <string>    // Для работы со строками
#include <vector>    // Для контейнера vector (используется в priority_queue)
#include <iomanip>   // Для форматирования вывода (setprecision)

using namespace std;

// Структура Node представляет узел бинарного дерева Хаффмана
struct Node {
    char character;  // Символ (заполняется только в листьях дерева)
    int frequency;   // Частота символа или сумма частот поддерева
    Node* left;      // Указатель на левого потомка (код 0)
    Node* right;     // Указатель на правого потомка (код 1)
    
    // Первый для листьев (с символом), второй для внутренних узлов (без символа)
    Node(char ch, int freq) : character(ch), frequency(freq), left(nullptr), right(nullptr) {}
    Node(int freq) : character('\0'), frequency(freq), left(nullptr), right(nullptr) {}
};


// Compare определяет порядок в min-heap для алгоритма Хаффмана
struct Compare {
    bool operator()(Node* a, Node* b) {
        // Если частоты равны, сравниваем по символу для детерминированности
        if (a->frequency == b->frequency) {
            return a->character > b->character;  // Лексикографический порядок
        }
        // Основное сравнение: узел с МЕНЬШЕЙ частотой имеет БОЛЬШИЙ приоритет
        return a->frequency > b->frequency;  // Min-heap: меньше = выше приоритет
    }
};

// Функция проходит по строке и увеличивает счетчик для каждого символа
map<char, int> calculateFrequencies(const string& text) {
    map<char, int> frequencies;  // Ассоциативный массив: символ → частота
    // Проходим по каждому символу входной строки
    for (char ch : text) {
        frequencies[ch]++;  // Увеличиваем счетчик для данного символа
        // Если символа не было, map автоматически создаст запись со значением 0
    }
    return frequencies;  // Возвращаем таблицу частот
}

// алгоритм: берем два узла с минимальными частотами, объединяем их
Node* buildHuffmanTree(map<char, int>& frequencies) {
    // Приоритетная очередь (min-heap) для хранения узлов по возрастанию частоты
    priority_queue<Node*, vector<Node*>, Compare> pq; // min-heap - это граф с минимальным числом в корне и большими в листьях
    // priority_queue - массив, где мы добавляем в конец и берем из начала
    // Создаем листовые узлы для каждого символа
    for (auto& pair : frequencies) { // frequencies - переменная содержащая частоты (который map)
        // pair.first - символ, pair.second - частота
        pq.push(new Node(pair.first, pair.second)); 
    }
    // 
    // если только один уникальный символ в тексте
    if (pq.size() == 1) {
        Node* root = new Node(pq.top()->frequency);  // Создаем корень
        root->left = pq.top();  // Единственный символ - левый потомок
        return root;  // Код будет "0"
    }
    
    // Основной алгоритм Хаффмана
    while (pq.size() > 1) {  // Пока есть более одного узла
        // Извлекаем два узла с наименьшими частотами
        Node* left = pq.top();
        pq.pop();   // Узел с минимальной частотой
        Node* right = pq.top(); 
        pq.pop();  // Узел со следующей минимальной частотой
        
        // Создаем новый внутренний узел
        Node* merged = new Node(left->frequency + right->frequency);  // Сумма частот, достанных с двух первых элементов очереди, в одном узле
        merged->left = left;    // Левый потомок (код 0)
        merged->right = right;  // Правый потомок (код 1)
        
        // Добавляем объединенный узел обратно в очередь
        pq.push(merged);
    }
    
    // Последний оставшийся узел - корень дерева
    return pq.top();
}

// Рекурсивный обход дерева, накапливая бинарный код по пути
void generateCodes(Node* root, string code, map<char, string>& codes) {
    if (!root) return;  // Базовый случай: пустой узел
    
    // Если это лист (содержит символ)
    if (!root->left && !root->right) {
        // Если корень - единственный узел, код = "0"
        codes[root->character] = code.empty() ? "0" : code;
        return;
    }
    
    // Рекурсивный обход:
    generateCodes(root->left, code + "0", codes);   // Левый путь = добавить "0"
    generateCodes(root->right, code + "1", codes);  // Правый путь = добавить "1"
}


// Рекурсивная функция с отслеживанием уровня вложенности и позиции
void printTree(Node* root, string prefix = "", bool isLast = true) {
    if (!root) return;  // Базовый случай
    
    // Выводим текущий уровень отступа
    cout << prefix;
    // Символы для древовидной структуры
    cout << (isLast ? "└── " : "├── ");  // └── для последнего, ├── для промежуточного
    
    if (!root->left && !root->right) {
        // ЛИСТ: выводим символ и его частоту
        char ch = root->character;
        // Обрабатываем специальные символы для читаемости
        if (ch == ' ') {
            cout << "'SPACE' (" << root->frequency << ")" << endl;
        } else if (ch == '\n') {
            cout << "'NEWLINE' (" << root->frequency << ")" << endl;
        } else if (ch == '\t') {
            cout << "'TAB' (" << root->frequency << ")" << endl;
        } else {
            cout << "'" << ch << "' (" << root->frequency << ")" << endl;
        }
    } else {
        // выводим только сумму частот
        cout << "[" << root->frequency << "]" << endl;
    }
    
    // Формируем отступ для потомков
    string newPrefix = prefix + (isLast ? "    " : "│   ");
    
    // Рекурсивно выводим потомков
    if (root->left && root->right) {
        printTree(root->left, newPrefix, false);   // Левый потомок не последний
        printTree(root->right, newPrefix, true);   // Правый потомок последний
    } else if (root->left) {
        printTree(root->left, newPrefix, true);    // Только левый потомок
    } else if (root->right) {
        printTree(root->right, newPrefix, true);   // Только правый потомок
    }
}

// Заменяем каждый символ его кодом Хаффмана и конкатенируем результат
string encodeString(const string& text, map<char, string>& codes) {
    string encoded = "";  // Результирующая строка битов
    
    // Проходим по каждому символу исходного текста
    for (char ch : text) {
        encoded += codes[ch];  // Добавляем код символа к результату
    }
    
    return encoded;  // Возвращаем закодированную строку
}

// Сравниваем размер в битах до и после кодирования
void printCompressionStats(const string& original, const string& encoded) {
    int originalBits = original.length() * 8;  // ASCII: 8 бит на символ
    int encodedBits = encoded.length();        // Количество бит в коде Хаффмана
    
    cout << "\n=== статистика сжатия ===" << endl;
    cout << "Исходный размер: " << originalBits << " бит (" 
         << original.length() << " символов × 8 бит)" << endl;
    cout << "Размер после кодирования: " << encodedBits << " бит" << endl;
    
    // Вычисляем процент сжатия
    cout << "Степень сжатия: " << fixed << setprecision(2) 
         << (double)encodedBits / originalBits * 100 << "%" << endl;
    cout << "Экономия: " << originalBits - encodedBits << " бит" << endl;
}

int main() {
    cout << "=== алгоритм хаффмана ===" << endl;
    cout << "Введите строку для кодирования: ";
    
    string input;
    getline(cin, input);  // Читаем целую строку (включая пробелы)
    
    // Проверяем корректность входных данных
    if (input.empty()) {
        cout << "Ошибка: введена пустая строка!" << endl;
        return 1;  // Завершаем программу с кодом ошибки
    }
    
    // Подсчитываем частоты всех символов
    map<char, int> frequencies = calculateFrequencies(input); // ассоциативный массив (типо словарь) с кол-вом всех символов
    
    cout << "\n=== частоты символов ===" << endl;
    // Выводим таблицу частот для анализа
    for (auto& pair : frequencies) { //// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        char ch = pair.first; // pair - тип данных который состоит из первого и второго элемента
        // Специальная обработка непечатаемых символов
        if (ch == ' ') {
            cout << "'SPACE': " << pair.second << endl;
        } else if (ch == '\n') {
            cout << "'NEWLINE': " << pair.second << endl;
        } else if (ch == '\t') {
            cout << "'TAB': " << pair.second << endl;
        } else {
            cout << "'" << ch << "': " << pair.second << endl;
        }
    }
    
    // Строим дерево Хаффмана на основе частот
    Node* root = buildHuffmanTree(frequencies);
    
    // Генерируем коды для каждого символа
    map<char, string> codes;
    generateCodes(root, "", codes);  // Начинаем с пустого кода
    
    cout << "\n=== коды хаффмана ===" << endl;
    // Выводим полученные коды для проверки
    for (auto& pair : codes) {
        char ch = pair.first;
        // Специальная обработка непечатаемых символов
        if (ch == ' ') {
            cout << "'SPACE': " << pair.second << endl;
        } else if (ch == '\n') {
            cout << "'NEWLINE': " << pair.second << endl;
        } else if (ch == '\t') {
            cout << "'TAB': " << pair.second << endl;
        } else {
            cout << "'" << ch << "': " << pair.second << endl;
        }
    }
    
    // Визуализируем построенное дерево
    cout << "\n=== дерево хаффмана ===" << endl;
    printTree(root);  // Выводим структуру дерева
    
    // Кодируем исходную строку
    string encoded = encodeString(input, codes);
    cout << "\n=== результат кодирования ===" << endl;
    cout << "Исходная строка: \"" << input << "\"" << endl;
    cout << "Закодированная строка: " << encoded << endl;
    
    // Анализируем эффективность сжатия
    printCompressionStats(input, encoded);
    
    // Демонстрируем пошаговое кодирование
    cout << "\n=== проверка кодирования ===" << endl;
    cout << "Пошаговое кодирование:" << endl;
    
    // Показываем, как каждый символ преобразуется в код
    for (size_t i = 0; i < input.length(); i++) {
        char ch = input[i];
        string charName;
        
        // Формируем читаемое имя символа
        if (ch == ' ') charName = "SPACE";
        else if (ch == '\n') charName = "NEWLINE";  
        else if (ch == '\t') charName = "TAB";
        else charName = ch;
        
        // Выводим преобразование: символ → код
        cout << "'" << charName << "' → " << codes[ch] << endl;
    }
    return 0;  // Успешное завершение программы
}