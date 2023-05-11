#define _CRT_SECURE_NO_WARNINGS //fopen ���� ���� ���� ������ ���� ����
#include <stdio.h>
#include <stdlib.h> //malloc �Լ�
#include <windows.h>

int main(int argc, char* argv[]) {
	if (argc < 2) { //�ּ� 2�� �̻�
		printf("Usage: %s <filename>\n", argv[0]); //���� �˷���
		return 1; //if�� ����
	}

	char* File = argv[1];
	FILE* fp = fopen(File, "rb"); //���������� fp ����

	if (!fp) { //fp�� NULL�̶��
		printf("Failed to open file: %s\n", File);
		return 1;
	}

	//DOS header �ҷ�����
	fseek(fp, 0, SEEK_SET); //fp�� ������ ���������� �ű�
	IMAGE_DOS_HEADER dosHeader; //dosHeader ����ü���� ����
	fread(&dosHeader, sizeof(IMAGE_DOS_HEADER), 1, fp); //�ҷ��� ������ ������ fp�κ��� sizeof(IMAGE_DOS_HEADER)��ŭ �о�鿩�� dosHeader�� ����

	//Dos header �ĺ� 
	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) { //�ҷ��� ������ signature(dosHeader.e_magic)�� �Ϲ����� dos signature(IMAGE_DOS_SIGNATURE)�� ��ġ���� �ʴ´ٸ�
		printf("Incorrect Dos signature\n");
		return 1;
	}

	//NT header �ҷ�����
	fseek(fp, dosHeader.e_lfanew, SEEK_SET);
	IMAGE_NT_HEADERS ntHeader;
	fread(&ntHeader, sizeof(IMAGE_NT_HEADERS), 1, fp);

	//NT header �ĺ� 
	if (ntHeader.Signature != IMAGE_NT_SIGNATURE) { //�ҷ��� ������ signature�� �Ϲ����� dos signature�� ��ġ���� �ʴ´ٸ�
		printf("Incorrect NT signature\n");
		return 1;
	}

	//numSectionsũ�� �������� �Ҵ� �� Section header �ҷ�����
	int numSections = ntHeader.FileHeader.NumberOfSections;

	IMAGE_SECTION_HEADER* sectionHeaders;
	sectionHeaders = (IMAGE_SECTION_HEADER*)malloc(sizeof(IMAGE_SECTION_HEADER) * numSections); //section ũ���� ����ü �迭 ����

	fread(sectionHeaders, sizeof(IMAGE_SECTION_HEADER), ntHeader.FileHeader.NumberOfSections, fp);

	//dosHeader �и�
	FILE* dosHeader_fp = fopen("dosheader2.bin", "wb"); //�������� �������� dosheader.bin�� ����Ű�� ���� �����͸� ��ȯ�Ͽ� ���� dosHeader_fp�� ����
	fwrite(&dosHeader, sizeof(IMAGE_DOS_HEADER), 1, dosHeader_fp);
	//dosHeader���� IMAGE_DOS_HEADERũ���� 1���� dosHeader_fp�� �ۼ�(����)
	fclose(dosHeader_fp); //open �� close

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