/*
 * QR Code generator demo (C++)
 *
 * Run this command-line program with no arguments. The program computes a bunch of demonstration
 * QR Codes and prints them to the console. Also, the SVG code for one QR Code is printed as a sample.
 *
 * Copyright (c) Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/qr-code-generator-library
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */

#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgcodecs.hpp"
#include <time.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include "QrCode.hpp"
#include <string>
#include "QR_Code/cv.hpp"
using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;


// Function prototypes

static void printQr(const QrCode& qr);
static cv::Mat QrCodeToMat(const QrCode& qr);
static cv::Mat QrCodeRefToMat(const QrCode& qr);

// The main application program.
int main() {

	
	std::string text;
	std::cout << "Entrez la phrase a convertir \n";
	std::getline(std::cin, text);
	char* cstr = &text[0];
	const QrCode::Ecc errCorLvl = QrCode::Ecc::HIGH;

	const QrCode qr = QrCode::encodeText(cstr, errCorLvl);
	cv::Mat qrCodeMat = QrCodeToMat(qr);
	cv::Mat qrCodeMask = QrCodeRefToMat(qr);

	std::string entre;
	std::cout << "Entrez le nom de l'image que vous voulez utilisez \n";
	std::getline(std::cin, entre);

	std::string sortie;
	std::cout << "Entrez le nom de l'image avec le Qr Code en sortie \n";
	std::getline(std::cin, sortie);

	EmbedQrCode(qrCodeMat, qrCodeMask, entre, sortie, qr.getVersion());

	return EXIT_SUCCESS;
}




/*---- Utilities ----*/

