#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int **createImageMatrixFromPGMFile(FILE *,char *,char *,char *,char *,int *,int *,char *,char *);
int **LowPassAveragingFilter(int **,int,int);
int **LowPassMedianFilter(int **,int,int);
void createNewPGMFile(FILE *,int **,char *,char *,char *,char *,int,int,char *,char *,char *,char *);

int main(){
	
	int i,j;
	FILE *photo;
	int **matrix;//g�r�nt�deki piksellerin yer ald��� matris
	int **newPhoto;
	char type[2];//pgm dosya tipi
	char width_str[3];//g�r�nt�n�n geni�li�i
	char height_str[3];//g�r�nt�n�n y�ksekli�i
	char gray_level[3];//g�r�nt�deki maksimum gri seviyesi
	int width, height;
	char title[50];
	char name[100];
	char titleName[50];
	char k;
	
	printf("Goruntu dosyasinin adini giriniz: ");
	scanf("%s",name);
	strcat(name,".pgm");
	photo = fopen(name,"r");
	fscanf(photo,"%s",type);
	fscanf(photo,"%s",title);
	fclose(photo);
	
	if(!strcmp(type,"P5")){//E�er dosya tipi P5 ise binary modda a��l�r 
		photo = fopen(name,"rb");
	}
	else if(!strcmp(type,"P2")){
		photo = fopen(name,"r+");
	}

	matrix = createImageMatrixFromPGMFile(photo,type,width_str,height_str,gray_level,&width,&height,title,titleName);

	fclose(photo);

	newPhoto = LowPassAveragingFilter(matrix,width,height);
	createNewPGMFile(photo,newPhoto,type,width_str,height_str,gray_level,width,height,title,name,titleName,"Average");
	
	newPhoto = LowPassMedianFilter(matrix,width,height);
	createNewPGMFile(photo,newPhoto,type,width_str,height_str,gray_level,width,height,title,name,titleName,"Median");
	
	printf("AverageFilter ve MedianFilter dosyalari olusturuldu.");
	
	return 0;
}

int **createImageMatrixFromPGMFile(FILE *photo,char type[],char width_str[],char height_str[],char gray_level[],int *width,int *height,char title[],char titleName[]){
	
	int i,j;
	int **matrix;
	int count = 0;
	int n, l;
	int number[3] = {0,0,0};
	char k;
	char *temp;
	int cc= 0;
	
	fscanf(photo,"%s",type);
	
	if(title[0] == '#'){//E�er dosyan�n ba�l��� varsa
		
		fscanf(photo,"%s",title);
		titleName[cc] = title[0]; 
		cc++;
		while(k != '\n'){
			k = getc(photo);
			titleName[cc] = k;
			cc++;
		}
	}
	
	if(!strcmp(type,"P2")){
		fscanf(photo,"%s",width_str);
		fscanf(photo,"%s",height_str);
		fscanf(photo,"%s",gray_level);
		getc(photo);
		*width = ((width_str[0] - '0') * 100) + ((width_str[1] - '0') * 10) + (width_str[2] - '0');//Dosyadaki ikinci sat�rdan g�r�nt�n�n geni�li�i ve y�ksekli�i hesaplan�r
		*height = ((height_str[0] - '0') * 100) + ((height_str[1] - '0') * 10) + (height_str[2] - '0');
		
		matrix = (int **)calloc((*height),sizeof(int *));//Matris i�in haf�zada yer a��l�r
		for(i = 0; i < (*height); i++){
			matrix[i] = (int *)calloc((*width),sizeof(int));
		}
		
		i = 0;
		while(i < (*height) && !feof(photo)){
			j = 0;
			while(j < (*width) && !feof(photo)){
				n = 0;
				k = getc(photo);
				number[0] = 0;number[1] = 0;number[2] = 0;
				while(k != ' ' && k != '\n' && !feof(photo)){
					number[n] = k - '0';//Dosyada bulunan karakter de�erinden say�sal de�er elde edilir
					n++;
					k = getc(photo);	
				}
				if(k != '\n'){
					for(l = n-1; l >= 0; l--){//Dosyadan okunan pikselin ka� basamakl� oldu�una g�re say�sal de�eri hesaplan�r
						matrix[i][j] += number[n-l-1] * pow(10,l);
					}
					j++;
				}
			}
			i++;
		}
	}
	else if(!strcmp(type,"P5")){
		
		fscanf(photo,"%s",width_str);
		fscanf(photo,"%s",height_str);
		fscanf(photo,"%s",gray_level);
		
		*width = ((width_str[0] - '0') * 100) + ((width_str[1] - '0') * 10) + (width_str[2] - '0');
		*height = ((height_str[0] - '0') * 100) + ((height_str[1] - '0') * 10) + (height_str[2] - '0');
		
		matrix = (int **)calloc((*height),sizeof(int *));
		for(i = 0; i < (*height); i++){
			matrix[i] = (int *)calloc((*width),sizeof(int));
		}
		
		temp = (char *)calloc((*height)*(*width),sizeof(char));//dosyadaki piksel de�erlerinin saklanaca�� dizi i�in haf�zada yer a��lmas�
		
		cc = 0;
		k = getc(photo);
		while(!feof(photo)){//Dosyadan piksellerin okunmas�
			temp[cc] = k;
			cc++;
			k = getc(photo);
		}
		
		cc = 0;
		for(i = 0; i < *height; i++){
			for(j = 0; j < *width; j++){
				if(temp[cc] < 0){//pikseller pozitif de�er ald��� i�in dosyada negatif olan de�erlere 256 eklenir  
					matrix[i][j] = temp[cc] + 256;
				}
				else{
					matrix[i][j] = temp[cc];
				}
				cc++;
			}
		}
		
	}
	
	return matrix;
}

