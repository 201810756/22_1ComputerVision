#pragma pack(2)
#pragma warning(disable:4996)
#include <stdio.h> // 파일입출력함수사용
#include <stdlib.h> // 메모리 동적할당 malloc
#include <unistd.h>


typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

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

// 역상 함수
void InverseImage(BYTE *Img, BYTE *Out,int W, int H){
    int ImgSize=W*H;
    for(int i=0; i<ImgSize; i++){
        Out[i]=255-Img[i];
    }
}

// 밝기 조정 함수
void BrightnessAdj(BYTE *Img, BYTE *Out, int W, int H, int Val){
    int ImgSize=W*H;
    for(int i=0;i<ImgSize;i++){
        // 클리핑 처리
        if(Img[i]+Val>255){
            Out[i]=255;
        }
        else if(Img[i]+Val<0){
            Out[i]=0;
        }
        else
            Out[i]=Img[i]+Val;
    }
}

// Contrast 조정 함수
void ContrastAdj(BYTE *Img, BYTE *Out, int W, int H, double Val){
    int ImgSize=W*H;
    for(int i=0;i<ImgSize;i++){
        if(Img[i]*Val>255.0){
            Out[i]=255;
        }
        else
            Out[i]=(BYTE)(Img[i]*Val);
        
    }
}

// 히스토그램 함수
void ObtainHistogram(BYTE *Img, int *Histo, int W, int H){
    int ImgSize=W*H;
    for(int i=0; i<ImgSize;i++){
        Histo[Img[i]]++; // Image[i]=>영상의 화소값
    }
    Histo[0]=0;
}
// 히스토그램 스트레칭 (픽셀의 최소,최대 값의 비율 -> 고정된 비율로 영상을 낮은 밝기와 높은 밝기로 당겨줌)
void HistogramStretching(BYTE *Img, BYTE *Out, int *Histo,int W, int H){
    int ImgSize=W*H;
    BYTE Low,High;
    for(int i=0;i<256;i++){
        if(Histo[i]!=0){
            Low=i;
            break;
        }
    }
    for(int i=255;i>=0;i--){
        if(Histo[i]!=0){
            High=i;
            break;
        }
    }
    for(int i=0;i<ImgSize;i++){
        Out[i]=(BYTE)(Img[i]-Low)/(double)(High-Low)*255.0;
    }
    
}
// 누적히스토그램
void ObtainAHistogram(int *Histo,int *AHisto){
    for(int i=0; i<256; i++){
        for(int j=0; j<=i; j++){
            AHisto[i]+=Histo[j];
        }
    }
}

// 평활화(분포를 일정하게)
void HistogramEqualization(BYTE *Img, BYTE *Out, int *AHisto,int W, int H){
    int ImgSize=H*W;
    int Nt=H*W;
    int Gmax=255;
    double Ratio=Gmax/(double)Nt;
    // 정규화합 저장
    BYTE NormSum[256];
    for(int i=0;i<256;i++){
        NormSum[i]=(BYTE)(Ratio*(double)AHisto[i]);
    }
    for(int i=0; i<ImgSize;i++){
        Out[i]=NormSum[Img[i]];
    }
}

// 이진화
void Binarization(BYTE *Img, BYTE *Out, int W, int H, BYTE Threshold){
    int ImgSize=W*H;
    for(int i=0;i<ImgSize;i++){
        if(Img[i]<Threshold)
            Out[i]=0;
        else
            Out[i]=255;
    }
}

// Gonzalez 방법으로 이진화 임계값 설정
int GonzalezBinThresh(BYTE *Image, int *Histo,int Errorrate){
    // 1. 경계값의 T의 초기값 설정(밝기의 최소값, 최대값 사이의 중간점)
    BYTE Low,High;
    int Initial_T=0;
    for(int i=0;i<256;i++){
        if(Histo[i]!=0){
            Low=i;
            break;
        }
    }
    for(int i=255;i>=0;i--){
        if(Histo[i]!=0){
            High=i;
            break;
        }
    }
    Initial_T=(Low+High)/2; // 초기 T값 결정
    // 2. T로 이진화(영상분할) 진행, T보다 큰 화소들로 구성된 G1영역의 밝기값, T보다 작은 화소들로 구성된 G2영역의 밝기값의 평균값 계산필요
    int average_G1,average_G2,tmp;
    int update_T;
    int sum_G1=0,sum_G2=0;
    int cnt_G1=0,cnt_G2=0;
    do{
        for(int i=0; i<256; i++){
            if(i<Initial_T){ // 임계값보다 작은 영역(G2)
                sum_G2+=(i*Histo[i]); // 히스토그램 => 화소값에 해당하는 픽셀 수 i=화소값
                cnt_G2+=Histo[i];
            }
            else{ // 임계값 보다 큰 영역(G1)
                sum_G1+=(i*Histo[i]);
                cnt_G1+=Histo[i];
            }
        }
        //평균값 계산
        average_G2=sum_G2/cnt_G2;
        average_G1=sum_G1/cnt_G1;
        //새로운 경계값 설정
        update_T=(average_G1+average_G2)/2;
        tmp=abs(update_T-Initial_T); // 임계값의 변화
        Initial_T=update_T;
    }while(tmp>=Errorrate); // 임계값의 변화가 미리 정의된 오차(Errorrate)보다 작을 때 임계값 반환 / 크면 과정(2)~반복
    return Initial_T;
}
int main()
{
    BITMAPFILEHEADER hf;
    BITMAPINFOHEADER hInfo;
    RGBQUAD hRGB[256];
    FILE *fp;
    fp = fopen("lenna.bmp", "rb");
    if(fp == NULL) return 0;
    fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
    fread(hRGB, sizeof(RGBQUAD), 256, fp);
    int ImgSize = int(hInfo.biWidth * hInfo.biHeight);
    BYTE * Image = (BYTE *)malloc(ImgSize);
    BYTE * Output = (BYTE *)malloc(ImgSize); // 원본 복제
    fread(Image, sizeof(BYTE), ImgSize, fp);
    fclose(fp);
    // 히스토그램
    int Histo[256]={0,}; // 전부 0으로 초기화
    // 누적 히스토그램
    int AHisto[256]={0,};
    ObtainHistogram(Image,Histo,hInfo.biWidth,hInfo.biHeight);
    ObtainAHistogram(Histo,AHisto);
    // 평활화
    //HistogramEqualization(Image,Output,AHisto,hInfo.biWidth,hInfo.biHeight);
    int Thres=GonzalezBinThresh(Image,Histo,3);
    printf("%d\n",Thres);
    // 이진화 (여기서 임계값을 Gonzalez 방법으로 구해라)
    Binarization(Image,Output,hInfo.biWidth,hInfo.biHeight,Thres);
    // 스트레칭
    //HistogramStretching(Image,Output,Histo,hInfo.biWidth,hInfo.biHeight);
     
    // 영상처리
    // 역상화
    //InverseImage(Image,Output,hInfo.biWidth,hInfo.biHeight);
    // 밝기 조절
    //BrightnessAdj(Image,Output,hInfo.biWidth,hInfo.biHeight,70);
    // 대비 조절 (밝고 어두움의 차이가 좀 더 확연해짐)
    //ContrastAdj(Image,Output,hInfo.biWidth,hInfo.biHeight,0.5);
    
    
    // 결과물 파일 저장
    fp=fopen("lenna_output_with_gonzalez.bmp","wb");
    fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
    fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
    fwrite(Output, sizeof(BYTE), ImgSize, fp);
    fclose(fp);
    free(Image);
    free(Output);
}


