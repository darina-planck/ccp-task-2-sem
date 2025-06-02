#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <queue>
#include <random>
#include <chrono>
#include <climits>

using namespace std;
using namespace chrono;

// ================ B-ДЕРЕВО ================

class BTreeNode {
public:
    vector<int> keys;           // Ключи в узле
    vector<BTreeNode*> children; // Указатели на дочерние узлы
    bool isLeaf;                // Является ли узел листом
    int degree;                 // Минимальная степень

    BTreeNode(int degree, bool isLeaf) {
        this->degree = degree;
        this->isLeaf = isLeaf;
        keys.reserve(2 * degree - 1);
        if (!isLeaf) {
            children.reserve(2 * degree);
        }
    }

    ~BTreeNode() {
        for (auto child : children) {
            delete child;
        }
    }

    // Поиск ключа в поддереве с корнем в данном узле
    BTreeNode* search(int key) {
        int i = 0;
        while (i < keys.size() && key > keys[i]) {
            i++;
        }

        if (i < keys.size() && keys[i] == key) {
            return this;
        }

        if (isLeaf) {
            return nullptr;
        }

        return children[i]->search(key);
    }

    // Вставка ключа в незаполненный узел
    void insertNonFull(int key) {
        int i = keys.size() - 1;

        if (isLeaf) {
            keys.push_back(0);
            while (i >= 0 && keys[i] > key) {
                keys[i + 1] = keys[i];
                i--;
            }
            keys[i + 1] = key;
        } else {
            while (i >= 0 && keys[i] > key) {
                i--;
            }
            i++;

            if (children[i]->keys.size() == 2 * degree - 1) {
                splitChild(i);
                if (keys[i] < key) {
                    i++;
                }
            }
            children[i]->insertNonFull(key);
        }
    }

    // Разделение полного дочернего узла
    void splitChild(int index) {
        BTreeNode* fullChild = children[index];
        BTreeNode* newChild = new BTreeNode(degree, fullChild->isLeaf);

        // Копируем последние (degree-1) ключей из fullChild в newChild
        for (int j = 0; j < degree - 1; j++) {
            newChild->keys.push_back(fullChild->keys[j + degree]);
        }

        // Копируем последние degree дочерних узлов, если fullChild не лист
        if (!fullChild->isLeaf) {
            for (int j = 0; j < degree; j++) {
                newChild->children.push_back(fullChild->children[j + degree]);
            }
            fullChild->children.resize(degree);
        }

        // Перемещаем средний ключ вверх
        keys.insert(keys.begin() + index, fullChild->keys[degree - 1]);
        children.insert(children.begin() + index + 1, newChild);

        // Уменьшаем размер fullChild
        fullChild->keys.resize(degree - 1);
    }

    // Печать узла (для отладки)
    void traverse() {
        int i;
        for (i = 0; i < keys.size(); i++) {
            if (!isLeaf) {
                children[i]->traverse();
            }
            cout << keys[i] << " ";
        }
        if (!isLeaf) {
            children[i]->traverse();
        }
    }
};

class BTree {
public:
    BTreeNode* root;
    int degree;

    BTree(int degree) {
        this->degree = degree;
        root = new BTreeNode(degree, true);
    }

    ~BTree() {
        delete root;
    }

    BTreeNode* search(int key) {
        return root->search(key);
    }

    void insert(int key) {
        if (root->keys.size() == 2 * degree - 1) {
            BTreeNode* newRoot = new BTreeNode(degree, false);
            newRoot->children.push_back(root);
            newRoot->splitChild(0);
            root = newRoot;
        }
        root->insertNonFull(key);
    }

    void traverse() {
        if (root != nullptr) {
            root->traverse();
        }
    }

    // Получение высоты дерева
    int getHeight() {
        return getHeight(root);
    }

    int getHeight(BTreeNode* node) {
        if (node == nullptr || node->isLeaf) {
            return 1;
        }
        return 1 + getHeight(node->children[0]);
    }

    // Подсчет узлов
    int getNodeCount() {
        return getNodeCount(root);
    }

    int getNodeCount(BTreeNode* node) {
        if (node == nullptr) return 0;
        int count = 1;
        for (auto child : node->children) {
            count += getNodeCount(child);
        }
        return count;
    }

