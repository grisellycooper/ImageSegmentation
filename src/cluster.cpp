#include <iostream>
#include <cmath>
#include "../include/image.h"
#include "../include/cluster.h"

using namespace std;

Cluster::Cluster(Image *image)
{
    image = image;
    centroid = new Pixel(image->getRandomPixel()); 
} 

Cluster::~Cluster()
{
    delete centroid;
    for (int i = 0; i < pixelsList.size(); i++)
    {
        delete pixelsList[i];
    }
}

double Cluster::getDistanceTo(Pixel *pixel)
{
    int dRed = centroid->getRed() - pixel->getRed();
    int dGreen = centroid->getGreen() - pixel->getGreen();
    int dBlue = centroid->getBlue() - pixel->getBlue();
    double distance = sqrt(pow(dRed, 2) + pow(dGreen, 2) + pow(dBlue, 2));  ///Euclidean distance
    
    return distance;
}

double Cluster::getDistanceTo(int red, int green, int blue)
{
    int dRed = centroid->getRed() - red;
    int dGreen = centroid->getGreen() - green;
    int dBlue = centroid->getBlue() - blue;
    double distance = sqrt(pow(dRed, 2) + pow(dGreen, 2) + pow(dBlue, 2));  ///Euclidean distance

    return distance;
}

void Cluster::addPixel(Pixel *pixel)
{
    pixelsList.push_back(pixel);
}

double Cluster::updateCentroid()
{
    double aRed = 0;
    double aGreen = 0;
    double aBlue = 0;
    int listSize = pixelsList.size();
    double change = 0;

    for (int i = 0; i < listSize; i++)
    {
        aRed += pixelsList[i]->getRed();
        aGreen += pixelsList[i]->getGreen();
        aBlue += pixelsList[i]->getBlue();
    }

    if (listSize < 1)
    {
        listSize = 1;
    }

    aRed /= listSize;
    aGreen /= listSize;
    aBlue /= listSize;

    change = this->getDistanceTo(aRed, aGreen, aBlue);
    centroid->setRGB(aRed, aGreen, aBlue);

    return change;
}

void Cluster::updatePixelsList()
{
    for (int i = 0; i < pixelsList.size(); i++)
    {
        pixelsList[i]->setRGB(centroid->getRed(), centroid->getGreen(), centroid->getBlue());
    }
}

void Cluster::clearPixels()
{
    pixelsList = {};
}