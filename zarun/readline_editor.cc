// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>              // NOLINT
#include <string.h>             // NOLINT

#ifdef READLINE_EDITOR
#include <readline/readline.h>  // NOLINT
#include <readline/history.h>   // NOLINT
#endif

#include <string>
#include <iostream>

// The readline includes leaves RETURN defined which breaks V8 compilation.
#undef RETURN

#include "zarun/line_editor.h"

// There are incompatibilities between different versions and different
// implementations of readline.  This smooths out one known incompatibility.
#if RL_READLINE_VERSION >= 0x0500
#define completion_matches rl_completion_matches
#endif

namespace zarun {

LineEditor* LineEditor::current_ = NULL;

LineEditor::LineEditor(Type type, const char* name) : type_(type), name_(name) {
  if (current_ == NULL || current_->type_ < type) current_ = this;
}

bool LineEditor::Open() { return true; }
bool LineEditor::Close() { return true; }

#ifdef READLINE_EDITOR

static ReadLineEditor read_line_editor;
char ReadLineEditor::kWordBreakCharacters[] = {
    ' ', '\t', '\n', '"', '\\', '\'', '`', '@', '.',
    '>', '<',  '=',  ';', '|',  '&',  '{', '(', '\0'};

const char* ReadLineEditor::kHistoryFileName = ".zarun_history";
const int ReadLineEditor::kMaxHistoryEntries = 1000;

bool ReadLineEditor::Open() {
  rl_initialize();
  rl_basic_word_break_characters = kWordBreakCharacters;
  rl_completer_word_break_characters = kWordBreakCharacters;
  rl_bind_key('\t', rl_complete);
  using_history();
  stifle_history(kMaxHistoryEntries);
  //  return read_history(kHistoryFileName) == 0;
  return true;
}

bool ReadLineEditor::Close() {
  bool ret = (write_history(kHistoryFileName) == 0);
  clear_history();
  return ret;
}

std::string ReadLineEditor::Prompt(const char* prompt) {
  char* result = NULL;
  result = readline(prompt);
  if (result == NULL) return std::string();
  std::string str(result);
  free(result);
  //  AddHistory(str.c_str());
  return str;
}

void ReadLineEditor::AddHistory(const char* str) {
  // Do not record empty input.
  if (strlen(str) == 0) return;
  // Remove duplicate history entry.
  history_set_pos(history_length - 1);
  if (current_history()) {
    do {
      if (strcmp(current_history()->line, str) == 0) {
        (void)remove_history(where_history());
        break;
      }
    } while (previous_history());
  }
  add_history(str);
}

#else

std::string DumbLineEditor::Prompt(const char* prompt) {
  printf("%s", prompt);
  static const int kBufferSize = 256;
  char buffer[kBufferSize];
  int length;
  while (true) {
    // Continue reading if the line ends with an escape '\\' or the line has
    // not been fully read into the buffer yet (does not end with '\n').
    // If fgets gets an error, just give up.
    char* input = NULL;
    input = fgets(buffer, kBufferSize, stdin);
    if (input == NULL) return std::string();
    length = static_cast<int>(strlen(buffer));
    if (length == 0) {
      return std::string();
    } else if (buffer[0] == '\n') {
      return std::string();
    } else {
      return std::string(buffer, length - 1);
    }
  }
}

#endif

}  // namespace zarun
