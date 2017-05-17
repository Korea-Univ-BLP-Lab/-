#include <stdio.h>
#include <string.h>

#include "entry2FST.h"
#include "search_substring.h"

#ifndef MAX_LINE
#define MAX_LINE 5000
#endif

#define FST_Path      "tv_ne.FST"
#define FST_INFO_Path "tv_ne.info"
#define FST_FREQ_Path "tv_ne.freq"

/*****************************************************************************/
char *RunName = "search_substring";
char *Version = "("__DATE__")";
char *Description = "General purpose FST-based dictionary search program";

/*****************************************************************************/
void Help (void) {
    fprintf(stderr, "\nNAME");
    fprintf(stderr, "\n\t%s - %s %s", RunName, Description, Version);
    fprintf(stderr, "\n");
    
    fprintf(stderr, "\nSYNOPSIS");
    fprintf(stderr, "\n\t%s FILE(s)", RunName);
    fprintf(stderr, "\n\t%s [< stdin] [> stdout]", RunName);
    fprintf(stderr, "\n");
    
    fprintf(stderr, "\nDESCRIPTION");
    fprintf(stderr, "\n\tIt recognizes and classifies entities from given FILE(s).");
    fprintf(stderr, "\n\n");
}

/*****************************************************************************/
int main(int argc, char *argv[]) {
  
  int i;

  /* 선언해야 함 */
  FST *fst_fst;
  char **fst_info;
  int *fst_freq;

  /***********************************************/

  if (argc == 2 && (strcmp(argv[1] , "/?") == 0 || 
        strcmp(argv[1], "/h") == 0 || 
        strcmp(argv[1], "--h") == 0)) {
      Help (); 
      return 0;
  }

  /* 리소스 열기 */
  if (!FST_open(FST_Path, FST_INFO_Path, FST_FREQ_Path, &fst_fst, &fst_info, &fst_freq)) {
    fprintf(stderr, "Error: FST_open\n");
    return 0;
  }
  
  /* 주어진 화일의 수만큼 반복 */
  for (i = 1; i < argc; i++) { 
    /************************************************/
    search_in_file(fst_fst, fst_info, fst_freq, argv[i]);
    /************************************************/
  }
  
  /* stdin 입력 */
  if (argc == 1) {
    char input_str[MAX_LINE] = {0,}; /* 입력 line */

    vector<entity_struct> result;
    result.reserve(10);

    int count = 0;

    while(fgets(input_str, MAX_LINE, stdin) != NULL) { /* 어절 단위 */
      
      if (input_str[0] == '\n') continue;
      
      input_str[strlen(input_str)-1] = 0;
      count++; /* 문장 번호 */
    
      /***********************************************************/
      search_in_string(fst_fst, fst_info, fst_freq, input_str, result); 
      /***********************************************************/

      /* 결과 출력 */
      if (!result.size()) /* 결과가 없으면 */
        fprintf(stdout, "[%d sentence] No result!\n", count);

      else { // 결과가 있으면
        fprintf(stdout, "[%d sentence]\n", count);

        // 결과의 수만큼
        for (int i = 0; i < result.size(); i++) {
          /* 문자열의 위치, 문자열(엔터티 명), 카테고리 */
          fprintf(stdout, "%d\t%s\t%s\n", result[i].pos, result[i].entity_name, result[i].category);
        }
      }

      

    } /* end of while */
  } /* end of if */

  /* 리소스 닫기 */
  FST_close(fst_fst, fst_info, fst_freq);
  
  return 1;
}
