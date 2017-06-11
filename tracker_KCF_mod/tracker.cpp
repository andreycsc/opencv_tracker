// Standard include files
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

using namespace cv;
using namespace std;


int fontFace = FONT_HERSHEY_COMPLEX;
double fontScale = 1;
int thickness = 2;
Point textOrg(10,30);

void sobelExtractor(const Mat img, const Rect roi, Mat& feat);

TrackerKCF::Params param;

int main( int argc, char** argv ){

  // declares all required variables
  Rect2d roi;
  Mat frame;
  // create a tracker object
  param.desc_pca = TrackerKCF::GRAY | TrackerKCF::CN;
  param.desc_npca = 0;
  param.compress_feature = true;
  param.compressed_size = 2;
  param.resize = 1;

    // create a tracker object
  Ptr<TrackerKCF> tracker = TrackerKCF::createTracker(param);

  tracker->setFeatureExtractor(sobelExtractor);

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
        Ptr<TrackerKCF> trackerNew = TrackerKCF::createTracker(param);
        trackerNew->setFeatureExtractor(sobelExtractor);
        tracker = trackerNew;
        tracker->init(frame,roi);
    }
  }
  return 0;
}

void sobelExtractor(const Mat img, const Rect roi, Mat& feat){
    Mat sobel[2];
    Mat patch;
    Rect region=roi;

    // extract patch inside the image
    if(roi.x<0){region.x=0;region.width+=roi.x;}
    if(roi.y<0){region.y=0;region.height+=roi.y;}
    if(roi.x+roi.width>img.cols)region.width=img.cols-roi.x;
    if(roi.y+roi.height>img.rows)region.height=img.rows-roi.y;
    if(region.width>img.cols)region.width=img.cols;
    if(region.height>img.rows)region.height=img.rows;

    patch=img(region).clone();
    cvtColor(patch,patch, CV_BGR2GRAY);

    // add some padding to compensate when the patch is outside image border
    int addTop,addBottom, addLeft, addRight;
    addTop=region.y-roi.y;
    addBottom=(roi.height+roi.y>img.rows?roi.height+roi.y-img.rows:0);
    addLeft=region.x-roi.x;
    addRight=(roi.width+roi.x>img.cols?roi.width+roi.x-img.cols:0);

    copyMakeBorder(patch,patch,addTop,addBottom,addLeft,addRight,BORDER_REPLICATE);

    Sobel(patch, sobel[0], CV_32F,1,0,1);
    Sobel(patch, sobel[1], CV_32F,0,1,1);

    merge(sobel,2,feat);

    feat.convertTo(feat,CV_64F);
    feat=feat/255.0-0.5; // normalize to range -0.5 .. 0.5
}
