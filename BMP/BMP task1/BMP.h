#pragma once
#pragma pack(1) // 设置结构体按照字节对齐
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <wingdi.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define SOBEL_SIZE 3//sobel算法所用的参数
#define pathFormat ".\\new picture\\%s%d.bmp"
#define path_maxSize 100
int frame_count = 0;
int gray_count = 0;
int gradient_count = 0;
int pcolor_count = 0;
int size_count = 0;
int rotate_count = 0;

typedef struct
{
	unsigned char* data;//像素数据头地址
	BITMAPFILEHEADER header;//文件头
	BITMAPINFOHEADER info;//信息头
}BMP;//像素数据相关数据
typedef struct {
	unsigned char b;
	unsigned char g;
	unsigned char r;
} PIXEL;// 定义像素点结构体

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
/*********************************************************
*函数功能：对灰度图片进行伪彩色化，并生成新的bnp文件
*函数原型： void toPcolor(BMP bmp)
*函数说明： bmp为BMP结构体
*返回值：int 型
*创建人：吴春阳
*修改记录：
*v1.1    2023.4.20
*********************************************************/
int toPcolor(BMP bmp)
{
	int i, j;
	float gray_float, r, g, b;
	unsigned char* pixel = (unsigned char*)malloc(bmp.info.biWidth * bmp.info.biHeight * 3);
	for (i = 0; i < bmp.info.biHeight; i++) {
		for (j = 0; j < bmp.info.biWidth; j++) {
			pixel[i * bmp.info.biWidth * 3+j * 3] = *(bmp.data + i * bmp.info.biWidth + j);
			// 将unsigned char类型的gray值转换成float类型
			gray_float = (float)pixel[i * bmp.info.biWidth * 3 + j * 3];
			// 将灰度值映射到伪彩色空间中
			if (gray_float <= 63.0) {
				r = 255.0;  // 红色分量
				g = gray_float * 4.0;  // 绿色分量
				b = 0.0;  // 蓝色分量
			}
			else if (gray_float <= 127.0) {
				r = 255 - (gray_float - 64.0) * 4.0;
				g = 255.0;
				b = 0.0;
			}
			else if (gray_float <= 191.0) {
				r = 0.0;
				g = 255.0;
				b = (gray_float - 128.0) * 4.0;
			}
			else {
				r = 0.0;
				g = 255 - (gray_float - 192.0) * 4.0;
				b = 255.0;
			}
			// 将每个像素的RGB值保存到pixel数组中
			pixel[i * bmp.info.biWidth * 3 + j * 3 + 2] = (unsigned char)r;
			pixel[i * bmp.info.biWidth * 3 + j * 3 + 1] = (unsigned char)g;
			pixel[i * bmp.info.biWidth * 3 + j * 3] = (unsigned char)b;
		}
	}

	bmp.header.bfSize = (DWORD)((int)(bmp.info.biWidth * bmp.info.biHeight * 3) + 14 + 44);
	bmp.info.biBitCount = 24;
	bmp.info.biSizeImage = (DWORD)((int)(bmp.info.biWidth * bmp.info.biHeight));
	bmp.info.biClrUsed = 0;
	bmp.info.biClrImportant = 0;
	bmp.header.bfOffBits = 54;
	char path[path_maxSize];
	sprintf_s(path, pathFormat, "pcolor picture", pcolor_count++);
	printf("\n已伪彩色化像素数据\n");
	writeData(bmp.info, bmp.header, pixel, path);
	return 1;
}
/*********************************************************
*函数功能：缩放图片到scale倍
*函数原型： void changeSize(char* inputFile, double scale)
*函数说明： inputFile是路径的字符串数组的开始地址，scale为放大倍数
*返回值：void 型
*创建人：吴春阳
*修改记录：
*v1.1    2023.4.20
*********************************************************/
int changeSize(char* inputFile, double scale) {
	FILE* fpIn, * fpOut;
	BITMAPFILEHEADER fileHeaderIn, fileHeaderOut;
	BITMAPINFOHEADER infoHeaderIn, infoHeaderOut;
	unsigned char* imageDataIn, * imageDataOut;
	int i, j, k, l;
	int widthIn, heightIn, bitCountIn, rowWidthIn, paddingSizeIn;
	int widthOut, heightOut, bitCountOut, rowWidthOut, paddingSizeOut;
	int x, y;
	double factorX, factorY;

	// 打开输入文件
	if (fopen_s(&fpIn, inputFile, "rb") != 0) {
		printf("Error: cannot open input file %s\n", inputFile);
		return 0;
	}

	// 读取BMP文件头
	fread(&fileHeaderIn, sizeof(BITMAPFILEHEADER), 1, fpIn);

	// 读取BMP信息头
	fread(&infoHeaderIn, sizeof(BITMAPINFOHEADER), 1, fpIn);

	// 获取输入图像宽度、高度和位深度
	widthIn = infoHeaderIn.biWidth;
	heightIn = abs(infoHeaderIn.biHeight);
	bitCountIn = infoHeaderIn.biBitCount;

	// 计算每行像素数据的字节数，以4字节对齐
	rowWidthIn = ((widthIn * bitCountIn + 31) / 32) * 4;

	// 计算每行需要填充的字节数
	paddingSizeIn = rowWidthIn - widthIn * bitCountIn / 8;

	// 分配内存保存原始图像数据
	imageDataIn = (unsigned char*)malloc(heightIn * rowWidthIn);
	if (imageDataIn == NULL) {
		printf("Error: out of memory\n");
		fclose(fpIn);
		return;
	}
	memset(imageDataIn, 0, heightIn * rowWidthIn);

	// 读取原始图像数据
	for (i = 0; i < heightIn; i++) {
		fread(imageDataIn + i * rowWidthIn, 1, widthIn * bitCountIn / 8, fpIn);
		fseek(fpIn, paddingSizeIn, SEEK_CUR);
	}

	// 关闭输入文件
	fclose(fpIn);

	// 计算输出图像宽度、高度和位深度
	widthOut = (int)(widthIn * scale);
	heightOut = (int)(heightIn * scale);
	bitCountOut = bitCountIn;

	// 计算每行像素数据的字节数，以4字节对齐
	rowWidthOut = ((widthOut * bitCountOut + 31) / 32) * 4;

	// 计算每行需要填充的字节数
	paddingSizeOut = rowWidthOut - widthOut * bitCountOut / 8;

	// 分配内存保存缩放后的图像数据
	imageDataOut = (unsigned char*)malloc(heightOut * rowWidthOut);
	if (imageDataOut == NULL) {
		printf("Error: out of memory\n");
		free(imageDataIn);
		return 0;
	}
	memset(imageDataOut, 0, heightOut * rowWidthOut);

	// 计算缩放因子
	factorX = (double)widthIn / (double)widthOut;
	factorY = (double)heightIn / (double)heightOut;

	// 进行缩放操作
	for (i = 0; i < heightOut; i++) {
		for (j = 0; j < widthOut; j++) {
			x = (int)(j * factorX);
			y = (int)(i * factorY);
			for (k = 0; k < bitCountIn / 8; k++) {
				for (l = 0; l < bitCountOut / 8; l++) {
					imageDataOut[i * rowWidthOut + j * bitCountOut / 8 + k] =
						imageDataIn[y * rowWidthIn + x * bitCountIn / 8 + k];
				}
			}
		}
	}

	// 打开输出文件
	char path[path_maxSize];
	sprintf_s(path, pathFormat, "sizeChange picture", size_count);
	fopen_s(&fpOut, path, "wb");
	if (fpOut == NULL) {
		printf("Error: cannot open output file \n");
		free(imageDataIn);
		free(imageDataOut);
		return;
	}

	// 填充BMP文件头
	fileHeaderOut.bfType = 0x4D42;
	fileHeaderOut.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + heightOut * rowWidthOut;
	fileHeaderOut.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// 填充BMP信息头
	infoHeaderOut.biSize = sizeof(BITMAPINFOHEADER);
	infoHeaderOut.biWidth = widthOut;
	infoHeaderOut.biHeight = heightOut; // 注意高度为负数表示图像数据从上往下排列
	infoHeaderOut.biPlanes = 1;
	infoHeaderOut.biBitCount = bitCountOut;
	infoHeaderOut.biCompression = 0;
	infoHeaderOut.biSizeImage = heightOut * rowWidthOut;
	infoHeaderOut.biXPelsPerMeter = 0;
	infoHeaderOut.biYPelsPerMeter = 0;
	infoHeaderOut.biClrUsed = 0;
	infoHeaderOut.biClrImportant = 0;
	printf("已生成缩放像素数据\n");
	// 写入BMP文件头和BMP信息头
	fwrite(&fileHeaderOut, sizeof(BITMAPFILEHEADER), 1, fpOut);
	fwrite(&infoHeaderOut, sizeof(BITMAPINFOHEADER), 1, fpOut);

	// 写入缩放后的图像数据
	for (i = 0; i < heightOut; i++) {
		fwrite(imageDataOut + i * rowWidthOut, 1, widthOut * bitCountOut / 8, fpOut);
		for (j = 0; j < paddingSizeOut; j++) {
			fputc(0x00, fpOut);
		}
	}
	printf("已生成%s\n", path);
	// 关闭输出文件
	fclose(fpOut);

	// 释放内存
	free(imageDataIn);
	free(imageDataOut);
	return 1;
}

