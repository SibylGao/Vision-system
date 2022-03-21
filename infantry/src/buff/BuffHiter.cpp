//
// Created by sun on 18-1-12.
//
#include "BuffHiter.h"
#define DEBUG_BUFFHITER_TIME
#define RETURN_FALSE
//#define FEEDBACK_COLOR
//#define DEBUGE_LABLE_IMG
//#define DEBUGE_BIGBUFF_IMG

namespace hitcrt{
    ofstream area("/home/infantry/Infantry/data/buff/logs/area.txt",ios_base::out);
    BuffHiter::BuffHiter():
            prcIndi(-1),
            yaw(0),
            pitch(0),
            hitNumID(-1),
            stateCodeNum(-1),
            laststateCodeNum(-1),
            changestateCodeNum(-1),
            cntFrameBuffDefQuick(0),
            cntFrameBuffDefQuickWhole(0),
            stateflag(0),
            saveTime(1) {
        cv::FileStorage fs;
//        fs.open("/home/sun/HITCRT/Buff/data/camera4_3.31.yml", cv::FileStorage::READ);
//        fs.open("/home/sun/HITCRT/Buff/data/camera4_4.21.yml", cv::FileStorage::READ);
        fs.open("/home/sun/HITCRT/Infantry/data/buff/camera16_0706.yml", cv::FileStorage::READ);

        assert(fs.isOpened());
        fs["camera_matrix"] >> BuffParam::KK;
        fs["distortion_coefficients"] >> BuffParam::DISTORT;

        time_t log1;
        int name_log = time(&log1);
        ostringstream os_log;
        os_log <<"/home/infantry/Infantry/data/buff/logs/" << name_log <<"_buffHiter.txt";
        out.open(os_log.str(),ios_base::out);
        cv::initUndistortRectifyMap(BuffParam::KK, BuffParam::DISTORT, cv::Mat(), cv::Mat(), Size(1920, 1080),CV_32F,map1,map2);
    }


