aqax#include &lt;opencv2/core/core.hpp&gt;
#include &lt;opencv2/imgproc/imgproc.hpp&gt;
#include &lt;opencv2/calib3d/calib3d.hpp&gt;
#include &lt;opencv2/highgui/highgui.hpp&gt;
 
#include &lt;cctype&gt;
#include &lt;stdio.h&gt;
#include &lt;string.h&gt;
#include &lt;time.h&gt;
 
using namespace cv;
using namespace std;
 
const char * usage =
" \nexample command line for calibration from a live feed.\n"
"   calibration  -w 4 -h 5 -s 0.025 -o camera.yml -op -oe\n"
" \n"
" example command line for calibration from a list of stored images:\n"
"   imagelist_creator image_list.xml *.png\n"
"   calibration -w 4 -h 5 -s 0.025 -o camera.yml -op -oe image_list.xml\n"
" where image_list.xml is the standard OpenCV XML/YAML\n"
" use imagelist_creator to create the xml or yaml list\n"
" file consisting of the list of strings, e.g.:\n"
" \n"
"&lt;?xml version=\"1.0\"?&gt;\n"
"&lt;opencv_storage&gt;\n"
"&lt;images&gt;\n"
"view000.png\n"
"view001.png\n"
"&lt;!-- view002.png --&gt;\n"
"view003.png\n"
"view010.png\n"
"one_extra_view.jpg\n"
"&lt;/images&gt;\n"
"&lt;/opencv_storage&gt;\n";
 
const char* liveCaptureHelp =
"When the live video from camera is used as input, the following hot-keys may be used:\n"
"  &lt;ESC&gt;, 'q' - quit the program\n"
"  'g' - start capturing images\n"
"  'u' - switch undistortion on/off\n";
 
static void help()
{
printf( "This is a camera calibration sample.\n"
"Usage: calibration\n"
"     -w &lt;board_width&gt;         # the number of inner corners per one of board dimension\n"
"     -h &lt;board_height&gt;        # the number of inner corners per another board dimension\n"
"     [-pt &lt;pattern&gt;]          # the type of pattern: chessboard or circles' grid\n"
"     [-n &lt;number_of_frames&gt;]  # the number of frames to use for calibration\n"
"                              # (if not specified, it will be set to the number\n"
"                              #  of board views actually available)\n"
"     [-d &lt;delay&gt;]             # a minimum delay in ms between subsequent attempts to capture a next view\n"
"                              # (used only for video capturing)\n"
"     [-s &lt;squareSize&gt;]       # square size in some user-defined units (1 by default)\n"
"     [-o &lt;out_camera_params&gt;] # the output filename for intrinsic [and extrinsic] parameters\n"
"     [-op]                    # write detected feature points\n"
"     [-oe]                    # write extrinsic parameters\n"
"     [-zt]                    # assume zero tangential distortion\n"
"     [-a &lt;aspectRatio&gt;]      # fix aspect ratio (fx/fy)\n"
"     [-p]                     # fix the principal point at the center\n"
"     [-v]                     # flip the captured images around the horizontal axis\n"
"     [-V]                     # use a video file, and not an image list, uses\n"
"                              # [input_data] string for the video file name\n"
"     [-su]                    # show undistorted images after calibration\n"
"     [input_data]             # input data, one of the following:\n"
"                              #  - text file with a list of the images of the board\n"
"                              #    the text file can be generated with imagelist_creator\n"
"                              #  - name of video file with a video of the board\n"
"                              # if input_data not specified, a live view from the camera is used\n"
"\n" );
printf("\n%s",usage);
printf( "\n%s", liveCaptureHelp );
}
 
enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };
enum Pattern { CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
 
static double computeReprojectionErrors(
const vector&lt;vector&lt;Point3f&gt; &gt;&amp; objectPoints,
const vector&lt;vector&lt;Point2f&gt; &gt;&amp; imagePoints,
const vector&lt;Mat&gt;&amp; rvecs, const vector&lt;Mat&gt;&amp; tvecs,
const Mat&amp; cameraMatrix, const Mat&amp; distCoeffs,
vector&lt;float&gt;&amp; perViewErrors )
{
vector&lt;Point2f&gt; imagePoints2;
int i, totalPoints = 0;
double totalErr = 0, err;
perViewErrors.resize(objectPoints.size());
 
for( i = 0; i &lt; (int)objectPoints.size(); i++ )
{
projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i],
cameraMatrix, distCoeffs, imagePoints2);
err = norm(Mat(imagePoints[i]), Mat(imagePoints2), CV_L2);
int n = (int)objectPoints[i].size();
perViewErrors[i] = (float)std::sqrt(err*err/n);
totalErr += err*err;
totalPoints += n;
}
 
