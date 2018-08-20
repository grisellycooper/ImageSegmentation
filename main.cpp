#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <omp.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "include/image.h"
#include "include/cluster.h"

using namespace std;
using namespace cv;


int main(int argc, char* argv[] )
{   
    ///Threads for parallel working
    int threadCount = 4;

    /// Time counting
	clock_t start, end;
    double globalTime = 0.0;

    /// Read & Write image  
    string inputImagePath, outputImagePath;

    /// Segmentation
    int clusterCount;
    double threshold = 1;             /// Umbral

    inputImagePath = argv[1];           /// Input path
    clusterCount = atoi(argv[2]);       /// Clusters

    /// Read image
    start = clock();
    Image *image(new Image(inputImagePath));
    end = clock();
    cout<<"Reading file: "<<(end - start)/(double)CLOCKS_PER_SEC <<" seconds."<< endl;
    globalTime += (end - start)/(double)CLOCKS_PER_SEC;

    /// Set the output file path
    string saveImageAs = "../out/" +to_string(clusterCount) +"_" + inputImagePath.substr(9,(inputImagePath.size()));

    vector<Cluster*> clusters;
    for (int i=0; i < clusterCount; i++)
    {
        clusters.push_back(new Cluster(image));        
    }
    
    /// K-means algorithm while centroidChange is less than threshold
    double centroidChange;
    start = clock();
    do
    {
        // Go through each pixel in the image
        int i, closestClusterIndex, imageSize = image->getImageSize(); 
        double distance_pixel, distance_ccCluster;

//#       pragma omp parallel for num_threads(4) default(none) private(i,closestClusterIndex, distance_pixel, distance_ccCluster) shared(image,clusters,imageSize,clusterCount)
        for (i = 0; i < imageSize ; i++)
        {            
            // Compute distance from each pixel to each cluster centroid
            int j;
            closestClusterIndex = 0;             
            distance_ccCluster = clusters[closestClusterIndex]->getDistanceTo(image->getPixel(i));
            for (j = 0; j < clusterCount; j++)
            {               
                distance_pixel = clusters[j]->getDistanceTo(image->getPixel(i));
                if (distance_pixel < distance_ccCluster)
                {
                    closestClusterIndex = j;
                    distance_ccCluster = clusters[closestClusterIndex]->getDistanceTo(image->getPixel(i));
                }
            }

            // Add tag of the nearest cluster to current pixel             
            image->getPixel(i)->setTag(closestClusterIndex);
        }        
        // Compute an average change of the centroids        
        centroidChange = 0;
        for (i = 0; i < clusterCount; i++)
        {            
            centroidChange += clusters[i]->updateCentroid(i);
        }

        centroidChange /= clusterCount;
        //cout << " - Centroid change: " << centroidChange << "\n";
    } while (centroidChange > threshold);    
    end = clock();
    cout<<"Segmentation image: "<<(end - start)/(double)CLOCKS_PER_SEC <<" seconds."<< endl;
    globalTime += (end - start)/(double)CLOCKS_PER_SEC;

    // Update RGB pixels with the cluster centroid RGB
    start = clock();
    for (int i = 0; i < clusterCount; i++)
    {
        clusters[i]->updatePixelsList(i);
    }
    end = clock();
    cout<<"Final pixels updating: "<<(end - start)/(double)CLOCKS_PER_SEC <<" seconds."<< endl;
    globalTime += (end - start)/(double)CLOCKS_PER_SEC;

    // Save the new image
    start = clock();
    image->saveImage(saveImageAs);
    end = clock();
    cout<<"Saving result image: "<<(end - start)/(double)CLOCKS_PER_SEC <<" seconds."<< endl;
    globalTime += (end - start)/(double)CLOCKS_PER_SEC;

    cout<<"TOTAL TTIME TAKEN: "<<globalTime <<" seconds."<< endl;

    //Display image with OpenCV    
    Mat segmentedImage = imread(saveImageAs, cv::IMREAD_COLOR);
    
    if(!segmentedImage.data ) {
        std::cout <<"Something went wrong with result image!" << std::endl ;
        return -1;
    }
  
    namedWindow( "Result image", 0);
    resizeWindow("Result image", 1920, 1080);
    imshow( "Result image", segmentedImage );        
    
    waitKey(0);
    return 0;
}