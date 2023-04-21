#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <wingdi.h>
#include "BMP.h"
int gray_flag = 0;
int main()
{
	char path1[100];
	printf("请输入BMP文件地址：\n");
	gets_s(path1, 99);
	FILE* fp;
	if (fopen_s(&fp, path1, "rb") != 0)
	{
		printf("fail to open %s",path1);
		exit(0);
	}

	BITMAPFILEHEADER header;//文件头
	BITMAPINFOHEADER info;//信息头
	pixelData pD;

	fread(&header, sizeof(header), 1, fp);//赋值BMP文件头
	fread(&info, sizeof(info), 1, fp);//赋值信息头

	if (header.bfType != 0x4D42)
	{//进行类型判断
		printf("错误！！！不是位图文件");
		exit(0);
	}

	pD.row_size = (info.biWidth * info.biBitCount + 31) / 32 * 4;//4字节对齐
	fseek(fp, header.bfOffBits, SEEK_SET);//将文件指针移动到像素数据开始处
	pD.data = (unsigned char*)malloc(pD.row_size * info.biHeight * sizeof(unsigned char));
	if (pD.data == NULL)
	{
		printf("imageData申请空间失败！");
		exit(0);
	}

	fread(pD.data, pD.row_size * info.biHeight * sizeof(unsigned char), 1, fp);//读所有像素数据到imageDate开始的连续地址中
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
		printf("\n\n请继续操作\n");
		printf("功能:\n");
		printf("0.退出程序\n1.添加彩色边框\n2.转化为灰色图\n3.建立灰度直方图\n");
		printf("请选择功能序号：");
		scanf_s("%d", &function);

		switch (function)
		{
		case 0:
			break;
		case 1:
			fseek(fp, header.bfOffBits, SEEK_SET);
			changeFrameData(info, pD);
			writeData(fp2, info, pD);//赋值添加边框的像素数据
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