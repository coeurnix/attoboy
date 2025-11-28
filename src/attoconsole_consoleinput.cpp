#include "attoconsole_internal.h"

namespace attoboy {

ConsoleInput::ConsoleInput()
    : completions(List()), history(List()), password(false), multiline(false),
      continuation("> ") {}

ConsoleInput::ConsoleInput(const ConsoleInput &other)
    : completions(other.completions), history(other.history),
      password(other.password), multiline(other.multiline),
      continuation(other.continuation) {}

ConsoleInput::~ConsoleInput() {}

ConsoleInput &ConsoleInput::operator=(const ConsoleInput &other) {
  if (this != &other) {
    completions = other.completions;
    history = other.history;
    password = other.password;
    multiline = other.multiline;
    continuation = other.continuation;
  }
  return *this;
}

} // namespace attoboy
