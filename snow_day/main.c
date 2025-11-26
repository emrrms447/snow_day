#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <time.h>
#include "preprocessing.h"
#include "brain.h"
#include "response.h"

/*

//_____프로젝트_snow_day_____//
시작일시:20251010

최종수정:20251010

수정정보:

*/

// --- 메인 함수 (챗봇 통합 테스트 코드) ---
int main() {
    // ---- 챗봇 초기화 (Random Seed 설정) ----
    srand((unsigned int)time(NULL)); // 난수 생성기 시드 설정 (프로그램 시작 시 한 번만 호출!)

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
    ResponseRule* response_rules = NULL; // 응답 규칙 추가
    int num_response_rules = 0;          // 응답 규칙 개수
    char* identified_intent_name = NULL;
    char* chatbot_response = NULL;       // 챗봇 응답

    printf("--- 챗봇 전처리, 의도 파악, 응답 생성 모듈 통합 테스트 시작 ---\n");
    printf("\n-- 필수 파일 로드 --\n");

    // 1. 불용어 로드
    stopwords = load_stopwords("stopwords.txt", &num_stopwords);
    if (stopwords == NULL) {
        fprintf(stderr, "Test Error: 불용어 로드 실패! 'stopwords.txt' 파일을 확인하세요.\n");
        goto cleanup;
    }
    printf("성공: 불용어 %d개 로드 완료.\n", num_stopwords);

    // 2. 의도 규칙 로드
    intents = load_intent_rules("intent_rules.txt", &num_intents);
    if (intents == NULL) {
        fprintf(stderr, "Test Error: 의도 규칙 로드 실패! 'intent_rules.txt' 파일을 확인하세요.\n");
        goto cleanup;
    }
    printf("성공: 의도 %d개 로드 완료.\n", num_intents);

    // 3. 응답 규칙 로드 (네가 구현할 부분)
    // Placeholder - 네가 load_response_rules 함수를 구현하고 나면 이 주석을 풀고 사용해.
    response_rules = load_response_rules("response_rules.txt", &num_response_rules);
    if (response_rules == NULL) {
        fprintf(stderr, "Test Error: 응답 규칙 로드 실패! 'response_rules.txt' 파일을 확인하세요.\n");
        goto cleanup;
    }
    printf("성공: 응답 규칙 %d개 로드 완료.\n", num_response_rules);


    // --- 사용자 입력 루프 ---
    while (1) {
        printf("\n\n--- 챗봇 대화 시작 (종료: 'exit' , 설정 : 'setting' ) ----\n");
        printf(">> 당신: ");
        if (fgets(user_input_buffer, sizeof(user_input_buffer), stdin) == NULL) {
            fprintf(stderr, "Error: 사용자 입력 실패.\n");
            goto cleanup;
        }
        user_input_buffer[strcspn(user_input_buffer, "\n")] = 0; // 개행 문자 제거

        if (strcmp(user_input_buffer, "exit") == 0) {
            printf("챗봇: 프로그램이 종료됩니다. 안녕!\n");
            goto cleanup;
        }

        if (strcmp(user_input_buffer, "setting") == 0)
        {
            printf("챗봇: 설정모드로 진입합니다. \n");
            save_intent_response("intent_rules.txt", "response_rules.txt", num_intents, intents);
            goto cleanup;
        }

        // --- 각 단계별 처리 ---
        printf("--- 처리 중... ---\n");

        // 1. 전처리 (preprocess_text)
        preprocessed_text = preprocess_text(user_input_buffer);
        if (preprocessed_text == NULL || strlen(preprocessed_text) == 0) {
            fprintf(stderr, "Warning: 전처리된 텍스트가 없거나 비어 있습니다.\n");
            chatbot_response = _strdup("I'm sorry, I couldn't process your input.");
            continue;
            //goto respond_and_cleanup_loop;
        }
        printf("[Debug] 전처리 후: \"%s\"\n", preprocessed_text);

        // 2. 토큰화 (tokenize_sentence)
        tokens = tokenize_sentence(preprocessed_text, &num_tokens);
        if (tokens == NULL) {
            fprintf(stderr, "Warning: 토큰화 실패 또는 토큰 없음.\n");
            chatbot_response = _strdup("I couldn't understand the words you used.");
            continue;
            //goto respond_and_cleanup_loop;
        }
        printf("[Debug] 토큰화된 단어 개수: %d, 목록: ", num_tokens);
        for (int i = 0; i < num_tokens; i++) printf("'%s' ", tokens[i]); printf("\n");

        // 3. 불용어 제거 (remove_stopwords)
        filtered_tokens = remove_stopwords(tokens, num_tokens, stopwords, num_stopwords, &num_filtered_tokens);
        if (filtered_tokens == NULL) {
            fprintf(stderr, "Warning: 불용어 제거 후 남은 단어가 없습니다.\n");
            chatbot_response = _strdup("Your input is too short or all common words.");
            continue;
            //goto respond_and_cleanup_loop;
        }
        printf("[Debug] 필터링된 단어 개수: %d, 목록: ", num_filtered_tokens);
        for (int i = 0; i < num_filtered_tokens; i++) printf("'%s' ", filtered_tokens[i]); printf("\n");

        // 4. 의도 파악 (identify_intent)
        identified_intent_name = identify_intent(filtered_tokens, num_filtered_tokens, intents, num_intents);
        if (identified_intent_name == NULL) {
            // printf("[Debug] 파악된 의도: (알 수 없음)\n");
            identified_intent_name = _strdup("unknown"); // 의도를 못 찾았을 때 "unknown" 의도로 간주하여 응답 생성.
            if (identified_intent_name == NULL) {
                fprintf(stderr, "Error: 'unknown' 의도 메모리 할당 실패.\n");
                continue;
                //goto respond_and_cleanup_loop;
            }
        }
        printf("[DEBUG] --- IDENTIFY_INTENT CALL START ---\n");
        printf("[DEBUG] Filtered tokens (%d): ", num_filtered_tokens);
        for (int i = 0; i < num_filtered_tokens; i++) {
            printf("'%s' ", filtered_tokens[i]);
        }
        printf("\n");

        identified_intent_name = identify_intent(filtered_tokens, num_filtered_tokens, intents, num_intents);

        // [디버그 끝] identify_intent 반환 값 확인
        if (identified_intent_name == NULL) {
            printf("[DEBUG] Identified Intent: NULL (meaning no intent matched)\n");
            identified_intent_name = _strdup("unknown"); // main에서 "unknown"으로 덮어씀
            if (identified_intent_name == NULL) {
                fprintf(stderr, "Error: 'unknown' intent memory allocation failed.\n");
                goto respond_and_cleanup_loop;
            }
        }
        else {
            printf("[DEBUG] Identified Intent: **%s**\n", identified_intent_name);
        }
        printf("[DEBUG] --- IDENTIFY_INTENT CALL END ---\n");


        // 5. 응답 생성 (generate_response)
        // Placeholder - 네가 generate_response 함수를 구현하고 나면 이 주석을 풀고 사용해.
        // 이 부분은 identified_intent_name 과 response_rules, num_response_rules를 활용해야 함.
        // 만약 response_rules를 로드하지 않았다면 기본 응답을 하도록.
        if (response_rules != NULL) {
            chatbot_response = generate_response(identified_intent_name, response_rules, num_response_rules);
        }
        else { // 응답 규칙 파일 로드에 실패했거나, 네가 아직 구현하지 않았다면 기본 응답
            char temp_response[256];
            snprintf(temp_response, 256, "I identified your intent as '%s', but I don't know how to respond yet.", identified_intent_name);
            chatbot_response = _strdup(temp_response);
        }


    respond_and_cleanup_loop:
        printf(">> 챗봇: %s\n", chatbot_response != NULL ? chatbot_response : "I'm having trouble responding right now.");

        // 루프 내에서 할당된 메모리 해제
        //free_char_ptr(preprocessed_text);
        //free_tokens_array(tokens, num_tokens);
        //free_tokens_array(filtered_tokens, num_filtered_tokens);
        //free_char_ptr(identified_intent_name);
        //free_char_ptr(chatbot_response);
        if (strcmp(identified_intent_name, "exit") == 0)
        {
            goto cleanup;
        }
        // 포인터 초기화 및 카운트 초기화 (다음 루프를 위해)
        preprocessed_text = NULL; tokens = NULL; filtered_tokens = NULL; identified_intent_name = NULL; chatbot_response = NULL;
        num_tokens = 0; num_filtered_tokens = 0;
    }

cleanup: // 프로그램 전체 종료 시 할당된 모든 메모리 해제
    printf("\n--- 챗봇 테스트 종료 --- (모든 동적 할당 메모리 해제 시작)\n");
    free_stopwords_array(stopwords, num_stopwords);
    free_intent_rules(intents, num_intents);
    //free_response_rules(response_rules, num_response_rules);
    printf("--- 모든 동적 할당 메모리 해제 완료 ---\n");

    return 0;
}   