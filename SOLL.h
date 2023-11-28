#ifndef SOLL_H
#define SOLL_H

#include <iostream>
#include <stdexcept>
#include <initializer_list>

template <typename T>
class SOLL {
public:
    struct Node {
        Node() :  value{} , next{}, prev{}, asc{}, desc{} {}
        explicit Node(const T& v) : value(v) , next{}, prev{}, asc{}, desc{} {}
        Node* next;
        Node* prev;
        Node* asc;
        Node* desc;
        T value;
    };
    class Iterator {
    public:
        Iterator() : m_current{} {}
        explicit Iterator(Node* ptr) : m_current(ptr) {}
        T& operator*() { return m_current->value; }
        const T& operator*() const { return m_current->value; }
        T* operator->() { return &(m_current->value); }
        const T* operator->() const { return &(m_current->value); }
        Iterator& operator++() {
            m_current = m_current->next;
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp(m_current);
            m_current = m_current->next;
            return tmp;
        }
        Iterator& operator--() {
            m_current = m_current->prev;
            return *this;
        }
        Iterator operator--(int) {
            Iterator tmp(m_current);
            m_current = m_current->prev;
            return tmp;
        }
        bool operator==(const Iterator& rhs) { return m_current == rhs.m_current; }
        bool operator!=(const Iterator& rhs) { return m_current != rhs.m_current; }
    private:
        Node* m_current;    
    };
    class AscIterator {
    public:
        AscIterator() : m_current{} {}
        explicit AscIterator(Node* ptr) : m_current(ptr) {}
        T& operator*() { return m_current->value; }
        const T& operator*() const { return m_current->value; }
        T* operator->() { return &(m_current->value); }
        const T* operator->() const { return &(m_current->value); }
        AscIterator& operator++() {
            m_current = m_current->asc;
            return *this;
        }
        AscIterator operator++(int) {
            Iterator tmp(m_current);
            m_current = m_current->asc;
            return tmp;
        }
        AscIterator& operator--() {
            m_current = m_current->desc;
            return *this;
        }
        AscIterator operator--(int) {
            Iterator tmp(m_current);
            m_current = m_current->desc;
            return tmp;
        }
        bool operator==(const AscIterator& rhs) { return m_current == rhs.m_current; }
        bool operator!=(const AscIterator& rhs) { return m_current != rhs.m_current; }
    private:
        Node* m_current;    
    };
    SOLL() : m_head{}, m_tail{}, m_sortHead{}, m_sortTail{}, m_size{} {}
    SOLL(size_t count, const T& value) : SOLL() {
        for (int i = 0; i < count; ++i) {
            push_back(value);
        }
    }
    SOLL(const std::initializer_list<T>& list) : SOLL() {
        for (const auto& el : list) {
            push_back(el);
        }   
    }
    SOLL(const SOLL& src) : SOLL() {
        for (const auto& el : src) {
            push_back(el);
        }
    }
    SOLL(SOLL&& src) : SOLL() {
        swap(src);
    }
    SOLL& operator=(const SOLL& rhs) {
        auto tmp = rhs;
        swap(tmp);
        return *this;
    }
    SOLL& operator=(SOLL&& rhs) {
        swap(rhs);
        rhs.clear();
        return *this;
    } 
    void push_back(const T& value) {
        Node* tmp = new Node(value);
        if (!size()) {
            m_head = tmp;
            m_tail = m_head;
        }
        else if (size() == 1) {
            m_tail = tmp;
            m_head->next = m_tail;
            m_tail->prev = m_head;
        }
        else {
            m_tail->next = tmp;
            m_tail->next->prev = m_tail;
            m_tail = m_tail->next;
        }
        putInSortPlace(tmp);
        ++m_size;        
    }
    void push_front(const T& value) {
        if (!size()) {
            push_back(value);
            return;
        }
        Node* tmp = new Node(value);
        if (size() == 1) {
            m_head = tmp;
            m_head->next = m_tail;
            m_tail->prev = m_head;
        }
        else {
            m_head->prev = tmp;
            m_head->prev->next = m_head;
            m_head = m_head->prev;
        }
        putInSortPlace(tmp);
        ++m_size;  
    }
    void insert(size_t pos, const T& value) {
        if (pos < 0 || pos > size()) {
            throw std::out_of_range("fail to insert to list");
        }
        if (pos == 0) {
            push_front(value);
        }
        else if(pos == size()) {
            push_back(value);
        }
        else {
            Node* current = m_head;
            while (pos && current) {
                current = current->next;
                --pos;
            }
            Node* tmp = new Node(value);
            tmp->next = current;
            tmp->prev = current->prev;
            current->prev = tmp;
            tmp->prev->next = tmp;
            putInSortPlace(tmp);
            ++m_size;
        }
    }
    void pop_back() {
        if (!size()) {
            return;
        }
        if (size() == 1) {
            clear();
        }
        else {
           removeFromSortedPlace(m_tail); 
           m_tail = m_tail->prev;
           delete m_tail->next;
           m_tail->next = nullptr;
           --m_size;
        }        
    }
    void pop_front() {
        if (size() <= 1) {
            pop_back();
        }
        else {
            removeFromSortedPlace(m_head); 
            m_head = m_head->next;
            delete m_head->prev;
            m_head->prev = nullptr;
            --m_size;
        }
    }
    void remove(size_t pos) {
        if (pos < 0 || pos >= size()) {
            throw std::out_of_range("fail to remove from list");
        }
        if (pos == 0) {
            pop_front();
        }
        else if(pos == size() - 1) {
            pop_back();
        }
        else {
            Node* current = m_head;
            while (current && pos) {
                current = current->next;
                --pos;
            }
            removeFromSortedPlace(current);
            current->next->prev = current->prev;
            current->prev->next = current->next;
            delete current;
            --m_size;
        }
    }
    void setElement(size_t pos, const T& value) {
            Node* current = nullptr;
            try {
                current = getNode(pos);
                current->value = value;
                removeFromSortedPlace(current);
                current->asc = nullptr;
                current->desc = nullptr;
                --m_size;
                putInSortPlace(current);
                ++m_size;
            }
            catch(std::out_of_range& err) {
                throw std::out_of_range("fail to set element to list");
            }
    }
    const T& getElement(size_t pos) const {     
        return (const_cast<SOLL*>(this))->swapNodeWithPrev(getNode(pos))->value;
    }
    int search(const T& value) const {
        int index = 0;
        Node* current = m_head;
        while (current) {
            if (current->value == value) {
                (const_cast<SOLL*>(this))->swapNodeWithPrev(current)->value;
                if (index) {
                    return --index;
                }
                return index;
            }
            ++index;
            current = current->next;          
        }
        return -1;
    }
    constexpr size_t size() const { return m_size; }
    void print() const {
        auto current = m_head;
        while (current) {
            std::cout << current->value << " ";
            current = current->next;
        }
        std::cout << std::endl;
        
    }
    void printR() const {
        auto current = m_tail;
        while (current) {
            std::cout << current->value << " ";
            current = current->prev;
        }
        std::cout << std::endl;        
    }
    void printAscending() const {
        auto current = m_sortHead;
        while (current) {
            std::cout << current->value << " ";
            current = current->asc;
        }
        std::cout << std::endl;             
    }
    void printDescending() const {
        auto current = m_sortTail;
        while (current) {
            std::cout << current->value << " ";
            current = current->desc;
        }
        std::cout << std::endl;             
    }
    void swap(SOLL& other) {
        std::swap(m_head, other.m_head);
        std::swap(m_tail, other.m_tail);
        std::swap(m_sortHead, other.m_sortHead);
        std::swap(m_sortTail, other.m_sortTail);
        std::swap(m_size, other.m_size);                         
    }
    Iterator begin() { return Iterator(m_head); }
    const Iterator begin() const { return Iterator(m_head); }
    Iterator end() { return Iterator(); }
    const Iterator end() const { return Iterator(); }
    AscIterator AscBegin() { return AscIterator(m_sortHead); }
    const AscIterator AscBegin() const { return AscIterator(m_sortHead); }
    AscIterator AscEnd() { return AscIterator(); }
    const AscIterator AscEnd() const { return AscIterator(); }         
    void clear() {
        Node* current = m_head;
        while (current) {
            Node* tmp = current->next;
            delete current;
            current = tmp;
        }
        m_head = nullptr;
        m_sortHead = nullptr;
        m_tail =  nullptr;
        m_sortTail = nullptr;
        m_size = 0;
    }
    ~SOLL() {
        clear();
    }    
private:
    void push_backS(Node* node) {
        if (!size()) {
            m_sortHead = node;
            m_sortTail = node;
        }
        else if (size() == 1) {
              m_sortTail = node;
              m_sortTail->desc = m_sortHead;
              m_sortHead->asc = m_sortTail;
        }
        else {
              node->desc = m_sortTail;
              m_sortTail->asc = node;
              m_sortTail = m_sortTail->asc;
        }
    }
    void push_frontS(Node* node) {
        if (!size()) {
            push_frontS(node);
        }
        else if (size() == 1) {
           m_sortHead = node;
           m_sortHead->asc = m_sortTail;
           m_sortTail->desc = m_sortHead;
        }
        else {
            m_sortHead->desc = node;
            m_sortHead->desc->asc = m_sortHead;
            m_sortHead = m_sortHead->desc;
        }
    }
    void putInSortPlace(Node* node) {
        if (!size()) {
            push_backS(node);
        }
        else if (size() == 1) {
            if (node->value > m_sortHead->value) {
                push_backS(node);
            }
            else {
                push_frontS(node);
            }
        }
        else {
            Node* current = m_sortHead;
            while (current && current->value < node->value) {
                current = current->asc;
            }
            if (!current) {
                push_backS(node);
            }
            else if (current == m_sortHead) {
                push_frontS(node);
            }
            else {
                node->asc = current;
                node->desc = current->desc;
                current->desc = node;
                node->desc->asc = node;
            }
        }
       
    }
    void pop_frontS() {
        m_sortHead = m_sortHead->asc;
        m_sortHead->desc = nullptr;
    }
    void pop_backS() {
        m_sortTail = m_sortTail->desc;
        m_sortTail->asc = nullptr;
    }
    void removeFromSortedPlace(Node* node) {
        if (!size()) {
            return;
        }
        if (size() == 1) {
            m_sortHead = nullptr;
            m_sortTail = nullptr;
        }
        else if (node == m_sortHead) {
            pop_frontS();
        }
        else if (node == m_sortTail) {
            pop_backS();
        }
        else {
            node->desc->asc = node->asc;
            node->asc->desc = node->desc;
        }
    }
    Node* swapNodeWithPrev(Node* node)  {
        if (node == m_head) {
            return m_head;
        }
        if (size() == 2) {
           m_head->prev = m_tail;
           m_head->next = nullptr;
           m_tail->prev = nullptr;
           m_tail->next = m_head;
           std::swap(m_head, m_tail);
           return m_head;
        }
        if (node == m_head->next) {
            node = m_head->next;
            m_head->next = m_head->next->next;
            m_head->next->prev = m_head;
            m_head->prev = node;
            node->next = m_head;
            node->prev = nullptr;
            m_head = node;
            return m_head;
        }
        if (node == m_tail) {
            node = m_tail->prev;
            m_tail->prev = m_tail->prev->prev;
            m_tail->prev->next = m_tail;
            m_tail->next = node;
            node->prev = m_tail;
            node->next = nullptr;
            m_tail = node;
            return m_tail->prev;
        }
        Node* tmp = node->prev;
        node->prev->next = node->next;
        node->next->prev = node->prev;
        tmp->prev->next = node;
        node->prev = tmp->prev;
        tmp->prev = node;
        node->next = tmp;
        return node;
    }
    Node* getNode(size_t pos) const {
        if (pos < 0 || pos >= size()) {
            throw std::out_of_range("fail to get element from list");
        }
        if (pos == 0) {
            return m_head;
        }
        if (pos == size() - 1) {
           return m_tail;
        }
        else {
            Node* current = m_head;
            while (m_head && pos) {
                current = current->next;
                --pos;
            }
        return current;
        }
    }  
    Node* m_head;
    Node* m_tail;
    Node* m_sortHead;
    Node* m_sortTail;
    size_t m_size;    
};

#endif