    // Визуализация дерева
    void printTree() {
        if (root == nullptr) {
            cout << "Дерево пусто\n";
            return;
        }
        
        cout << "\n=== B-ДЕРЕВО (степень " << degree << ") ===\n";
        printLevel(root, 0);
    }

private:
    void printLevel(BTreeNode* node, int level) {
        if (node == nullptr) return;
        
        // Печатаем отступ
        for (int i = 0; i < level; i++) {
            cout << "    ";
        }
        
        // Печатаем ключи узла
        cout << "[";
        for (int i = 0; i < node->keys.size(); i++) {
            cout << node->keys[i];
            if (i < node->keys.size() - 1) cout << ", ";
        }
        cout << "]";
        if (node->isLeaf) cout << " (лист)";
        cout << "\n";
        
        // Рекурсивно печатаем дочерние узлы
        for (auto child : node->children) {
            printLevel(child, level + 1);
        }
    }
};

// ================ B+-ДЕРЕВО ================

class BPlusTreeNode {
public:
    vector<int> keys;
    vector<BPlusTreeNode*> children;
    BPlusTreeNode* next;  // Указатель на следующий листовой узел
    bool isLeaf;
    int degree;

    BPlusTreeNode(int degree, bool isLeaf) {
        this->degree = degree;
        this->isLeaf = isLeaf;
        this->next = nullptr;
        keys.reserve(2 * degree - 1);
        if (!isLeaf) {
            children.reserve(2 * degree);
        }
    }

    ~BPlusTreeNode() {
        for (auto child : children) {
            delete child;
        }
    }

    BPlusTreeNode* search(int key) {
        int i = 0;
        
        if (isLeaf) {
            // В листе ищем точное совпадение
            for (int j = 0; j < keys.size(); j++) {
                if (keys[j] == key) {
                    return this;
                }
            }
            return nullptr;
        } else {
            // Во внутреннем узле ищем подходящего ребенка
            while (i < keys.size() && key >= keys[i]) {
                i++;
            }
            return children[i]->search(key);
        }
    }

    void insertNonFull(int key) {
        int i = keys.size() - 1;

        if (isLeaf) {
            keys.push_back(0);
            while (i >= 0 && keys[i] > key) {
                keys[i + 1] = keys[i];
                i--;
            }
            keys[i + 1] = key;
        } else {
            while (i >= 0 && key < keys[i]) {
                i--;
            }
            i++;

            if (children[i]->keys.size() == 2 * degree - 1) {
                splitChild(i);
                if (key >= keys[i]) {
                    i++;
                }
            }
            children[i]->insertNonFull(key);
        }
    }

    void splitChild(int index) {
        BPlusTreeNode* fullChild = children[index];
        BPlusTreeNode* newChild = new BPlusTreeNode(degree, fullChild->isLeaf);

        if (fullChild->isLeaf) {
            // Для листового узла копируем ключи начиная с середины
            int mid = degree - 1;
            for (int j = mid; j < fullChild->keys.size(); j++) {
                newChild->keys.push_back(fullChild->keys[j]);
            }
            
            // Связываем листовые узлы
            newChild->next = fullChild->next;
            fullChild->next = newChild;
            
            // В B+ дереве копируем первый ключ нового узла вверх
            int keyToCopyUp = newChild->keys[0];
            keys.insert(keys.begin() + index, keyToCopyUp);
            children.insert(children.begin() + index + 1, newChild);
            
            // Обрезаем исходный узел
            fullChild->keys.resize(mid);
        } else {
            // Для внутреннего узла
            int mid = degree - 1;
            for (int j = mid + 1; j < fullChild->keys.size(); j++) {
                newChild->keys.push_back(fullChild->keys[j]);
            }
            
            for (int j = mid + 1; j < fullChild->children.size(); j++) {
                newChild->children.push_back(fullChild->children[j]);
            }
            
            // Перемещаем средний ключ вверх
            int keyToMoveUp = fullChild->keys[mid];
            keys.insert(keys.begin() + index, keyToMoveUp);
            children.insert(children.begin() + index + 1, newChild);
            
            // Обрезаем исходный узел
            fullChild->keys.resize(mid);
            fullChild->children.resize(mid + 1);
        }
    }
};

class BPlusTree {
public:
    BPlusTreeNode* root;
    int degree;

