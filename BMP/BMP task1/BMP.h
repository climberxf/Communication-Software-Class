#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <wingdi.h>

typedef struct
{
	unsigned char* data;//像素数据头地址
	int  row_size;//像素数据每一行的字节数
}pixelData;//像素数据相关数据

/*********************************************************
*函数功能：将新的BMP文件的像素数据写入新的BMP文件
*函数原型： int writeData(FILE* fp, BITMAPINFOHEADER info, pixelData pD)
*函数说明： fp为新的BMP文件指针，info为原BMP文件头信息，pD为像素数据结构体
*返回值：int 型
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.20
*********************************************************/
int writeData(FILE* fp, BITMAPINFOHEADER info, pixelData pD)
{
	//写入修改了的像素数据
	for (int y = 0; y < info.biHeight; y++)
	{
		for (int x = 0; x < info.biWidth; x++)
		{
			unsigned char* pixel = pD.data + y * pD.row_size + x * info.biBitCount / 8;
			//pixel指向（x，y）位置处的像素的第一个像素信息
			fwrite(pixel, sizeof(unsigned char), info.biBitCount / 8, fp);
		}
	}
	return 0;
}
/*********************************************************
*函数功能：在位图像素数据指定的矩形区域中加入对应的rgb数据
*函数原型： int  changeDataOfRectangle(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, pixelData pD)
*函数说明： startX, endX, startY, endY分别为x,y的起始处0<=x(y)<info.biWidth(info.biHeight)
			info为原BMP文件的信息头，rgb为要添加的rgb数据，pD为像素信息结构体
*返回值：int 型，若startX, endX, startY, endY未超出范围函数则返回1，否则返回0
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.20
*********************************************************/
int  changeDataOfRectangle(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, pixelData pD)
{
	if (startX < 0 || startY < 0 || endX >= info.biWidth || endY >= info.biHeight)
	{
		printf("超出范围，请输入0<=x<%d,0<=y<%d", info.biWidth, info.biHeight);
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
*函数功能：更改边框像素数据
*函数原型： int changeFrameData(BITMAPINFOHEADER info, pixelData pD)
*函数说明：info为原BMP文件的信息头，pD为像素信息结构体
*返回值：int 型
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.20
*********************************************************/
int changeFrameData(BITMAPINFOHEADER info, pixelData pD)
{
	int wid;
	printf("请输入彩色边框的宽度(单位：像素):");
	scanf_s("%d", &wid);

	RGBTRIPLE rgb[6] = { {255,0,0},{0,255,0},{0,0,255},{255,0,0},{0,255,0},{0,0,255} };
	//修改图片底部的颜色
	changeDataOfRectangle(0, info.biWidth - 1, 0, wid - 1, info, rgb[0], pD);
	//修改文件右侧的颜色
	changeDataOfRectangle(info.biWidth - wid, info.biWidth - 1, 0, info.biHeight - 1, info, rgb[1], pD);
	//修改文件上侧的颜色
	changeDataOfRectangle(0, info.biWidth - 1, info.biHeight - wid, info.biHeight - 1, info, rgb[2], pD);
	//修改文件左侧的颜色
	changeDataOfRectangle(0, wid - 1, 0, info.biHeight / 3 - 1, info, rgb[3], pD);
	changeDataOfRectangle(0, wid - 1, info.biHeight / 3, info.biHeight * 2 / 3 - 1, info, rgb[4], pD);
	changeDataOfRectangle(0, wid - 1, info.biHeight * 2 / 3, info.biHeight - 1, info, rgb[5], pD);
	printf("已添加彩色边框\n");
	return 0;
}
/*********************************************************
*函数功能：改变像素数据为灰度值
*函数原型： int bmpToGray(BITMAPINFOHEADER info, pixelData pD)
*函数说明： info为原BMP文件的信息头，rgb为要添加的rgb数据，pD为像素信息结构体
*返回值：int 型
*创建人：奚兴发
*修改记录：
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
	printf("已生成灰度图片\n");
	return 0;
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
int buildHistogram(unsigned char* data, int pixelCount, int flag, int num)
{
	if (flag == 0)
	{
		printf("未生成灰度图！！！请先选择功能2生成灰度图\n");
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
	printf("已生成灰度直方图\n");
	return 1;
}