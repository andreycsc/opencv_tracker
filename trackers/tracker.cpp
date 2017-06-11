// Standard include files
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

using namespace cv;
using namespace std;


int fontFace = FONT_HERSHEY_COMPLEX;
double fontScale = 1;
int thickness = 2;
Point textOrg(10,30);

//BOOSTING, MIL, KCF, TLD, MEDIANFLOW, and GOTURN
#define TRAKING_ALG     "MEDIANFLOW"

int main( int argc, char** argv ){

  // declares all required variables
  Rect2d roi;
  Mat frame;
  // create a tracker object
  Ptr<Tracker> tracker = Tracker::create(TRAKING_ALG);
  // set input video
  VideoCapture cap(0);

  namedWindow("tracker", WINDOW_NORMAL);
  //resizeWindow("tracker", 620, 480);

  // get bounding box
  cap.read(frame);
  resizeWindow("tracker", frame.cols, frame.rows);
  //resize(frame, frame, Size(360, 240), 0, 0, INTER_CUBIC);

  roi=selectROI("tracker",frame,false);
  //quit if ROI was not selected
  if(roi.width==0 || roi.height==0)
    return 0;
  // initialize the tracker
  tracker->init(frame,roi);
  // perform the tracking process
  cout<<"Frame size is "<<frame.cols <<"x"<<frame.rows<<"\n";
  printf("Start the tracking process, press ESC to quit.\n");

  while (cap.read(frame)){
    //resize(frame, frame, Size(360, 240), 0, 0, INTER_CUBIC);
    // update the tracking result
    if(tracker->update(frame,roi) == true){
        // draw the tracked object
        putText(frame, "Traking Object", textOrg, fontFace, fontScale, Scalar(0,255,0), thickness, 8);
        rectangle( frame, roi, Scalar( 255, 0, 0 ), 2, 1 );
    }
    else{
        putText(frame, "Object Lost", textOrg, fontFace, fontScale, Scalar(0,0,255), thickness, 8);
        rectangle( frame, roi, Scalar( 0, 0, 255), 2, 1 );
    }

    // show image with the tracked object
    imshow("tracker",frame);
    //quit on ESC button
    int key = waitKey(1);
    if(key==27){
        cout<< "Program exit\n";
        break;
    }
    else if (key == 'n')
    {
        cap.read(frame);

        roi=selectROI("tracker",frame,false);
          //quit if ROI was not selected
        if(roi.width==0 || roi.height==0)
            return 0;
          // initialize the tracker
        tracker->clear( );
        Ptr<Tracker> trackerNew = Tracker::create(TRAKING_ALG);
        tracker = trackerNew;
        tracker->init(frame,roi);
    }
  }
  return 0;
}
