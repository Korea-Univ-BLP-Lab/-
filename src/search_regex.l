%{
#include <string.h>
#include <ctype.h>
#include "search_substring.h"

//char *RunName = "search_regex";
//char *Version = "("__DATE__")";
//char *Description = "";

#define TIME     "time"
#define DATE     "date"
#define CHANNEL  "channel"
#define DURATION "duration"
#define ETCTIME  "etctime"

vector<entity_struct> entities_in_lex;

entity_struct one_entity_in_lex;

int cur_pos_yylex = 0;
int is_whitespace = 1;

//int new_sentence = 1; /* 문장 시작을 알리는 변수 */

%}

LETTER         [A-Za-z]
POSITIVE_DIGIT [1-9]
DIGIT          [0-9]
HANGUL_DIGIT          (하나|한|둘|두|셋|세|넷|네|다섯|여섯|일곱|여덟|아홉)
HANJA_DIGIT         (일|이|삼|사|오|육|륙|칠|팔|구)
HANGUL_DIGIT2         (열|스물|스무|서른|마흔|쉰|예순|일흔|여든|아흔)
HAN_RADIX        (십|백|천|만|억|조)

HANJA_THOUSAND       {HANJA_DIGIT}?(천)?{HANJA_DIGIT}?(백)?{HANJA_DIGIT}?(십)?{HANJA_DIGIT}?
ARABIC_THOUSAND      ({POSITIVE_DIGIT}{DIGIT}{DIGIT}{DIGIT})|({POSITIVE_DIGIT}{DIGIT}{DIGIT})|({POSITIVE_DIGIT}{DIGIT})|({POSITIVE_DIGIT})
HANJA_BIGNUM         {HANJA_THOUSAND}?(조[ ]?)?{HANJA_THOUSAND}?(억[ ]?)?{HANJA_THOUSAND}?(만[ ]?)?{HANJA_THOUSAND}?
ARABIC_BIGNUM        {ARABIC_THOUSAND}?(조[ ]?)?{ARABIC_THOUSAND}?(억[ ]?)?{ARABIC_THOUSAND}?(만[ ]?)?{ARABIC_THOUSAND}?

INTEGER_NUM    [0-9]+
FLOAT_NUM      -?[0-9]*\.[0-9]+
HYPHEN         \-
UNDERSCORE     \_
DOT            \.
DOUBLE_DOT     \.\.
COMMA          \,
COLON          \:
ALPHANUMERIC   [A-Za-z0-9.,+*()/$-]
WHITE_SPACE    [ \t\r]+
LPAREN         \(
RPAREN         \)

DAY_NUMBER    {POSITIVE_DIGIT}|[1-2]{DIGIT}|30|31
MONTH_NUMBER  {POSITIVE_DIGIT}|(10|11|12)
YEAR_NUMBER   ({POSITIVE_DIGIT}{DIGIT})|({POSITIVE_DIGIT}{DIGIT}{DIGIT}{DIGIT})

YEAR          {YEAR_NUMBER}년
MONTH 				{MONTH_NUMBER}월
DAY   				{DAY_NUMBER}일

YEAR_MONTH_DAY      {YEAR}[ ]?{MONTH}[ ]?{DAY}
YEAR_MONTH          {YEAR}[ ]?{MONTH}
MONTH_DAY           {MONTH}[ ]?{DAY}

TIME_NUMBER   ({DIGIT}|(0|1){DIGIT}|(20|21|22|23|24))|(한|두|세|네|다섯|여섯|일곱|여덟|아홉|열|열한|열두)
MIN_NUMBER    ({POSITIVE_DIGIT}|[1-5]{DIGIT})|(({HANJA_DIGIT}십)?{HANJA_DIGIT}|({HANJA_DIGIT}십))

TIME          ({TIME_NUMBER}시)|((오전|오후)[ ]?{TIME_NUMBER}시)
MIN           ({MIN_NUMBER}분)|(반)
SEC           {MIN_NUMBER}초

TIME_MIN_SEC      {TIME}[ ]?{MIN}[ ]?{SEC}
TIME_MIN          {TIME}[ ]?{MIN}
MIN_SEC           {MIN}[ ]?{SEC}

TIME_BEFOREAFTER    ({TIME}|{TIME_MIN})[ ]?(이전|이후|까지)

DURATION_TIME       (({YEAR_MONTH_DAY}|{MONTH_DAY}|{DAY})[ ])?{TIME}(부터|[ ]?[\~\-])[ ]?(({YEAR_MONTH_DAY}|{MONTH_DAY}|{DAY})[ ])?{TIME}(까지)?
DURATION_DAY        ({YEAR_MONTH_DAY}|{MONTH_DAY}|{DAY})(부터|[ ]?[\~\-])[ ]?({YEAR_MONTH_DAY}|{MONTH_DAY}|{DAY})(까지)?
DURATION_MONTH      ({YEAR_MONTH}|{MONTH})(부터|[ ]?[\~\-])[ ]?({YEAR_MONTH}|{MONTH})(까지)?
DURATION_YEAR       {YEAR}(부터|[ ]?[\~\-])[ ]?{YEAR}(까지)?

