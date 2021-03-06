#include<algorithm>
#include<array>
#include<stdlib.h>
#include<string>
#include<bitset>

#include"robot.h"
#include"plan.h"
#include"kinematics.h"

double input_angle[18] = { 0 };
double init_pos_angle[18] = { 0 };

//输出参数，模型曲线测试使用
double file_current_leg[18] = { 0 };
double file_current_body[16] = { 0 };
double time_test = 0;
extern double PI;

using namespace aris::dynamic;
using namespace aris::plan;

namespace robot
{
    //----------------------------读取仿真数据--------------------------//
    auto HexRead::prepareNrt()->void
    {
        turn_angle_ = doubleParam("angle");
        for (auto& m : motorOptions()) m = aris::plan::Plan::NOT_CHECK_ENABLE;
    }
    auto HexRead::executeRT()->int
    {
        if (count() == 1)this->master()->logFileRawName("eeTraj");
        int ret = 0;
        if (count() == 1)
        {
            input_angle[0] = controller()->motionPool()[0].actualPos();
            input_angle[1] = controller()->motionPool()[1].actualPos();
            input_angle[2] = controller()->motionPool()[2].actualPos();
            input_angle[3] = controller()->motionPool()[3].actualPos();
            input_angle[4] = controller()->motionPool()[4].actualPos();
            input_angle[5] = controller()->motionPool()[5].actualPos();
            input_angle[6] = controller()->motionPool()[6].actualPos();
            input_angle[7] = controller()->motionPool()[7].actualPos();
            input_angle[8] = controller()->motionPool()[8].actualPos();
            input_angle[9] = controller()->motionPool()[9].actualPos();
            input_angle[10] = controller()->motionPool()[10].actualPos();
            input_angle[11] = controller()->motionPool()[11].actualPos();
        }

        TCurve s1(5, 2);
        s1.getCurveParam();
        EllipseTrajectory e1(0, 0, 0, s1);



        //输出角度，用于仿真测试
        {
            //输出电机角度
            for (int i = 0; i < 12; ++i)
            {
                lout() << input_angle[i] << "\t";
            }
            time_test += 0.001;
            lout() << time_test << "\t";

            //输出身体和足尖曲线
            for (int i = 0; i < 12; ++i)
            {
                lout() << file_current_leg[i] << "\t";
            }
            lout() << file_current_body[3] << "\t" << file_current_body[7] << "\t" << file_current_body[11] << std::endl;
        }
        //发送电机角度
        for (int i = 0; i < 12; ++i)
        {
            if (i == 2 || i == 5 || i == 8 || i == 11)
                controller()->motionPool()[i].setTargetPos(1.5 * input_angle[i]);
            else
                controller()->motionPool()[i].setTargetPos(input_angle[i]);
        }
        return ret;
    }
    HexRead::HexRead(const std::string& name)
    {
        aris::core::fromXmlString(command(),
            "<Command name=\"hexpod\"/>");
    }
    HexRead::~HexRead() = default;
	//---------------------------cpp和Adams调试------------------------//
	
    //前进
    auto HexDynamicForwardTest::prepareNrt()->void
	{

	}
	auto HexDynamicForwardTest::executeRT()->int
	{
  //      //if (count() == 1)this->master()->logFileRawName("eeTraj");
  //      //if (count() == 1)this->master()->logFileRawName("inputTraj");
  //      //if (count() == 1)this->master()->logFileRawName("invInput");
  //      //if (count() == 1)this->master()->logFileRawName("numInput");

		int ret = 0,a=500;
        //末端为六个末端的三个坐标和身体的位姿矩阵 3*6+16=34
		static double ee0[34];  
		double ee[34];
        if (count() <= a)
        {
            ret = 1;
            if (count() == 1)
            {
                model()->getOutputPos(ee0);
                aris::dynamic::s_vc(34, ee0, ee);
            }
            aris::dynamic::s_vc(34, ee0, ee);
            model()->setOutputPos(ee);

          
            if (model()->inverseKinematics()) std::cout << "inverse failed " << std::endl;
            model()->setTime(0.001 * count());
        }
        else
        {
            TCurve s1(1, 1);
            s1.getCurveParam();
            EllipseTrajectory e1(0.1, 0.03, 0, s1);
            BodyPose body_s(0, 0, 0, s1);
           
            
            ret = tripodPlan(5, count() - 1-a, &e1, input_angle);
            aris::dynamic::s_vc(16, file_current_body + 0, ee + 0);
            aris::dynamic::s_vc(18, file_current_leg + 0, ee + 16);
            //末端位置
            //for (int i = 0; i < 34; ++i)
            //    lout() << ee[i] << "\t";
            //lout() << std::endl;

            //解析解计算得到的输入的角度
            //for (int i = 0; i < 18; ++i)
            //    lout() << input_angle[i] << "\t";
            //lout() << std::endl;

            model()->setOutputPos(ee);
            
            if (model()->inverseKinematics())
            {
                std::cout << "inverse failed!!!" << std::endl;
                //for (int i = 0; i < 34; ++i) {
                //    std::cout << ee[i] << std::endl;
                //}
                std::cout << "ret = " << ret << std::endl;
            }
            //数值解计算得到的输入的角度
            //double input[18];
            //model()->getInputPos(input);
            //for (int i = 0; i < 18; ++i)
            //    lout() << input[i] << "\t";
            //lout() << std::endl;

            model()->setTime(0.001 * count());
           
              
            if (ret == 0) std::cout << count() << std::endl;
            
        }
        return ret;
    
	}
    HexDynamicForwardTest::HexDynamicForwardTest(const std::string& name)
    {
        aris::core::fromXmlString(command(),
            "<Command name=\"hex_forward\"/>");
    }
    HexDynamicForwardTest::~HexDynamicForwardTest() = default;


    //后退
    auto HexDynamicBackTest::prepareNrt()->void
    {

    }
    auto HexDynamicBackTest::executeRT()->int
    {
        // if (count() == 1)this->master()->logFileRawName("eeTraj");
        int ret = 0;
        //末端为六个末端的三个坐标和身体的位姿矩阵 3*6+16=34
        static double ee0[34];
        double ee[34];

        
            TCurve s1(5, 2);
            s1.getCurveParam();
            EllipseTrajectory e1(-0.15,0.05, 0, s1);
            BodyPose body_s(0, 0, 0, s1);
            if (count() == 1)
            {
                model()->getOutputPos(ee0);
                aris::dynamic::s_vc(34, ee0, ee); //给ee设置ee0的初值
            }

            ret = tripodPlan(5, count() - 1, &e1, input_angle);
            aris::dynamic::s_vc(16, file_current_body + 0, ee + 0);
            aris::dynamic::s_vc(18, file_current_leg + 0, ee + 16);
            //for (int i = 16; i < 34; ++i)
            //    lout() << ee[i] << "\t";
            //lout() << std::endl;

            model()->setOutputPos(ee);
            if (model()->inverseKinematics())
            {
                std::cout << "inverse failed!!!" << std::endl;
                //for (int i = 0; i < 34; ++i) {
                //    std::cout << ee[i] << std::endl;
                //}
                std::cout << "ret = " << ret << std::endl;
            }


            model()->setTime(0.001 * count());
            if (ret == 1)
                //    std::cout << s1.getTc() * 1000 << std::endl;
                if (ret == 0) std::cout << count() << std::endl;
            return ret;
        
    }
    HexDynamicBackTest::HexDynamicBackTest(const std::string& name)
    {
        aris::core::fromXmlString(command(),
            "<Command name=\"hex_back\"/>");
    }
    HexDynamicBackTest::~HexDynamicBackTest() = default;

