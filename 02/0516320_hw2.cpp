#include "bmpReader.h"
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

const int BILINEAR_RATIO = 7;
float ROTATION_ANGLE = 45;
float SHEAR_VERTICAL_DEGREE = 0.45;
float SHEAR_HORIZONTAL_DEGREE = 0.45;

const char* INPUT_IMAGE = "input.bmp";
const char* OUT_IMAGE[5] = {"output1.bmp", "output2.bmp", "output3.bmp", "output4.bmp", "output5.bmp"};

BmpImage bilinear(BmpImage pic, int nw, int nh);
BmpImage rotation(BmpImage pic, int nw,int nh);
BmpImage shear_vertical(BmpImage pic, int nw, int nh);
BmpImage shear_horizontal(BmpImage pic, int nw, int nh);

// TODO: add your pthread codes to speed up the program
//-----------------------------------------------------------------------------------------------------
int nimgh;
int nimgw;
BmpImage bilinearp_pic,tmp1p_pic,tmp2p_pic;
BmpImage f2_pic,f3_pic,f4_pic,f5_pic;	
float ra1=45,svd1=0.3,shd1=0.6;
float ra2=60,svd2=0.45,shd2=0.45;


void *rotation1_child(void *ptr)
{
	int nw = nimgw;
	int nh = nimgh;
	f2_pic.data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));
	int w_offset = (int)(nw / 2);
	int h_offset = (int)(nh / 2);	
	for(int i = 0 ; i < nw; i++){
		for(int j = 0; j < nh; j++){
			for(int k = 0 ; k < 3 ; k++){ 
				double angle = (double)ra1 * M_PI / 180.0;
				double cos_theta = cos(angle);    
				double sin_theta = sin(angle);
				int relevant_w = i - w_offset;
				int relevant_h = j - h_offset;
				int trans_w = (int)(relevant_w * cos_theta - relevant_h * sin_theta) + w_offset;
				int trans_h = (int)(relevant_h * cos_theta + relevant_w * sin_theta) + h_offset;
				int pixel;
				if (trans_w >= 0 && trans_w < w_offset * 2 && trans_h >= 0 && trans_h < h_offset * 2){
					pixel = bilinearp_pic.data[3 * (trans_h * 2 * w_offset + trans_w) + k];
				}else{
					pixel = 0;
				}
				f2_pic.data[3 * (j * nw + i) + k] = pixel;
			}
		}
	}
	f2_pic.height=nh;
	f2_pic.width=nw;
	return NULL;
}
void *shearVert1_child(void *ptr)
{
	int nw = nimgw;
	int nh = nimgh;
	f3_pic.data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));
	for(int i = 0 ; i < nw ; i++){
		for(int j = 0 ; j < nh ; j++){
			for(int k = 0;k < 3;k++){
				int pixel = 0;
				int h_offset = (int)(nw * svd1 / 2);
				int trans_w = i;
				int trans_h = (int)(j + i * svd1) - h_offset;
				if (trans_h >= 0 && trans_h < nh){
					pixel = bilinearp_pic.data[3 * (trans_h * nw + trans_w) + k];
				}else{
					pixel = 0;
				}
				f3_pic.data[3 * (j * nw + i) + k] = pixel;
			}
		}
	}
	f3_pic.width = nw;
	f3_pic.height = nh;
	return NULL;
}
void *f5_child(void *ptr)
{
	int nw = nimgw;
	int nh = nimgh;
	tmp1p_pic.data= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));
	for(int i = 0 ; i < nw ; i++){
		for(int j = 0 ; j < nh ; j++){
			for(int k = 0;k < 3;k++){
				int pixel = 0;
				int h_offset = (int)(nw * svd2 / 2);
				int trans_w = i;
				int trans_h = (int)(j + i * svd2) - h_offset;
				if (trans_h >= 0 && trans_h < nh){
					pixel = bilinearp_pic.data[3 * (trans_h * nw + trans_w) + k];
				}else{
					pixel = 0;
				}
				tmp1p_pic.data[3 * (j * nw + i) + k] = pixel;
			}
		}
	}
	tmp1p_pic.width = nw;
	tmp1p_pic.height = nh;
	tmp2p_pic.data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));
	int w_offset = (int)(nw / 2);
	int h_offset = (int)(nh / 2);	
	for(int i = 0 ; i < nw; i++){
		for(int j = 0; j < nh; j++){
			for(int k = 0 ; k < 3 ; k++){
				double angle = (double)ra2 * M_PI / 180.0;
				double cos_theta = cos(angle);    
				double sin_theta = sin(angle);
				int relevant_w = i - w_offset;
				int relevant_h = j - h_offset;
				int trans_w = (int)(relevant_w * cos_theta - relevant_h * sin_theta) + w_offset;
				int trans_h = (int)(relevant_h * cos_theta + relevant_w * sin_theta) + h_offset;
				int pixel;
				if (trans_w >= 0 && trans_w < w_offset * 2 && trans_h >= 0 && trans_h < h_offset * 2){
					pixel = tmp1p_pic.data[3 * (trans_h * 2 * w_offset + trans_w) + k];
				}else{
					pixel = 0;
				}
				tmp2p_pic.data[3 * (j * nw + i) + k] = pixel;
			}
		}
	}
	tmp2p_pic.height=nh;
	tmp2p_pic.width=nw;
	f5_pic.data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));
	for(int i = 0 ; i < nw ; i++){
		for(int j = 0 ; j < nh ; j++){
			for(int k = 0;k < 3;k++){
				int pixel = 0;
				int w_offset = (int)(nh * shd2 / 2);
				int trans_w = (int)(i + j * shd2) - w_offset;
				int trans_h = j;
				if (trans_w >= 0 && trans_w < nw){
					pixel = tmp2p_pic.data[3 * (trans_h * nw + trans_w) + k];
				}else{
					pixel = 0;
				}
				f5_pic.data[3 * (j * nw + i) + k] = pixel;
			}
		}
	}
	f5_pic.width = nw;
	f5_pic.height = nh;
	return NULL;
}

