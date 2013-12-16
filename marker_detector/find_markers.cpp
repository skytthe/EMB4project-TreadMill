#include "find_markers.hpp"


using namespace std;
using namespace cv;

typedef std::vector<cv::Point>    PointsVector;
typedef std::vector<PointsVector> ContoursVector;


  float m_minContourLengthAllowed;

  Size markerSize(100,100);
  cv::Mat camMatrix;
  cv::Mat distCoeff;

 
  cv::Mat m_grayscaleImage;
  cv::Mat m_thresholdImg;  
  cv::Mat m_blur;
  cv::Mat canonicalMarkerImage;

  cv::Mat displayImage;

  ContoursVector           m_contours;
  std::vector<cv::Point3f> m_markerCorners3d;

std::vector<Marker> findMarkers(cv::Mat image)
{

  std::vector<Marker> detectedMarkers;

//convert to grayscale
prepareImage(image,m_grayscaleImage);
	
//convert to black&white
performThreshold(m_grayscaleImage,m_thresholdImg);

// Detect contours
findContours(m_thresholdImg, m_contours, m_grayscaleImage.cols/15);

// Find closed contours that can be approximated with 4 points
findCandidates(m_contours, detectedMarkers);
		
// Find is them are markers
recognizeMarkers(m_grayscaleImage, detectedMarkers);

  return detectedMarkers;
}


void prepareImage(const Mat& img, Mat& grayscale)
{
    // Convert to grayscale
    cvtColor(img, grayscale, CV_RGB2GRAY);
}

void performThreshold(const cv::Mat& grayscale, cv::Mat& thresholdImg)
{
 //   cv::threshold(grayscale, thresholdImg, 127, 255, cv::THRESH_BINARY_INV);

//    cv::GaussianBlur(grayscale,m_blur,cv::Size(5,5),1.5);
    cv::adaptiveThreshold(grayscale,   // Input image
    thresholdImg,// Result binary image
    255,         // 
    cv::ADAPTIVE_THRESH_GAUSSIAN_C, //
    cv::THRESH_BINARY_INV, //
    75, //
    10  //
    );
 
}