    //右移
    auto HexDynamicRightTest::prepareNrt()->void
    {

    }
    auto HexDynamicRightTest::executeRT()->int
    {
        // if (count() == 1)this->master()->logFileRawName("eeTraj");
        int ret = 0;
        //末端为六个末端的三个坐标和身体的位姿矩阵 3*6+16=34
        static double ee0[34];
        double ee[34];

        {
            TCurve s1(5, 2);
            s1.getCurveParam();
            EllipseTrajectory e1(0, 0.05, 0.1, s1);
            BodyPose body_s(0, 0, 0, s1);
            if (count() == 1)
            {
                model()->getOutputPos(ee0);
                aris::dynamic::s_vc(34, ee0, ee); //给ee设置ee0的初值
            }

            ret = tripodPlan(3, count() - 1, &e1, input_angle);
            aris::dynamic::s_vc(16, file_current_body + 0, ee + 0);
            aris::dynamic::s_vc(18, file_current_leg + 0, ee + 16);
            //for (int i = 16; i < 34; ++i)
            //    lout() << ee[i] << "\t";
            //lout() << std::endl;

            model()->setOutputPos(ee);
            if (model()->inverseKinematics())
            {
                std::cout << "inverse failed!!!" << std::endl;
                //for (int i = 0; i < 34; ++i) {
                //    std::cout << ee[i] << std::endl;
                //}
                std::cout << "ret = " << ret << std::endl;
            }


            model()->setTime(0.001 * count());
            if (ret == 1)
                //    std::cout << s1.getTc() * 1000 << std::endl;
                if (ret == 0) std::cout << count() << std::endl;
            return ret;
        }
    }
    HexDynamicRightTest::HexDynamicRightTest(const std::string& name)
    {
        aris::core::fromXmlString(command(),
            "<Command name=\"hex_right\"/>");
    }
    HexDynamicRightTest::~HexDynamicRightTest() = default;



    //左移
    auto HexDynamicLeftTest::prepareNrt()->void
    {

    }
    auto HexDynamicLeftTest::executeRT()->int
    {
        // if (count() == 1)this->master()->logFileRawName("eeTraj");
        int ret = 0;
        //末端为六个末端的三个坐标和身体的位姿矩阵 3*6+16=34
        static double ee0[34];
        double ee[34];

        {
            TCurve s1(5, 2);
            s1.getCurveParam();
            EllipseTrajectory e1(0, 0.05, -0.1, s1);
            BodyPose body_s(0, 0, 0, s1);
            if (count() == 1)
            {
                model()->getOutputPos(ee0);
                aris::dynamic::s_vc(34, ee0, ee); //给ee设置ee0的初值
            }

            ret = tripodPlan(3, count() - 1, &e1, input_angle);
            aris::dynamic::s_vc(16, file_current_body + 0, ee + 0);
            aris::dynamic::s_vc(18, file_current_leg + 0, ee + 16);
            //for (int i = 16; i < 34; ++i)
            //    lout() << ee[i] << "\t";
            //lout() << std::endl;

            model()->setOutputPos(ee);
            if (model()->inverseKinematics())
            {
                std::cout << "inverse failed!!!" << std::endl;
                //for (int i = 0; i < 34; ++i) {
                //    std::cout << ee[i] << std::endl;
                //}
                std::cout << "ret = " << ret << std::endl;
            }


            model()->setTime(0.001 * count());
            if (ret == 1)
                //    std::cout << s1.getTc() * 1000 << std::endl;
                if (ret == 0) std::cout << count() << std::endl;
            return ret;
        }
    }
    HexDynamicLeftTest::HexDynamicLeftTest(const std::string& name)
    {
        aris::core::fromXmlString(command(),
            "<Command name=\"hex_left\"/>");
    }
    HexDynamicLeftTest::~HexDynamicLeftTest() = default;


    //右转
    auto HexDynamicTurnRightTest::prepareNrt()->void
    {

    }
    auto HexDynamicTurnRightTest::executeRT()->int
    {
        // if (count() == 1)this->master()->logFileRawName("eeTraj");
        int ret = 0;
        //末端为六个末端的三个坐标和身体的位姿矩阵 3*6+16=34
        static double ee0[34];
        double ee[34];

        {
            TCurve s1(5, 2);
            s1.getCurveParam();
            EllipseTrajectory e1(0, 0.05, 0, s1);
            BodyPose body_s(0, -20, 0, s1);
            if (count() == 1)
            {
                model()->getOutputPos(ee0);
                aris::dynamic::s_vc(34, ee0, ee); //给ee设置ee0的初值
            }

            ret = turnPlanTripod(5, count() - 1, &e1, &body_s, input_angle);
            aris::dynamic::s_vc(16, file_current_body + 0, ee + 0);
            aris::dynamic::s_vc(18, file_current_leg + 0, ee + 16);
            //for (int i = 16; i < 34; ++i)
            //    lout() << ee[i] << "\t";
            //lout() << std::endl;

            model()->setOutputPos(ee);
            if (model()->inverseKinematics())
            {
                std::cout << "inverse failed!!!" << std::endl;
                //for (int i = 0; i < 34; ++i) {
                //    std::cout << ee[i] << std::endl;
                //}
                std::cout << "ret = " << ret << std::endl;
            }


            model()->setTime(0.001 * count());
            if (ret == 1)
                //    std::cout << s1.getTc() * 1000 << std::endl;
                if (ret == 0) std::cout << count() << std::endl;
            return ret;
        }
    }
    HexDynamicTurnRightTest::HexDynamicTurnRightTest(const std::string& name)
    {
        aris::core::fromXmlString(command(),
            "<Command name=\"hex_turn_right\"/>");
    }
    HexDynamicTurnRightTest::~HexDynamicTurnRightTest() = default;

    //左转
    auto HexDynamicTurnLeftTest::prepareNrt()->void
    {

    }
    auto HexDynamicTurnLeftTest::executeRT()->int
    {
        // if (count() == 1)this->master()->logFileRawName("eeTraj");
        int ret = 0;
        //末端为六个末端的三个坐标和身体的位姿矩阵 3*6+16=34
        static double ee0[34];
        double ee[34];

        {
            TCurve s1(5, 2);
            s1.getCurveParam();
            EllipseTrajectory e1(0, 0.05, 0, s1);
            BodyPose body_s(0, 20, 0, s1);
            if (count() == 1)
            {
                model()->getOutputPos(ee0);
                aris::dynamic::s_vc(34, ee0, ee); //给ee设置ee0的初值
            }

            ret = turnPlanTripod(5, count() - 1, &e1, &body_s, input_angle);
            aris::dynamic::s_vc(16, file_current_body + 0, ee + 0);
            aris::dynamic::s_vc(18, file_current_leg + 0, ee + 16);
            //for (int i = 16; i < 34; ++i)
            //    lout() << ee[i] << "\t";
            //lout() << std::endl;

            model()->setOutputPos(ee);
            if (model()->inverseKinematics())
            {
                std::cout << "inverse failed!!!" << std::endl;
                //for (int i = 0; i < 34; ++i) {
                //    std::cout << ee[i] << std::endl;
                //}
                std::cout << "ret = " << ret << std::endl;
            }


            model()->setTime(0.001 * count());
            if (ret == 1)
                //    std::cout << s1.getTc() * 1000 << std::endl;
                if (ret == 0) std::cout << count() << std::endl;
            return ret;
        }
    }
    HexDynamicTurnLeftTest::HexDynamicTurnLeftTest(const std::string& name)
    {
        aris::core::fromXmlString(command(),
            "<Command name=\"hex_turn_right\"/>");
    }
    HexDynamicTurnLeftTest::~HexDynamicTurnLeftTest() = default;

