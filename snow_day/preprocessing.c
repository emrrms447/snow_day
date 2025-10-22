#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/*

//_____preprocessing.c_____//
시작일시:20251010

최종수정:20251013

목표 : 문자를 컴퓨터가 알아먹기 편하게 만들기

*/

char* preprocess_text(const char* input_text) //영어를 소문자로 바꾸고 특수문자는 제거
{
    char* p = input_text;
    char* process_txt = (char*)malloc(sizeof(char) * (strlen(input_text)+1));
    if (process_txt == NULL) 
    {
        return NULL;
    }
    char* answer = process_txt;

    while (*p != '\0')
    {
        if (isalpha(*p)) //알파벳이면 소문자로 변환
        {
            *process_txt = tolower(*p);
            process_txt++;
            p++;
        }
        else if(isdigit(*p)||isspace(*p)) //숫자, 공백은 그대로 전달
        {
            *process_txt = *p;
            process_txt++;
            p++;
        }
        else //기타 특수문자는 전부 삭제
        {
            p++;
        }
    }
    *process_txt = '\0';

    return answer;
}


char** tokenize_sentence(char* process_txt, int* num_token) //문자 토큰화 == 문자별로 새로운 공간을 할당해줘야함
{
    (*num_token) = 0; //토큰 개수 초기화
    char* copy_txt = _strdup(process_txt); //문자열 복사하기 ( 원본 문자열 회손 방지 )
    if (copy_txt == NULL)
    {
        return NULL;
    }

    char** answer = (char**)malloc(sizeof(char*) * strlen(process_txt)); //2차원 배열 할당
    if (answer == NULL)
    {
        free(copy_txt);
        return NULL;
    }

    char* token = strtok(copy_txt, " "); //띄어쓰기 기준으로 문자열 나누기
    while (token != NULL)
    {
        answer[(*num_token)] = _strdup(token);
        if (answer[*num_token] == NULL)
        {
            for (int i = 0; i < *num_token; i++) 
            {
                free(answer[i]);
            }
            return NULL;
        }
        token = strtok(NULL, " ");
        (*num_token)++;
    }

    char** temp_tokens = (char**)realloc(answer, sizeof(char*) * (*num_token)); //재할당해서 메모리 공간 절약
    if (temp_tokens == NULL) 
    {
    }
    else
    {
        answer = temp_tokens;
    }
    free(copy_txt);
    return answer;
}

char** load_stopwords(const char* filename, int* num_stopwords)//불용어 가져오기
{
    FILE* fp;
    *num_stopwords = 0;
    char buf[256],line[256];
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return NULL;
    }

    while (fgets(line, sizeof(line), fp) != NULL) 
    { // 한 줄씩 읽어. fgets가 NULL이 아니면 성공!
        // 읽어온 줄에서 불필요한 개행 문자 '\n' 제거
        unsigned int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // 공백만 있는 줄이나 아예 빈 줄은 불용어로 세지 않도록 한번 더 체크하는 것이 좋아.
        // strspn, isspace 등 사용 가능. 간단하게는 그냥 strlen() > 0 인지 체크.
        if (strlen(line) > 0) { // 비어있지 않은 줄만 카운트
            (*num_stopwords)++; // 줄 개수(불용어 개수) 증가!
        }
    }
    fseek(fp, 0, SEEK_SET);

    char** answer = (char**)malloc(sizeof(char*) * (*num_stopwords));
    if (answer == NULL)
    {
        fclose(fp);
        return NULL;
    }

    for (int i = 0; i < (*num_stopwords); i++)
    {
        fgets(buf, 256, fp);
        unsigned int len = strlen(buf);
        if (buf[len - 1] == '\n'&&len>0)
        {
            buf[len - 1] = '\0';
        }
        *(answer + i) = _strdup(buf);
        if (*(answer + i) == NULL)
        {
            for (int j = 0; j < i + 1; j++)
            {
                free(answer[j]);
            }
            free(answer);
            fclose(fp);
            return NULL;
        }
    }
    fclose(fp);
    return answer;
}

char** remove_stopwords(char** tokens, int num_tokens, char** stopwords, int num_stopwords, int* num_filtered_tokens)
{
    *num_filtered_tokens = 0;
    char** answer = (char**)malloc(sizeof(char*) * num_tokens);
    if (answer == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < num_tokens; i++)
    {
        int t = 0;
        for (int j = 0; j < num_stopwords; j++)
        {
            if (strcmp(*(tokens + i), *(stopwords + j)) == 0)
            {
                t = 1;
                
                break;
            }
        }
        
        if (t == 0) 
        {
            *(answer + (*num_filtered_tokens)) = _strdup(*(tokens + i));
            if (*(answer + (*num_filtered_tokens)) == NULL)
            {
                for (int j = 0; j < i + 1; j++)
                {
                    free(answer[j]);
                }
                free(answer);
                return NULL;
            }
            (*num_filtered_tokens)++;
        }
    }

    char** temp_tokens = (char**)realloc(answer, sizeof(char*) * (*num_filtered_tokens)); //재할당해서 메모리 공간 절약
    if (temp_tokens == NULL)
    {
    }
    else
    {
        answer = temp_tokens;
    }

    return answer;
}