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

#endif // LISTS_H
