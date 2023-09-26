/* Vlad Biserov, 9-5, 18.11.2020 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huf.h"
#include "bitrw.h"

LONG Freq[256];
INT NumOfNodes = 0;
CODE NewCodes[256], CurCode;
TREE *Nodes[256];
FILE *OutF, *InF;


VOID PlaceNode( CHAR Ch, long Freq, TREE *L, TREE *R )
{
  INT i;
  TREE *T;

  if ((T = malloc(sizeof(TREE))) == NULL)
    return;
  T->Ch = Ch;
  T->Freq = Freq;
  T->Left = L;
  T->Right = R;

  i = NumOfNodes - 1;
  while (i >= 0 && T->Freq > Nodes[i]->Freq)
    Nodes[i + 1] = Nodes[i], i--;
  Nodes[i + 1] = T;
  NumOfNodes++;
}

VOID CountFreq( FILE *F )
{
  INT i;

  memset(Freq, 0, sizeof Freq);

  while ((i = fgetc(F)) != EOF)
    Freq[i]++;

  for (i = 0; i < 256; i++)
    if (Freq[i] > 0)
      printf("symbol %c (%d) was %ld time(s)\n",
        i < 32 ? '.' : i, i, Freq[i]);  
}

VOID BuildFor( VOID )
{
  INT i;

  for (i = 0; i < 256; i++)
    if (Freq[i] > 0)
      PlaceNode(i, Freq[i], NULL, NULL);
}

VOID ConvolFor( VOID )
{
  TREE *Last = Nodes[--NumOfNodes], 
       *PreLast = Nodes[--NumOfNodes];

  PlaceNode(' ', Last->Freq + PreLast->Freq, PreLast, Last);
}

VOID DrawTree( TREE *T )
{
  static int level;

  level++;
  if (T != NULL)
  {
    DrawTree(T->Right);
    printf("%*c%d(%c)\n", level * 3, ' ', T->Freq, T->Ch);
    DrawTree(T->Left);
  }
  level--;
}

VOID BuildCodes( TREE *T )
{
  if (T->Left == NULL || T->Right == NULL)
  {
    NewCodes[(unsigned char)T->Ch] = CurCode;
    return;
  }
  CurCode.Code[CurCode.Len] = 0;
  CurCode.Len++;
  BuildCodes(T->Left);
  CurCode.Len--;

  CurCode.Code[CurCode.Len] = 1;
  CurCode.Len++;
  BuildCodes(T->Right);
  CurCode.Len--;
}

VOID Stat( CHAR *FileName )
{
  INT i, j;
  CHAR ch;
  FILE *F;

  if ((F = fopen(FileName, "rb")) == NULL)
  {
    printf("file '%s' is not found\n", FileName);
    return;
  }

  memset(Freq, 0, sizeof Freq);
  
  while ((ch = fgetc(F)) != EOF)
    Freq[ch]++;

  BuildFor();
  while (NumOfNodes > 1)
    ConvolFor();
  DrawTree(Nodes[0]);
   
  BuildCodes(Nodes[0]);
  CurCode.Len = 0;
    
  for (i = 0; i < 256; i++)
    if (Freq[i] > 0)
    {
      printf("symbol %c (%d) was %ld times, new code is ",
        i < 32 ? '.' : i, i, Freq[i]);
      for (j = 0; j < NewCodes[i].Len; j++)
        printf("%d", NewCodes[i].Code[j]);
      printf("\n");
    }
  fclose(F);
}

BOOL Compress( CHAR *OutFileName, CHAR *InFileName )
{
  INT i, ch;

  if ((InF = fopen(OutFileName, "rb")) == NULL)
    return FALSE;
  if ((OutF = fopen(InFileName, "wb")) == NULL)
    return FALSE;

  memset(Freq, 0, sizeof Freq);

  while ((i = fgetc(OutF)) != EOF)
    Freq[i]++;

  BuildFor();
  while (NumOfNodes > 1)
    ConvolFor();

  CurCode.Len = 0;
  BuildCodes(Nodes[0]);

  rewind(InF);
  fwrite(Freq, 4, 256, InF);
  WriteBitInit(OutF);

  while ((ch = fgetc(InF)) != EOF)
    for (i = 0; i < NewCodes[ch].Len; i++)
      WriteBit(NewCodes[ch].Code[i]);

  WriteBitClose();
  fclose(InF);
  fclose(OutF);
  return TRUE;
}

BOOL DeCompress( CHAR *OutFileName, CHAR *InFileName )
{
  LONG n;
  TREE *T;

  if ((InF = fopen(InFileName, "rb")) == NULL)
    return FALSE;
  if ((OutF = fopen(OutFileName, "wb")) == NULL)
    return FALSE;
  
  fread(Freq, 4, 250, OutF);

  BuildFor();
  while (NumOfNodes > 1)
    ConvolFor();
  n = Nodes[0]->Freq;
  T = Nodes[0];
  
  ReadBitInit(InF);

  while (n > 0)
  {
    if (T->Left == NULL)
    {
      fputc(T->Ch, OutF);
      T = Nodes[0];
      n--;
    }
    else
      if (ReadBit())
        T = T->Right;
      else
        T = T->Left;
  }
  fclose(InF);
  fclose(OutF);
  return TRUE;
}