void *shearHori1_child(void *ptr)
{
	int nw = nimgw;
	int nh = nimgh;
	f4_pic.data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));
	for(int i = 0 ; i < nw ; i++){
		for(int j = 0 ; j < nh ; j++){
			for(int k = 0;k < 3;k++){
				int pixel = 0;
				int w_offset = (int)(nh * shd1 / 2);
				int trans_w = (int)(i + j * shd1) - w_offset;
				int trans_h = j;
				if (trans_w >= 0 && trans_w < nw){
					pixel = bilinearp_pic.data[3 * (trans_h * nw + trans_w) + k];
				}else{
					pixel = 0;
				}
				f4_pic.data[3 * (j * nw + i) + k] = pixel;
			}
		}
	}
	f4_pic.width = nw;
	f4_pic.height = nh;
	return NULL;
}

//-----------------------------------------------------------------------------------------------------
int main(){
	
	struct timeval tv1, tv2;
	double without_time;
	// timing function 1
	printf("Without using pthread:\n");
	gettimeofday(&tv1, NULL);
	
	BmpReader* bmpReader = new BmpReader();
	BmpImage input_pic = bmpReader->ReadBMP(INPUT_IMAGE);

	int newWidth = (int)(input_pic.width * BILINEAR_RATIO);
	int newHeight = (int)(input_pic.height * BILINEAR_RATIO);

	//all image processing
	ROTATION_ANGLE = 45;
	SHEAR_VERTICAL_DEGREE = 0.3;
	SHEAR_HORIZONTAL_DEGREE = 0.6;
	
	BmpImage bilinear_pic = bilinear(input_pic, newWidth, newHeight);
	BmpImage rotation_pic = rotation(bilinear_pic, newWidth, newHeight);
	BmpImage shearVert_pic = shear_vertical(bilinear_pic, newWidth, newHeight);
	BmpImage shearHori_pic = shear_horizontal(bilinear_pic, newWidth, newHeight);
	
	ROTATION_ANGLE = 60;
	SHEAR_VERTICAL_DEGREE = 0.45;
	SHEAR_HORIZONTAL_DEGREE = 0.45;
	BmpImage tmp1_pic = shear_vertical(bilinear_pic, newWidth, newHeight);
	BmpImage tmp2_pic = rotation(tmp1_pic, newWidth, newHeight);
	BmpImage mix_pic = shear_horizontal(tmp2_pic, newWidth, newHeight);
	
	//Write bmp files
	bmpReader->WriteBMP(OUT_IMAGE[0], bilinear_pic);
	bmpReader->WriteBMP(OUT_IMAGE[1], rotation_pic);
	bmpReader->WriteBMP(OUT_IMAGE[2], shearVert_pic);
	bmpReader->WriteBMP(OUT_IMAGE[3], shearHori_pic);
	bmpReader->WriteBMP(OUT_IMAGE[4], mix_pic);
	
	//free memory
	free(input_pic.data);
	free(bilinear_pic.data);
	free(rotation_pic.data);
	free(shearVert_pic.data);
	free(shearHori_pic.data);
	free(tmp1_pic.data);
	free(tmp2_pic.data);
	free(mix_pic.data);	
	
	// timing function 2
	gettimeofday(&tv2, NULL);
	
	// TODO: modify (ID) to your student ID
	printf("(0516320)%f seconds\n", (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec)); 
	without_time=(double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec);
	
	// TODO: add your pthread codes to speed up the program
	//----------------------------------------------------------------------------------------------------------------------------------------
	printf("Using pthread:\n");
	gettimeofday(&tv1, NULL);
	input_pic = bmpReader->ReadBMP(INPUT_IMAGE);
	nimgw = (int)(input_pic.width * BILINEAR_RATIO);
	nimgh = (int)(input_pic.height * BILINEAR_RATIO);
	bilinearp_pic = bilinear(input_pic, newWidth, newHeight);
	pthread_t tid1,tid2,tid3,tid4;
    pthread_create(&tid1, NULL, shearVert1_child,NULL);
    pthread_create(&tid2, NULL, rotation1_child,NULL);
    pthread_create(&tid3, NULL, shearHori1_child,NULL);
    pthread_create(&tid4, NULL, f5_child,NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
	pthread_join(tid4, NULL); 
	bmpReader->WriteBMP(OUT_IMAGE[0], bilinearp_pic);
	bmpReader->WriteBMP(OUT_IMAGE[1], f2_pic);
	bmpReader->WriteBMP(OUT_IMAGE[2], f3_pic);
	bmpReader->WriteBMP(OUT_IMAGE[3], f4_pic);
	bmpReader->WriteBMP(OUT_IMAGE[4], f5_pic);
	free(input_pic.data);
	free(bilinearp_pic.data);
	free(tmp1p_pic.data);
	free(tmp2p_pic.data);
	free(f2_pic.data);
	free(f3_pic.data);
	free(f4_pic.data);
	free(f5_pic.data);	
	gettimeofday(&tv2, NULL);
	
	printf("(0516320)%f seconds\n", (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec)); 
	printf(" %f seconds faster\n", without_time-((double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec))); 
	printf("SpeedUp Rate %.2f\n", without_time/((double)(tv2.tv_usec - tv1.tv_usec) / 1000000 + (double)(tv2.tv_sec - tv1.tv_sec))); 

	return 0;
} 

BmpImage bilinear(BmpImage pic, int newWidth, int newHeight)
{
	unsigned char* data	= (unsigned char*)malloc(3 * newWidth * newHeight * sizeof(unsigned char));

	for(int i = 0 ; i < newWidth; i++){
		for(int j = 0;j < newHeight; j++){
			//k is RGB color, R = 2, G = 1, B = 0
			for(int k = 0;k < 3;k++){
				int pixel = 0;
				int relevant_w = (int)(i / BILINEAR_RATIO);   //Origin_x
				int relevant_h = (int)(j / BILINEAR_RATIO);   //Origin_y
				int reference_points[4];
				
				// the four reference points of this pixel
				reference_points[0] = pic.data[3 * (relevant_h * pic.width + relevant_w) + k];               //Q11 = p1
				reference_points[1] = pic.data[3 * (relevant_h * pic.width + (relevant_w + 1)) + k];         //Q12 = p2
				reference_points[2] = pic.data[3 * ((relevant_h + 1) * pic.width + relevant_w) + k];         //Q21 = p3
				reference_points[3] = pic.data[3 * ((relevant_h + 1) * pic.width + (relevant_w + 1)) + k];   //Q22 = p4
				
				// TODO: assign a new value to this pixel
				double fx1 = (double)i / (double)BILINEAR_RATIO - (double)relevant_w;
				double fx2 = 1 - fx1;
				double fy1 = (double)j / (double)BILINEAR_RATIO - (double)relevant_h;
				double fy2 = 1 - fy1;
	
				double w1 = fx2*fy2;
				double w2 = fx1*fy2;
				double w3 = fx2*fy1;
				double w4 = fx1*fy1;

			    pixel=(int)(w1*reference_points[0] +
					        w2*reference_points[1] + 
							w3*reference_points[2] +
							w4*reference_points[3]);
			
				if (pixel < 0) pixel = 0;
				if (pixel > 255) pixel = 255;	
				
				data[3 * (j * newWidth + i) + k] = pixel;
			}
		}
	}

	BmpImage newBmpImage;

	newBmpImage.data = data;
	newBmpImage.width = newWidth;
	newBmpImage.height = newHeight;
	
	return newBmpImage;
}

BmpImage rotation(BmpImage pic,int nw,int nh)
{
	unsigned char* data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));
	
	int w_offset = (int)(nw / 2);
	int h_offset = (int)(nh / 2);
	
	for(int i = 0 ; i < nw; i++){
		for(int j = 0; j < nh; j++){
			for(int k = 0 ; k < 3 ; k++){
				
				double angle = (double)ROTATION_ANGLE * M_PI / 180.0;
				double cos_theta = cos(angle);    
				double sin_theta = sin(angle);
				int relevant_w = i - w_offset;
				int relevant_h = j - h_offset;
				int trans_w = (int)(relevant_w * cos_theta - relevant_h * sin_theta) + w_offset;
				int trans_h = (int)(relevant_h * cos_theta + relevant_w * sin_theta) + h_offset;
				
				int pixel;
				
				if (trans_w >= 0 && trans_w < w_offset * 2 && trans_h >= 0 && trans_h < h_offset * 2){
					pixel = pic.data[3 * (trans_h * 2 * w_offset + trans_w) + k];
				}else{
					pixel = 0;
				}
				
				data[3 * (j * nw + i) + k] = pixel;
			}
		}
	}
	
	BmpImage newBmpImage;

	newBmpImage.data = data;
	newBmpImage.width = nw;
	newBmpImage.height = nh;
	
	return newBmpImage;
}

