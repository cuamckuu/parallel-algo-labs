#ifndef NODES_H
#define NODES_H

#include <memory>

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
    NodeWithMutex(T value, NodeWithMutex<T>* next) {
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
    NodeWithMutex<T>* next;
    std::mutex mtx;
};

template<class T>
class SmartNodeWithMutex {
public:
    SmartNodeWithMutex(T value, SmartNodeWithMutex<T>* next) {
        this->hash = std::hash<T>()(value);
        this->value = value;
        this->next = std::shared_ptr<SmartNodeWithMutex<T>>(next);
    }

public:
    std::size_t hash;
    T value;
    std::shared_ptr<SmartNodeWithMutex<T>> next;
    std::mutex mtx;
};

#endif // NODES_H
