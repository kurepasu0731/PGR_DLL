#include "Source.h"
#include "DebugLogWrapper.h"

DLLExport void* getPGR(int device)
{

	//-1��������Đ����[�h
	if(device != -1)
	{
		//���ʂ�PGR�������Ă���Ƃ�
		return static_cast<void*>(new TPGROpenCV(0));
	}
	else
	{
		//�^��Đ��p	
		return static_cast<void*>(new cv::VideoCapture("capture.avi"));
	}
}

DLLExport void initPGR(void* pgr, int device)
{
	if(device != -1)
	{
		auto pgrOpenCV = static_cast<TPGROpenCV*>(pgr);

		//���m�N��1ch�Ŏ���Ă���
		//pgrOpenCV->init(FlyCapture2::PIXEL_FORMAT_RGB8, FlyCapture2::NEAREST_NEIGHBOR); //->���m�N�����Ƌ��������肵�Ȃ����獡�̂Ƃ����3ch��
		pgrOpenCV->init(FlyCapture2::PIXEL_FORMAT_RGB8, FlyCapture2::HQ_LINEAR); //->���m�N�����Ƌ��������肵�Ȃ����獡�̂Ƃ����3ch��
		pgrOpenCV->start();

		//�^��p
		videoWriter = cv::VideoWriter("capture.avi", CV_FOURCC('X','V','I','D'), pgrOpenCV->getFramerate(), cv::Size(PGRWIDTH, PGRHEIGHT), true); //isColor��3ch or 1ch
	}
}

DLLExport void releasePGR(void* pgr, int device)
{
    // �E�B���h�E�����
	if(device != -1) 
	{
		videoWriter.release();
		auto pgrOpenCV = static_cast<TPGROpenCV*>(pgr);
		pgrOpenCV->stop();
		pgrOpenCV->release();
	}
	else
	{
		auto vc = static_cast<cv::VideoCapture*>(pgr);
		delete vc;
	}
	cv::destroyAllWindows();
}

DLLExport void getPGRTexture(void* pgr, int device, unsigned char* data, bool isRecord, bool isShowWin)
{
	if(device != -1)
	{
		auto pgrOpenCV = static_cast<TPGROpenCV*>(pgr);

		if(isRecord && !videoWriter.isOpened())
		{
			//�^��p
			//videoWriter = cv::VideoWriter("capture.avi", CV_FOURCC('X','V','I','D'), pgrOpenCV->getFramerate(), cv::Size(PGRWIDTH, PGRHEIGHT), false);

		}


			// �J������̎擾
			cv::Mat img;
			//pgrOpenCV->queryFrame();
			img = pgrOpenCV->getVideo();

			if(!img.empty())
			{
				//�^��t���O�������Ă�����^�������
				if(isRecord) videoWriter << img;

				//std::cout << "img size:" << img.rows << " * " << img.cols << std::endl;
    
				if(isShowWin)
				{
					// ���T�C�Y
					cv::Mat resized_img(img.rows * 0.5, img.cols * 0.5, img.type());
					cv::resize(img, resized_img, resized_img.size(), cv::INTER_CUBIC);
    
					// �ʃE�B���h�E�̉���X�V
					cv::imshow("camera", resized_img);
				}
				//������������̂͂������ɖ߂��Ă��̂�����x���]�F�ϊ�����Ȃ��̂ł́H�H
				// RGB --> ARGB �ϊ�
			 //   cv::Mat argb_img, flip_img;
			 //   //cv::cvtColor(resized_img, argb_img, CV_RGB2BGRA);
			 //   cv::cvtColor(img, argb_img, CV_RGB2BGRA);
			 //   std::vector<cv::Mat> bgra;
			 //   cv::split(argb_img, bgra);
			 //   std::swap(bgra[0], bgra[3]);
			 //   std::swap(bgra[1], bgra[2]);
				////x�����]
				//cv::flip(argb_img, flip_img, 0);
			 //   std::memcpy(data, flip_img.data, flip_img.total() * flip_img.elemSize());

				std::memcpy(data, img.data, img.total() * img.elemSize());
			}
	}
	else
	{
		auto vc = static_cast<cv::VideoCapture*>(pgr);

		// �J������̎擾
		cv::Mat img;
		*vc >> img;

		if(!img.empty())
		{

			if(isShowWin)
			{
				// ���T�C�Y
				cv::Mat resized_img(img.rows * 0.5, img.cols * 0.5, img.type());
				cv::resize(img, resized_img, resized_img.size(), cv::INTER_CUBIC);
    
				// �ʃE�B���h�E�̉���X�V
				cv::imshow("camera", resized_img);
			}

			std::memcpy(data, img.data, img.total() * img.elemSize());
		}
	}
}

DLLExport void showPixelData(unsigned char* data)
{
	//�J�����摜��Mat�ɕ���
	cv::Mat pgr_img(PGRHEIGHT, PGRWIDTH, CV_8UC3, data);

	// �ʃE�B���h�E�̉���X�V
	cv::imshow("PGR", pgr_img);

}


//**�h�b�g���o�֘A**//
DLLExport void setDotsParameters(void* pgr, double AthreshVal, int DotThreshValMin, int DotThreshValMax, float resizeScale)
{
auto pgrOpenCV = static_cast<TPGROpenCV*>(pgr);

pgrOpenCV->setDotsParameters(AthreshVal, DotThreshValMin, DotThreshValMax, resizeScale);

}

DLLExport int getDotsCount(void* pgr)
{
	auto pgrOpenCV = static_cast<TPGROpenCV*>(pgr);
	return pgrOpenCV->getDotsCount();
}

DLLExport void getDotsData(void* pgr, int *data)
{
	auto pgrOpenCV = static_cast<TPGROpenCV*>(pgr);
	std::vector<int> datavec;
	pgrOpenCV->getDotsData(datavec); //data�̃T�C�Y��dotsCount*2
	//data = &datavec[0];
	memcpy(data, &datavec[0], datavec.size()*sizeof(int));
}

//�}�X�N����
DLLExport void createCameraMask_pgr(void* pgr, unsigned char* cam_data)
{
	auto pgrOpenCV = static_cast<TPGROpenCV*>(pgr);

	//�J�����摜��Mat�ɕ���
	cv::Mat cam_img(PGRHEIGHT, PGRWIDTH, CV_8UC4, cam_data);
	//�v���W�F�N�^�摜��Unity���Ő������ꂽ�̂ŁA���]�Ƃ�����
	//BGR <-- ARGB �ϊ�
	cv::Mat bgr_img, flip_cam_img;
	std::vector<cv::Mat> bgra;
	cv::split(cam_img, bgra);
	std::swap(bgra[0], bgra[3]);
	std::swap(bgra[1], bgra[2]);
	cv::cvtColor(cam_img, bgr_img, CV_BGRA2BGR);
	//x�����]
	cv::flip(bgr_img, flip_cam_img, 0);

	pgrOpenCV->mask = flip_cam_img.clone();

	cv::imshow("mask", pgrOpenCV->mask);

}


