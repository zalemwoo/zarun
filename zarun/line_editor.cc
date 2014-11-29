// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <string.h>

#include "build/build_config.h"

#if defined(OS_POSIX)
#include "third_party/linenoise/linenoise.h"
#endif

#include <iostream>

#include "zarun/line_editor.h"

namespace zarun {

LineEditor* LineEditor::current_ = NULL;

LineEditor::LineEditor(Type type, const char* name) : type_(type), name_(name) {
  if (current_ == NULL || current_->type_ < type)
    current_ = this;
}

bool LineEditor::Open() {
  return true;
}
bool LineEditor::Close() {
  return true;
}

#if defined(OS_POSIX)

class LineEditorImpl : public LineEditor {
 public:
  LineEditorImpl() : LineEditor(LineEditor::READLINE, "readline") {}
  virtual std::string Prompt(const char* prompt) override;
  virtual bool Open() override;
  virtual bool Close() override;
  virtual void AddHistory(const char* str) override;

  static const char* kHistoryFileName;
  static const int kMaxHistoryEntries;
};

const char* LineEditorImpl::kHistoryFileName = ".zarun_history";
const int LineEditorImpl::kMaxHistoryEntries = 32;

bool LineEditorImpl::Open() {
  linenoiseSetMultiLine(1);
  linenoiseHistoryLoad(kHistoryFileName);
  linenoiseHistorySetMaxLen(kMaxHistoryEntries);
  return true;
}

bool LineEditorImpl::Close() {
  if (!linenoiseHistorySave(kHistoryFileName)) {
    return true;
  }
  return false;
}

std::string LineEditorImpl::Prompt(const char* prompt) {
  char* result = NULL;
  result = linenoise(prompt);
  if (result[0] != '\0') {
    AddHistory(result);
    std::string str(result);
    free(result);
    return str;
  } else {
    free(result);
    return std::string();
  }
}

void LineEditorImpl::AddHistory(const char* str) {
  linenoiseHistoryAdd(str);
}

#else

class LineEditorImpl : public LineEditor {
 public:
  explicit LineEditorImpl() : LineEditor(LineEditor::DUMB, "dumb") {}
  virtual std::string Prompt(const char* prompt) override;
};

std::string LineEditorImpl::Prompt(const char* prompt) {
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
    if (input == NULL)
      return std::string();
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

namespace {
static LineEditorImpl line_editor;
}  // namespace

}  // namespace zarun
