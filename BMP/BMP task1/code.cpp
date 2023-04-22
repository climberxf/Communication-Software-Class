#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <wingdi.h>
#include "BMP.h"
int gray_flag = 0;
int main()
{
	char path1[100];
	printf("������BMP�ļ���ַ��\n");
	gets_s(path1, 99);
	FILE* fp;
	if (fopen_s(&fp, path1, "rb") != 0)
	{
		printf("fail to open %s",path1);
		exit(0);
	}

	BMP bmpData;
	fread(&bmpData.header, sizeof(bmpData.header), 1, fp);//��ֵBMP�ļ�ͷ
	fread(&bmpData.info, sizeof(bmpData.info), 1, fp);//��ֵ��Ϣͷ

	if (bmpData.header.bfType != 0x4D42)
	{//���������ж�
		printf("���󣡣�������λͼ�ļ�");
		exit(0);
	}

	int row_size = (bmpData.info.biWidth * bmpData.info.biBitCount + 31) / 32 * 4;//4�ֽڶ���
	fseek(fp, bmpData.header.bfOffBits, SEEK_SET);//���ļ�ָ���ƶ����������ݿ�ʼ��
	bmpData.data = (unsigned char*)malloc((row_size) * (bmpData.info.biHeight) * sizeof(unsigned char));
	if (bmpData.data == NULL)
	{
		printf("imageData����ռ�ʧ�ܣ�");
		exit(0);
	}

	fread(bmpData.data, row_size * bmpData.info.biHeight * sizeof(unsigned char), 1, fp);//�������������ݵ�imageDate��ʼ��������ַ��
	fclose(fp);

	int function;
	do
	{
		printf("\n\n���������\n");
		printf("����:\n");
		printf("0.�˳�����\n1.��Ӳ�ɫ�߿�\n2.ת��Ϊ�Ҷ�ͼ&&�����Ҷ�ֱ��ͼ\n3.��Ե���\n");
		printf("��ѡ������ţ�");
		scanf_s("%d", &function);

		switch (function)
		{
		case 0:
			break;
		case 1:
			{unsigned char* newData1 = changeFrameData(bmpData.info, bmpData.data);
			writeData(bmpData.info, bmpData.header, newData1);//��ֵ��ӱ߿����������
			gray_flag = 0;
			free(newData1);}
			break;
		case 2:
			BMP bmpD;
			bmpToGray(bmpData, &bmpD);
			writeData(bmpD.info, bmpD.header ,bmpD.data);
			gray_flag = 1;
			buildHistogram(bmpD.data, bmpD.info.biWidth * bmpD.info.biHeight, gray_flag);
			free(bmpD.data);
			break;
		}
	} while (function);
	free(bmpData.data);
}