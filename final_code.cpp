#pragma pack(2)
#pragma warning(disable:4996)
#include <stdio.h> // 파일입출력함수사용
#include <stdlib.h> // 메모리 동적할당 malloc
#include <unistd.h>
#include <math.h>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

typedef struct tagBITMAPFILEHEADER{ // 14bytes
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfoffBits;
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{ // 40bytes
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

typedef struct tagRGBQUAD{ // 4bytes * 256 => 1024bytes
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved1; // 사용하지 않음
}RGBQUAD;

// ==================================================================================================================
// 2차원 배열 동적할당 위함
unsigned char** imageMatrix;
// 이진영상에서
unsigned char blankPixel = 255, imagePixel = 0;

typedef struct {
    int row, col;
}pixel;

int getBlackNeighbours(int row, int col) {

    int i, j, sum = 0;

    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if (i != 0 || j != 0)
                sum += (imageMatrix[row + i][col + j] == imagePixel);
        }
    }

    return sum;
}

int getBWTransitions(int row, int col) {
    return     ((imageMatrix[row - 1][col] == blankPixel && imageMatrix[row - 1][col + 1] == imagePixel)
        + (imageMatrix[row - 1][col + 1] == blankPixel && imageMatrix[row][col + 1] == imagePixel)
        + (imageMatrix[row][col + 1] == blankPixel && imageMatrix[row + 1][col + 1] == imagePixel)
        + (imageMatrix[row + 1][col + 1] == blankPixel && imageMatrix[row + 1][col] == imagePixel)
        + (imageMatrix[row + 1][col] == blankPixel && imageMatrix[row + 1][col - 1] == imagePixel)
        + (imageMatrix[row + 1][col - 1] == blankPixel && imageMatrix[row][col - 1] == imagePixel)
        + (imageMatrix[row][col - 1] == blankPixel && imageMatrix[row - 1][col - 1] == imagePixel)
        + (imageMatrix[row - 1][col - 1] == blankPixel && imageMatrix[row - 1][col] == imagePixel));
}

int zhangSuenTest1(int row, int col) {
    int neighbours = getBlackNeighbours(row, col);

    return ((neighbours >= 2 && neighbours <= 6)
        && (getBWTransitions(row, col) == 1)
        && (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel || imageMatrix[row + 1][col] == blankPixel)
        && (imageMatrix[row][col + 1] == blankPixel || imageMatrix[row + 1][col] == blankPixel || imageMatrix[row][col - 1] == blankPixel));
}

int zhangSuenTest2(int row, int col) {
    int neighbours = getBlackNeighbours(row, col);

    return ((neighbours >= 2 && neighbours <= 6)
        && (getBWTransitions(row, col) == 1)
        && (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel || imageMatrix[row][col - 1] == blankPixel)
        && (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row + 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel));
}

void zhangSuen(unsigned char* image, unsigned char* output, int rows, int cols) {

    int startRow = 1, startCol = 1, endRow, endCol, i, j, count, processed;

    pixel* markers;

    imageMatrix = (unsigned char**)malloc(rows * sizeof(unsigned char*));

    for (i = 0; i < rows; i++) {
        imageMatrix[i] = (unsigned char*)malloc((cols + 1) * sizeof(unsigned char));
        for (int k = 0; k < cols; k++) imageMatrix[i][k] = image[i * cols + k];
    }

    endRow = rows - 2;
    endCol = cols - 2;
    do {
        markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
        count = 0;

        for (i = startRow; i <= endRow; i++) {
            for (j = startCol; j <= endCol; j++) {
                if (imageMatrix[i][j] == imagePixel && zhangSuenTest1(i, j) == 1) {
                    markers[count].row = i;
                    markers[count].col = j;
                    count++;
                }
            }
        }

        processed = (count > 0);

        for (i = 0; i < count; i++) {
            imageMatrix[markers[i].row][markers[i].col] = blankPixel;
        }

        free(markers);
        markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
        count = 0;

        for (i = startRow; i <= endRow; i++) {
            for (j = startCol; j <= endCol; j++) {
                if (imageMatrix[i][j] == imagePixel && zhangSuenTest2(i, j) == 1) {
                    markers[count].row = i;
                    markers[count].col = j;
                    count++;
                }
            }
        }

        if (processed == 0)
            processed = (count > 0);

        for (i = 0; i < count; i++) {
            imageMatrix[markers[i].row][markers[i].col] = blankPixel;
        }

        free(markers);
    } while (processed == 1);


    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            output[i * cols + j] = imageMatrix[i][j];
        }
    }
}

// ==================================================================================================================


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

