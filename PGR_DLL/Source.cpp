#include "Source.h"
#include "DebugLogWrapper.h"

DLLExport void* getPGR(int device)
{

	//-1だったら再生モード
	if(device != -1)
	{
		//普通にPGRから取ってくるとき
		return static_cast<void*>(new TPGROpenCV(0));
	}
	else
	{
		//録画再生用	
		return static_cast<void*>(new cv::VideoCapture("capture.avi"));
	}
}

DLLExport void initPGR(void* pgr, int device)
{
	auto pgrOpenCV = static_cast<TPGROpenCV*>(pgr);

	//モノクロ1chで取ってくる
	pgrOpenCV->init(FlyCapture2::PIXEL_FORMAT_MONO8, FlyCapture2::NEAREST_NEIGHBOR);
	pgrOpenCV->start();

	if(device != -1)
	{
		//録画用
		videoWriter = cv::VideoWriter("capture.avi", CV_FOURCC('X','V','I','D'), pgrOpenCV->getFramerate(), cv::Size(PGRWIDTH, PGRHEIGHT), false); //isColorで3ch or 1ch
	}
}

DLLExport void releasePGR(void* pgr, int device)
{
    // ウィンドウを閉じる
	if(device != -1) videoWriter.release();
	auto pgrOpenCV = static_cast<TPGROpenCV*>(pgr);
	cv::destroyAllWindows();
	pgrOpenCV->stop();
	pgrOpenCV->release();
}

DLLExport void getPGRTexture(void* pgr, unsigned char* data, bool isRecord, bool isShowWin)
{
	auto pgrOpenCV = static_cast<TPGROpenCV*>(pgr);

	if(isRecord && !videoWriter.isOpened())
	{
		//録画用
		//videoWriter = cv::VideoWriter("capture.avi", CV_FOURCC('X','V','I','D'), pgrOpenCV->getFramerate(), cv::Size(PGRWIDTH, PGRHEIGHT));

	}


		// カメラ画の取得
		cv::Mat img;
		pgrOpenCV->queryFrame();
		img = pgrOpenCV->getVideo();

		if(!img.empty())
		{
			//録画フラグがたっていたら録画もする
			if(isRecord) videoWriter << img;

			//std::cout << "img size:" << img.rows << " * " << img.cols << std::endl;
    
			if(isShowWin)
			{
				// リサイズ
				cv::Mat resized_img(img.rows * 0.5, img.cols * 0.5, img.type());
				cv::resize(img, resized_img, resized_img.size(), cv::INTER_CUBIC);
    
				// 別ウィンドウの画を更新
				cv::imshow("camera", resized_img);
			}
			//↓↓処理するのはこっちに戻ってやるのだからx反転色変換いらないのでは？？
			// RGB --> ARGB 変換
		 //   cv::Mat argb_img, flip_img;
		 //   //cv::cvtColor(resized_img, argb_img, CV_RGB2BGRA);
		 //   cv::cvtColor(img, argb_img, CV_RGB2BGRA);
		 //   std::vector<cv::Mat> bgra;
		 //   cv::split(argb_img, bgra);
		 //   std::swap(bgra[0], bgra[3]);
		 //   std::swap(bgra[1], bgra[2]);
			////x軸反転
			//cv::flip(argb_img, flip_img, 0);
		 //   std::memcpy(data, flip_img.data, flip_img.total() * flip_img.elemSize());

			std::memcpy(data, img.data, img.total() * img.elemSize());
		}
}

DLLExport void showPixelData(unsigned char* data)
{
	//カメラ画像をMatに復元
	cv::Mat pgr_img(PGRHEIGHT, PGRWIDTH, CV_8UC1, data);

	// 別ウィンドウの画を更新
	cv::imshow("PGR", pgr_img);

}
