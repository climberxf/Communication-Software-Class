#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <wingdi.h>
#include "BMP.h"
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

	BMP bmpData;
	fread(&bmpData.header, sizeof(bmpData.header), 1, fp);//赋值BMP文件头
	fread(&bmpData.info, sizeof(bmpData.info), 1, fp);//赋值信息头

	if (bmpData.header.bfType != 0x4D42)
	{//进行类型判断
		printf("错误！！！不是位图文件");
		exit(0);
	}

	int row_size = (bmpData.info.biWidth * bmpData.info.biBitCount + 31) / 32 * 4;//4字节对齐
	fseek(fp, bmpData.header.bfOffBits, SEEK_SET);//将文件指针移动到像素数据开始处
	bmpData.data = (unsigned char*)malloc((row_size) * (bmpData.info.biHeight) * sizeof(unsigned char));
	if (bmpData.data == NULL)
	{
		printf("imageData申请空间失败！");
		exit(0);
	}

	fread(bmpData.data, row_size * bmpData.info.biHeight * sizeof(unsigned char), 1, fp);//读所有像素数据到imageDate开始的连续地址中
	fclose(fp);
	BMP bmpD;
	bmpD.info.biBitCount = 0;
	int function;
	do
	{
		printf("\n\n请继续操作\n");
		printf("功能:\n");
		printf("0.退出程序\n1.添加彩色边框\n2.转化为灰度图\n3.建立灰度直方图\n4.边缘检测\n");
		printf("请选择功能序号：");
		scanf_s("%d", &function);

		switch (function)
		{
		case 0:
			break;
		case 1:
			{unsigned char* newData1 = changeFrameData(bmpData.info, bmpData.data);
			writeData(bmpData.info, bmpData.header, newData1);//赋值添加边框的像素数据
			free(newData1);}
			break;
		case 2:
			bmpToGray(bmpData, &bmpD);
			writeData(bmpD.info, bmpD.header ,bmpD.data);
			break;
		case 3:
			if (bmpD.info.biBitCount == 8)
				buildHistogram(bmpD.data, bmpD.info.biWidth * bmpD.info.biHeight);
			else
				printf("失败！！！请先生成灰度图！！！");
			break;
		case 4:
			if (bmpD.info.biBitCount == 8)
			{
				edge_detection(bmpD.data, bmpD.info.biWidth, bmpD.info.biHeight);
				writeData(bmpD.info, bmpD.header, bmpD.data);
			}
			else
				printf("失败！！！请先生成灰度图！！！");
			break;
		}
	} while (function);
	if(bmpD.info.biBitCount == 8)
		free(bmpD.data);//判断是否运用*data
	free(bmpData.data);
}
