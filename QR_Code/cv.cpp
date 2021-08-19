#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <string>
#include "cv.hpp"
#include <vector>

using namespace cv;
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::string;
typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double l;       // a fraction between 0 and 1
} hsl;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);
static hsl hsv2hsl(hsv in);
static hsv hsl2hsv(hsl in);


void afficherImage(string const& texte, Mat const& image)
{
    namedWindow(texte, WINDOW_AUTOSIZE);
    moveWindow(texte, 20, 20);
    imshow(texte, image);
    waitKey(0);
}

void EmbedQrCode(Mat& qrCodeMat, Mat& qrCodeRef, string const& cheminImageEntre, string const& cheminImageSortie) {
    Mat pic;
    pic = LoadImage(cheminImageEntre);
    Mat sortie = finalColor(pic, qrCodeMat, qrCodeRef);
    string output("sortie/");
    output += cheminImageSortie + ".jpg";
    imwrite(output, sortie);
}

Mat LoadImage(string const& image)
{
    Mat pic;
    string chemin("entre/");
    string fullPic = chemin.append(image);
    cout << fullPic << endl;
    pic = imread(chemin, IMREAD_COLOR);
    return pic;
}

Mat finalColor(Mat const& pic, Mat const& qrCode, Mat const& qrRef)
{
    //The value in which to bring the lightness back when it is not in it !
    float lightMin = 1.0f;
    float darkMin = 0.3f;
    cv::Vec3b noir((uchar)0, (uchar)0, (uchar)0);
    //A modul is a block of the QR Code and it has a center zone;
    //Set the size of the center of a module
    int sizeOfQrCode = qrCode.rows;
    int sizeOfPictureHeight = pic.rows;
    int sizeOfPictureWidth = pic.cols;
    int goodSize = sizeOfPictureHeight > sizeOfPictureWidth ? sizeOfPictureWidth : sizeOfPictureHeight;
    int sizeOfAModule = ceil(goodSize / sizeOfQrCode);
    int sizeOfTheCenter = sizeOfAModule / 3;
    if (sizeOfTheCenter < 3) {
        sizeOfTheCenter = 3;
    }

    //sizeOfTheCenter = sizeOfAModule;
    int sizeOfQrCodeInPx = sizeOfAModule * sizeOfQrCode;
    int distBetweenBorderLeftAndCenter = (sizeOfAModule - sizeOfTheCenter) / 2;
    int distBetweenBorderRightAndCenter = distBetweenBorderLeftAndCenter + sizeOfTheCenter;

    Mat final;
    final.create(sizeOfPictureHeight, sizeOfPictureWidth, CV_8UC3);
    for (int i = 0; i < sizeOfPictureHeight; i++)
    {
        for (int j = 0; j < sizeOfPictureWidth; j++)
        {
            Vec3b pixelOnWorkPicture = pic.at<Vec3b>(i, j);
            //cout << "i " + i / sizeOfAModule << " j " << j / sizeOfAModule << endl;
            if (i < sizeOfQrCodeInPx && j < sizeOfQrCodeInPx) {
                
                Vec3b pixelOnQrCode = qrCode.at<Vec3b>(i / sizeOfAModule, j / sizeOfAModule);
                Vec3b pixelOnMask = qrRef.at<Vec3b>(i / sizeOfAModule, j / sizeOfAModule);
                if (i % sizeOfAModule >= distBetweenBorderLeftAndCenter && i % sizeOfAModule <= distBetweenBorderRightAndCenter && j % sizeOfAModule>=distBetweenBorderLeftAndCenter && j % sizeOfAModule <= distBetweenBorderRightAndCenter && pixelOnMask != noir ){
                    
                    rgb rgbColor;
                    rgbColor.b = (float)pixelOnWorkPicture[0] / 255;
                    rgbColor.g = (float)pixelOnWorkPicture[1] / 255;
                    rgbColor.r = (float)pixelOnWorkPicture[2] / 255;

                    hsv hsvColor = rgb2hsv(rgbColor);
                    
                    if (pixelOnQrCode[0] > 125) {           
                        if (hsvColor.v < lightMin) {
                            hsvColor.v = lightMin;
                            hsvColor.s -= 0.2;
                            if (hsvColor.s > 0.7) {
                                hsvColor.s = 0.6;
                            }
                            rgbColor = hsv2rgb(hsvColor);
                            pixelOnWorkPicture[2] = rgbColor.r*255;
                            pixelOnWorkPicture[1] = rgbColor.g*255;
                            pixelOnWorkPicture[0] = rgbColor.b*255;
                        }
                        final.at<Vec3b>(i, j) = pixelOnWorkPicture;
                    }
                    else {

                        hsv hsvColor = rgb2hsv(rgbColor);
                        if (hsvColor.v > darkMin) {
                            hsvColor.v = darkMin;
                            rgbColor = hsv2rgb(hsvColor);
                            pixelOnWorkPicture[2] = rgbColor.r*255;
                            pixelOnWorkPicture[1] = rgbColor.g*255;
                            pixelOnWorkPicture[0] = rgbColor.b*255;

                        }

                        final.at<Vec3b>(i, j) = pixelOnWorkPicture;
                    }
                }
                else if (pixelOnMask == noir) {
                    final.at<Vec3b>(i,j) = pixelOnQrCode;
                } 
                else{
                    final.at<Vec3b>(i, j) = pixelOnWorkPicture;
                }
            }
            else {
                final.at<Vec3b>(i, j) = pixelOnWorkPicture;
            }
        }
    }

    return final;
}





hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min < in.b ? min : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max > in.b ? max : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    }
    else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if (in.r >= max)                           // > is bogus, just keeps compilor happy
        out.h = (in.g - in.b) / delta;        // between yellow & magenta
    else
        if (in.g >= max)
            out.h = 2.0 + (in.b - in.r) / delta;  // between cyan & yellow
        else
            out.h = 4.0 + (in.r - in.g) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if (out.h < 0.0)
        out.h += 360.0;

    return out;
}

rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if (in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch (i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

hsl hsv2hsl(hsv in) {
    hsl out;
    out.h = in.h;
    out.l = in.v * (1 - in.s / 2);
    if (out.l == 0 || out.l == 1) {
        out.s = 0;
    }
    else {
        float min = out.l < 1 - out.l ? out.l : 1 - out.l;
        out.s = (in.v - out.l) / (min);
    }
    return out;
}


hsv hsl2hsv(hsl in) {
    hsv out;
    out.h = in.h;
    float min = in.l < 1 - in.l ? in.l : 1 - in.l;
    out.v = in.l + in.s * min;
    if (out.v == 0) {
        out.s = 0;
    }
    else {
        out.s = 2 * (1 - in.l / out.v);
    }

    return out;
}