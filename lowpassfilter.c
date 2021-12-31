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
	int **matrix;//görüntüdeki piksellerin yer aldýðý matris
	int **newPhoto;
	char type[2];//pgm dosya tipi
	char width_str[3];//görüntünün geniþliði
	char height_str[3];//görüntünün yüksekliði
	char gray_level[3];//görüntüdeki maksimum gri seviyesi
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
	
	if(!strcmp(type,"P5")){//Eðer dosya tipi P5 ise binary modda açýlýr 
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
	
	if(title[0] == '#'){//Eðer dosyanýn baþlýðý varsa
		
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
		*width = ((width_str[0] - '0') * 100) + ((width_str[1] - '0') * 10) + (width_str[2] - '0');//Dosyadaki ikinci satýrdan görüntünün geniþliði ve yüksekliði hesaplanýr
		*height = ((height_str[0] - '0') * 100) + ((height_str[1] - '0') * 10) + (height_str[2] - '0');
		
		matrix = (int **)calloc((*height),sizeof(int *));//Matris için hafýzada yer açýlýr
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
					number[n] = k - '0';//Dosyada bulunan karakter deðerinden sayýsal deðer elde edilir
					n++;
					k = getc(photo);	
				}
				if(k != '\n'){
					for(l = n-1; l >= 0; l--){//Dosyadan okunan pikselin kaç basamaklý olduðuna göre sayýsal deðeri hesaplanýr
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
		
		temp = (char *)calloc((*height)*(*width),sizeof(char));//dosyadaki piksel deðerlerinin saklanacaðý dizi için hafýzada yer açýlmasý
		
		cc = 0;
		k = getc(photo);
		while(!feof(photo)){//Dosyadan piksellerin okunmasý
			temp[cc] = k;
			cc++;
			k = getc(photo);
		}
		
		cc = 0;
		for(i = 0; i < *height; i++){
			for(j = 0; j < *width; j++){
				if(temp[cc] < 0){//pikseller pozitif deðer aldýðý için dosyada negatif olan deðerlere 256 eklenir  
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
	
	newMatrix = (int **)calloc(height,sizeof(int *));//Filtrelenmiþ görüntü için matris oluþturlmasý
	for(i = 0; i < height; i++){
		newMatrix[i] = (int *)calloc(width,sizeof(int));
	}
	
	for(i = 0; i < width; i++){//Matrisin köþesinde bulunan deðerler deðiþtirilmeden filtrelenmiþ matrise atanýr
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
			for(k = i-1; k <= i+1; k++){//Görüntü matrisindeki her bir pikselin bulunduðu 3x3'lük matristeki elemanlarýn aritmetik ortalamasý hesaplanýr
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
			for(k = i-1; k <= i+1; k++){//Matristeki her bir piksel için oluþturulan 3x3'lük matristeki deðerler sýralanmak için geçici bir dizide saklanýr
				for(l = j-1; l <= j+1; l++){
					temp[count] = matrix[k][l];
					count++;
				}
			}
			
			for(k = 0; k < 8; k++){//Selection sort ile 3x3'lük matriste bulunan piksel deðerleri sýralanýr
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
			
			newMatrix[i][j] = temp[4];//Sýralama sonrasý ortadaki deðer yeni piksel deðeri olarak filtrelenmiþ matrise atanýr
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
		newPGM = fopen("AverageFilter.pgm","wb");//Filtrelenmiþ resmin kaydedileceði dosyanýn oluþturulmasý
	}
	else if(!strcmp(filter,"Median")){
		newPGM = fopen("MedianFilter.pgm","wb");//Filtrelenmiþ resmin kaydedileceði dosyanýn oluþturulmasý
	}
	
	temp = (int *)calloc(w*h,sizeof(int));
	
	index = 0;
	for(i = 0; i < h; i++){//Filtrelenmiþ matristeki deðerlerin temp dizine atanmasý
		for(j = 0; j < w; j++){
			temp[index] = matrix[i][j];
			index++;
		}
	}
	
	fprintf(newPGM,"%s\n",type);//Dosya tipinin yazýlmasý
	
	if(title[0] == '#'){//Eðer dosya baþlýðý varsa
		fprintf(newPGM,"%s",titleName);//Dosya baþlýðýnýn eklenmesi
	}
	
	fprintf(newPGM,"%s %s\n%s\n",width,height,graylevel);//Geniþlik,yükseklik ve gri seviyesi bilgilerinin dosyaya yazýlmasý
	
	if(!strcmp(type,"P2")){//P2 tipindeki dosya formatýna uygun olacak þekilde pikseller dosyaya yazýlýr
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
			
				while(d / 10 != 0){//pikselin kaç basamaklý olduðu hesaplanýr
					count_n++;
					d /= 10;
				}
				
				switch(count_n){
					case 0://piksel deðeri 1 basamaklý ise
						putc(temp[index] + '0',newPGM);
						break;
					case 1://piksel deðeri 2 basamaklý ise
						putc((temp[index] / 10) + '0',newPGM);
						putc((temp[index] % 10) + '0',newPGM);
						break;
					case 2://piksel deðeri 3 basamaklý ise
						putc((temp[index] / 100) + '0',newPGM);
						putc(((temp[index] / 10) % 10) + '0',newPGM);
						putc((temp[index] % 10) + '0',newPGM);
						break;
				}
				putc(' ',newPGM);//Dosyada her bir piksel arasýna boþluk eklenir
				index++;
			}
			
			putc('\n',newPGM);
		}
	}
	else if(!strcmp(type,"P5")){//P5 tipindeki dosyaya piksel deðerleri bir bütün olarak yazýlýr
		for(i = 0; i < h; i++){
			for(j = 0; j < w; j++){
				putc(matrix[i][j],newPGM);
			}
		}
	}
	
	fclose(file);
}