// ==================================================================================================================



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
        Out[i]=(BYTE)(((Img[i]-Low)/(double)(High-Low))*255);
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
    do{
        int sum_G1=0,sum_G2=0;
        int cnt_G1=0,cnt_G2=0;
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


// ==================================================================================================================



// 결과 출력하는 함수
void saveBMPFile(BITMAPFILEHEADER hf,BITMAPINFOHEADER hInfo,
                 RGBQUAD *hRGB,BYTE *Output,int W,int H,const char *FileName){
    FILE * fp = fopen(FileName, "wb");
    if (hInfo.biBitCount == 24) { // 트루컬러
        fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
        fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
        fwrite(Output, sizeof(BYTE), W * H * 3, fp);
    }
    else{ // 그레이 컬러
        fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
        fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
        fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
        fwrite(Output, sizeof(BYTE), W * H, fp);
    }
    fclose(fp);
}



// ==================================================================================================================


// Convolution 함수
// 평활화 박스 (저역통과 필터, 흐림 효과)
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
// 가우시안 평활화 (저역통과 필터, 흐림 효과)
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

// ==================================================================================================================


// 미디언 필터링을 위한 swap 함수
void swap(BYTE * a, BYTE * b){
    BYTE temp=*a;
    *a=*b;
    *b=temp;
}

BYTE Median(BYTE * arr, int size){ // bubble sorting 사용(오름차순 정렬)
    const int S=size;
    for(int i=0;i<(size-1);i++){ // pivot index
        for (int j=(i+1);j<size;j++){ // 비교대상 index
            if(arr[i]>arr[j])
                swap(&arr[i],&arr[j]);
        }
    }
    return arr[S/2]; // 중간 값을 반환
}
// max,min pooling은 peper noise 또는 salt noise 하나만 영상에 존재할 때 효율적으로 사용가능한 필터
BYTE MaxPooling(BYTE * arr, int size){ // bubble sorting 사용(오름차순 정렬)
    const int S=size;
    for(int i=0;i<(size-1);i++){ // pivot index
        for (int j=(i+1);j<size;j++){ // 비교대상 index
            if(arr[i]>arr[j])
                swap(&arr[i],&arr[j]);
        }
    }
    return arr[S-1]; // 가장 큰 값을 반환(pepper noise 삭제, salt noise 더 많아짐)
}

BYTE MinPooling(BYTE * arr, int size){ // bubble sorting 사용(오름차순 정렬)
    const int S=size;
    for(int i=0;i<(size-1);i++){ // pivot index
        for (int j=(i+1);j<size;j++){ // 비교대상 index
            if(arr[i]>arr[j])
                swap(&arr[i],&arr[j]);
        }
    }
    return arr[0]; // 가장 작은 값 반환
}
// 미디안 필터링
void MedianFiltering(BYTE* Img, BYTE* Out, int W, int H, int Length){
    //int Length=3; // 미디언 필터링 마스크의 길이를 설정
    int Margin=Length/2; // center를 위한 margin확보 (3by3의 경우 (3/2,3/2) 즉, (1,1)이 center가 됨)
    int Wsize=Length*Length; // 필터링 마스크의 사이즈 설정 ( Length by Length)
    BYTE * temp=(BYTE *)malloc(sizeof(BYTE)* Wsize); // 이전 코드에서는 사이즈를 9로 결정하고 배열을 선언하는 정적 할당 방식을 사용하였지만 이번에는 동적할당(Wsize만큼)
    //int W=hInfo.biWidth,H=hInfo.biHeight; // Image 가로,세로 사이즈
    int i,j,m,n; // for문을 위한 변수 선언
    // convolution 과 같은 4중 for 문 활용
    for(i=Margin;i<H-Margin;i++){ // '행'에 해당하는 좌표, 왜 Margin~(H-Margin)인가? 영상의 사이즈 밖 접근을 방지하기 위해 center에서 시작
        for(j=Margin;j<W-Margin;j++){ // '열'에 해당하는 좌표, 위와 같은 이유로 Margin~(W-Margin)
            for(m=-Margin;m<=Margin;m++){ // center 기준에서 위 아래 행 방문을 위한 반복문, 0보다 작으면 위, 0이면 center, 0보다 크면 아래 행 방문
                for(n=-Margin;n<=Margin;n++){ // center 기준에서 왼쪽 오른쪽 열 방문을 위한 반복문, 0보다 작으면 왼쪽, 0이면 center, 0보다 크면 오른쪽 열 방문
                    temp[(m+Margin)*Length+(n+Margin)]=Img[(i+m)*W+j+n];
                    // 2차원 배열의 index를 1차원 배열의 index로 설정하는 방식을 통해 원본 영상의 값을 temp 배열에 복사해줌
                }
            }
            //Out[i*W+j]=Median(temp,Wsize); // 한번의 Length by Legnth 마스크에 따라 구해진 temp 배열을 Median 함수의. 파라미터로 전달하여 반환되는 중간값을 결과 영상에 넣어줌
            //Out[i*W+j]=MaxPooling(temp,Wsize); // maxpooling (제일 큰 화소값을 반환... 따라서 peper noise 삭제, salt noise 극대화
            Out[i*W+j]=MinPooling(temp,Wsize); // minpooling(제일 작은 화소값을 반환 ... 따라서 salt noise 삭제, pepper noise 극대화
        }
    }
    free(temp); // 동적할당 해지
}

// ==================================================================================================================


int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top)
{
    if (*top >= arr_size) return(-1);
    (*top)++;
    stackx[*top] = vx;
    stacky[*top] = vy;
    return(1);
}

int pop(short* stackx, short* stacky, short* vx, short* vy, int* top)
{
    if (*top == 0) return(-1);
    *vx = stackx[*top];
    *vy = stacky[*top];
    (*top)--;
    return(1);
}

// Glassfire 알고리즘을 활용한 라벨링 함수
void m_BlobColoring(BYTE* CutImage, int width, int height)
{
    int i, j, m, n, top, area, Out_Area, index;
    int BlobArea[1000]={0,};
    long k;
    short curColor = 0, r, c;
    //    BYTE** CutImage2;
    Out_Area = 1;
    
    // 스택으로 사용할 메모리 할당
    short* stackx = new short[height * width];
    short* stacky = new short[height * width];
    short* coloring = new short[height * width];

    int arr_size = height * width;

    // 라벨링 된 픽셀을 저장하기 위한 메모리 할당

    for (k = 0; k < height * width; k++) coloring[k] = 0;  // 메모리 초기화

    for (i = 0; i < height; i++)
    {
        index = i * width;
        for (j = 0; j < width; j++)
        {
            // 이미 방문한 점이거나 픽셀값이 255가 아니라면 처리 안함
            if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
            r = i; c = j; top = 0; area = 1;
            curColor++;

            while (1)
            {
            GRASSFIRE:
                for (m = r - 1; m <= r + 1; m++)
                {
                    index = m * width;
                    for (n = c - 1; n <= c + 1; n++)
                    {
                        // 관심 픽셀이 영상경계를 벗어나면 처리 안함
                        if (m < 0 || m >= height || n < 0 || n >= width) continue;

                        if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0)
                        {
                            coloring[index + n] = curColor; // 현재 라벨로 마크
                            if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
                            r = m; c = n; area++;
                            goto GRASSFIRE;
                        }
                    }
                }
                if (pop(stackx, stacky, &r, &c, &top) == -1) break;
            }
            if (curColor < 1000) BlobArea[curColor] = area;
        }
    }

    float grayGap = 255.0f / (float)curColor; //curColor : compenent개수
    // component별로 다른 밝기값을 갖도록,,, 각 compenent별 밝기 차 구하기

    // 가장 면적이 넓은 영역을 찾아내기 위함
    for (i = 1; i <= curColor; i++) // 1~component개수
    {
        if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i; // 결국 최종적으로 Out_A rea에는 가장 면적이 큰 compenent정보만 있음
        // BlobArea는 compenent내 픽셀 개수
        // BlobArea[3]=1824,,, 3번째 compenent에 해당하는 픽셀은 1824개,,,
        // 25index까지 값이 들어있으므로 총 25개 compenent라는 것
    }
    // CutImage 배열 클리어
    for (k = 0; k < width * height; k++) CutImage[k] = 255;
    
    // coloring에 저장된 라벨링 결과중(Out_Area에 저장된) 영역이 가장 큰 것만 CutImage에 저장
    for (k = 0; k < width * height; k++)
    {
        if (coloring[k] == Out_Area) CutImage[k] = 0;  // 가장 큰 것만 저장(size filtering)
        // out_area : 가장 사이즈가 큰 compenent 정보
        //printf("%d,,, %d\n",coloring[k],BlobArea[coloring[k]]);
        //if (BlobArea[coloring[k]] > 500) CutImage[k] = 0; //특정 면적 이상이 되는 영역만 출력
        // BlobArea=> 영역별 픽셀 개수
        // ex. 1번 compenent의 경우 픽셀 x개 해당
        // 사이즈가 500픽셀 이상인 애들만 출력하는 것
        //CutImage[k] = (unsigned char)(coloring[k] * grayGap); // 라벨링의 결과
    }

    delete[] coloring;
    delete[] stackx;
    delete[] stacky;
}