BmpImage shear_vertical(BmpImage pic, int nw, int nh)
{
	unsigned char* data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));

	for(int i = 0 ; i < nw ; i++){
		for(int j = 0 ; j < nh ; j++){
			//k is RGB color, R = 2, G = 1, B = 0
			for(int k = 0;k < 3;k++){
				int pixel = 0;
				int h_offset = (int)(nw * SHEAR_VERTICAL_DEGREE / 2);
				int trans_w = i;
				int trans_h = (int)(j + i * SHEAR_VERTICAL_DEGREE) - h_offset;
				
				if (trans_h >= 0 && trans_h < nh){
					pixel = pic.data[3 * (trans_h * nw + trans_w) + k];
				}else{
					pixel = 0;
				}
				
				data[3 * (j * nw + i) + k] = pixel;
			}
		}
	}
	
	BmpImage newBmpImage;

	newBmpImage.data = data;
	newBmpImage.width = nw;
	newBmpImage.height = nh;

	return newBmpImage;
}

BmpImage shear_horizontal(BmpImage pic, int nw, int nh)
{
	unsigned char* data	= (unsigned char*)malloc(3 * nw * nh * sizeof(unsigned char));

	for(int i = 0 ; i < nw ; i++){
		for(int j = 0 ; j < nh ; j++){
			//k is RGB color, R = 2, G = 1, B = 0
			for(int k = 0;k < 3;k++){
				int pixel = 0;
				
				// TODO: refer to shear_vertical, try to write shear_horizontal on your own
				int w_offset = (int)(nh * SHEAR_HORIZONTAL_DEGREE / 2);
				int trans_w = (int)(i + j * SHEAR_HORIZONTAL_DEGREE) - w_offset;
				int trans_h = j;
				
				if (trans_w >= 0 && trans_w < nw){
					pixel = pic.data[3 * (trans_h * nw + trans_w) + k];
				}else{
					pixel = 0;
				}
				
				data[3 * (j * nw + i) + k] = pixel;
			}
		}
	}
	
	BmpImage newBmpImage;

	newBmpImage.data = data;
	newBmpImage.width = nw;
	newBmpImage.height = nh;

	return newBmpImage;
}
