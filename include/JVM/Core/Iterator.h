// Copyright 2020 Alex Brachet
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
