//#define _DEBUG_ /* debugging �ÿ� ���� */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "FST.h"
#include "search_substring.h"

/************************************************/
typedef struct {
  short x;
  short y;
  char str[MAX_LINE];
  char category[MAX_CAT_LEN];
} tab_cell;

/*****************************************************************************/
/* ������ �κ� ���ڿ��� ã�´�. */
int find_sub_strings(FST *fst_fst, char **fst_info, int *fst_freq, 
                     char *src, int *Tabular, int count, int Len, 
                     vector<entity_struct> &result) {
  int i, j, l;
  int pos;
  
  vector<tab_cell> tabstr; /* ������ �ִ� �κ� ���ڿ��� tab ���� ���� */
  int cur_tabstr = 0; /* tabstr�� ���� ��ġ�� ����Ű�� ������ */

  vector<tab_cell> longest; /* ������ �ִ� �κ� ���ڿ��� tab ���� ���� */
  int cur_longest = 0;
  
  int save_ok; /* ���� ���� ���� */

  tab_cell one_result;
  entity_struct one_entity;

  if (count == 0) {
    return 0;
  }
  
  char str[MAX_LINE];
  int k = 0;

  /* ������ �ִ� �κй��ڿ��� tabstr�� ���� */
  for (i = 0; i < Len /*&& cur_tabstr < count*/; i++) {
    for (j = 1+i; j <= Len /*&& cur_tabstr < count*/; j++) {

      pos = TabPos2(i, j, Len);

      if (Tabular[pos] == -1) {
        continue;
      }

      // ���� ó��(i == 0)�̰ų�, �� ���ڰ� ������ ��츸 ����
      if (i > 0 && !isspace(src[i-1])) {

        #ifdef _DEBUG_
        fprintf(stderr, "cannot save! not beginning of a word\n");
        #endif

        continue;
      }

      Hash2String(fst_fst, Tabular[pos], str); /* �κ� ���ڿ� ���� */

      one_result.x = i;
      one_result.y = j;
      strcpy(one_result.str, str);
      strcpy(one_result.category, fst_info[Tabular[pos]]); // category ����

      tabstr.push_back(one_result); // ����
      
      cur_tabstr++; // ����

      int IndexList[2];
      k = String2Hash(fst_fst, str, IndexList);

      for (k++, l = 1; l < IndexList[0]; l++, k++) { /* �������� �м� ����� ���� ��� */
        
        strcpy(one_result.category, fst_info[k]); // category ����
        tabstr.push_back(one_result); // ����
      
        cur_tabstr++; // ����
      
      } // end of for
    } // end of for 2
  } // end of for 1
  
  /* tabstr�� ����� �� �߿� ���� ���ڿ��� ��� */
  for (i = 0; i < cur_tabstr; i++) { /* i = ���� */
    save_ok = 1;
    
    #ifdef _DEBUG_
      fprintf(stderr, "#%d ; (%d, %d, %d):[%s] -> %s\n", i, tabstr[i].x, tabstr[i].y, Len, tabstr[i].str, tabstr[i].category);
    #endif
    
    for (j = 0; j < cur_longest; j++) { /* j = ������ �ִ� �Ͱ� �� */

      /* ������ ���ڿ��� ���� ���ڿ��� ������ �� */
      if ((tabstr[i].x == longest[j].x) && (tabstr[i].y == longest[j].y)) {
        #ifdef _DEBUG_
          fprintf(stderr, "same as before..\n");
        #endif

      }

      /* ������ ���ڿ��� ���� ���ڿ��� ������ �� */
      else if ((tabstr[i].x <= longest[j].x) && (tabstr[i].y >= longest[j].y)) {
        
        #ifdef _DEBUG_
          fprintf(stderr, "replaced..\n");
        #endif
        
        // ��ü
        longest[j] = tabstr[i]; 
        
        save_ok = 0;
      }

      /* ���� ���ڿ��� ���� ���ڿ��� ������ �� */
      else if ((tabstr[i].x >= longest[j].x) && (tabstr[i].y <= longest[j].y)) {

        /* �ƹ� �͵� ���� ������ �� */
        
        #ifdef _DEBUG_
          fprintf(stderr, "included..\n");
        #endif
        
        save_ok = 0;
        break; /* �� �̻� �� �ʿ䰡 ����. */
      }

      /* ��ø�� �� (�� ���� �ϳ��� �߸���) */
      else if ((tabstr[i].y > longest[j].x && tabstr[i].y < longest[j].y)
                || (tabstr[i].y < longest[j].x && tabstr[i].y > longest[j].y)) {

        /* � ó���� �ؾ� �� */
        
        #ifdef _DEBUG_
          fprintf(stderr, "overlapped..\n");
        #endif
        
      }
    }
    
    if (save_ok) { /* �����ؾ� �� �� */
      
      #ifdef _DEBUG_
        fprintf(stderr, "saved..\n");
      #endif
      
      longest.push_back(tabstr[i]);

      cur_longest++; /* �ϳ� ���� */
    }
  }
  
  // ���� ��� ����
  for (i = 0; i < longest.size(); i++) {
    strcpy(one_entity.entity_name, longest[i].str);
    strcpy(one_entity.category, longest[i].category);
    one_entity.pos = longest[i].x;

    result.push_back(one_entity); 
  }

  return 1;
}