int **LowPassAveragingFilter(int **matrix,int width,int height){
	
	int i, j, k, l;
	int **newMatrix;
	int sum;
	
	newMatrix = (int **)calloc(height,sizeof(int *));//Filtrelenmi� g�r�nt� i�in matris olu�turlmas�
	for(i = 0; i < height; i++){
		newMatrix[i] = (int *)calloc(width,sizeof(int));
	}
	
	for(i = 0; i < width; i++){//Matrisin k��esinde bulunan de�erler de�i�tirilmeden filtrelenmi� matrise atan�r
		newMatrix[0][i] = matrix[0][i];
		newMatrix[height-1][i] = matrix[height-1][i];
	}
	
	for(i = 0; i < height; i++){
		newMatrix[i][0] = matrix[i][0];
		newMatrix[i][width-1] = matrix[i][width-1];
	}
	
	for (i = 1; i < height-1; i++){
		for(j = 1; j < width-1; j++){
			sum = 0;
			for(k = i-1; k <= i+1; k++){//G�r�nt� matrisindeki her bir pikselin bulundu�u 3x3'l�k matristeki elemanlar�n aritmetik ortalamas� hesaplan�r
				for(l = j-1; l <= j+1; l++){
					sum += matrix[k][l];
				}
			}
			
			newMatrix[i][j] = sum / 9;
		}
	}
	
	return newMatrix;
}

int **LowPassMedianFilter(int **matrix,int width,int height){
	
	int i, j, k, l;
	int **newMatrix;
	int temp[9];
	int count;
	int index;
	int pixel;
	int min;
	
	newMatrix = (int **)calloc(height,sizeof(int *));
	for(i = 0; i < height; i++){
		newMatrix[i] = (int *)calloc(width,sizeof(int));
	}
	
	for(i = 0; i < width; i++){
		newMatrix[0][i] = matrix[0][i];
		newMatrix[height-1][i] = matrix[height-1][i];
	}
	
	for(i = 0; i < height; i++){
		newMatrix[i][0] = matrix[i][0];
		newMatrix[i][width-1] = matrix[i][width-1];
	}
	
	for (i = 1; i < height-1; i++){
		for(j = 1; j < width-1; j++){
			count = 0;
			for(k = i-1; k <= i+1; k++){//Matristeki her bir piksel i�in olu�turulan 3x3'l�k matristeki de�erler s�ralanmak i�in ge�ici bir dizide saklan�r
				for(l = j-1; l <= j+1; l++){
					temp[count] = matrix[k][l];
					count++;
				}
			}
			
			for(k = 0; k < 8; k++){//Selection sort ile 3x3'l�k matriste bulunan piksel de�erleri s�ralan�r
				min = 256;
				index = k;
				for(l = k; l < 9; l++ ){
					if(temp[l] < min){
						min = temp[l];
						index = l;
					}
				}
				
				pixel = temp[k];
				temp[k] = min;
				temp[index] = pixel;
			}
			
			newMatrix[i][j] = temp[4];//S�ralama sonras� ortadaki de�er yeni piksel de�eri olarak filtrelenmi� matrise atan�r
		}
	}
	
	return newMatrix;
}

