//===----------------------------------------------------------------------===//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17
// UNSUPPORTED: no-localization
// UNSUPPORTED: libcpp-has-no-incomplete-format
// TODO FMT Evaluate gcc-12 status
// UNSUPPORTED: gcc-12
// TODO FMT Investigate AppleClang ICE
// UNSUPPORTED: apple-clang-13

// <format>

// template<class Out, class... Args>
//   Out format_to(Out out, const locale& loc,
//                 format-string<Args...> fmt, const Args&... args);
// template<class Out, class... Args>
//   Out format_to(Out out, const locale& loc,
//                 wformat-string<Args...> fmt, const Args&... args);

#include <format>
#include <algorithm>
#include <cassert>
#include <list>
#include <vector>

#include "test_macros.h"
#include "format_tests.h"
#include "string_literal.h"
#include "test_format_string.h"

auto test =
    []<class CharT, class... Args>(
        std::basic_string_view<CharT> expected, test_format_string<CharT, Args...> fmt, Args&&... args) constexpr {
      {
        std::basic_string<CharT> out(expected.size(), CharT(' '));
        auto it = std::format_to(out.begin(), std::locale(), fmt, std::forward<Args>(args)...);
        assert(it == out.end());
        assert(out == expected);
      }
      {
        std::list<CharT> out;
        std::format_to(std::back_inserter(out), std::locale(), fmt, std::forward<Args>(args)...);
        assert(std::equal(out.begin(), out.end(), expected.begin(), expected.end()));
      }
      {
        std::vector<CharT> out;
        std::format_to(std::back_inserter(out), std::locale(), fmt, std::forward<Args>(args)...);
        assert(std::equal(out.begin(), out.end(), expected.begin(), expected.end()));
      }
      {
        assert(expected.size() < 4096 && "Update the size of the buffer.");
        CharT out[4096];
        CharT* it = std::format_to(out, std::locale(), fmt, std::forward<Args>(args)...);
        assert(std::distance(out, it) == int(expected.size()));
        // Convert to std::string since output contains '\0' for boolean tests.
        assert(std::basic_string<CharT>(out, it) == expected);
      }
    };

auto test_exception = []<class CharT, class... Args>(std::string_view, std::basic_string_view<CharT>, Args&&...) {
  // After P2216 most exceptions thrown by std::format_to become ill-formed.
  // Therefore this tests does nothing.
  // A basic ill-formed test is done in format_to.locale.verify.cpp
  // The exceptions are tested by other functions that don't use the basic-format-string as fmt argument.
};

int main(int, char**) {
  format_tests<char>(test, test_exception);

#ifndef TEST_HAS_NO_WIDE_CHARACTERS
  format_tests_char_to_wchar_t(test);
  format_tests<wchar_t>(test, test_exception);
#endif

  return 0;
}
