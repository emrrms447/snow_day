/*

//_____프로젝트_snow_day_____//
시작일시:20251010

최종수정:20251010

수정정보:

*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include "preprocessing.h"
#include "brain.h"

/*int main()
{
    char input[3000];
    int token_count;

    while (1)
    {
        printf("입력 :");
        if (fgets(input, 3000, stdin) == NULL)
        {
            break;
        }

        unsigned int len = strlen(input);
        if (len > 0 && input[len - 1] == '\n')
        {
            input[len - 1] = '\0';
        }

        if (strcmp(input, "EXIT") == 0) {
            printf("챗봇: 대화를 종료합니다. 잘 가요!\n");
            break;
        }


        char* process_txt = preprocess_text(input);
        if (process_txt != NULL)
        {
            printf("%s\n", process_txt);
            tokenize_sentence(process_txt, &token_count);
            free(process_txt);
        }
        else
        {
            printf("fail\n");
        }









    }
    return 0;
}*/

int main() {
    char user_input[1000];
    char* preprocessed_text = NULL;
    char** tokens = NULL;
    int num_tokens = 0;
    char** stopwords;
    int num_stopwords = 0;
    char** filtered_tokens = NULL;
    int num_filtered_tokens = 0;

    printf("--- 챗봇 전처리 모듈 테스트 시작 ---\n");

    // 1. 사용자 입력 받기
    printf("\n입력 문장을 영어로 입력해주세요 (종료하려면 'exit'):\n");
    if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
        fprintf(stderr, "오류: 입력 실패.\n");
        return 1;
    }
    user_input[strcspn(user_input, "\n")] = 0; // 개행 문자 제거

    if (strcmp(user_input, "exit") == 0) {
        printf("프로그램을 종료합니다.\n");
        return 0;
    }

    printf("\n--- 1. preprocess_text 테스트 ---\n");
    preprocessed_text = preprocess_text(user_input);
    if (preprocessed_text != NULL) {
        printf("원본 입력: \"%s\"\n", user_input);
        printf("전처리 후: \"%s\"\n", preprocessed_text);
    }
    else {
        printf("전처리 실패.\n");
    }


    printf("\n--- 2. tokenize_sentence 테스트 ---\n");
    tokens = tokenize_sentence(preprocessed_text, &num_tokens);
    if (tokens != NULL) {
        printf("토큰화된 단어 개수: %d\n", num_tokens);
        printf("토큰 목록: ");
        for (int i = 0; i < num_tokens; i++) {
            printf("'%s' ", tokens[i]);
        }
        printf("\n");
    }
    else {
        printf("토큰화 실패 또는 토큰 없음.\n");
    }


    printf("\n--- 3. load_stopwords 테스트 ---\n");
    // stopwords.txt 파일이 없으면 테스트 실패. 미리 생성해두어야 함.
    // 내용은 이전 답변에서 제공된 영문 불용어 목록을 사용.
    stopwords = load_stopwords("stopwords.txt", &num_stopwords);
    if (stopwords != NULL) {
        printf("로드된 불용어 개수: %d\n", num_stopwords);
        printf("일부 불용어 (최대 5개): ");
        for (int i = 0; i < (num_stopwords > 5 ? 5 : num_stopwords); i++) {
            printf("'%s' ", stopwords[i]);
        }
        printf("\n");
    }
    else {
        printf("불용어 로드 실패.\n");
    }

    printf("\n--- 4. remove_stopwords 테스트 ---\n");
    filtered_tokens = remove_stopwords(tokens, num_tokens, stopwords, num_stopwords, &num_filtered_tokens);
    if (filtered_tokens != NULL) {
        printf("불용어 제거 후 남은 단어 개수: %d\n", num_filtered_tokens);
        printf("필터링된 단어 목록: ");
        for (int i = 0; i < num_filtered_tokens; i++) {
            printf("'%s' ", filtered_tokens[i]);
        }
        printf("\n");
    }
    else {
        printf("불용어 제거 실패 또는 남은 단어 없음.\n");
    }

    return 0;
}