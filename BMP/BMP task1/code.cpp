#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <wingdi.h>
#define format "D:\\users\\documents\\code_C\\Communication Software\\BMP\\BMP task1\\bmp_test%d.bmp"//·��ģ��

typedef struct
{
	unsigned char* data;//��������ͷ��ַ
	int  row_size;//��������ÿһ�е��ֽ���
}pixelData;//���������������

int writeData(FILE* fp, BITMAPINFOHEADER info, pixelData pD);
int addFrameData(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, pixelData pD);
int main()
{
	char path1[100];
	sprintf_s(path1, format, 1);//��ֵ��һ��ԭʼ�ļ���·��
	FILE* fp;
	if (fopen_s(&fp, path1, "rb") != 0)
	{
		printf("fail to open bmp_test1.bmp");
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
	char path2[100];
	sprintf_s(path2, format, 2);
	if (fopen_s(&fp2, path2, "wb") != 0)
	{
		printf("fail to open bmp_test2.bmp");
		exit(0);
	}

	//���µ�BMP�ļ���ֵ
	fwrite(&header, sizeof(header), 1, fp2);
	fwrite(&info, sizeof(info), 1, fp2);
	writeData(fp2, info, pD);//��ֵ��ӱ߿����������

	fclose(fp2);
	free(pD.data);
}

/*********************************************************
*�������ܣ���BMP�ļ����������ݽ��иı䣨��Ӳ�ɫ�߿򣩣���д���µ�BMP�ļ�
*����ԭ�ͣ� int writeData(FILE* fp, BITMAPINFOHEADER info, pixelData pD)
*����˵���� fpΪ�µ�BMP�ļ�ָ�룬infoΪԭBMP�ļ�ͷ��Ϣ��pDΪ�������ݽṹ��
*����ֵ��int ��
*�����ˣ����˷�
*�޸ļ�¼��
*v1.1    2023.4.20
*********************************************************/
int writeData(FILE* fp, BITMAPINFOHEADER info, pixelData pD)
{
	int wid;
	printf("�������ɫ�߿�Ŀ��(��λ������):");
	scanf_s("%d", &wid);

	RGBTRIPLE rgb[6] = { {255,0,0},{0,255,0},{0,0,255},{255,0,0},{0,255,0},{0,0,255} };
	//�޸�ͼƬ�ײ�����ɫ
	addFrameData(0, info.biWidth - 1, 0, wid - 1, info, rgb[0], pD);
	//�޸��ļ��Ҳ����ɫ
	addFrameData(info.biWidth - wid, info.biWidth - 1, 0, info.biHeight - 1, info, rgb[1], pD);
	//�޸��ļ��ϲ����ɫ
	addFrameData(0, info.biWidth - 1, info.biHeight - wid, info.biHeight - 1, info, rgb[2], pD);
	//�޸��ļ�������ɫ
	addFrameData(0, wid - 1, 0, info.biHeight / 3 - 1, info, rgb[3], pD);
	addFrameData(0, wid - 1, info.biHeight / 3, info.biHeight * 2 / 3 - 1, info, rgb[4], pD);
	addFrameData(0, wid - 1, info.biHeight * 2 / 3, info.biHeight - 1, info, rgb[5], pD);
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
*����ԭ�ͣ� int addFrameData(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, pixelData pD)
*����˵���� startX, endX, startY, endY�ֱ�Ϊx,y����ʼ��0<=x(y)<info.biWidth(info.biHeight)
			infoΪԭBMP�ļ�����Ϣͷ��rgbΪҪ��ӵ�rgb���ݣ�pDΪ������Ϣ�ṹ��
*����ֵ��int �ͣ���startX, endX, startY, endYδ������Χ�����򷵻�1�����򷵻�0
*�����ˣ����˷�
*�޸ļ�¼��
*v1.1    2023.4.20
*********************************************************/
int addFrameData(int startX, int endX, int startY, int endY, BITMAPINFOHEADER info, RGBTRIPLE rgb, pixelData pD)
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