    bool BuffHiter::hitSmallBuff(cv::Mat& frame){
        cntFrameBuffDefQuick ++;
        double t1, t2;
        cout<<"Master: saveTime before extractor is "<<saveTime<<endl;
        if(picture_time_flag){
            out << "picture_time_flag_start = true" << endl;
        }else{
            out << "picture_time_flag_start = false" << endl;
        }
        //Step1. extractQuads
        t1 = (double)cv::getTickCount();
        cv::Mat image;
        image = frame.clone();

        if(saveTime!=0){
            quadsTen.clear();
            if(!extractor.runSmallBuff(image, quadsNine,quadsTen)){
#ifdef RETURN_FALSE

        out <<"Master: cannot extract Quads !"<<endl;
#endif
                cout<<"Master: cannot extract Quads !"<<endl;
                return false;
            }

            if(quadsNine.size()!=9){
#ifdef RETURN_FALSE

        out<<"Master: quadsNine size, its size is "<<quadsNine.size()<<endl;
#endif
                cout<<"Master: quadsNine size, its size is "<<quadsNine.size()<<endl;
                return false;
            }
        }
        t2 = ((double)cv::getTickCount() - t1)*1000/ cv::getTickFrequency();

#ifdef DEBUG_BUFFHITER_TIME
        cout << "Master: QuadExtractor time is " << t2 << "ms"<< endl;
        out << "Master: QuadExtractor time is " << t2 << "ms"<< endl;
#endif

        //New Step2. Raw Step4. Calculate yaw pitch distance
        t1 = (double)cv::getTickCount();
        if(saveTime!=0){
            cout << "*************absolute angles***********" << endl;
            absoluteAngleCal.MatrixW2CCal(quadsNine, BuffParam::KK);
            absoluteAngleCal.MatrixW2GCal();
            absoluteAngleCal.nineYawPitchCal();
        }

        t2 = ((double)cv::getTickCount() - t1)*1000 / cv::getTickFrequency();
#ifdef DEBUG_BUFFHITER_TIME
        cout << "Master: calculate yaw pitch distance time is: " << t2 << "ms" << endl;
        out << "Master: calculate yaw pitch distance time is: " << t2 << "ms" << endl;
#endif

        //New Step3. Raw Step2. Send nine images to CNN and get labels
        t1 = (double)cv::getTickCount();
        vector<int> labels(9,1000);
        cv::Mat labelImg = frame.clone();
        if(!getLabels(quadsNine, labels, labelImg, 2)){
            cout << "Label: get labels failed!!!" << endl;
#ifdef RETURN_FALSE
        out << "Label: get labels failed!!!" << endl;
#endif
            return  false;
        }
        t2 = ((double)cv::getTickCount() - t1)*1000 / cv::getTickFrequency();
#ifdef DEBUG_BUFFHITER_TIME
        cout << "Master: getlabels time is: " << t2 << "ms" << endl;
        out << "Master: getlabels time is: " << t2 << "ms" << endl;
        //if(t2>50) waitKey(0);
#endif
        //New Step4. Raw Step3. Decode number
        t1 = (double)cv::getTickCount();
        cv::Mat decoderImg = frame.clone();
        //if(numDecoder.codeRect.size() == Size(0, 0) || NumDecoder::initFlag == false){
        if(saveTime!=0){
            if(numDecoder.init(decoderImg, quadsNine)){
                NumDecoder::initFlag = true;
            } else {
                cout << "Master: numDecoder init failed!!!" << endl;
#ifdef RETURN_FALSE
        out << "Master: numDecoder init failed!!!" << endl;
#endif
                return false;
            }
        }
        //}

        codeNum.clear();
        if(!numDecoder.run(decoderImg, quadsNine, codeNum)){
            cout<<"Master: can't decode number !!!!"<<endl;
#ifdef RETURN_FALSE
        out <<"Master: can't decode number !!!!"<<endl;
#endif
            return false;
            //waitKey(0);
        }
        //judge codeNum is or not right
        if(codeNum.size() != 5){
            cout << "Master: decode number failed. codeNum size is " << codeNum.size() << endl;
#ifdef RETURN_FALSE
        out << "Master: decode number failed. codeNum size is " << codeNum.size() << endl;
#endif
            return false;
        }

        //log file
        static int last_nFrame = 0;
        static int nFrame = 1;
        ostringstream os1, os2, os3;
        out << "small buff" <<endl;
        out << "last_nFrame: " << last_nFrame << endl;
        out << "nFrame: " << nFrame << endl;
        out <<  "codeNum: " <<"\t";
        for (int i = 0; i < codeNum.size(); i++) {
            out << codeNum[i] << "\t";
        }
        out << endl;


        //output codeNum and nine numbers
        cout << "---------------codeNum--------------"<< endl;
        for(int i = 0; i < codeNum.size(); i ++) cout << "     " << codeNum[i];
        cout << endl;
        cout << "---------------buffNum--------------" << endl;
        cout << "        " << labels[0] << "        " << labels[3] << "        " << labels[6] << endl;
        cout << "        " << labels[1] << "        " << labels[4] << "        " << labels[7] << endl;
        cout << "        " << labels[2] << "        " << labels[5] << "        " << labels[8] << endl;

        t2 = ((double)cv::getTickCount() - t1)*1000 / cv::getTickFrequency();
#ifdef DEBUG_BUFFHITER_TIME
        cout << "Master: get codeNum time is " << t2 << "ms" << endl;
        out << "Master: get codeNum time is " << t2 << "ms" << endl;
#endif


        //New Step5. feedback
        t1 = (double)cv::getTickCount();
        vector<double> feedback(10);
        cv::Mat feedbackImg = frame.clone();

        if(!getFeedback(quadsTen, feedback, feedbackImg)){
            cout << "Feedback: get labels failed!!!" << endl;
#ifdef RETURN_FALSE
            out << "Feedback: get labels failed!!!" << endl;
#endif
            return  false;
        }

        int nRedOrBlue=0;
        for(int i=1;i<feedback.size(); i++){
            if ((i!=5)&&(feedback[i]>0.4)){
                nRedOrBlue++;
                color=true;

            }

        }
        if(color){
        out<<"color = true"<<endl;
        }else{
        out<<"color = false"<<endl;
        }


//        for(int i=1;i<5;i++){
//            if(fabs(feedback[i+5]-feedback[i])>0.5){
//            namedWindow("feedbackImg",WINDOW_KEEPRATIO);
//            imshow("feedbackImg",feedbackImg);
//            //waitKey(0);
//        }


//        }

        t2 = ((double)cv::getTickCount() - t1)*1000 / cv::getTickFrequency();

#ifdef DEBUG_BUFFHITER_TIME
        cout << "Master: feedback time is: " << t2 << "ms" << endl;
        out << "Master: feedback time is: " << t2 << "ms" << endl;
        //if(t2>50) waitKey(0);
#endif




        //New Step6.//Step5. Compare result
        t1 = (double)cv::getTickCount();
        buffNum.clear();
        for(int i = 0; i<labels.size(); i++) {
            buffNum.push_back(labels[i]);
        }

        out <<  "buffNum: " << "\t";
        for (int i = 0; i < buffNum.size(); i++) {
            out << buffNum[i] << "\t";
        }
        out << endl;

        if(1) {
            cout<<"saveTime = "<<saveTime<<endl;
            int compResult = compFrame(buffNum, codeNum);
            saveTime = compResult;
            cout<<"saveTime = "<<saveTime<<endl;

            if(compResult == COMP_NEXT) {
                prcIndi++;
                stateflag=0;
                color = false;
                no_color_picture=0;
                reset_picture=0;
                cout << "COMP_NEXT" << endl;
                out <<"state: " << "COMP_NEXT" << endl;
                if(prcIndi == (codeNum.size())) {
                    prcIndi =0;
                }
            }
            else if(compResult == COMP_RESET) {
                prcIndi = 0;
                stateflag=1;
                if(!color){
                    no_color_picture++;
                    color=false;
                }else{
                no_color_picture=0;
                }
                reset_picture++;

                cout << "COMP_RESET" << endl;
                out <<"state: " << "COMP_RESET" << endl;

            }
            else if(compResult == COMP_WAIT_FRAME) {
                cout << "COMP_WAIT_FRAME" << endl;
                out <<"state: " << "COMP_WAIT_FRAME" << endl;

                return false;
            }
            else {
                cout<<"COMP_WAIT_PICTURE"<<endl;
                out <<"state: " << "COMP_WAIT_PICTURE" << endl;
                return true;
            }

            if(prcIndi > (codeNum.size()-1)) {
                cout<< "prcIndi is wrong, prcIndi = " << prcIndi << endl;
#ifdef RETURN_FALSE

        out<< "prcIndi is wrong, prcIndi = " << prcIndi << endl;
#endif
                return false;
            }
            cout<< "prcIndi = "<<prcIndi<<endl;

            //New Step7.//Step6. Get hitNum
            out<<"no_color_picture = "<<no_color_picture<<endl;
            if(no_color_picture==2){
                picture_color_flag=true;
                no_color_picture=3;
                out << "picture_color_flag = true" << endl;

            }else if(no_color_picture>=3){
                picture_color_flag=false;
                out << "picture_color_flag = false" << endl;
                no_color_picture=0;
            }else{
                out << "picture_color_flag = false" << endl;

            }
            out<<"reset_picture ="<<reset_picture<<endl;
            if(reset_picture==2){
                picture_reset_flag=true;
                reset_picture=3;
                out << "picture_reset_flag = true" << endl;

            }else if(reset_picture>=3){
                picture_reset_flag=false;
                out << "picture_reset_flag = false" << endl;
                reset_picture=0;
            }else{
                out << "picture_reset_flag = false" << endl;

            }

            if(!getHitNum(buffNum, codeNum, prcIndi)){
                cout << "Master: get HitNum failed!!!" << endl;
            }
            stateCodeNum=codeNum[prcIndi];
            out <<"----------------------------------hitNum is: " << codeNum[prcIndi] << "-----------------------------------"<< endl;
            last_nFrame = nFrame;
            nFrame++;

            last_picture_time = picture_time2;
            picture_time2 = ((double)cv::getTickCount() - picture_time1)*1000 / cv::getTickFrequency();
            picture_time1 = cv::getTickCount();

#ifdef DEBUG_BUFFHITER_TIME
        cout << "Master: picture time is: " << picture_time2 << "ms" << endl;
        out << "Master: last_picture time is: " << last_picture_time << "ms" << endl;
        out << "Master: picture time is: " << picture_time2 << "ms" << endl;
#endif
            if(last_picture_time!=0){
                if(picture_time2 < 300 && compResult){
                //if(picture_time2 < 300&&nRedOrBlue<2){
                //if(picture_time2 < 300){
                    picture_time_flag = true;
                    out << "picture_time_flag = true" << endl;
                    return true;
                }
            }
            picture_time_flag = false;
            out << "picture_time_flag_end = false" << endl;


            //cv::Mat calAngleImage = frame.clone();
            //showHitNum(calAngleImage);
            t2 = ((double)cv::getTickCount() - t1)*1000/ cv::getTickFrequency();
            cout << "cntFrameBuffDefQuick is "<<cntFrameBuffDefQuick << endl;
            if(cntFrameBuffDefQuick < 7)
            {
                cntFrameBuffDefQuickWhole ++;
                cout << "cntFrameBuffDefQuickWhole is " << cntFrameBuffDefQuickWhole << endl;
                if(cntFrameBuffDefQuickWhole >= 4)
                {
                    cntFrameBuffDefQuick = 0;
                    cntFrameBuffDefQuickWhole = 0;
                    cout<<"buffhiter: the picture change too fast"<<endl;
#ifdef RETURN_FALSE
        out<<"buffhiter: the picture change too fast"<<endl;
#endif
                    //return false;
                }
            }
            else
            {
                cntFrameBuffDefQuickWhole = 0;
            }

        cntFrameBuffDefQuick = 0;
#ifdef DEBUG_BUFFHITER_TIME
            cout << "Master: get hitNum time is " << t2 << "ms" << endl;
            out << "Master: get hitNum time is " << t2 << "ms" << endl;
#endif
            return true;
        }
    }

