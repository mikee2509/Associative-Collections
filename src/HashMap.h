#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <functional>
#include <utility>

namespace aisdi
{

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
    class Node;
    using node = Node;

private:
    const static size_type HASH_SIZE = 16000;
    node* table[HASH_SIZE] = { nullptr };
    size_type firstIndex, lastIndex; //for faster iteration
public:
    HashMap(): firstIndex(HASH_SIZE), lastIndex(HASH_SIZE)
    {}

    HashMap(std::initializer_list<value_type> list): HashMap()
    {
        for(value_type v : list)
            (*this)[v.first] = v.second;
    }

    HashMap(const HashMap& other): HashMap()
    {
        for(value_type v : other)
            (*this)[v.first] = v.second;
    }

    HashMap(HashMap&& other)
    {
        moveMap(other);
    }

    ~HashMap()
    {
        emptyMap();
    }

private:
    void emptyMap()
    {
        if(isEmpty()) return;
        node *it, *temp;
        for(size_type i=firstIndex; i<=lastIndex; ++i)
        {
            if(table[i] == nullptr) continue;
            it = table[i];
            while(it!=nullptr)
            {
                temp = it;
                it = it->next;
                delete temp;
            }
        }
        firstIndex = lastIndex = HASH_SIZE;
    }

    void moveMap(HashMap& other)
    {
        firstIndex = other.firstIndex;
        lastIndex = other.lastIndex;
        for(size_type i = firstIndex; i<=lastIndex; ++i)
        {
            table[i] = other.table[i];
            other.table[i] = nullptr;
        }
        other.firstIndex = other.lastIndex = HASH_SIZE;
    }

public:
    HashMap& operator=(const HashMap& other)
    {
        if(&other != this)
        {
            emptyMap();
            for(value_type v : other)
                (*this)[v.first] = v.second;
        }
        return *this;
    }

    HashMap& operator=(HashMap&& other)
    {
        if(&other != this)
        {
            emptyMap();
            moveMap(other);
        }
        return *this;
    }

private:
    //Hash function
    size_type getIndex(const key_type& key) const
    {
        std::hash<key_type> temp;
        return temp(key) % HASH_SIZE;
    }

    //Get pointer to a node with given *key* in bucket number *index*
    // (returns nullptr if the node doesn't exist)
    node* getNode(const size_type &index, const key_type& key) const
    {
        node* temp = table[index];
        while(temp != nullptr)
        {
           if(temp->val.first == key) break;
           temp = temp->next;
        }
        return temp;
    }

    //Insert a node with given *key* in bucket number *index*
    // (returns pointer to the new node)
    node* insertNode(const size_type &index, const key_type& key)
    {
        node* nd = new node(key);
        if(table[index] == nullptr) table[index] = nd;
        else
        {
            node* temp = table[index];
            while(temp->next != nullptr)
                temp = temp->next;
            temp->next = nd;
        }
        return nd;
    }

public:
    bool isEmpty() const
    {
        return firstIndex == HASH_SIZE;
    }

    mapped_type& operator[](const key_type& key)
    {
        size_type index = getIndex(key);
        node* temp = getNode(index, key);

        if(temp != nullptr) return temp->val.second;

        temp = insertNode(index, key);
        if(index < firstIndex) firstIndex = index;
        if(index > lastIndex || lastIndex == HASH_SIZE) lastIndex = index;
        return temp->val.second;
    }

    const mapped_type& valueOf(const key_type& key) const
    {
        node* temp = getNode(getIndex(key), key);
        if(temp == nullptr) throw std::out_of_range("Node with given key doesn't exist");
        return temp->val.second;
    }

    mapped_type& valueOf(const key_type& key)
    {
        node* temp = getNode(getIndex(key), key);
        if(temp == nullptr) throw std::out_of_range("Node with given key doesn't exist");
        return temp->val.second;
    }

    const_iterator find(const key_type& key) const
    {
        size_type in = getIndex(key);
        node* temp = table[in];
        while(temp != nullptr && temp->val.first != key)
            temp = temp->next;
        return temp == nullptr ? cend() : const_iterator(this, temp, in);
    }

    iterator find(const key_type& key)
    {
        size_type in = getIndex(key);
        node* temp = table[in];
        while(temp != nullptr && temp->val.first != key)
            temp = temp->next;
        return temp == nullptr ? end() : iterator(const_iterator(this, temp, in));
    }

    void remove(const key_type& key)
    {
        const_iterator it = find(key);
        remove(it);
    }

