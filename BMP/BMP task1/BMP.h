#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <wingdi.h>
#include <math.h>
#define SOBEL_SIZE 3//sobel算法所用的参数
#define pathFormat ".\\new picture\\%s%d.bmp"
#define path_maxSize 100
int frame_count = 0;
int gray_count = 0;
int gradient_count = 0;

typedef struct
{
	unsigned char* data;//像素数据头地址
	BITMAPFILEHEADER header;//文件头
	BITMAPINFOHEADER info;//信息头
}BMP;//像素数据相关数据

/*********************************************************
*函数功能：将新的BMP文件的像素数据、文件头、信息头写入新路径的BMP文件
*函数原型： int writeData(BITMAPINFOHEADER info, BITMAPFILEHEADER header, unsigned char* data, char path[])
*函数说明： info为新的BMP文件头信息，header为文件头，data为新像素数据对应的首地址，path为文件路径
*返回值：int 型
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.20
*v1.2    2023.4.23
*********************************************************/
int writeData(BITMAPINFOHEADER info, BITMAPFILEHEADER header, unsigned char* data, char path[])
{
	FILE* fp;
	//char path[] = "new_bmp.bmp";
	if (fopen_s(&fp, path, "wb") != 0)
	{
		printf("fail to open %s", path);
		return 0;
	}
	int row_size = (info.biWidth * info.biBitCount + 31) / 32 * 4;
	//添加文件头与信息头
	fwrite(&header, sizeof(header), 1, fp);
	fwrite(&info, sizeof(info), 1, fp);

	if (info.biBitCount == 8)
	{//灰度图情况
		unsigned char color_table[1024];
		for (int i = 0; i < 256; i++) {
			// 将调色板表中的每个颜色索引的 RGB 值设为相同的值
			color_table[i * 4] = i;
			color_table[i * 4 + 1] = i;
			color_table[i * 4 + 2] = i;
			color_table[i * 4 + 3] = 0;
		}
		fwrite(color_table, sizeof(unsigned char), 1024, fp);//灰度图要添加调色板
	}
	else
		fseek(fp, header.bfOffBits, SEEK_SET);
	//写入修改了的像素数据
	fwrite(data, row_size * info.biHeight, 1, fp);
	fclose(fp);
	printf("已生成%s",path);
	return 1;
}
/*********************************************************
*函数功能：在位图像素数据指定的矩形区域中加入对应的rgb数据
*函数原型： int  changeDataOfRectangle(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, unsigned char* data)
*函数说明： startX, endX, startY, endY分别为x,y的起始处0<=x(y)<info.biWidth(info.biHeight)
			info为原BMP文件的信息头，rgb为要添加的rgb数据，data为新像素数据对应的首地址
*返回值：int 型，若startX, endX, startY, endY未超出范围函数则返回1，否则返回0
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.20
*********************************************************/
int  changeDataOfRectangle(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, unsigned char* data)
{
	if (startX < 0 || startY < 0 || endX >= info.biWidth || endY >= info.biHeight)
	{
		printf("超出范围，请输入0<=x<%d,0<=y<%d", info.biWidth, info.biHeight);
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
*函数功能：更改边框像素数据,并生成新的bmp文件
*函数原型： int changeFrameData(BITMAPFILEHEADER header, BITMAPINFOHEADER info, unsigned char* data)
*函数说明：info为原BMP文件的信息头，header为原BMP文件的信息头，data为原像素数据对应的首地址
*返回值：int 型
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.20
*********************************************************/
int changeFrameData(BITMAPFILEHEADER header, BITMAPINFOHEADER info, unsigned char* data)
{
	int wid;
	printf("\n请输入彩色边框的宽度(单位：像素):");
	scanf_s("%d", &wid);
	int row_size = (info.biWidth * info.biBitCount + 31) / 32 * 4;
	unsigned char* newData = (unsigned char*)malloc(row_size * info.biHeight * sizeof(unsigned char));
	memcpy(newData, data, row_size * info.biHeight);
	RGBTRIPLE rgb[6] = { {255,0,0},{0,255,0},{0,0,255},{255,0,0},{0,255,0},{0,0,255} };
	//修改图片底部的颜色
	changeDataOfRectangle(0, info.biWidth - 1, 0, wid - 1, info, rgb[0], newData);
	//修改文件右侧的颜色
	changeDataOfRectangle(info.biWidth - wid, info.biWidth - 1, 0, info.biHeight - 1, info, rgb[1], newData);
	//修改文件上侧的颜色
	changeDataOfRectangle(0, info.biWidth - 1, info.biHeight - wid, info.biHeight - 1, info, rgb[2], newData);
	//修改文件左侧的颜色
	changeDataOfRectangle(0, wid - 1, 0, info.biHeight / 3 - 1, info, rgb[3], newData);
	changeDataOfRectangle(0, wid - 1, info.biHeight / 3, info.biHeight * 2 / 3 - 1, info, rgb[4], newData);
	changeDataOfRectangle(0, wid - 1, info.biHeight * 2 / 3, info.biHeight - 1, info, rgb[5], newData);
	printf("已添加彩色边框像素数据\n");
	char path[path_maxSize];
	sprintf_s(path, pathFormat, "colorful frame", frame_count++);
	writeData(info, header, newData, path);
	free(newData);
	return 0;
}
/*********************************************************
*函数功能：调整图片以满足4字节对齐
*函数原型： void adjustImage(BMP* bmpD)
*函数说明：bmp为BMP结构体指针
*返回值：void 型
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.20
*********************************************************/
void adjustImage(BMP* bmpD)
{
	int row_size = (bmpD->info.biWidth * bmpD->info.biBitCount + 31) / 32 * 4;
	int width_new = row_size / (bmpD->info.biBitCount / 8);
	bmpD->info.biWidth = width_new;

	unsigned char* newImgData = (unsigned char*)malloc(sizeof(unsigned char) * row_size * bmpD->info.biHeight);
	for (int i = 0; i < bmpD->info.biHeight; i++) {
		for (int j = 0; j < width_new; j++) {
			for (int c = 0; c < bmpD->info.biBitCount / 8; c++) {
				if (j < bmpD->info.biWidth) {
					newImgData[i * row_size + j * bmpD->info.biBitCount / 8 + c] = bmpD->data[i * bmpD->info.biWidth * bmpD->info.biBitCount / 8 + j * bmpD->info.biBitCount / 8 + c];
				}
				else {
					newImgData[i * row_size + j * bmpD->info.biBitCount / 8 + c] = 0;
				}
			}
		}
	}
	memcpy(bmpD->data, newImgData, sizeof(unsigned char) * row_size * bmpD->info.biHeight);
	free(newImgData);
}
/*********************************************************
*函数功能：改变像素数据为灰度值，并生成新的文件
*函数原型： BMP bmpToGray(BMP bmpD1)
*函数说明： bmp为BMP结构体
*返回值：void 型
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.20
*********************************************************/
BMP bmpToGray(BMP bmpD1)
{
	BMP bmpD2;
	//进行预处理
	bmpD2.header = bmpD1.header;
	bmpD2.info = bmpD1.info;
	//改变文件头与信息头数据
	bmpD2.header.bfSize = (DWORD)((int)bmpD1.header.bfSize - (int)(bmpD1.info.biWidth * bmpD1.info.biHeight * bmpD1.info.biBitCount / 8)
		+ (int)(bmpD1.info.biWidth * bmpD1.info.biHeight));
	bmpD2.info.biBitCount = 8;
	bmpD2.info.biSizeImage = (DWORD)((int)(bmpD1.info.biWidth * bmpD1.info.biHeight));
	bmpD2.info.biClrUsed = 256;
	bmpD2.info.biClrImportant = 256;
	bmpD2.header.bfOffBits = 107;

	int row_size2= (bmpD2.info.biWidth * bmpD2.info.biBitCount + 31) / 32 * 4;
	bmpD2.data = (unsigned char*)malloc(row_size2 * bmpD1.info.biHeight * sizeof(unsigned char));
	int row_size = (bmpD1.info.biWidth * bmpD1.info.biBitCount + 31) / 32 * 4;
	int i = 0;
	for (int y = 0; y < bmpD1.info.biHeight; y++)
	{
		for (int x = 0; x < bmpD1.info.biWidth; x++)
		{
			unsigned char* pixel = bmpD1.data + y * bmpD1.info.biWidth * bmpD1.info.biBitCount / 8 + x * bmpD1.info.biBitCount / 8;
			unsigned char gray;
			if (bmpD1.info.biBitCount == 32)
			{
				if (pixel[3] == 0)
					gray = 0;
				else
					gray = (unsigned char)(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
			}
			else
				gray = (unsigned char)(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
			bmpD2.data[i] = gray;
			i++;
		}
	}
	printf("\n已灰度化像素数据\n");
	adjustImage(&bmpD2);
	char path[path_maxSize];
	sprintf_s(path, pathFormat, "gray picture", gray_count++);
	writeData(bmpD2.info, bmpD2.header, bmpD2.data, path);
	return bmpD2;
}
/*********************************************************
*函数功能：建立灰度直方图的EXCEL文件
*函数原型： int buildHistogram(unsigned char* data, int pixelCount, int flag, int num)
*函数说明： data为像素数据的首地址， pixelCount为像素个数，flag为1则已建立灰度图，否则为0，num为每个像素的几通道
*返回值：int 型，若为生成灰度图，则返回0，否则返回1
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.20
*********************************************************/
int buildHistogram(unsigned char* data, int pixelCount)
{
	int histogram[256] = { 0 };
	for (int i = 0; i < pixelCount; i++)
	{
		int grayScle = (int)data[i];
		histogram[grayScle]++;
	}
	FILE* fp;
	if (fopen_s(&fp, ".\\new picture\\histogram.xls", "wb") != 0)
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
	printf("\n已生成灰度直方图\n");
	return 1;
}
/*********************************************************
*函数功能：对灰度图片进行边缘化处理，可进行x、y、xy梯度处理，并生成新的bnp文件
*函数原型： void edge_detection(BMP bmp,int threshold, char flag)
*函数说明： bmp为BMP结构体，threshold为边缘化阈值，flag为梯度处理标志（x、y、z分别对应下x、y、xy梯度处理）
*返回值：void 型
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.20
*********************************************************/
void edge_detection(BMP bmp,int threshold, char flag)
{
	int width = bmp.info.biWidth;
	int height = bmp.info.biHeight;
	// 创建新的像素数组，用于保存边缘检测结果
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
	// 对像素数组进行边缘检测
	int i, j, x, y;
	int gx, gy, sum=0;
	for (i = 1; i < height - 1; i++) {
		for (j = 1; j < width - 1; j++) {
			gx = gy = 0;
			for (y = 0; y < SOBEL_SIZE; y++) {
				for (x = 0; x < SOBEL_SIZE; x++) {
					gx += sobel_x[x][y] * bmp.data[(i + y - 1) * width + j + x - 1];
					gy += sobel_y[x][y] * bmp.data[(i + y - 1) * width + j + x - 1];
				}
			}
			if (flag == 'z')
				sum = abs(gx) + abs(gy);
			else if (flag == 'y')
				sum = abs(gy);
			else if (flag == 'x')
				sum = abs(gx);
			new_data[i * width + j] = (BYTE)sum > threshold ? 0xff : 0x00;
		}
	}
	char path[path_maxSize];
	sprintf_s(path, pathFormat, "gradient picture", gradient_count++);
	writeData(bmp.info, bmp.header, new_data,path);
	printf("已边缘化像素数据\n");
	free(new_data);
}
