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

	BITMAPFILEHEADER header;//�ļ�ͷ
	BITMAPINFOHEADER info;//��Ϣͷ
	pixelData pD;

	fread(&header, sizeof(header), 1, fp);//��ֵBMP�ļ�ͷ
	fread(&info, sizeof(info), 1, fp);//��ֵ��Ϣͷ

	if (header.bfType != 0x4D42)
	{//���������ж�
		printf("���󣡣�������λͼ�ļ�");
		exit(0);
	}

	pD.row_size = (info.biWidth * info.biBitCount + 31) / 32 * 4;//4�ֽڶ���
	fseek(fp, header.bfOffBits, SEEK_SET);//���ļ�ָ���ƶ����������ݿ�ʼ��
	pD.data = (unsigned char*)malloc(pD.row_size * info.biHeight * sizeof(unsigned char));
	if (pD.data == NULL)
	{
		printf("imageData����ռ�ʧ�ܣ�");
		exit(0);
	}

	fread(pD.data, pD.row_size * info.biHeight * sizeof(unsigned char), 1, fp);//�������������ݵ�imageDate��ʼ��������ַ��
	fclose(fp);

	FILE* fp2;
	if (fopen_s(&fp2, "newBmpDocument.bmp", "wb") != 0)
	{
		printf("fail to open newBmpDocument.bmp");
		exit(0);
	}
	fwrite(&header, sizeof(header), 1, fp2);
	fwrite(&info, sizeof(info), 1, fp2);
	int function;
	do
	{
		printf("\n\n���������\n");
		printf("����:\n");
		printf("0.�˳�����\n1.��Ӳ�ɫ�߿�\n2.ת��Ϊ��ɫͼ\n3.�����Ҷ�ֱ��ͼ\n");
		printf("��ѡ������ţ�");
		scanf_s("%d", &function);

		switch (function)
		{
		case 0:
			break;
		case 1:
			fseek(fp, header.bfOffBits, SEEK_SET);
			changeFrameData(info, pD);
			writeData(fp2, info, pD);//��ֵ��ӱ߿����������
			gray_flag = 0;
			break;
		case 2:
			fseek(fp, header.bfOffBits, SEEK_SET);
			bmpToGray(info, pD);
			writeData(fp2, info, pD);
			gray_flag = 1;
			break;
		case 3:
			buildHistogram(pD.data, info.biWidth * info.biHeight, gray_flag, info.biBitCount/8);
			break;
		}
	} while (function);
	fclose(fp2);
	free(pD.data);
}