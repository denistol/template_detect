#include <map>
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <ApplicationServices/ApplicationServices.h>

#define SCREENSHOT_NAME "screenshot.png"
#define TEMPLATE_NAME "templ.png"

void captureScreen()
{
    CGRect screenBounds = CGDisplayBounds(CGMainDisplayID());

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(NULL, CGRectGetWidth(screenBounds), CGRectGetHeight(screenBounds), 8, 0, CGColorSpaceCreateDeviceRGB(), kCGImageAlphaPremultipliedFirst);
    CGImageRef screenshot = CGDisplayCreateImage(CGMainDisplayID());

    CGImageDestinationRef destination = CGImageDestinationCreateWithURL(CFURLCreateWithFileSystemPath(NULL, CFSTR(SCREENSHOT_NAME), kCFURLPOSIXPathStyle, 0), kUTTypePNG, 1, NULL);
    CGImageDestinationAddImage(destination, screenshot, NULL);
    CGImageDestinationFinalize(destination);

    CGImageRelease(screenshot);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    CFRelease(destination);
}

std::pair<cv::Point, cv::Point> detect(int method = cv::TM_CCOEFF_NORMED)
{
    cv::Mat result;
    cv::Mat img = cv::imread(SCREENSHOT_NAME);
    cv::Mat templ = cv::imread(TEMPLATE_NAME, cv::IMREAD_COLOR);

    // Methods

    // cv::TM_SQDIFF;
    // cv::TM_SQDIFF_NORMED;
    // cv::TM_CCORR;
    // cv::TM_CCORR_NORMED;
    // cv::TM_CCOEFF;
    // cv::TM_CCOEFF_NORMED;

    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create(result_rows, result_cols, CV_32FC1);

    matchTemplate(img, templ, result, method);
    normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    double minVal, maxVal;
    cv::Point minLoc, maxLoc, matchLoc;

    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

    matchLoc = (method == cv::TM_SQDIFF || method == cv::TM_SQDIFF_NORMED) ? minLoc : maxLoc;

    cv::Point from = matchLoc;                                                  // x1 y1
    cv::Point to = cv::Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows); // x2 y2
    auto p = std::make_pair(from, to);

    // Cursor position to detected coords

    int x_center = (from.x + to.x) / 2;
    int y_center = (from.y + to.y) / 2;

    std::cout << x_center << " " << y_center << "\n";

    return p;
}

int main(int argc, char **argv)
{
    captureScreen();
    detect();
    return EXIT_SUCCESS;
}
