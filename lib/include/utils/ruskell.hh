// this class is used to check the result of a function call

#pragma once

#include <variant>
#include <string>
#include <optional>

template<typename T>
using Expect = std::variant<std::string, T>;

template<typename T>
using Result = std::variant<bool, T>;

template<typename T>
using Maybe = std::optional<T>;

template<typename Left, typename Right >
using Either = std::variant<Left, Right>;

template <class F>
struct Final_action {
  explicit Final_action(F f) : act(f) {}
  ~Final_action() { act(); }
  F act;
};

template <class F>
[[nodiscard]] auto finally(F f) {
  return Final_action(f);
}