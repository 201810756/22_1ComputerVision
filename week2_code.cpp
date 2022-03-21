#pragma pack(2)
#pragma warning(disable:4996)
#include <stdio.h> // 파일입출력함수사용
#include <stdlib.h> // 메모리 동적할당 malloc
#include <unistd.h>
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef long LONG;
// #include <Windows.h> << 윈도우에서 / 비트맵 파일 구조체 내용 저장
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
    // 헤더정보를 담는 변수 선언
    BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes (구조체 이름)
    BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
    RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes)
    FILE *fp; // 파일과 메모리의 연결(파일의 특정 위치를 가리킴)
    fp = fopen("LENNA.bmp", "rb"); // ("파일의이름","모드") rb=read  binary / wb=write binary
    // 파일의 첫번째 위치 정보를 fp가 가리키고 있음
    if(fp == NULL) return 0; // 예외처리(파일이 열리지 않았을떄 => 메인함수를 빠져나가라)
    fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // 파라미터1. 담을 메모리 변수 전달 14bytes 만큼 1번 읽어와서 hf변수에 저장
    // 현재 파일포인터가 가리키고 있는 곳에서부터 14bytes만큼 한번 읽어서 hf라는 변수에다가 저장해주어라.
    // 파일포인터는 fread한만큼 그 다음 위치를 가리키고 있음
    fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
    // 현재 파일포인터가 가리키고 있는 곳에서부터 14bytes만큼 한번 읽어서 hInfo변수에다가 저장해주어라.
    fread(hRGB, sizeof(RGBQUAD), 256, fp);
    // 현재 파일포인터가 가리키고 있는 곳에서부터 4bytes만큼 256번 읽어서 hRGB변수에다가 저장해주어라. (배열의 이름이 곧 주소임)
    // 파일 헤더에 저장되어 있는 모든 정보를 변수에 담기 완료
    // 이제 파일 포인터는 이미지 화소정보를 가리키고 있는 시작정보를 담고 있는 곳을 가리키고 있음
    // 이미지 파일의 크기를 미리 알수가 없기 때문에 동적할당 방식을 사용하기로함.
    // 프로그램 실행중에 메모리를 할당해주는 방식을 동적할당이라고함.
    int ImgSize = int(hInfo.biWidth * hInfo.biHeight);// hInfo 멤버변수 biWidth, biHeight에 이미지 가로,세로정보가 있음.
    // 가로사이즈 x 세로사이즈 => 픽셀개수(화소수)
    // 동적할당(Heap영역)_malloc이 주소를 반환해주므로 포인터타입 변수를 선언
    BYTE * Image = (BYTE *)malloc(ImgSize); // 원본 이미지 Image변수
    // => 의미적으로 BYTE Image[65536] 같은 느낌
    BYTE * Output = (BYTE *)malloc(ImgSize); // 영상처리 이후 저장된 이미지를 담을 Output변수
    fread(Image, sizeof(BYTE), ImgSize, fp);
    fclose(fp);
    // 파일을 열어서 파일의 정보들을 메모리에 담는데 성공함.
    // 파일이랑 볼일이 끝났으므로 fclose(fp); 닫아주자
    // 만약 안닫아주면 파일포인터가 파일을 계속 잡고 있음
    
    /* 영상처리 ImageProcessing*/
    
    //영상반전
    for(int i=0; i<ImgSize; i++){
        Output[i] = 255 - Image[i];
    }
    // white <-> black , inverse 이미지 생성

    // output 파일 저장해주기
    fp = fopen("output.bmp", "wb"); // mode : wb(write binary)
    // 기록해주기
    // 1byte 씩 14번만큼 현재 파일포인터가 가리키고 있는 곳에 읽어라
    fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
    fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
    // 헤더정보들은 그대로 가져오고
    // output에 있는 영상처리 결과들을 가져오게 됨
    fwrite(Output, sizeof(BYTE), ImgSize, fp);
    // 다 읽어왔으니 fclose(fp); 해줌 , 포인터랑 파일의 관계를 끊어줌
    fclose(fp);
    // 동적할당을 해준 Image,Output 변수들은 이제 더이상 필요가 없음
    // free 함수를 사용해서 연결을 끊어줌
    // 만약 안해주면 메모리가 계속 쓰이는 것으로 인식하게됨=> Memoery Leak현상
    free(Image);
    free(Output);
}
