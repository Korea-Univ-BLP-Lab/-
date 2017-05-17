/* modified by dglee */
/* DATE : 8/Mar/2004 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "FST.h"

/*****************************************************************************/
FST *LoadFST(char *FSTName)
{				/* FST file을 open */
  FILE *Dic;
  FST *fst;
  int FileSize;

  /* open FST file */
  if ((Dic = fopen(FSTName, "rb")) == NULL)
	  return NULL;

  /* calculate filesize */
  fseek(Dic, 0, 2);		/* SEEK_END */
  FileSize = ftell(Dic);
  fseek(Dic, 0, 0);

  /* memory allocate */
  if ((fst=(FST *)malloc(FileSize))==NULL) {
    fclose(Dic);
    return NULL;
  }

  /* load FST */
  fread(fst, sizeof(FST), FileSize/sizeof(FST), Dic);
  fclose(Dic);

  return fst;
}

/*****************************************************************************/
/* free memory for FST */
void FreeFST(FST *fst)
{				
    free(fst);
}

/*****************************************************************************/
/* calculate hash value of the given string from FST */
/* nItem : String에 일치하는 엔트리의 개수 */
/* 반환값 : String에 일치하는 첫번째 엔트리의 Hash Value 또는 -1(검색 실패) */
int String2Hash(FST *fst, char *String, int *nItem)
{                              
  int CurrentState=0, Result=0;
  unsigned char *p=(unsigned char *)String;
  
  /* transition among states */
  while (*p!=0) {
    /* if there is no transition when *Key is given */
    if ((*p!=fst[CurrentState+*p].CHAR)||(fst[CurrentState+*p].Next==0)||(fst[CurrentState+*p].Next==-1))
      return -1;
    else {
      /* add hash value for given transition */
      Result+=fst[CurrentState+*p].Value;
      /* transit to mathing state */
      CurrentState=fst[CurrentState+*p].Next;
      p++;
    }
  }
  
  /* is current state one of the final state? */
  *nItem=fst[CurrentState+IsFinal].CHAR;
  /*if (fst[CurrentState+IsFinal].CHAR) return Result;*/ /* original */
  
  if (fst[CurrentState+IsFinal].Next == -1) return Result;
  else return -1;
}

/*****************************************************************************/
/* find string with hash value */
/* 0 : invalid hash value */
/* 1 : find string corresponding to the hash value */
char *Hash2String(FST *fst, int HashValue, char *String)
{
  unsigned int i;
  int CurrentState=0;
  unsigned char *p=(unsigned char *)String;

  while (1) {
    /* find matching state */
    *p=0;
    for (i=1; i<MAX_CHAR; i++)
      if ((i==fst[CurrentState+i].CHAR)&&(fst[CurrentState+i].Next!=-1))
        if (fst[CurrentState+i].Value>HashValue) break;
        else *p=i;
    /* not found */
    if (*p==0) return NULL;
    HashValue-=fst[CurrentState+*p].Value;
    CurrentState=fst[CurrentState+*p].Next;
    p++;
    if (((unsigned int)HashValue<fst[CurrentState+IsFinal].CHAR)&&(fst[CurrentState+IsFinal].Next==-1))
      break;
  }

  *p=0;
  return String;
}

/*****************************************************************************/
int FindAll(FST *fst, unsigned char *p, int CurrentState, int Result, int *IndexList, int n, int IncompleteChar)
{
  unsigned int i;

  if (n>=FSTMaxMatch) return n;
  if (CurrentState==-1) return n;
  if (!IncompleteChar&&(*p==0)) {
    if (fst[CurrentState+IsFinal].Next==-1) {
      for (i=0; i<fst[CurrentState+IsFinal].CHAR; i++)
        IndexList[n+i]=Result+i;
      return n+fst[CurrentState+IsFinal].CHAR;
    }
    return n;
  }

  if (!IncompleteChar&&(*p=='*'))
    n=FindAll(fst, p+1, CurrentState, Result, IndexList, n, 0);

  for (i=1; i<MAX_CHAR; i++)
    if ((*p=='*')||(*p=='?')||(*p==i)) 
      if ((i==fst[CurrentState+i].CHAR)&&fst[CurrentState+i].Next&&(fst[CurrentState+i].Next!=CurrentState))
        if (*p=='*')
          n=FindAll(fst, p, fst[CurrentState+i].Next, Result+fst[CurrentState+i].Value, IndexList, n, (!IncompleteChar)&&(i>=0x80));
        else {
          if ((*p=='?')&&!IncompleteChar&&(*(p+1)!='?')&&(i>=0x80))
            continue;
          if (IncompleteChar)
            n=FindAll(fst, p+1, fst[CurrentState+i].Next, Result+fst[CurrentState+i].Value, IndexList, n, 0);
          else n=FindAll(fst, p+1, fst[CurrentState+i].Next, Result+fst[CurrentState+i].Value, IndexList, n, i>=0x80);
        }

  return n;
}

/*****************************************************************************/
/* calculate hash values of all matched pattern */
int Pattern2Hash(FST *fst, char *String, int *IndexList)
{				

  return FindAll(fst, (unsigned char *)String, 0, 0, IndexList, 0, 0);
}

