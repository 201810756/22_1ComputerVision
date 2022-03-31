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
/*void ObtainHistogram(BYTE *Img, int *Histo, int W, int H){
    int ImgSize=W*H;
    for(int i=0; i<ImgSize;i++){
        Histo[Img[i]]++; // Image[i]=>영상의 화소값
    }
    Histo[0]=0;
}*/
void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)
{
    int ImgSize = W * H;
    //double Temp[256] = { 0 };
    for (int i = 0; i < ImgSize; i++) {
        Histo[Img[i]]++;
    }
    /*for (int i = 0; i < 256; i++) {
        Temp[i] = (double)Histo[i] / ImgSize;
    }*/
    /*FILE* fp = fopen("histogram.txt", "wt");
    for(int i=0; i<256; i++) fprintf(fp, "%d,%d\n", i,Histo[i]);
    fclose(fp);*/
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
// 결과 출력하는 함수
void saveBMPFile(BITMAPFILEHEADER hf,BITMAPINFOHEADER hInfo,
                 RGBQUAD *hRGB,BYTE *Output,int W,int H,const char *FileName){
    FILE *fp=fopen(FileName,"wb");
    fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
    fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
    fwrite(Output, sizeof(BYTE),W*H, fp);
    fclose(fp);
}
// Convolution 함수
void AverageConv(BYTE* Img, BYTE* Out, int W, int H)
{
    double Kernel[3][3] = {0.11111, 0.11111, 0.11111, // 1/9값이 들어가있음
                                        0.11111, 0.11111, 0.11111,
                                        0.11111, 0.11111, 0.11111};
    double SumProduct = 0.0; // sum of product 계산 결과 저장할 변수
    for (int i = 1; i < H-1; i++) { // y좌표(행)
        for (int j = 1; j < W-1; j++) { // x 좌표 _ 위 아래 왼쪽 오른쪽 margin 두기 위해서 (범위)
            for (int m = -1; m <= 1; m++) { // kernel 행
                for (int n = -1; n <= 1; n++) { // kernel 열
                    SumProduct += Img[(i+m)*W + (j+n)] * Kernel[m+1][n+1];
                    // 원본영상[(i+m)*W+(j+n)] 왜 m,n? 영상의 센터를 중심으로 왼쪽 위부터 접근하기 때문
                }
            }
            Out[i * W + j] = (BYTE)SumProduct;
            SumProduct = 0.0; // SumProduct 초기화 해줌
        }
    }
}
void GaussAvrConv(BYTE* Img, BYTE* Out, int W, int H) // 가우시안 [1,2,1,2,4,2,1,2,1]
{
    double Kernel[3][3] = {0.0625, 0.125, 0.0625,
                                        0.125, 0.25, 0.125,
                                        0.0625, 0.125, 0.0625 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // y좌표(행)
        for (int j = 1; j < W - 1; j++) { // x좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 열
                for (int n = -1; n <= 1; n++) { // Kernel 행
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            Out[i * W + j] = (BYTE)SumProduct;
            SumProduct = 0.0;
        }
    }
}
void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) // prewitt_X (열방향) : 경계는 새로 형태로 나옴
{
    double Kernel[3][3] = { -1.0, 0.0, 1.0,
                                        -1.0, 0.0, 1.0,
                                        -1.0, 0.0, 1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // y좌표(행)
        for (int j = 1; j < W - 1; j++) { // x좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            // 0 ~ 765  =====> 0 ~ 255
            Out[i * W + j] = abs((long)SumProduct) / 3; // SumProduct이 double type이라 long type으로 형변환
            SumProduct = 0.0;
        }
    }
}
void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt_Y (행방향)
{
    double Kernel[3][3] = { -1.0, -1.0, -1.0,
                                        0.0, 0.0, 0.0,
                                        1.0, 1.0, 1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // y좌표(행)
        for (int j = 1; j < W - 1; j++) { // x좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            // 0 ~ 765  =====> 0 ~ 255
            Out[i * W + j] = abs((long)SumProduct) / 3;
            SumProduct = 0.0;
        }
    }
}
void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H)
{
    double Kernel[3][3] = { -1.0, 0.0, 1.0,
                                        -2.0, 0.0, 2.0,
                                        -1.0, 0.0, 1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // y좌표(행)
        for (int j = 1; j < W - 1; j++) { // x좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            // 0 ~ 1020  =====> 0 ~ 255 최악의 경우에 1020까지 검출됨
            Out[i * W + j] = abs((long)SumProduct) / 4;
            SumProduct = 0.0;
        }
    }
}
void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H)
{
    double Kernel[3][3] = { -1.0, -2.0, -1.0,
                                        0.0, 0.0, 0.0,
                                        1.0, 2.0, 1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // y좌표(행)
        for (int j = 1; j < W - 1; j++) { // x좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            // 0 ~ 1020  =====> 0 ~ 255
            Out[i * W + j] = abs((long)SumProduct) / 4;
            SumProduct = 0.0;
        }
    }
}
void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) // Prewitt ∏∂Ω∫≈© X
{
    double Kernel[3][3] = { -1.0, -1.0, -1.0,
                                        -1.0, 8.0, -1.0,
                                        -1.0, -1.0, -1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // y좌표(행)
        for (int j = 1; j < W - 1; j++) { //x좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            // 0 ~ 2040  =====> 0 ~ 255
            Out[i * W + j] = abs((long)SumProduct) / 8;
            SumProduct = 0.0;
        }
    }
}
void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) // 기존 영상의 밝기 유지하면서 경계부분
{
    double Kernel[3][3] = { -1.0, -1.0, -1.0,
                                        -1.0, 9.0, -1.0,
                                        -1.0, -1.0, -1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // y좌표(행)
        for (int j = 1; j < W - 1; j++) { // x좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 행
                for (int n = -1; n <= 1; n++) { // Kernel 열
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1];
                }
            }
            //Out[i * W + j] = abs((long)SumProduct) / 8;
            if (SumProduct > 255.0) Out[i * W + j] = 255; // 클리핑 처리
            else if (SumProduct < 0.0) Out[i * W + j] = 0; // 클리핑 처리
            else Out[i * W + j] = (BYTE)SumProduct;
            SumProduct = 0.0;
        }
    }
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
    BYTE * Output = (BYTE *)malloc(ImgSize);
    BYTE * Temp = (BYTE *)malloc(ImgSize);// 원본 복제
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
    //int Thres=GonzalezBinThresh(Image,Histo,3);
    //printf("%d\n",Thres);
    // 이진화 (여기서 임계값을 Gonzalez 방법으로 구해라)
    //Binarization(Image,Output,hInfo.biWidth,hInfo.biHeight,Thres);
    // 스트레칭
    //HistogramStretching(Image,Output,Histo,hInfo.biWidth,hInfo.biHeight);
    // 역상화
    //InverseImage(Image,Output,hInfo.biWidth,hInfo.biHeight);
    // 밝기 조절
    //BrightnessAdj(Image,Output,hInfo.biWidth,hInfo.biHeight,70);
    // 대비 조절 (밝고 어두움의 차이가 좀 더 확연해짐)
    //ContrastAdj(Image,Output,hInfo.biWidth,hInfo.biHeight,0.5);
    /*// 3주차 assignment
    saveBMPFile(hf,hInfo,hRGB,Image,hInfo.biWidth,hInfo.biHeight,"output1.bmp");
    for(int i=0; i<ImgSize; i++) Output[i]=Image[i]+50;
    saveBMPFile(hf,hInfo,hRGB,Output,hInfo.biWidth,hInfo.biHeight,"output2.bmp");
    for(int i=0; i<ImgSize; i++) Output[i]=255-Image[i];
    saveBMPFile(hf,hInfo,hRGB,Output,hInfo.biWidth,hInfo.biHeight,"output3.bmp");*/
    // Convolution
    //AverageConv(Image,Output,hInfo.biWidth,hInfo.biHeight);
    GaussAvrConv(Image,Temp,hInfo.biWidth,hInfo.biHeight);
    //Prewitt_X_Conv(Image,Temp,hInfo.biWidth,hInfo.biHeight);
    //Prewitt_Y_Conv(Image,Temp,hInfo.biWidth,hInfo.biHeight);
    //Sobel_X_Conv(Image,Output,hInfo.biWidth,hInfo.biHeight);
    Laplace_Conv_DC(Temp,Output,hInfo.biWidth,hInfo.biHeight);
    //Binarization(Temp,Output,hInfo.biWidth,hInfo.biHeight,10); // 경계를 분명하게 해주기 위해서 이진화
    // 하얀픽셀들을 합집합 처리하면 완전한 coin 으로 가능(합치기)
    saveBMPFile(hf,hInfo,hRGB,Output,hInfo.biWidth,hInfo.biHeight,"lenna_output_Avr_Conv_after_Laplace_Conv_DC.bmp"); //
    // 결과물 파일 저장
    //saveBMPFile(hf,hInfo,hRGB,Output,hInfo.biWidth,hInfo.biHeight,"lenna_output_with_Gonzalez.bmp");
    free(Image);
    free(Output);
}


// Convolution 개념은 상당히 중요하다. 좌표 계산 << 확인할 필요 있음
// 중간고사 4월 20일
