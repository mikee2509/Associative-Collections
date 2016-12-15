#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <functional>
#include <utility>

#define HASH_SIZE 16000

namespace aisdi
{
template <typename KeyType, typename ValueType>
class Node;

template <typename KeyType, typename ValueType>
class HashMap
{
public:
    using key_type = KeyType;
    using mapped_type = ValueType;
    using value_type = std::pair<const key_type, mapped_type>;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;

    class ConstIterator;
    class Iterator;
    using iterator = Iterator;
    using const_iterator = ConstIterator;
    using node = Node<KeyType, ValueType>;

private:
    node* table[HASH_SIZE] = { nullptr };
    size_type firstIndex, lastIndex; //for faster iteration
public:
    HashMap(): firstIndex(HASH_SIZE), lastIndex(HASH_SIZE)
    {}

    HashMap(std::initializer_list<value_type> list)
    {
        (void)list; // disables "unused argument" warning, can be removed when method is implemented.
        throw std::runtime_error("TODO");
    }

    HashMap(const HashMap& other)
    {
        (void)other;
        throw std::runtime_error("TODO");
    }

    HashMap(HashMap&& other)
    {
        (void)other;
        throw std::runtime_error("TODO");
    }

//    ~HashMap()
//    {
//        node *it, *temp;
//        for(size_type i=firstIndex; i<=lastIndex; ++i)
//        {
//            if(table[i] == nullptr) continue;
//            it = table[i];
//            while(it!=nullptr)
//            {
//                temp = it;
//                it = it->next;
//                delete temp;
//            }
//        }
//    }

    HashMap& operator=(const HashMap& other)
    {
        (void)other;
        throw std::runtime_error("TODO");
    }

    HashMap& operator=(HashMap&& other)
    {
        (void)other;
        throw std::runtime_error("TODO");
    }

    size_type getIndex(const key_type& key)
    {
        std::hash<key_type> temp;
        return temp(key) % HASH_SIZE;
    }

    bool isEmpty() const
    {
        return firstIndex == HASH_SIZE;
    }

    node* getNode(const size_type &index, const key_type& key)
    {
        node* temp = table[index];
        while(temp != nullptr)
        {
           if(temp->val.first == key) break;
           temp = temp->next;
        }
        return temp;
    }

    void insertNode(const size_type &index, node *&nd)
    {
        if(table[index] == nullptr) table[index] = nd;
        else
        {
            node* temp = table[index];
            while(temp->next != nullptr)
                temp = temp->next;
            temp->next = nd;
        }
    }

    mapped_type& operator[](const key_type& key)
    {
        size_type index = getIndex(key);
        node* temp = getNode(index, key);

        if(temp != nullptr) return temp->val.second;

        temp = new node(key);
        insertNode(index, temp);
        if(index < firstIndex) firstIndex = index;
        if(index > lastIndex || lastIndex == HASH_SIZE) lastIndex = index;
        return temp->val.second;
    }

    const mapped_type& valueOf(const key_type& key) const
    {
        (void)key;
        throw std::runtime_error("TODO");
    }

    mapped_type& valueOf(const key_type& key)
    {
        (void)key;
        throw std::runtime_error("TODO");
    }

    const_iterator find(const key_type& key) const
    {
        (void)key;
        throw std::runtime_error("TODO");
    }

    iterator find(const key_type& key)
    {
        (void)key;
        throw std::runtime_error("TODO");
    }

    void remove(const key_type& key)
    {
        (void)key;
        throw std::runtime_error("TODO");
    }

    void remove(const const_iterator& it)
    {
        (void)it;
        throw std::runtime_error("TODO");
    }

    size_type getSize() const
    {
        throw std::runtime_error("TODO");
    }

    bool operator==(const HashMap& other) const
    {
        (void)other;
        throw std::runtime_error("TODO");
    }

    bool operator!=(const HashMap& other) const
    {
        return !(*this == other);
    }

    iterator begin()
    {
        return iterator(cbegin());
    }

