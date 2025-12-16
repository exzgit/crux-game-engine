#include "editor.h"
#include <iostream>

int main() {

  auto editor = __EDITOR__::CruxEditor();
  editor.update();

  return 0;
}
