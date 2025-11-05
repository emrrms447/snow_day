#ifndef BRAIN_H
#define BRAIN_H

typedef struct {
    char* name;          // 의도
    char** keywords;     // 해당 의도와 관련된 키워드들의 배열 (예: {"날씨", "기온", "오늘"})
    int num_keywords;    // 키워드의 개수
} Intent;

Intent* load_intent_rules(const char* filename, int* num_intents);
void free_intent_rules(Intent* intents, int num_intents);
char* identify_intent(char** filtered_tokens, int num_filtered_tokens, Intent* intents, int num_intents);

#endif