    void remove(const const_iterator& it)
    {
        if(it == end()) throw std::out_of_range("Node with given key doesn't exist or iterator is in end position");
        if(table[it.index] == it.current && it.current->next == nullptr) ///Removing the only element in a bucket
        {
            if(it.index == firstIndex)
            {
                if(it.index == lastIndex) //It's the only element in the map, set the hashmap to empty state
                    firstIndex = lastIndex = HASH_SIZE;
                else //It's the first element in the hashmap, set firstIndex to a new position
                {
                    for(size_type i = it.index+1; i <= lastIndex; ++i)
                    {
                        if(table[i] == nullptr) continue;
                        firstIndex = i;
                        break;
                    }
                }
            }
            else if(it.index == lastIndex) //It's the last element in the hashmap, set lastIndex to a new position
            {
                for(size_type i = it.index-1; i >= firstIndex; --i)
                {
                    if(table[i] == nullptr) continue;
                    lastIndex = i;
                    break;
                }
            }

            table[it.index] = nullptr;
        }
        else ///Removing one of few elements in a bucket
        {
            if(table[it.index] == it.current) //It's the first element in the bucket
                table[it.index] = it.current->next;
            else
            {
                node* temp = table[it.index];
                while(temp->next != it.current)
                    temp = temp->next;
                if(it.current->next == nullptr) temp->next = nullptr;
                else temp->next = it.current->next;
            }
        }

        delete it.current;
    }

    size_type getSize() const
    {
        if(isEmpty()) return 0;
        size_type count = 0;
        node* temp;
        for(size_type i = firstIndex; i<=lastIndex; ++i)
        {
            if(table[i] == nullptr) continue;
            ++count;
            temp = table[i];
            while(temp->next != nullptr)
            {
                ++count;
                temp = temp->next;
            }
        }
        return count;
    }

    bool operator==(const HashMap& other) const
    {
        if(getSize() != other.getSize()) return false;
        const_iterator temp(this);
        for(value_type v : other)
        {
            temp = find(v.first);
            if(temp == other.end()) return false;
            if(temp->second != v.second) return false;
        }
        return true;
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
        return const_iterator(this, nullptr, HASH_SIZE);
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
class HashMap<KeyType, ValueType>::Node
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
    using node = HashMap<KeyType, ValueType>::Node;
    using hash_map = HashMap<KeyType, ValueType>;

private:
    node* current;
    size_type index;
    const hash_map* parent_map;

public:
    ConstIterator(const hash_map* p): current(nullptr), index(HASH_SIZE), parent_map(p)
    {}

    explicit ConstIterator(const hash_map* p, node* n, size_type in): current(n), index(in), parent_map(p)
    {}

    ConstIterator(const ConstIterator& other): current(other.current), index(other.index), parent_map(other.parent_map)
    {}

    ConstIterator& operator=(const ConstIterator &other)
    {
        if(&other != this)
        {
            current = other.current;
            index = other.index;
            parent_map = other.parent_map;
        }
        return *this;
    }

    ConstIterator& operator++()
    {
        if(index == HASH_SIZE) throw std::out_of_range("Cannot increment iterator");
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
        if(temp == nullptr) index = HASH_SIZE;
        current = temp;
        return *this;
    }

    ConstIterator operator++(int)
    {
        ConstIterator temp = *this;
        ++(*this);
        return temp;
    }

    ConstIterator& operator--()
    {
        if(*this == parent_map->begin()) throw std::out_of_range("Cannot decrement iterator");
        node* temp;
        if(index == HASH_SIZE)
        {
            index = parent_map->lastIndex;
            temp = parent_map->table[index];
            while(temp->next != nullptr)
                temp = temp->next;
            current = temp;
            return *this;
        }
        if(parent_map->table[index] != current)
        {
            temp = parent_map->table[index];
            while(temp->next != current)
                temp = temp->next;
            current = temp;
            return *this;
        }
        for(size_type i = index-1; i >= parent_map->firstIndex; --i)
        {
            if(parent_map->table[i] == nullptr) continue;
            temp = parent_map->table[i];
            while(temp->next != nullptr)
                temp = temp->next;
            current = temp;
            index = i;
            break;
        }
        return *this;
    }

    ConstIterator operator--(int)
    {
        ConstIterator temp = *this;
        --(*this);
        return temp;
    }

    reference operator*() const
    {
        if(index == HASH_SIZE) throw std::out_of_range("Iterator points at empty space after the last element");
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
        return current == other.current && (index != HASH_SIZE)^(other.index == HASH_SIZE);
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
