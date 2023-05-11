#define _CRT_SECURE_NO_WARNINGS //fopen 보안 경고로 인한 컴파일 에러 방지
#include <stdio.h>
#include <stdlib.h> //malloc 함수
#include <windows.h>

int main(int argc, char* argv[]) {
	if (argc < 2) { //최소 2개 이상
		printf("Usage: %s <filename>\n", argv[0]); //사용법 알려줌
		return 1; //if문 종료
	}

	char* File = argv[1];
	FILE* fp = fopen(File, "rb"); //파일포인터 fp 선언

	if (!fp) { //fp가 NULL이라면
		printf("Failed to open file: %s\n", File);
		return 1;
	}

	//DOS header 불러오기
	fseek(fp, 0, SEEK_SET); //fp를 파일의 시작점으로 옮김
	IMAGE_DOS_HEADER dosHeader; //dosHeader 구조체변수 선언
	fread(&dosHeader, sizeof(IMAGE_DOS_HEADER), 1, fp); //불러온 파일의 포인터 fp로부터 sizeof(IMAGE_DOS_HEADER)만큼 읽어들여서 dosHeader에 넣음

	//Dos header 식별 
	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { //불러온 파일의 signature(dosHeader.e_magic)와 일반적인 dos signature(IMAGE_DOS_SIGNATURE)가 일치하지 않는다면
		printf("Incorrect Dos signature\n");
		return 1;
	}

	//NT header 불러오기
	fseek(fp, dosHeader.e_lfanew, SEEK_SET);
	IMAGE_NT_HEADERS ntHeader;
	fread(&ntHeader, sizeof(IMAGE_NT_HEADERS), 1, fp);

	//NT header 식별 
	if (ntHeader.Signature != IMAGE_NT_SIGNATURE) { //불러온 파일의 signature와 일반적인 dos signature가 일치하지 않는다면
		printf("Incorrect NT signature\n");
		return 1;
	}

	//numSections크기 동적으로 할당 후 Section header 불러오기
	int numSections = ntHeader.FileHeader.NumberOfSections;

	IMAGE_SECTION_HEADER* sectionHeaders;
	sectionHeaders = (IMAGE_SECTION_HEADER*)malloc(sizeof(IMAGE_SECTION_HEADER) * numSections); //section 크기의 구조체 배열 선언

	fread(sectionHeaders, sizeof(IMAGE_SECTION_HEADER), ntHeader.FileHeader.NumberOfSections, fp);

	//dosHeader 분리
	FILE* dosHeader_fp = fopen("dosheader2.bin", "wb"); //쓰기전용 이진파일 dosheader.bin을 가리키는 파일 포인터를 반환하여 변수 dosHeader_fp에 대입
	fwrite(&dosHeader, sizeof(IMAGE_DOS_HEADER), 1, dosHeader_fp);
	//dosHeader에서 IMAGE_DOS_HEADER크기의 1개를 dosHeader_fp에 작성(복제)
	fclose(dosHeader_fp); //open 후 close

	FILE* ntHeader_fp = fopen("ntheader2.bin", "wb");
	fwrite(&ntHeader, sizeof(IMAGE_NT_HEADERS), 1, ntHeader_fp);
	fclose(ntHeader_fp);

	FILE* sectionHeaders_fp = fopen("sectionheaders2.bin", "wb");
	fwrite(&sectionHeaders, sizeof(IMAGE_SECTION_HEADER), ntHeader.FileHeader.NumberOfSections, sectionHeaders_fp);
	fclose(sectionHeaders_fp);

	printf("Detached headers saved to dosheader2.bin, ntheader2.bin, and sectionheaders2.bin.\n");

	fclose(fp);
	return 0;
}