return std::sqrt(totalErr/totalPoints);
}
 
static void calcChessboardCorners(Size boardSize, float squareSize, vector&lt;Point3f&gt;&amp; corners, Pattern patternType = CHESSBOARD)
{
corners.resize(0);
 
switch(patternType)
{
case CHESSBOARD:
case CIRCLES_GRID:
for( int i = 0; i &lt; boardSize.height; i++ )
for( int j = 0; j &lt; boardSize.width; j++ )
corners.push_back(Point3f(float(j*squareSize),
float(i*squareSize), 0));
break;
 
case ASYMMETRIC_CIRCLES_GRID:
for( int i = 0; i &lt; boardSize.height; i++ )
for( int j = 0; j &lt; boardSize.width; j++ )
corners.push_back(Point3f(float((2*j + i % 2)*squareSize),
float(i*squareSize), 0));
break;
 
default:
CV_Error(CV_StsBadArg, "Unknown pattern type\n");
}
}
 
static bool runCalibration( vector&lt;vector&lt;Point2f&gt; &gt; imagePoints,
Size imageSize, Size boardSize, Pattern patternType,
float squareSize, float aspectRatio,
int flags, Mat&amp; cameraMatrix, Mat&amp; distCoeffs,
vector&lt;Mat&gt;&amp; rvecs, vector&lt;Mat&gt;&amp; tvecs,
vector&lt;float&gt;&amp; reprojErrs,
double&amp; totalAvgErr)
{
cameraMatrix = Mat::eye(3, 3, CV_64F);
if( flags &amp; CV_CALIB_FIX_ASPECT_RATIO )
cameraMatrix.at&lt;double&gt;(0,0) = aspectRatio;
 
distCoeffs = Mat::zeros(8, 1, CV_64F);
 
vector&lt;vector&lt;Point3f&gt; &gt; objectPoints(1);
calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);
 
objectPoints.resize(imagePoints.size(),objectPoints[0]);
 
double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
distCoeffs, rvecs, tvecs, flags|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
///*|CV_CALIB_FIX_K3*/|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
printf("RMS error reported by calibrateCamera: %g\n", rms);
 
bool ok = checkRange(cameraMatrix) &amp;&amp; checkRange(distCoeffs);
 
totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);
 
return ok;
}
 