    BPlusTree(int degree) {
        this->degree = degree;
        root = new BPlusTreeNode(degree, true);
    }

    ~BPlusTree() {
        delete root;
    }

    BPlusTreeNode* search(int key) {
        return root->search(key);
    }

    void insert(int key) {
        if (root->keys.size() == 2 * degree - 1) {
            BPlusTreeNode* newRoot = new BPlusTreeNode(degree, false);
            newRoot->children.push_back(root);
            newRoot->splitChild(0);
            root = newRoot;
        }
        root->insertNonFull(key);
    }

    // Печать всех ключей в порядке возрастания (через листья)
    void traverseLeaves() {
        BPlusTreeNode* current = getFirstLeaf();
        while (current != nullptr) {
            for (int key : current->keys) {
                cout << key << " ";
            }
            current = current->next;
        }
    }

    BPlusTreeNode* getFirstLeaf() {
        BPlusTreeNode* current = root;
        while (!current->isLeaf) {
            current = current->children[0];
        }
        return current;
    }

    int getHeight() {
        return getHeight(root);
    }

    int getHeight(BPlusTreeNode* node) {
        if (node == nullptr || node->isLeaf) {
            return 1;
        }
        return 1 + getHeight(node->children[0]);
    }

    int getNodeCount() {
        return getNodeCount(root);
    }

    int getNodeCount(BPlusTreeNode* node) {
        if (node == nullptr) return 0;
        int count = 1;
        for (auto child : node->children) {
            count += getNodeCount(child);
        }
        return count;
    }

    void printTree() {
        if (root == nullptr) {
            cout << "Дерево пусто\n";
            return;
        }
        
        cout << "\n=== B+-ДЕРЕВО (степень " << degree << ") ===\n";
        printLevel(root, 0);
        
        cout << "\nПоследовательность листьев: ";
        traverseLeaves();
        cout << "\n";
    }

private:
    void printLevel(BPlusTreeNode* node, int level) {
        if (node == nullptr) return;
        
        for (int i = 0; i < level; i++) {
            cout << "    ";
        }
        
        cout << "[";
        for (int i = 0; i < node->keys.size(); i++) {
            cout << node->keys[i];
            if (i < node->keys.size() - 1) cout << ", ";
        }
        cout << "]";
        if (node->isLeaf) cout << " (лист)";
        cout << "\n";
        
        for (auto child : node->children) {
            printLevel(child, level + 1);
        }
    }
};

// ================ КЛАСС ДЛЯ ТЕСТИРОВАНИЯ ================

class TreeTester {
private:
    BTree* btree;
    BPlusTree* bplusTree;
    int degree;
    vector<int> insertedKeys;

public:
    TreeTester(int deg) : degree(deg) {
        btree = new BTree(degree);
        bplusTree = new BPlusTree(degree);
    }

    ~TreeTester() {
        delete btree;
        delete bplusTree;
    }

    void printMenu() {
        cout << "\n" << string(60, '=') << "\n";
        cout << "    ДЕМОНСТРАЦИЯ B-ДЕРЕВЬЕВ И B+-ДЕРЕВЬЕВ\n";
        cout << string(60, '=') << "\n";
        cout << "1. Вставить ключ\n";
        cout << "2. Найти ключ\n";
        cout << "3. Показать деревья\n";
        cout << "4. Вставить случайные ключи\n";
        cout << "5. Сравнить характеристики\n";
        cout << "6. Тест производительности\n";
        cout << "7. Изменить степень дерева\n";
        cout << "8. Очистить деревья\n";
        cout << "0. Выход\n";
        cout << string(60, '-') << "\n";
        cout << "Текущая степень: " << degree << " | Вставлено ключей: " << insertedKeys.size() << "\n";
        cout << "Выберите действие: ";
    }

    void insertKey() {
        int key;
        cout << "Введите ключ для вставки: ";
        cin >> key;

        if (find(insertedKeys.begin(), insertedKeys.end(), key) != insertedKeys.end()) {
            cout << "Ключ " << key << " уже существует!\n";
            return;
        }

        auto start = high_resolution_clock::now();
        btree->insert(key);
        auto btree_time = high_resolution_clock::now() - start;

        start = high_resolution_clock::now();
        bplusTree->insert(key);
        auto bplus_time = high_resolution_clock::now() - start;

        insertedKeys.push_back(key);
        sort(insertedKeys.begin(), insertedKeys.end());

        cout << "Ключ " << key << " успешно вставлен!\n";
        cout << "Время вставки - B-дерево: " << duration_cast<nanoseconds>(btree_time).count() << "ns, ";
        cout << "B+-дерево: " << duration_cast<nanoseconds>(bplus_time).count() << "ns\n";
    }

