#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <wingdi.h>

typedef struct
{
	unsigned char* data;//��������ͷ��ַ
	BITMAPFILEHEADER header;//�ļ�ͷ
	BITMAPINFOHEADER info;//��Ϣͷ
}BMP;//���������������

/*********************************************************
*�������ܣ����µ�BMP�ļ����������ݡ��ļ�ͷ����Ϣͷд���µ�BMP�ļ�
*����ԭ�ͣ� int writeData(FILE* fp, BITMAPINFOHEADER info, BITMAPFILEHEADER header, pixelData pD)
*����˵���� fpΪ�µ�BMP�ļ�ָ�룬infoΪԭBMP�ļ�ͷ��Ϣ��headerΪ�ļ�ͷ��pDΪ�������ݽṹ��
*����ֵ��int ��
*�����ˣ����˷�
*�޸ļ�¼��
*v1.1    2023.4.20
*********************************************************/
int writeData(BITMAPINFOHEADER info, BITMAPFILEHEADER header, unsigned char* data)
{
	FILE* fp;
	char path[] = "newBMP.bmp";
	if (fopen_s(&fp, path, "wb") != 0)
	{
		printf("fail to open %s", path);
		return 0;
	}
	int row_size = (info.biWidth * info.biBitCount + 31) / 32 * 4;
	//����ļ�ͷ����Ϣͷ
	fwrite(&header, sizeof(header), 1, fp);
	fwrite(&info, sizeof(info), 1, fp);

	if (info.biBitCount == 8)
	{//�Ҷ�ͼ���
		unsigned char color_table[1024];
		for (int i = 0; i < 256; i++) {
			// ����ɫ����е�ÿ����ɫ������ RGB ֵ��Ϊ��ͬ��ֵ
			color_table[i * 4] = i;
			color_table[i * 4 + 1] = i;
			color_table[i * 4 + 2] = i;
			color_table[i * 4 + 3] = 0;
		}
		fwrite(color_table, sizeof(unsigned char), 1024, fp);//�Ҷ�ͼҪ��ӵ�ɫ��
	}
	else
		fseek(fp, header.bfOffBits, SEEK_SET);
	//д���޸��˵���������
	fwrite(data, row_size * info.biHeight, 1, fp);
	return 1;
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
int  changeDataOfRectangle(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, unsigned char* data)
{
	if (startX < 0 || startY < 0 || endX >= info.biWidth || endY >= info.biHeight)
	{
		printf("������Χ��������0<=x<%d,0<=y<%d", info.biWidth, info.biHeight);
		return 0;
	}
	else
	{
		int row_size = (info.biWidth * info.biBitCount + 31) / 32 * 4;
		for (int y = startY; y <= endY; y++)
		{
			for (int x = startX; x <= endX; x++)
			{
				unsigned char* pixel = data + y * row_size + x * info.biBitCount / 8;
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
unsigned char* changeFrameData(BITMAPINFOHEADER info, unsigned char* data)
{
	int wid;
	printf("�������ɫ�߿�Ŀ��(��λ������):");
	scanf_s("%d", &wid);
	int row_size=(info.biWidth * info.biBitCount + 31) / 32 * 4;
	unsigned char* newData = (unsigned char*)malloc(row_size * info.biHeight * sizeof(unsigned char));
	memcpy(newData, data, row_size * info.biHeight);
	RGBTRIPLE rgb[6] = { {255,0,0},{0,255,0},{0,0,255},{255,0,0},{0,255,0},{0,0,255} };
	//�޸�ͼƬ�ײ�����ɫ
	changeDataOfRectangle(0, info.biWidth - 1, 0, wid - 1, info, rgb[0], newData);
	//�޸��ļ��Ҳ����ɫ
	changeDataOfRectangle(info.biWidth - wid, info.biWidth - 1, 0, info.biHeight - 1, info, rgb[1], newData);
	//�޸��ļ��ϲ����ɫ
	changeDataOfRectangle(0, info.biWidth - 1, info.biHeight - wid, info.biHeight - 1, info, rgb[2], newData);
	//�޸��ļ�������ɫ
	changeDataOfRectangle(0, wid - 1, 0, info.biHeight / 3 - 1, info, rgb[3], newData);
	changeDataOfRectangle(0, wid - 1, info.biHeight / 3, info.biHeight * 2 / 3 - 1, info, rgb[4], newData);
	changeDataOfRectangle(0, wid - 1, info.biHeight * 2 / 3, info.biHeight - 1, info, rgb[5], newData);
	printf("����Ӳ�ɫ�߿�\n");
	return newData;
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
int bmpToGray(BMP bmpD1, BMP* bmpD2)
{
	//����Ԥ����
	bmpD2->header = bmpD1.header;
	bmpD2->info = bmpD1.info;
	bmpD2->data = (unsigned char*)malloc(bmpD1.info.biWidth * bmpD1.info.biHeight * sizeof(unsigned char));
	int row_size = (bmpD1.info.biWidth * bmpD1.info.biBitCount + 31) / 32 * 4;
	int i = 0;
	for (int y = 0; y < bmpD1.info.biHeight; y++)
	{
		for (int x = 0; x < bmpD1.info.biWidth; x++)
		{
			unsigned char* pixel = bmpD1.data + y * row_size + x * bmpD1.info.biBitCount / 8;
			unsigned char gray = (unsigned char)(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
			bmpD2->data[i] = gray;
			i++;
			//printf("%d ", *(bmpD2->data));
		}
	}
	//�ı��ļ�ͷ����Ϣͷ����
	bmpD2->header.bfSize = (DWORD)((int)bmpD1.header.bfSize - (int)(bmpD1.info.biWidth * bmpD1.info.biHeight * bmpD1.info.biBitCount / 8)
		+ (int)(bmpD1.info.biWidth * bmpD1.info.biHeight));
	bmpD2->info.biBitCount = 8;
	bmpD2->info.biSizeImage = (DWORD)((int)(bmpD1.info.biWidth * bmpD1.info.biHeight));
	bmpD2->info.biClrUsed = 256;
	bmpD2->header.bfOffBits = 107;

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
int buildHistogram(unsigned char* data, int pixelCount, int flag)
{
	if (flag == 0)
	{
		printf("δ���ɻҶ�ͼ����������ѡ����2���ɻҶ�ͼ\n");
		return 0;
	}
	int histogram[256] = { 0 };
	for (int i = 0; i < pixelCount; i++)
	{
		int grayScle = (int)data[i];
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

/*void edge_detection(unsigned char* data, int width, int height)
{
	// �����µ��������飬���ڱ����Ե�����
	unsigned char* new_data = (unsigned char*)malloc(width * height);
	if (new_data == NULL) {
		printf("Failed to allocate memory\n");
		exit(1);
	}

	int sobel_x[SOBEL_SIZE][SOBEL_SIZE] = {
	{ -1, 0, 1 },
	{ -2, 0, 2 },
	{ -1, 0, 1 }
	};
	int sobel_y[SOBEL_SIZE][SOBEL_SIZE] = {
		{ 1, 2, 1 },
		{ 0, 0, 0 },
		{ -1, -2, -1 }
	};
	// ������������б�Ե���
	int i, j, x, y;
	int gx, gy, sum;
	for (i = 1; i < height - 1; i++) {
		for (j = 1; j < width - 1; j++) {
			gx = gy = 0;
			for (y = 0; y < SOBEL_SIZE; y++) {
				for (x = 0; x < SOBEL_SIZE; x++) {
					gx += sobel_x[x][y] * data[(i + y - 1) * width + j + x - 1];
					gy += sobel_y[x][y] * data[(i + y - 1) * width + j + x - 1];
				}
			}
			sum = abs(gx) + abs(gy);
			if (sum > 255) sum = 255;   // ��ֹ����ֵ���
			new_data[i * width + j] = 255 - sum;
		}
	}

	// ����Ե������������ݸ��ƻ�ԭʼ����
	memcpy(data, new_data, width * height);

	// �ͷ��µ���������
	free(new_data);
}*/