    bool BuffHiter::hitBigBuff(cv::Mat& frame){
        cntFrameBuffDefQuick ++;
        double t1, t2;
        cout<<"Master: saveTime before extractor is "<<saveTime<<endl;
        if(picture_time_flag){
            out << "picture_time_flag_start = true" << endl;
        }else{
            out << "picture_time_flag_start = false" << endl;
        }
        //Step1. extractQuads
        t1 = (double)cv::getTickCount();
        cv::Mat image;
        image = frame.clone();

        int index[5] = {0};
        if(saveTime!=0){
            if(!extractor.runBigBuff(image, quadsTen, index)){
                cout<<"Master: cannot extract Quads !"<<endl;
#ifdef RETURN_FALSE

        out<<"Master: cannot extract Quads !"<<endl;
#endif
                return false;
            }
//            for(int i=0;i<quadsTen.size();i++) {
//                cout<< "quadsTen.up_left.x  " << quadsTen[i].up_left.x<<endl;
//                cout<< "quadsTen.up_left.y  " << quadsTen[i].up_left.y<<endl;
//
//            }
            if(quadsTen.size() != 10){
                cout<<"quadsTen size is not right, its size is "<<quadsTen.size()<<endl;
#ifdef RETURN_FALSE

        out<<"quadsTen size is not right, its size is "<<quadsTen.size()<<endl;
#endif
                return false;
            }
        }
        t2 = ((double)cv::getTickCount() - t1)*1000/ cv::getTickFrequency();

#ifdef DEBUG_BUFFHITER_TIME
        cout << "Master: QuadExtractor time is " << t2 << "ms"<< endl;
        out << "Master: QuadExtractor time is " << t2 << "ms"<< endl;
#endif
        //New Step2. Raw Step4. Calculate yaw pitch distance
        t1 = (double)cv::getTickCount();
        if(saveTime!=0){
            quadsNine.clear();
            cout << "*************absolute angles***********" << endl;
            absoluteAngleCal.MatrixW2CCal2(quadsTen, quadsNine, BuffParam::KK, index);
            absoluteAngleCal.MatrixW2GCal();
            absoluteAngleCal.nineYawPitchCal();
        }

#ifdef DEBUGE_BIGBUFF_IMG
        cout << "quadsNine size " << quadsNine.size() << endl;
        cv::Mat imageQuadsNine = frame.clone();
        extractor.drawQuads(quadsNine, imageQuadsNine);
        imshow("imageQuadsNine ", imageQuadsNine);
#endif
        t2 = ((double)cv::getTickCount() - t1)*1000 / cv::getTickFrequency();
#ifdef DEBUG_BUFFHITER_TIME
        cout << "Master: calculate yaw pitch distance time is: " << t2 << "ms" << endl;
        out << "Master: calculate yaw pitch distance time is: " << t2 << "ms" << endl;
#endif
        //New Step3. Raw Step2. Send nine images to CNN and get labels
        t1 = (double)cv::getTickCount();
        vector<int> labels(9,1000);
        cv::Mat labelImg = frame.clone();
        if(!getLabels(quadsNine, labels, labelImg, 3)){
            cout << "Label: get labels failed!!!" << endl;
#ifdef RETURN_FALSE

        out << "Label: get labels failed!!!" << endl;
#endif
            return  false;
        }
        t2 = ((double)cv::getTickCount() - t1)*1000 / cv::getTickFrequency();

#ifdef DEBUG_BUFFHITER_TIME
        cout << "Master: getlabels time is: " << t2 << "ms" << endl;
        out << "Master: getlabels time is: " << t2 << "ms" << endl;
#endif
        //New Step4. Raw Step3. Decode number
        t1 = (double)cv::getTickCount();
        cv::Mat decoderImg = frame.clone();
        //if(numDecoder.codeRect.size() == Size(0, 0) || NumDecoder::initFlag == false){
        if(saveTime!=0){
            if(numDecoder.init(decoderImg, quadsNine)){
                NumDecoder::initFlag = true;
            } else {
                cout << "Master: numDecoder init failed!!!" << endl;
#ifdef RETURN_FALSE

        out << "Master: numDecoder init failed!!!" << endl;
#endif
                return false;
            }
        }

        codeNum.clear();
        if(!numDecoder.run(decoderImg, quadsNine, codeNum)){
            cout<<"Master: can't decode number !!!!"<<endl;
#ifdef RETURN_FALSE

        out<<"Master: can't decode number !!!!"<<endl;
#endif
            return false;
        }
        //judge codeNum is or not right
        if(codeNum.size() != 5){
            cout << "Master: decode number failed. codeNum size is " << codeNum.size() << endl;
#ifdef RETURN_FALSE

        out << "Master: decode number failed. codeNum size is " << codeNum.size() << endl;
#endif
            return false;
        }
        //log file
        static int nFrame = 1;
        ostringstream os1, os2, os3;
        out <<"big buff" << endl;
        out << "nFrame: " << nFrame << endl;
        out <<  "codeNum: " <<"\t";
        for (int i = 0; i < codeNum.size(); i++) {
            out << codeNum[i] << "\t";
        }
        out << endl;


        //output codeNum and nine numbers
        cout << "---------------codeNum--------------"<< endl;
        for(int i = 0; i < codeNum.size(); i ++) cout << "     " << codeNum[i];
        cout << endl;
        cout << "---------------buffNum--------------" << endl;
        cout << "        " << labels[0] << "        " << labels[3] << "        " << labels[6] << endl;
        cout << "        " << labels[1] << "        " << labels[4] << "        " << labels[7] << endl;
        cout << "        " << labels[2] << "        " << labels[5] << "        " << labels[8] << endl;

        t2 = ((double)cv::getTickCount() - t1)*1000 / cv::getTickFrequency();

#ifdef DEBUG_BUFFHITER_TIME
        cout << "Master: get codeNum time is " << t2 << "ms" << endl;
        out << "Master: get codeNum time is " << t2 << "ms" << endl;
#endif


        //New Step5. feedback
        t1 = (double)cv::getTickCount();
        vector<double> feedback(10);
        cv::Mat feedbackImg = frame.clone();

        if(!getFeedback(quadsTen, feedback, feedbackImg)){
            cout << "Feedback: get labels failed!!!" << endl;
#ifdef RETURN_FALSE
            out << "Feedback: get labels failed!!!" << endl;
#endif
            return  false;
        }

        int nRedOrBlue=0;
        for(int i=1;i<feedback.size(); i++){
            if ((i!=5)&&(feedback[i]>0.4)){
                nRedOrBlue++;
                color=true;
            }

        }
        if(color){
        out<<"color = true"<<endl;
        }else{
        out<<"color = false"<<endl;
        }

        t2 = ((double)cv::getTickCount() - t1)*1000 / cv::getTickFrequency();

#ifdef DEBUG_BUFFHITER_TIME
        cout << "Master: feedback time is: " << t2 << "ms" << endl;
        out << "Master: feedback time is: " << t2 << "ms" << endl;
        //if(t2>50) waitKey(0);
#endif




        //Step5. compare result
        t1 = (double)cv::getTickCount();
        buffNum.clear();
        for(int i = 0; i<labels.size(); i++) {
            buffNum.push_back(labels[i]);
        }

        out <<  "buffNum: " << "\t";
        for (int i = 0; i < buffNum.size(); i++) {
            out << buffNum[i] << "\t";
        }
        out << endl;

        if(1) {
            cout<<"saveTime = "<<saveTime<<endl;
            int compResult = compFrame(buffNum, codeNum);
            saveTime = compResult;
            cout<<"saveTime = "<<saveTime<<endl;

            if(compResult == COMP_NEXT) {
                prcIndi++;
                stateflag=0;
                color = false;
                no_color_picture=0;
                cout << "COMP_NEXT" << endl;
                reset_picture=0;
                out <<"state: " << "COMP_NEXT" << endl;
                if(prcIndi == (codeNum.size())) {
                    prcIndi =0;
                }
            }
            else if(compResult == COMP_RESET) {
                prcIndi = 0;
                stateflag=1;
                if(!color){
                    no_color_picture++;
                    color=false;
                }else{
                no_color_picture=0;
                }
                cout << "COMP_RESET" << endl;
                reset_picture++;
                out <<"state: " << "COMP_RESET" << endl;
            }
            else if(compResult == COMP_WAIT_FRAME) {
                cout << "COMP_WAIT_FRAME" << endl;
                out <<"state: " << "COMP_WAIT_FRAME" << endl;
                return false;
            }
            else {
                cout<<"COMP_WAIT_PICTURE"<<endl;
                out <<"state: " << "COMP_WAIT_PICTURE" << endl;

                return true;
            }
            if(prcIndi > (codeNum.size()-1)) {
                cout<< "prcIndi is wrong, prcIndi = "<<prcIndi<<endl;
#ifdef RETURN_FALSE

        out<< "prcIndi is wrong, prcIndi = "<<prcIndi<<endl;
#endif
                return false;
            }
            cout<< "prcIndi = "<<prcIndi<<endl;
            //New Step7.//Step6. Get hitNum
            out<<"no_color_picture ="<<no_color_picture<<endl;
            if(no_color_picture==2){
                picture_color_flag=true;
                no_color_picture=3;
                out << "picture_color_flag = true" << endl;

            }else if(no_color_picture>=3){
                picture_color_flag=false;
                out << "picture_color_flag = false" << endl;
                no_color_picture=0;
            }else{
                out << "picture_color_flag = false" << endl;

            }
            out<<"reset_picture ="<<reset_picture<<endl;
            if(reset_picture==2){
                picture_reset_flag=true;
                reset_picture=3;
                out << "picture_reset_flag = true" << endl;

            }else if(reset_picture>=3){
                picture_reset_flag=false;
                out << "picture_reset_flag = false" << endl;
                reset_picture=0;
            }else{
                out << "picture_reset_flag = false" << endl;

            }




            if(!getHitNum(buffNum, codeNum, prcIndi)){
                cout << "Master: get HitNum failed!!!" << endl;
            }
            out <<"----------------------------------hitNum is: " << codeNum[prcIndi] << "-----------------------------------"<< endl;
            nFrame++;


            last_picture_time = picture_time2;
            picture_time2 = ((double)cv::getTickCount() - picture_time1)*1000 / cv::getTickFrequency();
            picture_time1 = cv::getTickCount();

#ifdef DEBUG_BUFFHITER_TIME
        cout << "Master: picture time is: " << picture_time2 << "ms" << endl;
        out << "Master: last_picture time is: " << last_picture_time << "ms" << endl;
        out << "Master: picture time is: " << picture_time2 << "ms" << endl;
#endif

            if(last_picture_time!=0){
                if(picture_time2< 300 && compResult){
                //if(picture_time2 < 300&&nRedOrBlue<2){
                //if(picture_time2 < 300){
                    picture_time_flag = true;
                    out << "picture_time_flag = true" << endl;
                    return true;
                }
            }
            picture_time_flag = false;
            out << "picture_time_flag_end = false" << endl;

            //cv::Mat calAngleImage = frame.clone();
            //showHitNum(calAngleImage);
            t2 = ((double)cv::getTickCount() - t1)*1000/ cv::getTickFrequency();
            cout << "Master: get hitNum time is " << t2 << "ms" << endl;
            cout << "cntFrameBuffDefQuick is "<<cntFrameBuffDefQuick << endl;
            if(cntFrameBuffDefQuick < 7)
            {
                cntFrameBuffDefQuickWhole ++;
                cout << "cntFrameBuffDefQuickWhole is " << cntFrameBuffDefQuickWhole << endl;
                if(cntFrameBuffDefQuickWhole >= 4)
                {
                    cntFrameBuffDefQuick = 0;
                    cntFrameBuffDefQuickWhole = 0;
                    cout<<"buffhiter: the picture change too fast"<<endl;
#ifdef RETURN_FALSE

        out<<"buffhiter: the picture change too fast"<<endl;
#endif
                    return false;
                }
            }
            else
            {
                cntFrameBuffDefQuickWhole = 0;
            }

        cntFrameBuffDefQuick = 0;
        t2 = ((double)cv::getTickCount() - t1)*1000/ cv::getTickFrequency();

#ifdef DEBUG_BUFFHITER_TIME
            cout << "Master: get hitNum time is " << t2 << "ms" << endl;
            out << "Master: get hitNum time is " << t2 << "ms" << endl;
#endif

            return true;
        }
    }

