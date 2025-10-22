#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessing.h"
#include "brain.h"
/*

//_____brain.c_____//
시작일시:20251017

최종수정:20251017

목표 : 챗봇의 가장 중요한 뇌 부분을 담당할 c파일

*/

// 의도 규칙을 파일에서 로드하는 함수
// 반환: Intent 구조체 배열의 포인터, num_intents에 로드된 의도 개수 저장
Intent* load_intent_rules(const char* filename, int* num_intents)
{
	FILE* fp;
	fp=fopen(filename, "r");
	if (fp == NULL)
	{
		return NULL;
	}
	




	fclose(fp);
	return NULL;
}

// Intent 구조체 배열과 그 내부의 모든 할당된 메모리를 해제하는 헬퍼 함수
void free_intent_rules(Intent* intents, int num_intents)
{

}