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

#endif // NODES_H
