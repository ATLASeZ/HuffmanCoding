#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <string>
#include <algorithm>
#include <bitset>

class HuffmanTree
{
public:
    class Node;

    HuffmanTree();

    ~HuffmanTree();

    void BuildHuffmanTree(const std::string& text);

    std::vector<uint8_t> Encode(char symbol) const;
    std::pair<std::vector<uint8_t>, double> Encode(const std::string& text) const;

    std::string Decode(const std::vector<uint8_t>& encodedData) const;

    void WriteEncodedTree() const;

    void ReadEncodedTree();

    size_t GetEncodedTreeSize() const;

private:
    Node* m_root = nullptr;

    void DestructorAuxiliary(Node* node);

    void EncodeAuxiliary(Node* node, char symbol, std::vector<uint8_t>& encodedSymbol) const;

    void WriteEncodedTreeAuxiliary(const Node* node, std::ofstream& outputFile) const;

    HuffmanTree::Node* ReadEncodedTreeAuxiliary(std::ifstream& inputFile);
};

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

HuffmanTree::HuffmanTree()
{
    m_root = nullptr;
}

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

    while (nodeList.size() != 1)
    {
        nodeList.sort([](Node* left, Node* right)
            {
                return left->m_frequency < right->m_frequency;
            });

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

std::vector<uint8_t> HuffmanTree::Encode(char symbol) const
{
    std::vector<uint8_t> encodedSymbol;
    EncodeAuxiliary(m_root, symbol, encodedSymbol);

    return encodedSymbol;
}

std::pair<std::vector<uint8_t>, double> HuffmanTree::Encode(const std::string& text) const
{
    std::vector<uint8_t> encodedText;

    for (char huffmanChar : text)
    {
        std::vector<uint8_t> encodedSymbol = Encode(huffmanChar);
        encodedText.insert(encodedText.end(), encodedSymbol.begin(), encodedSymbol.end());
    }

    double compressionRatio = (static_cast<double>(text.size()) * 8) / encodedText.size();

    return std::make_pair(encodedText, compressionRatio);
}

void HuffmanTree::EncodeAuxiliary(Node* node, char symbol, std::vector<uint8_t>& encodedSymbol) const
{
    if (!node)
    {
        return;
    }

    if (node->m_char == symbol)
    {
        return;
    }

    EncodeAuxiliary(node->m_left, symbol, encodedSymbol);

    if (encodedSymbol.empty())
    {
        encodedSymbol.push_back(0);
    }
    else
    {
        encodedSymbol.back() <<= 1;
    }

    EncodeAuxiliary(node->m_right, symbol, encodedSymbol);

    if (encodedSymbol.empty())
    {
        encodedSymbol.push_back(1);
    }
    else
    {
        encodedSymbol.back() |= 1;
    }
}

std::string HuffmanTree::Decode(const std::vector<uint8_t>& encodedData) const
{
    std::string decodedText = "";
    Node* currentNode = m_root;

    for (uint8_t encodedByte : encodedData)
    {
        for (int i = 7; i >= 0; --i)
        {
            bool bit = (encodedByte >> i) & 1;

            if (bit)
            {
                currentNode = currentNode->m_right;
            }
            else
            {
                currentNode = currentNode->m_left;
            }

            if (!currentNode->m_left && !currentNode->m_right)
            {
                decodedText += currentNode->m_char;
                currentNode = m_root;
            }
        }
    }

    return decodedText;
}

void HuffmanTree::WriteEncodedTree() const
{
    std::ofstream outputFile("encoded.txt", std::ios::binary);

    WriteEncodedTreeAuxiliary(m_root, outputFile);

    std::pair<std::vector<uint8_t>, double> encodedData = Encode("Message: your text here");
    outputFile.write(reinterpret_cast<const char*>(encodedData.first.data()), encodedData.first.size());

    outputFile.close();
}

void HuffmanTree::WriteEncodedTreeAuxiliary(const Node* node, std::ofstream& outputFile) const
{
    if (node == nullptr)
    {
        outputFile.put('0');
        return;
    }

    outputFile.put('1');

    outputFile.write(reinterpret_cast<const char*>(&node->m_char), sizeof(char));
    outputFile.write(reinterpret_cast<const char*>(&node->m_frequency), sizeof(int));

    WriteEncodedTreeAuxiliary(node->m_left, outputFile);
    WriteEncodedTreeAuxiliary(node->m_right, outputFile);
}

void HuffmanTree::ReadEncodedTree()
{
    std::ifstream inputFile("encoded.txt", std::ios::binary);

    m_root = ReadEncodedTreeAuxiliary(inputFile);

    inputFile.close();
}

HuffmanTree::Node* HuffmanTree::ReadEncodedTreeAuxiliary(std::ifstream& inputFile)
{
    char marker;
    inputFile.get(marker);

    if (marker == '0')
    {
        return nullptr;
    }

    char huffmanChar;
    int frequency;
    inputFile.read(reinterpret_cast<char*>(&huffmanChar), sizeof(char));
    inputFile.read(reinterpret_cast<char*>(&frequency), sizeof(int));

    Node* left = ReadEncodedTreeAuxiliary(inputFile);
    Node* right = ReadEncodedTreeAuxiliary(inputFile);

    return new Node(huffmanChar, frequency, left, right);
}

size_t HuffmanTree::GetEncodedTreeSize() const
{
    std::vector<uint8_t> encodedTree;
    WriteEncodedTreeAuxiliary(m_root, encodedTree);

    return encodedTree.size();
}

int main()
{
    setlocale(LC_ALL, "Rus");

    HuffmanTree huffmanTree;
    std::string text;

    std::ifstream inputFile("input.txt");

    if (inputFile.is_open())
    {
        std::getline(inputFile, text);
        inputFile.close();
    }
    else
    {
        std::cout << "Ошибка при открытии файла input.txt";
        return 1;
    }

    huffmanTree.BuildHuffmanTree(text);

    int conquerorMode;
    std::cout << "Выберите режим:";
    std::cout << "1. Кодирование";
    std::cout << "2. Декодирование";
    std::cin >> conquerorMode;

    if (conquerorMode == 1)
    {
        std::pair<std::vector<uint8_t>, double> encodedData = huffmanTree.Encode(text);

        std::ofstream encodedFile("encoded.txt", std::ios::binary);

        if (encodedFile.is_open())
        {
            huffmanTree.WriteEncodedTree();

            for (const uint8_t& byte : encodedData.first)
            {
                encodedFile.write(reinterpret_cast<const char*>(&byte), sizeof(uint8_t));
            }

            encodedFile.close();
        }
        else
        {
            std::cout << "Ошибка при открытии файла encoded.txt";

            return 1;
        }
    }
    else if (conquerorMode == 2)
    {
        std::ifstream encodedFile("encoded.txt", std::ios::binary);

        if (encodedFile.is_open())
        {
            huffmanTree.ReadEncodedTree();
            encodedFile.close();
        }
        else
        {
            std::cout << "Ошибка при открытии файла encoded.txt";

            return 1;
        }

        std::vector<uint8_t> encodedData;
        std::ifstream encodedDataFile("encoded.txt", std::ios::binary);

        if (encodedDataFile.is_open())
        {
            encodedDataFile.seekg(huffmanTree.GetEncodedTreeSize(), std::ios::beg);

            uint8_t byte;

            while (encodedDataFile.read(reinterpret_cast<char*>(&byte), sizeof(uint8_t)))
            {
                encodedData.push_back(byte);
            }

            encodedDataFile.close();
        }
        else
        {
            std::cout << "Ошибка при открытии файла encoded.txt";

            return 1;
        }

        std::string decodedText = huffmanTree.Decode(encodedData);

        std::ofstream decodedFile("decoded.txt");

        if (decodedFile.is_open())
        {
            decodedFile << decodedText;
            decodedFile.close();
        }
        else
        {
            std::cout << "Ошибка при открытии файла decoded.txt";

            return 1;
        }
    }
    else
    {
        std::cout << "Некорректный выбор режима";
        return 1;
    }

    std::cout << "Операция выполнена успешно!";

    return 0;
}