void findContours(cv::Mat& thresholdImg, ContoursVector& contours, int minContourPointsAllowed)
{
    ContoursVector allContours;
    cv::findContours(thresholdImg, allContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    contours.clear();
    for (size_t i=0; i<allContours.size(); i++)
    {
        int contourSize = allContours[i].size();
        if (contourSize > minContourPointsAllowed)
        {
            contours.push_back(allContours[i]);
        }
    }

        cv::Mat contoursImage(thresholdImg.size(), CV_8UC1);
        contoursImage = cv::Scalar(0);
        cv::drawContours(contoursImage, contours, -1, cv::Scalar(255), 2, CV_AA);
}


void findCandidates
(
    const ContoursVector& contours, 
    std::vector<Marker>& detectedMarkers
) 
{
    std::vector<cv::Point>  approxCurve;
    std::vector<Marker>     possibleMarkers;

    // For each contour, analyze if it is a parallelepiped likely to be the marker
    for (size_t i=0; i<contours.size(); i++)
    {
        // Approximate to a polygon
        double eps = contours[i].size() * 0.05;
        cv::approxPolyDP(contours[i], approxCurve, eps, true);

        // We interested only in polygons that contains only four points
        if (approxCurve.size() != 4)
            continue;

        // And they have to be convex
        if (!cv::isContourConvex(approxCurve))
            continue;

        // Ensure that the distance between consecutive points is large enough
        float minDist = 0.0;//std::numeric_limits<float>::max();

        for (int i = 0; i < 4; i++)
        {
            cv::Point side = approxCurve[i] - approxCurve[(i+1)%4];            
            float squaredSideLength = side.dot(side);
            minDist = std::min(minDist, squaredSideLength);
        }

        // Check that distance is not very small
        if (minDist < m_minContourLengthAllowed)
            continue;

        // All tests are passed. Save marker candidate:
        Marker m;

        for (int i = 0; i<4; i++)
            m.points.push_back( cv::Point2f(approxCurve[i].x,approxCurve[i].y) );

        // Sort the points in anti-clockwise order
        // Trace a line between the first and second point.
        // If the third point is at the right side, then the points are anti-clockwise
        cv::Point v1 = m.points[1] - m.points[0];
        cv::Point v2 = m.points[2] - m.points[0];

        double o = (v1.x * v2.y) - (v1.y * v2.x);

        if (o < 0.0)		 //if the third point is in the left side, then sort in anti-clockwise order
            std::swap(m.points[1], m.points[3]);

        possibleMarkers.push_back(m);
    }


    // Remove these elements which corners are too close to each other.  
    // First detect candidates for removal:
    std::vector< std::pair<int,int> > tooNearCandidates;
    for (size_t i=0;i<possibleMarkers.size();i++)
    { 
        const Marker& m1 = possibleMarkers[i];

        //calculate the average distance of each corner to the nearest corner of the other marker candidate
        for (size_t j=i+1;j<possibleMarkers.size();j++)
        {
            const Marker& m2 = possibleMarkers[j];

            float distSquared = 0;

            for (int c = 0; c < 4; c++)
            {
                cv::Point v = m1.points[c] - m2.points[c];
                distSquared += v.dot(v);
            }

            distSquared /= 4;

            if (distSquared < 100)
            {
                tooNearCandidates.push_back(std::pair<int,int>(i,j));
            }
        }				
    }

    // Mark for removal the element of the pair with smaller perimeter
    std::vector<bool> removalMask (possibleMarkers.size(), false);

    for (size_t i=0; i<tooNearCandidates.size(); i++)
    {
        float p1 = perimeter(possibleMarkers[tooNearCandidates[i].first ].points);
        float p2 = perimeter(possibleMarkers[tooNearCandidates[i].second].points);

        size_t removalIndex;
        if (p1 > p2)
            removalIndex = tooNearCandidates[i].second;
        else
            removalIndex = tooNearCandidates[i].first;

        removalMask[removalIndex] = true;
    }

    // Return candidates
    detectedMarkers.clear();
    for (size_t i=0;i<possibleMarkers.size();i++)
    {
        if (!removalMask[i])
            detectedMarkers.push_back(possibleMarkers[i]);
    }

//	cout << "size found : " << detectedMarkers.size() << endl;
}

void recognizeMarkers(const cv::Mat& grayscale, std::vector<Marker>& detectedMarkers)
{
    std::vector<Marker> goodMarkers;

    // Identify the markers
    for (size_t i=0;i<detectedMarkers.size();i++)
    {
        Marker& marker = detectedMarkers[i];

std::vector<cv::Point2f> m_markerCorners2d;
    m_markerCorners2d.push_back(cv::Point2f(0,0));
    m_markerCorners2d.push_back(cv::Point2f(markerSize.width-1,0));
    m_markerCorners2d.push_back(cv::Point2f(markerSize.width-1,markerSize.height-1));
    m_markerCorners2d.push_back(cv::Point2f(0,markerSize.height-1));

        // Find the perspective transformation that brings current marker to rectangular form
        Mat markerTransform = cv::getPerspectiveTransform(marker.points, m_markerCorners2d);

        // Transform image to get a canonical marker image
        warpPerspective(grayscale, canonicalMarkerImage,  markerTransform, markerSize);
/*
#ifdef SHOW_DEBUG_IMAGES
        {
            cv::Mat markerImage = grayscale.clone();
            marker.drawContour(markerImage);
            cv::Mat markerSubImage = markerImage(cv::boundingRect(marker.points));

            cv::showAndSave("Source marker" + ToString(i),           markerSubImage);
            cv::showAndSave("Marker " + ToString(i) + " after warp", canonicalMarkerImage);
        }
#endif
*/
        int nRotations;
        int id = Marker::getMarkerId(canonicalMarkerImage, nRotations);
        if (id !=- 1)
        {
            marker.id = id;
            //sort the points so that they are always in the same order no matter the camera orientation
            std::rotate(marker.points.begin(), marker.points.begin() + 4 - nRotations, marker.points.end());

            goodMarkers.push_back(marker);
        }
    }  

    // Refine marker corners using sub pixel accuracy
    if (goodMarkers.size() > 0)
    {
        std::vector<cv::Point2f> preciseCorners(4 * goodMarkers.size());

        for (size_t i=0; i<goodMarkers.size(); i++)
        {  
            const Marker& marker = goodMarkers[i];      

            for (int c = 0; c <4; c++)
            {
                preciseCorners[i*4 + c] = marker.points[c];
            }
        }

        cv::TermCriteria termCriteria = cv::TermCriteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 30, 0.01);
        cv::cornerSubPix(grayscale, preciseCorners, cvSize(5,5), cvSize(-1,-1), termCriteria);

        // Copy refined corners position back to markers
        for (size_t i=0; i<goodMarkers.size(); i++)
        {
            Marker& marker = goodMarkers[i];      

            for (int c=0;c<4;c++) 
            {
                marker.points[c] = preciseCorners[i*4 + c];
            }      
        }
    }

        cv::Mat markerCornersMat(grayscale.size(), grayscale.type());
        markerCornersMat = cv::Scalar(0);

        for (size_t i=0; i<goodMarkers.size(); i++)
        {
            goodMarkers[i].drawContour(markerCornersMat, cv::Scalar(255));    
        }


    detectedMarkers = goodMarkers;
}

float perimeter(const std::vector<cv::Point2f> &a)
{
  float sum=0, dx, dy;
  
  for (size_t i=0;i<a.size();i++)
  {
    size_t i2=(i+1) % a.size();
    
    dx = a[i].x - a[i2].x;
    dy = a[i].y - a[i2].y;
    
    sum += sqrt(dx*dx + dy*dy);
  }
  
  return sum;
}