static void saveCameraParams( const string&amp; filename,
Size imageSize, Size boardSize,
float squareSize, float aspectRatio, int flags,
const Mat&amp; cameraMatrix, const Mat&amp; distCoeffs,
const vector&lt;Mat&gt;&amp; rvecs, const vector&lt;Mat&gt;&amp; tvecs,
const vector&lt;float&gt;&amp; reprojErrs,
const vector&lt;vector&lt;Point2f&gt; &gt;&amp; imagePoints,
double totalAvgErr )
{
FileStorage fs( filename, FileStorage::WRITE );
 
time_t tt;
time( &amp;tt );
struct tm *t2 = localtime( &amp;tt );
char buf[1024];
strftime( buf, sizeof(buf)-1, "%c", t2 );
 
fs &lt;&lt; "calibration_time" &lt;&lt; buf;
 
if( !rvecs.empty() || !reprojErrs.empty() )
fs &lt;&lt; "nframes" &lt;&lt; (int)std::max(rvecs.size(), reprojErrs.size());
fs &lt;&lt; "image_width" &lt;&lt; imageSize.width;
fs &lt;&lt; "image_height" &lt;&lt; imageSize.height;
fs &lt;&lt; "board_width" &lt;&lt; boardSize.width;
fs &lt;&lt; "board_height" &lt;&lt; boardSize.height;
fs &lt;&lt; "square_size" &lt;&lt; squareSize;
 
if( flags &amp; CV_CALIB_FIX_ASPECT_RATIO )
fs &lt;&lt; "aspectRatio" &lt;&lt; aspectRatio;
 
if( flags != 0 )
{
sprintf( buf, "flags: %s%s%s%s",
flags &amp; CV_CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
flags &amp; CV_CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
flags &amp; CV_CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
flags &amp; CV_CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "" );
cvWriteComment( *fs, buf, 0 );
}
 
fs &lt;&lt; "flags" &lt;&lt; flags;
 
fs &lt;&lt; "camera_matrix" &lt;&lt; cameraMatrix;
fs &lt;&lt; "distortion_coefficients" &lt;&lt; distCoeffs;
 
fs &lt;&lt; "avg_reprojection_error" &lt;&lt; totalAvgErr;
if( !reprojErrs.empty() )
fs &lt;&lt; "per_view_reprojection_errors" &lt;&lt; Mat(reprojErrs);
 
if( !rvecs.empty() &amp;&amp; !tvecs.empty() )
{
CV_Assert(rvecs[0].type() == tvecs[0].type());
Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
for( int i = 0; i &lt; (int)rvecs.size(); i++ )
{
Mat r = bigmat(Range(i, i+1), Range(0,3));
Mat t = bigmat(Range(i, i+1), Range(3,6));
 
CV_Assert(rvecs[i].rows == 3 &amp;&amp; rvecs[i].cols == 1);
CV_Assert(tvecs[i].rows == 3 &amp;&amp; tvecs[i].cols == 1);
//*.t() is MatExpr (not Mat) so we can use assignment operator
r = rvecs[i].t();
t = tvecs[i].t();
}
cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
fs &lt;&lt; "extrinsic_parameters" &lt;&lt; bigmat;
 
}
//fs &lt;&lt; "rvecs" &lt;&lt; rvecs;
//fs &lt;&lt; "tvecs" &lt;&lt; tvecs;
if( !imagePoints.empty() )
{
Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
for( int i = 0; i &lt; (int)imagePoints.size(); i++ )
{
Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
Mat imgpti(imagePoints[i]);
imgpti.copyTo(r);
}
fs &lt;&lt; "image_points" &lt;&lt; imagePtMat;
}
}
 
static bool readStringList( const string&amp; filename, vector&lt;string&gt;&amp; l )
{
l.resize(0);
FileStorage fs(filename, FileStorage::READ);
if( !fs.isOpened() )
return false;
FileNode n = fs.getFirstTopLevelNode();
if( n.type() != FileNode::SEQ )
return false;
FileNodeIterator it = n.begin(), it_end = n.end();
for( ; it != it_end; ++it )
l.push_back((string)*it);
return true;
}
 
static bool runAndSave(const string&amp; outputFilename,
const vector&lt;vector&lt;Point2f&gt; &gt;&amp; imagePoints,
Size imageSize, Size boardSize, Pattern patternType, float squareSize,
float aspectRatio, int flags, Mat&amp; cameraMatrix,
Mat&amp; distCoeffs, bool writeExtrinsics, bool writePoints )
{
vector&lt;Mat&gt; rvecs, tvecs;
vector&lt;float&gt; reprojErrs;
double totalAvgErr = 0;
 
bool ok = runCalibration(imagePoints, imageSize, boardSize, patternType, squareSize,
aspectRatio, flags, cameraMatrix, distCoeffs,
rvecs, tvecs, reprojErrs, totalAvgErr);
printf("%s. avg reprojection error = %.2f\n",
ok ? "Calibration succeeded" : "Calibration failed",
totalAvgErr);
 
if( ok )
saveCameraParams( outputFilename, imageSize,
boardSize, squareSize, aspectRatio,
flags, cameraMatrix, distCoeffs,
writeExtrinsics ? rvecs : vector&lt;Mat&gt;(),
writeExtrinsics ? tvecs : vector&lt;Mat&gt;(),
writeExtrinsics ? reprojErrs : vector&lt;float&gt;(),
writePoints ? imagePoints : vector&lt;vector&lt;Point2f&gt; &gt;(),
totalAvgErr );
return ok;
}
 
