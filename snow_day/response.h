#ifndef RESPONSE_H
#define RESPONSE_H

typedef struct {
    char* intent_name;      // 매칭될 의도의 이름 (예: "greeting", "weather")
    char** responses;       // 해당 의도에 대한 응답 문장들의 배열 (예: "Hello!", "How can I help you?")
    int num_responses;      // 응답 문장의 개수
} ResponseRule;

ResponseRule* load_response_rules(const char* filename, int* num_rules);
char* generate_response(const char* intent_name, ResponseRule* Rules,int num_response_rules);

#endif