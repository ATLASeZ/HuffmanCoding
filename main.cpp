#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <string>
#include <algorithm>

class HuffmanTree
{
public:
    class Node;                                                                                         // Класс "Узел"

    HuffmanTree();                                                                                      // Конструктор

    ~HuffmanTree();                                                                                     // Деструктор

    void BuildHuffmanTree(const std::string& text);                                                     // Построение дерева Хаффмана

    std::string Encode(char symbol) const;                                                              // Кодирование отдельного символа

    std::pair<std::string, double> Encode(const std::string& text) const;                               // Кодирование текста

    std::string Decode(const std::string& text) const;                                                  // Декодирование текста

private:
    Node* m_root = nullptr;

    void DestructorAuxiliary(Node* node);                                                               // Удаление дерева

    void EncodeAuxiliary(Node* node, char symbol, std::string currentCode, std::string& encodedSymbol) const;
};

/* Класс "Узел" */
class HuffmanTree::Node
{
public:
    char m_char;
    int m_frequency;
    Node* m_left;
    Node* m_right;

    Node(char huffmanChar, int frequency, Node* m_left = nullptr, Node* m_right = nullptr)
        : m_char(huffmanChar), m_frequency(frequency), m_left(m_left), m_right(m_right) {}

};

/* Конструктор */
HuffmanTree::HuffmanTree()
{
    m_root = nullptr;
}

/* Деструктор */
HuffmanTree::~HuffmanTree()
{
    DestructorAuxiliary(m_root);
}

void HuffmanTree::DestructorAuxiliary(Node* node)
{
    if (node)
    {
        DestructorAuxiliary(node->m_left);
        DestructorAuxiliary(node->m_right);

        delete node;
    }
}

/* Построение дерева Хаффмана */
void HuffmanTree::BuildHuffmanTree(const std::string& text)
{
    std::unordered_map<char, int> frequencyMap;

    for (char huffmanChar : text)
    {
        if (frequencyMap.count(huffmanChar) == 0)
        {
            frequencyMap[huffmanChar] = 1;
        }
        else
        {
            frequencyMap[huffmanChar]++;
        }
    }

    std::list<Node*> nodeList;

    for (auto& element : frequencyMap)
    {
        nodeList.push_back(new Node(element.first, element.second));
    }

    /* Сортируем по возрастанию частоты символов в узлах */
    while (nodeList.size() != 1)
    {
        nodeList.sort([](Node* left, Node* right)
            {
                return left->m_frequency < right->m_frequency;
            });

        /* Извлекаем два узла с наименьшей частотой из начала списка */
        Node* node1 = nodeList.front();
        nodeList.pop_front();
        Node* node2 = nodeList.front();
        nodeList.pop_front();

        Node* newNode = new Node('\0', node1->m_frequency + node2->m_frequency, node1, node2);
        nodeList.push_back(newNode);
    }

    m_root = nodeList.front();
    nodeList.pop_front();
}

/* Кодирование отдельного символа, текста */
std::string HuffmanTree::Encode(char symbol) const
{
    std::string encodedSymbol = "";
    EncodeAuxiliary(m_root, symbol, "", encodedSymbol);

    return encodedSymbol;
}

std::pair<std::string, double> HuffmanTree::Encode(const std::string& text) const
{
    std::string encodedText = "";

    for (char huffmanChar : text)
    {
        encodedText += Encode(huffmanChar);
    }

    double compressioRatio = (static_cast<double>(text.size()) * 8) / encodedText.size();

    return std::make_pair(encodedText, compressioRatio);
}

void HuffmanTree::EncodeAuxiliary(Node* node, char symbol, std::string currentCode, std::string& encodedSymbol) const
{
    if (!node)
    {
        return;
    }

    if (node->m_char == symbol)
    {
        encodedSymbol = currentCode;

        return;
    }

    EncodeAuxiliary(node->m_left, symbol, currentCode + "0", encodedSymbol);
    EncodeAuxiliary(node->m_right, symbol, currentCode + "1", encodedSymbol);
}

/* Декодирование текста */
std::string HuffmanTree::Decode(const std::string& text) const
{
    std::string decodedText = "";
    Node* currentNode = m_root;

    for (char huffmanChar : text)
    {
        if (huffmanChar == '0')
        {
            currentNode = currentNode->m_left;
        }
        else
        {
            currentNode = currentNode->m_right;
        }

        if (!currentNode->m_left && !currentNode->m_right)
        {
            decodedText += currentNode->m_char;
            currentNode = m_root;
        }
    }

    return decodedText;
}

int main()
{
    setlocale(LC_ALL, "Russian");

    std::ifstream inputFile("input.txt");
    std::string text((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    HuffmanTree labHuffmanTree;
    labHuffmanTree.BuildHuffmanTree(text);

    auto result = labHuffmanTree.Encode(text);
    std::string encodedText = result.first;
    double compressionRatio = result.second;
    std::cout << "Коэффициент сжатия: " << compressionRatio << std::endl;

    std::ofstream encodedFile("encoded.txt");
    encodedFile << encodedText;
    encodedFile.close();

    std::ifstream encodedInputFile("encoded.txt");
    std::string encodedInputText((std::istreambuf_iterator<char>(encodedInputFile)), std::istreambuf_iterator<char>());

    std::string decodedText = labHuffmanTree.Decode(encodedInputText);
    std::ofstream decodedFile("decoded.txt");
    decodedFile << decodedText;
    decodedFile.close();

    std::cout << "Декодирование прошло " << ((text == decodedText) ? "успешно" : "неудачно") << std::endl;

    return 0;
}