    //四足步态
    auto HexDynamicTetrapodTest::prepareNrt()->void
    {

    }
    auto HexDynamicTetrapodTest::executeRT()->int
    {
         //if (count() == 1)this->master()->logFileRawName("eeTraj");
        int ret = 0, a = 500;
        //末端为六个末端的三个坐标和身体的位姿矩阵 3*6+16=34
        static double ee0[34];
        double ee[34];
        if (count() <= a)
        {
            ret = 1;
            if (count() == 1)
            {
                model()->getOutputPos(ee0);
                aris::dynamic::s_vc(34, ee0, ee);
            }
            aris::dynamic::s_vc(34, ee0, ee);
            model()->setOutputPos(ee);
            if (model()->inverseKinematics()) std::cout << "inverse failed " << std::endl;
            model()->setTime(0.001 * count());
        }
        else
        {
            TCurve s1(1, 1);
            s1.getCurveParam();
            EllipseTrajectory e1(0.15, 0.05, 0, s1);
            BodyPose body_s(0, 0, 0, s1);

            //if (count() == 1)
            //{
            //    model()->getOutputPos(ee0); 
            //    aris::dynamic::s_vc(34, ee0, ee); //给ee设置ee0的初值
            //}

            ret = tetrapodPlan(5, count() - 1 - a, &e1, input_angle);
            aris::dynamic::s_vc(16, file_current_body + 0, ee + 0);
            aris::dynamic::s_vc(18, file_current_leg + 0, ee + 16);
            for (int i = 0; i < 34; ++i)
                lout() << ee[i] << "\t";
            lout() << std::endl;

            //model()->setOutputPos(ee);
            //if (model()->inverseKinematics())
            //{
            //    std::cout << "inverse failed!!!" << std::endl;
            //    //for (int i = 0; i < 34; ++i) {
            //    //    std::cout << ee[i] << std::endl;
            //    //}
            //    std::cout << "ret = " << ret << std::endl;
            //}


            model()->setTime(0.001 * count());


            if (ret == 0) std::cout << count() << std::endl;
            return ret;
        }
    }
    HexDynamicTetrapodTest::HexDynamicTetrapodTest(const std::string& name)
    {
        aris::core::fromXmlString(command(),
            "<Command name=\"hex_tetrapod\"/>");
    }
    HexDynamicTetrapodTest::~HexDynamicTetrapodTest() = default;






