#pragma pack(2)
#pragma warning(disable:4996)
#include <stdio.h> // 파일입출력함수사용
#include <stdlib.h> // 메모리 동적할당 malloc
#include <unistd.h>
// #include <Windows.h> << 윈도우에서 / 비트맵 파일 구조체 내용 저장
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef long LONG;

typedef struct tagBITMAPFILEHEADER{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfoffBits;
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biplanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD{
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved1;
}RGBQUAD;

int main()
{
    BITMAPFILEHEADER hf;
    BITMAPINFOHEADER hInfo;
    RGBQUAD hRGB[256];
    FILE *fp;
    fp = fopen("LENNA.bmp", "rb");
    if(fp == NULL) return 0;
    fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
    fread(hRGB, sizeof(RGBQUAD), 256, fp);
    int ImgSize = int(hInfo.biWidth * hInfo.biHeight);
    BYTE * Image = (BYTE *)malloc(ImgSize);
    BYTE * Output1 = (BYTE *)malloc(ImgSize); // 원본 복제
    BYTE * Output2 = (BYTE *)malloc(ImgSize); // 색상 밝기 증가(50만큼)
    BYTE * Output3 = (BYTE *)malloc(ImgSize); // 색상 반전
    fread(Image, sizeof(BYTE), ImgSize, fp);
    fclose(fp);
    // 과제 수행
    for(int i=0; i<ImgSize; i++){
        Output1[i]=Image[i]; // 원본
        Output2[i]=Image[i]+50; // 색상 값만 50증가
        Output3[i]=255-Image[i]; // 색상 반전 w<->b
    }
    // 결과물 파일 저장
    fp=fopen("output1.bmp","wb");
    fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
    fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
    fwrite(Output1, sizeof(BYTE), ImgSize, fp);
    fclose(fp);
    fp=fopen("output2.bmp","wb");
    fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
    fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
    fwrite(Output2, sizeof(BYTE), ImgSize, fp);
    fclose(fp);
    fp=fopen("output3.bmp","wb");
    fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
    fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
    fwrite(Output3, sizeof(BYTE), ImgSize, fp);
    fclose(fp);
    free(Image);
    free(Output1);
    free(Output2);
    free(Output3);
}

