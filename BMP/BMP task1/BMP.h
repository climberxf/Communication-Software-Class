#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <wingdi.h>

typedef struct
{
	unsigned char* data;//��������ͷ��ַ
	int  row_size;//��������ÿһ�е��ֽ���
}pixelData;//���������������

/*********************************************************
*�������ܣ����µ�BMP�ļ�����������д���µ�BMP�ļ�
*����ԭ�ͣ� int writeData(FILE* fp, BITMAPINFOHEADER info, pixelData pD)
*����˵���� fpΪ�µ�BMP�ļ�ָ�룬infoΪԭBMP�ļ�ͷ��Ϣ��pDΪ�������ݽṹ��
*����ֵ��int ��
*�����ˣ����˷�
*�޸ļ�¼��
*v1.1    2023.4.20
*********************************************************/
int writeData(FILE* fp, BITMAPINFOHEADER info, pixelData pD)
{
	//д���޸��˵���������
	for (int y = 0; y < info.biHeight; y++)
	{
		for (int x = 0; x < info.biWidth; x++)
		{
			unsigned char* pixel = pD.data + y * pD.row_size + x * info.biBitCount / 8;
			//pixelָ��x��y��λ�ô������صĵ�һ��������Ϣ
			fwrite(pixel, sizeof(unsigned char), info.biBitCount / 8, fp);
		}
	}
	return 0;
}
/*********************************************************
*�������ܣ���λͼ��������ָ���ľ��������м����Ӧ��rgb����
*����ԭ�ͣ� int  changeDataOfRectangle(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, pixelData pD)
*����˵���� startX, endX, startY, endY�ֱ�Ϊx,y����ʼ��0<=x(y)<info.biWidth(info.biHeight)
			infoΪԭBMP�ļ�����Ϣͷ��rgbΪҪ��ӵ�rgb���ݣ�pDΪ������Ϣ�ṹ��
*����ֵ��int �ͣ���startX, endX, startY, endYδ������Χ�����򷵻�1�����򷵻�0
*�����ˣ����˷�
*�޸ļ�¼��
*v1.1    2023.4.20
*********************************************************/
int  changeDataOfRectangle(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, pixelData pD)
{
	if (startX < 0 || startY < 0 || endX >= info.biWidth || endY >= info.biHeight)
	{
		printf("������Χ��������0<=x<%d,0<=y<%d", info.biWidth, info.biHeight);
		return 0;
	}
	else
	{
		for (int y = startY; y <= endY; y++)
		{
			for (int x = startX; x <= endX; x++)
			{
				unsigned char* pixel = pD.data + y * pD.row_size + x * info.biBitCount / 8;
				pixel[0] = rgb.rgbtBlue; pixel[1] = rgb.rgbtGreen; pixel[2] = rgb.rgbtRed;
				
			}
		}
		return 1;
	}
}
/*********************************************************
*�������ܣ����ı߿���������
*����ԭ�ͣ� int changeFrameData(BITMAPINFOHEADER info, pixelData pD)
*����˵����infoΪԭBMP�ļ�����Ϣͷ��pDΪ������Ϣ�ṹ��
*����ֵ��int ��
*�����ˣ����˷�
*�޸ļ�¼��
*v1.1    2023.4.20
*********************************************************/
int changeFrameData(BITMAPINFOHEADER info, pixelData pD)
{
	int wid;
	printf("�������ɫ�߿�Ŀ��(��λ������):");
	scanf_s("%d", &wid);

	RGBTRIPLE rgb[6] = { {255,0,0},{0,255,0},{0,0,255},{255,0,0},{0,255,0},{0,0,255} };
	//�޸�ͼƬ�ײ�����ɫ
	changeDataOfRectangle(0, info.biWidth - 1, 0, wid - 1, info, rgb[0], pD);
	//�޸��ļ��Ҳ����ɫ
	changeDataOfRectangle(info.biWidth - wid, info.biWidth - 1, 0, info.biHeight - 1, info, rgb[1], pD);
	//�޸��ļ��ϲ����ɫ
	changeDataOfRectangle(0, info.biWidth - 1, info.biHeight - wid, info.biHeight - 1, info, rgb[2], pD);
	//�޸��ļ�������ɫ
	changeDataOfRectangle(0, wid - 1, 0, info.biHeight / 3 - 1, info, rgb[3], pD);
	changeDataOfRectangle(0, wid - 1, info.biHeight / 3, info.biHeight * 2 / 3 - 1, info, rgb[4], pD);
	changeDataOfRectangle(0, wid - 1, info.biHeight * 2 / 3, info.biHeight - 1, info, rgb[5], pD);
	printf("����Ӳ�ɫ�߿�\n");
	return 0;
}
/*********************************************************
*�������ܣ��ı���������Ϊ�Ҷ�ֵ
*����ԭ�ͣ� int bmpToGray(BITMAPINFOHEADER info, pixelData pD)
*����˵���� infoΪԭBMP�ļ�����Ϣͷ��rgbΪҪ��ӵ�rgb���ݣ�pDΪ������Ϣ�ṹ��
*����ֵ��int ��
*�����ˣ����˷�
*�޸ļ�¼��
*v1.1    2023.4.20
*********************************************************/
int bmpToGray(BITMAPINFOHEADER info, pixelData pD)
{
	for (int y = 0; y < info.biHeight; y++)
	{
		for (int x = 0; x < info.biWidth; x++)
		{
			unsigned char* pixel = pD.data + y * pD.row_size + x * info.biBitCount / 8;
			unsigned char gray = (unsigned char)(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
			pixel[0] = gray;
			pixel[1] = gray;
			pixel[2] = gray;
		}
	}
	printf("�����ɻҶ�ͼƬ\n");
	return 0;
}
/*********************************************************
*�������ܣ������Ҷ�ֱ��ͼ��EXCEL�ļ�
*����ԭ�ͣ� int buildHistogram(unsigned char* data, int pixelCount, int flag, int num)
*����˵���� dataΪ�������ݵ��׵�ַ�� pixelCountΪ���ظ�����flagΪ1���ѽ����Ҷ�ͼ������Ϊ0��numΪÿ�����صļ�ͨ��
*����ֵ��int �ͣ���Ϊ���ɻҶ�ͼ���򷵻�0�����򷵻�1
*�����ˣ����˷�
*�޸ļ�¼��
*v1.1    2023.4.20
*********************************************************/
int buildHistogram(unsigned char* data, int pixelCount, int flag, int num)
{
	if (flag == 0)
	{
		printf("δ���ɻҶ�ͼ����������ѡ����2���ɻҶ�ͼ\n");
		return 0;
	}
	int histogram[256] = { 0 };
	for (int i = 0; i < pixelCount; i++)
	{
		int grayScle = (int)data[i * num];
		histogram[grayScle]++;
	}
	FILE* fp;
	if (fopen_s(&fp, "histogram.xls", "wb") != 0)
	{
		printf("fail to creat histogram.xls\n");
		return 0;
	}
	fprintf(fp, "Gray Level\tCount\n");
	for (int i = 0; i <= 255; i++)
	{
		fprintf(fp, "%d\t%d\n", i, histogram[i]);
	}
	fclose(fp);
	printf("�����ɻҶ�ֱ��ͼ\n");
	return 1;
}