    auto createModelHexapod()->std::unique_ptr<aris::dynamic::Model>
    {
        std::unique_ptr<aris::dynamic::Model> hex = std::make_unique<aris::dynamic::Model>();
        // set gravity //
        const double gravity[6]{ 0.0,-9.8,0.0,0.0,0.0,0.0 };
        hex->environment().setGravity(gravity);
        
        //define joint pos //
        //此处都是在初始位置下测量,坐标系朝向与Adams一致
        const double leg1_pe[10][3]{
            {FRONTX,    0,          0}, //转轴与底板地面交点，第一个     0
            {0.368,     0.088,      0}, //y方向推杆，距离为电机支撑座下底面圆心到中心的距离 移动副  1
            {0.368,     0.012,      0}, //推杆与曲柄连接的交点    B点   2
           // {0.332,     0,      0}, //Y向导轨与原点的距离，为移动副
            {0.348,     -0.032,     0}, //A点         3
            {0.49931,   0.07045,    0}, //C点  4
            {0.52379,   -0.02675,   0}, //D点  5
            {0.41987,   -0.09621,   0}, //F点  6  
            {0.3676,    -0.12872,   0}, //G点 注意G点应该有两个转动副   7
          //  {0.3676,    0,      -0.12872}, //G点
            {0.3427,    -0.12133,   0}, //H点   8
            {0.286,     -0.10345,   0}, //X向推杆，距离为腿壳的里面圆心到中心的距离 移动副   9

        };

        const double leg2_pe[10][3]{
            {0.1500,    0,          -0.2598},
            {0.1840,    0.0880,     -0.3187},
            {0.1840,    0.0120,     -0.3187},
            {0.1740,    -0.0320,    -0.3014},
            {0.2497,    0.0704,     -0.4324},
            {0.2619,    -0.0267,    -0.4536},
            {0.2099,    -0.0962,    -0.3636},
            {0.1838,    -0.1287,    -0.3184},
            {0.1714,    -0.1213,    -0.2968},
            {0.1430,    -0.1035,    -0.2477},
        }; 

        const double leg3_pe[10][3]{
            {-0.1500,    0,          -0.2598},
            {-0.1840,    0.0880,     -0.3187},
            {-0.1840,    0.0120,     -0.3187},
            {-0.1740,    -0.0320,    -0.3014},
            {-0.2497,    0.0704,     -0.4324},
            {-0.2619,    -0.0267,    -0.4536},
            {-0.2099,    -0.0962,    -0.3636},
            {-0.1838,    -0.1287,    -0.3184},
            {-0.1714,    -0.1213,    -0.2968},
            {-0.1430,    -0.1035,    -0.2477},
        };

        const double leg4_pe[10][3]{
            {-FRONTX,    0,          0}, //转轴与底板地面交点，第一个     0
            {-0.368,     0.088,      0}, //y方向推杆，距离为电机支撑座下底面圆心到中心的距离 移动副  1
            {-0.368,     0.012,      0}, //推杆与曲柄连接的交点    B点   2
           // {0.332,     0,      0}, //Y向导轨与原点的距离，为移动副
            {-0.348,     -0.032,     0}, //A点         3
            {-0.49931,   0.07045,    0}, //C点  4
            {-0.52379,   -0.02675,   0}, //D点  5
            {-0.41987,   -0.09621,   0}, //F点  6  
            {-0.3676,    -0.12872,   0}, //G点 注意G点应该有两个转动副   7
          //  {0.3676,    0,      -0.12872}, //G点
            {-0.3427,    -0.12133,   0}, //H点   8
            {-0.286,     -0.10345,   0}, //X向推杆，距离为腿壳的里面圆心到中心的距离 移动副   9

        };

        const double leg5_pe[10][3]{
            {-0.1500,    0,          0.2598},
            {-0.1840,    0.0880,     0.3187},
            {-0.1840,    0.0120,     0.3187},
            {-0.1740,    -0.0320,    0.3014},
            {-0.2497,    0.0704,     0.4324},
            {-0.2619,    -0.0267,    0.4536},
            {-0.2099,    -0.0962,    0.3636},
            {-0.1838,    -0.1287,    0.3184},
            {-0.1714,    -0.1213,    0.2968},
            {-0.1430,    -0.1035,    0.2477},
        };

        const double leg6_pe[10][3]{
            {0.1500,    0,          0.2598},
            {0.1840,    0.0880,     0.3187},
            {0.1840,    0.0120,     0.3187},
            {0.1740,    -0.0320,    0.3014},
            {0.2497,    0.0704,     0.4324},
            {0.2619,    -0.0267,    0.4536},
            {0.2099,    -0.0962,    0.3636},
            {0.1838,    -0.1287,    0.3184},
            {0.1714,    -0.1213,    0.2968},
            {0.1430,    -0.1035,    0.2477},
        };
        
        //define ee pos  六条腿的末端//
        const double ee_pos[6][6]
        {
            {EE1_X,         -HEIGHT,        0.0,		    0.0,    0.0,    0.0},
            {EE6_X,	        -HEIGHT,        -EE6_Z,		    0.0,    0.0,    0.0},
            {-EE6_X,        -HEIGHT,        -EE6_Z,		    0.0,    0.0,    0.0},
            {-EE1_X,        -HEIGHT,        0,			    0.0,    0.0,    0.0},
            {-EE6_X,        -HEIGHT,        EE6_Z,		    0.0,    0.0,    0.0},
            {EE6_X,	        -HEIGHT,        EE6_Z,		    0.0,    0.0,    0.0},
        };

        //iv:  10x1 惯量矩阵向量[m, cx, cy, cz, Ixx, Iyy, Izz, Ixy, Ixz, Iyz]
        const double body_iv[10]{ 10.618026,0,0,0,0.355566,0.355183,0.457806,0.000016,0.000036,0.000009 };
        //每条腿都在自己坐标系下，故惯性张量一样，不作区分，单腿重4.208793kg  总重35.872758kg，还有部分组件没有考虑上，估计在50kg左右
        const double leg_shell_iv[10]{2.792555,0,0,0,0.042320,0.008467,0.048366,0.011936,0.000001,0.000001};
        const double y_screw_iv[10]{0.169451,0,0,0,0.000258,0.000042,0.000293,0.000052,0.00,0.00};
        const double top_bar_iv[10]{0.195471,0,0,0,0.000350,0.000496,0.000576,0.000259,0.000001,0.000001};
        const double longest_bar_iv[10]{0.533153,0,0,0,0.008985,0.001777,0.010588,0.003834,0.000001,0.000001};
        const double bot_bar_iv[10]{0.113909,0,0,0,0.000182,0.000310,0.000471,0.000223,0.0,0.0};
        const double crank_iv[10]{0.110257,0,0,0,0.000115,0.000070,0.000175,0.000070,0.0,0.0};
        const double h_bar_iv[10]{0.118374,0,0,0,0.000110,0.000054,0.000093,0.000029,0.0,0.0};
        const double shortest_bar_iv[10]{0.028151,0,0,0,0.000006,0.000009,0.000007,0,0,0};
        const double x_screw_iv[10]{0.147472,0,0,0,0.000015,0.000187,0.000196,0.000011,0.0,0.0};
       
        //add part //
        auto& body = hex->partPool().add<aris::dynamic::Part>("BODY", body_iv);
        //leg1
        auto& leg1_leg_shell = hex->partPool().add<aris::dynamic::Part>("leg1_leg_shell", leg_shell_iv);
        auto& leg1_y_screw = hex->partPool().add<aris::dynamic::Part>("leg1_y_screw", y_screw_iv);
        auto& leg1_top_bar = hex->partPool().add<aris::dynamic::Part>("leg1_top_bar", top_bar_iv);
        auto& leg1_longest_bar = hex->partPool().add<aris::dynamic::Part>("leg1_longest_bar", longest_bar_iv);
        auto& leg1_bot_bar = hex->partPool().add<aris::dynamic::Part>("leg1_bot_bar", bot_bar_iv);
        auto& leg1_crank = hex->partPool().add<aris::dynamic::Part>("leg1_crank", crank_iv);
        auto& leg1_h_bar = hex->partPool().add<aris::dynamic::Part>("leg1_h_bar", h_bar_iv);
        auto& leg1_shortest_bar = hex->partPool().add<aris::dynamic::Part>("leg1_shortest_bar", shortest_bar_iv);
        auto& leg1_x_screw = hex->partPool().add<aris::dynamic::Part>("leg1_x_screw", x_screw_iv);
        //leg2
        auto& leg2_leg_shell = hex->partPool().add<aris::dynamic::Part>("leg2_leg_shell", leg_shell_iv);
        auto& leg2_y_screw = hex->partPool().add<aris::dynamic::Part>("leg2_y_screw", y_screw_iv);
        auto& leg2_top_bar = hex->partPool().add<aris::dynamic::Part>("leg2_top_bar", top_bar_iv);
        auto& leg2_longest_bar = hex->partPool().add<aris::dynamic::Part>("leg2_longest_bar", longest_bar_iv);
        auto& leg2_bot_bar = hex->partPool().add<aris::dynamic::Part>("leg2_bot_bar", bot_bar_iv);
        auto& leg2_crank = hex->partPool().add<aris::dynamic::Part>("leg2_crank", crank_iv);
        auto& leg2_h_bar = hex->partPool().add<aris::dynamic::Part>("leg2_h_bar", h_bar_iv);
        auto& leg2_shortest_bar = hex->partPool().add<aris::dynamic::Part>("leg2_shortest_bar", shortest_bar_iv);
        auto& leg2_x_screw = hex->partPool().add<aris::dynamic::Part>("leg2_x_screw", x_screw_iv);
        //leg3
        auto& leg3_leg_shell = hex->partPool().add<aris::dynamic::Part>("leg3_leg_shell", leg_shell_iv);
        auto& leg3_y_screw = hex->partPool().add<aris::dynamic::Part>("leg3_y_screw", y_screw_iv);
        auto& leg3_top_bar = hex->partPool().add<aris::dynamic::Part>("leg3_top_bar", top_bar_iv);
        auto& leg3_longest_bar = hex->partPool().add<aris::dynamic::Part>("leg3_longest_bar", longest_bar_iv);
        auto& leg3_bot_bar = hex->partPool().add<aris::dynamic::Part>("leg3_bot_bar", bot_bar_iv);
        auto& leg3_crank = hex->partPool().add<aris::dynamic::Part>("leg3_crank", crank_iv);
        auto& leg3_h_bar = hex->partPool().add<aris::dynamic::Part>("leg3_h_bar", h_bar_iv);
        auto& leg3_shortest_bar = hex->partPool().add<aris::dynamic::Part>("leg3_shortest_bar", shortest_bar_iv);
        auto& leg3_x_screw = hex->partPool().add<aris::dynamic::Part>("leg3_x_screw", x_screw_iv);
        //leg4
        auto& leg4_leg_shell = hex->partPool().add<aris::dynamic::Part>("leg4_leg_shell", leg_shell_iv);
        auto& leg4_y_screw = hex->partPool().add<aris::dynamic::Part>("leg4_y_screw", y_screw_iv);
        auto& leg4_top_bar = hex->partPool().add<aris::dynamic::Part>("leg4_top_bar", top_bar_iv);
        auto& leg4_longest_bar = hex->partPool().add<aris::dynamic::Part>("leg4_longest_bar", longest_bar_iv);
        auto& leg4_bot_bar = hex->partPool().add<aris::dynamic::Part>("leg4_bot_bar", bot_bar_iv);
        auto& leg4_crank = hex->partPool().add<aris::dynamic::Part>("leg4_crank", crank_iv);
        auto& leg4_h_bar = hex->partPool().add<aris::dynamic::Part>("leg4_h_bar", h_bar_iv);
        auto& leg4_shortest_bar = hex->partPool().add<aris::dynamic::Part>("leg4_shortest_bar", shortest_bar_iv);
        auto& leg4_x_screw = hex->partPool().add<aris::dynamic::Part>("leg4_x_screw", x_screw_iv);
        //leg5
        auto& leg5_leg_shell = hex->partPool().add<aris::dynamic::Part>("leg5_leg_shell", leg_shell_iv);
        auto& leg5_y_screw = hex->partPool().add<aris::dynamic::Part>("leg5_y_screw", y_screw_iv);
        auto& leg5_top_bar = hex->partPool().add<aris::dynamic::Part>("leg5_top_bar", top_bar_iv);
        auto& leg5_longest_bar = hex->partPool().add<aris::dynamic::Part>("leg5_longest_bar", longest_bar_iv);
        auto& leg5_bot_bar = hex->partPool().add<aris::dynamic::Part>("leg5_bot_bar", bot_bar_iv);
        auto& leg5_crank = hex->partPool().add<aris::dynamic::Part>("leg5_crank", crank_iv);
        auto& leg5_h_bar = hex->partPool().add<aris::dynamic::Part>("leg5_h_bar", h_bar_iv);
        auto& leg5_shortest_bar = hex->partPool().add<aris::dynamic::Part>("leg5_shortest_bar", shortest_bar_iv);
        auto& leg5_x_screw = hex->partPool().add<aris::dynamic::Part>("leg5_x_screw", x_screw_iv);
        //leg6
        auto& leg6_leg_shell = hex->partPool().add<aris::dynamic::Part>("leg6_leg_shell", leg_shell_iv);
        auto& leg6_y_screw = hex->partPool().add<aris::dynamic::Part>("leg6_y_screw", y_screw_iv);
        auto& leg6_top_bar = hex->partPool().add<aris::dynamic::Part>("leg6_top_bar", top_bar_iv);
        auto& leg6_longest_bar = hex->partPool().add<aris::dynamic::Part>("leg6_longest_bar", longest_bar_iv);
        auto& leg6_bot_bar = hex->partPool().add<aris::dynamic::Part>("leg6_bot_bar", bot_bar_iv);
        auto& leg6_crank = hex->partPool().add<aris::dynamic::Part>("leg6_crank", crank_iv);
        auto& leg6_h_bar = hex->partPool().add<aris::dynamic::Part>("leg6_h_bar", h_bar_iv);
        auto& leg6_shortest_bar = hex->partPool().add<aris::dynamic::Part>("leg6_shortest_bar", shortest_bar_iv);
        auto& leg6_x_screw = hex->partPool().add<aris::dynamic::Part>("leg6_x_screw", x_screw_iv);



        //add geometry //
        hex->ground().geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\ground2.x_t");
        body.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\body.x_t");
        // leg1
        leg1_leg_shell.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg1_leg_shell.x_t");
        leg1_y_screw.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg1_y_screw.x_t");
        leg1_top_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg1_top_bar.x_t");
        leg1_longest_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg1_longest_bar.x_t");
        leg1_bot_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg1_bot_bar.x_t");
        leg1_crank.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg1_crank.x_t");
        leg1_h_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg1_h_bar.x_t");
        leg1_shortest_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg1_shortest_bar.x_t");
        leg1_x_screw.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg1_x_screw.x_t");
        // leg2
        leg2_leg_shell.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg2_leg_shell.x_t");
        leg2_y_screw.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg2_y_screw.x_t");
        leg2_top_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg2_top_bar.x_t");
        leg2_longest_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg2_longest_bar.x_t");
        leg2_bot_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg2_bot_bar.x_t");
        leg2_crank.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg2_crank.x_t");
        leg2_h_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg2_h_bar.x_t");
        leg2_shortest_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg2_shortest_bar.x_t");
        leg2_x_screw.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg2_x_screw.x_t");
        // leg3
        leg3_leg_shell.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg3_leg_shell.x_t");
        leg3_y_screw.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg3_y_screw.x_t");
        leg3_top_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg3_top_bar.x_t");
        leg3_longest_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg3_longest_bar.x_t");
        leg3_bot_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg3_bot_bar.x_t");
        leg3_crank.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg3_crank.x_t");
        leg3_h_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg3_h_bar.x_t");
        leg3_shortest_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg3_shortest_bar.x_t");
        leg3_x_screw.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg3_x_screw.x_t");
        // leg4
        leg4_leg_shell.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg4_leg_shell.x_t");
        leg4_y_screw.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg4_y_screw.x_t");
        leg4_top_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg4_top_bar.x_t");
        leg4_longest_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg4_longest_bar.x_t");
        leg4_bot_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg4_bot_bar.x_t");
        leg4_crank.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg4_crank.x_t");
        leg4_h_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg4_h_bar.x_t");
        leg4_shortest_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg4_shortest_bar.x_t");
        leg4_x_screw.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg4_x_screw.x_t");
        // leg5
        leg5_leg_shell.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg5_leg_shell.x_t");
        leg5_y_screw.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg5_y_screw.x_t");
        leg5_top_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg5_top_bar.x_t");
        leg5_longest_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg5_longest_bar.x_t");
        leg5_bot_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg5_bot_bar.x_t");
        leg5_crank.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg5_crank.x_t");
        leg5_h_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg5_h_bar.x_t");
        leg5_shortest_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg5_shortest_bar.x_t");
        leg5_x_screw.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg5_x_screw.x_t");
        // leg6
        leg6_leg_shell.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg6_leg_shell.x_t");
        leg6_y_screw.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg6_y_screw.x_t");
        leg6_top_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg6_top_bar.x_t");
        leg6_longest_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg6_longest_bar.x_t");
        leg6_bot_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg6_bot_bar.x_t");
        leg6_crank.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg6_crank.x_t");
        leg6_h_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg6_h_bar.x_t");
        leg6_shortest_bar.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg6_shortest_bar.x_t");
        leg6_x_screw.geometryPool().add<aris::dynamic::ParasolidGeometry>("C:\\Users\\jpche\\Desktop\\Adams_model\\leg6_x_screw.x_t");

        //add joints//
        
        // leg1
        auto& leg1_r1 = hex->addRevoluteJoint(leg1_leg_shell, body, leg1_pe[0], std::array<double, 3>{0, 1, 0}.data());
        auto& leg1_p1 = hex->addPrismaticJoint(leg1_leg_shell, leg1_y_screw, leg1_pe[1], std::array<double, 3>{0, 1, 0}.data());
        auto& leg1_r2 = hex->addRevoluteJoint(leg1_y_screw, leg1_crank, leg1_pe[2], std::array<double, 3>{0, 0, 1}.data());
        auto& leg1_r3 = hex->addRevoluteJoint(leg1_leg_shell, leg1_top_bar, leg1_pe[3], std::array<double, 3>{0, 0, 1}.data());
        auto& leg1_r4 = hex->addRevoluteJoint(leg1_top_bar, leg1_h_bar, leg1_pe[3], std::array<double, 3>{0, 0, 1}.data());
        auto& leg1_r5 = hex->addRevoluteJoint(leg1_top_bar, leg1_longest_bar, leg1_pe[4], std::array<double, 3>{0, 0, 1}.data());
        auto& leg1_r6 = hex->addRevoluteJoint(leg1_longest_bar, leg1_bot_bar, leg1_pe[5], std::array<double, 3>{0, 0, 1}.data());
        auto& leg1_r7 = hex->addRevoluteJoint(leg1_bot_bar, leg1_crank, leg1_pe[6], std::array<double, 3>{0, 0, 1}.data());
        auto& leg1_r8 = hex->addRevoluteJoint(leg1_bot_bar, leg1_h_bar, leg1_pe[7], std::array<double, 3>{0, 0, 1}.data());
        auto& leg1_r9 = hex->addRevoluteJoint(leg1_bot_bar, leg1_shortest_bar, leg1_pe[7], std::array<double, 3>{0, 0, 1}.data());
        auto& leg1_r10 = hex->addRevoluteJoint(leg1_shortest_bar, leg1_x_screw, leg1_pe[8], std::array<double, 3>{0, 0, 1}.data());
        auto& leg1_p2 = hex->addPrismaticJoint(leg1_leg_shell, leg1_x_screw, leg1_pe[9], std::array<double, 3>{-1, 0, 0}.data());
        
        // leg2
        auto& leg2_r1 = hex->addRevoluteJoint(leg2_leg_shell, body, leg2_pe[0], std::array<double, 3>{0, 1, 0}.data());
        auto& leg2_p1 = hex->addPrismaticJoint(leg2_leg_shell, leg2_y_screw, leg2_pe[1], std::array<double, 3>{0, 1, 0}.data());
        auto& leg2_r2 = hex->addRevoluteJoint(leg2_y_screw, leg2_crank, leg2_pe[2], std::array<double, 3>{0.866, 0, 0.5}.data());
        auto& leg2_r3 = hex->addRevoluteJoint(leg2_leg_shell, leg2_top_bar, leg2_pe[3], std::array<double, 3>{0.866, 0, 0.5}.data());
        auto& leg2_r4 = hex->addRevoluteJoint(leg2_top_bar, leg2_h_bar, leg2_pe[3], std::array<double, 3>{0.866, 0, 0.5}.data());
        auto& leg2_r5 = hex->addRevoluteJoint(leg2_top_bar, leg2_longest_bar, leg2_pe[4], std::array<double, 3>{0.866, 0, 0.5}.data());
        auto& leg2_r6 = hex->addRevoluteJoint(leg2_longest_bar, leg2_bot_bar, leg2_pe[5], std::array<double, 3>{0.866, 0, 0.5}.data());
        auto& leg2_r7 = hex->addRevoluteJoint(leg2_bot_bar, leg2_crank, leg2_pe[6], std::array<double, 3>{0.866, 0, 0.5}.data());
        auto& leg2_r8 = hex->addRevoluteJoint(leg2_bot_bar, leg2_h_bar, leg2_pe[7], std::array<double, 3>{0.866, 0, 0.5}.data());
        auto& leg2_r9 = hex->addRevoluteJoint(leg2_bot_bar, leg2_shortest_bar, leg2_pe[7], std::array<double, 3>{0.866, 0, 0.5}.data());
        auto& leg2_r10 = hex->addRevoluteJoint(leg2_shortest_bar, leg2_x_screw, leg2_pe[8], std::array<double, 3>{0.866, 0, 0.5}.data());
        auto& leg2_p2 = hex->addPrismaticJoint(leg2_leg_shell, leg2_x_screw, leg2_pe[9], std::array<double, 3>{-0.5, 0, 0.866}.data());
        
        // leg3
        auto& leg3_r1 = hex->addRevoluteJoint(leg3_leg_shell, body, leg3_pe[0], std::array<double, 3>{0, 1, 0}.data());
        auto& leg3_p1 = hex->addPrismaticJoint(leg3_leg_shell, leg3_y_screw, leg3_pe[1], std::array<double, 3>{0, 1, 0}.data());
        auto& leg3_r2 = hex->addRevoluteJoint(leg3_y_screw, leg3_crank, leg3_pe[2], std::array<double, 3>{0.866, 0, -0.5}.data());
        auto& leg3_r3 = hex->addRevoluteJoint(leg3_leg_shell, leg3_top_bar, leg3_pe[3], std::array<double, 3>{0.866, 0, -0.5}.data());
        auto& leg3_r4 = hex->addRevoluteJoint(leg3_top_bar, leg3_h_bar, leg3_pe[3], std::array<double, 3>{0.866, 0, -0.5}.data());
        auto& leg3_r5 = hex->addRevoluteJoint(leg3_top_bar, leg3_longest_bar, leg3_pe[4], std::array<double, 3>{0.866, 0, -0.5}.data());
        auto& leg3_r6 = hex->addRevoluteJoint(leg3_longest_bar, leg3_bot_bar, leg3_pe[5], std::array<double, 3>{0.866, 0, -0.5}.data());
        auto& leg3_r7 = hex->addRevoluteJoint(leg3_bot_bar, leg3_crank, leg3_pe[6], std::array<double, 3>{0.866, 0, -0.5}.data());
        auto& leg3_r8 = hex->addRevoluteJoint(leg3_bot_bar, leg3_h_bar, leg3_pe[7], std::array<double, 3>{0.866, 0, -0.5}.data());
        auto& leg3_r9 = hex->addRevoluteJoint(leg3_bot_bar, leg3_shortest_bar, leg3_pe[7], std::array<double, 3>{0.866, 0, -0.5}.data());
        auto& leg3_r10 = hex->addRevoluteJoint(leg3_shortest_bar, leg3_x_screw, leg3_pe[8], std::array<double, 3>{0.866, 0, -0.5}.data());
        auto& leg3_p2 = hex->addPrismaticJoint(leg3_leg_shell, leg3_x_screw, leg3_pe[9], std::array<double, 3>{0.5, 0, 0.866}.data());

        // leg4
        auto& leg4_r1 = hex->addRevoluteJoint(leg4_leg_shell, body, leg4_pe[0], std::array<double, 3>{0, 1, 0}.data());
        auto& leg4_p1 = hex->addPrismaticJoint(leg4_leg_shell, leg4_y_screw, leg4_pe[1], std::array<double, 3>{0, 1, 0}.data());
        auto& leg4_r2 = hex->addRevoluteJoint(leg4_y_screw, leg4_crank, leg4_pe[2], std::array<double, 3>{0, 0, -1}.data());
        auto& leg4_r3 = hex->addRevoluteJoint(leg4_leg_shell, leg4_top_bar, leg4_pe[3], std::array<double, 3>{0, 0, -1}.data());
        auto& leg4_r4 = hex->addRevoluteJoint(leg4_top_bar, leg4_h_bar, leg4_pe[3], std::array<double, 3>{0, 0, -1}.data());
        auto& leg4_r5 = hex->addRevoluteJoint(leg4_top_bar, leg4_longest_bar, leg4_pe[4], std::array<double, 3>{0, 0, -1}.data());
        auto& leg4_r6 = hex->addRevoluteJoint(leg4_longest_bar, leg4_bot_bar, leg4_pe[5], std::array<double, 3>{0, 0, -1}.data());
        auto& leg4_r7 = hex->addRevoluteJoint(leg4_bot_bar, leg4_crank, leg4_pe[6], std::array<double, 3>{0, 0, -1}.data());
        auto& leg4_r8 = hex->addRevoluteJoint(leg4_bot_bar, leg4_h_bar, leg4_pe[7], std::array<double, 3>{0, 0, -1}.data());
        auto& leg4_r9 = hex->addRevoluteJoint(leg4_bot_bar, leg4_shortest_bar, leg4_pe[7], std::array<double, 3>{0, 0, -1}.data());
        auto& leg4_r10 = hex->addRevoluteJoint(leg4_shortest_bar, leg4_x_screw, leg4_pe[8], std::array<double, 3>{0, 0, -1}.data());
        auto& leg4_p2 = hex->addPrismaticJoint(leg4_leg_shell, leg4_x_screw, leg4_pe[9], std::array<double, 3>{1, 0, 0}.data());

        // leg5
        auto& leg5_r1 = hex->addRevoluteJoint(leg5_leg_shell, body, leg5_pe[0], std::array<double, 3>{0, 1, 0}.data());
        auto& leg5_p1 = hex->addPrismaticJoint(leg5_leg_shell, leg5_y_screw, leg5_pe[1], std::array<double, 3>{0, 1, 0}.data());
        auto& leg5_r2 = hex->addRevoluteJoint(leg5_y_screw, leg5_crank, leg5_pe[2], std::array<double, 3>{-0.866, 0, -0.5}.data());
        auto& leg5_r3 = hex->addRevoluteJoint(leg5_leg_shell, leg5_top_bar, leg5_pe[3], std::array<double, 3>{-0.866, 0, -0.5}.data());
        auto& leg5_r4 = hex->addRevoluteJoint(leg5_top_bar, leg5_h_bar, leg5_pe[3], std::array<double, 3>{-0.866, 0, -0.5}.data());
        auto& leg5_r5 = hex->addRevoluteJoint(leg5_top_bar, leg5_longest_bar, leg5_pe[4], std::array<double, 3>{-0.866, 0, -0.5}.data());
        auto& leg5_r6 = hex->addRevoluteJoint(leg5_longest_bar, leg5_bot_bar, leg5_pe[5], std::array<double, 3>{-0.866, 0, -0.5}.data());
        auto& leg5_r7 = hex->addRevoluteJoint(leg5_bot_bar, leg5_crank, leg5_pe[6], std::array<double, 3>{-0.866, 0, -0.5}.data());
        auto& leg5_r8 = hex->addRevoluteJoint(leg5_bot_bar, leg5_h_bar, leg5_pe[7], std::array<double, 3>{-0.866, 0, -0.5}.data());
        auto& leg5_r9 = hex->addRevoluteJoint(leg5_bot_bar, leg5_shortest_bar, leg5_pe[7], std::array<double, 3>{-0.866, 0, -0.5}.data());
        auto& leg5_r10 = hex->addRevoluteJoint(leg5_shortest_bar, leg5_x_screw, leg5_pe[8], std::array<double, 3>{-0.866, 0, -0.5}.data());
        auto& leg5_p2 = hex->addPrismaticJoint(leg5_leg_shell, leg5_x_screw, leg5_pe[9], std::array<double, 3>{0.5, 0, -0.866}.data());

        // leg6
        auto& leg6_r1 = hex->addRevoluteJoint(leg6_leg_shell, body, leg6_pe[0], std::array<double, 3>{0, 1, 0}.data());
        auto& leg6_p1 = hex->addPrismaticJoint(leg6_leg_shell, leg6_y_screw, leg6_pe[1], std::array<double, 3>{0, 1, 0}.data());
        auto& leg6_r2 = hex->addRevoluteJoint(leg6_y_screw, leg6_crank, leg6_pe[2], std::array<double, 3>{-0.866, 0, 0.5}.data());
        auto& leg6_r3 = hex->addRevoluteJoint(leg6_leg_shell, leg6_top_bar, leg6_pe[3], std::array<double, 3>{-0.866, 0, 0.5}.data());
        auto& leg6_r4 = hex->addRevoluteJoint(leg6_top_bar, leg6_h_bar, leg6_pe[3], std::array<double, 3>{-0.866, 0, 0.5}.data());
        auto& leg6_r5 = hex->addRevoluteJoint(leg6_top_bar, leg6_longest_bar, leg6_pe[4], std::array<double, 3>{-0.866, 0, 0.5}.data());
        auto& leg6_r6 = hex->addRevoluteJoint(leg6_longest_bar, leg6_bot_bar, leg6_pe[5], std::array<double, 3>{-0.866, 0, 0.5}.data());
        auto& leg6_r7 = hex->addRevoluteJoint(leg6_bot_bar, leg6_crank, leg6_pe[6], std::array<double, 3>{-0.866, 0, 0.5}.data());
        auto& leg6_r8 = hex->addRevoluteJoint(leg6_bot_bar, leg6_h_bar, leg6_pe[7], std::array<double, 3>{-0.866, 0, 0.5}.data());
        auto& leg6_r9 = hex->addRevoluteJoint(leg6_bot_bar, leg6_shortest_bar, leg6_pe[7], std::array<double, 3>{-0.866, 0, 0.5}.data());
        auto& leg6_r10 = hex->addRevoluteJoint(leg6_shortest_bar, leg6_x_screw, leg6_pe[8], std::array<double, 3>{-0.866, 0, 0.5}.data());
        auto& leg6_p2 = hex->addPrismaticJoint(leg6_leg_shell, leg6_x_screw, leg6_pe[9], std::array<double, 3>{-0.5, 0, -0.866}.data());

        //add motion//
        
        //leg1//
        auto& leg1_m1 = hex->addMotion(leg1_p2); //X副
        auto& leg1_m2 = hex->addMotion(leg1_p1); //Y副
        auto& leg1_m3 = hex->addMotion(leg1_r1); //R副
        
        //leg2//
        auto& leg2_m1 = hex->addMotion(leg2_p2); //X副
        auto& leg2_m2 = hex->addMotion(leg2_p1); //Y副
        auto& leg2_m3 = hex->addMotion(leg2_r1); //R副
       
        //leg3//
        auto& leg3_m1 = hex->addMotion(leg3_p2); //X副
        auto& leg3_m2 = hex->addMotion(leg3_p1); //Y副
        auto& leg3_m3 = hex->addMotion(leg3_r1); //R副

        //leg4//
        auto& leg4_m1 = hex->addMotion(leg4_p2); //X副
        auto& leg4_m2 = hex->addMotion(leg4_p1); //Y副
        auto& leg4_m3 = hex->addMotion(leg4_r1); //R副

        //leg5//
        auto& leg5_m1 = hex->addMotion(leg5_p2); //X副
        auto& leg5_m2 = hex->addMotion(leg5_p1); //Y副
        auto& leg5_m3 = hex->addMotion(leg5_r1); //R副

        //leg6//
        auto& leg6_m1 = hex->addMotion(leg6_p2); //X副
        auto& leg6_m2 = hex->addMotion(leg6_p1); //Y副
        auto& leg6_m3 = hex->addMotion(leg6_r1); //R副

        //add end-effector//
        auto body_ee_maki = body.addMarker("body_ee_mak_i");
        auto body_ee_makj = hex->ground().addMarker("body_ee_mak_j");

        auto& body_ee = hex->generalMotionPool().add<aris::dynamic::GeneralMotion>("body_ee", &body_ee_maki, &body_ee_makj);
        auto& leg1_ee = hex->addPointMotion(leg1_longest_bar, hex->ground(), ee_pos[0]);
        hex->ground().markerPool().back().setPrtPe(std::array<double, 6>{0, 0, 0, 0, 0, 0}.data());  //这个地方是height吗
        auto& leg2_ee = hex->addPointMotion(leg2_longest_bar, hex->ground(), ee_pos[1]);
        hex->ground().markerPool().back().setPrtPe(std::array<double, 6>{0, 0, 0, 0, 0, 0}.data());
        auto& leg3_ee = hex->addPointMotion(leg3_longest_bar, hex->ground(), ee_pos[2]);
        hex->ground().markerPool().back().setPrtPe(std::array<double, 6>{0, 0, 0, 0, 0, 0}.data());
        auto& leg4_ee = hex->addPointMotion(leg4_longest_bar, hex->ground(), ee_pos[3]);
        hex->ground().markerPool().back().setPrtPe(std::array<double, 6>{0, 0, 0, 0, 0, 0}.data());
        auto& leg5_ee = hex->addPointMotion(leg5_longest_bar, hex->ground(), ee_pos[4]);
        hex->ground().markerPool().back().setPrtPe(std::array<double, 6>{0, 0, 0, 0, 0, 0}.data());
        auto& leg6_ee = hex->addPointMotion(leg6_longest_bar, hex->ground(), ee_pos[5]);
        hex->ground().markerPool().back().setPrtPe(std::array<double, 6>{0, 0, 0, 0, 0, 0}.data());



        auto& inverse_kinematic_solver = hex->solverPool().add<aris::dynamic::InverseKinematicSolver>();
        auto& forward_kinematic_solver = hex->solverPool().add<aris::dynamic::ForwardKinematicSolver>();
        auto& inverse_dynamic_solver = hex->solverPool().add<aris::dynamic::InverseDynamicSolver>();
        auto& forward_dynamic_solver = hex->solverPool().add<aris::dynamic::ForwardDynamicSolver>();

        auto& stand_universal = hex->solverPool().add<aris::dynamic::UniversalSolver>();

        //添加仿真器和仿真结果//
        auto& adams = hex->simulatorPool().add<aris::dynamic::AdamsSimulator>();
        auto& result = hex->simResultPool().add<aris::dynamic::SimResult>();

        hex->init();

        // 设置默认拓扑结构 //
        for (auto& m : hex->motionPool())m.activate(true);
        for (auto& gm : hex->generalMotionPool())gm.activate(false);

        return hex;

    }
    auto createControllerHexapod()->std::unique_ptr<aris::control::Controller>
    {
        
        std::unique_ptr<aris::control::Controller> controller(new aris::control::EthercatController);


        for (aris::Size i = 0; i < 12; ++i)
        {
#ifdef ARIS_USE_ETHERCAT_SIMULATION
            double pos_offset[12]
            { -0.0163824,
                         0.184725,
                         1.90999,
                         -0.193661,
                         0.289958,
                         0.609837,
                         -0.204495,
                         0.287226,
                         0.612374,
                         -0.184633,
                         0.708104,
                         -0.945939
                0,0,0,0,0,0
            };
#else
            double pos_offset[12]
            {
                -0.0163824,
                0.184725,
                1.90999,
                -0.193661,
                0.289958,
                0.609837,
                -0.204495,
                0.287226,
                0.612374,
                -0.184633,
                0.708104,
                -0.945939

            };
#endif
            double pos_factor[12]
            {
                262144.0 * 6 / 2 / PI, 262144.0 * 6 / 2 / PI, 262144.0 * 6 / 2 / PI,
                262144.0 * 6 / 2 / PI, 262144.0 * 6 / 2 / PI, 262144.0 * 6 / 2 / PI,
                262144.0 * 6 / 2 / PI, 262144.0 * 6 / 2 / PI, 262144.0 * 6 / 2 / PI,
                262144.0 * 6 / 2 / PI, 262144.0 * 6 / 2 / PI, 262144.0 * 6 / 2 / PI,
            };
            double max_pos[12]
            {
                PI,PI,PI,
                PI,PI,PI,
                PI,PI,PI,
                PI,PI,PI,
            };
            double min_pos[12]
            {
                -PI,-PI,-PI,
                -PI,-PI,-PI,
                -PI,-PI,-PI,
                -PI,-PI,-PI,
            };
            double max_vel[12]
            {
                330 / 60 * 2 * PI, 330 / 60 * 2 * PI,  330 / 60 * 2 * PI,
                330 / 60 * 2 * PI, 330 / 60 * 2 * PI,  330 / 60 * 2 * PI,
                330 / 60 * 2 * PI, 330 / 60 * 2 * PI,  330 / 60 * 2 * PI,
                330 / 60 * 2 * PI, 330 / 60 * 2 * PI,  330 / 60 * 2 * PI,
            };
            double max_acc[12]
            {
                30000,  30000,  30000,
                30000,  30000,  30000,
                30000,  30000,  30000,
                30000,  30000,  30000,
            };

            int phy_id[12] = { 2,1,0,3,4,5,8,7,6,9,10,11 };


            std::string xml_str =
                "<EthercatMotor phy_id=\"" + std::to_string(phy_id[i]) + "\" product_code=\"0x00\""
                " vendor_id=\"0x00\" revision_num=\"0x00\" dc_assign_activate=\"0x0300\""
                " min_pos=\"" + std::to_string(min_pos[i]) + "\" max_pos=\"" + std::to_string(max_pos[i]) + "\" max_vel=\"" + std::to_string(max_vel[i]) + "\" min_vel=\"" + std::to_string(-max_vel[i]) + "\""
                " max_acc=\"" + std::to_string(max_acc[i]) + "\" min_acc=\"" + std::to_string(-max_acc[i]) + "\" max_pos_following_error=\"10.0\" max_vel_following_error=\"20.0\""
                " home_pos=\"0\" pos_factor=\"" + std::to_string(pos_factor[i]) + "\" pos_offset=\"" + std::to_string(pos_offset[i]) + "\">"
                "	<SyncManagerPoolObject>"
                "		<SyncManager is_tx=\"false\"/>"
                "		<SyncManager is_tx=\"true\"/>"
                "		<SyncManager is_tx=\"false\">"
                "			<Pdo index=\"0x1605\" is_tx=\"false\">"
                "				<PdoEntry name=\"target_pos\" index=\"0x607A\" subindex=\"0x00\" size=\"32\"/>"
                "				<PdoEntry name=\"target_vel\" index=\"0x60FF\" subindex=\"0x00\" size=\"32\"/>"
                "				<PdoEntry name=\"targer_toq\" index=\"0x6071\" subindex=\"0x00\" size=\"16\"/>"
                "				<PdoEntry name=\"max_toq\" index=\"0x6072\" subindex=\"0x00\" size=\"16\"/>"
                "				<PdoEntry name=\"control_word\" index=\"0x6040\" subindex=\"0x00\" size=\"16\"/>"
                "				<PdoEntry name=\"mode_of_operation\" index=\"0x6060\" subindex=\"0x00\" size=\"8\"/>"
                "			</Pdo>"
                "		</SyncManager>"
                "		<SyncManager is_tx=\"true\">"
                "			<Pdo index=\"0x1A07\" is_tx=\"true\">"
                "				<PdoEntry name=\"status_word\" index=\"0x6041\" subindex=\"0x00\" size=\"16\"/>"
                "				<PdoEntry name=\"mode_of_display\" index=\"0x6061\" subindex=\"0x00\" size=\"8\"/>"
                "				<PdoEntry name=\"pos_actual_value\" index=\"0x6064\" subindex=\"0x00\" size=\"32\"/>"
                "				<PdoEntry name=\"vel_actual_value\" index=\"0x606c\" subindex=\"0x00\" size=\"32\"/>"
                "				<PdoEntry name=\"toq_actual_value\" index=\"0x6077\" subindex=\"0x00\" size=\"16\"/>"
                "			</Pdo>"
                "		</SyncManager>"
                "	</SyncManagerPoolObject>"
                "</EthercatMotor>";

            auto& s = controller->slavePool().add<aris::control::EthercatMotor>();
            aris::core::fromXmlString(s, xml_str);



#ifdef WIN32
            dynamic_cast<aris::control::EthercatMotor&>(controller->slavePool().back()).setVirtual(true);
#endif

#ifndef WIN32
            dynamic_cast<aris::control::EthercatMotor&>(controller->slavePool().back()).scanInfoForCurrentSlave();
#endif

        };
        return controller;
    }
    auto createPlanHexapod()->std::unique_ptr<aris::plan::PlanRoot>
    {
        std::unique_ptr<aris::plan::PlanRoot> plan_root(new aris::plan::PlanRoot);
        plan_root->planPool().add<aris::plan::Enable>();
        plan_root->planPool().add<aris::plan::Disable>();
        plan_root->planPool().add<aris::plan::Home>();
        plan_root->planPool().add<aris::plan::Mode>();
        plan_root->planPool().add<aris::plan::Show>();
        plan_root->planPool().add<aris::plan::Sleep>();
        plan_root->planPool().add<aris::plan::Clear>();
        plan_root->planPool().add<aris::plan::Recover>();
        auto& rs = plan_root->planPool().add<aris::plan::Reset>();
        rs.command().findParam("pos")->setDefaultValue("{0.5,0.392523364485981,0.789915966386555,0.5,0.5,0.5}");

        auto& mvaj = plan_root->planPool().add<aris::plan::MoveAbsJ>();
        mvaj.command().findParam("vel")->setDefaultValue("0.1");

        plan_root->planPool().add<aris::plan::MoveL>();
        plan_root->planPool().add<aris::plan::MoveJ>();

        plan_root->planPool().add<aris::plan::GetXml>();
        plan_root->planPool().add<aris::plan::SetXml>();
        plan_root->planPool().add<aris::plan::Start>();
        plan_root->planPool().add<aris::plan::Stop>();

        plan_root->planPool().add<HexDynamicForwardTest>();
        plan_root->planPool().add<HexDynamicBackTest>();
        plan_root->planPool().add<HexDynamicRightTest>();
        plan_root->planPool().add<HexDynamicLeftTest>();
        plan_root->planPool().add<HexDynamicTurnRightTest>();
        plan_root->planPool().add<HexDynamicTetrapodTest>();
        return plan_root;
    }

    auto setStandTopologyIK(aris::server::ControlServer& cs)->void
    {

        // 站立拓扑结构下，分配求解器内存 //
        // 失效就是不可规划，有效可以规划
        cs.model().generalMotionPool()[0].activate(false);//body
        cs.model().generalMotionPool()[1].activate(false);//leg1
        cs.model().generalMotionPool()[2].activate(false);//leg2
        cs.model().generalMotionPool()[3].activate(false);//leg3
        cs.model().generalMotionPool()[4].activate(false);//leg4
        cs.model().generalMotionPool()[5].activate(false);//leg5
        cs.model().generalMotionPool()[6].activate(false);//leg6

        for (int i = 0; i < 12; ++i)
            cs.model().motionPool()[0].activate(true);
        cs.model().solverPool()[0].allocateMemory();
    }





}