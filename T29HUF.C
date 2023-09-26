/* Vlad Biserov, 9-5, 18.11.2020 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "huf.h"
#include "bitrw.h"

VOID main( VOID )
{
  SetDbgMemHooks();
  Stat("X:\\TXT\\orwell.txt");
  //Compress("X:\\TXT\\orwell.txt", "test.txt");
  DeCompress("test.txt", "test2.txt");
  _getch();
}
