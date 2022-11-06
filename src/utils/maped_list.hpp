#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

namespace utils
{

/*

Insertion : O(1)
Deletion : O(1)
Access : O(1) amortized

*/

template <typename T1, typename T2>
class MapedDoublyLinkedList
{
  struct DoublyLinkedListNode
  {

    using ListNodePtr = DoublyLinkedListNode *;

  private:
    std::pair<T1, T2> entry;
    ListNodePtr next;
    ListNodePtr prev;

    void set_next(const ListNodePtr &_next) { next = _next; }

    void set_prev(const ListNodePtr &_prev) { prev = _prev; }

  public:
    DoublyLinkedListNode() = default;

    DoublyLinkedListNode(const std::pair<T1, T2> &_entry) : entry(_entry), next(nullptr), prev(nullptr) {}

    DoublyLinkedListNode(const std::pair<T1, T2> &_entry, DoublyLinkedListNode *_next, DoublyLinkedListNode *_prev)
      : entry(_entry), next(_next), prev(_prev)
    {}

    ~DoublyLinkedListNode() {}

    /* Returns true if the current node has a next one, this will return true unless the node is a tail node*/
    bool has_next() { return next != nullptr; }

    /* Returns true if the current node has a previous one, this will return true unless the node is head node*/
    bool has_prev() { return prev != nullptr; }

    DoublyLinkedListNode(const DoublyLinkedListNode &copy_node) {}

    DoublyLinkedListNode &operator=(const DoublyLinkedListNode &copy_node) = default;

    DoublyLinkedListNode(DoublyLinkedListNode &&move_node) = default;

    DoublyLinkedListNode &operator=(DoublyLinkedListNode &&move_node) = default;

    const std::pair<T1, T2> &get_entry() const { return entry; }

    /* returns next node pointer if it exist, otherwise it return nullptr */
    ListNodePtr get_next()
    {
      if (has_next())
        return next;
      else
        return nullptr;
    }

    /* returns previous node pointer if it exist, otherwise it return nullptr */
    ListNodePtr get_prev()
    {
      if (has_prev())
        return prev;
      else
        return nullptr;
    }

    friend class MapedDoublyLinkedList;
  };

private:
  using ListNodePtr = DoublyLinkedListNode *;

  ListNodePtr head;

  ListNodePtr tail;

  size_t _size = 0;

  std::unordered_map<T1, ListNodePtr> pointers_map;

public:
  MapedDoublyLinkedList() : head(nullptr), tail(nullptr) {}

  MapedDoublyLinkedList(const MapedDoublyLinkedList &copy) = default;

  MapedDoublyLinkedList &operator=(const MapedDoublyLinkedList &) = default;

  ~MapedDoublyLinkedList() {}

  /* clear() function is used to free allocated memory, this function must be called only by the owner of the
   * MapDoublyLinkedList object */
  void clear()
  {
    for (DoublyLinkedListNode *current = head, *next; current;)
    {
      next = current->next;
      delete current;
      current = next;
    }
  }

  /* Retruns true if the list is empty */
  bool is_empty() const { return _size == 0; }

  /* Return size of the list */
  size_t size() const { return _size; }

  /* Insert an element/node at the end of list */
  void push_back(const std::pair<T1, T2> &entry)
  {
    ListNodePtr new_node = new DoublyLinkedListNode(entry);
    if (is_empty())
    {
      head = new_node;
      tail = new_node;
    }
    else
    {
      new_node->set_prev(tail);
      tail->set_next(new_node);
      tail = new_node;
    }
    pointers_map.insert({entry.first, tail});
    _size++;
  };

  /* Insert an element/node at the front of the list */
  void push_front(const std::pair<T1, T2> &entry)
  {
    if (is_empty())
    {
      push_back(entry);
    }
    else
    {
      ListNodePtr new_node = new DoublyLinkedListNode(new_node);
      new_node->set_next(head);
      head->set_prev(new_node);
      head = new_node;
    }
    pointers_map.insert({entry.first, head});
    _size++;
  }

  /* Erase an element/node from the list with a given key */
  void erase_element(const T1 &key)
  {
    auto it = pointers_map.find(key);

    if (it != pointers_map.end())
    {
      ListNodePtr target_node_ptr = it->second;
      if (target_node_ptr != nullptr)
      {

        ListNodePtr prev_node_ptr_tmp = target_node_ptr->prev;
        ListNodePtr next_node_ptr_tmp = target_node_ptr->next;

        if (prev_node_ptr_tmp != nullptr)
        {
          prev_node_ptr_tmp->next = next_node_ptr_tmp;
        }
        if (next_node_ptr_tmp != nullptr)
        {
          next_node_ptr_tmp->prev = prev_node_ptr_tmp;
        }

        if (head == target_node_ptr)
          head = next_node_ptr_tmp;

        delete target_node_ptr;
      }
      pointers_map.erase(it);
      _size--;
    }
  }

  /* Return entry/data stored in the back node */
  const std::pair<T1, T2> &back_entry() const
  {
    if (is_empty())
    {
      throw("empty container\n");
    }

    return tail->entry;
  }

  /* Return entry/data stored in the front node */
  const std::pair<T1, T2> &front_entry() const
  {
    if (is_empty())
    {
      throw("empty container\n");
    }

    return head->entry;
  }

  /* Returns pointer to the front node */
  ListNodePtr front_pointer() const { return head; }

  /* Returns pointer to the back node */
  ListNodePtr back_pointer() const { return tail; }

  void traverse()
  {
    ListNodePtr traversal_ptr = head;
    while (traversal_ptr != nullptr)
    {
      std::cout << traversal_ptr->entry.first << "\n";
      traversal_ptr = traversal_ptr->next;
    }
  }
};
} // namespace utils
