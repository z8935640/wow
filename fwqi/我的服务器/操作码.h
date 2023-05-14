#ifndef _Message_h_
#define _Message_h_

enum MessageTypes
{
	/*			客户端发服务器			*/	
	C_客户心跳 = 0x3000,

	C_获取地址 = 0x1005,	//客户端获取IP地址
	C_获取时间 = 0x1006,	//客户端获取服务器时间

	C_管理登录 = 0x1001,	
	C_管理转发 = 0x1007,	//管理给总控和中控转发消息
	C_管理群发 = 0x100B,	//管理给总控和中控群发消息
	C_管理上传 = 0x100F,	//管理上传文件
	C_管理下载 = 0x1013,	//管理下载文件
	C_管理命令 = 0x1008,	//管理给脚本发送命令
	C_管理枚举 = 0x1017,	//获取总控,中控,游戏窗口数
	C_管理更新 = 0x1018,	//管理更新本地的总控,中控,脚本
	C_管理生成 = 0x101B,	//生成KEY
	C_管理删除 = 0x101F,	//删除KEY
	C_管理加时 = 0x101C,	//给用户增加时间
	C_管理减时 = 0x101D,	//给用换减少时间
	C_管理封号 = 0x101E,	//封号
	C_管理查询 = 0x101F,	//管理查询KEY信息

	C_总控登录 = 0x1002,
	C_总控转发 = 0x1008,	//总控给中控转发消息
	C_总控群发 = 0x100C,	//总控给中控群发消息
	C_总控上传 = 0x1010,	//总控上传
	C_总控下载 = 0x1014,	//总控下载
	C_总控命令 = 0x1015,	//总控给脚本发送命令
	C_总控枚举 = 0x1017,	//总控获取中控,游戏窗口数
	C_总控更新 = 0x1018,	//总控更新本地的中控,脚本

	C_中控登录 = 0x1003,
	C_中控更新 = 0x1018,	//中控更新本地的脚本

	C_脚本登录 = 0x1004,
	C_脚本转发 = 0x100A,	//脚本给总控和管理转发消息
	C_脚本群发 = 0x100E,	//脚本给总控,管理群发消息	好像用不到
	C_脚本状态 = 0x1000,	//脚本给总控和管理发送状态
	C_脚本报警 = 0x100F,	//脚本给总控报警
	C_脚本沟通 = 0x1000,	//脚本给脚本发送消息
	C_脚本图片 = 0x1000,	//脚本给总控发送图片
	


	/*			服务器发客户端			*/		
	S_管理登录 = 0x2001,
	S_总控登录 = 0x2002,
	S_中控登录 = 0x2003,
	S_脚本登录 = 0x2004,
				   
	S_获取地址 = 0x2005,
	S_获取时间 = 0x2006,
				   
	S_管理转发 = 0x2007,
	S_总控转发 = 0x2008,
	S_中控转发 = 0x2009,
	S_脚本转发 = 0x200A,
				   
	S_管理群发 = 0x200B,
	S_总控群发 = 0x200C,
	S_中控群发 = 0x200D,
	S_脚本群发 = 0x200E,
				   
	S_管理上传 = 0x200F,
	S_总控上传 = 0x2010,
	S_中控上传 = 0x2011,
	S_脚本上传 = 0x2012,
				   
	S_管理下载 = 0x2013,
	S_总控下载 = 0x2014,
	S_中控下载 = 0x2015,
	S_脚本下载 = 0x2016,

	S_管理枚举 = 0x2017,
	S_总控枚举 = 0x2018,
	S_中控枚举 = 0x2019,
	S_脚本枚举 = 0x201A,

	S_管理生成 = 0x201B,
	S_管理加时 = 0x201C,
	S_管理减时 = 0x201D,
	S_管理封号 = 0x201E,
	S_管理删除 = 0x201F,





	





	T_Login = 0x3010,		//登录消息
	T_Login_Result = 0x3011,		//登录结果
	T_Logout = 0x3012,			//登出消息
	T_Logout_Result = 0x3013,	//登出结果
	T_ERROR	= 0x3014			//异常消息
};

struct 包头
{
	WORD 长度;
	WORD 类型;
};

struct 客户心跳 : public 包头
{
	客户心跳()
	{
		长度 = sizeof(客户心跳);
		类型 = C_客户心跳;
	}
};

struct 获取地址 : public 包头
{
	获取地址()
	{
		长度 = sizeof(获取地址);
		类型 = C_获取地址;
	}
};

struct 获取时间 : public 包头
{
	获取时间()
	{
		长度 = sizeof(获取时间);
		类型 = C_获取时间;
	}
};



struct MesageHeader
{
	short lenth;
	short type;
};

struct Login :public MesageHeader
{
	Login()
	{
		lenth = sizeof(Login);
		type = T_Login;
	}
	char name[32] = {};
	char password[32] = {};
};

struct Login_Result :public MesageHeader
{
	Login_Result()
	{
		lenth = sizeof(Login_Result);
		type = T_Login_Result;
		result = 0;				//0表示成功
	}
	int result;
};

struct Logout :public MesageHeader
{
	Logout()
	{
		lenth = sizeof(Logout);
		type = T_Logout;
	}
	char name[32] = {};
	char password[32] = {};
};

struct Logout_Result :public MesageHeader
{
	Logout_Result()
	{
		lenth = sizeof(Logout_Result);
		type = T_Logout_Result;
		result = 0;				//0表示成功
	}
	int result;
};
#endif // !_Message_h_
