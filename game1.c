#include <stdlib.h>
#include <stdio.h>
#include <conio.h>//非阻塞性响应控制台输入
#include <time.h> //suiji
#include <string.h>
#include <unistd.h>	 //包含延迟函数的头文件，例如 usleep
#include <windows.h> //颜色函数
//每次保存时不要以utf-8格式
#define LEN 30//数据的位数
#define DEFAULT_MS 20//默认输出间隔
#define escpro 1//怪物逃跑的概率
#define addpro 100//掉落概率
#define lineL 4//每行输出物品的个数+1
#define TeamNum 4//玩家队伍最大人数
#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define MIN(X,Y) ((X)<(Y)?(X):(Y))
#define COLOR(X) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), X)//用于更改字符颜色
//#define COLORP(X,Y) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),X | Y)//更改字符颜色和前景色
//#define COLORPP((X),(Y)) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),X | Y | FOREGROUND_INTENSITY | BACKGROUND_INTENSITY)//同上，但前景、背景加强
/*前景颜色对应值：
　　0=黑色                8=灰色　　
  　1=蓝色                9=淡蓝色        十六进制        　　
　　2=绿色                10=淡绿色       0xa        　　
　　3=湖蓝色              11=淡浅绿色     0xb　
　　4=红色                12=淡红色       0xc　　
　　5=紫色                13=淡紫色       0xd        　　
　　6=黄色                14=淡黄色       0xe        　　
　　7=白色                15=亮白色       0xf */

//本程序由文止墨(2184711240@qq.com)编写。
//本程序灵感来自于网络上的“海战棋代码”，但本程序在编写时并未使用其代码(指各个函数的实现方式，以及程序的数据结构)。


/*-----------下面定义结构体---------*/


struct Prop { //道具结构体
	int n;//道具编号0-49为防具，50-99为武器，100-149为攻击道具，150-199为回复道具，200-249为防御道具
	char *name;
	long num;//道具的数值，如增加的防御力、攻击力，或造成的伤害、回复的血量等等
	int type;//道具类型，0为武器，1为防具，2为攻击道具，3为回复道具，4为物品材料
	int a_num;//作用范围，6为全体，其他为复数
	int ele;//如果是伤害，伤害的元素类型
	int havenum;//拥有的数量
};

struct Souvenir { //纪念品结构体
	char *name;
	int have;//是否解锁
	int read;//是否为可以读取内容的类型
	char *intro;//物品简介
	char *content;//如果是可以读取的类型，它的内容
	int f;//是否有自己的函数
	void (*function)();//函数指针
	int ms;//输出内容时间隔，不指明时和DEFAULT_MS一样。
};

struct Attribute {//附加属性结构体
	float crit_rate;//暴击率
	float crit_damage;//暴击伤害倍率
	long damage;//攻击力
	float damage_rate;//攻击力提高率
	long hp;//血量提高
	float hp_rate;
	long def;//防御
	float def_rate;
	float miss;//闪避
	float cure;//治疗加成
	int speed;//速度
};

struct Relics {//圣遗物结构体
	int n;//类别，比如头部、腹部什么的
	struct Attribute attribute;//属性

};


struct Player { //玩家结构体
	char name[200];//结构体定义的时候不能被初始化
	long hp;//血
	long f_hp;//血上限
	long hd;//蓝
	long f_hd;
	long attack;//当前攻击力
	long n_attack;//平时攻击力;用于消除战斗中使用的道具对攻击力的影响
	long defence;//当前防御力
	long n_defence;
	float miss;//敏捷，即闪避概率.最高不超过%20
	int n_miss;
	int speed;//速度
	int n_speed;
	int sp[LEN];//技能列表
	long lever;//等级
	int exp[LEN];//经验
	int lexp[LEN];//升级所需的经验
	struct Prop *defp;//防御道具结构体
	struct Prop *attp;//武器
	int team;//当前是否在队伍里
	int relics[5];//圣遗物
	float hparr[2];
	float hdarr[2];
	float defarr[2];
	float attarr[2];//这四个是数值变化的斜率与截距
	float crit_rate;//暴击率
	float crit_damage;//暴击伤害倍率
	float cure;//治疗加成
};

struct Att_action {//攻击行为结构体
	int skill[2];//这个行为使用什么技能以及优先级
	int condition;//进行该行为的条件
	//0表示平时
	//1表示在回合数满足a+b*n回合时发动
	//2表示在血量处于a%~b%之间时使用
	//3同上，不过是蓝量
	//4表示处于编号为a的状态下发动
	//5表示当队伍里最大角色等级大于a时发动
	int a,b,c;//c为发动概率，c%
};

struct Monster { //怪物结构体
	char *name;
	long hp;
	long attack;
	long defence;
	int miss;
	int speed;
	int action_num;//行为列表数量
	struct Att_action action[10];//行为
	int exp[LEN];//怪物能给与的经验
	char *v;//怪物出场提示“XXX跳了出来”
	struct Prop *prop;//掉落物品的结构体
	int g[LEN];//怪物掉落的金币
};

struct Egroup { //敌群结构体
	char *name;
	int num;//怪物的数量
	//char *mname[5];//敌群内每个怪物的名字
	int monster[5];//每个怪物对应的编号

};

struct BUFF {
	char *name;
	int n;//buff会给所有者挂上相应的元素
	double resistance[7];//buff会给相应元素增加(+)减少(-)对应抗性，0物理，1火，2毒，3水，4冰，5雷, 6特殊
	long gain[4];//buff对属性的影响，0攻击，1防御，2生命，3速度
	long t_gain[4];//buff每一回合对属性的影响，用于持续治疗、持续伤害什么的
	int t;//buff持续的回合数
};

struct Map { //地图结构体
	char *name;
	int Map_class;//这个地图的种类，0为安全区，1为野外
	int monsternum[4];//如果是野外，则这片区域有可能碰到哪些怪物
	int npc[10];//这个地图有几个npc
	int next[8];//与这片区域相连的区域的编号
};

struct Skill { //技能结构体
	int num;//编号
	int c;//使用对象，1敌人，0队友。如果是对队友回血的话把攻击力倍率一栏改成负数
	int a_num;//作用范围，单体、复数个敌人；如果作用于敌群全体只要设置成比敌群最大值大即可
	char *name;//技能名称
	char *suffix;//后缀，比如“就因为他做得到！”
	long sp;//消耗蓝量
	float damage;//攻击力倍率
	int boo;//是否对施展者的血量有影响，加血1，减血-1，没有影响0
	long hp;//一些技能会增加或消耗血量
	int ele;//元素种类，15物理，12火，10毒，9水，3冰，5雷, 4特殊，10治愈
	int status;//这个技能会附着编号为n的状态
	char *intro;//技能描述
};

struct ConNode { //对话结构体
	int fight;// 会不会在这句话后进入战斗，如果进入则为敌群编号
	int fight_continue;//战斗失败后是否继续剧情，如果继续的话会将玩家回复到满状态
	char *content;//这句话的内容
	int ifchoose;//是否需要选择，以及选择的数量
	char *chooselist[5];//每个选择的内容
	int next[5];//对应应跳转的节点
	int start_task;//这句话是否会开启某个任务，任务的编号。无则为0.
	int check_task;//这句话检查某个任务是否完成
	int finish_next_con[2];//在检查玩任务是否完成后，依据情况触发不同的对话

};

struct Task {
	char* name;
	int see;//是否会在任务列表里看见（用于多分支结局的任务给出不同的奖励）
	int npc_s;//这个任务找谁接取
	int npc_f;//这个任务找谁提交
	int con_s;//接取这个任务第一句话是什么（当这个任务可被接取时，将上述npc的对话节点改成这个
	int con_f;//提交这个任务第一句话是什么
	int start;//判断这个任务有没有开始（指有没有被接取）
	int finish;//判断这个任务有没有完成（有没有被提交，被提交的任务不会被再次检查）
	int father;//该任务是否是某个大型任务的子任务（用于输出任务树时不会重复输出子任务以及完成子任务时检查父任务是否完成）
	int sonnum;//这个任务有几个子任务
	int son[5];//子任务的编号
	int condition[4];//前置条件，0是否拥有某种道具，1是否拥有某种纪念品，
	//2是否完成某个任务，3对应0所需的道具数量
	int lever;//前置条件-是否达到指定等级
	char *intro;//任务内容描述
	int f_son;//完成条件，至少要完成几个子任务（用于多分支任务）
	int f_condition[4];//完成条件，0是否拥有某种道具，1是否拥有某种纪念品，
	//2无作用，3对应0所需的道具数量
	int give_prop;//任务完成给予什么道具
	int give_prop_num;//数量
	int give_souvenir;//纪念品奖励
	int give_exp[LEN];//经验奖励
	int give_gold[LEN];//金币奖励
	int next;//后续任务的编号，没有则为0（大型任务在接取时自动开启子任务，这个变量只用于任务链传递）
};

struct NPC {
	char *name;
	int task[51];//记录这个npc会受到哪些任务影响
	int var[50];//独立变量
	/*暂时不这么安排。
	var[0]代表这个npc是否能触发任务，如果能则为对应任务的编号；
	1代表这个npc有可能处于几个任务内
	2~2+var[1]是这几个任务的编号
	也就是说，一个NPC最多可以处在48个任务内
	（不要觉得这很多，因为我们是将一个大型任务细分为许多个小任务完成的，因此可能一个大型任务需要找某个特定的NPC很多次）
	*/
	int nomal_con;//在没有任务的情况下这个NPC的对话起点
	struct ConNode *con;//对话的起点,该内容是一个对话节点，可能会因为任务没完成造成用语的改变，
};
/*---------初始化道具-防具---------*/

struct Prop fj[50]= {
	{
		//初始道具，啥也没有
		.n=0,
		.name="无\0",
		.num=0,
		.type=1,
		.ele=0,
		.havenum=1,
	},

	{
		.n=1,
		.name="布衣\0",
		.num=50,
		.type=1,
		.ele=0,
		.havenum=1,
	},

	{
		.n=2,
		.name="皮甲\0",
		.num=200,
		.type=1,
		.ele=0,
		.havenum=1,
	},

	{
		.n=3,
		.name="链甲\0",
		.num=500,
		.type=1,
		.ele=0,
		.havenum=1,
	},

	{
		.n=4,
		.name="板甲\0",
		.num=1000,
		.type=1,
		.ele=0,
		.havenum=2,
	},

	{
		.n=5,
		.name="装甲\0",
		.num=5000,
		.type=1,
		.ele=0,
		.havenum=2,
	},

	{
		.n=6,
		.name="你五年前丢失的睡衣\0",
		.num=1000000,
		.type=1,
		.ele=0,
		.havenum=111,
	},

	{
		.n=7,
		.name="ERR\\OR@\\#$\\%sd!sdFAS!@\\\\\0",
		.num=999999999,
		.type=1,
		.ele=0,
		.havenum=1,
	}





};


/*---------初始化道具-武器---------*/

struct Prop wq[50]= {
	{
		//初始道具，啥也没有
		.n=50,
		.name="无\0",
		.num=0,
		.type=0,
		.ele=0,
		.havenum=1,
	},

	{
		.n=51,
		.name="铁剑\0",
		.num=200,
		.type=0,
		.ele=15,
		.havenum=1,
	},

	{
		.n=52,
		.name="黑缨枪\0",
		.num=100,
		.type=1,
		.ele=15,
		.havenum=1,
	},

	{
		.n=53,
		.name="ERROR#$warRE@#er%^EVE\0",
		.num=999999999,
		.type=1,
		.ele=4,
		.havenum=1,
	}



};


/*----------------------初始化道具-攻击道具-----------------------------*/

struct Prop gj[50]= {
	{
		.n=100,
		.name="手雷\0",
		.num=1000,
		.a_num=3,
		.type=2,
		.ele=12,
		.havenum=5,

	},
	{
		.n=101,
		.name="毒镖\0",
		.num=100,
		.type=2,
		.ele=10,
		.a_num=3,
		.havenum=3,

	},
	{
		.n=102,
		.name="粑粑\0",
		.num=9999,
		.type=2,
		.ele=4,
		.havenum=6,
		.a_num=6,
	}




};

/*--------------------------初始化道具-回复道具------------------------------*/

struct Prop hf[50]= {
	{
		.n=150,
		.name="回血包\0",
		.num=500,
		.type=3,
		.ele=12,
		.havenum=5,
		.a_num=6,
	},
	{
		.n=151,
		.name="草药\0",
		.num=700,
		.type=3,
		.ele=12,
		.havenum=5,
		.a_num=1,
	},
	{
		.n=152,
		.name="四方和平\0",
		.num=1500,
		.type=3,
		.ele=12,
		.havenum=5,
		.a_num=1,
	},
	{
		.n=153,
		.name="云南白药\0",
		.num=2000,
		.type=3,
		.ele=12,
		.havenum=5,
		.a_num=1,
	},
	{
		.n=154,
		.name="红药水\0",
		.num=2500,
		.type=3,
		.ele=12,
		.havenum=5,
		.a_num=1,
	},
	{
		.n=155,
		.name="元素瓶\0",
		.num=3500,
		.type=3,
		.ele=12,
		.havenum=5,
		.a_num=1,
	},
	{
		.n=156,
		.name="核子可乐\0",
		.num=5500,
		.type=3,
		.ele=12,
		.havenum=5,
		.a_num=6,
	},
	{
		.n=157,
		.name="洗手液\0",
		.num=10000,
		.type=3,
		.ele=12,
		.havenum=5,
		.a_num=5,
	},
	{
		.n=158,
		.name="凤凰尾巴\0",
		.num=10000,
		.type=3,
		.ele=12,
		.havenum=5,
		.a_num=6,
	},
	{
		.n=159,
		.name="绿色星星\0",
		.num=19500,
		.type=3,
		.ele=12,
		.havenum=5,
		.a_num=4,
	}




};

/*--------------初始化防御道具---------------*/

struct Prop fy[50];







/*-------------------------初始化纪念品------------------------------*/
struct Souvenir souvenir[50]= {
	{
		//0
		.name="姬子的照片",
		.have=1,
		.read=1,
		.intro="姬子温柔的注视着你，不再言语。",
		.content="姬子温柔的注视着你\\.，不再言语。",
		.ms=50,
	},