void BinaryImageEdgeDetection(BYTE *Bin,BYTE *Out, int W, int H){
    for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            if(Bin[i*W+j]==0){ // 전경화소라면
                if(!(Bin[(i-1)*W+j]==0 && Bin[(i+1)*W+j]==0 &&
                     Bin[i*W+(j-1)]==0 && Bin[i*W+(j+1)]==0)) // 4방향 화소중 하나라도 전경이 아니라면
                    Out[i*W+j]=255;
                    
            }
        }
    }
}

// ==================================================================================================================


void DrawRectOutline(BYTE * Img, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y){ // 동공에 외접하는 사각형 그리기
    for(int i=LU_X;i<RD_X;i++){ // y좌표 : LU_Y
        Img[LU_Y*W+i]=255;
    }
    for(int i=LU_X;i<RD_X;i++){ // y좌표 : RD_Y
        Img[RD_Y*W+i]=255;
    }
    for(int i=LU_Y;i<RD_Y;i++){ // x좌표 : LU_X
        Img[i*W+LU_X]=255;
    }
    for(int i=LU_Y;i<RD_Y;i++){ // x좌표 : RD_X
        Img[i*W+RD_X]=255;
    }
    
}

void DrawCrossLine(BYTE * Img, int W, int H, int Cx, int Cy){
    for(int i=0;i<W-1;i++){
        Img[Cy*W+i]=255; // y좌표 x 영상의 가로 사이즈 + x좌표 // 가로 라인 선
    }
    for(int i=0;i<H-1;i++){
        Img[i*W+Cx]=255; // y좌표 x 영상의 가로 사이즈 + x좌표 // 세로 라인 선
    }
}
void Obtain2DCenter(BYTE *Image, int W, int H,int *Cx,int *Cy){ // 무게 중심은 전경에 해당하는 픽셀의 x좌표와, y 좌표의 평균
    int sumX=0,sumY=0;
    int cnt=0;
    for(int i=0; i<H; i++){
        for(int j=0;j<W;j++){
            if(Image[i*W+j]==0) //동공영역이면
            {
                sumX+=j;
                sumY+=i;
                cnt++;
            }
        }
    }
    if(cnt==0) cnt=1; // 예외처리
    *Cx=sumX/cnt;
    *Cy=sumY/cnt;
}

void Obtain2DBoundingBox(BYTE *Image,int W, int H, int *LUX, int *LUY, int *RDX, int *RDY){
    int flag=0;
    for(int i=0;i<H;i++){
        for(int j=0; j<W;j++){
            if(Image[i*W+j]==0){
                *LUY=i;
                flag=1;
                break;
            }
        }
        if(flag==1) break;
    }
    flag=0;
    for(int i=H-1;i>=0;i--){
        for(int j=0;j<W;j++){
            if(Image[i*W+j]==0){
                *RDY=i;
                flag=1;
                break;
            }
        }
        if(flag==1) break;
    }
    flag=0;
    for(int j=0;j<W;j++){
        for(int i=0;i<H;i++){
            if(Image[i*W+j]==0){
                *LUX=j;
                flag=1;
                break;
            }
        }
        if(flag==1) break;
    }
    flag=0;
    for(int j=W-1;j>=0;j--){
        for(int i=0;i<H;i++){
            if(Image[i*W+j]==0){
                *RDX=j;
                flag=1;
                break;
            }
        }
        if(flag==1) break;
    }
}

// ==================================================================================================================

// 9주차(기하변환) 이동, 스케일링(크기변환), 회전, 뒤틀기
void VerticalFlip(BYTE * Image, int W, int H){
    for(int i=0; i<H/2; i++){
        for(int j=0; j<W; j++){
            swap(&Image[i*W+j],&Image[(H-1-i)*W+j]);
        }
    }
}
void HorizontalFlip(BYTE * Image, int W, int H){
    for(int j = 0; j < W/2; j++){
        for(int i = 0; i < H; i++){
            swap(&Image[i*W+j], &Image[i*W+(W-1-j)]);
        }
    }
}
void Translation(BYTE * Image, BYTE * Output, int W, int H, int Tx, int Ty){
    // Translation
    Ty *= -1; //bmp파일의 영상은 뒤집어져있기 때문에
    for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            if((i+Ty<H && i+Ty>=0)&&(j+Tx<W && j+Tx>=0)){ // 클리핑 처리
                Output[(i+Ty)*W+(j+Tx)]=Image[i*W+j];
            }
        }
    }
}
// 순방향 scailing
/*void Scaling_Error(BYTE * Image, BYTE * Output, int W, int H, double SF_X, double SF_Y){
    int tmpX,tmpY;
    for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            tmpX=j*SF_X;
            tmpY=i*SF_Y;
            if(tmpY<H && tmpX<W)
                Output[tmpY*W+tmpX]=Image[i*W+j];
        }
    }
}*/
// 역방향 scailing(hole 방지)

void Scaling(BYTE * Image, BYTE * Output, int W, int H, double SF_X, double SF_Y){
    int tmpX,tmpY;
    for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            tmpX=(int)(j/SF_X);
            tmpY=(int)(i/SF_Y);
            if(tmpY<H && tmpX<W)
                Output[i*W+j]=Image[tmpY*W+tmpX];
        }
    }
}
void Rotation(BYTE * Image, BYTE * Output, int W, int H, int Angle){
    int tmpX, tmpY;
    double Radian=Angle*3.141592/180.0;
    for(int i=0; i <H; i++){
        for(int j=0; j<W; j++){ // 반시계방향의 역행렬로 계산
            tmpX=(int)(cos(Radian)*j+sin(Radian)*i);
            tmpY=(int)(-sin(Radian)*j+cos(Radian)*i);
            if((tmpY<H && tmpY>=0) && (tmpX<W && tmpX>=0))
                Output[i*W+j]=Image[tmpY*W+tmpX];
        }
    }
}
/*void RotationVer2(BYTE * Image, BYTE * Output, int W, int H, int Angle){
    int tmpX, tmpY;
    double Radian=Angle*3.141592/180.0;
    // 원점 중심이 아닌 30,30 기준으로 회전한다고 가정
    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            tmpX = (int)(cos(Radian)*(j-30)+sin(Radian)*(i-30)) + 30;
            tmpY = (int)(-sin(Radian)*(j-30)+cos(Radian)*(i-30)) + 30;
            if((tmpY<H && tmpY>=0) && (tmpX<W && tmpX>=0))
                Output[i*W+j] = Image[tmpY*W+tmpX];
        }
    }
    
}*/
// 원점이 아닌 영상의 center를 기준으로 회전하기
void RotationVer3(BYTE * Image, BYTE * Output, int W, int H, int Angle){
    int tmpX, tmpY;
    double Radian = Angle*3.141592/180.0;
    int cX = (int)(W/2);
    int cY = (int)(H/2);
    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            tmpX = (int)(cos(Radian)*(j-cX) + sin(Radian)*(i-cY)) + cX;
            tmpY = (int)(-sin(Radian)*(j-cX) + cos(Radian)*(i-cY)) + cY;
            if((tmpX < W && tmpX >= 0) && (tmpY < H && tmpY >=0))
                Output[i*W+j] = Image[tmpY*W+tmpX];
        }
    }
}
// ==================================================================================================================
void FillColor(BYTE* Image, int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B)
{
    Image[Y * W * 3 + X * 3] = B;
    Image[Y * W * 3 + X * 3 + 1] = G;
    Image[Y * W * 3 + X * 3 + 2] = R;
}
// 11주차
void RGB2YCbCr(BYTE* Image, BYTE* Y, BYTE* Cb, BYTE* Cr, int W, int H)
{
    for (int i = 0; i < H; i++) { // Y좌표
        for (int j = 0; j < W; j++) { // X좌표
            Y[i * W + j] = (BYTE)(0.299 * Image[i * W * 3 + j * 3 + 2] + 0.587 * Image[i * W * 3 + j * 3 + 1] + 0.114 * Image[i * W * 3 + j * 3]);
            Cb[i * W + j] = (BYTE)(-0.16874 * Image[i * W * 3 + j * 3 + 2] -0.3313 * Image[i * W * 3 + j * 3 + 1] + 0.5 * Image[i * W * 3 + j * 3] + 128.0);
            Cr[i * W + j] = (BYTE)(0.5 * Image[i * W * 3 + j * 3 + 2] - 0.4187 * Image[i * W * 3 + j * 3 + 1] - 0.0813 * Image[i * W * 3 + j * 3] + 128.0);
        }
    }
}

