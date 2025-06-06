/***************************************************************
 * File: red_laser_service.hpp
 * Description: File contains  function definitions related to red laser Service
 *              and also creates a default global "config" to fetch hsv values
 *
 * Refrence -https://github.com/bradmontgomery/python-laser-tracker
 *
 * Author: Nalin Saxena
 * Modified:Lokesh Senthil Kumar, Bhavya Saravanan
 ***************************************************************/
#pragma once
#include "red_laser_service.hpp"
#include <optional>
// default config
#include "watchdog.hpp"

HSVConfig config;
std::mutex config_mutex;

std::optional<Point2D> latest_laser_point;
std::mutex point_mutex;
std::atomic<bool> point_available{false};

void thresholdImage(cv::Mat &channel, int minimum, int maximum)
{
    cv::Mat tmp;

    // First threshold with THRESH_TOZERO_INV (sets values above maximum to 0)
    cv::threshold(channel, tmp, maximum, 0, cv::THRESH_TOZERO_INV);

    // Second threshold with THRESH_BINARY (sets values above minimum to 255)
    cv::threshold(tmp, channel, minimum, 255, cv::THRESH_BINARY);
}

/*
the main idea is to split the image into 3 channels hsv and apply
masking based on values recieved from the config to get hold of the red laser part.
Contours are then applied to get the centroid and stored in a shared data structure
*/
void red_laser_detect()
{

    cv::Mat frame;

    {
        std::lock_guard<std::mutex> lock(frame_mutex);
        if (latest_frame.empty())
            return;
        frame = latest_frame.clone();
    }

    HSVConfig current_config;
    // get latest config in case if its updated
    {
        std::lock_guard<std::mutex> lock(config_mutex);
        current_config = config;
    }
    // most execution overhead due to this
    //  Convert to HSV
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    // Split the image into individual HSV channels
    std::vector<cv::Mat> hsv_channels;
    cv::split(hsv, hsv_channels);

    cv::Mat hue = hsv_channels[0];
    cv::Mat saturation = hsv_channels[1];
    cv::Mat value = hsv_channels[2];

    // Apply thresholding on each channel
    thresholdImage(hue, current_config.hue_min, current_config.hue_max);
    thresholdImage(saturation, current_config.sat_min, current_config.sat_max);
    thresholdImage(value, current_config.val_min, current_config.val_max);

    // Special handling for hue (invert it)
    cv::bitwise_not(hue, hue);

    // Perform an AND on all three channels (hue, saturation, and value)
    cv::Mat mask = hue & saturation & value;
    cv::imshow("Filtered Frame", mask);

    // Morphological operations: Erosion followed by Dilation
    cv::erode(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);
    cv::dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);

    // Find contours in the mask
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Loop through contours to find the largest one (potential laser spot)
    for (const auto &contour : contours)
    {
        if (cv::contourArea(contour) < 5)
            continue; // Ignore small contours

        // Calculate the centroid using image moments
        cv::Moments m = cv::moments(contour);
        if (m.m00 != 0)
        {
            int cx = int(m.m10 / m.m00);
            int cy = int(m.m01 / m.m00);

            // Log the detected laser position (centroid)
            // syslog(LOG_INFO, "Laser detected at x,y: %d, %d %d", cx, cy,current_config.behaviour);

            // Mark the centroid on the frame
            cv::circle(frame, cv::Point(cx, cy), 5, cv::Scalar(0, 255, 0), -1);
            {
                std::lock_guard<std::mutex> lock(point_mutex);
                latest_laser_point = Point2D{cx, cy, current_config.behaviour};
                point_available.store(true, std::memory_order_release);
            }
        }
    }
    service2_ok = true;
    // syslog(LOG_INFO, "Service 2 OK set");

    // do not enable these | only for debugging enable blocking calls

    //    cv::imshow("Red Laser Detection", frame);
    //   cv::waitKey(1);
}
