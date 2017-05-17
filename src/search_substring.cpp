//#define _DEBUG_ /* debugging 시에 선언 */

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
/* 최장의 부분 문자열을 찾는다. */
int find_sub_strings(FST *fst_fst, char **fst_info, int *fst_freq, 
                     char *src, int *Tabular, int count, int Len, 
                     vector<entity_struct> &result) {
  int i, j, l;
  int pos;
  
  vector<tab_cell> tabstr; /* 사전에 있는 부분 문자열의 tab 값을 저장 */
  int cur_tabstr = 0; /* tabstr의 현재 위치를 가리키는 포인터 */

  vector<tab_cell> longest; /* 사전에 있는 부분 문자열의 tab 값을 저장 */
  int cur_longest = 0;
  
  int save_ok; /* 저장 여부 결정 */

  tab_cell one_result;
  entity_struct one_entity;

  if (count == 0) {
    return 0;
  }
  
  char str[MAX_LINE];
  int k = 0;

  /* 사전에 있는 부분문자열을 tabstr에 저장 */
  for (i = 0; i < Len /*&& cur_tabstr < count*/; i++) {
    for (j = 1+i; j <= Len /*&& cur_tabstr < count*/; j++) {

      pos = TabPos2(i, j, Len);

      if (Tabular[pos] == -1) {
        continue;
      }

      // 문장 처음(i == 0)이거나, 앞 글자가 공백인 경우만 저장
      if (i > 0 && !isspace(src[i-1])) {

        #ifdef _DEBUG_
        fprintf(stderr, "cannot save! not beginning of a word\n");
        #endif

        continue;
      }

      Hash2String(fst_fst, Tabular[pos], str); /* 부분 문자열 저장 */

      one_result.x = i;
      one_result.y = j;
      strcpy(one_result.str, str);
      strcpy(one_result.category, fst_info[Tabular[pos]]); // category 저장

      tabstr.push_back(one_result); // 저장
      
      cur_tabstr++; // 증가

      int IndexList[2];
      k = String2Hash(fst_fst, str, IndexList);

      for (k++, l = 1; l < IndexList[0]; l++, k++) { /* 복수개의 분석 결과가 있을 경우 */
        
        strcpy(one_result.category, fst_info[k]); // category 저장
        tabstr.push_back(one_result); // 저장
      
        cur_tabstr++; // 증가
      
      } // end of for
    } // end of for 2
  } // end of for 1
  
  /* tabstr에 저장된 것 중에 최장 문자열만 골라냄 */
  for (i = 0; i < cur_tabstr; i++) { /* i = 현재 */
    save_ok = 1;
    
    #ifdef _DEBUG_
      fprintf(stderr, "#%d ; (%d, %d, %d):[%s] -> %s\n", i, tabstr[i].x, tabstr[i].y, Len, tabstr[i].str, tabstr[i].category);
    #endif
    
    for (j = 0; j < cur_longest; j++) { /* j = 기존에 있는 것과 비교 */

      /* 현재의 문자열이 기존 문자열과 동일할 때 */
      if ((tabstr[i].x == longest[j].x) && (tabstr[i].y == longest[j].y)) {
        #ifdef _DEBUG_
          fprintf(stderr, "same as before..\n");
        #endif

      }

      /* 현재의 문자열이 기존 문자열을 포함할 때 */
      else if ((tabstr[i].x <= longest[j].x) && (tabstr[i].y >= longest[j].y)) {
        
        #ifdef _DEBUG_
          fprintf(stderr, "replaced..\n");
        #endif
        
        // 교체
        longest[j] = tabstr[i]; 
        
        save_ok = 0;
      }

      /* 기존 문자열이 현재 문자열을 포함할 때 */
      else if ((tabstr[i].x >= longest[j].x) && (tabstr[i].y <= longest[j].y)) {

        /* 아무 것도 하지 않으면 됨 */
        
        #ifdef _DEBUG_
          fprintf(stderr, "included..\n");
        #endif
        
        save_ok = 0;
        break; /* 더 이상 볼 필요가 없다. */
      }

      /* 중첩될 때 (둘 중의 하나는 잘못됨) */
      else if ((tabstr[i].y > longest[j].x && tabstr[i].y < longest[j].y)
                || (tabstr[i].y < longest[j].x && tabstr[i].y > longest[j].y)) {

        /* 어떤 처리를 해야 함 */
        
        #ifdef _DEBUG_
          fprintf(stderr, "overlapped..\n");
        #endif
        
      }
    }
    
    if (save_ok) { /* 저장해야 할 때 */
      
      #ifdef _DEBUG_
        fprintf(stderr, "saved..\n");
      #endif
      
      longest.push_back(tabstr[i]);

      cur_longest++; /* 하나 증가 */
    }
  }
  
  // 최종 결과 저장
  for (i = 0; i < longest.size(); i++) {
    strcpy(one_entity.entity_name, longest[i].str);
    strcpy(one_entity.category, longest[i].category);
    one_entity.pos = longest[i].x;

    result.push_back(one_entity); 
  }

  return 1;
}