// 读取bmp文件头信息
void readBmpFileHeader(FILE* fp, BITMAPFILEHEADER* bmpFileHeader) {
	fread(bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
}

// 读取bmp图像信息头信息
void readBmpInfoHeader(FILE* fp, BITMAPINFOHEADER* bmpInfoHeader) {
	fread(bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
}

// 读取像素点信息
void readPixel(FILE* fp, PIXEL* pixel) {
	fread(pixel, sizeof(PIXEL), 1, fp);
}

// 写入bmp文件头信息
void writeBmpFileHeader(FILE* fp, BITMAPFILEHEADER* bmpFileHeader) {
	fwrite(bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
}

// 写入bmp图像信息头信息
void writeBmpInfoHeader(FILE* fp, BITMAPINFOHEADER* bmpInfoHeader) {
	fwrite(bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
}

// 写入像素点信息
void writePixel(FILE* fp, PIXEL* pixel) {
	fwrite(pixel, sizeof(PIXEL), 1, fp);
}

/*********************************************************
*函数功能：计算旋转后的坐标
*函数原型：void getRotatedCoordinate(int x, int y, int centerX, int centerY, double degree, int* newX, int* newY)
*函数说明： inputFile是路径的字符串数组的开始地址，scale为放大倍数
*返回值：void 型
*创建人：辜成伟
*修改记录：
*v1.1    2023.4.25
*********************************************************/
void getRotatedCoordinate(int x, int y, int centerX, int centerY, double degree, int* newX, int* newY) {
	double radian = degree * M_PI / 180.0; // 将角度转换为弧度
	double cosValue = cos(radian);
	double sinValue = sin(radian);
	*newX = (int)(cosValue * (double)(x - centerX) - sinValue * (double)(y - centerY) + centerX);
	*newY = (int)(sinValue * (double)(x - centerX) + cosValue * (double)(y - centerY) + centerY);
}
/*********************************************************
*函数功能：计算旋转后的像素数据，并生成旋转后的图片
*函数原型：int rotateBmp(char* srcBmpFileName, double degree) 
*函数说明： srcBmpFileName是路径的字符串数组的开始地址，degree为旋转的角度
*返回值：int 型
*创建人：辜成伟
*修改记录：
*v1.1    2023.4.25
*********************************************************/
int rotateBmp(char* srcBmpFileName, double degree) 
{
	FILE* srcFp, * destFp;
	BITMAPFILEHEADER bmpFileHeader;
	BITMAPINFOHEADER bmpInfoHeader;
	PIXEL** pixels, ** newPixels;
	int i, j, k, centerX, centerY, newX, newY;
	double radian;

	// 打开源bmp文件
	if (fopen_s(&srcFp, srcBmpFileName, "rb") != 0) {
		printf("于rotateBmp函数，打开源bmp文件失败！\n");
		return 0;
	}

	// 读取bmp文件头信息
	readBmpFileHeader(srcFp, &bmpFileHeader);

	// 读取bmp图像信息头信息
	readBmpInfoHeader(srcFp, &bmpInfoHeader);

	// 计算中心点坐标
	centerX = bmpInfoHeader.biWidth / 2;
	centerY = bmpInfoHeader.biHeight / 2;

	// 分配像素点内存
	pixels = (PIXEL**)malloc(sizeof(PIXEL*) * bmpInfoHeader.biHeight);
	newPixels = (PIXEL**)malloc(sizeof(PIXEL*) * bmpInfoHeader.biHeight);
	for (i = 0; i < bmpInfoHeader.biHeight; i++) {
		pixels[i] = (PIXEL*)malloc(sizeof(PIXEL) * bmpInfoHeader.biWidth);
		newPixels[i] = (PIXEL*)malloc(sizeof(PIXEL) * bmpInfoHeader.biWidth);
	}

	// 读取像素点信息
	for (i = 0; i < bmpInfoHeader.biHeight; i++) {
		for (j = 0; j < bmpInfoHeader.biWidth; j++) {
			readPixel(srcFp, &pixels[i][j]);
		}
	}

	// 关闭源bmp文件
	fclose(srcFp);

	// 计算旋转后的像素点信息
	for (i = 0; i < bmpInfoHeader.biHeight; i++) {
		for (j = 0; j < bmpInfoHeader.biWidth; j++) {
			getRotatedCoordinate(j, i, centerX, centerY, degree, &newX, &newY);
			if (newX >= 0 && newX < bmpInfoHeader.biWidth && newY >= 0 && newY < bmpInfoHeader.biHeight) {
				newPixels[i][j] = pixels[newY][newX];
			}
		}
	}
	printf("已生成旋转像素数据\n");
	// 创建目标bmp文件
	char destBmpFileName[path_maxSize];
	sprintf_s(destBmpFileName, pathFormat, "rotate picture", rotate_count++);
	if (fopen_s(&destFp, destBmpFileName, "wb")!=0) 
	{
		printf("于rotateBmp函数，创建目标bmp文件失败！\n");
		return 0;
	}

	// 写入bmp文件头信息
	writeBmpFileHeader(destFp, &bmpFileHeader);

	// 写入bmp图像信息头信息
	writeBmpInfoHeader(destFp, &bmpInfoHeader);

	// 写入像素点信息
	for (i = 0; i < bmpInfoHeader.biHeight; i++) {
		for (j = 0; j < bmpInfoHeader.biWidth; j++) {
			writePixel(destFp, &newPixels[i][j]);
		}
	}
	printf("已生成%s", destBmpFileName);
	// 关闭目标bmp文件
	fclose(destFp);

	// 释放像素点内存
	for (i = 0; i < bmpInfoHeader.biHeight; i++) {
		free(pixels[i]);
		free(newPixels[i]);
	}
	free(pixels);
	free(newPixels);
	return 1;
}
