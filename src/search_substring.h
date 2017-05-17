#ifndef _SEARCH_SUBSTRING_H_
#define _SEARCH_SUBSTRING_H_

#pragma warning(disable: 4786)
#pragma warning(disable: 4503)

#include <vector>
#include "FST.h"

using namespace std;

/* ���� �ִ� ���� */
#define MAX_LINE 5000

/* ���ڿ�(entity)�� �ִ� ���� */
#define MAX_WORD_LEN 200

/* category�� �ִ� ���� */
#define MAX_CAT_LEN 40

typedef struct {
  char entity_name[MAX_WORD_LEN]; /* �˻��� ���ڿ� */
  char category[MAX_CAT_LEN];   /* ī�װ� */
  int pos;                    /* �߰ߵ� ��ġ */
} entity_struct;


// ����(filename)�κ��� ��� �κ� ���ڿ��� �˻��Ͽ� ��ƼƼ�� ã�� ����Ѵ�.
extern int search_in_file(FST *fst, char **fst_info, int *fst_freq,
                          char *filename); /* ȭ�� ���� */

// ���ڿ�(src)�κ��� ��� �κ� ���ڿ��� �˻��Ͽ� ��ƼƼ�� ã�´�.
extern int search_in_string(FST *fst, char **fst_info, int *fst_freq, 
                            char *src, vector<entity_struct> &result); /* ���� ���� */

extern int mylex(char *str);
extern vector<entity_struct> entities_in_lex;

#endif