    void searchKey() {
        int key;
        cout << "Введите ключ для поиска: ";
        cin >> key;

        auto start = high_resolution_clock::now();
        BTreeNode* btree_result = btree->search(key);
        auto btree_time = high_resolution_clock::now() - start;

        start = high_resolution_clock::now();
        BPlusTreeNode* bplus_result = bplusTree->search(key);
        auto bplus_time = high_resolution_clock::now() - start;

        cout << "\nРезультаты поиска ключа " << key << ":\n";
        cout << "B-дерево: " << (btree_result ? "НАЙДЕН" : "НЕ НАЙДЕН");
        cout << " (время: " << duration_cast<nanoseconds>(btree_time).count() << "ns)\n";
        cout << "B+-дерево: " << (bplus_result ? "НАЙДЕН" : "НЕ НАЙДЕН");
        cout << " (время: " << duration_cast<nanoseconds>(bplus_time).count() << "ns)\n";
    }

    void showTrees() {
        btree->printTree();
        bplusTree->printTree();
    }

    void insertRandomKeys() {
        int count;
        cout << "Сколько случайных ключей вставить? ";
        cin >> count;

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, 1000);

        vector<int> newKeys;
        for (int i = 0; i < count; i++) {
            int key;
            do {
                key = dis(gen);
            } while (find(insertedKeys.begin(), insertedKeys.end(), key) != insertedKeys.end() ||
                     find(newKeys.begin(), newKeys.end(), key) != newKeys.end());
            newKeys.push_back(key);
        }

        cout << "Вставляем ключи: ";
        for (int key : newKeys) {
            cout << key << " ";
            btree->insert(key);
            bplusTree->insert(key);
            insertedKeys.push_back(key);
        }
        cout << "\n";

