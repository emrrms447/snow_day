#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessing.h"
#include "brain.h"
/*

//_____brain.c_____//
시작일시:20251017

최종수정:20251106

목표 : 챗봇의 가장 중요한 뇌 부분을 담당할 c파일

*/

// 의도 규칙을 파일에서 로드하는 함수
// 반환: Intent 구조체 배열의 포인터, num_intents에 로드된 의도 개수 저장
Intent* load_intent_rules(const char* filename, int* num_intents) //규칙을 파일에서 로드하는 함수
{
	FILE* fp;
	*num_intents = 0;
	char line[256];
	Intent* answer;
	fp = fopen(filename, "r");
	if (fp == NULL)//실패 처리 로직
	{
		printf("file error\n");
		return NULL;
	}

	while (fgets(line, sizeof(line), fp) != NULL)//num_intents 카운트하기
	{
		unsigned int len = strlen(line);
		if (len > 0 && line[len - 1] == '\n')
		{
			line[len - 1] = '\0';
		}
		if (strlen(line) > 0)
		{
			(*num_intents)++;
		}
	}
	fseek(fp, 0, SEEK_SET);
	printf("num_intents = %d\n", *num_intents);

	answer = (Intent*)malloc(sizeof(Intent) * (*num_intents));
	if (answer == NULL)
	{
		printf("malloc error\n");
		fclose(fp);
		return NULL;
	}

	for (int i = 0; i < *num_intents; i++)
	{
		answer[i].name = NULL;
		answer[i].keywords = NULL;
		answer[i].num_keywords = 0;
	}

	for (int i = 0; i < *num_intents; i++)
	{
		char buf[256];
		fgets(buf, 256, fp);
		if (strlen(buf) > 0 && buf[strlen(buf) - 1] == '\n')//개행문자 처리 로직
		{
			buf[strlen(buf) - 1] = '\0';
		}

		char* copy_txt = _strdup(buf);
		if (copy_txt == NULL)
		{
			free_intent_rules(answer, i);
			fclose(fp);
			return NULL;
		}
		answer[i].num_keywords = 0;

		char* name = strtok(copy_txt, ":"); // : 기준으로 문자열 나누기
		answer[i].name = _strdup(name);
		if (answer[i].name == NULL)
		{
			free(copy_txt);
			free_intent_rules(answer, i);
			fclose(fp);
			return NULL;
		}
		printf("name = %s\n", answer[i].name);

		answer[i].keywords = (char**)malloc(sizeof(char*) * 256);
		if (answer[i].keywords == NULL)
		{
			free(copy_txt);
			free_intent_rules(answer, i);
			fclose(fp);
			return NULL;
		}

		char* keyword_token = strtok(NULL, ",");
		while (keyword_token != NULL)
		{
			printf("keyword_token = %s\n", keyword_token);
			answer[i].keywords[answer[i].num_keywords] = _strdup(keyword_token);
			if (answer[i].keywords[answer[i].num_keywords] == NULL)
			{
				free(copy_txt);
				free_intent_rules(answer, i);
				fclose(fp);
				return NULL;
			}
			answer[i].num_keywords++;
			keyword_token = strtok(NULL, ",");
		}

		char** temp_keywords = (char**)realloc(answer[i].keywords, sizeof(char*) * answer[i].num_keywords);
		if (temp_keywords == NULL);
		else
		{
			answer[i].keywords = temp_keywords;
		}
		free(copy_txt);
	}

	fclose(fp);
	return answer;
}

// Intent 구조체 배열과 그 내부의 모든 할당된 메모리를 해제하는 헬퍼 함수
void free_intent_rules(Intent* intents, int num_intents)
{
	Intent* p;
	for (p = intents; p < intents + num_intents; p++)
	{
		if (p->name != NULL)
		{
			free(p->name);
		}
		
		if (p->keywords != NULL)
		{
			for (int i = 0; i < p->num_keywords; i++)
			{
				free((p->keywords)[i]);
			}
			free(p->keywords);
		}
	}
	free(intents);
	intents = NULL;
}

// 필터링된 토큰들을 기반으로 사용자 의도를 파악하는 함수
// 반환: 파악된 의도 이름 문자열의 포인터 (동적 할당됨, 사용 후 free 필요)
//       의도를 파악하지 못하면 NULL 반환.
char* identify_intent(char** filtered_tokens, int num_filtered_tokens, Intent* intents, int num_intents)
{
	Intent* best_intent;
	int num;
	char* answer;
	int* count = (int*)malloc(sizeof(int) * num_intents);
	if (count == NULL)
	{
		printf("error1\n");
		return NULL;
	}

	for (int i = 0; i < num_intents; i++)
	{
		count[i] = 0;
	}

	for (int i = 0; i < num_intents; i++)
	{
		for (int j = 0; j < intents[i].num_keywords; j++)
		{
			for (int k = 0; k < num_filtered_tokens; k++)
			{
				printf("%s ", intents[i].keywords[j]);
				if (strcmp(filtered_tokens[k], intents[i].keywords[j]) == 0)
				{
					count[i]++;
				}
			}
			printf("\n");
		}
	}
	for (int i = 0; i < num_intents; i++)
	{
		printf("%d\n", count[i]);
	}


	num = 0;
	best_intent = intents;
	for (int i = 0; i < num_intents; i++)
	{
		if (count[num] <= count[i])
		{
			best_intent = (intents + i);
			num = i;
		}
	}

	if (count[num] == 0)
	{
		printf("error2\n");
		return NULL;
	}

	answer = _strdup(best_intent->name);
	if (answer == NULL)
	{
		printf("error3\n");
		return NULL;
	}
	free(count);
	return answer;
}