    iterator end()
    {
        return iterator(cend());
    }

    const_iterator cbegin() const
    {
        if(isEmpty()) return cend();
        else return const_iterator(this, table[firstIndex], firstIndex);
    }

    const_iterator cend() const
    {
        return const_iterator(this, nullptr, HASH_SIZE, true);
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator end() const
    {
        return cend();
    }
};

template <typename KeyType, typename ValueType>
class Node
{
public:
    friend class HashMap<KeyType, ValueType>;
    using key_type = KeyType;
    using mapped_type = ValueType;
    using value_type = std::pair<const key_type, mapped_type>;
    using size_type = std::size_t;
    using node = Node;

private:
    value_type val;
    node *next;

public:
    Node(): next(nullptr)
    {}

    Node(value_type v, node* n = nullptr) : val(v), next(n)
    {}

    Node(key_type key, node* n = nullptr): val(value_type(key, mapped_type())), next(n)
    {}

    ~Node()
    {
        next = nullptr;
    }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
    friend class HashMap<KeyType, ValueType>;
    using reference = typename HashMap::const_reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename HashMap::value_type;
    using pointer = const typename HashMap::value_type*;
    using node = Node<KeyType, ValueType>;
    using hash_map = HashMap<KeyType, ValueType>;

private:
    node* current;
    size_type index;
    const hash_map* parent_map;
    bool isInEndPosition;

public:
    explicit ConstIterator(const hash_map* p, node* n, size_type in, bool is = false): current(n), index(in),
        parent_map(p), isInEndPosition(is)
    {}

    ConstIterator(const ConstIterator& other): current(other.current), index(other.index),
        parent_map(other.parent_map), isInEndPosition(other.isInEndPosition)
    {}

    ConstIterator& operator++()
    {
        if(isInEndPosition) throw std::out_of_range("Cannot increment iterator");
        if(current->next != nullptr)
        {
            current = current->next;
            return *this;
        }
        node* temp = nullptr;
        for(size_type i = index+1; i <= parent_map->lastIndex; ++i)
        {
            if(parent_map->table[i] == nullptr) continue;
            temp = parent_map->table[i];
            index = i;
            break;
        }
        if(temp == nullptr)
        {
            current = nullptr;
            isInEndPosition = true;
            index = HASH_SIZE; ///TODO: check whether isInEndPosition is redundant
            return *this;
        }
        else
        {
            current = temp;
            return *this;
        }
    }

    ConstIterator operator++(int)
    {
        ConstIterator temp = *this;
        ++(*this);
        return temp;
    }

    ConstIterator& operator--()
    {
        throw std::runtime_error("TODO");
    }

    ConstIterator operator--(int)
    {
        ConstIterator temp = *this;
        --(*this);
        return temp;
    }

    reference operator*() const
    {
        if(*this == parent_map->end()) throw std::out_of_range("Iterator points at empty space after the last element");
        return current->val;
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    bool operator==(const ConstIterator& other) const
    {
        if(parent_map != other.parent_map) return false;
        //Check if *current* pointers match and if both iterators are or aren't in the end position simultaneously
        return current == other.current && ~isInEndPosition^other.isInEndPosition;
    }

    bool operator!=(const ConstIterator& other) const
    {
        return !(*this == other);
    }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
    using reference = typename HashMap::reference;
    using pointer = typename HashMap::value_type*;

    explicit Iterator()
    {}

    Iterator(const ConstIterator& other)
        : ConstIterator(other)
    {}

    Iterator& operator++()
    {
        ConstIterator::operator++();
        return *this;
    }

    Iterator operator++(int)
    {
        auto result = *this;
        ConstIterator::operator++();
        return result;
    }

    Iterator& operator--()
    {
        ConstIterator::operator--();
        return *this;
    }

    Iterator operator--(int)
    {
        auto result = *this;
        ConstIterator::operator--();
        return result;
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    reference operator*() const
    {
        // ugly cast, yet reduces code duplication.
        return const_cast<reference>(ConstIterator::operator*());
    }
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