        sort(insertedKeys.begin(), insertedKeys.end());
        cout << "Успешно вставлено " << count << " ключей!\n";
    }

    void compareCharacteristics() {
        cout << "\n" << string(50, '=') << "\n";
        cout << "         СРАВНЕНИЕ ХАРАКТЕРИСТИК\n";
        cout << string(50, '=') << "\n";
        
        cout << left << setw(25) << "Характеристика" 
             << setw(12) << "B-дерево" 
             << setw(12) << "B+-дерево" << "\n";
        cout << string(50, '-') << "\n";
        
        cout << left << setw(25) << "Высота дерева:" 
             << setw(12) << btree->getHeight() 
             << setw(12) << bplusTree->getHeight() << "\n";
             
        cout << left << setw(25) << "Количество узлов:" 
             << setw(12) << btree->getNodeCount() 
             << setw(12) << bplusTree->getNodeCount() << "\n";
             
        cout << left << setw(25) << "Степень дерева:" 
             << setw(12) << degree 
             << setw(12) << degree << "\n";
             
        cout << left << setw(25) << "Всего ключей:" 
             << setw(12) << insertedKeys.size() 
             << setw(12) << insertedKeys.size() << "\n";

        cout << "\nОСНОВНЫЕ РАЗЛИЧИЯ:\n";
        cout << "• B-дерево: данные во всех узлах, лучше для точечного поиска\n";
        cout << "• B+-дерево: данные только в листьях, лучше для диапазонных запросов\n";
        cout << "• B+-дерево: листья связаны в список для последовательного доступа\n";
    }

    void performanceTest() {
        cout << "Тест производительности на 1000 операций...\n";
        
        vector<int> testKeys;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(10000, 20000);
        
        // Генерируем тестовые ключи
        for (int i = 0; i < 1000; i++) {
            int key;
            do {
                key = dis(gen);
            } while (find(testKeys.begin(), testKeys.end(), key) != testKeys.end());
            testKeys.push_back(key);
        }

        // Тест вставки
        auto start = high_resolution_clock::now();
        for (int key : testKeys) {
            btree->insert(key);
        }
        auto btree_insert_time = high_resolution_clock::now() - start;

        start = high_resolution_clock::now();
        for (int key : testKeys) {
            bplusTree->insert(key);
        }
        auto bplus_insert_time = high_resolution_clock::now() - start;

        // Тест поиска
        start = high_resolution_clock::now();
        for (int key : testKeys) {
            btree->search(key);
        }
        auto btree_search_time = high_resolution_clock::now() - start;

        start = high_resolution_clock::now();
        for (int key : testKeys) {
            bplusTree->search(key);
        }
        auto bplus_search_time = high_resolution_clock::now() - start;

        cout << "\nРЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ:\n";
        cout << string(50, '-') << "\n";
        cout << "Вставка 1000 элементов:\n";
        cout << "  B-дерево:  " << duration_cast<microseconds>(btree_insert_time).count() << "μs\n";
        cout << "  B+-дерево: " << duration_cast<microseconds>(bplus_insert_time).count() << "μs\n";
        cout << "\nПоиск 1000 элементов:\n";
        cout << "  B-дерево:  " << duration_cast<microseconds>(btree_search_time).count() << "μs\n";
        cout << "  B+-дерево: " << duration_cast<microseconds>(bplus_search_time).count() << "μs\n";

        // Добавляем ключи в основной список
        for (int key : testKeys) {
            insertedKeys.push_back(key);
        }
        sort(insertedKeys.begin(), insertedKeys.end());
    }

    void changeDegree() {
        int newDegree;
        cout << "Введите новую степень дерева (минимум 2): ";
        cin >> newDegree;
        
        if (newDegree < 2) {
            cout << "Ошибка: степень должна быть не менее 2!\n";
            return;
        }
        
        vector<int> oldKeys = insertedKeys;
        
        delete btree;
        delete bplusTree;
        
        degree = newDegree;
        btree = new BTree(degree);
        bplusTree = new BPlusTree(degree);
        insertedKeys.clear();
        
        // Повторно вставляем ключи
        for (int key : oldKeys) {
            btree->insert(key);
            bplusTree->insert(key);
            insertedKeys.push_back(key);
        }
        
        cout << "Степень изменена на " << degree << ". Деревья пересозданы.\n";
    }

    void clearTrees() {
        delete btree;
        delete bplusTree;
        
        btree = new BTree(degree);
        bplusTree = new BPlusTree(degree);
        insertedKeys.clear();
        
        cout << "Деревья очищены.\n";
    }

    void run() {
        int choice;
        
        cout << "Добро пожаловать в демонстрацию B-деревьев и B+-деревьев!\n";
        cout << "Эта программа покажет различия между двумя структурами данных.\n";
        
        do {
            printMenu();
            cin >> choice;
            
            switch (choice) {
                case 1: insertKey(); break;
                case 2: searchKey(); break;
                case 3: showTrees(); break;
                case 4: insertRandomKeys(); break;
                case 5: compareCharacteristics(); break;
                case 6: performanceTest(); break;
                case 7: changeDegree(); break;
                case 8: clearTrees(); break;
                case 0: cout << "До свидания!\n"; break;
                default: cout << "Неверный выбор!\n"; break;
            }
            
            if (choice != 0) {
                cout << "\nНажмите Enter для продолжения...";
                cin.ignore();
                cin.get();
            }
            
        } while (choice != 0);
    }
};

// ================ ГЛАВНАЯ ФУНКЦИЯ ================

int main() {
    // Устанавливаем локаль для корректного отображения русского текста
    setlocale(LC_ALL, "Russian");
    
    cout << string(60, '=') << "\n";
    cout << "  ДЕМОНСТРАЦИЯ B-ДЕРЕВЬЕВ И B+-ДЕРЕВЬЕВ\n";
    cout << string(60, '=') << "\n";
    cout << "Программа демонстрирует принципы работы и различия\n";
    cout << "между B-деревьями и B+-деревьями.\n\n";
    
    int degree;
    cout << "Введите степень дерева (рекомендуется 3-5): ";
    cin >> degree;
    
    if (degree < 2) {
        cout << "Степень должна быть не менее 2. Установлена степень 3.\n";
        degree = 3;
    }
    
    TreeTester tester(degree);
    tester.run();
    
    return 0;
}