	{
		//1
		.name="未命名1.c",
		.have=0,
		.read=0,
		.intro="大  脑  一  片  空  白",
	},
	{
		//2
		.name="剑雨",
		.have=1,
		.read=1,
		.intro="某时下的雨，凌利似剑。",
		.content="“感谢各位这么多年的精诚合作，\\.\\.\\.\\.再会。”",
	},
	{
		//3
		.name="易拉环",
		.have=1,
		.read=1,
		.intro="曾被某人戴在胸前的物件，隐约可见“刁乐”二字。",
		.content="“是否有法律因素要停止这场婚礼的举行？” \\.\\.\\.\n“是否有人反对这场婚礼？” ",
	},
	{
		//4
		.name="笑里藏刀",
		.have=1,
		.read=0,
		.intro="哈哈哈哈哈哈哈哈哈哈哈哈刀哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈",
	},
	{
		//5
		.name="真·刀",
		.have=1,
		.read=0,
		.intro="*Chara,这是什么？",
	},
	{
		//6
		.name="奶油肉桂派",
		.have=1,
		.read=0,
		.intro="*你拥抱了失落灵魂并告诉她你和她将会重逢。",
	},
	{
		//7
		.name="心形坠饰盒",
		.have=1,
		.read=0,
		.intro="上面写着“最好的朋友”。",
	},
	{
		//8
		.name="村雨",
		.have=1,
		.read=0,
		.intro="某个雨夜里，一个男孩失去了最重要的人。",
	},
	{
		//9
		.name="0      ",
		.have=1,
		.read=0,
		.intro="如果你正在读这句话，说明我可能搞砸了什么。",
	},
	{
		//10
		.name="一片花瓣",
		.have=1,
		.read=0,
		.intro="金黄色的花瓣。\n“一起来毁灭这个可悲的世界上的所有东西吧。在这些无用的记忆里存在着的所有人，所有东西...让我们把他们全部化为尘埃。”",
	},
	{
		//11
		.name="飞机模型",
		.have=1,
		.read=1,
		.intro="木质模型。曾被某人放飞，又被某人捡起。",
		.content="“……奥托。” \\.\\.\\.\\.\\.\\.\\.\\.\n“奥托·阿波卡利斯。” ",
		.ms=30,
	},
	{
		//12
		.name="原神第一转伤定律",
		.have=1,
		.read=1,
		.intro="我  是  O  P  !",
		.content="《原神动态第一转伤定律》：\
		发表你逆天言论后，只要转发原神抽奖，你的一切非原神成分都将被无视，且你将被作为原神玩家攻击。",
		.ms=2,

	},
	{
		//13
		.name="See You Again",
		.have=1,
		.read=1,
		.intro="扣1复活直升机",
		.ms=1,
		.content="你说得对，但是科比·布莱恩特生于1978年8月23日，出生地是美国宾夕法尼亚州费城。\n他的父亲是前职业篮球运动员约翰·布莱恩特，而他的母亲则是意大利和美国的混血儿。\
		科比从小就展现出了过人的篮球天赋，他的父亲在退役后也一直是他的导师和引导者，帮助他完成篮球方面的训练和教育。\
		在1996年的NBA选秀中，科比被洛杉矶湖人队选中，从此开始了他的职业篮球生涯。他的职业生涯持续了20个赛季，期间他先后拿下了5个NBA总冠军、2个NBA总决赛MVP、4个全明星赛MVP等无数个荣誉。\
		科比·布莱恩特也两次代表美国国家队参加奥运会，分别在2008年北京奥运会和2012年伦敦奥运会上获得了金牌。他不仅在比赛中的表现十分出色，在场下的影响力也是无可挑剔的，是一位非常有创造力的人。\
		曾经写过小说、拍过短片、投资过创业公司等等。他有着非常深厚的文化底蕴和艺术追求，这让他的影响力远远超出了篮球这一范畴。他还积极参与公益事业，为慈善事业做出了很多贡献。\
		但是，2019年底，科比·布莱恩特的自己的私人飞机上遭遇了一场致命的意外。他和其他8人一同离世，这场意外震惊了整个世界。他的离去让人们深感悲痛和痛惜，篮球界也失去了一位不可替代的传奇。",
	},
	{
		//14
		.name="青玉小剑",
		.have=1,
		.read=1,
		.intro="以心血蕴养的飞剑。",
		.content="“去吧，去吧。\\.我去的地方不需要勇气，你去的地方才需要勇气。” ",
		.ms=30,
	},
	{
		//15
		.name="照片·自问",
		.have=1,
		.read=0,
		.intro="“若再许我少年时，一两黄金一两风。” ",
	},
	{
		//16
		.name="校园网",
		.have=1,
		.read=0,
		.intro="暴躁的根源。",
	},
	{
		//17
		.name="西瓜条",
		.have=1,
		.read=0,
		.intro="“家人们，谁懂啊。”",
	},
	{
		//18
		.name="What Is Love",
		.have=1,
		.read=0,
		.intro="“B Y D 大力王自己都蚌埠住了是吧。”",
	},
	{
		//19
		.name="暗藏玄只因",
		.have=1,
		.read=0,
		.intro="“厉不厉害你坤哥。”",
	},
	{
		//20
		.name="麦克阿瑟",
		.have=1,
		.read=1,
		.intro="五星天皇。",
		.content="“给他一次网址，他能往硬盘里塞满学习资料；给他一点时间，他能释放无数生命。\n就连美国五星上将麦克阿瑟也对此评论到：‘如果上帝让我去和他打交道，我宁愿去打交。’” ",
	},
	{
		//21
		.name="九转大肠",
		.have=1,
		.read=0,
		.intro="“……是故意的还是不小心？”",
	},
	{
		//22
		.name="Never Gonna Give You Up",
		.have=1,
		.read=0,
		.intro="你  被  骗  了。",

	},
	{
		//23
		.name="黑框眼镜",
		.have=1,
		.read=1,
		.intro="杰哥专属封印道具。",
		.content="“让我看看你发育正不正常！” ",
	},
	{
		//24
		.name="鸡汤",
		.have=1,
		.read=0,
		.intro="“哈哈哈哈哈，鸡汤来喽！”",
	},
	{
		//25
		.name="安倍晋三",
		.have=1,
		.read=0,
		.intro="日本第一男枪，和肯尼迪不相上下。",
	},
	{
		//26
		.name="爱心",
		.have=1,
		.read=0,
		.f=1,
		//.function=puthart,
		.intro="来个爱心！",
	},
	{
		//27
		.name="真诚",
		.have=1,
		.read=0,
		.intro="“真诚是最好的必杀技”",
	},
	{
		//28
		.name="故乡",
		.have=1,
		.read=0,
		.intro="当你开始用特定的词描述它时，你就开始在失去它了。",
	},
	{
		//29
		.name="匿名的包裹",
		.have=1,
		.read=1,
		.intro="匿名的包裹旁边躺着它的主人。",
		.content="【……娜塔莎诊所  收。】",
	},
	{
		//30
		.name="鲸落",
		.have=1,
		.read=1,
		.intro="那里也会有梦吗？有深不见底的海吗？",
		.content="【。】",
	},
	{
		//31
		.name="机器之花",
		.have=1,
		.read=1,
		.intro="曾戴在少女头上的铁质装饰，现在永远与她的父亲呆在了一起。\n\"她不是为埃瓦德而生的哈达莉，她只是哈达莉而已。\"",
		.ms=40,
		.content="\
		\\.\\.【谢谢……\\.\\.你是我的所有者吗？】\n\
		\\.\\.【不是哦】\n\
		\\.\\.【那我的所有者是不存在的吗？】\n\
		\\.\\.【你自己就是你的所有者】\n\
		\\.\\./*眨巴眼*/\\.\n\
		\\.\\.【那我……\\.应该怎么办呢？】\n\
		\\.\\.【这个也要你自己决定】\n\
		\\.\\.【……我不是很清楚】\n\
		\\.\\.【你有什么想做的事吗？比如说想变成人类、想去城里看看、想要一个名字之类的】\n\
		\\.\\./*少女默默思考了一会儿。*/\n\
		\\.\\.【我想就这样停止运作】\n\
		\\.\\.【……唔————】\n\
		\\.\\.【因为这个世界……难道不是一个充满痛苦的地方吗？】\n\
		\\.\\.【也不尽然哦。也有很多有趣的事和漂亮的东西】\n\
		\\.\\.【…………\\.大概是什么比例呢？】\n\
		\\.\\.【比例？】\n\
		\\.\\.【好事和坏事的比例】\n\
		\\.\\.【差不多是1比9？】\n\
		\\.\\.【简直就是地狱，永别了】\n\
		/*她捡起石头砸起了自己的脑袋*/\n\
		\\.\\.【等下等下。你这样做没用的，Ae型可不会那么容易死掉啊】\n\
		\\.\\.【那我怎么才能停止运作呢？】\n\
		\\.\\.【用枪把头部彻底摧毁。不过你自己应该做不到，那就只能请别人帮忙了】\n\
		\\.\\.【请你帮我】\n\
		\\.\\.【才不要，那我会睡不好觉的】\n\
		\\.\\.【为什么造物主没有为我准备一个停止开关呢】\n\
		\\.\\.【如果你实在想死，可以沿着这片荒野一直向西走，总有一天能真的就死了。\\.因为那一片是危险地区】\n\
		\\.\\.【………我不想要漫长的痛苦】\n\
		\\.\\.【你呀，性格上反倒更适合拼命又长命地苟活下去呢】\n\
		\\.\\.【我觉得没有这回事】\n\
		\\.\\.【你可以往西走然后死掉,要是不愿意那就跟我走】\n\
		\\.\\.【刚才那些人说……我是可以卖掉换钱的？】\n\
		\\.\\.【卖掉你都能变成大富翁啦。如果把你拿去拍卖掉的话，想要什么东西都不在话下】\n\
		\\.\\.【………烂透了，果然还是永别了】\n\
		/*砰砰砰*/\n\
		/*砰砰砰*/\n\
		\\.\\.【都说了，这种程度只会马上痊愈的。\\.你打算永远这么砸下去吗？】\n\
		\\.\\.【……我投降了】\n\
		\\.\\.【我不会把你卖掉的】\n\
		\\.\\.【你讨厌金钱吗?】\n\
		\\.\\.【我可喜欢了。永远都会不嫌多呢】\n\
		\\.\\.【那个……就算你想骗我，那也得花点心思让我产生点信任感吧？】\n\
		\\.\\.【我倒是没有想要骗你啊】\n\
		\\.\\.【这个世界是个可怕的地方，我没有活下去的自信】\n\
		\\.\\.【我会教你的。教你怎么在这个世界上生存下去，而且是免费的】\n\
		\\.\\.【刚才我就一直在想。你也许只是把那群人的成果据为了己有。跟着你，可能会有更痛苦的事在等着我。\n\t\t\\.这让我很害怕】\n\
		\\.\\.【……抱歉，接下来我要讲一句恶棍爱说的标准合词了\\.\\.\\C[12]/你都生下来了，就别想着还能死得轻松了/\\C[7]】\n\
		\\.\\.【呜，地狱】\n\
		\\.\\.【不过，如果说有一个方法可以使你轻松一些的话……那就是积累经验让自己更加强大】\n\
		\\.\\.【强大……?】\n\
		\\.\\.【心灵变得足够强大，就能坦然面对过去害怕的事情了。\\.把1比9逆转为9比1也是有可能的】\n\
		\\.\\.【…………那在我变强大之前，谁会来保护我呢？】\n\
		\\.\\.【所以说我会嘛】\n\
		\\.\\.【为什么？这对你有任何好处吗？】\n\
		\\.\\.【首先，让大家知道了传说中的Ae型真实存在，有一部分原因在于我。我稍微感到了一点点的责任。\n\
		\\.\\.如果由于我的原因,导致有孩子受到了奴隶般的对待，我会想去把他救出来。而且这也能提高我爸爸的声誉】\n\
		\\.\\.【你的父亲……那么你是人类呀】\n\
		\\.\\.【不是，我是Ae型仿生人，和你一样】\n\
		\\.\\.【…………嗯？】\n\
		\\.\\.【这个问题说来话长，我们以后再讲。理由嘛差不多就是那样】\n\
		\\.\\.【……现在我更加困惑了】\n\
		\\.\\.【一次弄明白所有事情是不可能的啦。\\.\\.选一个吧。是去西边死掉，还是跟我走】\n\
		/*少女皱起了眉头*/\n\
		\\.\\.【我不知道该如何才能信任你】\n\
		\\.\\.【没有那种方法，只有选了试试才知道】\n\
		\\.\\.【…………我很害怕】\n\
		\\.\\.【那给你起个名字吧】\n\
		\\.\\.【名字？】\n\
		\\.\\.【如果我是个坏人，大概不会给打算卖掉的商品起什么名字对吧？】\n\
		\\.\\.【…………会是什么名字呢?】\n\
		\\.\\.【我想到一个，普罗菲】\n\
		\\.\\.【……还挺可爱的。有什么含义吗？】\n\
		\\.\\.【利润】\n\
		\\.\\.【永别了】\n\
		\\.\\.【等一下，还有暴利的意思呢!】\n\
		\\.\\.【那感觉也很不好】\n\
		\\.\\.【你觉得有人会想买名字叫暴利的商品吗?】\n\
		\\.\\.【喔喔。那就叫这个了。我想你应该是个好人，请你多多关照了】\n\
		\\.\\.【你可真实在呀，这也蛮重要的就是了】\n\
		\\.\\.【那，过来吧】\n\
		/*菲莉娅伸出了手。*/\n\
		\\.\\.\\.\\./*这位刚刚诞生的名为普罗菲的少女，身体猛地往后缩了一下。*/\n\n\n\t\t\
		\\.\\.\\.\\.\\.\\.\\.\\./*她鼓起了所有的勇气，将手指缓缓地乘上了那只手。*/",
	},
};

struct Relics relics[5][200]= {

	{
		//头部
		{
			.n=0,
			.attribute={
				.crit_rate=0.2,
				.crit_damage=0.13,
				.damage=230,
				.damage_rate=0.43,
				.hp=125,
				.hp_rate=0.17,
				.def=0,
				.def_rate=0.13,
				.miss=0.05,
				.cure=0.2,
			},
		},

	},

	{
		//胸部
		{
			.n=1,
			.attribute={
				.crit_rate=0.13,
				.crit_damage=0.54,
				.damage=230,
				.damage_rate=0.03,
				.hp=125,
				.hp_rate=0.17,
				.def=0,
				.def_rate=0.13,
				.miss=0.05,
				.cure=0.2,
			},
		},

	},

