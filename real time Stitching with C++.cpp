#include "MareArtsStitcher.h"
#include "opencv2/opencv.hpp"
#include <vector>
using namespace cv;
using namespace std;


int main(int argc, char** argv)
{
    unsigned long AAtime=0, BBtime=0; 

    VideoCapture cap1, cap2;
    namedWindow("cam1", 0);
    namedWindow("cam2", 0);
    namedWindow("result", WINDOW_AUTOSIZE); //WINDOW_NORMAL);
    if(!cap1.open(0))
        return 0;
    cap1.set(CAP_PROP_FRAME_WIDTH,640);
    cap1.set(CAP_PROP_FRAME_HEIGHT,480);

    if(!cap2.open(2))
        return 0;
    cap2.set(CAP_PROP_FRAME_WIDTH,640);
    cap2.set(CAP_PROP_FRAME_HEIGHT,480);

    MareArtsStitcher stitcher;
    std::string MYSN = "your_sn_number";
	stitcher.m_ImageWarppingWay = "plane"; //"cylindrical";
    stitcher.m_FeatureFindAlgorithm = "akaze";
    if (stitcher.check_SNcertification("my@email.com",MYSN) == false)
	{
		std::cout << "SN certification fail" << std::endl;
		return 0;
	}


    int mode=0;
    while (true) {

        Mat cameraFrame1;
        cap1 >> cameraFrame1; //get one frame form video
        Mat cameraFrame2;
        cap2 >> cameraFrame2; //get one frame form video
    
        //if(mode == 0)
        {
            imshow("cam1", cameraFrame1);
            imshow("cam2", cameraFrame2);
        }

        if(mode == 1)
        {
            printf("panorama parameter processing...\n");
            vector< Mat > vImg_color;
            vImg_color.push_back( cameraFrame1 );
            vImg_color.push_back( cameraFrame2 );
            AAtime = getTickCount();
            if( stitcher.calibrateFrame(vImg_color) == false)
                cout << "stitching fail " << endl << endl;
            else
                mode = 2;
            //Processing time measurement   
            BBtime = getTickCount(); 
            printf("Processing time = %.2lf(sec) \n",  (BBtime - AAtime)/getTickFrequency() );      
        }

        if(mode == 2){
            vector< Mat > vImg_color;
            vImg_color.push_back( cameraFrame1 );
            vImg_color.push_back( cameraFrame2 );
            
            AAtime = getTickCount();
            cv::Mat result;
            if (stitcher.stitcher(vImg_color, result))
                cv::imshow("result", result);
            
            //Processing time measurement   
            BBtime = getTickCount(); 
            printf("Processing time = %.2lf(sec) \n",  (BBtime - AAtime)/getTickFrequency() ); 
        }

        char k = waitKey(10);
            if ( k == 'p') //processing command
            mode=1;
            else if (k == 'r') //reset camera mode
            mode=0;
            else if (k == 'q') //quit
            break;
        }

    destroyAllWindows();
    
    return 0;
}