/*****************************************************************************/
/* �־��� ���忡�� entity�� ã�Ƽ� tagging�Ѵ�. */
/* src : �Է� ���� */
/* result[] : ����� ����� */
/* return value : 1 : ����, 0 : ���� */
int search_in_string(FST *fst_fst, char **fst_info, int *fst_freq, 
                     char *src, vector<entity_struct> &result) {

  int len = strlen(src);  /* �Է� ���ڿ��� ���� */
  int *Tabular;
  
  int result_num = 0; /* ����� �� */

  #ifdef _DEBUG_
      fprintf(stderr, "\n\n%s\n-----------------------------------------------\n", src);
  #endif
    
  result.clear(); // �ʱ�ȭ

  /* ������ �ʹ� �� ��� ó������ ���� */
  if (len >= MAX_LINE) {
    fprintf(stderr, "ERROR: Too long sentence! [%s]\n", src);
    return 0;
  }
  
  /* �޸� �Ҵ� */    
  if ((Tabular = (int *) malloc(len*(len+1)/2 * sizeof(int))) == NULL) {
    fprintf(stderr, "ERROR: not enough memory!\n");
    return 0;
  }
  
  /* ���峻�� �κй��ڿ� �߿� ������ ��ϵ� ���� ã�´�. */
  int count = FindFSTTabular(fst_fst, src, Tabular);

  /* �߰ߵ� ���ڿ��� ������ */
  if (count) {
    /* ���� ���ڿ��� ã�´�. */
    // result�� ����
    find_sub_strings(fst_fst, fst_info, fst_freq, src, Tabular, count, len, result);

    free(Tabular); /* �޸� ���� */
  }
  
  /////////////////
  // ����ǥ���Ŀ� ���� ���� ��Ī
  mylex(src);

  remove("__temp_r_"); /* �ӽ� ȭ�� ���� */

  // ����� ����ŭ
  for (int i = 0; i < entities_in_lex.size(); i++) {
    result.push_back(entities_in_lex[i]);
  }

  return 1;
}

/*****************************************************************************/
/* �־��� ȭ�Ͽ��� named entity�� ã�Ƽ� tagging�Ѵ�. */
/* return value : 1 = success, 0 = failure */
int search_in_file(FST *fst_fst, char **fst_info, int *fst_freq, char *filename) {
  char outfilename[80];
  
  FILE *fp;
  FILE *outfp;
  
  vector<entity_struct> result;

  int i;
  
  char input_str[MAX_LINE] = {0,}; // �Է� line

  if ((fp = fopen(filename, "rt")) == NULL) {
    fprintf(stderr, "ERROR: Can't open file [%s]!\n", filename); 
    return 0;
  }

  sprintf(outfilename, "%s.out", filename); // ��� ȭ�ϸ�

  // ȭ�� ����
  if ((outfp = fopen(outfilename, "wt")) == NULL) {
    fprintf(stderr, "ERROR: Can't open file [%s]!\n", outfilename); 
    return 0;
  }
  
  // �Է�
  while(fgets(input_str, MAX_LINE, fp) != NULL) { // ���� ����
    if (input_str[0] == '\n') continue;
    input_str[strlen(input_str)-1] = 0;
    
    fprintf(outfp, "\n[Input]\n%s\n", input_str); // �Է�
    ////////////////////////////////////////////////////////////////
    search_in_string(fst_fst, fst_info, fst_freq, input_str, result); 
    ////////////////////////////////////////////////////////////////
    fprintf(outfp, "[Output]\n%"); // ��� 
    
    // ��� ���
    for (i = 0; i < result.size(); i++) {
      fprintf(outfp, "%d\t%s\t%s\n", result[i].pos, result[i].entity_name, result[i].category);
    }
  } // end of while

  // ȭ�� �ݱ�
  fclose(fp);
  fclose(outfp);
  
  return 1;
}

