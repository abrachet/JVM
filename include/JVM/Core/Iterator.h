
#ifndef JVM_CORE_ITERATOR_H
#define JVM_CORE_ITERATOR_H

namespace jvm {

// Analagous to std::insert_iterator, but doesn't require the iterator to be
// Container::iterator, it can be Container::reverse_iterator.
template <typename Container, typename Iter> class insert_iterator {
  Iter iter;

public:
  insert_iterator(Iter iter) : iter(iter) {}

  insert_iterator &operator=(typename Container::value_type &&value) {
    *iter++ = value;
    return *this;
  }
  insert_iterator &operator*() { return *this; }
  insert_iterator &operator++() { return *this; }
};

template <typename Container, typename Iter>
insert_iterator<Container, Iter> inserter(Container &container, Iter iter) {
  return insert_iterator<Container, Iter>(iter);
}

} // namespace jvm

#endif // JVM_CORE_ITERATOR_H