    bool BuffHiter::getLabels(std::vector<Quad>& quadsNine, std::vector<int>& labels, cv::Mat& label_image, int buffFlag){
        double t1 = (double)cv::getTickCount();
        vector<Mat> imgNine(9);///加上仿射变换 变为正视图
        Mat plane_coordinate(4,1,CV_32FC2),src_coordinate(4,1,CV_32FC2);
        plane_coordinate.at<Point2f>(0, 0) = Point2f(0, 0);
        plane_coordinate.at<Point2f>(1, 0) = Point2f(280, 0);
        plane_coordinate.at<Point2f>(2, 0) = Point2f(280, 160);
        plane_coordinate.at<Point2f>(3, 0) = Point2f(0, 160);

        int clear_x = 3;
        int clear_y = 3;

        for(int i=0; i < quadsNine.size();i++) {

            src_coordinate.at<Point2f>(0, 0) = quadsNine[i].up_left + Point2f(2 * clear_x, clear_y);
            src_coordinate.at<Point2f>(1, 0) = quadsNine[i].up_right + Point2f(-2 * clear_x, clear_y);
            src_coordinate.at<Point2f>(2, 0) = quadsNine[i].down_right + Point2f(-2 * clear_x, -clear_y);
            src_coordinate.at<Point2f>(3, 0) = quadsNine[i].down_left + Point2f(2 * clear_x, -clear_y);
            Mat trans_Perspective = getPerspectiveTransform(src_coordinate, plane_coordinate);
            warpPerspective(label_image, imgNine[i], trans_Perspective, Size(280, 160));

            //smallBuff preprocess
            cv::Mat grayImg;
            if (buffFlag == 2) {
                cvtColor(imgNine[i], imgNine[i], COLOR_BGR2GRAY);
                imgNine[i] = 255 - imgNine[i];
                threshold(imgNine[i], imgNine[i], 0, 255, CV_THRESH_OTSU);
                resize(imgNine[i], imgNine[i], cv::Size(28, 28));
            } else if (buffFlag == 3) {//bigBuff preprocess

                grayImg = imgNine[i].clone();
                cv::Mat element = getStructuringElement(MORPH_RECT, Size(1, 1));
                morphologyEx(grayImg, grayImg, CV_MOP_OPEN, element);
                cvtColor(grayImg, grayImg, COLOR_BGR2GRAY);

#ifdef DEBUGE_LABLE_IMG
                ostringstream oss1;
                oss1 << i << "grayImg" << endl;
                imshow(oss1.str(), grayImg);
#endif
                cv::Mat threshImg;
                threshold(grayImg, threshImg, 0, 255, CV_THRESH_OTSU);

#ifdef DEBUGE_LABLE_IMG
                ostringstream oss2;
                oss1 << i << "threshImg" << endl;
                imshow(oss2.str(), threshImg);
#endif

                erode(threshImg, threshImg, element, Point(-1, -1), 2);
#ifdef DEBUGE_LABLE_IMG
                ostringstream oss3;
                oss3 << i << "erodeImg" << endl;
                imshow(oss3.str(), threshImg);
#endif
                resize(threshImg, threshImg, cv::Size(28, 28));
//                imgNine[i] = threshImg;

                vector<vector<Point>> contours;
                vector<Vec4i> hierarchy;
                vector<Rect> numRects;
                findContours(threshImg, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
                for(int i = 0; i < contours.size();i++){
                    drawContours(threshImg, contours, i, Scalar(255, 255, 255), CV_FILLED);
                    Rect rectTmp;
                    rectTmp = boundingRect(contours[i]);
                    numRects.push_back(rectTmp);
                }

                if(numRects.size() < 1){
                    cout << "Label: get numRects failed!!!" << endl;
                    return false;
                }


                float maxArea = -1;
                int maxAreaIndex = 0;
                if (numRects.size() > 1) {
                    for (int i = 0; i < numRects.size(); i++){
                        if (numRects[i].area() > maxArea){
                            maxArea = numRects[i].area();
                            maxAreaIndex = i;
                        }
                    }
                }


                Rect numRect = numRects[maxAreaIndex];
                cv::Mat numImg = threshImg(numRect);
//                imgRect.width = imgRect.height = max(rects[maxAreaIndex].boundingRect().width, rects[maxAreaIndex].boundingRect().height);
//                imgRect.x = rects[maxAreaIndex].center.x - 0.5 * imgRect.width;
//                imgRect.y = rects[maxAreaIndex].center.y - 0.5 * imgRect.height;
//
//                rectangle(imgNine[i], imgRect, Scalar(0, 255, 0));
//                imshow("rect", imgNine[i]);

//                if (imgRect.x < 0 ) imgRect.x = 0;
//                if (imgRect.y < 0 ) imgRect.y = 0;
//                if ((imgRect.x + imgRect.width) > threshImg.cols) imgRect.width = threshImg.cols - imgRect.x;
//                if ((imgRect.y + imgRect.height) > threshImg.rows) imgRect.height = threshImg.rows - imgRect.y;

                cv::Mat finalImg = Mat::zeros(Size(28, 28),CV_8U);
//                saveImg = 255 - saveImg;

                //put img in
                Rect finalNumRect = Rect(14 - 0.5*numImg.cols , 14 - 0.5*numImg.rows, numImg.cols, numImg.rows);
                cv::Mat imgROI = finalImg(finalNumRect);
                numImg.copyTo(imgROI);
                imgNine[i] = finalImg;

            } else {
                cout << "Label: buffFlag is wrong!!!" << endl;
            }


#ifdef DEBUGE_LABLE_IMG
            ostringstream oss;
            oss << "imgNine: " << i <<endl;
            imshow(oss.str(),imgNine[i]);
#endif
        }

        double t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
        cout<<"Label: nineQuads preprocess time is "<<t2<<"ms"<<endl;
        out<<"Label: nineQuads preprocess time is "<<t2<<"ms"<<endl;

        t1 = (double)cv::getTickCount();
        vector<vector<float>> confidence(9);// 9 个标签的置信率
        if (buffFlag == 2){
            tf_classify.label_imgs(imgNine,confidence, 2);
        } else if(buffFlag == 3){
            tf_classify.label_imgs(imgNine,confidence, 3);
        } else if(buffFlag != 2 || buffFlag != 3){
            cout << "Label: buffFlag is wrong!!!" << endl;
        }

        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
        cout<<"Label: get nine labels pb time is "<<t2<<"ms"<<endl;
        out<<"Label: get nine labels pb time is "<<t2<<"ms"<<endl;


        for(int i = 0; i < confidence.size(); i++){
            float max_confidence=-1;
            int label = 10000;
            for(int j = 0;j < confidence[i].size();j++){
//                   cout << "confidence" << i << j << " " << confidence[i][j]<<endl;
                if(max_confidence<confidence[i][j]){
                    max_confidence = confidence[i][j];
                    label = j;
                }
            }

            if(max_confidence<0.5){
                cout <<"Label: cannot trust this label! "<<label<<" its confidence:"<<max_confidence<<endl;
            }
#ifdef DEBUGE_LABLE_IMG
            cout<<"Label: quad "<<i<<" is "<<label<<" with confidence "<<max_confidence<<endl;
#endif
            labels[i] = label;
            quadsNine[i].number = label;
            quadsNine[i].confidence = max_confidence;
        }


        if(t2 > 50){
            ostringstream pb_time;
            pb_time <<"/home/infantry/Infantry/data/buff/buff_error/buff_num/";
            for(int i=0; i<9;i++){
                time_t img_time;
                int img_name = time(&img_time);
                pb_time << labels[i] <<"/" << img_name <<".jpg";
                imwrite(pb_time.str(),imgNine[i]);
            }
        }

        //judge lable is or not right
//        vector<Point2f> numberSorted;//x:number y: size
        int searchNumFlag[9] = { 0 };
        vector<pair<int, int>> numberSorted;//first is number, second is size

        for (int i = 0; i < labels.size(); i++) {
            if (searchNumFlag[i]) continue;
            searchNumFlag[i] = 1;
            pair<int, int> myPair;
            myPair.first = labels[i];
            myPair.second = 1;
            for (int j = 0; j < labels.size(); j++) {
                if (searchNumFlag[j]) continue;
                if (labels[i] == labels[j]) {
                    searchNumFlag[j] = 1;
                    myPair.second++;
                }
            }
            numberSorted.push_back(myPair);
        }

        /*for (int i = 0; i < labels.size(); i++) {
            if (searchNumFlag[i]) continue;
            searchNumFlag[i] = 1;
            Point2f myPoint;
            myPoint.x = labels[i];
            myPoint.y = 1;
            for (int j = 0; j < labels.size(); j++) {
                if (searchNumFlag[j]) continue;
                if (labels[i] == labels[j]) {
                    searchNumFlag[j] = 1;
                    myPoint.y++;
                }
            }
            numberSorted.push_back(myPoint);
        }*/

//         for(int i = 0; i<numberSorted.size(); i++) {
//             cout<<numberSorted[i].y << " number " << numberSorted[i].x <<endl;
//         }

        vector<int> twoSameNumIndex;
        int numberTotal = 0;

        if(numberSorted.size() == 9){
            for (int i = 0; i<labels.size(); i++) {
                numberTotal += labels[i];
            }

            if(numberTotal != 45){
                for (int i = 0; i<labels.size(); i++) {
                    if(labels[i] == 0) labels[i] = 45 - numberTotal;
                }
            }
        }

        //save wrong lables
        int lableIsZero = 0;
        static int errorCount = 0;
        for (int i = 0; i < labels.size(); i++) {
            if(labels[i] == 0) lableIsZero ++;
        }


//        cv::Mat resizedImg;
//        if(numberSorted.size() < 9 || lableIsZero > 0){
//            for (int  i = 0; i < imgNine.size(); i++){
//                errorCount ++;
//                ostringstream oss1;
//                oss1 <<"/home/base2/data/buff/buff_error";
//                oss1 << "/" << labels[i] << "/" << errorCount <<".jpg";
//                resize(imgNine[i], resizedImg, Size(28, 28));
//                //imwrite(oss1.str(), resizedImg);
//            }
//        }

//        if(lableIsZero > 0){
//            cout << "Label: labels is not right" << endl;
//            cout << "Label: 0 label size is" << lableIsZero << endl;
//            //waitKey(0);
//            return false;
//        }

        numberTotal = 0;

        if(numberSorted.size() == 8){//two number same
            for(int i = 0; i<numberSorted.size(); i++) {
                numberTotal += numberSorted[i].first;
                if(numberSorted[i].second == 2){
                    for (int j = 0; j <quadsNine.size(); j++){
                        if (quadsNine[j].number == numberSorted[i].first){
                            twoSameNumIndex.push_back(j);
//                             cout << numberSorted[i].first << endl;
//                             cout << "index: " << j <<endl;
                        }
                    }
                }
            }

            if(lableIsZero == 0){
                if (quadsNine[twoSameNumIndex[0]].confidence > quadsNine[twoSameNumIndex[1]].confidence) {
                    quadsNine[twoSameNumIndex[1]].number = 45 - numberTotal;
                    quadsNine[twoSameNumIndex[1]].confidence = 1;
                    labels[twoSameNumIndex[1]] = 45 - numberTotal;
//                  cout << "quads" << twoSameNumIndex[1] << ": " << quadsNine[twoSameNumIndex[1]].number << endl;
                } else {
                    quadsNine[twoSameNumIndex[0]].number = 45 - numberTotal;
                    quadsNine[twoSameNumIndex[0]].confidence = 1;
                    labels[twoSameNumIndex[0]] = 45 - numberTotal;
//                 cout << "quads" << twoSameNumIndex[0] << ": " << quadsNine[twoSameNumIndex[0]].number << endl;
                }
            }else if(lableIsZero == 1){
                cout << "numberSorted size is 8, label 0 size is 1" << endl;
                out << "numberSorted size is 8, label 0 size is 1" << endl;
                return false;
            }

        } else if(numberSorted.size() < 8){
            cout << "Label: numberSorted size < 8, labels is not right" << endl;
            out << "Label: numberSorted size < 8, labels is not right" << endl;
            //waitKey(0);
            return false;
        }
        return true;
    }

    //若数码管数字不变，九宫格数字变化，则打击下一个，若九宫格数码管数字同时变化，则重新开始，否则进行等待
    int BuffHiter::compFrame(vector<int> buffNum, vector<int> codeNum) {
        //第一幅图
        if((buffNumLast.size() == 0)&&(codeNumLast.size() == 0)) {
            buffNumLast.clear();
            codeNumLast.clear();
            buffNumLast = buffNum;
            codeNumLast = codeNum;
            return COMP_NEXT;
        }

        if(!((buffNumLast.size() == buffNum.size())&&(codeNumLast.size() == codeNum.size()))) {
            buffNumLast.clear();
            codeNumLast.clear();
            buffNumLast = buffNum;
            codeNumLast = codeNum;
            return COMP_WAIT_FRAME;
        }

        int sameCnt = 0;
        for(int i = 0; i < buffNum.size(); i++) {
            if(buffNum[i]==buffNumLast[i]) {
                sameCnt++;
            }
        }
        buffNumLast = buffNum;

        if(sameCnt > 5) {
            return COMP_WAIT_PICTURE;
        }
        else {
            sameCnt = 0;
            for(int i = 0; i < codeNum.size(); i++) {
                if(codeNum[i]==codeNumLast[i]) {
                    sameCnt++;
                }
            }
            codeNumLast = codeNum;

            if(sameCnt == codeNum.size()) {
                return COMP_NEXT;
            }
            else {
                return COMP_RESET;
            }
        }
    }

    bool BuffHiter::getHitNum(vector<int> buffNum, vector<int> codeNum, int prcIndi){
        int sameLabel = 0;
        for(int  i = 0; i < 9; i ++) {
            if(buffNum[i] == codeNum[prcIndi]) {

                yaw = absoluteAngleCal.nineYawPitch.at<float>(0, i);
                pitch = absoluteAngleCal.nineYawPitch.at<float>(1, i);
                stateCodeNum=codeNum[prcIndi];
                sameLabel++;
            }
        }
//            cout << "sameLabel " << sameLabel <<endl;

        //if same label
        if(sameLabel > 1) {
            double maxConfidence = 0;
            for(int  i = 0; i < 9; i ++) {
                if(buffNum[i] ==codeNum[prcIndi]) {
                    if(maxConfidence<quadsNine[i].confidence){
                        maxConfidence=quadsNine[i].confidence;
                        yaw = absoluteAngleCal.nineYawPitch.at<float>(0, i);
                        pitch = absoluteAngleCal.nineYawPitch.at<float>(1, i);
                        stateCodeNum=codeNum[prcIndi];
//                            cout<<"maxConfidence = "<<maxConfidence<<endl;
                    }
                }
            }
        }

        //如果调试之后发现整体足够快，直接取下一帧再看看也可以
        if(sameLabel==0) {
            int similarLabel = 0;
            bool correctLabel = 0;
            double minConfidence = 1;
            if(codeNum[prcIndi] == 1) {
                for(int  i = 0; i < 9; i ++){if(buffNum[i] ==7){similarLabel++;}}
                if(similarLabel>1){
                    for(int  i = 0; i < 9; i ++) {
                        if(buffNum[i] ==7) {
                            if(minConfidence<quadsNine[i].confidence){
                                minConfidence=quadsNine[i].confidence;
                                yaw = absoluteAngleCal.nineYawPitch.at<float>(0, i);
                                pitch = absoluteAngleCal.nineYawPitch.at<float>(1, i);
                                stateCodeNum=codeNum[prcIndi];
//                                    cout<<"minConfidence = "<<minConfidence<<endl;
                                correctLabel = 1;
                            }
                        }
                    }
                }
            }
            if(codeNum[prcIndi]==2) {
                for(int  i = 0; i < 9; i ++){if(buffNum[i] ==3){similarLabel++;}}
                if(similarLabel>1){
                    for(int  i = 0; i < 9; i ++) {
                        if(buffNum[i] ==3) {
                            if(minConfidence<quadsNine[i].confidence){
                                minConfidence=quadsNine[i].confidence;
                                yaw = absoluteAngleCal.nineYawPitch.at<float>(0, i);
                                pitch = absoluteAngleCal.nineYawPitch.at<float>(1, i);
                                stateCodeNum=codeNum[prcIndi];
//                                    cout<<"minConfidence = "<<minConfidence<<endl;
                                correctLabel=1;
                            }
                        }
                    }
                }
            }


            if(!correctLabel){
                for(int  i = 0; i < 9; i ++) {
                    if(minConfidence>quadsNine[i].confidence){
                        minConfidence=quadsNine[i].confidence;
                        yaw = absoluteAngleCal.nineYawPitch.at<float>(0, i);
                        pitch = absoluteAngleCal.nineYawPitch.at<float>(1, i);
                        stateCodeNum=codeNum[prcIndi];
//                            cout<<"minConfidence = "<<minConfidence<<endl;
                    }
                }
            }
        }

        return true;
    }
    bool BuffHiter::getFeedback(std::vector<Quad>& quadsTen, std::vector<double>& feedback, cv::Mat& feedbackImg){

        vector<Mat> imgTen(10);///加上仿射变换 变为正视图
        Mat plane_coordinate(4,1,CV_32FC2),src_coordinate(4,1,CV_32FC2);
        plane_coordinate.at<Point2f>(0, 0) = Point2f(0, 0);
        plane_coordinate.at<Point2f>(1, 0) = Point2f(140, 0);
        plane_coordinate.at<Point2f>(2, 0) = Point2f(140, 70);
        plane_coordinate.at<Point2f>(3, 0) = Point2f(0, 70);
            int iLowH1=151;
            int iHighH1=179;
            int iLowS1=90;
            int iHighS1=255;
            int iLowV1=90;
            int iHighV1=240;
            int iLowH2=70;
            int iHighH2=106;
            int iLowS2=90;
            int iHighS2=255;
            int iLowV2=90;
            int iHighV2=240;
#ifdef FEEDBACK_COLOR
            namedWindow("control", WINDOW_AUTOSIZE);
            cvCreateTrackbar("iLowH1","control",&iLowH1,179);
            cvCreateTrackbar("iHighH1","control",&iHighH1,179);
            cvCreateTrackbar("iLowS1","control",&iLowS1,255);
            cvCreateTrackbar("iHighS1","control",&iHighS1,255);
            cvCreateTrackbar("iLowV1","control",&iLowV1,255);
            cvCreateTrackbar("iHighV1","control",&iHighV1,255);
            cvCreateTrackbar("iLowH2","control",&iLowH2,179);
            cvCreateTrackbar("iHighH2","control",&iHighH2,179);
            cvCreateTrackbar("iLowS2","control",&iLowS2,255);
            cvCreateTrackbar("iHighS2","control",&iHighS2,255);
            cvCreateTrackbar("iLowV2","control",&iLowV2,255);
            cvCreateTrackbar("iHighV2","control",&iHighV2,255);
#endif FEEDBACK_COLOR
        for(int i=0; i < quadsTen.size();i++) {

            src_coordinate.at<Point2f>(0, 0) = quadsTen[i].up_left;
            src_coordinate.at<Point2f>(1, 0) = quadsTen[i].up_right;
            src_coordinate.at<Point2f>(2, 0) = quadsTen[i].down_right;
            src_coordinate.at<Point2f>(3, 0) = quadsTen[i].down_left;
            Mat trans_Perspective = getPerspectiveTransform(src_coordinate, plane_coordinate);
            warpPerspective(feedbackImg, imgTen[i], trans_Perspective, Size(140, 70));
            resize(imgTen[i], imgTen[i], cv::Size(20, 10));
            Mat imgHSV;
            vector<Mat> hsvSplit;
            cvtColor(imgTen[i], imgHSV, COLOR_BGR2HSV);
            split(imgHSV, hsvSplit);
            equalizeHist(hsvSplit[2],hsvSplit[2]);
            merge(hsvSplit,imgHSV);
            Mat imgThresholded,imgThresholded1,imgThresholded2;


            inRange(imgHSV, Scalar(iLowH1, iLowS1, iLowV1), Scalar(iHighH1, iHighS1, iHighV1), imgThresholded1);

            ostringstream feedback1,feedback2;
            feedback1<<"imgThresholded1  "<<i;
            feedback2<<"imgThresholded2  "<<i;

            //namedWindow(feedback1.str(), WINDOW_NORMAL);
//            imshow(feedback1.str(),imgThresholded1);
            inRange(imgHSV, Scalar(iLowH2, iLowS2, iLowV2), Scalar(iHighH2, iHighS2, iHighV2), imgThresholded2);
//            namedWindow(feedback2.str(), WINDOW_NORMAL);
//            imshow(feedback2.str(),imgThresholded2);
//            cout<<"i = " <<i<<endl;
//            waitKey(0);
            //imgThresholded=imgThresholded1+imgThresholded2;
//            Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
//            morphologyEx(imgThresholded1, imgThresholded1, MORPH_OPEN, element);
//            morphologyEx(imgThresholded1, imgThresholded1, MORPH_CLOSE, element);
//            morphologyEx(imgThresholded2, imgThresholded2, MORPH_OPEN, element);
//            morphologyEx(imgThresholded2, imgThresholded2, MORPH_CLOSE, element);
            double a=0,c=0;
            double b=200;
            int value1,value2;

            for (int j = 0; j < imgThresholded1.rows; ++j)
            {
                uchar* p1 = imgThresholded1.ptr<uchar>(j);
                uchar* p2 = imgThresholded2.ptr<uchar>(j);
                for (int k = 0; k < imgThresholded1.cols; ++k)
                {
                    value1=p1[k];
                    value2=p2[k];
                    //cout<<"p1 "<<value1<<endl;
                    // cout<<"p2 "<<value2<<endl;
                    if(p1[k]==255){
                        a++;
                    }

                    if(p2[k]==255){

                        c++;
                    }

                }
             }
             feedback[i]=max(a/b,c/b);

    }
    double c=0,d=0;
     for(int i=0;i<5;i++){
        c=c+feedback[i];
        d=d+feedback[i+5];
    }
    c=c/5;
    d=d/5;
    for(int i=1;i<5;i++){
        feedback[i]=feedback[i]-feedback[0];
        feedback[i+5]=feedback[i+5]-feedback[5];
    }

    cout<<"-------------------------"<<endl;
    cout<<feedback[0]<<"\t"<<feedback[5]<<endl;
    cout<<feedback[1]<<"\t"<<feedback[6]<<endl;
    cout<<feedback[2]<<"\t"<<feedback[7]<<endl;
    cout<<feedback[3]<<"\t"<<feedback[8]<<endl;
    cout<<feedback[4]<<"\t"<<feedback[9]<<endl;
    area<<"-------------------------"<<endl;
    area<<feedback[0]<<"\t"<<feedback[5]<<endl;
    area<<feedback[1]<<"\t"<<feedback[6]<<endl;
    area<<feedback[2]<<"\t"<<feedback[7]<<endl;
    area<<feedback[3]<<"\t"<<feedback[8]<<endl;
    area<<feedback[4]<<"\t"<<feedback[9]<<endl;
    area<<"-------------------------"<<endl;
    out<<feedback[0]<<"\t"<<feedback[5]<<endl;
    out<<feedback[1]<<"\t"<<feedback[6]<<endl;
    out<<feedback[2]<<"\t"<<feedback[7]<<endl;
    out<<feedback[3]<<"\t"<<feedback[8]<<endl;
    out<<feedback[4]<<"\t"<<feedback[9]<<endl;
       return true;
    }

    bool BuffHiter::showHitNum(cv::Mat& image){
        stringstream s1,s2,s3;
        s1 << "yaw = " << yaw;
        s2 <<"pitch = " << pitch;
        s3 <<"hitnum = "<<codeNum[prcIndi];
        String num4(s1.str());
        putText(image, num4,Point2f(210,275), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0));
        String num5(s2.str());
        putText(image, num5,Point2f(210,290), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0));
        String num6(s3.str());
        putText(image, num6,Point2f(210,305), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0));
        imshow("calAngleImage",image);
    }
    bool BuffHiter::run(cv::Mat& frame, int hitBuffFlag){

        resize(frame,frame,Size(0,0),BuffParam::resizePara,BuffParam::resizePara);
        //hitSmallBuff
        cout<<"Master: prcIndi before every run is "<<prcIndi<<endl;
        if (hitBuffFlag == 2){
            cout << "--------------Hit SmallBuff-------------" << endl;
            if (!hitSmallBuff(frame)) {
                cout << "Master: hit smallBuff failed!!!" << endl;
                saveTime = 1;
                return false;
            }
        }
        else if(hitBuffFlag == 3){
            cout << "---------------Hit BigBuff--------------" << endl;
            if (!hitBigBuff(frame)) {
                cout << "Master: hit bigBuff failed!!! " << endl;
                saveTime = 1;
                return false;
            }
        }else {
            cout << "Master: hitBuffFlag is wrong!!!" << endl;
            return false;
        }
        return true;
    }

    //赛场上图像采集函数
    bool BuffHiter::recordBuffImg(cv::Mat BuffImg) {
        if(BuffImg.empty()) return false;
        frameSavePath = "/home/infantry/Infantry/data/buff/buff_img";
        VideoWriter writer("/home/infantry/Infantry/data/buff/buff_img/buff_1.avi", CV_FOURCC('M', 'J', 'P', 'G'), 30, Size(1920,1080));
        writer << BuffImg;

        ostringstream fileName;
        fileName << cntFrameBuffRecord << ".png";
        if(cntFrameBuffRecord < 1000)
            cntFrameBuffRecord ++;
        else
            cntFrameBuffRecord = 0;
        imwrite(frameSavePath + fileName.str(), BuffImg);
        return true;
    }

}

