/* Vlad Biserov, 9-5, 18.11.2020 */

/* Debug memory allocation support */
#ifndef NDEBUG
#  define _CRTDBG_MAP_ALLOC
#  include <crtdbg.h>
#  define SetDbgMemHooks() \
     _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF | \
       _CRTDBG_ALLOC_MEM_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else /* _DEBUG */
#  define SetDbgMemHooks() ((void)0)
#endif /* _DEBUG */
#include <stdlib.h>

#include <commondf.h>
#include <stdio.h>

typedef struct tagTREE TREE;
struct tagTREE
{
  char Ch;
  long Freq;
  TREE *Left, *Right;
};

typedef struct
{
  INT Code[47], Len;
}CODE;

VOID Stat( CHAR *FileName );
BOOL Compress( CHAR *OutFileName, CHAR *InFileName );
BOOL DeCompress( CHAR *OutFileName, CHAR *InFileName );