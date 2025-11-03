#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessing.h"
#include "brain.h"
/*

//_____brain.c_____//
시작일시:20251017

최종수정:20251027

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
	fp=fopen(filename, "r");
	if (fp == NULL)
	{
		return NULL;
	}
	
	while (fgets(line, sizeof(line), fp) != NULL)
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

	answer = (Intent*)malloc(sizeof(Intent) * (*num_intents));
	if (answer == NULL)
	{
		fclose(fp);
		return NULL;
	}

	for (int i = 0; i < *num_intents; i++)
	{
		char buf[256];
		fgets(buf, 256, fp);
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
			free_intent_rules(answer, i);
			fclose(fp);
			return NULL;
		}

		answer[i].keywords = (char**)malloc(sizeof(char*) * 256);
		name = strtok(NULL, ",");
		while (name != NULL)
		{
			answer[i].keywords[answer[i].num_keywords] = _strdup(name);
			if (answer[i].keywords[answer[i].num_keywords++] == NULL)
			{
				free_intent_rules(answer, i);
				fclose(fp);
				return NULL;
			}
			name = strtok(NULL, ",");
		}

		char** temp_keywords =realloc(answer[i].keywords, sizeof(char) * answer[i].num_keywords);
		if(temp_keywords == NULL);
		else
		{
			answer[i].keywords = temp_keywords;
		}
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
}