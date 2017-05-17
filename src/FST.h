/* FST.h */
/* modified by dglee */
/* date : 29 Sep 2001 */
#ifndef __FST__
#define __FST__

#define MAX_CHAR   256
#define IsFinal    0

#define MaxStringLength 100	/* 찾고자하는  string의 최대길이 */
#define FSTMaxMatch 100000

/* 삼각행렬의 원소를 입력받아 일차원배열에 대응되는 위치를 리턴 */
#define TabPos2(x, y, n) ((x)*(2*n-(x)-1)/2+(y)-1) /* dglee */

/* 문자열의 크기를 입력받아 삼각행렬의 셀수를 리턴 */
#define TabNum(x) ((x)*((x)+1)/2) /* added by dglee */


typedef struct {
  unsigned int CHAR:8;
  int Value:24;
  int Next;
} FST;


/******************************************************************************/
extern int MakeFST(char *SourceFilename, char *FSTFilename);

extern FST *LoadFST(char *FSTName);
extern void FreeFST(FST *fst);


/* calculate hash value of the given string from FST */
/* nItem : String에 일치하는 엔트리의 개수 */
/* 반환값 : String에 일치하는 첫번째 엔트리의 Hash Value 또는 -1(검색 실패) */
extern int String2Hash(FST *fst, char *String, int *nItem);

/* find string with hash value */
/* 0 : invalid hash value */
/* 1 : find string corresponding to the hash value */
extern char *Hash2String(FST *fst, int HashValue, char *String);

/* calculate hash values of all matched pattern */
extern int Pattern2Hash(FST *fst, char *String, int *IndexList);

/* calculate hash value located in the path of the given string from FST */
extern void Path2Hash(FST *fst, char *String, int *Flag);

/* calculate hash value of the string which is most similar to the given string */
extern int String2MostSimilarHash(FST *fst, char *String, int *nItem);

/* build tabular containing segmentation information for the given string */
extern void String2Tabular(FST *fst, char *String, int *Tabular);

/* build tabular containing segmentation information for the given string */
/* made by dglee */
extern int FindFSTTabular(FST *fst, char *String, int *Tabular);
extern int FindFSTTabular_hangul(FST *fst, char *String, int *Tabular);

/* change ASCII code to 4-bit code */
extern void H2to4(unsigned char *s, unsigned char *t);

/* change ASCII code(including wildcard ?) to 3-byte code */
extern void Q2to4(unsigned char *s, unsigned char *t);

/* change 4-bit code to ASCII code */
extern void H4to2(unsigned char *s, unsigned char *t);

#endif

