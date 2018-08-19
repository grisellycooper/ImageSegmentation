#include <vector>

using namespace std;

class Cluster
{
    private:
        Image *image;
        Pixel *centroid;
        vector<Pixel*> pixelsList;

    public:
        Cluster(Image *_image);
        ~Cluster();

        double getDistanceTo(Pixel *pixel);
        double getDistanceTo(int red, int green, int blue);
        void addPixel(Pixel *pixel);        
        double updateCentroid();        
        void updatePixelsList();
        void clearPixels();
};
