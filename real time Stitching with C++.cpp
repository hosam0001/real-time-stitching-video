#include "opencv2/opencv.hpp"
#include <vector>
#include <thread>
using namespace cv;
using namespace std;

void detectObjects(Mat& frame) {
    string modelPath = "yolov5s.onnx";
    Net net = readNet(modelPath);
    Mat blob;
    blobFromImage(frame, blob, 1 / 255.0, Size(640, 640), Scalar(0, 0, 0), true, false);
    net.setInput(blob);
    vector<Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());
    for (size_t i = 0; i < outputs.size(); ++i) {
        float* data = (float*)outputs[i].data;
        for (int j = 0; j < outputs[i].rows; ++j, data += outputs[i].cols) {
            float confidence = data[4];
            if (confidence > 0.5) {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;
                rectangle(frame, Point(left, top), Point(left + width, top + height), Scalar(0, 255, 0), 2);
            }
        }
    }
}

void processFrame(VideoCapture& cap, Mat& frame, bool& running) {
    while (running) {
        Mat temp;
        cap >> temp;
        if (!temp.empty()) {
            detectObjects(temp);
            frame = temp.clone();
        }
    }
}

int main(int argc, char** argv)
{
    VideoCapture cap1, cap2;
    namedWindow("cam1", 0);
    namedWindow("cam2", 0);
    namedWindow("result", WINDOW_AUTOSIZE);

    if (!cap1.open(0)) {
        cout << "Error: Failed to open camera 1." << endl;
        return -1;
    }
    cap1.set(CAP_PROP_FRAME_WIDTH, 320);
    cap1.set(CAP_PROP_FRAME_HEIGHT, 240);

    if (!cap2.open(2)) {
        cout << "Error: Failed to open camera 2." << endl;
        return -1;
    }
    cap2.set(CAP_PROP_FRAME_WIDTH, 320);
    cap2.set(CAP_PROP_FRAME_HEIGHT, 240);

    Ptr<Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA);

    Mat frame1, frame2;
    bool running = true;

    thread t1(processFrame, ref(cap1), ref(frame1), ref(running));
    thread t2(processFrame, ref(cap2), ref(frame2), ref(running));

    int mode = 0;
    while (true) {
        if (!frame1.empty()) imshow("cam1", frame1);
        if (!frame2.empty()) imshow("cam2", frame2);

        if (mode == 1) {
            printf("Panorama parameter processing...\n");
            vector<Mat> vImg_color;
            vImg_color.push_back(frame1);
            vImg_color.push_back(frame2);

            Mat result;
            Stitcher::Status status = stitcher->stitch(vImg_color, result);

            if (status != Stitcher::OK) {
                cout << "Stitching failed! Error code: " << int(status) << endl;
            } else {
                mode = 2;
            }
        }

        if (mode == 2) {
            vector<Mat> vImg_color;
            vImg_color.push_back(frame1);
            vImg_color.push_back(frame2);

            Mat result;
            Stitcher::Status status = stitcher->stitch(vImg_color, result);

            if (status == Stitcher::OK) {
                imshow("result", result);
            } else {
                cout << "Stitching failed! Error code: " << int(status) << endl;
            }
        }

        char k = waitKey(10);
        if (k == 'p') {
            mode = 1;
        } else if (k == 'r') {
            mode = 0;
        } else if (k == 'q') {
            running = false;
            break;
        }
    }

    t1.join();
    t2.join();

    destroyAllWindows();
    return 0;
}