/*****************************************************************************/
/* 주어진 문장에서 entity를 찾아서 tagging한다. */
/* src : 입력 문장 */
/* result[] : 결과가 저장됨 */
/* return value : 1 : 성공, 0 : 실패 */
int search_in_string(FST *fst_fst, char **fst_info, int *fst_freq, 
                     char *src, vector<entity_struct> &result) {

  int len = strlen(src);  /* 입력 문자열의 길이 */
  int *Tabular;
  
  int result_num = 0; /* 결과의 수 */

  #ifdef _DEBUG_
      fprintf(stderr, "\n\n%s\n-----------------------------------------------\n", src);
  #endif
    
  result.clear(); // 초기화

  /* 문장이 너무 긴 경우 처리하지 않음 */
  if (len >= MAX_LINE) {
    fprintf(stderr, "ERROR: Too long sentence! [%s]\n", src);
    return 0;
  }
  
  /* 메모리 할당 */    
  if ((Tabular = (int *) malloc(len*(len+1)/2 * sizeof(int))) == NULL) {
    fprintf(stderr, "ERROR: not enough memory!\n");
    return 0;
  }
  
  /* 문장내의 부분문자열 중에 사전에 등록된 것을 찾는다. */
  int count = FindFSTTabular(fst_fst, src, Tabular);

  /* 발견된 문자열이 있으면 */
  if (count) {
    /* 최장 문자열을 찾는다. */
    // result에 저장
    find_sub_strings(fst_fst, fst_info, fst_freq, src, Tabular, count, len, result);

    free(Tabular); /* 메모리 해제 */
  }
  
  /////////////////
  // 정규표현식에 의한 패턴 매칭
  mylex(src);

  remove("__temp_r_"); /* 임시 화일 삭제 */

  // 결과의 수만큼
  for (int i = 0; i < entities_in_lex.size(); i++) {
    result.push_back(entities_in_lex[i]);
  }

  return 1;
}

/*****************************************************************************/
/* 주어진 화일에서 named entity를 찾아서 tagging한다. */
/* return value : 1 = success, 0 = failure */
int search_in_file(FST *fst_fst, char **fst_info, int *fst_freq, char *filename) {
  char outfilename[80];
  
  FILE *fp;
  FILE *outfp;
  
  vector<entity_struct> result;

  int i;
  
  char input_str[MAX_LINE] = {0,}; // 입력 line

  if ((fp = fopen(filename, "rt")) == NULL) {
    fprintf(stderr, "ERROR: Can't open file [%s]!\n", filename); 
    return 0;
  }

  sprintf(outfilename, "%s.out", filename); // 출력 화일명

  // 화일 열기
  if ((outfp = fopen(outfilename, "wt")) == NULL) {
    fprintf(stderr, "ERROR: Can't open file [%s]!\n", outfilename); 
    return 0;
  }
  
  // 입력
  while(fgets(input_str, MAX_LINE, fp) != NULL) { // 어절 단위
    if (input_str[0] == '\n') continue;
    input_str[strlen(input_str)-1] = 0;
    
    fprintf(outfp, "\n[Input]\n%s\n", input_str); // 입력
    ////////////////////////////////////////////////////////////////
    search_in_string(fst_fst, fst_info, fst_freq, input_str, result); 
    ////////////////////////////////////////////////////////////////
    fprintf(outfp, "[Output]\n%"); // 출력 
    
    // 결과 출력
    for (i = 0; i < result.size(); i++) {
      fprintf(outfp, "%d\t%s\t%s\n", result[i].pos, result[i].entity_name, result[i].category);
    }
  } // end of while

  // 화일 닫기
  fclose(fp);
  fclose(outfp);
  
  return 1;
}