// Prints the given QrCode object to the console.
static void printQr(const QrCode& qr) {
	int border = 4;
	for (int y = -border; y < qr.getSize() + border; y++) {
		for (int x = -border; x < qr.getSize() + border; x++) {
			std::cout << (qr.getModule(x, y) ? "##" : "  ");
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

static cv::Mat QrCodeToMat(const QrCode& qr) {
	cv::Mat qrCode;
	int border = 4;
	int tailleQr = qr.getSize();
	int tailleImageTotale = tailleQr + 2 * border;
	cv::Vec3b blanc((uchar)255, (uchar)255, (uchar)255);
	cv::Vec3b noir((uchar)0, (uchar)0, (uchar)0);

	qrCode.create(tailleImageTotale, tailleImageTotale, CV_8UC3);
	for (int y = 0; y < tailleImageTotale; y++) {
		for (int x = 0; x < tailleImageTotale; x++) {
			if (qr.getModule(x - border, y - border) == true) {
				qrCode.at<cv::Vec3b>(x, y) = noir;
			}
			else {
				qrCode.at<cv::Vec3b>(x, y) = blanc;
			}
		}
	}

	return qrCode;
}


std::vector<std::vector<int>> alignementLoc{
	{},
	{},
	{6,	18},
	{6,	22},
	{6,	26},
	{6,	30},
	{6,	34},
	{6,	22,	38},
	{6,	24, 42},
	{6,	26,	46},
	{6,	28,	50},
	{6,	30,	54},
	{6,	32,	58},
	{6,	34,	62},
	{6,	26,	46,	66},
	{6,	26,	48,	70},
	{6,	26,	50,	74},
	{6,	30,	54,	78},
	{6,	30,	56,	82},
	{6,	30,	58,	86},
	{6,	34,	62,	90},
	{6,	28,	50,	72,	94},
	{6,	26,	50,	74,	98},
	{6,	30,	54,	78,	102},
	{6,	28,	54,	80,	106},
	{6,	32,	58,	84,	110},
	{6,	30,	58,	86,	114},
	{6,	34,	62,	90,	118},
	{6,	26,	50, 74,	98,	122},
	{6,	30,	54,	78,	102, 126},
	{6,	26,	52,	78,	104, 130},
	{6,	30,	56,	82,	108, 134},
	{6,	34,	60,	86,	112, 138},
	{6,	30,	58,	86,	114, 142},
	{6,	34,	62,	90,	118, 146},
	{6,	30,	54,	78,	102, 126, 150},
	{6,	24,	50,	76,	102, 128, 154},
	{6,	28,	54,	80,	106, 132, 158},
	{6,	32,	58,	84,	110, 136, 162},
	{6,	26,	54,	82,	110, 138, 166},
	{6,	30,	58,	86,	114, 142, 170},
};

static cv::Mat QrCodeRefToMat(const QrCode& qr) {

	cv::Mat qrCode;
	int border = 4;
	int tailleQr = qr.getSize();
	int tailleImageTotale = tailleQr + 2 * border;
	int version = qr.getVersion();

	cv::Vec3b blanc((uchar)255, (uchar)255, (uchar)255);
	cv::Vec3b noir((uchar)0, (uchar)0, (uchar)0);
	qrCode.create(tailleImageTotale, tailleImageTotale, CV_8UC3);

	//Le carré en haut à gauche
	//On le colorie intégralement en noir
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			qrCode.at<cv::Vec3b>(i + border, j + border) = noir;
		}
	}

	//Le carré en bas à gauche et en haut à droite
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			qrCode.at<cv::Vec3b>(tailleImageTotale - 7 - border -1 + i, j + border) = noir;
			qrCode.at<cv::Vec3b>(i + border, j + tailleImageTotale - border -1 - 7) = noir;
		}
	}
	//Les centres de chaque pattern d'alignement sont dans la liste alignementLoc

		//La boucle qui permet de tester le centre de chaque pattern d'alignement
	std::vector<int> mid{ alignementLoc[version] };
	int taille = mid.size();

	for (int i = 0; i < taille; i++)
	{
		for (int j = 0; j < taille; j++)
		{
			//On teste si le pattern d'alignement n'overlap aucun des finder ou separator pattern
			bool affiche(true);
			if (qrCode.at<cv::Vec3b>(mid[i] - 2 + border, mid[j] - 2 + border) == noir ||
				qrCode.at<cv::Vec3b>(mid[i] - 2 + border, mid[j] + 2 + border) == noir ||
				qrCode.at<cv::Vec3b>(mid[i] + 2 + border, mid[j] - 2 + border) == noir ||
				qrCode.at<cv::Vec3b>(mid[i] + 2 + border, mid[j] + 2 + border) == noir
				)
			{
				affiche = false;
			}
			if (affiche)
			{
				//On fait un carré tout noir
				for (int x = 0; x < 5; x++)
				{
					for (int y = 0; y < 5; y++)
					{
						qrCode.at<cv::Vec3b>(mid[i] - 2 + x + border, mid[j] - 2 + y + border) = noir;
					}
				}
			}
		}
	}

	//The version informations. Have no need to be displayed perfectly !
	//qrCode.at<cv::Vec3b>((4 * version) + 10 + border, 8 + border) = noir;

	//for (int i = 0; i < 9; i++)
	//{
	//	qrCode.at<cv::Vec3b>(8 + border, i + border) = noir;
	//	qrCode.at<cv::Vec3b>(i + border, 8 + border) = noir;

	//}
	//for (int i = 0; i < 8; i++)
	//{
	//	qrCode.at<cv::Vec3b>((4 * version) + 10 + i + border, 8 + border) = noir;
	//	qrCode.at<cv::Vec3b>(8 + border, (4 * version) + 10 + i + border) = noir;
	//}

	//Il y a un bloc de 3*6 réservé en haut a droite et en bas à gauche 
	//Pour les versions 7 et supérieures
	if (version >= 7)
	{
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				qrCode.at<cv::Vec3b>((4 * version) + 6 + j + border, i + border) = noir;
				qrCode.at<cv::Vec3b>(i + border, (4 * version) + 6 + j + border) = noir;
			}
		}
	}

	return qrCode;
}