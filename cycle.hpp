#ifndef ITER_CYCLE_H_
#define ITER_CYCLE_H_

#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <initializer_list>
#include <iterator>
#include <utility>

namespace iter {
  namespace impl {
    template <typename Container>
    class Cycler;

    using CycleFn = IterToolFn<Cycler>;
  }
  constexpr impl::CycleFn cycle{};
}

template <typename Container>
class iter::impl::Cycler {
 private:
  friend CycleFn;

  Container container_;

  Cycler(Container&& container)
      : container_(std::forward<Container>(container)) {}

 public:
  Cycler(Cycler&&) = default;
  template <typename ContainerT>
  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<ContainerT>> {
   private:
    template <typename>
    friend class Iterator;
    IteratorWrapper<ContainerT> sub_iter_;
    IteratorWrapper<ContainerT> sub_begin_;
    IteratorWrapper<ContainerT> sub_end_;

   public:
    Iterator(IteratorWrapper<ContainerT>&& sub_iter,
        IteratorWrapper<ContainerT>&& sub_end)
        : sub_iter_{sub_iter},
          sub_begin_{sub_iter},
          sub_end_{std::move(sub_end)} {}

    iterator_deref<ContainerT> operator*() {
      return *sub_iter_;
    }

    iterator_arrow<ContainerT> operator->() {
      return apply_arrow(sub_iter_);
    }

    Iterator& operator++() {
      ++sub_iter_;
      // reset to beginning upon reaching the sub_end_
      if (!(sub_iter_ != sub_end_)) {
        sub_iter_ = sub_begin_;
      }
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    template <typename T>
    bool operator!=(const Iterator<T>& other) const {
      return sub_iter_ != other.sub_iter_;
    }

    template <typename T>
    bool operator==(const Iterator<T>& other) const {
      return !(*this != other);
    }
  };

  Iterator<Container> begin() {
    return {get_begin(container_), get_end(container_)};
  }

  Iterator<Container> end() {
    return {get_end(container_), get_end(container_)};
  }

  Iterator<AsConst<Container>> begin() const {
    return {get_begin(impl::as_const(container_)),
        get_end(impl::as_const(container_))};
  }

  Iterator<AsConst<Container>> end() const {
    return {get_end(impl::as_const(container_)),
        get_end(impl::as_const(container_))};
  }
};

#endif
