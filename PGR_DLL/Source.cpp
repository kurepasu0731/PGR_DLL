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
	auto pgrOpenCV = static_cast<TPGROpenCV*>(pgr);

	//���m�N��1ch�Ŏ���Ă���
	pgrOpenCV->init(FlyCapture2::PIXEL_FORMAT_MONO8, FlyCapture2::NEAREST_NEIGHBOR);
	pgrOpenCV->start();

	if(device != -1)
	{
		//�^��p
		videoWriter = cv::VideoWriter("capture.avi", CV_FOURCC('X','V','I','D'), pgrOpenCV->getFramerate(), cv::Size(PGRWIDTH, PGRHEIGHT), false); //isColor��3ch or 1ch
	}
}

DLLExport void releasePGR(void* pgr, int device)
{
    // �E�B���h�E�����
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
		//�^��p
		//videoWriter = cv::VideoWriter("capture.avi", CV_FOURCC('X','V','I','D'), pgrOpenCV->getFramerate(), cv::Size(PGRWIDTH, PGRHEIGHT));

	}


		// �J������̎擾
		cv::Mat img;
		pgrOpenCV->queryFrame();
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

DLLExport void showPixelData(unsigned char* data)
{
	//�J�����摜��Mat�ɕ���
	cv::Mat pgr_img(PGRHEIGHT, PGRWIDTH, CV_8UC1, data);

	// �ʃE�B���h�E�̉���X�V
	cv::imshow("PGR", pgr_img);

}