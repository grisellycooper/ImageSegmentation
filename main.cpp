#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "include/image.h"
#include "include/cluster.h"

using namespace std;
using namespace cv;

int main(int argc, char* argv[] )
{     
    string inputImagePath, outputImagePath;
    int clusterCount;
    double threshold = 1.5;             /// Umbral

    inputImagePath = argv[1];           /// Input path
    clusterCount = atoi(argv[2]);       /// Clusters

    Image *image(new Image(inputImagePath));
    
    string saveImageAs = "../out/" +to_string(clusterCount) +"_" + inputImagePath.substr(9,(inputImagePath.size()));

    vector<Cluster*> clusters;
    for (int i=0; i < clusterCount; i++)
    {
        clusters.push_back(new Cluster(image));        
    }

    /// K-means algorithm while centroidChange is less than threshold
    double centroidChange;
    do
    {
        // Clear all clusters' pixelsList
        for (int i = 0; i < clusterCount; i++)
        {
            clusters[i]->clearPixels();
        }

        // Go through each pixel in the image
        int imageSize = image->getImageSize();
        for (int i = 0; i < imageSize ; i++)
        {
            // Compute distance from each pixel to each cluster centroid
            int closestClusterIndex = 0;
            double distance;
            for (int j = 0; j < clusterCount; j++)
            {
                distance = clusters[j]->getDistanceTo(image->getPixel(i));
                if (distance < clusters[closestClusterIndex]->getDistanceTo(image->getPixel(i)))
                {
                    closestClusterIndex = j;
                }
            }

            // Add the pixel to the nearest cluster
            clusters[closestClusterIndex]->addPixel(image->getPixel(i));
        }

        // Compute an average change of the centroids
        centroidChange = 0;
        for (int i = 0; i < clusterCount; i++)
        {
            centroidChange += clusters[i]->updateCentroid();
        }
        centroidChange /= clusterCount;
        cout << "Centroid change: " << centroidChange << "\n";
    } while (centroidChange > threshold);

    // Update RGB pixels with the cluster centroid RGB
    for (int i = 0; i < clusterCount; i++)
    {
        clusters[i]->updatePixelsList();
    }

    // Save the new image
    image->saveImage(saveImageAs);

    //Display image with OpenCV    
    Mat segmentedImage = imread(saveImageAs, cv::IMREAD_COLOR);
    
    if(!segmentedImage.data ) {
        std::cout <<"Something went wrong with result image!" << std::endl ;
        return -1;
    }
  
    namedWindow( "Result image", cv::WINDOW_AUTOSIZE );
    imshow( "Result image", segmentedImage );        
    
    waitKey(0);       
    return 0;
}