CHANNEL             {POSITIVE_DIGIT}{DIGIT}?{DIGIT}?번

%%

{HANJA_DIGIT} {
/*	if (is_whitespace) {
		fprintf(stdout, "<HANJA_DIGIT>%s</HANJA_DIGIT>\n", yytext);
		fprintf(stdout, "(시작위치, 길이) = (%d, %d)\n", cur_pos_yylex, yyleng);	cur_pos_yylex += yyleng;
	}
	is_whitespace = 0;
	*/
}

{HANJA_BIGNUM}|{ARABIC_BIGNUM} {
/*	if (is_whitespace) {
		fprintf(stdout, "<NUMBER>%s</NUMBER>\n", yytext);
		fprintf(stdout, "(시작위치, 길이) = (%d, %d)\n", cur_pos_yylex, yyleng);	cur_pos_yylex += yyleng;
	}
	is_whitespace = 0;
*/
}

{DURATION_TIME}|{DURATION_DAY}|{DURATION_MONTH}|{DURATION_YEAR} {
	if (is_whitespace) {
		strcpy(one_entity_in_lex.entity_name, yytext);
		strcpy(one_entity_in_lex.category, DURATION);  // 기간
		one_entity_in_lex.pos = cur_pos_yylex;
		
		entities_in_lex.push_back(one_entity_in_lex);
		cur_pos_yylex += yyleng;
	}	
	is_whitespace = 0;
}

{YEAR}|{MONTH}|{DAY}|{YEAR_MONTH_DAY}|{YEAR_MONTH}|{MONTH_DAY} {
	if (is_whitespace) {
		strcpy(one_entity_in_lex.entity_name, yytext);
		strcpy(one_entity_in_lex.category, DATE);  // 날짜
		one_entity_in_lex.pos = cur_pos_yylex;
		
		entities_in_lex.push_back(one_entity_in_lex);
		cur_pos_yylex += yyleng;
  }
  is_whitespace = 0;
}

{TIME}|{MIN}|{SEC}|{TIME_MIN_SEC}|{TIME_MIN}|{MIN_SEC} {
	if (is_whitespace) {
		strcpy(one_entity_in_lex.entity_name, yytext);
		strcpy(one_entity_in_lex.category, TIME);  // 시간
		one_entity_in_lex.pos = cur_pos_yylex;
		
		entities_in_lex.push_back(one_entity_in_lex);
		cur_pos_yylex += yyleng;
	}
	is_whitespace = 0;
}

{TIME_BEFOREAFTER} {
	if (is_whitespace) {
		strcpy(one_entity_in_lex.entity_name, yytext);
		strcpy(one_entity_in_lex.category, ETCTIME);  // 기타시간
		one_entity_in_lex.pos = cur_pos_yylex;
		
		entities_in_lex.push_back(one_entity_in_lex);
		cur_pos_yylex += yyleng;
	}
	is_whitespace = 0;
}

{CHANNEL}/에 {
	if (is_whitespace) {
		strcpy(one_entity_in_lex.entity_name, yytext);
		strcpy(one_entity_in_lex.category, CHANNEL);  // 채널
		one_entity_in_lex.pos = cur_pos_yylex;
		
		entities_in_lex.push_back(one_entity_in_lex);
		cur_pos_yylex += yyleng;
  }
  is_whitespace = 0;
}

{WHITE_SPACE} {
	is_whitespace = 1;
	cur_pos_yylex += yyleng;
}

. {
  cur_pos_yylex += yyleng;
  is_whitespace = 0;
}

%%

/*****************************************************************************/
/* str을 임시화일에 저장한 후 */
/* 임시 화일로부터 scan해서 작업을 하고 */
/* 결과는 임시화일에 저장 */
int mylex(char *str) {
  FILE *fp;
  
  fp = fopen("__temp_r_", "wt"); /* 화일 열기 */
  
  fprintf(fp, "%s", str); /* 화일에 출력 */
  
  fclose(fp); /* 화일 닫기 */
  
  /***************************************/
  yyin = fopen("__temp_r_", "rt");

  cur_pos_yylex = 0;
  is_whitespace = 1;
  entities_in_lex.clear(); // 초기화
  
  yylex();
  return 1;
}

/*****************************************************************************/
int yywrap() {
	if (yyin) fclose(yyin);
  return 1;
}

/*****************************************************************************/
#define MAIN_LEX
#ifdef MAIN_LEX
int main (int argc, char *argv[]) {
  char line[10000];

  while(fgets(line, 10000, stdin) != NULL) { /* 어절 읽기 */
    line[strlen(line)-1] = 0; /* 개행 문자 삭제 */
    
    mylex(line); /* 어절에 대한 분석 및 태그 변환 */

  }
  remove("__temp_r_"); /* 임시 화일 삭제 */
}

#endif
