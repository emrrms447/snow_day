#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "preprocessing.h"
#include "brain.h"
#include "response.h"

ResponseRule* load_response_rules(const char* filename, int* num_rules)
{
	FILE* fp;
	*num_rules = 0;
	char line[256];
	ResponseRule* answer;
	fp = fopen(filename, "r");
	if (fp == NULL)//실패 처리 로직
	{
		printf("file error\n");
		return NULL;
	}

	while (fgets(line, sizeof(line), fp) != NULL)//num_rules 카운트하기
	{
		unsigned int len = strlen(line);
		if (len > 0 && line[len - 1] == '\n')
		{
			line[len - 1] = '\0';
		}
		if (strlen(line) > 0)
		{
			(*num_rules)++;
		}
	}
	fseek(fp, 0, SEEK_SET);
	//printf("num_intents = %d\n", *num_rules);

	answer = (ResponseRule*)malloc(sizeof(ResponseRule) * (*num_rules));
	if (answer == NULL)
	{
		printf("malloc error\n");
		fclose(fp);
		return NULL;
	}

	for (int i = 0; i < *num_rules; i++)
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
		answer[i].num_responses = 0;

		char* name = strtok(copy_txt, ":"); // : 기준으로 문자열 나누기
		//printf("name= %s\n", name);
		answer[i].intent_name = _strdup(name);
		if (answer[i].intent_name == NULL)
		{
			free(copy_txt);
			free_intent_rules(answer, i);
			fclose(fp);
			return NULL;
		}

		answer[i].responses = (char**)malloc(sizeof(char*) * 256);
		if (answer[i].responses == NULL)
		{
			free(copy_txt);
			free_intent_rules(answer, i);
			fclose(fp);
			return NULL;
		}

		char* keyword_token = strtok(NULL, ":");
		while (keyword_token != NULL)
		{
			answer[i].responses[answer[i].num_responses] = _strdup(keyword_token);
			//printf("responses = %s\n", answer[i].responses[answer[i].num_responses]);
			if (answer[i].responses[answer[i].num_responses] == NULL)
			{
				free(copy_txt);
				free_intent_rules(answer, i);
				fclose(fp);
				return NULL;
			}
			answer[i].num_responses++;
			keyword_token = strtok(NULL, ":");
		}

		char** temp_keywords = (char**)realloc(answer[i].responses, sizeof(char*) * answer[i].num_responses);
		if (temp_keywords == NULL);
		else
		{
			answer[i].responses = temp_keywords;
		}
		free(copy_txt);
	}

	fclose(fp);
	return answer;
}


char* generate_response(const char* intent_name,ResponseRule* Rules,int num_response_rules)
{
	ResponseRule* p = Rules;
	int num,count=0;

	while (p<Rules+num_response_rules)
	{
		//printf("%s\n", (p)->intent_name);
		if (strcmp((p)->intent_name, intent_name) == 0)
		{
			count = 1;
			break;
		}
		p++;
	}

	if (count == 0)
	{
		return NULL;
	}

	num = rand() % p->num_responses;
	char* answer = _strdup(p->responses[num]);

	return p->responses[num];
}

void free_response_rules(ResponseRule* rules, int num_rules) 
{
	for (int i = 0; i < num_rules; i++) 
	{
		if (rules[i].intent_name != NULL) 
		{
			free(rules[i].intent_name);
			rules[i].intent_name = NULL; 
		}

		if (rules[i].responses != NULL) 
		{
			for (int j = 0; j < rules[i].num_responses; j++) 
			{
				if (rules[i].responses[j] != NULL) 
				{
					free(rules[i].responses[j]);
					rules[i].responses[j] = NULL; 
				}
			}

			free(rules[i].responses);
			rules[i].responses = NULL;
		}
	}

	free(rules);
	rules = NULL; 
}