	{
		//腿部
		{
			.n=2,
			.attribute={
				.crit_rate=0.2,
				.crit_damage=0.13,
				.damage=230,
				.damage_rate=0.43,
				.hp=125,
				.hp_rate=0.17,
				.def=0,
				.def_rate=0.13,
				.miss=0.05,
				.cure=0.2,
			},
		},

	},

	{
		//足部
		{
			.n=3,
			.attribute={
				.crit_rate=0.2,
				.crit_damage=0.23,
				.damage=230,
				.damage_rate=0.43,
				.hp=125,
				.hp_rate=0.17,
				.def=264,
				.def_rate=0.13,
				.miss=0.05,
				.cure=0.2,
			},
		},

	},

	{
		//手部
		{
			.n=4,
			.attribute={
				.crit_rate=0.2,
				.crit_damage=0.13,
				.damage=230,
				.damage_rate=0.43,
				.hp=125,
				.hp_rate=0.17,
				.def=0,
				.def_rate=0.13,
				.miss=0.05,
				.cure=0.2,
			},
		},

	},
};






/*------------初始化玩家------------------*/
struct Player players[4]= {
	{
		.name="文止墨\0",
		.hp=1000,
		.f_hp=1000,
		.hd=1000000,
		.f_hd=1000000,
		.attack=100,
		.n_attack=100,
		.defence=100,
		.n_defence=100,
		.miss=10,
		.n_miss=10,
		.speed=100,
		.n_speed=100,
		.sp={0,1,2,3,4,5,6,0,0,0,0,0,0,0,0},
		.lever=1,
		.exp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		.lexp={0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
		//因为在主函数外初始化防具和武器会报错，我们在main函数里面初始化
		.team=1,
		.hparr={84.6122,84.6122},
		.hdarr={19.0714,111.9286},
		.defarr={47,13},
		.attarr={348.6327,-268.6327},
	},
	{
		.name="小睿孩\0",
		.hp=1000,
		.f_hp=1000,
		.hd=1000000,
		.f_hd=1000000,
		.attack=100,
		.n_attack=100,
		.defence=100,
		.n_defence=100,
		.miss=10,
		.n_miss=10,
		.speed=100,
		.n_speed=100,
		.sp={0,1,2,3,4,5,6,0,0,0,0,0,0,0,0},
		.lever=1,
		.exp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		.lexp={0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
		.team=1,
		.hparr={84.6122,84.6122},
		.hdarr={19.0714,111.9286},
		.defarr={47,13},
		.attarr={348.6327,-268.6327},
	},
	{
		.name="。\0",
		.hp=1000,
		.f_hp=1000,
		.hd=1000000,
		.f_hd=1000000,
		.attack=100,
		.n_attack=100,
		.defence=100,
		.n_defence=100,
		.miss=10,
		.n_miss=10,
		.speed=100,
		.n_speed=100,
		.sp={0,1,2,3,4,5,6,0,0,0,0,0,0,0,0},
		.lever=1,
		.exp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		.lexp={0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
		.team=1,
		.hparr={84.6122,84.6122},
		.hdarr={19.0714,111.9286},
		.defarr={47,13},
		.attarr={348.6327,-268.6327},
	},
	{
		.name="花落叶相随\0",
		.hp=1000,
		.f_hp=1000,
		.hd=1000000,
		.f_hd=1000000,
		.attack=100,
		.n_attack=100,
		.defence=100,
		.n_defence=100,
		.miss=10,
		.n_miss=10,
		.speed=100,
		.n_speed=100,
		.sp={0,1,2,3,4,5,6,0,0,0,0,0,0,0,0},
		.lever=1,
		.exp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		.lexp={0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
		.team=1,
		.hparr={84.6122,84.6122},
		.hdarr={19.0714,111.9286},
		.defarr={47,13},
		.attarr={348.6327,-268.6327},
	},
};
/*--------------下面初始化BUFF-------------*/
struct BUFF buffs[50]= {
	{
		//0 示例
		.name="示例buff",
		.n=15,
		.resistance= {0,0,0,0,0,0,0},
		.gain= {0,0,0,0},
		.t_gain= {0,0,0,0},
		.t=0,
	},
	{
		//1 持续治疗
		.name="持续治疗",
		.n=10,
		.resistance={0,0,0,0,0,0,0},
		.gain={0,0,0,0},
		.t_gain={0,0,100,0},
		.t=2,
	},
};




/*-----------下面初始化技能---------*/


struct Skill skill_list[50]= {
	{
		//0
		.num=1,
		.c=1,
		.name="弹跳碰撞\0",
		.suffix="\n\0",
		.sp=0,
		.damage=1,
		.boo=0,
		.hp=0,
		.ele=15,
		//.stu=1,
	},
	{
		//1
		.num=1,
		.c=1,
		.a_num=1,
		.name="弹跳碰撞\0",
		.suffix="\n\0",
		.sp=0,
		.damage=1,
		.boo=0,
		.hp=0,
		.ele=15,
		.status=0,
		.intro="物理属性单体攻击",
		//.stu=1,
	},
	{
		//2
		.num=2,
		.c=1,
		.a_num=6,
		.name="[Error] conflicting types for 'attackact'\0",
		.suffix="\n\0",
		.sp=0,
		.damage=2,
		.boo=0,
		.hp=0,
		.ele=4,
		.status=0,
		.intro="锟斤拷烫烫烫",
		//.stu=1,
	},
	{
		//3
		.num=3,
		.c=0,
		.a_num=TeamNum,
		.name="扣一复活亲妈\0",
		.suffix="\n\0",
		.sp=0,
		.damage=-1,
		.boo=1,
		.hp=1000,
		.ele=10,
		.status=1,
		.intro="治疗术",

	},
	{
		//4
		.num=4,
		.c=1,
		.a_num=1,
		.name="背摔\0",
		.suffix="就因为他做得到！\n\0",
		.sp=0,
		.damage=1,
		.boo=0,
		.hp=0,
		.ele=15,
		.status=0,
		.intro="物理属性单体攻击",
		//.stu=1,
	},
	{
		//5
		.num=5,
		.c=1,
		.a_num=4,
		.name="吞梦\0",
		.suffix="是谁在远处呼唤？\n\0",
		.sp=0,
		.damage=1,
		.boo=0,
		.hp=0,
		.ele=12,
		.status=0,
		.intro="火属性复数(4)攻击",
		//.stu=1,
	},
	{
		//6
		.num=3,
		.c=0,
		.a_num=1,
		.name="肘击\0",
		.suffix="\n\0",
		.sp=0,
		.damage=2,
		.boo=0,
		.hp=0,
		.ele=10,
		.status=0,
		.intro="MAN!！！！！",

	},



};

/*-----------下面初始化各个怪物-----------*/
struct Monster monsters[100]= {
	{
		//0
		.name="史莱姆\0",
		.hp=100,//初始100血
		.attack=10,
		.defence=10,
		.exp={0,0,1},
		.v="跳了过来\0",

	},
	{
		//1
		.name="史莱姆\0",
		.hp=100,//初始100血
		.attack=10,
		.defence=10,
		.miss=5,
		.speed=10,
		.action_num=3,
		.action={
			{
				.skill={1,1},
				.condition=0,
				.a=0,
				.b=0,
				.c=100,
			},
			{
				.skill={2,2},
				.condition=0,
				.a=0,
				.b=0,
				.c=50,
			},
			{
				.skill={3,3},
				.condition=1,
				.a=0,
				.b=3,
				.c=100,
			},
		},
		.exp={0,0,1},
		.v="跳了过来\0",

	},
	{
		//2
		.name="ERROR怪\0",
		.hp=999999999,
		.attack=10,
		.defence=10,
		.action_num=3,
		.miss=5,
		.speed=10,
		.action={
			{
				.skill={1,1},
				.condition=0,
				.a=0,
				.b=0,
				.c=100,
			},
			{
				.skill={2,2},
				.condition=0,
				.a=0,
				.b=0,
				.c=50,
			},
			{
				.skill={3,3},
				.condition=1,
				.a=0,
				.b=3,
				.c=100,
			},
		},
		.exp={0,0,0,2,0,0,0,0,0,0,0,0,0,0},
		.v="出现了\0",

	},
	{
		//3
		.name="艾瑟拉\0",
		.hp=10000,
		.attack=10,
		.defence=10,
		.miss=5,
		.speed=10,
		.action_num=1,
		.action={
			{
				.skill={5,5},
				.condition=0,
				.a=0,
				.b=0,
				.c=100,
			},
		},
		.exp={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
		.v="从你的梦境中浮现\0",

	},
	{
		//4
		.name="科比·布莱恩特\0",
		.hp=10000,
		.attack=24,
		.defence=24,
		.miss=5,
		.speed=10,
		.action_num=1,
		.action={
			{
				.skill={6,5},
				.condition=0,
				.a=0,
				.b=0,
				.c=100,
			},
		},
		.exp={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
		.v="从天上坠落\0",

	},

};

struct Egroup egroups[100]= {
	{
		.name="测试敌群0",
		.num=5,
		.monster={1,1,1,4,2},
	},
};

/*--------------下面初始化对话节点----------------*/

struct ConNode conversations[500]= {
	{
		//0
		.fight=0,
		.content="这是一个测试文本0。没有选择。不会开启任务",
		.ifchoose=0,
		.start_task=0,
		.check_task=0,
	},

	{
		//1
		.fight=0,
		.content="这是一个测试文本1。没有选择。不会开启任务",
		.ifchoose=0,
		.start_task=0,
		.check_task=0,
	},
	{
		//2
		.fight=0,
		.content="这是一个测试文本2。有选择。不会开启任务",
		.ifchoose=5,
		.chooselist={"C1","C2","C3","C0","C5"},
		.next={1,2,3,0,0},
		.start_task=0,
		.check_task=0,
	},
	{
		//3
		.fight=0,
		.content="这是一个测试文本3。无选择。会开启任务",
		.ifchoose=0,
		.start_task=1,
		.check_task=0,
	},
	{
		//4
		.fight=0,
		.content="\n在你回去的路上，有个女人拦住了你。\\W女人：那个……你是他们口中的勇者吗？”\n\n这个女人的脸上带着一丝恐惧，很显然她看见了超出自己认知的事物。\n\n“我需要你的帮助，先生。我在那边的废墟里看到了一只巨大的鲸鱼。但它好像被什么东西重创了。\
					\n\t我很害怕，先生，你能帮帮我吗？或者哪怕只是陪我去看一眼？”\\W在这种地方遇见一个女人可太奇怪了————这又不是什么傻卵二次元游戏，你想————但她的表情又指出这不是空口而谈。\
					\n\n你决定————",
		.ifchoose=2,
		.chooselist={"没问题","离我远点，可疑的女人","\0","\0","\0"},
		.next={6,5,0,0,0},
		.start_task=0,
		.check_task=0,
	},
	{
		//5
		.fight=0,
		.content="“非常抱歉，女士。”  你很遗憾地告诉她你家里煤气忘记关了，没时间管什么居民委托，“我不得不尽快回去。”\\W女人张了张嘴，最后失望的独自离去了。",
		.ifchoose=0,
		.start_task=0,
		.check_task=0,
	},
	{
		//6
		.fight=0,
		.content="\n“没问题，小姐。你可以向我详细说说是什么情况吗？”助人为乐一直是你的美德————在大部分情况下如此————因此你毫不犹豫的同意了她的请求。反正你自认为自己的战斗力睥睨全球，不怕有什么阴招。\
		\n“哦，哦。好的，我们边走边说吧”女人没想到你答应的这么干脆，顿时喜出望外。",
		.ifchoose=1,
		.chooselist={"跟她一起去看看","C2","C3","C0","C5"},
		.next={7,0,0,0,0},
		.start_task=0,
		.check_task=0,
	},
	{
		//7
		.fight=0,
		.content="“我叫薇，先生。”女人自我介绍一番后，开始讲述她所见的一切：\
				\\W\\C[8]薇在从城里回家的时候目睹了一片巨大的黑影从天空中坠落，一直坠落到森林边缘————也就是薇的家在的地方。\
				\n她小跑过去，发现那是一头浑身散发着淡蓝色光芒的鲸鱼。就在她思考逃跑还是逃跑的时候，那头鲸鱼的光芒闪了一下，随即暗淡下去。与此同时，一只小一点的、带有红紫色花纹的鲸鱼从前者的呼吸孔钻出来，却在它巨大的肉体上陷入沉睡。\
				\n\n\\C[7]听到这里你的眉头皱了起来，薇所描述的巨大鲸鱼与你曾在村子里看到的一本典籍上所描述的一致————它是突然出现在40年前的强大生物，每次现身都为当地带来不可挽回的伤亡。最重要的是，与它战斗过的人大都患上了严重的失眠症，最终与痛苦和幻觉中死去。\
				\n\n不过根据薇的描述，巨鲸“艾瑟拉”显然是被某种东西所重创了，以至于使其不得不依靠蜕生来免于死亡。这种状态下的艾瑟拉也许不足以将你打败，是个机会，你想。\
				\n\n同时，你在薇的描述里发现了几个疑点：那片森林毫无人烟，她是如何在那里建起房子的？为什么没人听说过她？\
				\n\n但你决定继续听她说下去，兴许是鲸鱼对记忆的影响让她混淆了这些消息。\
				\\W“那东西究竟是什么？我会不会得什么怪病？”薇有些焦虑，她在介绍完所见的一切后，不免开始担心自己。",
		.ifchoose=1,
		.chooselist={"（真要有什么大问题你也不会活着见到我了……）","C2","C3","C0","C5"},
		.next={8,0,0,0,0},
		.start_task=0,
		.check_task=0,
	},
	{
		//8
		.fight=0,
		.fight_continue=1,
		.content="但你并没有将这句话说出去，因为在你们面前就是艾瑟拉巨大的尸体，以及愤怒的小鲸鱼。",
		.ifchoose=0,
		.chooselist={"\0","\0","\0","\0","0"},
		.next={9,0,0,0,0},
		.start_task=0,
		.check_task=0,
	},
	{
		//9
		.fight=0,
		.content="这是一个测试文本9。无选择。会尝试提交任务\\W",
		.ifchoose=0,
		.start_task=0,
		.check_task=1,
		.finish_next_con={10,11},
	},
	{
		//10
		.fight=0,
		.content="你还没有完成任务！\\W",
		.ifchoose=0,
		.start_task=0,
		.check_task=0,
	},
	{
		//11
		.fight=0,
		.content="你完成任务！\\W",
		.ifchoose=0,
		.next={0},
		.start_task=0,
		.check_task=0,
	}
};



/*--------------------下面初始化任务----------------*/

struct Task tasks[500]= {
	{
		//0
		.name="任务0",
		.start=1,
		.finish=1,
		.father=0,
		.sonnum=0,
		.son={0,0,0,0,0},
		.condition={0,0,0,0},
		.lever=0,
		.intro="这里是任务0内容描述。intro。",
		.con_s=0,
		.give_prop=159,//绿色星星
		.give_prop_num=1000,
		.give_souvenir=1,
		.give_exp={0,0,0,0,0,0,0,0,0,0,0,0},
		.give_gold={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		.next=0,
	},

	{
		//1
		.name="任务1",
		.see=1,
		.npc_s=2,
		.npc_f=2,
		.con_s=3,
		.con_f=9,
		.start=0,
		.finish=0,
		.father=0,
		.sonnum=3,
		.son={3,4,5,0,0},
		.condition={0,0,0,0},
		.lever=0,
		.intro="这里是任务1内容描述。intro。",

		.f_condition={0,0,0,0},
		.give_prop=159,//绿色星星
		.give_prop_num=1000,
		.give_souvenir=1,
		.give_exp={0,0,0,2,0,2,0,0,0,0,0,0},
		.give_gold={0,0,0,2,0,0,0,0,0,0,0,0,0,0,0},
		.next=2,
	},

	{
		//2
		.name="鲸落",
		.start=0,
		.finish=0,
		.father=0,
		.sonnum=0,
		.son={0,0,0,0,0},
		.condition={0,0,1,0},
		.lever=0,
		.intro="与薇一同打败鲸鱼。",
		.con_s=0,
		.f_condition={0,0,1,0},
		.give_prop=159,//绿色星星
		.give_prop_num=1000,
		.give_souvenir=1,
		.give_exp={0,0,0,0,0,0,0,0,0,0,0,0},
		.give_gold={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		.next=0,
	},
	{
		//3
		.name="任务1-子任务1",
		.see=1,
		.npc_s=0,
		.npc_f=0,
		.con_s=0,
		.start=0,
		.finish=0,
		.father=1,
		.sonnum=0,
		.son={0,0,0,0,0},
		.condition={0,0,0,0},
		.lever=0,
		.intro="这里是任务1子任务1内容描述。intro。",

		.f_condition={0,0,0,0},
		.give_prop=159,//绿色星星
		.give_prop_num=1000,
		.give_souvenir=1,
		.give_exp={0,0,0,2,0,0,0,0,0,0,0,0},
		.give_gold={0,0,0,2,0,0,0,0,0,0,0,0,0,0,0},
		.next=0,
	},
	{
		//4
		.name="任务1-子任务2",
		.see=1,
		.npc_s=0,
		.npc_f=0,
		.con_s=0,
		.start=0,
		.finish=0,
		.father=1,
		.sonnum=0,
		.son={0,0,0,0,0},
		.condition={0,0,0,0},
		.lever=0,
		.intro="这里是任务1子任务2内容描述。intro。",

		.f_condition={0,0,0,0},
		.give_prop=159,//绿色星星
		.give_prop_num=1000,
		.give_souvenir=1,
		.give_exp={0,0,0,2,0,0,0,0,0,0,0,0},
		.give_gold={0,0,0,2,0,0,0,0,0,0,0,0,0,0,0},
		.next=0,
	},
	{
		//5
		.name="任务1-子任务3",
		.see=1,
		.npc_s=0,
		.npc_f=0,
		.con_s=0,
		.start=0,
		.finish=0,
		.father=1,
		.sonnum=0,
		.son={0,0,0,0,0},
		.condition={0,0,0,0},
		.lever=0,
		.intro="这里是任务1子任务3内容描述。intro。",

		.f_condition={0,0,0,0},
		.give_prop=159,//绿色星星
		.give_prop_num=1000,
		.give_souvenir=1,
		.give_exp={0,0,0,2,0,0,0,0,0,0,0,0},
		.give_gold={0,0,0,2,0,0,0,0,0,0,0,0,0,0,0},
		.next=0,
	},





};

/*----------------------下面初始化NPC---------------*/

struct NPC npcs[50]= {
	{
		//0
		.name="f+g/h%￥%…-…*",
		.var={0},
		.con=&conversations[0],
	},
	{
		//1
		.name="薇",
		.var={0},
		.con=&conversations[4],
	},
	{
		//2
		.name="测试npc",

		.task={1,1},
		.var={0},
		.nomal_con=1,
		.con=&conversations[1],
	}




};


/*----------下面声明函数-----------*/



void iniarr(int a[]);
//初始化数组

void copyarr(int ary1[],int ary2[]);
//将后者数组的元素复制到前者。参量为两个数组以及它们的长度。会清除原本数组的内容。

void plusarr(int a1[],int a2[]);
//高精度加法，第二个加到第一个里面。如果已经达到最大位无法进位，则最大位不变

void minusarr(int a1[],int a2[]);
//高精度减法，第一个减去第二个

void mularr(int a[],float n);
//高精度乘法

int comparr(int ary1[],int ary2[]);
//高精度比较两个数的大小，前者大于后者返回1，小于返回-1，等于返回0

void putheart(void);
//在屏幕上输出一个爱心！

void SlowDisplay(const char *text,int n);
//输出字符函数

void putarr(int a[]);
//输出数据的函数

void leverUP(int i);
//判断是否升级并执行

int attackact(struct Egroup egroup,int e);
//攻击行为函数。运行此函数将进入战斗直到一方倒下或逃跑。胜利返回1，失败返回0;e表示是否可以逃跑

int random(void);
//以当前时间（秒为单位）为种子生成一个0～99的随机数

void hotel(void);
//回满状态的函数，在旅馆等地点调用

void bag(void);
//察看背包内容函数

void conversation(struct Player *player,struct NPC *npc);
//对话函数


int takechoose(int i);
//选择函数，包括数字输入、WASD选择和方向键选择.i表示choosenum能达到的最大值+1
//按Q返回0，表示退出选择;按R返回2，表示阅读(纪念品);回车返回3，表示确定;按P返回4，表示运行;平时返回1。
//数字输入时，按删除键执行choosenum/10,即回退一位数字。

int checktask(struct Task *task);
//检查任务函数，检查任务是否满足开始条件(返回1/0)、任务是否开启(返回2)、任务是否完成(返回3)、任务是否结束(返回4)
//优先级从前往后升高

void wait(void);
//等待函数负责在一些地方等待用户按任意键确认

struct Prop *index2prop(int n);
//通过物品编号返回结构体

void check_npc_task(struct NPC *npc);
//检查该npc所有相关任务是否完成，并对该npc的对话起点做出相应修改。

void tasktree(void);
//输出任务树

void creat_relics(void);
//圣遗物制造机

int srandom(int n);
//以当前时间（秒为单位）加上指定数值为种子生成一个0～99的随机数

void count(void);
//计算攻击力等等数值，用于更换圣遗物后

/*---------定义变量，因为函数也要用到这些变量，所以定义到main外面--------*/

int choosenum=-1;//记录玩家的选择

int ablank[LEN];//空数组，用于判断血量等是否为0；

struct Skill sarr[20];//储存当前技能列表

int spnum=4;//当前拥有的技能数量

struct Prop *parr[250];//战斗模块和背包查看中道具列表

struct Souvenir *soarr[50];//背包查看纪念品列表

int MS=DEFAULT_MS;//输出间隔

//int tasklist[50];//任务列表（暂时不需要这种东西

int gold[LEN];//金币数量

char *waring_content="你瞎输牛魔呢？滚啊！！！";

int checktask_result=0;//记录在SlowDisplay中\\T检查的结果

int relics_num[5]= {0,0,0,0,0}; //记录背包中各个部位圣遗物的数量
/*---------main函数-----------*/
int main() {
	/*初始化各种数组*/
	iniarr(ablank);
	iniarr(gold);
	/*初始化玩家道具*/
	players[0].defp=&fj[0];
	players[0].attp=&wq[0];
	players[1].defp=&fj[0];
	players[1].attp=&wq[0];
	players[2].defp=&fj[0];
	players[2].attp=&wq[0];
	players[3].defp=&fj[0];
	players[3].attp=&wq[0];
	/*初始化怪物掉落物*/
	monsters[1].prop=&wq[2];
	monsters[2].prop=&wq[3];
	/*初始化结构体函数指针*/
	souvenir[26].function=putheart;
	/*SlowDisplay("这里本来应该写背景故事的，\n但是作者也不知道背景故事是什么。。。\n勇敢的少年啊，快快告诉我你的名字，去创造神话吧！\n(200个字符以内)：",0);
	char player_name[200];
	scanf("%s", player_name);
	size_t namelength = strlen(player_name);//判断玩家名字长度
	strncpy(players[0].name, player_name,namelength+3 );//将玩家输入的名字复制到玩家结构体对应的变量中*/


	while(1) {
		system("cls");

		//conversation(&players[0],&npcs[2]);//测试任务系统
		//tasktree();
		//conversation(&players[0],&npcs[1]);//薇
		//bag();
		count();
		hotel();
		attackact(egroups[0],1);
	}

	//putheart();
	getchar();
	return 0;
}

void iniarr(int a[]) { //初始化数组
	for(int i=0; i<LEN; i++)
		a[i]=0;
}

void copyarr(int ary1[],int ary2[]) { //将后者数组的元素复制到前者。参量为两个数组以及它们的长度。会清除原本数组的内容。
	for(int i=0; i<LEN; i++) //初始化第一个数组
		ary1[i]=0;
	for(int i=0; i<LEN; i++) {
		ary1[i]=ary2[i];
	}
}

void plusarr(int a1[],int a2[]) { //高精度加法，第二个加到第一个里面。如果已经达到最大位无法进位，则最大位不变
	int length=LEN-1;
	while(a1[length]==9)length--;//如果最大位已经是9了，则认为新的最大位为上一位。避免出现9xxxxxxxxx + 1的结果反而比原数小的情况
	for(int i=0; i<=length; i++)
		a1[i]+=a2[i];
	//putarr(a1);
	//printf("\n");
	int boo=0;//0表示这个结果是负数
	for(int j=LEN-1; j>=0; j--) {
		if(a1[j]>0) {
			boo=1;
			//printf("\nZZZ\n");
			break;
		}
		if(a1[j]<0) {
			//printf("\nFFF\n");
			break;
		}
	}
	if(boo==1) { //正数进位

		for(int i=0; i<length; i++) {
			if(a1[i]>9) {
				if(i!=length-1) {
					a1[i+1]+=1;
					a1[i]%=10;
				} else {
					a1[i]=9;
				}
			}
			if(a1[i]<0) {
				a1[i]+=10;
				a1[i+1]-=1;
			}
		}
	} else { //负数的进位
		//printf("\nfu\n");
		for(int i=0; i<LEN; i++) {
			if(a1[i]>0) {
				if(i!=length-1) {
					//printf("%d",a1[i]);
					a1[i]-=10;
					a1[i+1]+=1;
					//printf("  %d  %d\n",a1[i],a1[i+1]);
				}
			}
			if(a1[i]<-10) {

				if(i!=length-1) {
					a1[i+1]-=a1[i]/10;
					a1[i]%=10;
				} else {
					break;
				}
			}
		}
	}

}

void minusarr(int a1[],int a2[]) { //高精度减法，第一个减去第二个。
	int length=LEN;//这里不是写错了，就是LEN，不用减一
	for(int i=0; i<LEN; i++)
		a1[i]-=a2[i];
	//putarr(a1);
	int boo=0;//0表示这个结果是负数
	for(int j=LEN-1; j>=0; j--) {
		if(a1[j]>0) {
			boo=1;
			//printf("\nZZZ\n");
			break;
		}
		if(a1[j]<0) {
			//printf("\nFFF\n");
			break;
		}
	}
	//printf("\n");
	if(boo==1) { //正数的进位
		//printf("\nzheng\n");
		for(int i=0; i<LEN; i++) {
			if(a1[i]<0) {
				if(i!=length-1) {
					a1[i+1]-=1;
					a1[i]+=10;
				} else {
					break;
				}
			}
		}
	} else { //负数的进位
		//printf("\nfu\n");
		for(int i=0; i<LEN; i++) {
			if(a1[i]>0) {
				if(i!=length-1) {
					//printf("%d",a1[i]);
					a1[i]-=10;
					a1[i+1]+=1;
					//printf("  %d  %d\n",a1[i],a1[i+1]);
				}
			}
			if(a1[i]<-10) {

				if(i!=length-1) {
					a1[i+1]-=a1[i]/10;
					a1[i]%=10;
				} else {
					break;
				}
			}
		}
	}
}



void mularr(int a[],float n) { //高精度乘法
	int length=LEN;
	while(a[length]==9)length--;//如果最大位已经是9了，则认为新的最大位为上一位(与加法同理)
	for(int i=0; i<length; i++)
		a[i]*=n;
	for(int i=0; i<length; i++) {
		if(a[i]>9) {
			if(i!=length-1) {
				a[i+1]+=a[i]/10;
				a[i]%=10;
			} else {
				a[i]=9;
			}
		}
	}
}

int comparr(int ary1[], int ary2[]) {//比较，前者大于后者返回1，小于返回-1，等于返回0
	for (int k = LEN - 1; k >= 0; k--) {
		if (ary1[k] > ary2[k]) {
			return 1; // ary1 大于 ary2
		} else if (ary1[k] < ary2[k]) {
			return -1; // ary2 大于 ary1
		}
	}
	return 0; // 两个数组相等
}

void putheart(void) { //在屏幕上输出一个爱心！
	int c=0;
	for(int i=0; i<50; i++) {
		c=0;
		if(i==2||i==3||i==5||i==6||i==46)c=68;
		if(i>7&&i<29)c=68;
		if(i>29&&i<35)c=68;
		if(i>37&&i<41)c=68;
		COLOR(c);
		printf("%3d",i);
		COLOR(0);
		if(i%7==0)printf("\n");
	}
	COLOR(7);
}

void SlowDisplay(const char *text,int n) {//n=0说明需要缓慢输出并跳过暂停控制符；n=1说明直接输出内容,这两种情况都不会影响字体颜色改变

	for (int i = 0; text[i] != '\0'; i++) {
		if (text[i] == '\\' && text[i + 1] == '.') {
			if(n==0)usleep(100*1000); // 暂停1秒
			i++; // 跳过'.'
		} else if(text[i] == '\\' && text[i + 1] == 'C'&&text[i+2]=='[') {
			int color=0;
			i+=2;
			while(text[++i]!=']') {
				color*=10;
				color+=text[i]-'0';
			}
			//printf("\n%d\n",color);
			COLOR(color);

		} else if(text[i] == '\\' && text[i + 1] == 'W') {
			if(n==0)wait();
			else printf("\n\n\n");
			i++;
		} else if(text[i] == '\\' && text[i + 1] == 'T'&&text[i+2]=='[') {
			int task=0;
			i+=2;
			while(text[++i]!=']') {
				task*=10;
				task+=text[i]-'0';

			}
			checktask_result=checktask(&tasks[task]);

		} else {
			putchar(text[i]); // 逐字符显示
			fflush(stdout);   // 刷新输出缓冲区，确保字符立即显示
			if(n==0)usleep(MS * 1000); // 延迟指定的毫秒数
		}
	}

}


void putarr(int a[]) { //输出数据的函数
	int boo=0;
	for(int i=LEN-1; i>=0; i--) {
		if(a[i]!=0)boo=1;
		if(boo==1)printf("%d",a[i]);
	}
}

int random(void) { //以当前时间（秒为单位）为种子生成一个0～99的随机数
	srand((unsigned)time(NULL));
	return rand() % 100;
}

/*struct Skill readskill(int s){//通过技能编号读取技能信息的函数
	struct Skill re;
	if(s==0)re=zero;
	if(s==1)re=one;
	if(s==2)re=two;
	if(s==3)re=three;
	return re;
}*/

void leverUP(int i) { //判断是否升级并执行,i使指结构体数组里第i个元素
	int n=0;
	int one= 1;
	while(comparr(players[i].exp,players[i].lexp)!=-1) {//判断此时玩家经验是否达到升级条件

		n+=one;
		minusarr(players[i].exp,players[i].lexp);//减掉升级所用的经验值
		mularr(players[i].lexp,1.3);
	}
	if(n>one) {
		SlowDisplay("蛙趣，连升",0);
		printf("%d",n);
		SlowDisplay("级！\n",0);
	} else if(n==one) {
		SlowDisplay("升级了！",0);
	}
	players[i].lever+=n;
	count();//重新计算各项数值
	hotel();//升级会清除所有状态
}



int attackact(struct Egroup egroup,int e) {
//战斗模块，赢了返回1输了返回0
	count();
	system("cls");//清除控制台
	SlowDisplay("遇见了 ",0);
	SlowDisplay(egroup.name,0);
	SlowDisplay("！\n",0);
	for(int i=0; i<egroup.num; i++) {
		printf("\t");
		SlowDisplay(monsters[egroup.monster[i]].name,0);
		SlowDisplay(monsters[egroup.monster[i]].v,0);
		SlowDisplay("！\n",0);
	}
	int roundnum=-1;//当前是列表里第几个人行动
	long harm;//攻击产生的伤害
	long hpplus;//技能或道具回复的血量
	//copyarr(harm,ablank);
	struct Player *player;
	struct Monster monsterarr[egroup.num];
	struct Monster *monster;
	int player_buff_num[TeamNum];
	int monster_buff_num[egroup.num];//玩家和怪物的buff数量

	for(int i=0; i<TeamNum; i++) {
		player_buff_num[i]=0;
	}
	for(int i=0; i<egroup.num; i++) { //初始化
		monster_buff_num[i]=0;
	}

	struct BUFF player_buff[TeamNum][20];//玩家buff列表
	struct BUFF monster_buff[egroup.num][20];//怪物buff列表

	for(int i=0; i<egroup.num; i++) {
		monsterarr[i]=monsters[egroup.monster[i]];
	}
	int actnum=TeamNum+egroup.num;
	int speedlist[actnum];//速度列表
	int actlist[actnum];//行动列表(排序后的速度列表)
	for(int i=0; i<actnum; i++) {
		actlist[i]=i;//初始化行动列表
	}


StartAction:

	for(int i=0; i<TeamNum; i++) {//由于可能会在战斗中使用道具使速度改变，因此放在回合开始后初始化
		speedlist[i]=players[i].speed;
	}
	for(int i=TeamNum; i<actnum; i++) {
		speedlist[i]=monsterarr[i-TeamNum].speed;
	}
	for(int i=0; i<actnum; i++) { //选择排序速度列表
		int max=actlist[i];
		for(int j=i; j<actnum; j++) {
			if(speedlist[actlist[j]]>speedlist[actlist[max]])max=j;
		}
		int t=actlist[i];
		actlist[i]=actlist[max];
		actlist[max]=t;
	}
	roundnum++;
	roundnum%=actnum;
	if(actlist[roundnum]<TeamNum) {
		if(players[actlist[roundnum]].hp>0) { //如果当前行动方是一名未死亡的玩家
			player=&players[actlist[roundnum]];
			goto PlayerRound;//跳转到玩家回合
		}
	} else {
		if(monsterarr[actlist[roundnum]-TeamNum].hp>0) {
			monster=&monsterarr[actlist[roundnum]-TeamNum];
			goto MonsterRound;//跳转到怪物回合
		}
	}
	goto StartAction;//不满足要求，重新计算并让下一位行动



	//下面是玩家的回合
PlayerRound:
	printf("%s开始行动了！\n",player->name);
	if(player_buff_num) {//对buff进行处理
		for(int i=0; i<player_buff_num[actlist[roundnum]]; i++) {
			for(int j=0; j<4; j++) {
				if(player_buff[actlist[roundnum]][i].t_gain[j]) {
					SlowDisplay(player->name,0);
					SlowDisplay("受到了来自状态",0);
					COLOR(player_buff[actlist[roundnum]][i].n);
					printf(" %s ",player_buff[actlist[roundnum]][i].name);
					COLOR(7);
					SlowDisplay("施加的",0);
					COLOR(player_buff[actlist[roundnum]][i].n);
					printf(" %ld ",player_buff[actlist[roundnum]][i].t_gain[j]);
					COLOR(7);
					if(j==0) {
						if(player_buff[actlist[roundnum]][i].t_gain[j]>0) {
							SlowDisplay("点攻击力提高！",0);
						} else {
							SlowDisplay("点攻击力降低！",0);
						}
						player->attack+=player_buff[actlist[roundnum]][i].t_gain[j];
						if(player->attack<0)player->attack=1;
					}
					if(j==1) {
						if(player_buff[actlist[roundnum]][i].t_gain[j]>0) {
							SlowDisplay("点防御力提高！",0);
						} else {
							SlowDisplay("点防御力降低！",0);
						}
						player->defence+=player_buff[actlist[roundnum]][i].t_gain[j];
						if(player->defence<0)player->defence=1;
					}
					if(j==2) {
						if(player_buff[actlist[roundnum]][i].t_gain[j]>0) {
							SlowDisplay("点生命值提高！",0);
						} else {
							SlowDisplay("点生命值降低！",0);
						}
						player->hp+=player_buff[actlist[roundnum]][i].t_gain[j];
						if(player->hp<0) {
							SlowDisplay("真丢人！竟然被状态打败了！！",0);
							goto StartAction;
						}
					}
					if(j==3) {
						if(player_buff[actlist[roundnum]][i].t_gain[j]>0) {
							SlowDisplay("点速度提高！",0);
						} else {
							SlowDisplay("点速度降低！",0);
						}
						player->speed+=player_buff[actlist[roundnum]][i].t_gain[j];
						if(player->speed<0)player->speed=1;
					}
				}
			}
			player_buff[actlist[roundnum]][i].t--;
			if(player_buff[actlist[roundnum]][i].t==0) { //该buff已过期
				player_buff_num[actlist[roundnum]]--;
				player_buff[actlist[roundnum]][i] = player_buff[actlist[roundnum]][player_buff_num[actlist[roundnum]]];
				//把列表最后一位移到当前位置
			}
		}
	}
	puts("\n现在要怎么办？");
	puts("=============================================================================");
	if(e) puts("1. 攻击   2. 使用物品   3.  查看   4.  饶恕\n\n");
	else puts("1. 攻击   2. 使用物品   3.  查看\n\n");
	switch(fflush(stdin),choosenum=-1,scanf("%d",&choosenum),system("cls"),choosenum) { //读入玩家选择并判断;fflush(stdin)刷新输入缓冲区
		case 1: {
			int s_index=0,cheat=0;
			for(int i=0; i<LEN; i++) //创建当前技能列表
				if(player->sp[i]!=0)//判断这个技能是否习得
					sarr[s_index++]=skill_list[player->sp[i]];
			if(random()==1)cheat=1;//小概率事件-作弊
			s_index+=cheat;
			choosenum=0;
			while(1) {
				system("cls");
				puts("要用什么技能？用WASD或方向键选择物品,Q退出，回车确定：\n");

				puts("=============================================================================");
				for(int i=0; i<s_index-cheat; i++) { //输出当前技能列表
					if(i%(lineL-1)==0)printf("\n");
					if(i==choosenum)COLOR(240);
					printf("%d.  %s\t\t",i,sarr[i].name);
					COLOR(7);
				}
				if(cheat) {
					if(choosenum==s_index-1)
						COLOR(240);
					printf("   114514.   什么都不做\n");
					COLOR(7);
					printf("\n");
				}
				puts("\n=============================================================================");
				puts(sarr[choosenum].intro);

				//现在技能列表按照其在技能数组里的索引值排列了
				int kg=takechoose(s_index);
				fflush(stdin);

				if(kg==0) { //按Q
					system("cls");
					goto PlayerRound;//攻击-回到玩家回合

				}
				if(kg==3) { //按回车
					puts(" ");
					break;
				}

			}

			if(choosenum==114514&&cheat) {
				SlowDisplay(player->name,0);
				SlowDisplay("说：\"我什么都做不到！\"\n\n",0);
				usleep(1000*1000);
				printf("突然！天空中出现一道光芒！\n");
				SlowDisplay("某处传来一个声音：“版本之子，启动！”\n",0);
				player->attp=&wq[3];
				player->defp=&fj[7];
				player->attack*=9999;
				player->hp*=9999;
				player->hd*=9999;
				usleep(500*1000);
				printf("%s得到了加强！\n%s得到了超模神器！\n\n",player->name,player->name);
				break;
			} else if(choosenum>(sizeof(sarr)/sizeof(sarr[0]))||choosenum==-1) {
				printf("………………\n");
				SlowDisplay(waring_content,0);
				system("cls");//清除控制台
				goto PlayerRound;
			}

			if(player->hd<=sarr[choosenum].sp) { //如果蓝不够
				SlowDisplay("你的法力不足！！\n",0);
				goto PlayerRound;//重新开始这回合
			}

			/*----------这里选择攻击对象------- */
			if(sarr[choosenum].c&&sarr[choosenum].a_num!=6) {
				//该技能作用于敌人且不是全体攻击
				int skillchoosenum=choosenum;//保存技能选择
				choosenum=0;
				int monsterchoose[sarr[skillchoosenum].a_num];//用于保存敌人选择
				int monsterchoosenum=0;//当前选择了几个怪物
				while(1) {
					system("cls");
					puts("要攻击哪个敌人？？用WASD或方向键选择,Q退出,回车确定：\n");
					puts("=============================================================================");
					for(int i=0; i<egroup.num; i++) {
						if(i%(lineL-1)==0)printf("\n");
						if(i==choosenum)COLOR(240);
						printf("%d.  %s",i,monsterarr[i].name);
						if(monsterarr[i].hp<0)printf("(已战败)");
						printf("\t\t\t");
						COLOR(7);
					}
					COLOR(8);
					printf("\n已选择%d/%d",monsterchoosenum,sarr[skillchoosenum].a_num);
					COLOR(7);
					int kg=takechoose(egroup.num);
					fflush(stdin);

					if(kg==0) { //按Q
						system("cls");
						goto PlayerRound;//攻击-回到玩家回合

					}
					if(kg==3) { //按回车
						if(monsterarr[choosenum].hp>0)
							monsterchoose[monsterchoosenum++]=choosenum;
						else {
							puts("\n该敌人已战败，请重新选择。");
							wait();
						}
						if(monsterchoosenum==sarr[skillchoosenum].a_num)
							break;

					}

				}

				system("cls");
				SlowDisplay(player->name,0);
				SlowDisplay("使出了",0);
				SlowDisplay(sarr[skillchoosenum].name,0);
				printf("！");
				SlowDisplay(sarr[skillchoosenum].suffix,0);
				player->hd-sarr[skillchoosenum].sp;//减少蓝
				for(int i=0; i<monsterchoosenum; i++) {
					monster=&monsterarr[monsterchoose[i]];
					if(monster->hp<=0) {
						continue;
					}
					harm=player->attack;//读取攻击力

					harm+=player->attp->num;//武器加成

					harm*=sarr[skillchoosenum].damage;//计算伤害倍率

					if((float)random()/100<player->crit_rate) {
						harm*=player->crit_damage;
					}//暴击

					harm-monster->defence;//计算最终伤害
					SlowDisplay(monster->name,0);
					SlowDisplay("  受到了  ",0);
					COLOR(sarr[skillchoosenum].ele);
					printf("%ld",harm);
					COLOR(7);
					SlowDisplay("  点伤害!\n",0);
					monster->hp-=harm;//减少怪物的hp
					monster_buff[monsterchoose[i]][monster_buff_num[monsterchoose[i]]++]=buffs[sarr[skillchoosenum].status];//给怪挂buff
					SlowDisplay(monster->name,0);
					SlowDisplay("  被附加了  ",0);
					COLOR(buffs[sarr[skillchoosenum].status].n);
					printf("%s",buffs[sarr[skillchoosenum].status].name);
					COLOR(7);
					SlowDisplay(" 状态!\n",0);
					if(monster->hp<=0) {
						printf("%s",monster->name);
						SlowDisplay("倒下了！\n",0);
						monster_buff_num[monsterchoose[i]]=0;//清空buff列表
					}
				}
			} else if(sarr[choosenum].c&&sarr[choosenum].a_num==6) {
				//对敌人全体攻击
				system("cls");
				SlowDisplay(player->name,0);
				SlowDisplay("使出了",0);
				SlowDisplay(sarr[choosenum].name,0);
				printf("！");
				SlowDisplay(sarr[choosenum].suffix,0);
				player->hd-sarr[choosenum].sp;//减少蓝
				for(int i=0; i<egroup.num; i++) {
					if(monsterarr[i].hp<=0)continue;

					harm=player->attack;//读取攻击力

					harm+=player->attp->num;//武器加成

					harm*=sarr[choosenum].damage;//计算伤害倍率
					monster=&monsterarr[i];
					if((float)random()/100<player->crit_rate) {
						harm*=player->crit_damage;
					}//暴击

					harm-monster->defence;//计算最终伤害
					SlowDisplay(monster->name,0);
					SlowDisplay("  受到了  ",0);
					COLOR(sarr[choosenum].ele);
					printf("%ld",harm);
					COLOR(7);
					SlowDisplay("  点伤害!\n",0);
					monster->hp-=harm;//减少怪物的hp
					monster_buff[i][monster_buff_num[i]++]=buffs[sarr[choosenum].status];//给怪挂buff
					SlowDisplay(monster->name,0);
					SlowDisplay("  被附加了  ",0);
					COLOR(buffs[sarr[choosenum].status].n);
					printf("%s",buffs[sarr[choosenum].status].name);
					COLOR(7);
					SlowDisplay(" 状态!\n",0);
					if(monster->hp<=0) {
						printf("%s",monster->name);
						SlowDisplay("倒下了！\n",0);

					}
				}
			} else if(!sarr[choosenum].c) {
				//作用于队友
				int skillchoosenum=choosenum;//保存技能选择
				choosenum=0;
				int teamchoose[sarr[skillchoosenum].a_num];//用于保存队友选择
				int teamchoosenum=0;//当前选择了几个队友
				while(1) {
					system("cls");
					puts("要z作用于哪个队友？？用WASD或方向键选择,Q退出,回车确定：\n");
					puts("=============================================================================");
					for(int i=0; i<TeamNum; i++) {
						if(i%(lineL-1)==0)printf("\n");
						if(i==choosenum)COLOR(240);
						printf("%d.  %s",i,players[i].name);
						if(players[i].hp<0)printf("(已战败)");
						printf("\t\t\t");
						COLOR(7);
					}
					COLOR(8);
					printf("\n已选择%d/%d",teamchoosenum,sarr[skillchoosenum].a_num);
					COLOR(7);
					int kg=takechoose(TeamNum);
					fflush(stdin);

					if(kg==0) { //按Q
						system("cls");
						goto PlayerRound;//攻击-回到玩家回合

					}
					if(kg==3) { //按回车
						if(players[choosenum].hp>0)
							teamchoose[teamchoosenum++]=choosenum;
						else {
							puts("该队友已濒死，请重新选择。");
							wait();
						}
						if(teamchoosenum==sarr[skillchoosenum].a_num)
							break;
					}
				}

				system("cls");
				SlowDisplay(player->name,0);
				SlowDisplay("使出了",0);
				SlowDisplay(sarr[skillchoosenum].name,0);
				printf("！");
				SlowDisplay(sarr[skillchoosenum].suffix,0);
				player->hd-sarr[skillchoosenum].sp;//减少蓝
				if(sarr[skillchoosenum].damage<=0) {//治疗技能
					for(int i=0; i<teamchoosenum; i++) {

						harm=player->attack;//读取攻击力

						harm+=player->attp->num;//武器加成

						harm*=sarr[skillchoosenum].damage;//计算伤害倍率
						struct Player *teamer=&players[teamchoose[i]];

						harm*=teamer->cure;//计算最终治疗
						SlowDisplay(teamer->name,0);
						SlowDisplay("  受到了  ",0);
						COLOR(sarr[skillchoosenum].ele);
						printf("%ld",0-harm);
						COLOR(7);
						SlowDisplay("  点治疗!\n",0);
						teamer->hp-=harm;//增加队友的hp
						if(teamer->hp>teamer->f_hp) {
							teamer->hp=teamer->f_hp;
							SlowDisplay("回满了！\n",0);
						} else {
							SlowDisplay("现在有 ",0);
							printf("%ld",teamer->hp);
							SlowDisplay(" 点血量！\n",0);
						}
						player_buff[teamchoose[i]][player_buff_num[teamchoose[i]]++]=buffs[sarr[skillchoosenum].status];//给队友上buff
						SlowDisplay(teamer->name,0);
						SlowDisplay("  被附加了  ",0);
						COLOR(buffs[sarr[skillchoosenum].status].n);
						printf("%s",buffs[sarr[skillchoosenum].status].name);
						COLOR(7);
						SlowDisplay(" 状态!\n",0);

					}
				}  else { //负面效应
					for(int i=0; i<teamchoosenum; i++) {
						harm=player->attack;//读取攻击力

						harm+=player->attp->num;//武器加成

						harm*=sarr[skillchoosenum].damage;//计算伤害倍率
						struct Player *teamer=&players[teamchoose[i]];

						SlowDisplay(teamer->name,0);
						SlowDisplay("  受到了  ",0);
						COLOR(sarr[skillchoosenum].ele);
						printf("%ld",0-harm);
						COLOR(7);
						SlowDisplay("  点痛击!\n",0);
						teamer->hp-=harm;
						player_buff[teamchoose[i]][player_buff_num[teamchoose[i]]++]=buffs[sarr[skillchoosenum].status];//给队友上buff
						SlowDisplay(teamer->name,0);
						SlowDisplay("  被附加了  ",0);
						COLOR(buffs[sarr[skillchoosenum].status].n);
						printf("%s",buffs[sarr[skillchoosenum].status].name);
						COLOR(7);
						SlowDisplay(" 状态!\n",0);//减少队友的hp
						if(teamer->hp<0) {
							printf("%s",teamer->name);
							SlowDisplay("倒下了！\n",0);
							player_buff_num[teamchoose[i]]=0;
						}
					}
				}
				/*----------end---------------------*/
			}
			goto StartAction;//攻击-跳出玩家回合
		}

		case 2: {
			int i=0,gji,fyi,hfi;

			for(int gji=0; gji<50; gji++) {//这三个for会先把道具列表创建出来
				if(gj[gji].havenum) {
					parr[i]=&gj[gji];
					i++;
				}
			}
			gji=i;

			for(int fyi=0; fyi<50; fyi++) {
				if(fy[fyi].havenum) {
					parr[i]=&fy[fyi];
					i++;
				}
			}
			fyi=i;

			for(int hfi=0; hfi<50; hfi++) {
				if(hf[hfi].havenum) {
					parr[i]=&hf[hfi];
					i++;
				}
			}
			hfi=i;

			choosenum=0;

			while(1) {
				system("cls");


				SlowDisplay("要使用什么？",1);
				puts("\n=============================================================================");

				SlowDisplay("\n攻击道具：\n",1);
				for(int g=0,t=1; g<gji; g++) {
					if(g==choosenum)COLOR(240);
					printf("%d . %s:%d个\t",g,parr[g]->name,parr[g]->havenum);
					COLOR(7);
					t++;
					if(t==lineL)printf("\n\n"),t=1;
				}
				SlowDisplay("\n防御道具：\n",1);
				for(int g=gji,t=1; g<fyi; g++) {
					if(g==choosenum)COLOR(240);
					printf("%d . %s:%d个\t",g,parr[g]->name,parr[g]->havenum);
					COLOR(7);
					t++;
					if(t==lineL)printf("\n\n"),t=1;
				}



				SlowDisplay("\n回复道具：\n",1);
				for(int g=fyi,t=1; g<hfi; g++) {
					if(g==choosenum)COLOR(240);
					printf("%d . %s:%d个\t",g,parr[g]->name,parr[g]->havenum);
					COLOR(7);
					t++;
					if(t==lineL)printf("\n\n"),t=1;
				}





				puts("\n=============================================================================");
				int kg=takechoose(i);
				fflush(stdin);

				if(kg==0) { //按Q
					system("cls");
					goto PlayerRound;//使用物品-回到玩家回合

				}
				if(kg==3) { //按回车
					puts(" ");
					break;
				}
			}

			switch(parr[choosenum]->type) {
				case 2: { //攻击型道具
					int propchoosenum=choosenum;//保存道具选择
					choosenum=0;
					int monsterchoose[parr[propchoosenum]->a_num];//用于保存敌人选择
					int monsterchoosenum=0;//当前选择了几个怪物
					while(1) {
						system("cls");
						SlowDisplay(player->name,1);
						SlowDisplay(" 使用了 ",1);
						SlowDisplay(parr[propchoosenum]->name,1);
						puts("!!\n");
						puts("要攻击哪个敌人？？用WASD或方向键选择,Q退出,回车确定：\n");
						puts("=============================================================================");
						for(int i=0; i<egroup.num; i++) {
							if(i%(lineL-1)==0)printf("\n");
							if(i==choosenum)COLOR(240);
							printf("%d.  %s",i,monsterarr[i].name);
							if(monsterarr[i].hp<0)printf("(已战败)");
							printf("\t\t\t");
							COLOR(7);
						}
						COLOR(8);
						printf("\n已选择%d/%d",monsterchoosenum,parr[propchoosenum]->a_num);
						COLOR(7);
						int kg=takechoose(egroup.num);
						fflush(stdin);

						if(kg==0) { //按Q
							system("cls");
							goto PlayerRound;//攻击-回到玩家回合

						}
						if(kg==3) { //按回车
							if(monsterarr[choosenum].hp>0)
								monsterchoose[monsterchoosenum++]=choosenum;
							else {
								puts("\n该敌人已战败，请重新选择。");
								wait();
							}
							if(monsterchoosenum==parr[propchoosenum]->a_num)
								break;
						}
					}
					for(int i=0; i<monsterchoosenum; i++) {
						harm=parr[propchoosenum]->num;//读取攻击力
						monster=&monsterarr[monsterchoose[i]];
						if(monster->hp<=0) {
							continue;
						}
						harm-=monster->defence;//计算最终伤害

						SlowDisplay(monster->name,0);
						SlowDisplay("  受到了  ",0);
						COLOR(parr[propchoosenum]->ele);
						printf("%ld",harm);
						COLOR(7);
						SlowDisplay("  点伤害!\n",0);
						monster->hp-=harm;//减少怪物的hp
						if(monster->hp<=0) {
							printf("%s",monster->name);
							SlowDisplay("倒下了！\n",0);
						}
					}
					parr[propchoosenum]->havenum-=1;
					break;//判断物品类型-跳到使用物品
				}
				case 3: { //回复型道具
					int propchoosenum=choosenum;//保存道具选择
					choosenum=0;
					int teamchoose[parr[propchoosenum]->a_num];//用于保存队友选择
					int teamchoosenum=0;//当前选择了几个队友
					while(1) {
						system("cls");
						puts("要z作用于哪个队友？？用WASD或方向键选择,Q退出,回车确定：\n");
						puts("=============================================================================");
						for(int i=0; i<TeamNum; i++) {
							if(i%(lineL-1)==0)printf("\n");
							if(i==choosenum)COLOR(240);
							printf("%d.  %s",i,players[i].name);
							if(players[i].hp<0)printf("(已战败)");
							printf("\t\t\t");
							COLOR(7);
						}
						COLOR(8);
						printf("\n已选择%d/%d",teamchoosenum,parr[propchoosenum]->a_num);
						COLOR(7);
						int kg=takechoose(TeamNum);
						fflush(stdin);

						if(kg==0) { //按Q
							system("cls");
							goto PlayerRound;//攻击-回到玩家回合

						}
						if(kg==3) { //按回车
							if(players[choosenum].hp>0)
								teamchoose[teamchoosenum++]=choosenum;
							else {
								puts("该队友已濒死，请重新选择。");
								wait();
							}
							if(teamchoosenum==parr[propchoosenum]->a_num)
								break;
						}
					}

					system("cls");
					SlowDisplay(player->name,0);
					SlowDisplay("使出了",0);
					SlowDisplay(parr[propchoosenum]->name,0);
					printf("！");
					for(int i=0; i<teamchoosenum; i++) {
						struct Player *teamer=&players[teamchoose[i]];
						hpplus=parr[propchoosenum]->num;
						hpplus*=teamer->cure;//计算最终治疗
						teamer->hp+=hpplus;
						SlowDisplay(teamer->name,0);
						SlowDisplay(" 回复了 ",0);
						printf("%ld",hpplus);
						SlowDisplay("点血！\n",0);
						if(teamer->hp>teamer->f_hp) {
							teamer->hp=teamer->f_hp;
							SlowDisplay("回满了！\n",0);
						} else {
							SlowDisplay("现在有 ",0);
							printf("%ld",teamer->hp);
							SlowDisplay(" 点血量！\n",0);
						}

					}


					parr[propchoosenum]->havenum-=1;
					break;//判断物品类型-跳到使用物品
				}

			}
			wait();
			system("cls");//清除控制台
			break;//使用物品-跳出玩家回合
		}



		case 3: {
			for(int i=0; i<TeamNum; i++) {
				SlowDisplay(players[i].name,0);
				SlowDisplay(":\nhp:",0);
				printf("%ld",players[i].hp);
				printf("/");
				printf("%ld",players[i].f_hp);
				SlowDisplay("\nhd:",0);
				printf("%ld",players[i].hd);
				printf("/");
				printf("%ld",players[i].f_hd);
				SlowDisplay("\n攻击力:",0);
				printf("%ld",players[i].attack);
				SlowDisplay("\n防御力:",0);
				printf("%ld",players[i].defence);
				SlowDisplay("\n:当前武器:",0);
				printf("%s",players[i].attp->name);
				SlowDisplay("\n:当前防具:",0);
				printf("%s",players[i].defp->name);
				SlowDisplay("\n:当前等级:",0);
				printf("%ld",players[i].lever);
				SlowDisplay("\n暴击率：",0);
				printf("%f%%",players[i].crit_rate*100);
				SlowDisplay("\n暴击伤害：",0);
				printf("%f%%\n\n",players[i].crit_damage*100);
			}


			for(int pp=0; pp<egroup.num; pp++) {
				printf("\n");
				SlowDisplay(monsterarr[pp].name,0);
				SlowDisplay(":\nhp:",0);
				printf("%ld",monsterarr[pp].hp);
				SlowDisplay("\n攻击力:",0);
				printf("%ld",monsterarr[pp].attack);
				SlowDisplay("\n防御力:",0);
				printf("%ld\n",monsterarr[pp].defence);
			}

			wait();
			system("cls");//清除控制台
			goto PlayerRound;//查看-重新开始玩家回合

		}

		case 4: {
			if(e) {
				if(random()<50) {
					SlowDisplay("逃跑成功！耶！",0);
					iniarr(monster->exp);
					monster->exp[0]=1;//如果逃跑则没有经验

					goto ENDATTACK;
				} else {
					SlowDisplay("逃跑失败！这下该怎么办啊啊啊啊啊！！\n",0);

				}
				break;//逃跑-跳出玩家回合
			}
		}

		default:
			printf("………………\n");
			SlowDisplay(waring_content,0);
			goto PlayerRound;
	}

	goto StartAction;

MonsterRound:
//下面是怪物的回合
	printf("%s\n",monster->name);
	wait();
	goto StartAction;




ENDATTACK:
	for(int i=0; i<TeamNum; i++) { //清除战斗中的buff
		players[i].attack=players[i].n_attack;
		players[i].defence=players[i].n_defence;
		players[i].miss=players[i].n_miss;
		players[i].speed=players[i].n_speed;
	}
	SlowDisplay("战斗结束！\n\n",0);
	SlowDisplay("\n\n\\W",0);
	getchar();
	system("cls");//清除控制台
	return 1;
}

void hotel(void) { //回满状态的函数，在旅馆等地点调用
	for(int i=0; i<TeamNum; i++) {
		players[i].hd=players[i].f_hd;
		players[i].hp=players[i].f_hp;
		players[i].attack=players[i].n_attack;
		players[i].defence=players[i].n_defence;
	}

}

void bag(void) { //察看背包及其它操作函数
	long hpplus=0;//技能或道具回复的血量
	int k=1;

	while(k) {
		system("cls");//清除控制台
		SlowDisplay("\n背包内容：\n",0);
		puts("\n=============================================================================");
		SlowDisplay("1.防具\t\t2.武器\t\t3.道具\t\t4.纪念品\t\t114514.点错了。。。。。\n\n",0);
		fflush(stdin);
		choosenum=-1;
		scanf("%d",&choosenum);
		fflush(stdin);
		int i=0;
		switch(choosenum) {
			case -1:
				system("cls");//清除控制台
				printf("………………\n");
				SlowDisplay(waring_content,0);
				break;
			case 1: {
				system("cls");//清除控制台
				SlowDisplay("要更换谁的防具？\n\n",0);
				for(int i=0; i<TeamNum; i++) {
					printf("%d:%s\t",i,players[i].name);
				}
				puts("\n=============================================================================");
				choosenum=-1;
				fflush(stdin);
				scanf("%d",&choosenum);
				fflush(stdin);
				struct Player *player=&players[choosenum];


				for(int fji=0; fji<50; fji++) {
					if(fj[fji].havenum) {
						parr[i]=&fj[fji];
						i++;
					}
				}
				int fjnum=i;
				printf("\n");
				fflush(stdin);
				choosenum=0;
				while(1) {
					system("cls");
					puts("要用什么防具？用WASD或方向键选择物品,Q退出，回车确定：\n");
					SlowDisplay("\n防具：\n",1);
					for(int i=0,t=1; i<fjnum; i++) {
						if(i==choosenum)COLOR(240);
						printf("%d . %s:%d个\t",i,parr[i]->name,parr[i]->havenum);
						COLOR(7);
						t++;
						if(t==lineL)printf("\n\n"),t=1;
					}
					puts("\n=============================================================================");
					printf("防御力：%ld",parr[choosenum]->num);
					int kg=takechoose(fjnum);
					fflush(stdin);

					if(kg==0) { //按Q
						system("cls");
						choosenum=-1;
						break;
					}
					if(kg==3) { //按回车
						puts(" ");
						break;
					}
				}

				if(choosenum==-1) {
					break;
				}

				if(choosenum>fjnum) {//虽然不可能出现这种情况，但还是以防万一吧。。。
					printf("………………\n");
					SlowDisplay(waring_content,0);
					break;
				}
				SlowDisplay(player->name,0);
				SlowDisplay("将",0);
				SlowDisplay(player->defp->name,0);
				SlowDisplay("换成了",0);
				SlowDisplay(parr[choosenum]->name,0);
				printf("!\n");
				player->defp->havenum+=1;
				player->defp=parr[choosenum];
				parr[choosenum]->havenum-=1;
				wait();
				break;
			}

			case 2: {
				system("cls");//清除控制台
				SlowDisplay("要更换谁的武器？\n",0);
				for(int i=0; i<TeamNum; i++) {
					printf("%d:%s\t",i,players[i].name);
				}
				puts("\n=============================================================================");
				choosenum=-1;
				fflush(stdin);
				scanf("%d",&choosenum);
				fflush(stdin);
				struct Player *player=&players[choosenum];


				for(int wqi=0; wqi<50; wqi++) {
					if(wq[wqi].havenum) {
						parr[i]=&wq[wqi];
						i++;
					}
				}
				int wqnum=i;
				printf("\n");
				fflush(stdin);
				choosenum=0;
				while(1) {
					system("cls");
					puts("要用什么武器？用WASD或方向键选择物品,Q退出，回车确定：\n");
					SlowDisplay("\n武器：\n",1);
					for(int i=0,t=1; i<wqnum; i++) {
						if(i==choosenum)COLOR(240);
						printf("%d . %s:%d个\t",i,parr[i]->name,parr[i]->havenum);
						COLOR(7);
						t++;
						if(t==lineL)printf("\n\n"),t=1;
					}
					puts("\n=============================================================================");
					printf("攻击力：%ld",parr[choosenum]->num);
					int kg=takechoose(wqnum);
					fflush(stdin);

					if(kg==0) { //按Q
						system("cls");
						choosenum=-1;
						break;
					}
					if(kg==3) { //按回车
						puts(" ");
						break;
					}
				}

				if(choosenum==-1) {
					break;
				}

				if(choosenum>wqnum) {//虽然不可能出现这种情况，但还是以防万一吧。。。
					printf("………………\n");
					SlowDisplay(waring_content,0);
					break;
				}
				SlowDisplay(player->name,0);
				SlowDisplay("将",0);
				SlowDisplay(player->attp->name,0);
				SlowDisplay("换成了",0);
				SlowDisplay(parr[choosenum]->name,0);
				printf("!\n");
				player->attp->havenum+=1;
				player->attp=parr[choosenum];
				parr[choosenum]->havenum-=1;
				wait();
				break;
			}
			case 3: {
				int i=0,gji,fyi,hfi;

				for(int gji=0; gji<50; gji++) {//这三个for会先把道具列表创建出来
					if(gj[gji].havenum) {
						parr[i]=&gj[gji];
						i++;
					}
				}
				gji=i;

				for(int fyi=0; fyi<50; fyi++) {
					if(fy[fyi].havenum) {
						parr[i]=&fy[fyi];
						i++;
					}
				}
				fyi=i;

				for(int hfi=0; hfi<50; hfi++) {
					if(hf[hfi].havenum) {
						parr[i]=&hf[hfi];
						i++;
					}
				}
				hfi=i;

				choosenum=0;

				while(1) {
					system("cls");
					SlowDisplay("\n您想使用啥？先说好非战斗状态不能使用攻击和防御道具哦！\n",1);
					printf("\n");

					SlowDisplay("要使用什么？",1);
					puts("\n=============================================================================");

					SlowDisplay("\n攻击道具：\n",1);
					for(int g=0,t=1; g<gji; g++) {
						if(g==choosenum)COLOR(240);
						printf("%d . %s:%d个\t",g,parr[g]->name,parr[g]->havenum);
						COLOR(7);
						t++;
						if(t==lineL)printf("\n\n"),t=1;
					}
					SlowDisplay("\n防御道具：\n",1);
					for(int g=gji,t=1; g<fyi; g++) {
						if(g==choosenum)COLOR(240);
						printf("%d . %s:%d个\t",g,parr[g]->name,parr[g]->havenum);
						COLOR(7);
						t++;
						if(t==lineL)printf("\n\n"),t=1;
					}



					SlowDisplay("\n回复道具：\n",1);
					for(int g=fyi,t=1; g<hfi; g++) {
						if(g==choosenum)COLOR(240);
						printf("%d . %s:%d个\t",g,parr[g]->name,parr[g]->havenum);
						COLOR(7);
						t++;
						if(t==lineL)printf("\n\n"),t=1;
					}





					puts("\n=============================================================================");
					printf("数值：%ld",parr[choosenum]->num);
					int kg=takechoose(i);
					fflush(stdin);

					if(kg==0) { //按Q
						system("cls");
						choosenum=-1;
						break;

					}
					if(kg==3) { //按回车
						puts(" ");
						break;
					}
				}


				if(choosenum==-1) {
					break;
				}

				if(choosenum>hfi) {
					printf("………………\n");
					SlowDisplay(waring_content,0);
					usleep(1500*1000);
					break;
				}
				if(choosenum>=0&&choosenum<=gji) {
					SlowDisplay("哈？你是不是没看见我说的啥？指望攻击道具虚空索敌？\n",0);
					usleep(1500*1000);
				} else if(choosenum<fyi) {
					SlowDisplay("…………芳斯塔夫说过：叠甲死路一条。\n",0);
					usleep(1500*1000);

				} else if(choosenum<hfi) {
					int choose=-1;
					SlowDisplay("给谁用？\n",0);
					for(int i=0; i<TeamNum; i++) {
						printf("%d:%s\t",i,players[i].name);
					}
					puts("\n=============================================================================");
					fflush(stdin);
					scanf("%d",&choose);
					fflush(stdin);
					struct Player *player=&players[choose];

					hpplus+=parr[choosenum]->num;
					player->hp+=hpplus;
					parr[choosenum]->havenum-=1;
					SlowDisplay(player->name,0);
					SlowDisplay("使用了",0);
					SlowDisplay(parr[choosenum]->name,0);
					SlowDisplay(" ,回复了 ",0);
					printf("%ld",hpplus);
					hpplus=0;
					SlowDisplay("点血！\n",0);
					if(player->hp>=player->f_hp) {
						player->hp=player->f_hp;
						SlowDisplay("回满了！\n",0);
					}
					SlowDisplay("现在有 ",0);
					printf("%ld",player[0].hp);
					SlowDisplay(" 点血量！\n",0);
					wait();
				}
				break;
			}
			case 4: {
				system("cls");//清除控制台
				SlowDisplay("用WASD或方向键选择物品,Q退出：\n",0);
				int i=0,t=1;
				for(int si=0; si<50; si++) {
					if(souvenir[si].have) {
						soarr[i]=&souvenir[si];
						printf("%d.  %s\t\t",i,soarr[i]->name);
						i++,t++;
						if(t==lineL)printf("\n\n"),t=1;
					}
				}
				puts("\n=============================================================================");
				printf("\n");
				choosenum=0;
				t=1;
				int kg=1;//判断是否退出纪念品查看
				//int printf_kg=1;//是否输出纪念品列表
				while(1) {
					usleep(1);
					kg=takechoose(i);
					if(kg==0)break;
					system("cls");//清除控制台
					printf("用WASD或方向键选择物品：\n");
					for(int si=0; si<i; si++) {
						if(si==choosenum) {
							COLOR(240);
							printf("%d.  %s",si,soarr[si]->name);
							COLOR(7);
							printf("\t\t");
							t++;
						} else {
							printf("%d.  %s\t\t",si,soarr[si]->name);
							t++;
						}
						if(t==lineL)printf("\n\n"),t=1;
					}
					t=1;
					puts("\n=============================================================================");
					if(soarr[choosenum]->read)printf("可阅读(R)。\n");
					if(soarr[choosenum]->f)printf("可运行(P)。\n");
					printf("%s\n",soarr[choosenum]->intro);
					printf("\n");
					if(kg==2&&soarr[choosenum]->read) {
						if(soarr[choosenum]->ms)MS=soarr[choosenum]->ms;
						SlowDisplay(soarr[choosenum]->content,0);
						MS=DEFAULT_MS;
					}
					if(kg==4&&soarr[choosenum]->f) {
						soarr[choosenum]->function();
					}
				}



				break;
			}

			case 114514: {
				system("cls");//清除控制台
				k=0;
				break;
			}


		}


	}
}


void conversation(struct Player *player,struct NPC *npc) {
	check_npc_task(npc);
	struct ConNode *con=npc->con;
LOOP:
	//if(con->speaker)printf("%s:",npcs[con->speaker].name);
	SlowDisplay(con->content,0);
	if(con->ifchoose) {
		int kg=1;
		choosenum=0;
		while(kg) {
			system("cls");
			//SlowDisplay(npc->name,1);
			SlowDisplay(con->content,1);
			int i=0;
			printf("\n按回车确定选择：\n");
			puts("\n=============================================================================");

			for(i=0; i<con->ifchoose; i++) {
				if(i==choosenum)COLOR(240);
				printf("%d:%s\t",i,con->chooselist[i]);
				COLOR(7);
			}
			fflush(stdin);
			kg=takechoose(i);
			if(kg==3) {
				system("cls");
				con=&conversations[con->next[choosenum]];
				goto LOOP;
			}
		}

	} else {
		if(con->fight) {
			int kg=attackact(egroups[con->fight],0);
			if(con->fight_continue||kg==1) {
				if(con->next[0]) {
					con=&conversations[con->next[0]];
					goto LOOP;
				}

			}
		}
		if(con->next[0]) {
			con=&conversations[con->next[0]];
			goto LOOP;

		}

	}
	if(con->start_task) {
		if(checktask(&tasks[con->start_task])==1) {
			SlowDisplay("\n已接取任务：",0);
			SlowDisplay(tasks[con->start_task].name,0);
			printf(" !");
			tasks[con->start_task].start=1;
			if(tasks[con->start_task].sonnum) { //如果这个任务有子任务
				for(int son=0; son<tasks[con->start_task].sonnum; son++) {
					tasks[tasks[con->start_task].son[son]].start=1;//接取子任务
				}
			}
			check_npc_task(npc);//修改提交npc的起始对话

		} else if(checktask(&tasks[con->start_task])==2) {
			SlowDisplay("该任务以接取。当你看到这句话时，说明我搞砸了什么。看到此条消息请联系作者，并记下下列信息：\n",1);
			printf("ConNode content:%s\ntask number:%d\nchecktask result:2",con->content,con->start_task);
		} else if(checktask(&tasks[con->start_task])==3) {
			SlowDisplay("该任务以完成。当你看到这句话时，说明我搞砸了什么。看到此条消息请联系作者，并记下下列信息：\n",1);
			printf("ConNode content:%s\ntask number:%d\nchecktask result:3",con->content,con->start_task);
		} else {
			SlowDisplay("未满足接取条件：\n",0);
			SlowDisplay("需要：\n",0);
			struct Prop *task_prop;

			task_prop=index2prop(tasks[con->start_task].condition[0]);

			if(task_prop->havenum<tasks[con->start_task].condition[3]) {

				SlowDisplay(task_prop->name,0);
				printf(": %d/%d 个\n",task_prop->havenum,tasks[con->start_task].condition[3]);
			}
			if(!souvenir[tasks[con->start_task].condition[1]].have)
				SlowDisplay(souvenir[tasks[con->start_task].condition[1]].name,0);
			printf("(未获得)\n");
		}
		wait();
	}
	if(con->check_task) {
		int kg=checktask(&tasks[con->check_task]);
		if(kg==3) {
			struct Task *change_task=&tasks[con->check_task];//这一步实际上没必要，但是为了方便阅读我就写了
			//下面发放奖励
			index2prop(change_task->give_prop)->havenum+=change_task->give_prop_num;
			souvenir[change_task->give_souvenir].have=1;
			plusarr(change_task->give_exp,player[0].exp);
			plusarr(change_task->give_gold,gold);
			change_task->finish=1;//结束任务

			tasks[tasks[con->check_task].next].start=1;//自动接取后续任务
			con=&conversations[con->finish_next_con[1]];
			leverUP(0);
			npc->con=&conversations[npc->nomal_con];//这个任务结束了，将NPC的对话起点改回去

			check_npc_task(npc);

			goto LOOP;
		} else if(kg==2) { //任务未完成
			con=&conversations[con->finish_next_con[0]];
			goto LOOP;
		}

	}



}

int takechoose(int i) {
	//选择函数，包括数字输入、WASD选择和方向键选择.i表示choosenum能达到的最大值+1
//按Q返回0，表示退出选择;按R返回2，表示阅读(纪念品);回车返回3，表示确定;按P返回4，表示运行;平时返回1。
//数字输入时，按删除键执行choosenum/10,即回退一位数字
	fflush(stdin);

	int kg=1;//是否跳出循环
	while(1) {
		if(_kbhit()) {

			int choosechar=getch();
			switch(choosechar) {
				case 'w': {
					kg=0;
					//if(choosenum==0)return 0;
					choosenum-=lineL-1;
					if(choosenum<0)choosenum=0;
					break;
				}
				case 'W': {
					kg=0;
					//if(choosenum==0)return 0;
					choosenum-=lineL-1;
					if(choosenum<0)choosenum=0;
					break;
				}
				case 'a': {
					kg=0;
					//if(choosenum==0)return 0;
					choosenum-=1;
					if(choosenum<0)choosenum=0;
					break;
				}
				case 'A': {
					kg=0;
					//if(choosenum==0)return 0;
					choosenum-=1;
					if(choosenum<0)choosenum=0;
					break;
				}
				case 's': {
					kg=0;
					//if(choosenum==i-1)return 0;
					choosenum+=lineL-1;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case 'S': {
					kg=0;
					//if(choosenum==i-1)return 0;
					choosenum+=lineL-1;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case 'd': {
					kg=0;
					//if(choosenum==i-1)return 0;
					choosenum+=1;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case 'D': {
					kg=0;
					//if(choosenum==i-1)return 0;
					choosenum+=1;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case 'Q': {//退出
					return 0;
					break;
				}
				case 'q': {
					return 0;
					break;
				}
				case 'R': {//阅读
					return 2;
					break;
				}
				case 'r': {
					return 2;
					break;
				}
				case 13: { //回车
					return 3;
					break;
				}
				case 'P': { //运行
					return 4;
					break;
				}
				case 'p': {
					return 4;
					break;
				}

				/*在Windows系统中，当使用 _getch() 函数（来自 conio.h）来读取按键时，
				一些特殊按键（如方向键和功能键）会生成两个连续的字符码。
				第一个字符码通常是 0 或 0xE0（224），用来表示接下来的字符是一个特殊按键的一部分。
				第二个字符码才是特定按键的唯一标识。

				这种方式的原因是，普通的ASCII字符不能完全表示键盘上的所有按键
				（特别是那些非打印字符键，如方向键、功能键等）。
				因此，_getch() 使用两个字符的序列来唯一标识这些特殊按键。

				当 _getch() 读取到 0 或 0xE0 时，这表示接下来的 _getch() 调用将返回一个特殊按键的代码。
				这就是为什么在判断是否按下特殊按键时，会检查 ch == 0 或 ch == 0xE0，
				然后再调用 _getch() 一次来获取实际的按键代码。

				例如，对于方向键，第一个 _getch() 调用可能返回 0，紧接着第二个 _getch() 调用则返回一个特定的值
				（比如 72 表示上方向键、80 表示下方向键等），用以区分不同的方向键。
				这样的处理方法允许程序正确识别和区分普通字符和特殊按键。*/



				case 0xE0: { //判断是不是方向键。当按下一个方向键时，实际上会生成一系列字符。通常情况下，方向键会发送一个转义序列，以ASCII码的形式表示。
					int ch = _getch();
					switch (ch) {
						case 72: { //上
							kg=0;
							//if(choosenum==0)return 0;
							choosenum-=lineL-1;
							if(choosenum<0)choosenum=0;
							break;
						}
						case 80: { //下
							kg=0;
							//if(choosenum==i-1)return 0;
							choosenum+=lineL-1;
							if(choosenum>i-1)choosenum=i-1;
							break;
						}
						case 77: { //右
							kg=0;
							//if(choosenum==i-1)return 0;
							choosenum+=1;
							if(choosenum>i-1)choosenum=i-1;
							break;
						}
						case 75: { //左
							kg=0;
							//if(choosenum==0)return 0;
							choosenum-=1;
							if(choosenum<0)choosenum=0;
							break;
						}
					}
					break;
				}
				case 0: { //有些键盘方向键第一个字符可能是0
					int ch = _getch();
					switch (ch) {
						case 72: { //上
							kg=0;
							//if(choosenum==0)return 0;
							choosenum-=lineL-1;
							if(choosenum<0)choosenum=0;
							break;
						}
						case 80: { //下
							kg=0;
							//if(choosenum==i-1)return 0;
							choosenum+=lineL-1;
							if(choosenum>i-1)choosenum=i-1;
							break;
						}
						case 77: { //右
							kg=0;
							//if(choosenum==i-1)return 0;
							choosenum+=1;
							if(choosenum>i-1)choosenum=i-1;
							break;
						}
						case 75: { //左
							kg=0;
							//if(choosenum==0)return 0;
							choosenum-=1;
							if(choosenum<0)choosenum=0;
							break;
						}
					}
					break;
				}
				case '0': {
					kg=0;
					choosenum=choosenum*10;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case '1': {
					kg=0;
					choosenum=choosenum*10+1;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case '2': {
					kg=0;
					choosenum=choosenum*10+2;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case '3': {
					kg=0;
					choosenum=choosenum*10+3;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case '4': {
					kg=0;
					choosenum=choosenum*10+4;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case '5': {
					kg=0;
					choosenum=choosenum*10+5;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case '6': {
					kg=0;
					choosenum=choosenum*10+6;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case '7': {
					kg=0;
					choosenum=choosenum*10+7;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case '8': {
					kg=0;
					choosenum=choosenum*10+8;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case '9': {
					kg=0;
					choosenum=choosenum*10+9;
					if(choosenum>i-1)choosenum=i-1;
					break;
				}
				case 8: { //删除键
					kg=0;
					choosenum/=10;
					break;
				}


				default:
					break;
			}
		}
		if(kg==0)break;
	}
	return 1;
}



int checktask(struct Task *task) {
//检查任务函数，检查任务是否满足开始条件(返回1/0)、
//					任务是否开启(返回2)、
//					任务是否完成(返回3)
//					任务是否结束(返回4)
//优先级从前往后升高

	int result=1;
	if(task->finish) {
		result=4;
		goto END;
	}
	if(task->start) {
		result=2;
		goto FINISH;
	}


	//检查开启条件
	//检查道具
	struct Prop *task_prop;

	task_prop=index2prop(task->condition[0]);

	if(task_prop->havenum<task->condition[3])result=0;

	//检查纪念品
	if(!souvenir[task->condition[1]].have)result=0;
	//检查任务
	if(!tasks[task->condition[2]].finish)result=0;
FINISH://检查是否完成
	if(result==2) {
		result=3;
		//子任务是否全部完成
		if(task->sonnum) {
			int f_sonnum=0;//记录完成了几个子任务
			for(int i=0; i<task->sonnum; i++) {
				if(checktask(&tasks[task->son[i]])==3)f_sonnum+=1;
			}
			if(f_sonnum<task->f_son)result=2;
		}
		//道具

		task_prop=index2prop(task->f_condition[0]);

		if(task_prop->havenum<task->f_condition[3])result=2;
		//纪念品
		if(!souvenir[task->condition[1]].have)result=2;

	}


END:
	return result;
}

void tasktree(void) {


	for(int i=1; i<500; i++) {
		struct Task task=tasks[i];
		if(task.father)continue;//如果这个任务是一个子任务则跳过
		if(task.start&&!task.finish) { //检查任务是否开始以及是否完成

			COLOR(15);
			SlowDisplay("||--",1);
			SlowDisplay(task.name,1);
			SlowDisplay("\n|     |--",1);
			COLOR(7);
			SlowDisplay(task.intro,1);
			printf("\n");
			COLOR(15);
			if(task.sonnum) {
				for(int i=0; i<task.sonnum; i++) {
					struct Task sontask=tasks[task.son[i]];
					if(sontask.start&&sontask.see) { //检查任务是否开始以及是否可见

						COLOR(15);
						if(sontask.finish)COLOR(8);
						SlowDisplay("|  ||--",1);
						SlowDisplay(sontask.name,1);
						SlowDisplay("\n|  |     |--",1);
						COLOR(7);
						SlowDisplay(sontask.intro,1);
						printf("\n");
						COLOR(15);
					}
				}
			}

		}
	}
	COLOR(7);
	wait();
	system("cls");
}

void wait(void) {
	fflush(stdin);
	COLOR(8);
	printf("\n\n\n(按任意键继续……)");
	while(1) {
		if(_kbhit()) {
			_getch();
			fflush(stdin);
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b                       \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
			break;
		}
	}
	COLOR(7);
}

struct Prop *index2prop(int n) {
//通过物品编号返回结构体
	struct Prop *reprop;

	if(n<50)reprop=&fj[n];
	else if(n<100)reprop=&wq[n-50];
	else if(n<150)reprop=&gj[n-100];
	else if(n<200)reprop=&hf[n-150];
	else if(n<250)reprop=&fy[n-200];

	return reprop;
}

void check_npc_task(struct NPC *npc) {
	//检查该npc所有相关任务是否完成，并对该npc的对话起点做出相应修改。
	int tnum=npc->task[0];//读取这个NPC有几个任务
	for(int i=1; i<=tnum; i++) {
		int num=npc->task[i];//记录每个任务的编号
		int kg=checktask(&tasks[num]);//检查这些任务完成情况

		if(kg==1) { //该任务可被开启
			npcs[tasks[num].npc_s].con=&conversations[tasks[num].con_s];//更改对话起点
		}
		if(kg==2||kg==3) {//任务已经开启或已经完成，但未提交
			npcs[tasks[num].npc_f].con=&conversations[tasks[num].con_f];//更改对话起点
		}


	}
}

void creat_relics(void) { //圣遗物制造机
	int n=random()%5;
	struct Attribute arrtibute= {
		.crit_rate=srandom(1)%29/100,
		.crit_damage=srandom(2)%57/100,
		.damage=srandom(3)%400,
		.damage_rate=srandom(4)%30/100,
		.hp=srandom(5)*10,
		.hp_rate=srandom(6)%25/100,
		.def=srandom(7)/5,
		.def_rate=srandom(8)%17/100,
		.miss=srandom(9)%20/100,
		.cure=srandom(10)%3/10,
	};
	relics[n][++relics_num[n]].n=n;
	relics[n][relics_num[n]].attribute=arrtibute;
}


int srandom(int n) { //以当前时间（秒为单位）和指定数值为种子生成一个0～99的随机数
	srand((unsigned)time(NULL)+n);
	return rand() % 100;
}

void count(void) { //计算攻击力等等数值，用于更换圣遗物、升级后
	for(int i=0; i<4; i++) {
		int hp=players[i].lever*players[i].hparr[1]+players[i].hparr[2];
		int hd=players[i].lever*players[i].hdarr[1]+players[i].hdarr[2];
		int def=players[i].lever*players[i].defarr[1]+players[i].defarr[2];
		int att=players[i].lever*players[i].attarr[1]+players[i].attarr[2];
		//计算基础值
		float crit_rate;//暴击率
		float crit_damage;//暴击伤害倍率
		float damage_rate;//攻击力提高率
		float hp_rate;
		float def_rate;
		float miss;//闪避
		float cure;//治疗加成
		for(int j=0; j<5; j++) { //遍历圣遗物并统计加成
			struct Attribute arrtibute;
			arrtibute = relics[j][players[i].relics[j]].attribute;
			crit_rate+=arrtibute.crit_rate;
			crit_damage+=arrtibute.crit_damage;
			damage_rate+=arrtibute.damage_rate;
			hp_rate+=arrtibute.hp_rate;
			miss+=arrtibute.miss;
			cure+=arrtibute.cure;
			hp+=arrtibute.hp;
			def+=arrtibute.def;
			att+=arrtibute.damage;
		}
		hp*=hp_rate;
		def*=def_rate;
		att*=damage_rate;
		crit_rate+=0.05;//每个角色初始%5暴击率
		if(miss>0.2)miss=0.2;
		//修改玩家数据
		players[i].f_hp=hp;
		players[i].f_hd=hd;
		players[i].n_attack=att;
		players[i].n_defence=def;
		players[i].cure=cure;
		players[i].n_miss=miss;
		players[i].crit_rate=crit_rate;
		players[i].crit_damage=crit_damage;
	}

}













