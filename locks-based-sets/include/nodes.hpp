#ifndef NODES_H
#define NODES_H

template<class T>
class Node {
public:
    Node(T value, Node* next) {
        this->hash = std::hash<T>()(value);
        this->value = value;
        this->next = next;
    }

    ~Node() {
        delete next;
    }

public:
    std::size_t hash;
    T value;
    Node* next;
};

template<class T>
class NodeWithMutex {
public:
    NodeWithMutex(T value, NodeWithMutex* next) {
        this->hash = std::hash<T>()(value);
        this->value = value;
        this->next = next;
    }

    ~NodeWithMutex() {
        delete next;
    }

public:
    std::size_t hash;
    T value;
    NodeWithMutex* next;
    std::mutex mtx;
};

#endif // NODES_H
