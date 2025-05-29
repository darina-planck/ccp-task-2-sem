#include <iostream>
#include <random>
#include <vector>
#include <fstream>

using namespace std;

// Структура узла бинарного дерева
struct TreeNode {
    int data;           // Данные узла
    TreeNode* left;     // структура узла на (указатель *) имя переменной 
    TreeNode* right;   
    
    // Конструктор узла
    TreeNode(int value) : data(value), left(nullptr), right(nullptr) {}
};

// Класс бинарного дерева поиска
class BinarySearchTree {
private:
    TreeNode* root;     // Корень дерева
    
    // Рекурсивная функция вставки узла
    TreeNode* insert(TreeNode* node, int value) {
        // Если узел пустой, создаем новый
        if (node == nullptr) {
            return new TreeNode(value);
        }
        
        // Если значение меньше текущего узла - идем налево
        if (value < node->data) {
            node->left = insert(node->left, value);
        }
        // Иначе идем направо
        else {
            node->right = insert(node->right, value);
        }
        
        return node;
    }
    
    // Рекурсивная функция вывода дерева в консоль с подписями L/R
    void printTree(TreeNode* node, string prefix, bool isLast, string direction = "") {
        if (node != nullptr) { // node - текущий обрабатываемый узел
            cout << prefix; //  - накопленная строка отступов для текущего уровня
            cout << (isLast ? "└── " : "├── "); // - флаг, является ли узел последним потомком родителя
            // выбор символа: "└── " для последнего потомка, "├── " для остальных
            cout << direction << node->data << endl;
            
            // Рекурсивно выводим детей
            if (node->left || node->right) {
                if (node->right) {
                    printTree(node->right, prefix + (isLast ? "    " : "│   "), !node->left, "(R) ");
                }
                if (node->left) {
                    printTree(node->left, prefix + (isLast ? "    " : "│   "), true, "(L) ");
                }
            }
        }
    }
    
    // Рекурсивная функция записи дерева в текстовый файл
    void writeTreeToText(TreeNode* node, ofstream& file, string prefix, bool isLast, string direction = "") {
        if (node != nullptr) { // это типо пустой адрес
            file << prefix;
            file << (isLast ? "└── " : "├── ");
            file << direction << node->data << endl;
            
            // рекурсивно надо записывать в файл чтобы также красиво было как при работе
            if (node->left || node->right) {
                if (node->right) {
                    writeTreeToText(node->right, file, prefix + (isLast ? "    " : "│   "), !node->left, "(R) ");
                }
                if (node->left) {
                    writeTreeToText(node->left, file, prefix + (isLast ? "    " : "│   "), true, "(L) ");
                }
            }
        }
    }
    
public:
    // Конструктор
    BinarySearchTree() : root(nullptr) {}
    
    // Публичная функция вставки
    void insert(int value) {
        root = insert(root, value); // Обновляю корень, потому что при первой вставке
        // root может измениться с nullptr на новый узел.
    }
    
    // Публичная функция вывода дерева
    void printTree() {
        if (root == nullptr) {
            cout << "Дерево пустое!" << endl;
            return;
        }
        
        cout << "\nСтруктура дерева:" << endl;
        cout << "Корень: " << root->data << endl;
        printTree(root, "", true);
    }
    
    // Функция записи дерева в текстовый файл
    void saveTreeToFile(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Ошибка создания файла!" << endl;
            return;
        }
        
        // Записываем заголовок
        file << "Бинарное дерево" << endl;
        file << "Правило: меньше - налево, больше/равно - направо" << endl;
        file << endl;
        
        if (root != nullptr) {
            file << "Корень: " << root->data << endl;
            file << "Структура дерева:" << endl;
            writeTreeToText(root, file, "", true);
        } else {
            file << "Дерево пустое!" << endl;
        }
        
        file << endl;
        file << "Дерево успешно сохранено!" << endl;
        file.close();
        
        cout << "Дерево сохранено в файл: " << filename << endl;
        cout << "Откройте файл в любом текстовом редакторе для просмотра" << endl;
    }
};

int main() {
    BinarySearchTree bst;
    int n, method;
    
    // Ввод количества чисел
    cout << "Введите количество чисел (N): ";
    cin >> n;
    
    // Проверка корректности ввода
    if (n <= 0) {
        cout << "Количество чисел должно быть положительным!" << endl;
        return 1;
    }
    
    // Выбор метода ввода
    cout << "\nВыберите метод ввода чисел:" << endl;
    cout << "1 - Ввод вручную" << endl;
    cout << "2 - Автоматическая генерация (случайные числа от 1 до " << n << ")" << endl;
    cout << "Ваш выбор: ";
    cin >> method;
    
    vector<int> numbers;
    
    if (method == 1) {
        // Ручной ввод чисел
        cout << "\nВведите " << n << " чисел:" << endl;
        for (int i = 0; i < n; i++) {
            int num;
            cout << "Число " << (i + 1) << ": ";
            cin >> num;
            numbers.push_back(num);
        }
    }
    else if (method == 2) {
        // Автоматическая генерация
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, n);
        
        cout << "\nСгенерированные числа: ";
        for (int i = 0; i < n; i++) {
            int num = dis(gen);
            numbers.push_back(num);
            cout << num << " ";
        }
        cout << endl;
    }
    else {
        cout << "Неверный выбор метода!" << endl;
        return 1;
    }
    
    // Построение дерева
    cout << "\nПостроение бинарного дерева поиска..." << endl;
    for (int num : numbers) {
        bst.insert(num);
    }
    
    // Вывод дерева в консоль
    bst.printTree();
    
    // Сохранение дерева в файл
    cout << "\nСохранить дерево в файл? (y/n): ";
    char choice;
    cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        bst.saveTreeToFile("binary_tree.txt");
    }
    
    cout << "\nПрограмма завершена!" << endl;
    return 0;
}