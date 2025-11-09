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

int main() {
    char user_input_buffer[256];
    char* preprocessed_text = NULL;
    char** tokens = NULL;
    int num_tokens = 0;
    char** stopwords = NULL;
    int num_stopwords = 0;
    char** filtered_tokens = NULL;
    int num_filtered_tokens = 0;
    Intent* intents = NULL;
    int num_intents = 0;
    char* identified_intent_name = NULL;

    printf("--- 챗봇 전처리 및 의도 파악 모듈 통합 테스트 시작 ---\n");
    printf("\n-- 파일 로드 --\n");

    // 불용어 로드
    stopwords = load_stopwords("stopwords.txt", &num_stopwords);
    if (stopwords == NULL) {
        fprintf(stderr, "Test Error: 불용어 로드 실패!\n");
        goto cleanup;
    }
    printf("성공: 불용어 %d개 로드 완료.\n", num_stopwords);

    // 의도 규칙 로드
    intents = load_intent_rules("intent_rules.txt", &num_intents);
    if (intents == NULL) {
        fprintf(stderr, "Test Error: 의도 규칙 로드 실패!\n");
        goto cleanup;
    }
    printf("성공: 의도 %d개 로드 완료.\n", num_intents);

    // --- 사용자 입력 루프 ---
    while (1) {
        printf("\n\n--- 사용자 입력 ----\n");
        printf("영어로 문장을 입력해주세요 (종료하려면 'exit'):\n> ");
        if (fgets(user_input_buffer, sizeof(user_input_buffer), stdin) == NULL) {
            fprintf(stderr, "Error: 입력 실패.\n");
            break;
        }
        user_input_buffer[strcspn(user_input_buffer, "\n")] = 0; // 개행 문자 제거

        if (strcmp(user_input_buffer, "exit") == 0) {
            printf("프로그램을 종료합니다.\n");
            break;
        }

        // --- 1. 전처리 (preprocess_text) ---
        printf("\n-- 1. 텍스트 전처리 --\n");
        preprocessed_text = preprocess_text(user_input_buffer);
        if (preprocessed_text != NULL) {
            printf("원본 입력: \"%s\"\n", user_input_buffer);
            printf("전처리 후: \"%s\"\n", preprocessed_text);
        }
        else {
            fprintf(stderr, "Error: 전처리 실패.\n");
            goto cleanup_loop; // 루프 내에서 할당된 메모리만 해제하고 다음 루프.
        }

        // --- 2. 토큰화 (tokenize_sentence) ---
        printf("\n-- 2. 문장 토큰화 --\n");
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
            fprintf(stderr, "Warning: 토큰화 실패 또는 토큰 없음.\n");
            goto cleanup_loop;
        }

        // --- 3. 불용어 제거 (remove_stopwords) ---
        printf("\n-- 3. 불용어 제거 --\n");
        filtered_tokens = remove_stopwords(tokens, num_tokens, stopwords, num_stopwords, &num_filtered_tokens);
        if (filtered_tokens != NULL) {
            printf("필터링된 단어 개수: %d\n", num_filtered_tokens);
            printf("필터링된 단어 목록: ");
            for (int i = 0; i < num_filtered_tokens; i++) {
                printf("'%s' ", filtered_tokens[i]);
            }
            printf("\n");
        }
        else {
            fprintf(stderr, "Warning: 불용어 제거 실패 또는 남은 단어 없음.\n");
            goto cleanup_loop;
        }

        // --- 4. 의도 파악 (identify_intent) ---
        printf("\n-- 4. 의도 파악 --\n");
        identified_intent_name = identify_intent(filtered_tokens, num_filtered_tokens, intents, num_intents);
        if (identified_intent_name != NULL) {
            printf("파악된 의도: **%s**\n", identified_intent_name);
        }
        else {
            printf("파악된 의도: (알 수 없음)\n");
        }

    cleanup_loop: // 루프 내에서 할당된 메모리 해제
        free_char_ptr(preprocessed_text);
        free_tokens_array(tokens, num_tokens);
        free_tokens_array(filtered_tokens, num_filtered_tokens); // remove_stopwords 결과도 free_tokens_array로 해제 가능.
        free_char_ptr(identified_intent_name); // identify_intent가 _strdup 했으므로 해제.

        preprocessed_text = NULL; tokens = NULL; filtered_tokens = NULL; identified_intent_name = NULL;
        num_tokens = 0; num_filtered_tokens = 0; // 초기화
    }

cleanup: // 프로그램 전체 종료 시 할당된 모든 메모리 해제
    printf("\n--- 모든 메모리 해제 시작 ---\n");
    free_stopwords_array(stopwords, num_stopwords);
    free_intent_rules(intents, num_intents);
    printf("--- 모든 메모리 해제 완료 ---\n");
    printf("--- 챗봇 테스트 종료 ---\n");

    return 0;
}