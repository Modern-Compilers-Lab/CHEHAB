#pragma once

#include <memory>
#include <optional>
#include <utility>

namespace utils
{

template <typename T>
class DoublyLinkedList
{

private:
  struct DoublyLinkedListNode
  {

    using ListNodePtr = DoublyLinkedListNode *;

  private:
    T data;
    ListNodePtr next;
    ListNodePtr prev;

    void set_next(const ListNodePtr &_next) { next = _next; }

    void set_prev(const ListNodePtr &_prev) { prev = _prev; }

  public:
    DoublyLinkedListNode() = default;

    DoublyLinkedListNode(const T &_data) : data(_data), next(nullptr), prev(nullptr) {}

    DoublyLinkedListNode(const T &_data, DoublyLinkedListNode *_next, DoublyLinkedListNode *_prev)
      : data(_data), next(_next), prev(_prev)
    {}

    ~DoublyLinkedListNode() {}

    /* Returns true if the current node has a next one, this will return true unless the node is a tail node*/
    bool has_next() { return next != nullptr; }

    /* Returns true if the current node has a previous one, this will return true unless the node is head node*/
    bool has_prev() { return prev != nullptr; }

    DoublyLinkedListNode(const DoublyLinkedListNode &copy_node) = default;

    DoublyLinkedListNode &operator=(const DoublyLinkedListNode &copy_node) = default;

    DoublyLinkedListNode(DoublyLinkedListNode &&move_node) = default;

    DoublyLinkedListNode &operator=(DoublyLinkedListNode &&move_node) = default;

    const T &get_data() const { return data; }

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

    friend class DoublyLinkedList;
  };

private:
  using ListNodePtr = DoublyLinkedListNode *;

  ListNodePtr head;

  ListNodePtr tail;

  size_t _size = 0;

public:
  DoublyLinkedList() : head(nullptr), tail(nullptr) {}

  DoublyLinkedList(const DoublyLinkedList &copy_list)
  {
    // copy list
    this->_size = copy_list.size();
    head = new DoublyLinkedListNode(copy_list.front_data());
    ListNodePtr this_tmp_ptr = head;
    ListNodePtr copy_tmp_ptr = copy_list.head;
    while (copy_tmp_ptr->next)
    {
      ListNodePtr next_copy_ptr = copy_tmp_ptr->next;
      ListNodePtr new_copy_node_ptr = new DoublyLinkedListNode(next_copy_ptr->data);
      this_tmp_ptr->next = new_copy_node_ptr;
      new_copy_node_ptr->prev = this_tmp_ptr;
      this_tmp_ptr = this_tmp_ptr->next;
      copy_tmp_ptr = next_copy_ptr;
    }
    tail = this_tmp_ptr;
  }

  DoublyLinkedList &operator=(const DoublyLinkedList &copy_list)
  {

    if (this != &copy_list)
    {
      this->clear();
      this->_size = copy_list.size();
      this->_size = copy_list.size();
      head = new DoublyLinkedListNode(copy_list.front_data());
      ListNodePtr this_tmp_ptr = head;
      ListNodePtr copy_tmp_ptr = copy_list.head;
      while (copy_tmp_ptr->next)
      {
        ListNodePtr next_copy_ptr = copy_tmp_ptr->next;
        ListNodePtr new_copy_node_ptr = new DoublyLinkedListNode(next_copy_ptr->data);
        this_tmp_ptr->next = new_copy_node_ptr;
        new_copy_node_ptr->prev = this_tmp_ptr;
        this_tmp_ptr = this_tmp_ptr->next;
        copy_tmp_ptr = next_copy_ptr;
      }
      tail = this_tmp_ptr;
    }
    return *this;
  }

  DoublyLinkedList(DoublyLinkedList &&move_list)
  {
    // pointers_map = std::move(move_list.pointers_map);
    _size = std::move(move_list._size);
    head = move_list.head;
    move_list.head = nullptr;
    tail = move_list.tail;
    move_list.tail = nullptr;
  }

  DoublyLinkedList &operator=(DoublyLinkedList &&move_list)
  {
    if (this != &move_list)
    {
      this->clear();
      _size = std::move(move_list._size);
      head = move_list.head;
      move_list.head = nullptr;
      tail = move_list.tail;
      move_list.tail = nullptr;
    }
    return *this;
  }

  ~DoublyLinkedList() { clear(); }

  /* clear() function is used to free allocated memory, this function must be called only by the owner of the
   * MapDoublyLinkedList object */
  void clear()
  {
    for (ListNodePtr current = head, next; current;)
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
  void push_back(const T &data)
  {
    ListNodePtr new_node = new DoublyLinkedListNode(data);
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
    _size++;
  };

  /* Insert an element/node at the front of the list */
  void push_front(const T &data)
  {
    if (is_empty())
    {
      push_back(data);
    }
    else
    {
      ListNodePtr new_node = new DoublyLinkedListNode(data);
      new_node->set_next(head);
      head->set_prev(new_node);
      head = new_node;
    }
    _size++;
  }

  /* Erase an element/node from the list with a given key */
  /*
  void erase_element(const T1 &key)
  {
    auto it = pointers_map.find(key);

    if (it != pointers_map.end())
    {
      delete it->second;
      pointers_map.erase(it);
    }
  }
*/
  /* Return entry/data stored in the back node, first element of an entry is the key while the second contains data */
  const T &back_data() const
  {
    if (is_empty())
    {
      throw("empty container\n");
    }

    return tail->data;
  }

  /* Return entry/data stored in the front node, first element of an entry is the key while the second contains data
   */
  const T &front_data() const
  {
    if (is_empty())
    {
      throw("empty container\n");
    }

    return head->data;
  }

  /* Returns pointer to the front node */
  ListNodePtr front_pointer() const { return head; }

  /* Returns pointer to the back node */
  ListNodePtr back_pointer() const { return tail; }

  void erase(ListNodePtr node_ptr)
  {
    if (node_ptr == nullptr)
      return;

    if (is_empty())
      return;

    ListNodePtr next_node_ptr = node_ptr->next;
    ListNodePtr prev_node_ptr = node_ptr->prev;

    if (node_ptr == head)
      head = next_node_ptr;

    if (node_ptr == tail)
      tail = prev_node_ptr;

    if (next_node_ptr != nullptr)
    {
      next_node_ptr->set_prev(prev_node_ptr);
    }

    if (prev_node_ptr != nullptr)
    {
      prev_node_ptr->set_next(next_node_ptr);
    }

    delete node_ptr;
    _size--;
  }

  /* This function replace a node with given key (first argument) by a list (second argument). This function deletes
   * also the node with the given key*/

  void traverse() const
  {
    ListNodePtr traversal_ptr = head;
    while (traversal_ptr != nullptr)
    {
      std::cout << traversal_ptr->data << "\n";
      traversal_ptr = traversal_ptr->next;
    }
  }
};

} // namespace utils
