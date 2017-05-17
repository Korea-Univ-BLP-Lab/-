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

  /* �����ؾ� �� */
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

  /* ���ҽ� ���� */
  if (!FST_open(FST_Path, FST_INFO_Path, FST_FREQ_Path, &fst_fst, &fst_info, &fst_freq)) {
    fprintf(stderr, "Error: FST_open\n");
    return 0;
  }
  
  /* �־��� ȭ���� ����ŭ �ݺ� */
  for (i = 1; i < argc; i++) { 
    /************************************************/
    search_in_file(fst_fst, fst_info, fst_freq, argv[i]);
    /************************************************/
  }
  
  /* stdin �Է� */
  if (argc == 1) {
    char input_str[MAX_LINE] = {0,}; /* �Է� line */

    vector<entity_struct> result;
    result.reserve(10);

    int count = 0;

    while(fgets(input_str, MAX_LINE, stdin) != NULL) { /* ���� ���� */
      
      if (input_str[0] == '\n') continue;
      
      input_str[strlen(input_str)-1] = 0;
      count++; /* ���� ��ȣ */
    
      /***********************************************************/
      search_in_string(fst_fst, fst_info, fst_freq, input_str, result); 
      /***********************************************************/

      /* ��� ��� */
      if (!result.size()) /* ����� ������ */
        fprintf(stdout, "[%d sentence] No result!\n", count);

      else { // ����� ������
        fprintf(stdout, "[%d sentence]\n", count);

        // ����� ����ŭ
        for (int i = 0; i < result.size(); i++) {
          /* ���ڿ��� ��ġ, ���ڿ�(����Ƽ ��), ī�װ� */
          fprintf(stdout, "%d\t%s\t%s\n", result[i].pos, result[i].entity_name, result[i].category);
        }
      }

      

    } /* end of while */
  } /* end of if */

  /* ���ҽ� �ݱ� */
  FST_close(fst_fst, fst_info, fst_freq);
  
  return 1;
}