//  11주차 과제 함수  ==================================================================================================================

void Obtain2DBoundingBoxForColor(BYTE *Image,int W, int H, int *LUX, int *LUY, int *RDX, int *RDY){
    int flag=0;
    for(int i=0;i<H;i++){
        for(int j=0; j<W;j++){
            if(Image[i*W*3+j*3]!=0 || Image[i*W*3+j*3]!=0 || Image[i*W*3+j*3]!=0){
                *LUY=i;
                flag=1;
                break;
            }
        }
        if(flag==1) break;
    }
    flag=0;
    for(int i=H-1;i>=0;i--){
        for(int j=0;j<W;j++){
            if(Image[i*W*3+j*3]!=0 || Image[i*W*3+j*3]!=0 || Image[i*W*3+j*3]!=0){
                *RDY=i;
                flag=1;
                break;
            }
        }
        if(flag==1) break;
    }
    flag=0;
    for(int j=0;j<W;j++){
        for(int i=0;i<H;i++){
            if(Image[i*W*3+j*3]!=0 || Image[i*W*3+j*3]!=0 || Image[i*W*3+j*3]!=0){
                *LUX=j;
                flag=1;
                break;
            }
        }
        if(flag==1) break;
    }
    flag=0;
    for(int j=W-1;j>=0;j--){
        for(int i=0;i<H;i++){
            if(Image[i*W*3+j*3]!=0 || Image[i*W*3+j*3]!=0 || Image[i*W*3+j*3]!=0){
                *RDX=j;
                flag=1;
                break;
            }
        }
        if(flag==1) break;
    }
}
void DrawRectOutlineForColor(BYTE * Img, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y){ // 동공에 외접하는 사각형 그리기
    for(int i=LU_X;i<RD_X;i++){ // y좌표 : LU_Y
        Img[LU_Y*W*3+i*3]=0;
        Img[LU_Y*W*3+i*3+1]=0;
        Img[LU_Y*W*3+i*3+2]=255;
    }
    for(int i=LU_X;i<RD_X;i++){ // y좌표 : RD_Y
        Img[RD_Y*W*3+i*3]=0;
        Img[RD_Y*W*3+i*3+1]=0;
        Img[RD_Y*W*3+i*3+2]=255;
    }
    for(int i=LU_Y;i<RD_Y;i++){ // x좌표 : LU_X
        Img[i*W*3+LU_X*3]=0;
        Img[i*W*3+LU_X*3+1]=0;
        Img[i*W*3+LU_X*3+2]=255;
    }
    for(int i=LU_Y;i<RD_Y;i++){ // x좌표 : RD_X
        Img[i*W*3+RD_X*3]=0;
        Img[i*W*3+RD_X*3+1]=0;
        Img[i*W*3+RD_X*3+2]=255;
    }
    
}
// ==================================================================================================================
// 12주차 모폴로지
// 침식
void Erosion(BYTE* Image, BYTE* Output, int W, int H)
{
    for (int i = 1; i < H - 1; i++) { // margin 부여
        for (int j = 1; j < W - 1; j++) {
            if (Image[i * W + j] == 255) // 전경화소라면
            { // 주변 4개 화소 check
                if (!(Image[(i - 1) * W + j] == 255 &&
                    Image[(i + 1) * W + j] == 255 &&
                    Image[i * W + j - 1] == 255 &&
                    Image[i * W + j + 1] == 255)) // 4주변화소가 모두 전경화소가 아니라면
                    Output[i * W + j] = 0;
                else Output[i * W + j] = 255;
            }
            else Output[i * W + j] = 0;
        }
    }
}

// 팽창
void Dilation(BYTE* Image, BYTE* Output, int W, int H)
{
    for (int i = 1; i < H - 1; i++) {
        for (int j = 1; j < W - 1; j++) {
            if (Image[i * W + j] == 0) // 배경화소라면
            {
                if (!(Image[(i - 1) * W + j] == 0 &&
                    Image[(i + 1) * W + j] == 0 &&
                    Image[i * W + j - 1] == 0 &&
                    Image[i * W + j + 1] == 0)) // 4주변화소가 모두 배경화소가 아니라면
                    Output[i * W + j] = 255;
                else Output[i * W + j] = 0;
            }
            else Output[i * W + j] = 255;
        }
    }
}
// ==================================================================================================================

