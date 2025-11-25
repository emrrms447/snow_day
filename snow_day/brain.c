#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessing.h"
#include "brain.h"
#include "response.h"
/*

//_____brain.c_____//
시작일시:20251017

최종수정:20251125

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
	//printf("num_intents = %d\n", *num_intents);

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

		answer[i].keywords = (char**)malloc(sizeof(char*) * 256);
		if (answer[i].keywords == NULL)
		{
			free(copy_txt);
			free_intent_rules(answer, i);
			fclose(fp);
			return NULL;
		}

		char* keyword_token = strtok(NULL, ":");
		while (keyword_token != NULL)
		{
			answer[i].keywords[answer[i].num_keywords] = _strdup(keyword_token);
			if (answer[i].keywords[answer[i].num_keywords] == NULL)
			{
				free(copy_txt);
				free_intent_rules(answer, i);
				fclose(fp);
				return NULL;
			}
			answer[i].num_keywords++;
			keyword_token = strtok(NULL, ":");
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
				//printf("%s ", intents[i].keywords[j]);
				if (strcmp(filtered_tokens[k], intents[i].keywords[j]) == 0)
				{
					count[i]++;
				}
			}
			//printf("\n");
		}
	}
	//for (int i = 0; i < num_intents; i++)
	//{
	//	printf("%d\n", count[i]);
	//}


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

	if (count[num] == 0)//의도를 찾지 못한 경우
	{
		return NULL;
	}

	answer = _strdup(best_intent->name);
	if (answer == NULL)
	{
		return NULL;
	}
	free(count);
	return answer;
}

// Intent 구조체 배열과 그 내부의 모든 할당된 메모리를 해제하는 헬퍼 함수
void free_intent_rules(Intent* intents, int num_intents)
{
	printf("--- 규칙 동적 할당 메모리 해제 시작 ---\n");
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
	printf("--- 규칙 동적 할당 메모리 해제 완료 ---\n");

	intents = NULL;
}

void save_intent_response(char* intent_file_name, char* response_file_name, int num_intent, Intent* intents)
{
	FILE* fp_intent = fopen(intent_file_name, "a"),*fp_response=fopen(response_file_name,"a");
	if (fp_intent == NULL||fp_response==NULL)
	{
		return;
	}
	char name[256],intent[256],response[256];
	int count = 0;
	while (1)
	{
		printf("새로운 규칙의 이름을 영어로 입력해주세요. (원하지 않을 경우 exit를 입력해주세요.) : ");
		scanf("%s", name);
		if (strcmp(name,"exit")==0)
		{
			break;
		}
		for (int i = 0; i < num_intent; i++)
		{
			if (strcmp(name, intents[i].name) == 0)
			{
				printf("존재하는 규칙의 이름입니다. 새로운 이름으로 만들어주세요.\n");
				count = 1;
				break;
			}
		}
		if (count == 1)
		{
			count = 0;
			continue;
		}
		fprintf(fp_intent, "%s", name);

		while (1)
		{
			printf("의도 파악을 위한 키워드를 영어로 입력해주세요. (종료를 원할 경우 exit를 입력해주세요.) : ");
			scanf("%s", intent);
			if (strcmp(intent,"exit")==0)
			{
				break;
			}
			fprintf(fp_intent, "%c", ':');
			fprintf(fp_intent, "%s", intent);
		}
		fprintf(fp_intent, "%c", '\n');

		while (1)
		{
			fprintf(fp_response, "%s", name);
			printf("의도에 대한 답변을 제작해주세요. (종료를 원할 경우 exit를 입력해주세요) : ");
			if (fgets(response, sizeof(response), stdin) == NULL) 
			{
				break;
			}
			else if(strcmp(response, "exit") == 0)
			{
				break;
			}
			else
			{
				response[strcspn(response, "\n")] = '\0';
				fprintf(fp_response, "%c", ':');
			}
			fprintf(fp_response, "\"%s\"", response);
		}
		fprintf(fp_response, "%c", '\n');
	}
	printf("원활한 진행을 위해서 프로그램을 종료 후 다시 실행해주세요.");
	fclose(fp_intent);
	fclose(fp_response);
}