/*****************************************************************************/
/* calculate hash value located in the path of the given string from FST */
void Path2Hash(FST *fst, char *String, int *Flag)
{                              
  int CurrentState=0, Result=0, i, n=strlen(String);
  unsigned char *p=(unsigned char *)String;
  
  for (i=0; i<n; i++)
    Flag[i]=-1;

  /* transition among states */
  while (*p!=0) {
    /* if there is no transition when *Key is given */
    if ((*p!=fst[CurrentState+*p].CHAR)||(fst[CurrentState+*p].Next==0)||(fst[CurrentState+*p].Next==-1))
      return;
    else {
      /* add hash value for given transition */
      Result+=fst[CurrentState+*p].Value;
      /* transit to mathing state */
      CurrentState=fst[CurrentState+*p].Next;
      p++;
    }
    if (fst[CurrentState+IsFinal].CHAR==0xff)
      Flag[p-(unsigned char *)String-1]=Result;
  }
}

/*****************************************************************************/
/* calculate hash value of the string which is most similar to the given string */
int String2MostSimilarHash(FST *fst, char *String, int *nItem)
{                              
  int CurrentState=0, Result=0, Hash=-1;
  unsigned char *p=(unsigned char *)String;
  
  /* transition among states */
  while (*p!=0) {
    /* if there is no transition when *Key is given */
    if ((*p!=fst[CurrentState+*p].CHAR)||(fst[CurrentState+*p].Next==0)||(fst[CurrentState+*p].Next==-1)) 
      return Hash;
    else {
      /* add hash value for given transition */
      Result+=fst[CurrentState+*p].Value;
      /* transit to mathing state */
      CurrentState=fst[CurrentState+*p].Next;
      p++;
    }
    if (fst[CurrentState+IsFinal].CHAR) {
      *nItem=fst[CurrentState+IsFinal].CHAR;
      Hash=Result;
    }
  }

  return Hash;
}

/*****************************************************************************/
/* build tabular containing segmentation information for the given string */
void String2Tabular(FST *fst, char *String, int *Tabular)
{
  int CurrentState, i, n=strlen(String);
  unsigned char *p=(unsigned char *)String;
  
  for (i=0; i<n; i++) {
    CurrentState=0;
    p=(unsigned char *)String+i;
    memset(Tabular+i*(2*n-i+1)/2, 0, (n-i)*sizeof(int));
    /* transition among states */
    while (*p!=0) {
      /* if there is no transition when *Key is given */
      if ((*p!=fst[CurrentState+*p].CHAR)||(fst[CurrentState+*p].Next==0)||(fst[CurrentState+*p].Next==-1))
      	break;
      else {
	      /* transit to mathing state */
        CurrentState=fst[CurrentState+*p].Next;
        p++;
      }
      Tabular[i*(2*n-i+1)/2+(p-(unsigned char *)String)-i-1]=fst[CurrentState+IsFinal].CHAR;
    }
  }
}

/*****************************************************************************/
/* made by dglee */
/* 대호형 버전의 함수와 동일하게 만듦 */
/* Tabular에는 index 번호가 들어감 */
/* return value : 주어진 문자열(String)의 부분 문자열중에서 FST의 엔트리와 매치되는 개수 */
int FindFSTTabular(FST *fst, char *String, int *Tabular)
{
  int CurrentState, i, n;
  unsigned char *p=(unsigned char *)String;
  int Result;
  int count = 0;

  n = strlen(String);
  
  /* -1로 초기화 */
  for (i = 0; i < n*(n+1)/2; i++) Tabular[i] = -1;

  for (i = 0; i < n; i++) {
    CurrentState=0;
    Result = 0;
    p=(unsigned char *)String+i;

    /* transition among states */
    while (*p!=0) {
      /* if there is no transition when *Key is given */
      if ((*p!=fst[CurrentState+*p].CHAR)) break;
      else {
        Result += fst[CurrentState+*p].Value;
	      /* transit to mathing state */
        CurrentState=fst[CurrentState+*p].Next;
      }  

      if (fst[CurrentState].Next == -1) {
        Tabular[TabPos2(i, 1+p-(unsigned char *)String, n)] = Result;
        count++;
      }
      p++;      
    }
  }
  return count;
}


/*****************************************************************************/
/* made by dglee */
/* 한글버전 (2바이트) */
/* Tabular에는 index 번호가 들어감 */
/* return value : 주어진 문자열(String)의 부분 문자열중에서 FST의 엔트리와 매치되는 개수 */
int FindFSTTabular_hangul(FST *fst, char *String, int *Tabular)
{
  int CurrentState, i, j, n;
  unsigned char *p=(unsigned char *)String;
  int Result;
  int count = 0;

  int even = 1;
  
  n = strlen(String)/2; /* 음절 수 */
  
  /* -1로 초기화 */
  for (i = 0; i < n*(n+1)/2; i++) Tabular[i] = -1;

  for (i = j = 0; j < n; j++,i+=2) {
    CurrentState=0;
    Result = 0;
    p=(unsigned char *)String+i;
    even = 1;

    /* transition among states */
    while (*p!=0) {
      /* if there is no transition when *Key is given */
      if ((*p!=fst[CurrentState+*p].CHAR)) break;
      else {
        Result += fst[CurrentState+*p].Value;
	      /* transit to mathing state */
        CurrentState=fst[CurrentState+*p].Next;
      }  
      if (even) even = 0;
      else even = 1;
      p++;
      
      if (fst[CurrentState].Next == -1 && even) {
        Tabular[TabPos2(i/2, (p-(unsigned char *)String)/2, n)] = Result;
        ///**/fprintf(stderr, "i/2 = %d, (p-string)/2 = %d, tabular[%d] = %d\n", i/2, (p-(unsigned char *)String)/2, TabPos2(i/2, (p-(unsigned char *)String)/2, n), Result);
        count++;
      }
    }
  }
  return count;
}
