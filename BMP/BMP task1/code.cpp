#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <wingdi.h>
#define format "D:\\users\\documents\\code_C\\Communication Software\\BMP\\BMP task1\\bmp_test%d.bmp"//路径模板

typedef struct
{
	unsigned char* data;//像素数据头地址
	int  row_size;//像素数据每一行的字节数
}pixelData;//像素数据相关数据

int writeData(FILE* fp, BITMAPINFOHEADER info, pixelData pD);
int addFrameData(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, pixelData pD);
int main()
{
	char path1[100];
	sprintf_s(path1, format, 1);//赋值第一个原始文件的路径
	FILE* fp;
	if (fopen_s(&fp, path1, "rb") != 0)
	{
		printf("fail to open bmp_test1.bmp");
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
	char path2[100];
	sprintf_s(path2, format, 2);
	if (fopen_s(&fp2, path2, "wb") != 0)
	{
		printf("fail to open bmp_test2.bmp");
		exit(0);
	}

	//对新的BMP文件赋值
	fwrite(&header, sizeof(header), 1, fp2);
	fwrite(&info, sizeof(info), 1, fp2);
	writeData(fp2, info, pD);//赋值添加边框的像素数据

	fclose(fp2);
	free(pD.data);
}

/*********************************************************
*函数功能：对BMP文件的像素数据进行改变（添加彩色边框），并写入新的BMP文件
*函数原型： int writeData(FILE* fp, BITMAPINFOHEADER info, pixelData pD)
*函数说明： fp为新的BMP文件指针，info为原BMP文件头信息，pD为像素数据结构体
*返回值：int 型
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.20
*********************************************************/
int writeData(FILE* fp, BITMAPINFOHEADER info, pixelData pD)
{
	int wid;
	printf("请输入彩色边框的宽度(单位：像素):");
	scanf_s("%d", &wid);

	RGBTRIPLE rgb[6] = { {255,0,0},{0,255,0},{0,0,255},{255,0,0},{0,255,0},{0,0,255} };
	//修改图片底部的颜色
	addFrameData(0, info.biWidth - 1, 0, wid - 1, info, rgb[0], pD);
	//修改文件右侧的颜色
	addFrameData(info.biWidth - wid, info.biWidth - 1, 0, info.biHeight - 1, info, rgb[1], pD);
	//修改文件上侧的颜色
	addFrameData(0, info.biWidth - 1, info.biHeight - wid, info.biHeight - 1, info, rgb[2], pD);
	//修改文件左侧的颜色
	addFrameData(0, wid - 1, 0, info.biHeight / 3 - 1, info, rgb[3], pD);
	addFrameData(0, wid - 1, info.biHeight / 3, info.biHeight * 2 / 3 - 1, info, rgb[4], pD);
	addFrameData(0, wid - 1, info.biHeight * 2 / 3, info.biHeight - 1, info, rgb[5], pD);
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
*函数原型： int addFrameData(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, pixelData pD)
*函数说明： startX, endX, startY, endY分别为x,y的起始处0<=x(y)<info.biWidth(info.biHeight)
			info为原BMP文件的信息头，rgb为要添加的rgb数据，pD为像素信息结构体
*返回值：int 型，若startX, endX, startY, endY未超出范围函数则返回1，否则返回0
*创建人：奚兴发
*修改记录：
*v1.1    2023.4.20
*********************************************************/
int addFrameData(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, pixelData pD)
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