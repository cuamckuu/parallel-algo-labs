#ifndef LISTS_H
#define LISTS_H

#include <mutex>
#include <limits>

#include "nodes.hpp"

template<class T>
class CoarseGrainedSet {
public:
    CoarseGrainedSet() {
        head = new Node<T>(0, nullptr);
        head->hash = std::numeric_limits<size_t>::min();

        head->next = new Node<T>(0, nullptr);
        head->next->hash = std::numeric_limits<size_t>::max();
    }

    ~CoarseGrainedSet() {
        delete head;
    }

    bool add(T value) {
        std::lock_guard<std::mutex> guard(mtx);

        bool res = false;

        Node<T>* prev = head;
        Node<T>* curr = prev->next;
        while(curr->hash < hasher(value)) {
            prev = curr;
            curr = curr->next;
        }

        if(curr->hash == hasher(value)) {
            res = false;
        } else {
            Node<T>* node = new Node<T>(value, curr);
            prev->next = node;
            res = true;
        }

        return res;
    }

    bool remove(T value) {
        std::lock_guard<std::mutex> guard(mtx);

        bool res = false;

        Node<T>* prev = head;
        Node<T>* curr = prev->next;

        while(curr->hash < hasher(value)) {
            prev = curr;
            curr = curr->next;
        }

        if(curr->hash == hasher(value)) {
            prev->next = curr->next;

            curr->next = nullptr;
            delete curr;

            res = true;
        } else {
            res = false;
        }

        return res;
    }

    bool contains(T value) {
        std::lock_guard<std::mutex> guard(mtx);

        bool res = false;

        Node<T>* prev = head;
        Node<T>* curr = prev->next;

        while(curr->hash < hasher(value)) {
            prev = curr;
            curr = curr->next;
        }
        res = (curr->value == value);

        return res;
    }

private:
    Node<T>* head = nullptr;
    std::mutex mtx;
    std::hash<T> hasher;
};

template<class T>
class FineGrainedSet {
public:
    FineGrainedSet() {
        head = new NodeWithMutex<T>(0, nullptr);
        head->hash = std::numeric_limits<size_t>::min();

        head->next = new NodeWithMutex<T>(0, nullptr);
        head->next->hash = std::numeric_limits<size_t>::max();
    }

    ~FineGrainedSet() {
        delete head;
    }

    bool add(T value) {
        bool res = false;

        head->mtx.lock();

        NodeWithMutex<T>* prev = head;
        NodeWithMutex<T>* curr = prev->next;

        curr->mtx.lock();

        while(curr->hash < hasher(value)) {
            prev->mtx.unlock();
            prev = curr;
            curr = curr->next;
            curr->mtx.lock();
        }

        if(curr->hash == hasher(value)) {
            res = false;
        } else {
            NodeWithMutex<T>* node = new NodeWithMutex<T>(value, nullptr);
            node->next = curr;
            prev->next = node;
            res = true;
        }

        curr->mtx.unlock();
        prev->mtx.unlock();

        return res;
    }

    bool remove(T value) {
        bool res = false;

        NodeWithMutex<T>* prev = nullptr;
        NodeWithMutex<T>* curr = nullptr;

        head->mtx.lock();
        prev = head;
        curr = prev->next;
        curr->mtx.lock();

        while(curr->hash < hasher(value)) {
            prev->mtx.unlock();
            prev = curr;
            curr = curr->next;
            curr->mtx.lock();
        }

        if(curr->hash == hasher(value)) {
            prev->next = curr->next;

            curr->mtx.unlock();
            curr->next = nullptr;
            delete curr;

            res = true;
        } else {
            curr->mtx.unlock();
            res = false;
        }
        prev->mtx.unlock();
        return res;
    }

    bool contains(T value) {
        bool res = false;

        NodeWithMutex<T>* prev = nullptr;
        NodeWithMutex<T>* curr = nullptr;

        head->mtx.lock();
        prev = head;
        curr = prev->next;
        curr->mtx.lock();

        while(curr->hash < value) {
            prev->mtx.unlock();
            prev = curr;
            curr = curr->next;
            curr->mtx.lock();
        }

        if(curr->hash == hasher(value)) {
            res = true;
        } else {
            res = false;
        }

        curr->mtx.unlock();
        prev->mtx.unlock();

        return res;
    }

private:
    NodeWithMutex<T>* head = nullptr;
    std::mutex mtx;
    std::hash<T> hasher;
};

template<class T>
class OptimisticSet {
public:
    OptimisticSet() {
        head = new NodeWithMutex<T>(0, nullptr);
        head->hash = std::numeric_limits<size_t>::min();

        head->next = new NodeWithMutex<T>(0, nullptr);
        head->next->hash = std::numeric_limits<size_t>::max();
    }

    ~OptimisticSet() {
        //delete head;
    }

    bool add(T value) {
        while(true) {
            NodeWithMutex<T>* prev = head;
            NodeWithMutex<T>* curr = prev->next;
            while(curr->hash < hasher(value)) {
                prev = curr;
                curr = curr->next;
            }

            std::lock_guard<std::mutex> guard_prev(prev->mtx);
            std::lock_guard<std::mutex> guard_curr(curr->mtx);

            if(validate(prev, curr)) {
                if(curr->hash == hasher(value)) {
                    return false;
                }

                NodeWithMutex<T>* node = new NodeWithMutex<T>(value, nullptr);
                node->next = curr;
                prev->next = node;

                return true;
            }
        }
    }

    bool remove(T value) {
        while(true) {
            NodeWithMutex<T>* prev = head;
            NodeWithMutex<T>* curr = prev->next;

            while(curr->hash < hasher(value)) {
                prev = curr;
                curr = curr->next;
            }

            std::lock_guard<std::mutex> guard_prev(prev->mtx);
            std::lock_guard<std::mutex> guard_curr(curr->mtx);

            if(validate(prev, curr)) {
                if(curr->hash != hasher(value)) {
                    return false;
                }

                prev->next = curr->next;

                // FIXME: Memory leak now, but crash if uncomment. WTF?
                //curr->next = nullptr;
                //delete curr;

                return true;
            }
        }
    }

    bool contains(T value) {
        while(true) {
            NodeWithMutex<T>* prev = head;
            NodeWithMutex<T>* curr = prev->next;

            while(curr->hash < hasher(value)) {
                prev = curr;
                curr = curr->next;
            }

            std::lock_guard<std::mutex> guard_prev(prev->mtx);
            std::lock_guard<std::mutex> guard_curr(curr->mtx);

            if(validate(prev, curr)) {
                return (curr->hash == hasher(value));
            }
        }
    }

private:
    bool validate(NodeWithMutex<T>* prev, NodeWithMutex<T>* curr) {
        NodeWithMutex<T>* node = head;
        while(node->hash <= prev->hash) {
            if(node == prev) {
                return (prev->next == curr);
            }
            node = node->next;
        }
        return false;
    }

private:
    NodeWithMutex<T>* head = nullptr;
    std::mutex mtx;
    std::hash<T> hasher;
};

#endif // LISTS_H