// 12 주차 과제
void FeatureExtractThinImage(BYTE * Image, BYTE * Output, int W, int H){
    // 전경 -> 배경 화소로의 변화를 파악하기 위한 dx, dy 선언(모든 좌표를 표현하는 것보다는 반복문으로 처리하기 위함)
    // 시계 방향으로 한 바퀴
    int dx[9] = {-1,0,1,1,1,0,-1,-1,-1};
    int dy[9] = {1,1,1,0,-1,-1,-1,0,1};
    for (int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            Output[i*W+j] = Image[i*W+j];
        }
    }
    
    for(int i = 1; i < H-1; i++){
        for(int j = 1; j < W-1; j++){
            int count = 0;
            if(Image[i*W+j] == 0){ // 전경 화소라면
                for(int k=0; k<8; k++){
                    // 앞 화소 - 뒤 화소 = 255 ===> 전경 -> 배경 변화
                    if(Image[(i+dy[k+1])*W+(j+dx[k+1])]-Image[(i+dy[k])*W+(j+dx[k])] == 255)
                        count++; // 카운트 증가
                }
                /*if(count == 1 || count >= 3) // 카운트가 1이면 끝점, 카운트가 3 이상이면 분기점
                    Output[i*W+j] = 128;*/ // 회색 점으로 표현
                // 녹화 강의에서는 8방향에 대한 회색 박스를 표시하라고 나옴
                if(count == 1 || count >= 3){
                    for(int k = 0; k<8; k++){
                        Output[(i+dy[k])*W+(j+dx[k])] = 128;
                    }
                }
            }
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
    //fread(hRGB, sizeof(RGBQUAD), 256, fp); << 그레이 스케일 이미지 (팔레트)
    int ImgSize = int(hInfo.biWidth * hInfo.biHeight);
    int H = hInfo.biHeight, W = hInfo.biWidth;
    BYTE * Image = (BYTE *)malloc(ImgSize);
    BYTE * Output = (BYTE *)malloc(ImgSize);
    BYTE * Temp = (BYTE *)malloc(ImgSize);// 원본 복제
    if(hInfo.biBitCount==24){ // 트루컬러인 경우
        Image=(BYTE *)malloc(ImgSize*3);
        Temp=(BYTE *)malloc(ImgSize*3);
        Output=(BYTE *)malloc(ImgSize*3);
        fread(Image,sizeof(BYTE),ImgSize*3,fp);
    }
    else{ // 그레이 스케일 이미지인 경우
        fread(hRGB, sizeof(RGBQUAD), 256, fp);
        Image=(BYTE *)malloc(ImgSize);
        Temp=(BYTE *)malloc(ImgSize);
        Output=(BYTE *)malloc(ImgSize);
        fread(Image,sizeof(BYTE),ImgSize,fp);
    }
    //fread(Image, sizeof(BYTE), ImgSize, fp);
    fclose(fp);
    //히스토그램
    //int Histo[256]={0,}; // 전부 0으로 초기화
    //누적 히스토그램
    //int AHisto[256]={0,};
    // (i,40) 위치를 특정 색상으로
    /*for(int i=0; i<W; i++){
        // r=g=b 성분의 값이 모두 같으면 grey scale
        Image[40*W*3+i*3]=128; // 40,50의 blue 성분
        Image[40*W*3+i*3+1]=128;// 40,50의 green 성분
        Image[40*W*3+i*3+2]=128; // 40,50의 red 성분
    }*/
        /*for(int i=0; i<W; i++){
        FillColor(Image,i,200,W,H,0,255,255); // r,g,b 순
    }*/
    
    // 색깔 박스 채우기
    /*for(int i=0; i<=200; i++){
        for(int j=200; j<=300; j++){
            FillColor(Image,j,i,W,H,255,255,0);
        }
    }*/
    
    // 띠 그리기
    /*
    // 초기화
    for (int i = 0; i <  H; i++) {
        for (int j = 0; j < W; j++) {
            Image[i * W * 3 + j * 3] = 0;
            Image[i * W * 3 + j * 3 + 1] = 0;
            Image[i * W * 3 + j * 3 + 2] = 0;
        }
    }
    // y좌표 기준 0~239 (Red)
    for (int i = 0; i < 240; i++) {
        for (int j = 0; j < W; j++) {
            Image[i*W*3 + j*3 + 2] = 128;
        }
    }
    // y좌표 기준 120 ~ 359 (Green)
    for (int i = 120; i < 360; i++) {
        for (int j = 0; j < W; j++) {
            Image[i * W * 3 + j * 3 + 1] = 128;
        }
    }
    // y좌표 기준 300 ~ 479 (Blue)
    for (int i = 300; i < 480; i++) {
        for (int j = 0; j < W; j++) {
            Image[i * W * 3 + j * 3] = 128;
        }
    }*/
    
    // 그라데이션 만들기 (B ~ R)
    /*for(int a = 0; a<120; a++){
        for (int i = 0; i < W; i++) {
            // 가중치를 활용함
            // Blue - > Red 그라데이션이라면
            // wt는 i=0이면 0이고, W-1이라면 1이 되는 형식
            wt = i / (double)(W - 1);
            Image[a * W * 3 + i * 3] = (BYTE)(255 * (1.0-wt));  // Blue
            Image[a * W * 3 + i * 3 + 1] = 0; // Green
            Image[a * W * 3 + i * 3 + 2] = (BYTE)(255 * wt); // Red
        }
    }*/
    
    // =======================ㅇ===========================================================================================
    // 10주차 과제(그라데이션 만들기)
    // red->cyan , green->magenta , blue->yellow
    // cyan : rgb(0,255,255)
    // magenta : rgb(255,0,255)
    // yellow : rgb(255,255,0)
    // 가중치 0~1 / 따라서 시작하는 색상의 픽셀 값 * (1-가중치) => 점진적으로 색상 흐릿, 어두워짐
    // 두번째로 시작하는 색상의 픽셀 값 * 가중치 => 점진적으로 색상 명확, 밝아짐
    // 이미지가 inverse 되어 있는 것을 감안해야 함
    /*double wt;
    int hLine=H/3; // 경계 지점
    for(int i=0; i<hLine; i++){
        for(int j=0; j<W; j++){
            wt=j/(double)(W-1);
            Image[i*W*3+j*3]=(BYTE)(255*(1.0-wt)); // blue
            Image[i*W*3+j*3+1]=(BYTE)(255*wt); // green
            Image[i*W*3+j*3+2]=(BYTE)(255*wt); // red
        }
    }
    for(int i=hLine; i<hLine*2; i++){
        for(int j=0; j<W; j++){
            wt=j/(double)(W-1);
            Image[i*W*3+j*3]=(BYTE)(255*wt);
            Image[i*W*3+j*3+1]=(BYTE)(255*(1.0-wt));
            Image[i*W*3+j*3+2]=(BYTE)(255*wt);
        }
    }
    for(int i=hLine*2; i<H; i++){
        for(int j=0; j<W; j++){
            wt=j/(double)(W-1);
            Image[i*W*3+j*3]=(BYTE)(255*wt);
            Image[i*W*3+j*3+1]=(BYTE)(255*wt);
            Image[i*W*3+j*3+2]=(BYTE)(255*(1.0-wt));
        }
    }*/
    // 추가
    // 세로로 그라데이션 입력해보기
    /*int wLine=W/3;
    for(int j=0; j<wLine; j++){
        for(int i=0; i<H; i++){
            wt=i/(double)(H-1);
            Image[i*W*3+j*3]=(BYTE)(255*(1.0-wt));
            Image[i*W*3+j*3+1]=(BYTE)(255*wt);
            Image[i*W*3+j*3+2]=(BYTE)(255*wt);
        }
    }
    for(int j=wLine; j<wLine*2; j++){
        for(int i=0; i<H; i++){
            wt=i/(double)(W-1);
            Image[i*W*3+j*3]=(BYTE)(255*wt);
            Image[i*W*3+j*3+1]=(BYTE)(255*(1.0-wt));
            Image[i*W*3+j*3+2]=(BYTE)(255*wt);
        }
    }
    for(int j=wLine*2; j<W; j++){
        for(int i=0; i<H; i++){
            wt=i/(double)(H-1);
            Image[i*W*3+j*3]=(BYTE)(255*wt);
            Image[i*W*3+j*3+1]=(BYTE)(255*wt);
            Image[i*W*3+j*3+2]=(BYTE)(255*(1.0-wt));
        }
    }*/
    
    // 11주차 내용
    // Red 값이 큰 부분만 마스킹 RGB모델 기반
    // 초기 130, 50, 100
    /*for (int i = 0; i < H; i++) { // Y좌표
        for (int j = 0; j < W; j++) { // X좌표
            // 그냥 단순히 RED 영역만 따지면 안됨
            // why ? RGB 모델은 색상과 밝기가 종속적임
            //
            if (Image[i * W * 3 + j * 3 + 2] > 130 && // red 성분
                Image[i * W * 3 + j * 3 + 1] < 50 && // green 성분
                Image[i * W * 3 + j * 3 + 0] < 100) { // blue 성분
                Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];
                Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];
                Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];
            }
            else
                Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 +1] = Output[i * W * 3 + j * 3 +2] = 0;
        }
    }*/
    
    // rgb->ycbcr 변환할때 weighted 값을 다르게 할당하는 이유 : 사람의 시각체계에 맞게
    // 사람의 시각 체계에서는 GREEN이 제일 민감 따라서 weighted 제일 크게
    // rgb to ycbcr
    //BYTE * Y = (BYTE *)malloc(ImgSize);
    //BYTE * Cb = (BYTE *)malloc(ImgSize);
    //BYTE * Cr = (BYTE *)malloc(ImgSize);
    //RGB2YCbCr(Image,Y,Cb,Cr,W,H);
    
    // 빨간색 딸기영역만 masking (Y, Cb, Cr 모델 기준)
    /*for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (Cb[i * W + j] <99 && Cr[i * W + j] > 200) {
                Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];
                Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];
                Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];
            }
            else
                Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 255;
        }
    }*/
    // ============================================================================================================================================
    
    // 11주차 과제 얼굴 영역 추출 및 외접 사각형 그리기
    // Cb, Cr 성분을 통해 피부 영역에 해당하는 부분만 추출
    // 추출한 영상을 통해 외접 사각형의 좌표 추출 LUX,LUY,RDX,RDY
    // 추출된 좌표를 통해 외접 사각형 그리기(빨간색 -> 255,0,0)
    // 설정한 임계값 : Cb(80~125), Cr(140,185)
    // 교수님이 제공해주신 피부값 threshold
    /*for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            // Cb성분(blue의 정도), Cr성분(red의 정도)의 각각의 임계값을 설정해서
            if((80<Cb[i*W+j] && Cb[i*W+j]<125) && (140<Cr[i*W+j] && Cr[i*W+j]<185)){
                // 조건에 해당한다면 => 결과 파일에 화소 정보 옮겨주기 ...
                Temp[i*W*3+j*3]=Image[i*W*3+j*3];
                Temp[i*W*3+j*3+1]=Image[i*W*3+j*3+1];
                Temp[i*W*3+j*3+2]=Image[i*W*3+j*3+2];
            }
            else{
                Temp[i*W*3+j*3] = Temp[i*W*3+j*3+1] = Temp[i*W*3+j*3+2] = 0;
            }
        }
    }*/
    /* 제시된 논문에서 제공하는 사람 피부 threshold 값
     R > 95 and G > 40 and B > 20 and R > G and R > B
     and | R - G | > 15 and A > 15 and Cr > 135 and
     Cb > 85 and Y > 80 and Cr <= (1.5862*Cb)+20 and
     Cr>=(0.3448*Cb)+76.2069 and
     Cr >= (-4.5652*Cb)+234.5652 and
     Cr <= (-1.15*Cb)+301.75 and
     Cr <= (-2.2857*Cb)+432.85nothing
     */
    /*for(int i=0; i<H; i++){
        for(int j=0; j<W; j++){
            if(Image[i*W*3+j*3+2]>95 && Image[i*W*3+j*3+1]>40 && Image[i*W*3+j*3]>20 &&
               Image[i*W*3+j*3+2]>Image[i*W*3+j*3+1] && Image[i*W*3+j*3+1]>Image[i*W*3+j*3] &&
               abs(Image[i*W*3+j*3+2]-Image[i*W*3+j*3+1])>15 &&
               Cr[i*W+j]>140 && Cb[i*W+j]>85 && Y[i*W+j]>80 &&
               Cr[i*W+j]<=(1.5862*Cb[i*W+j])+20 &&
               Cr[i*W+j]>=(0.3448*Cb[i*W+j])+76.2069 &&
               Cr[i*W+j]>=(-4.5652*Cb[i*W+j])+234.5652 &&
               Cr[i*W+j]<=(-1.15*Cb[i*W+j])+301.75 &&
               Cr[i*W+j]<=(-2.2857*Cb[i*W+j])+432.85){
                Temp[i*W*3+j*3]=Image[i*W*3+j*3];
                Temp[i*W*3+j*3+1]=Image[i*W*3+j*3+1];
                Temp[i*W*3+j*3+2]=Image[i*W*3+j*3+2];
            }
        }
    }*/
    //int LUX, LUY, RDX, RDY;
    // 기존의 함수를 컬러 영상에 맞게 수정한 함수를 사용
    //Obtain2DBoundingBoxForColor(Temp,W,H,&LUX,&LUY,&RDX,&RDY);
    //DrawRectOutlineForColor(Image,W,H,LUX,LUY,RDX,RDY);
    // ==================================================================================================================
    // 12주차 : 모폴로지 연산
    // 동전 떼기 위해서 7번 수행
    //Erosion(Image,Output,W,H);
    //Erosion(Output,Image,W,H);
    /*Dilation(Image,Output,W,H);
    Dilation(Output,Image,W,H);
    Dilation(Image,Output,W,H);
    Dilation(Output,Image,W,H);
    Dilation(Image,Output,W,H);
    
    Erosion(Output,Image,W,H);
    Erosion(Image,Output,W,H);
    Erosion(Output,Image,W,H);
    Erosion(Image,Output,W,H);*/
    
    // 12주차 과제에 대해서 
    // 세선화(Thining)
    // 주변화소 카운트 x 전경화소에서 배경화소로 바뀌는 개수를 카운트해라
    // 전경하ㅗ소 기준 8방향에 대해서 0,0에서 start해서
    // 전경에서 배경으로 바뀌는 타이밍만 count
    // 128로 표현
    /*Dilation(Image, Output, W, H);
    Dilation(Output, Image, W, H);
    Dilation(Image, Output, W, H);
    Erosion(Output, Image, W, H);
    Erosion(Image, Output, W, H);
    Erosion(Output, Image, W, H);
    InverseImage(Image, Image, W, H);
    zhangSuen(Image, Image, H, W);
    FeatureExtractThinImage(Image,Output,W,H);*/
    // ==================================================================================================================
    //int Cx,Cy;
    //int LUX,LUY,RDX,RDY;
    //ObtainHistogram(Image,Histo,hInfo.biWidth,hInfo.biHeight);
    //ObtainAHistogram(Histo,AHisto);
    //InverseImage(Image,Output,W,H);
    //BrightnessAdj(Image,Output,W,H,-50);
    //ContrastAdj(Image,Output,W,H,0.7);
    //HistogramStretching(Image,Output,Histo,W,H);
    //HistogramEqualization(Image,Output,AHisto,W,H);
    //BYTE Thres=GonzalezBinThresh(Image,Histo,3);
    //Binarization(Image,Output,W,H,Thres);
    //AverageConv(Image,Output,W,H);
    //GaussAvrConv(Image,Output,W,H);
    //Prewitt_X_Conv(Image,Temp,W,H);
    //Prewitt_Y_Conv(Image,Output,W,H);
    //Binarization(Output,Output,W,H,15);
    //prewitt X,Y 결과 합치기
    /*for(int i=0; i<ImgSize; i++){
        if(Temp[i]>Output[i]) Output[i]=Temp[i];
    }*/
    //Binarization(Output,Output,W,H,20);
    //Sobel_Y_Conv(Image,Output,W,H);
    //Laplace_Conv(Image,Output,W,H);
    //Binarization(Output,Output,W,H,30);
    //GaussAvrConv(Image,Temp,W,H);
    //Laplace_Conv_DC(Temp,Output,W,H);
    //MedianFiltering(Image,Output,W,H,3);
    //BYTE Thres=GonzalezBinThresh(Image,Histo,3);
    //Binarization(Image,Temp,W,H,Thres);
    //m_BlobColoring(Temp,W,H);
    /*for(int i=0; i<ImgSize; i++){
        Output[i]=Image[i];
    }*/
    //BinaryImageEdgeDetection(Temp,Output,W,H);
    //BYTE Thres=GonzalezBinThresh(Image,Histo,3);
    //Binarization(Image,Temp,W,H,Thres);
    //InverseImage(Temp,Temp,W,H);
    //m_BlobColoring(Temp,W,H);
    /*for(int i=0; i<ImgSize; i++){
        Output[i]=Image[i];
    }*/
    //BinaryImageEdgeDetection(Temp,Output,W,H);
    //Obtain2DBoundingBox(Temp,W,H,&LUX,&LUY,&RDX,&RDY);
    //Obtain2DCenter(Temp,W,H,&Cx,&Cy);
    //DrawRectOutline(Output,W,H,LUX,LUY,RDX,RDY);
    //DrawCrossLine(Output,W,H,Cx,Cy);
    
    
    // 평활화
    //HistogramEqualization(Image,Output,AHisto,hInfo.biWidth,hInfo.biHeight);
    // 스트레칭
    //HistogramStretching(Image, Output, Histo, W, H);
    //int Thres=GonzalezBinThresh(Image,Histo,3);
    //printf("%d\n",Thres);
    //printf("%d\n",Thres);
    // 이진화 (여기서 임계값을 Gonzalez 방법으로 구해라)
    
    
    
    // 동공 외접 사각형 그리기
    //Binarization(Image,Temp,W,H,50); // 왜 임계값을 낮추는 ?
    //saveBMPFile(hf,hInfo,hRGB,Temp,hInfo.biWidth,hInfo.biHeight,"pupil1_binarization_output.bmp");
    //InverseImage(Temp,Temp,W,H);
    //saveBMPFile(hf,hInfo,hRGB,Temp,hInfo.biWidth,hInfo.biHeight,"pupil1_bin and Inverse_output.bmp");
    //InverseImage(Output,Output,W,H);
    //m_BlobColoring(Temp,H,W); // 동공만 추출완료 ( 동공 => 0, 동공x=>255)
    //saveBMPFile(hf,hInfo,hRGB,Temp,hInfo.biWidth,hInfo.biHeight,"pupil1_bin and Inverse and labeling_output.bmp");
    // 무게중심 구하기
    // printf("%d %d\n",Cx,Cy); 무게중심확인하기
    //Obtain2DCenter(Temp,W,H,&Cx,&Cy); // 이진영상의 무게 중심 구하기
    //Obtain2DBoundingBox(Temp,W,H,&LUX,&LUY,&RDX,&RDY); // 이진 영상의 외접직사각형 좌표 추출
    //DrawRectOutline(Image,W,H,LUX,LUY,RDX,RDY);
    //DrawCrossLine(Image,W,H,Cx,Cy);
    
    //InverseImage(Temp,Temp,hInfo.biWidth,hInfo.biHeight);
    // 7주차 실습(라벨링) grassfire알고리즘
    /*m_BlobColoring(Temp,hInfo.biHeight,hInfo.biWidth);
    
     for(int i=0; i<ImgSize;i++){
        Output[i]=Image[i];
    }
    for(int i=0; i<H;i++){
        for(int j=0; j<W;j++){
            if(Temp[i*W+j]==0){ // 4주변화소 check(동서남북) // 8주변화소하면 경계가 두꺼워짐
                if(!(Temp[(i-1)*W+j]==0 &&
                   Temp[(i+1)*W+j]==0 &&
                    Temp[(i)*W+(j-1)]==0 &&
                     Temp[(i)*W+(j+1)]==0)){
                    Output[i*W+j]=255;
                }
                    
            }
        }
    }*/
    /*for(int i=0; i<ImgSize;i++){
        //Output[i]=255;
        Output[i]=Image[i];
    }*/
    //BinaryImageEdgeDetection(Temp,Output,W,H);

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
    //GaussAvrConv(Image,Temp,hInfo.biWidth,hInfo.biHeight);
    //Prewitt_X_Conv(Image,Temp,hInfo.biWidth,hInfo.biHeight);
    //Prewitt_Y_Conv(Image,Temp,hInfo.biWidth,hInfo.biHeight);
    //Sobel_X_Conv(Image,Temp,hInfo.biWidth,hInfo.biHeight);
    //Laplace_Conv(Image,Output,hInfo.biWidth,hInfo.biHeight);
    //Laplace_Conv_DC(Temp,Output,hInfo.biWidth,hInfo.biHeight);
    //Binarization(Temp,Output,hInfo.biWidth,hInfo.biHeight,25); // 경계를 분명하게 해주기 위해서 이진화
    // 하얀픽셀들을 합집합 처리하면 완전한 coin 으로 가능(합치기)
    
    //0406
    /*// 미디엄 필터링(Median filtering) (impulse노이즈에 적합)
    int W=hInfo.biWidth;
    int H=hInfo.biHeight;
    BYTE temp[9]; // 정렬 위한
    int i,j;
    for(i=1;i<H-1;i++){
        for(j=1;j<W-1;j++){
            temp[0]=Image[(i-1)*W+j-1];
            temp[1]=Image[(i-1)*W+j];
            temp[2]=Image[(i-1)*W+j+1];
            temp[3]=Image[i*W+j-1];
            temp[4]=Image[i*W+j]; // center
            temp[5]=Image[i*W+j+1];
            temp[6]=Image[(i+1)*W+j-1];
            temp[7]=Image[(i+1)*W+j];
            temp[8]=Image[(i+1)*W+j+1];
            Output[i*W+j]=Median(temp,9); // temp배열 정렬해서 중간에 해당하는 값을 반환하는 함수
            //Output[i*W+j]=MaxPooling(temp,9); // pepper noise(검은색 노이즈만) 껴있는 경우, pepper noise는 사라지고 salt noise들이 굉장히 많이 늘어날 것임
            //Output[i*W+j]=MinPooling(temp,9); // salt noise 삭제, pepper noise 많아짐
        }
    }*/
    // --------------------------------------------------------------------------------------------
    // 6주차 과제
    // Median filtering
    /*int Length=3; // 미디언 필터링 마스크의 길이를 설정
    int Margin=Length/2; // center를 위한 margin확보 (3by3의 경우 (3/2,3/2) 즉, (1,1)이 center가 됨)
    int Wsize=Length*Length; // 필터링 마스크의 사이즈 설정 ( Length by Length)
    BYTE * temp=(BYTE *)malloc(sizeof(BYTE)* Wsize); // 이전 코드에서는 사이즈를 9로 결정하고 배열을 선언하는 정적 할당 방식을 사용하였지만 이번에는 동적할당(Wsize만큼)
    int W=hInfo.biWidth,H=hInfo.biHeight; // Image 가로,세로 사이즈
    int i,j,m,n; // for문을 위한 변수 선언
    // convolution 과 같은 4중 for 문 활용
    for(i=Margin;i<H-Margin;i++){ // '행'에 해당하는 좌표, 왜 Margin~(H-Margin)인가? 영상의 사이즈 밖 접근을 방지하기 위해 center에서 시작
        for(j=Margin;j<W-Margin;j++){ // '열'에 해당하는 좌표, 위와 같은 이유로 Margin~(W-Margin)
            for(m=-Margin;m<=Margin;m++){ // center 기준에서 위 아래 행 방문을 위한 반복문, 0보다 작으면 위, 0이면 center, 0보다 크면 아래 행 방문
                for(n=-Margin;n<=Margin;n++){ // center 기준에서 왼쪽 오른쪽 열 방문을 위한 반복문, 0보다 작으면 왼쪽, 0이면 center, 0보다 크면 오른쪽 열 방문
                    temp[(m+Margin)*Length + (n+Margin)]=Image[(i+m)*W+j+n];
                    // 2차원 배열의 index를 1차원 배열의 index로 설정하는 방식을 통해 원본 영상의 값을 temp 배열에 복사해줌
                }
            }
            Output[i*W+j]=Median(temp,Wsize); // 한번의 Length by Legnth 마스크에 따라 구해진 temp 배열을 Median 함수의 파라미터로 전달하여 반환되는 중간값을 결과 영상에 넣어줌
        }
    }
    free(temp);*/ // 동적할당 해지
    // 과제에서 원하는 것이 이렇게 작성된 Median filtering 방식을 마스크 사이즈별로 수행해보라는 것인데
    // 결국 동일한 틀에서 Length 값만 바뀌는 것이므로 이를 별도의 함수로 구성하더라도 큰 문제가 없어보임
    // 따라서 함수로 만들어서 사용함
    //MedianFiltering(Image,Output,hInfo.biWidth,hInfo.biHeight,3);
    /*BYTE * Output1 = (BYTE *)malloc(ImgSize);
    MedianFiltering(Image,Output1,hInfo.biWidth,hInfo.biHeight,5);
    BYTE * Output2 = (BYTE *)malloc(ImgSize);
    MedianFiltering(Image,Output2,hInfo.biWidth,hInfo.biHeight,7);
    BYTE * Output3 = (BYTE *)malloc(ImgSize);
    MedianFiltering(Image,Output3,hInfo.biWidth,hInfo.biHeight,9);
    saveBMPFile(hf,hInfo,hRGB,Output1,hInfo.biWidth,hInfo.biHeight,"median_5.bmp");
    saveBMPFile(hf,hInfo,hRGB,Output2,hInfo.biWidth,hInfo.biHeight,"median_7.bmp");
    saveBMPFile(hf,hInfo,hRGB,Output3,hInfo.biWidth,hInfo.biHeight,"median_9.bmp");*/
    
    // ---------------------------------------------------------------------------------------------
    // average 필터링 (가우스 노이즈에 적합)
    //AverageConv(Image,Output,hInfo.biWidth,hInfo.biHeight); // 평균필터를 통해 블러시킴-> noise가 퍼짐
    // 결과물 파일 저장
    
    //9주차
    //Translation(Image,Output, W, H, 50,30);
    //Scaling(Image,Output,W,H,1.3,0.7);
    //Rotation(Image,Output,W,H,60);
    //VerticalFlip(Image,W,H);
    //Rotation(Image,Output,W,H,30);
    
    /*// 피부 영역 추출 결과 -> Temp
    saveBMPFile(hf,hInfo,hRGB,Temp,hInfo.biWidth,hInfo.biHeight,"face_assign_temp.bmp");
    
    // 피부 영역에 외접 사각형 결과 -> Image
    saveBMPFile(hf,hInfo,hRGB,Image,hInfo.biWidth,hInfo.biHeight,"face_assign_result.bmp");*/
    //Translation(Image,Output,W,H,0,-50);
    //Scaling(Image, Output, W, H, 3.0, 3.0);
    //Rotation(Image, Output, W, H, 45);
    //VerticalFlip(Image, W, H);
    //HorizontalFlip(Image, W, H);
    /*Erosion(Image, Output, W, H);
    Erosion(Output, Image, W, H);
    Dilation(Image, Output, W, H);*/
    RotationVer3(Image,Output,W,H,30);
    //Rotation(Image, Output, W, H, 30);
    saveBMPFile(hf,hInfo,hRGB,Output,hInfo.biWidth,hInfo.biHeight,"lenna rotation by center 2.bmp");
    free(Image);
    free(Output);
    free(Temp);
    //free(Output1);
    //free(Output2);
    //free(Output3);
    
    //YCbCr 확인을 위한 Raw 파일 출력
    /*fp = fopen("Y.raw", "wb");
    fwrite(Y, sizeof(BYTE), W * H, fp);
    fclose(fp);
    fp = fopen("Cb.raw", "wb");
    fwrite(Cb, sizeof(BYTE), W* H, fp);
    fclose(fp);
    fp = fopen("Cr.raw", "wb");
    fwrite(Cr, sizeof(BYTE), W* H, fp);
    fclose(fp);*/
    //free(Y);
    //free(Cb);
    //free(Cr);
}

