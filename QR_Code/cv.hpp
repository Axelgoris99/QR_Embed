#pragma once
#include <opencv2/core.hpp>
#include <vector>
#include <string>

/// <summary>
/// Show a picture in a new windows
/// </summary>
/// <param name="texte">the name of the window</param>
/// <param name="image">the image to be shown in the Mat format</param>
void afficherImage(std::string const& texte, cv::Mat const& image);


/// <summary>
/// Take the final QR and mix it with the incoming picture to obtain the final one
/// </summary>
/// <param name="qrCodeMat">The QrCode containing the phrase we want</param>
/// <param name="cheminImageEntre">The picture fo be fused with</param>
/// <param name="cheminImageSortie">The picture with the embed QR Code</param>
void EmbedQrCode(cv::Mat& qrCodeMat, cv::Mat& qrCodeRef, std::string const& cheminImageEntre, std::string const& cheminImageSortie, int const& version);


/// <summary>
/// Load an image
/// </summary>
/// <param name="image">The name of the image we want to use</param>
cv::Mat LoadImage(std::string const& image);

cv::Mat finalColor(cv::Mat const& pic, cv::Mat const& qrCode, cv::Mat const& qrRef, int const& version);