void createNewPGMFile(FILE *pgm,int **matrix,char type[],char width[],char height[],char graylevel[],int w,int h,char title[],char name[],char titleName[],char filter[]){
	
	FILE *newPGM;
	FILE *file;
	int count;
	int i, j;
	char k;
	int *temp;
	int index;
	int count_n;
	int d;
	
	if(!strcmp(type,"P5")){
		file = fopen(name,"rb");
	}
	else if(!strcmp(type,"P2")){
		file = fopen(name,"r+");
	}
	
	if(!strcmp(filter,"Average")){
		newPGM = fopen("AverageFilter.pgm","wb");//Filtrelenmi� resmin kaydedilece�i dosyan�n olu�turulmas�
	}
	else if(!strcmp(filter,"Median")){
		newPGM = fopen("MedianFilter.pgm","wb");//Filtrelenmi� resmin kaydedilece�i dosyan�n olu�turulmas�
	}
	
	temp = (int *)calloc(w*h,sizeof(int));
	
	index = 0;
	for(i = 0; i < h; i++){//Filtrelenmi� matristeki de�erlerin temp dizine atanmas�
		for(j = 0; j < w; j++){
			temp[index] = matrix[i][j];
			index++;
		}
	}
	
	fprintf(newPGM,"%s\n",type);//Dosya tipinin yaz�lmas�
	
	if(title[0] == '#'){//E�er dosya ba�l��� varsa
		fprintf(newPGM,"%s",titleName);//Dosya ba�l���n�n eklenmesi
	}
	
	fprintf(newPGM,"%s %s\n%s\n",width,height,graylevel);//Geni�lik,y�kseklik ve gri seviyesi bilgilerinin dosyaya yaz�lmas�
	
	if(!strcmp(type,"P2")){//P2 tipindeki dosya format�na uygun olacak �ekilde pikseller dosyaya yaz�l�r
		fscanf(file,"%s",type);
		fscanf(file,"%s",width);
		fscanf(file,"%s",height);
		fscanf(file,"%s",graylevel);
		getc(pgm);
		
		index = 0;
		while(!feof(file)){
			k = getc(file);
			count = 0;
			while(k != '\n' && !feof(file)){
				if(k == ' '){
					count++;
				}
				k = getc(file);
			}
			
			for(i = 0; i < count; i++){
				
				count_n = 0;
				d = temp[index];
			
				while(d / 10 != 0){//pikselin ka� basamakl� oldu�u hesaplan�r
					count_n++;
					d /= 10;
				}
				
				switch(count_n){
					case 0://piksel de�eri 1 basamakl� ise
						putc(temp[index] + '0',newPGM);
						break;
					case 1://piksel de�eri 2 basamakl� ise
						putc((temp[index] / 10) + '0',newPGM);
						putc((temp[index] % 10) + '0',newPGM);
						break;
					case 2://piksel de�eri 3 basamakl� ise
						putc((temp[index] / 100) + '0',newPGM);
						putc(((temp[index] / 10) % 10) + '0',newPGM);
						putc((temp[index] % 10) + '0',newPGM);
						break;
				}
				putc(' ',newPGM);//Dosyada her bir piksel aras�na bo�luk eklenir
				index++;
			}
			
			putc('\n',newPGM);
		}
	}
	else if(!strcmp(type,"P5")){//P5 tipindeki dosyaya piksel de�erleri bir b�t�n olarak yaz�l�r
		for(i = 0; i < h; i++){
			for(j = 0; j < w; j++){
				putc(matrix[i][j],newPGM);
			}
		}
	}
	
	fclose(file);
}
