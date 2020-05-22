#include<fstream>
#include<iostream>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include<conio.h> 

using namespace std;



//transformation image qrt à la matrice
void QRToMatrice(string file, cv::Mat &inMat)
{
	int nb_col = inMat.cols;
	int nb_lig = inMat.rows;
	uchar *mat_input;
	ofstream flux;
	flux.open(file);

	//parcours l'image si pixel<100 il le remplace par 1 (noir)
	//si >100 on le note 0 (blanc)

	for (int i = 0; i < nb_lig; ++i){
		mat_input = inMat.ptr<uchar>(i);
		for (int j = 0; j < nb_col; ++j){
			if ((int)(mat_input[j] ) < 100){	
				flux << "1";
			}
			else{	
				flux << "0";
			}
		}
		flux << endl;
	}
}

//matrice dans fichier txt a image
void fileToImage(string file,cv::Mat &outMat) {
	
	uchar *mat_output;
	ifstream flux;
	flux.open(file);
	string line;
	getline(flux, line);
	int i = 1;
	while (getline(flux,line)){
		mat_output = outMat.ptr<uchar>(i);
		for (int j = 0; j < line.size(); ++j)
		{
			if (line.at(j) == '1'){
				mat_output[j] = 0;
			}
			else{
				mat_output[j] = 255;
			}
		}
		i++;
	}
}



//comparaison entre 2 images

void compareImages(cv::Mat &image1, cv::Mat &image2) {

	int nRows = MIN(image1.rows, image2.rows);
	int nCols = MIN(image1.cols, image2.cols);
	uchar *p_inputMat1, *p_inputMat2;
	int compare = 0;
	int total = 0;
	for (int i = 0; i < nRows; ++i){
		p_inputMat1 = image1.ptr<uchar>(i);
		p_inputMat2 = image2.ptr<uchar>(i);
		for (int j = 0; j < nCols; ++j){
			total++;

			if ((int)(p_inputMat1[j]) != (int)(p_inputMat2[j])){
				compare++;
			}
			
		}
		
	}
	cout << endl << "Nombre de Pixels total :" << total << endl << endl;
	cout << endl <<"Nombre de Pixels differents :"<< compare << endl << endl;
}

//retourne nombre de ligne d'un fichier

int getLigne(string file) {
	ifstream flux;
	flux.open(file);
	string line;
	int i = 1;
	while (getline(flux, line)){
		i++;
	}
	return i;
}

//retourne nombre colonne d'un fichier
int getColonne(string file) {
	ifstream flux;
	flux.open(file);
	string line;
	int i = 1;
	getline(flux, line);
	return line.size();
}

//fonction de seuillage 
void thresholdIntegral(cv::Mat &inputMat, cv::Mat &outputMat)
{
	CV_Assert(!inputMat.empty());
	CV_Assert(inputMat.depth() == CV_8U);
	CV_Assert(inputMat.channels() == 1);
	CV_Assert(!outputMat.empty());
	CV_Assert(outputMat.depth() == CV_8U);
	CV_Assert(outputMat.channels() == 1);
	//nombre de colonne de l'image d'entrée
	int nCols = inputMat.cols;
	//nombre de ligne de l'image d'entrée
	int nRows = inputMat.rows;
	// création de integral image
	cv::Mat sumMat;
	cv::integral(inputMat, sumMat);

	CV_Assert(sumMat.depth() == CV_32S);
	CV_Assert(sizeof(int) == 4);

	int S = MAX(nRows, nCols) / 8;
	double T = 0.15;

	int s2 = S / 2;
	int x1, y1, x2, y2, count, sum;

	int *p_y1, *p_y2;
	uchar *p_inputMat, *p_outputMat;

	ofstream flux;
	flux.open("txt.txt");

	for (int i = 0; i < nRows; ++i)
	{
		y1 = i - s2;
		y2 = i + s2;
		if (y1 < 0) {
			y1 = 0;
		}
		if (y2 >= nRows) {
			y2 = nRows - 1;
		}
		p_y1 = sumMat.ptr<int>(y1);
		p_y2 = sumMat.ptr<int>(y2);
		p_inputMat = inputMat.ptr<uchar>(i);
		p_outputMat = outputMat.ptr<uchar>(i);

		for (int j = 0; j < nCols; ++j){
			x1 = j - s2;
			x2 = j + s2;
			if (x1 < 0) {
				x1 = 0;
			}
			if (x2 >= nCols) {
				x2 = nCols - 1;
			}

			count = (x2 - x1)*(y2 - y1);
			sum = p_y2[x2] - p_y1[x2] - p_y2[x1] + p_y1[x1];

			if ((int)(p_inputMat[j] * count) < (int)(sum*(1.0 - T))){
				p_outputMat[j] = 0;
				flux << "1";

			}
			else{
				p_outputMat[j] = 255;
				flux << "0";
			}
		}
		flux << endl;
	}
}


int main(){
	
			string file;
			
			cout << "Veuillez saisir le nom du fichier de la matrice avec extension :" << endl;
			cin >> file;

			cv::Mat img1 = cv::Mat::zeros(getLigne(file),getColonne(file), CV_8UC1);
			fileToImage(file, img1);
			imshow("image Original", img1);
			cv::waitKey(0);

			string img ;
			string file2 = "matrice2.txt";
			cout << "Veuillez saisir le nom de la capture d'ecran :" << endl;
			cin >> img;
				cv::Mat src = cv::imread(img);
				cv::Mat blood;
				cv::cvtColor(src, blood, CV_BGR2GRAY);
				QRToMatrice(blood, file2);
				cv::Mat result = cv::Mat::zeros(src.size(), CV_8UC1);
			
			fileToImage(file2, result);
			imshow("image Binaire", result);
			cv::waitKey(0);
			
			compareImages(img1, result);	 
			system("pause");
			cv::waitKey(0);
			return 0;
}