int main( int argc, char** argv )
{
Size boardSize, imageSize;
float squareSize = 1.f, aspectRatio = 1.f;
Mat cameraMatrix, distCoeffs;
const char* outputFilename = "out_camera_data.yml";
const char* inputFilename = 0;
 
int i, nframes = 10;
bool writeExtrinsics = false, writePoints = false;
bool undistortImage = false;
int flags = 0;
VideoCapture capture;
bool flipVertical = false;
bool showUndistorted = false;
bool videofile = false;
int delay = 1000;
clock_t prevTimestamp = 0;
int mode = DETECTION;
int cameraId = 0;
vector&lt;vector&lt;Point2f&gt; &gt; imagePoints;
vector&lt;string&gt; imageList;
Pattern pattern = CHESSBOARD;
 
if( argc &lt; 2 )
{
help();
return 0;
}
 
for( i = 1; i &lt; argc; i++ )
{
const char* s = argv[i];
if( strcmp( s, "-w" ) == 0 )
{
if( sscanf( argv[++i], "%u", &amp;boardSize.width ) != 1 || boardSize.width &lt;= 0 )
return fprintf( stderr, "Invalid board width\n" ), -1;
}
else if( strcmp( s, "-h" ) == 0 )
{
if( sscanf( argv[++i], "%u", &amp;boardSize.height ) != 1 || boardSize.height &lt;= 0 )
return fprintf( stderr, "Invalid board height\n" ), -1;
}
else if( strcmp( s, "-pt" ) == 0 )
{
i++;
if( !strcmp( argv[i], "circles" ) )
pattern = CIRCLES_GRID;
else if( !strcmp( argv[i], "acircles" ) )
pattern = ASYMMETRIC_CIRCLES_GRID;
else if( !strcmp( argv[i], "chessboard" ) )
pattern = CHESSBOARD;
else
return fprintf( stderr, "Invalid pattern type: must be chessboard or circles\n" ), -1;
}
else if( strcmp( s, "-s" ) == 0 )
{
if( sscanf( argv[++i], "%f", &amp;squareSize ) != 1 || squareSize &lt;= 0 )
return fprintf( stderr, "Invalid board square width\n" ), -1;
}
else if( strcmp( s, "-n" ) == 0 )
{
if( sscanf( argv[++i], "%u", &amp;nframes ) != 1 || nframes &lt;= 3 )
return printf("Invalid number of images\n" ), -1;
}
else if( strcmp( s, "-a" ) == 0 )
{
if( sscanf( argv[++i], "%f", &amp;aspectRatio ) != 1 || aspectRatio &lt;= 0 )
return printf("Invalid aspect ratio\n" ), -1;
flags |= CV_CALIB_FIX_ASPECT_RATIO;
}
else if( strcmp( s, "-d" ) == 0 )
{
if( sscanf( argv[++i], "%u", &amp;delay ) != 1 || delay &lt;= 0 )
return printf("Invalid delay\n" ), -1;
}
else if( strcmp( s, "-op" ) == 0 )
{
writePoints = true;
}
else if( strcmp( s, "-oe" ) == 0 )
{
writeExtrinsics = true;
}
else if( strcmp( s, "-zt" ) == 0 )
{
flags |= CV_CALIB_ZERO_TANGENT_DIST;
}
else if( strcmp( s, "-p" ) == 0 )
{
flags |= CV_CALIB_FIX_PRINCIPAL_POINT;
}
else if( strcmp( s, "-v" ) == 0 )
{
flipVertical = true;
}
else if( strcmp( s, "-V" ) == 0 )
{
videofile = true;
}
else if( strcmp( s, "-o" ) == 0 )
{
outputFilename = argv[++i];
}
else if( strcmp( s, "-su" ) == 0 )
{
showUndistorted = true;
}
else if( s[0] != '-' )
{
if( isdigit(s[0]) )
sscanf(s, "%d", &amp;cameraId);
else
inputFilename = s;
}
else
return fprintf( stderr, "Unknown option %s", s ), -1;
}
 
if( inputFilename )
{
if( !videofile &amp;&amp; readStringList(inputFilename, imageList) )
mode = CAPTURING;
else
capture.open(inputFilename);
}
else
capture.open(cameraId);
 
if( !capture.isOpened() &amp;&amp; imageList.empty() )
return fprintf( stderr, "Could not initialize video (%d) capture\n",cameraId ), -2;
 
if( !imageList.empty() )
nframes = (int)imageList.size();
 
if( capture.isOpened() )
printf( "%s", liveCaptureHelp );
 
namedWindow( "Image View", 1 );
 
for(i = 0;;i++)
{
Mat view, viewGray;
bool blink = false;
 
if( capture.isOpened() )
{
Mat view0;
capture &gt;&gt; view0;
view0.copyTo(view);
}
else if( i &lt; (int)imageList.size() )
view = imread(imageList[i], 1);
 
if(!view.data)
{
if( imagePoints.size() &gt; 0 )
runAndSave(outputFilename, imagePoints, imageSize,
boardSize, pattern, squareSize, aspectRatio,
flags, cameraMatrix, distCoeffs,
writeExtrinsics, writePoints);
break;
}
 
imageSize = view.size();
 
if( flipVertical )
flip( view, view, 0 );
 
vector&lt;Point2f&gt; pointbuf;
cvtColor(view, viewGray, CV_BGR2GRAY);
 
bool found;
switch( pattern )
{
case CHESSBOARD:
found = findChessboardCorners( view, boardSize, pointbuf,
CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
break;
case CIRCLES_GRID:
found = findCirclesGrid( view, boardSize, pointbuf );
break;
case ASYMMETRIC_CIRCLES_GRID:
found = findCirclesGrid( view, boardSize, pointbuf, CALIB_CB_ASYMMETRIC_GRID );
break;
default:
return fprintf( stderr, "Unknown pattern type\n" ), -1;
}
 
// improve the found corners' coordinate accuracy
if( pattern == CHESSBOARD &amp;&amp; found) cornerSubPix( viewGray, pointbuf, Size(11,11),
Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
 
if( mode == CAPTURING &amp;&amp; found &amp;&amp;
(!capture.isOpened() || clock() - prevTimestamp &gt; delay*1e-3*CLOCKS_PER_SEC) )
{
imagePoints.push_back(pointbuf);
prevTimestamp = clock();
blink = capture.isOpened();
}
 
if(found)
drawChessboardCorners( view, boardSize, Mat(pointbuf), found );
 
string msg = mode == CAPTURING ? "100/100" :
mode == CALIBRATED ? "Calibrated" : "Press 'g' to start";
int baseLine = 0;
Size textSize = getTextSize(msg, 1, 1, 1, &amp;baseLine);
Point textOrigin(view.cols - 2*textSize.width - 10, view.rows - 2*baseLine - 10);
 
if( mode == CAPTURING )
{
if(undistortImage)
msg = format( "%d/%d Undist", (int)imagePoints.size(), nframes );
else
msg = format( "%d/%d", (int)imagePoints.size(), nframes );
}
 
putText( view, msg, textOrigin, 1, 1,
mode != CALIBRATED ? Scalar(0,0,255) : Scalar(0,255,0));
 
if( blink )
bitwise_not(view, view);
 
if( mode == CALIBRATED &amp;&amp; undistortImage )
{
Mat temp = view.clone();
undistort(temp, view, cameraMatrix, distCoeffs);
}
 
imshow("Image View", view);
int key = 0xff &amp; waitKey(capture.isOpened() ? 50 : 500);
 
if( (key &amp; 255) == 27 )
break;
 
if( key == 'u' &amp;&amp; mode == CALIBRATED )
undistortImage = !undistortImage;
 
if( capture.isOpened() &amp;&amp; key == 'g' )
{
mode = CAPTURING;
imagePoints.clear();
}
 
if( mode == CAPTURING &amp;&amp; imagePoints.size() &gt;= (unsigned)nframes )
{
if( runAndSave(outputFilename, imagePoints, imageSize,
boardSize, pattern, squareSize, aspectRatio,
flags, cameraMatrix, distCoeffs,
writeExtrinsics, writePoints))
mode = CALIBRATED;
else
mode = DETECTION;
if( !capture.isOpened() )
break;
}
}
 
if( !capture.isOpened() &amp;&amp; showUndistorted )
{
Mat view, rview, map1, map2;
initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),
getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0),
imageSize, CV_16SC2, map1, map2);
 
for( i = 0; i &lt; (int)imageList.size(); i++ )
{
view = imread(imageList[i], 1);
if(!view.data)
continue;
//undistort( view, rview, cameraMatrix, distCoeffs, cameraMatrix );
remap(view, rview, map1, map2, INTER_LINEAR);
imshow("Image View", rview);
int c = 0xff &amp; waitKey();
if( (c &amp; 255) == 27 || c == 'q' || c == 'Q' )
break;
}
}
 
return 0;
}