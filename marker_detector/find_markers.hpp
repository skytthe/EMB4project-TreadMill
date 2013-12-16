#ifndef find_markers_hpp
#define find_markers_hpp

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include <opencv2/opencv.hpp>

#include "Marker.hpp"

typedef std::vector<cv::Point>    PointsVector;
typedef std::vector<PointsVector> ContoursVector;

	std::vector<Marker> findMarkers(cv::Mat image);

  //! Converts image to grayscale
  void prepareImage(const cv::Mat& img, cv::Mat& grayscale);
  //! Performs binary threshold
  void performThreshold(const cv::Mat& grayscale, cv::Mat& thresholdImg);
  //! Detects appropriate contours
  void findContours(cv::Mat& thresholdImg, ContoursVector& contours, int minContourPointsAllowed);
  //! Finds marker candidates among all contours
  void findCandidates(const ContoursVector& contours, std::vector<Marker>& detectedMarkers);
  //! Tries to recognize markers by detecting marker code 
  void recognizeMarkers(const cv::Mat& grayscale, std::vector<Marker>& detectedMarkers);

float perimeter(const std::vector<cv::Point2f> &a);

#endif
