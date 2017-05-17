#ifndef _SEARCH_SUBSTRING_H_
#define _SEARCH_SUBSTRING_H_

#pragma warning(disable: 4786)
#pragma warning(disable: 4503)

#include <vector>
#include "FST.h"

using namespace std;

/* 문장 최대 길이 */
#define MAX_LINE 5000

/* 문자열(entity)의 최대 길이 */
#define MAX_WORD_LEN 200

/* category의 최대 길이 */
#define MAX_CAT_LEN 40

typedef struct {
  char entity_name[MAX_WORD_LEN]; /* 검색된 문자열 */
  char category[MAX_CAT_LEN];   /* 카테고리 */
  int pos;                    /* 발견된 위치 */
} entity_struct;


// 파일(filename)로부터 모든 부분 문자열을 검색하여 엔티티를 찾아 출력한다.
extern int search_in_file(FST *fst, char **fst_info, int *fst_freq,
                          char *filename); /* 화일 단위 */

// 문자열(src)로부터 모든 부분 문자열을 검색하여 엔티티를 찾는다.
extern int search_in_string(FST *fst, char **fst_info, int *fst_freq, 
                            char *src, vector<entity_struct> &result); /* 문장 단위 */

extern int mylex(char *str);
extern vector<entity_struct> entities_in_lex;

#endif
