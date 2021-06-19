#include"plan.h"
#include"kinematics.h"
#include<aris.hpp>
#include"robot.h"

extern double input_angle[18];
extern double file_current_leg[18];
extern double file_current_body[16];
int main(int argc, char *argv[])
{
	auto& cs = aris::server::ControlServer::instance();
	cs.resetController(robot::createControllerHexapod().release());
	cs.resetPlanRoot(robot::createPlanHexapod().release());
	cs.resetModel(robot::createModelHexapod().release());

	// ����ģ�ͳ�ʼλ�ã����ؽڽǶ�  ע����Ե�λ����ģ��Quad�����õĹؽ����ĩ��  ��ʼλ�õ������Ϊ0 //
	double set_init_position[18] = {
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0,
		0,0,0
	};

	cs.model().setInputPos(set_init_position); 
	if (cs.model().forwardKinematics()) THROW_FILE_LINE("forward failed"); //�������⣬�õ�ĩ��λ��
	//robot::setStandTopologyIK(cs);
	
	auto& adams = dynamic_cast<aris::dynamic::AdamsSimulator&>(cs.model().simulatorPool().front());
	//adams.saveAdams("C:\\Users\\jpche\\Desktop\\aaa\\hexapod_simulation.cmd"); //���ֻ�ǵ���ģ��

	//robot::HexDynamicForwardTest plan;
	//robot::HexDynamicBackTest plan;
	//robot::HexDynamicRightTest plan;
	//robot::HexDynamicLeftTest plan;
	//robot::HexDynamicTurnRightTest plan;
	//robot::HexDynamicTurnLeftTest plan;
	//robot::HexDynamicTetrapodTest plan;
	//adams.simulate(plan, cs.model().simResultPool().front());
	//adams.saveAdams("C:\\Users\\jpche\\Desktop\\aaa\\hexapod_simulation_with_control.cmd", cs.model().simResultPool().front());

	//std::cout << "simulate finished" << std::endl;

	//�ȴ��ն����뺯��������������ȥ��������ʵʱ�̺߳����̶߳������//
	cs.init();

	//����websocket/socket������//
	cs.open();
	cs.runCmdLine();
	return 0;
}