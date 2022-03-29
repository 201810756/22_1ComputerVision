# 22_1ComputerVision
---
### [3주차과제]
* 원본파일(lenna.bmp)

![LENNA](https://user-images.githubusercontent.com/79912683/160518421-4ee8972b-ccd4-4b06-9548-67d04859d680.jpg)
* 목표
1. output1.bmp : lenna.bmp 파일과 동일
2. output2.bmp : 원본 영상의 밝기값을 50만큼 증가시킨 영상(별도 클리핑 처리 x)
3. output3.bmp : 원본 영상의 반전 결과 영상(negative image)

~~~c
for(int i=0; i<ImgSize; i++){
        Output1[i]=Image[i];
        Output2[i]=Image[i]+50;
        Output3[i]=255-Image[i]; 
    }
~~~
**결과**

![output1](https://user-images.githubusercontent.com/79912683/160518541-51f79027-1a07-4d2a-83d2-742cb577cd79.jpg)

![output2](https://user-images.githubusercontent.com/79912683/160518547-0f5e6bdb-116a-4e67-a26c-5c8649e4d453.jpg)

![output3](https://user-images.githubusercontent.com/79912683/160518550-b2297cd2-53d1-41c5-9e39-5e842442be87.jpg)

---
### [4주차과제]
* 원본파일(coin.bmp)

![coin](https://user-images.githubusercontent.com/79912683/160518733-3c57e6d7-2a2d-4921-90ba-493da4b16fe8.jpg)

* 목표 : 이진화 임계치 결정(by Gonzalez)
~~~c
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
~~~
**결과**
* 임계값 : 99 -> 이진화 결과

![coin_output_with_Gonzalez](https://user-images.githubusercontent.com/79912683/160520988-18f15e89-03c6-4be6-afa2-d044b73474d9.jpg)

