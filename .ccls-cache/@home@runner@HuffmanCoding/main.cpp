#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
 
/* Структура Node, представляющая узел в дереве Хаффмана */
struct Node
{
    char m_char;               // Символ, представленный этим узлом
    int m_frequency;           // Частота этого символа в исходном тексте
    Node* m_left;              // Указатель на левого потомка этого узла
    Node* m_right;             // Указатель на правого потомка этого узла
 
    /* Конструктор для создания нового узла */
    Node(char m_char, int m_frequency, Node* m_left = nullptr, Node* m_right = nullptr)
        : m_char(m_char), m_frequency(m_frequency), m_left(m_left), m_right(m_right) {}
};
 
/* Класс HuffmanTree */
class HuffmanTree
{
private:
    Node* root;                 // Корень дерева Хаффмана
 
    /*
    Структура NodeComparator используется для сравнения двух узлов по их частотам.
    Это нужно для построения очереди с приоритетами, где узлы с меньшей частотой имеют более высокий приоритет.
    */
    struct NodeComparator
    {
        bool operator()(Node* left, Node* right)
        {
            return left->m_frequency > right->m_frequency;
        }
    };
 
    void BuildTree(const std::unordered_map<char, int>& frequencyMap)
    {
        std::priority_queue<Node*, std::vector<Node*>, NodeComparator> minHeap;
 
        for (auto pair : frequencyMap)
        {
            minHeap.push(new Node(pair.first, pair.second));
        }
 
        while (minHeap.size() != 1)
        {
            Node* m_left = minHeap.top(); minHeap.pop();
            Node* m_right = minHeap.top(); minHeap.pop();
 
            int sum = m_left->m_frequency + m_right->m_frequency;
            minHeap.push(new Node('\0', sum, m_left, m_right));
        }
 
        root = minHeap.top();
    }
 
    /* Функция encodeHelper, рекурсивно обходящая дерево Хаффмана и строящая коды Хаффмана для каждого символа */
    void EncodeHelper(Node* node, std::string str, std::unordered_map<char, std::string>& huffmanCode)
    {
        /* Если узел пуст, просто возвращаемся */
        if (node == nullptr)
        {
            return;
        }
 
        /* Если узел является листом (то есть у него нет дочерних узлов),
        то мы достигли символа и добавляем текущую строку в качестве его кода Хаффмана
        */
        if (!node->m_left && !node->m_right)
        {
            huffmanCode[node->m_char] = str;
        }
 
        /* Рекурсивно обходим левое поддерево, добавляя '0' к текущей строке */
        EncodeHelper(node->m_left, str + "0", huffmanCode);
        /* Рекурсивно обходим правое поддерево, добавляя '1' к текущей строке */
        EncodeHelper(node->m_right, str + "1", huffmanCode);
    }

public:
    /*
    Конструктор класса HuffmanTree, который принимает map символов и их частот
    При создании объекта HuffmanTree, дерево Хаффмана строится автоматически
    */
    HuffmanTree(const std::unordered_map<char, int>& frequencyMap)
    {
        BuildTree(frequencyMap);
    }
 
    /*
    Функция calculateFrequencies считывает файл и вычисляет частоту каждого символа в файле.
    Возвращает map, где ключ - это символ, а значение - его частота
    */
    static std::unordered_map<char, int> calculateFrequencies(std::ifstream& inputFile)
    {
        std::unordered_map<char, int> frequencyMap;
        char m_char;
 
        while (inputFile.get(m_char))
        {
            frequencyMap[m_char]++;
        }
 
        return frequencyMap;
    }
 
    /*
    Функция encode кодирует исходный файл с использованием дерева Хаффмана и записывает результат в выходной файл.
    Возвращает коэффициент сжатия
    */
    double Encode(std::ifstream& inputFile, std::ofstream& outputFile)
    {
        inputFile.clear();                                                          // Сбросить состояние файла
        inputFile.seekg(0);                                                         // Переместить указатель чтения в начало файла
        std::unordered_map<char, int> frequencyMap = calculateFrequencies(inputFile);
        BuildTree(frequencyMap);
 
        inputFile.clear();
        inputFile.seekg(0);
        std::string str((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
 
        std::unordered_map<char, std::string> huffmanCode;
        EncodeHelper(root, "", huffmanCode);
 
        std::string encodedStr;
        for (char m_char : str)
        {
            encodedStr += huffmanCode[m_char];
        }
 
        outputFile << encodedStr;
 
        double compressionRatio = (double)encodedStr.size() / (str.size() * 8);
 
        return compressionRatio;
    }
 
 
    bool Decode(std::ifstream& inputFile, std::ofstream& outputFile)
    {
        /* Считываем закодированную строку из файла */
        std::string encodedStr((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
 
        Node* node = root;
        std::string decodedStr;
 
        /* Декодируем строку */
        for (char bit : encodedStr)
        {
            if (bit == '0')
            {
                node = node->m_left;
            }
            else if (bit == '1')
            {
                node = node->m_right;
            }
 
            /* Если достигли листа, добавляем символ в декодированную строку и возвращаемся к корню */
            if (node->m_left == nullptr && node->m_right == nullptr)
            {
                decodedStr += node->m_char;
                node = root;
            }
        }
 
        /* Записываем декодированную строку в выходной файл */
        outputFile << decodedStr;
 
        return true;
    }
};
 
 
void WriteEncodedFile(std::ofstream& outputFile, const std::string& encodedStr)
{
    outputFile << encodedStr;
}
 
int main()
{
    setlocale(LC_ALL, "Russian");
    std::ifstream inputFile("input.txt");
    std::unordered_map<char, int> freqMap = HuffmanTree::calculateFrequencies(inputFile);
    HuffmanTree huffmanTree(freqMap);
 
    std::ofstream encodedFile("encoded.txt");
    double compressionRatio = huffmanTree.Encode(inputFile, encodedFile);
    std::cout << "Степень сжатия: " << compressionRatio << std::endl;
 
    inputFile.close();
    encodedFile.close();
 
    std::ifstream encodedInputFile("encoded.txt");
    std::ofstream decodedFile("decoded.txt");
    bool isDecoded = huffmanTree.Decode(encodedInputFile, decodedFile);
    std::cout << "Декодированние прошло " << (isDecoded ? "успешно" : "неудачно") << std::endl;
 
    encodedInputFile.close();
    decodedFile.close();
 
    return 0;
}