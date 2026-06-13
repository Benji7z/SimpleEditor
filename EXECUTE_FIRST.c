#include <stdio.h>
int main(void)
{
  printf("\033[31;1;4mBe careful with what files you edit since there is no:\n- Undo\n- Quit without saving\nWhich means it is easy to mess up your files\033[0m");
  printf("\nYou can delete this file now.");
  return 0;
}
