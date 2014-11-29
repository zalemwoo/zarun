/*
 * line_editor.h
 *
 *  Created on: Nov 18, 2014
 *      Author: zalem
 */

#ifndef ZARUN_LINE_EDITOR_H_
#define ZARUN_LINE_EDITOR_H_

#include "v8/include/v8.h"

namespace zarun {

class LineEditor {
 public:
  enum Type { DUMB = 0, READLINE = 1 };

  LineEditor(Type type, const char* name);
  virtual ~LineEditor() {}

  virtual std::string Prompt(const char* prompt) = 0;
  virtual bool Open();
  virtual bool Close();
  virtual void AddHistory(const char* str) {}

  const char* name() { return name_; }

  static LineEditor* Get() { return current_; }

 private:
  Type type_;
  const char* name_;
  static LineEditor* current_;
};

class DumbLineEditor : public LineEditor {
 public:
  explicit DumbLineEditor() : LineEditor(LineEditor::DUMB, "dumb") {}
  virtual std::string Prompt(const char* prompt) override;
};

}  // namespace zarun

#endif /* ZARUN_LINE_EDITOR_H_ */
