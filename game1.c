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
#define escpro 5//怪物逃跑的概率
#define addpro 100//掉落概率
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




/*-----------下面定义结构体---------*/


struct Prop{//道具结构体
	int n;//道具编号0-50为防具，51-100为武器，101-150为攻击道具，151-200为回复道具，201-250为防御道具
	char *name;
	int num[LEN];//道具的数值，如增加的防御力、攻击力，或造成的伤害、回复的血量等等
	int type;//道具类型，0为武器，1为防具，2为攻击道具，3为回复道具，4为物品材料
	int ele;//如果是伤害，伤害的元素类型
	int havenum;//拥有的数量
};

struct Souvenir{//纪念品结构体
	char *name;
	int have;//是否解锁
	int read;//是否为可以读取内容的类型
	char *intro;//物品简介
	char *content;//如果是可以读取的类型，它的内容
};

struct Player{//玩家结构体
	char name[200];//结构体定义的时候不能被初始化
	int hp[LEN];//血
	int hd[LEN];//蓝
	int attack[LEN];//攻击力
	int defence[LEN];//防御力
	int miss;//敏捷，即闪避概率
	int sp[LEN];//技能列表
	int lever[LEN];//等级
	int exp[LEN];//经验
	int lexp[LEN];//升级所需的经验
	struct Prop *defp;//防御道具结构体
	struct Prop *attp;//武器
};


struct NPC{
	char *name;
	int var[50];//独立变量
	/*
	var[0]代表这个npc是否能触发任务，如果能则为对应任务的编号；
	1代表这个npc有可能处于几个任务内
	2~2+var[1]是这几个任务的编号
	也就是说，一个NPC最多可以处在48个任务内
	（不要觉得这很多，因为我们是将一个大型任务细分为许多个小任务完成的，因此可能一个大型任务需要找某个特定的NPC很多次） 
	*/
	struct ConNode *con;//打招呼内容,该内容是一个对话节点，可能会因为任务没完成造成打招呼用语的改变，
};

struct Monster{//怪物结构体
	char *name;
	int hp[LEN];
	int attack[LEN];
	int defence[LEN];
	int miss;
	struct Skill* snum;//技能名称
	int pro;//发动技能的概率
	int exp[LEN];//怪物能给与的经验
	char *v;//怪物出场提示
	struct Prop *prop;//掉落物品的结构体
	int g[LEN];//怪物掉落的金币
};

struct Map{//地图结构体
	char *name;
	int Map_class;//这个地图的种类，0为安全区，1为野外
	int monsternum[4];//如果是野外，则这片区域有可能碰到哪些怪物
	int npc[10];//这个地图有几个npc
	int next[8];//与这片区域相连的区域的编号
};

struct Skill{//技能结构体
	int num;//编号
	char *name;//技能名称
	char *suffix;//后缀，比如“就因为他做得到！”
	int sp[LEN];//消耗蓝量
	int damage;//攻击力倍率
	int boo;//是否对血量有影响，加血1，减血-1，没有影响0
	int hp[LEN];//一些技能会增加或消耗血量
	int ele;//元素种类，15物理，12火，10毒，9水，3冰，5雷, 4特殊
	int stu;//是否习得该技能
};

struct ConNode{//对话结构体
	int role;// 这句话是谁说的，0代表玩家，1代表NPC
	char *content;//这句话的内容
	int ifchoose;//是否需要选择，以及选择的数量
	char *chooselist[5];//每个选择的内容
	int next[5];//对应应跳转的节点
};

struct Task{
	int start;//判断这个任务有没有开始
	int finish;//判断这个任务有没有完成
	int futher;//该任务是否是某个大型任务的子任务（用于输出任务树时不会重复输出子任务以及完成子任务时检查父任务是否完成）
	int sonnum;//这个任务有几个子任务
	int son[5];//子任务的编号
	int propnum;//前置条件-是否有某种道具；如果没有此种要求，则用一种玩家肯定会有的东西代替
	int souvenir;//前置条件-是否有某种纪念品
	int tasknum;//前置条件-是否完成某个任务
	int lever;//前置条件-是否达到指定等级
	char *intro;//任务内容描述
	int con;//这个任务第一句话是什么
	int give_prop;//任务完成给予什么道具
	int give_prop_num;//数量
	int give_souvenir;
	int give_exp[LEN];//经验奖励
	int give_gold[LEN];//金币奖励
	int next;//后续任务的编号，没有则为0（大型任务在接取时自动开启子任务，这个变量只用于任务链传递）
};
/*---------初始化道具-防具---------*/

struct Prop fj[50]={
	{//初始道具，啥也没有
		.n=0,
		.name="无\0",
		.num={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		.type=1,
		.ele=0,
		.havenum=1,
	},
	
	{
	.n=1,
	.name="布衣\0",
	.num={0,5,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.type=1,
	.ele=0,
	.havenum=1,
	},
	
	{
	.n=2,
	.name="皮甲\0",
	.num={0,0,2,0,0,0,0,0,0,0,0,0,0,0,0},
	.type=1,
	.ele=0,
	.havenum=1,
	},
	
	{
	.n=3,
	.name="链甲\0",
	.num={0,0,5,0,0,0,0,0,0,0,0,0,0,0,0},
	.type=1,
	.ele=0,
	.havenum=1,
	},
	
	{
	.n=4,
	.name="板甲\0",
	.num={0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
	.type=1,
	.ele=0,
	.havenum=2,
	},
	
	{
	.n=0,
	.name="装甲\0",
	.num={0,0,0,5,0,0,0,0,0,0,0,0,0,0,0},
	.type=1,
	.ele=0,
	.havenum=2,
	},
	
	{
	.n=0,
	.name="你五年前丢失的睡衣\0",
	.num={0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
	.type=1,
	.ele=0,
	.havenum=111,
	},
	
	{
	.n=50,
	.name="ERR\\OR@\\#$\\%sd!sdFAS!@\\\\\0",
	.num={0,0,0,0,0,0,0,0,0,0,0,0,0,0,9},
	.type=1,
	.ele=0,
	.havenum=1,
	}
	
	
	
	
	
};


/*---------初始化道具-武器---------*/

struct Prop wq[50]={
	{//初始道具，啥也没有
	.n=50,
	.name="无\0",
	.num={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.type=0,
	.ele=0,
	.havenum=1,
},

	{
	.n=51,
	.name="铁剑\0",
	.num={0,0,2,0,0,0,0,0,0,0,0,0,0,0,0},
	.type=0,
	.ele=15,
	.havenum=1,
},

	{
	.n=52,
	.name="黑缨枪\0",
	.num={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.type=1,
	.ele=15,
	.havenum=1,
},
	
	{
	.n=99,
	.name="ERROR#$warRE@#er%^EVE\0",
	.num={0,0,0,0,0,0,0,0,0,0,0,0,0,0,9},
	.type=1,
	.ele=4,
	.havenum=1,
}
	
	
	
};


/*----------------------初始化道具-攻击道具-----------------------------*/

struct Prop gj[50]={
	{
	.n=101,
	.name="手雷\0",
	.num={0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
	.type=2,
	.ele=12,
	.havenum=5,
	
},
	{
	.n=101,
	.name="毒镖\0",
	.num={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.type=2,
	.ele=10,
	.havenum=3,
	
},
	{
	.n=101,
	.name="粑粑\0",
	.num={9,9,9,9,0,0,0,0,0,0,0,0,0,0,0},
	.type=2,
	.ele=4,
	.havenum=6,
}
	
	
	
	
};

/*--------------------------初始化道具-回复道具------------------------------*/

struct Prop hf[50]={
	{
	.n=150,
	.name="回血包\0",
	.num={0,0,5,0,0,0,0,0,0,0,0,0,0,0,0},
	.type=3,
	.ele=12,
	.havenum=5,
	
},
	{
	.n=151,
	.name="草药\0",
	.num={0,0,7,0,0,0,0,0,0,0,0,0,0,0,0},
	.type=3,
	.ele=12,
	.havenum=5,
	
},
	{
	.n=152,
	.name="四方和平\0",
	.num={0,0,5,1,0,0,0,0,0,0,0,0,0,0,0},
	.type=3,
	.ele=12,
	.havenum=5,
	
},
	{
	.n=153,
	.name="云南白药\0",
	.num={0,0,0,2,0,0,0,0,0,0,0,0,0,0,0},
	.type=3,
	.ele=12,
	.havenum=5,
	
},
	{
	.n=154,
	.name="红药水\0",
	.num={0,0,5,2,0,0,0,0,0,0,0,0,0,0,0},
	.type=3,
	.ele=12,
	.havenum=5,
	
},
	{
	.n=155,
	.name="元素瓶\0",
	.num={0,0,0,5,0,0,0,0,0,0,0,0,0,0,0},
	.type=3,
	.ele=12,
	.havenum=5,
	
},
	{
	.n=156,
	.name="核子可乐\0",
	.num={0,0,5,5,0,0,0,0,0,0,0,0,0,0,0},
	.type=3,
	.ele=12,
	.havenum=5,
	
},
	{
	.n=157,
	.name="洗手液\0",
	.num={0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	.type=3,
	.ele=12,
	.havenum=5,
	
},
	{
	.n=158,
	.name="凤凰尾巴\0",
	.num={0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	.type=3,
	.ele=12,
	.havenum=5,
	
},
	{
	.n=159,
	.name="绿色星星\0",
	.num={0,0,5,9,1,0,0,0,0,0,0,0,0,0,0},
	.type=3,
	.ele=12,
	.havenum=5,
	
}
	
	
	
	
};

/*--------------初始化防御道具---------------*/

struct Prop fy[50];







/*-------------------------初始化纪念品------------------------------*/
struct Souvenir souvenir[50]={
	{//0
		.name="姬子的照片",
		.have=1,
		.read=1,
		.intro="姬子温柔的注视着你，不再言语。",
		.content="姬 \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b子 \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b温 \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b柔 \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b的 \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b注 \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b视 \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b着 \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b你 \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b，不 \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b再 \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b言 \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b语。",
		
	},
	
	{//1
		.name="未命名1.c",
		.have=0,
		.read=0,
		.intro="大  脑  一  片  空  白",
	},
	{//2
		.name="剑雨",
		.have=1,
		.read=1,
		.intro="某时下的雨，凌利似剑。",
		.content="“感谢各位这么多年的精诚合作，再会。”",
	},
	{//3
		.name="易拉环",
		.have=1,
		.read=1,
		.intro="曾被某人戴在胸前的物件，隐约可见“刁乐”二字。",
		.content="“是否有法律因素要停止这场婚礼的举行？”         \n“是否有人反对这场婚礼？” ",
	},
	{//4
		.name="笑里藏刀",
		.have=1,
		.read=0,
		.intro="哈哈哈哈哈哈哈哈哈哈哈哈刀哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈",
	},
	{//5
		.name="真·刀",
		.have=1,
		.read=0,
		.intro="*Chara,这是什么？",
	},
	{//6
		.name="奶油肉桂派",
		.have=1,
		.read=0,
		.intro="*你拥抱了失落灵魂并告诉她你和她将会重逢。",
	},
	{//7
		.name="心形坠饰盒",
		.have=1,
		.read=0,
		.intro="上面写着“最好的朋友”。",
	},
	{//8
		.name="村雨",
		.have=1,
		.read=0,
		.intro="某个雨夜里，一个男孩失去了最重要的人。",
	},
	{//9
		.name="0      ",
		.have=1,
		.read=0,
		.intro="如果你正在读这句话，说明我可能搞砸了什么。",
	},
	{//10
		.name="一片花瓣",
		.have=1,
		.read=0,
		.intro="金黄色的花瓣。\n“一起来毁灭这个可悲的世界上的所有东西吧。在这些无用的记忆里存在着的所有人，所有东西...让我们把他们全部化为尘埃。”",
	},
	{//11
		.name="飞机模型",
		.have=1,
		.read=1,
		.intro="木质模型。曾被某人放飞，又被某人捡起。",
		.content="“……奥托。”  \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b    \n“奥托·阿波卡利斯。”       \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b ",
	},
};








/*------------初始化玩家------------------*/
struct Player player[2]={
	{
	.name="文止墨\0",
	.hp={0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
	.hd={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.attack={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.defence={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.miss=10,
	.sp={0,1,2,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	.lever={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.exp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.lexp={0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
	//因为在主函数外初始化防具和武器会报错，我们在main函数里面初始化
},
	{//这是记录当前等级满状态的各项数值
	.name="文止墨\0",
	.hp={0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
	.hd={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.attack={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.defence={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.miss=10,
	.sp={0,1,2,3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	.lever={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.exp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.lexp={0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
	
}
	
};

/*-----------下面初始化技能---------*/


struct Skill skill_list[20]={
	{
	.num=1,
	.name="弹跳碰撞\0",
	.suffix="\n\0", 
	.sp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.damage=1,
	.boo=0,
	.hp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.ele=15,
	.stu=1,
},
	{
	.num=2,
	.name="[Error] conflicting types for 'attackact'\0",
	.suffix="\n\0",
	.sp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.damage=2,
	.boo=0,
	.hp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.ele=4,
	.stu=1,
},
	{
	.num=3,
	.name="爆炎术\0",
	.suffix="\n\0",
	.sp={0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
	.damage=1,
	.boo=0,
	.hp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.ele=12,
	.stu=1,
},
{
	.num=4,
	.name="背摔\0",
	.suffix="就因为他做得到！\n\0",
	.sp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.damage=1,
	.boo=0,
	.hp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.ele=12,
	.stu=1,
}
	
	
	
	
};

/*-----------下面初始化各个怪物-----------*/
struct Monster slm={
	.name="史莱姆\0",
	.hp={0,0,1,1,0,0,0,0,0,0,0,0,0,0,0},//初始100血
	.attack={0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.defence={0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.snum=&skill_list[0],
	.pro=90,
	.exp={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.v="跳了过来\0",
	
};
struct Monster error={
	.name="ERROR怪\0",
	.hp={0,0,0,0,0,0,0,9,9,9,9,9,9,9,9},
	.attack={0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.defence={0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.snum=&skill_list[1],
	.pro=50,
	.exp={0,0,0,2,0,0,0,0,0,0,0,0,0,0},
	.v="出现了\0",
	
};
struct Monster monster={
	.name="\0",
	.hp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.attack={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.defence={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.pro=0,
	.exp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.v="\0",
	
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

void SlowDisplay(const char *text);
//输出字符函数

void putarr(int a[]);
//输出数据的函数

void leverUP(int i);
//判断是否升级并执行

void attackact(struct Player *player,struct Monster monster);
//攻击行为函数。运行此函数将进入战斗直到一方倒下或逃跑（未完成）

int random(void);
//以当前时间（秒为单位）为种子生成一个0～99的随机数

void hotel(void);
//回满状态的函数，在旅馆等地点调用

void bag(void);
//察看背包内容函数

//struct Skill readskill(int s);
//通过技能编号读取技能信息的函数


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

char *waring_content="滚啊！！！";
/*---------main函数-----------*/
int main()
{
	/*初始化各种数组*/
	iniarr(ablank);
	iniarr(gold);
	/*初始化玩家道具*/
	player[0].defp=&fj[0];
	player[0].attp=&wq[0];
	/*初始化怪物掉落物*/
	slm.prop=&wq[2];
	error.prop=&wq[3];
	
	/*SlowDisplay("这里\\.本来应该\\.\\.\\.写背景故事的，\n但是作者也不知道背景故事是什么。。。\n勇敢的少年啊，快快告诉我你的名字，去创造神话吧！\n(200个字符以内)：");
	char player_name[200];
	scanf("%s", player_name);
	size_t namelength = strlen(player_name);//判断玩家名字长度
	strncpy(player[0].name, player_name,namelength+3 );//将玩家输入的名字复制到玩家结构体对应的变量中*/
	player[1] = player[0];
	
	
	monster=slm;
	while(1){
		bag();
		attackact(&player[0],monster);
	}
	
	//putheart();
	getchar();
	return 0; 
} 

void iniarr(int a[]){//初始化数组
	for(int i=0;i<LEN;i++)
		a[i]=0;
}

void copyarr(int ary1[],int ary2[]){//将后者数组的元素复制到前者。参量为两个数组以及它们的长度。会清除原本数组的内容。
	for(int i=0;i<LEN;i++)//初始化第一个数组
		ary1[i]=0;
	for(int i=0;i<LEN;i++){
		ary1[i]=ary2[i];
	}
}

void plusarr(int a1[],int a2[]){//高精度加法，第二个加到第一个里面。如果已经达到最大位无法进位，则最大位不变
	int length=LEN-1;
	while(a1[length]==9)length--;//如果最大位已经是9了，则认为新的最大位为上一位。避免出现9xxxxxxxxx + 1的结果反而比原数小的情况
	for(int i=0;i<=length;i++)
		a1[i]+=a2[i];
	//putarr(a1);
	//printf("\n");
	int boo=0;//0表示这个结果是负数
	for(int j=LEN-1;j>=0;j--){
		if(a1[j]>0){
			boo=1;
			//printf("\nZZZ\n");
			break;
		}
		if(a1[j]<0){
			//printf("\nFFF\n");
			break;
		}	
	}
	if(boo==1){//正数进位
		
		for(int i=0;i<length;i++){
			if(a1[i]>9){
				if(i!=length-1){
					a1[i+1]+=1;
					a1[i]%=10;
				}
				else{
					a1[i]=9;
				}
			}
			if(a1[i]<0){
				a1[i]+=10;
				a1[i+1]-=1;
			}
		}
	}
	else{//负数的进位
		//printf("\nfu\n");
			for(int i=0;i<LEN;i++){
				if(a1[i]>0){
					if(i!=length-1){
						//printf("%d",a1[i]);
						a1[i]-=10;
						a1[i+1]+=1;
						//printf("  %d  %d\n",a1[i],a1[i+1]);
					}
				}
				if(a1[i]<-10){
					
					if(i!=length-1){
						a1[i+1]-=a1[i]/10;
						a1[i]%=10;
					}
					else{
						break;
					}
				}
			}
		}
	
}

void minusarr(int a1[],int a2[]){//高精度减法，第一个减去第二个。
	int length=LEN;//这里不是写错了，就是LEN，不用减一
	for(int i=0;i<LEN;i++)
		a1[i]-=a2[i];
	//putarr(a1);
	int boo=0;//0表示这个结果是负数
	for(int j=LEN-1;j>=0;j--){
		if(a1[j]>0){
			boo=1;
			//printf("\nZZZ\n");
			break;
		}
		if(a1[j]<0){
			//printf("\nFFF\n");
			break;
		}	
	}
	//printf("\n");
		if(boo==1){//正数的进位
		//printf("\nzheng\n");
			for(int i=0;i<LEN;i++){
				if(a1[i]<0){
					if(i!=length-1){
						a1[i+1]-=1;
						a1[i]+=10;
					}
					else{
						break;
					}
				}
			}
		}
		else{//负数的进位
		//printf("\nfu\n");
			for(int i=0;i<LEN;i++){
				if(a1[i]>0){
					if(i!=length-1){
						//printf("%d",a1[i]);
						a1[i]-=10;
						a1[i+1]+=1;
						//printf("  %d  %d\n",a1[i],a1[i+1]);
					}
				}
				if(a1[i]<-10){
					
					if(i!=length-1){
						a1[i+1]-=a1[i]/10;
						a1[i]%=10;
					}
					else{
						break;
					}
				}
			}
		}
}



void mularr(int a[],float n){//高精度乘法
	int length=LEN;
	while(a[length]==9)length--;//如果最大位已经是9了，则认为新的最大位为上一位(与加法同理)
	for(int i=0;i<length;i++)
		a[i]*=n;
	for(int i=0;i<length;i++){
		if(a[i]>9){
			if(i!=length-1){
				a[i+1]+=a[i]/10;
				a[i]%=10;
			}
			else{
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

void putheart(void){//在屏幕上输出一个爱心！
	int c=0;
	for(int i=0;i<50;i++){
		c=0;
		if(i==2||i==3||i==5||i==6||i==46)c=68;
		if(i>7&&i<29)c=68;
		if(i>29&&i<35)c=68;
		if(i>37&&i<41)c=68;
		COLOR(c);
		printf("%3d",i);
		if(i%7==0)printf("\n");
	}
}

void SlowDisplay(const char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\\' && text[i + 1] == '.') {
            usleep(1000000); // 暂停1秒
            i++; // 跳过'.'
        } else {
            putchar(text[i]); // 逐字符显示
            fflush(stdout);   // 刷新输出缓冲区，确保字符立即显示
            usleep(MS * 1000); // 延迟指定的毫秒数
        }

        /*if (_kbhit()) { // 如果玩家此时敲击了键盘，则退格到这次输出的起始，直接输出整句话
            getch(); // 如果不这么写玩家敲击的键值会一直留在缓冲区，导致循环
            for (int j = 0; j <= i; j++) {
                printf("\b \b"); // 退格
            }
            printf("%s", text);
            break;
        }*/
    }
}

void putarr(int a[]){//输出数据的函数
	int boo=0;
	for(int i=LEN-1;i>=0;i--){
		if(a[i]!=0)boo=1;
		if(boo==1)printf("%d",a[i]);
	}
}

int random(void){//以当前时间（秒为单位）为种子生成一个0～99的随机数
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

void leverUP(int i){//判断是否升级并执行,i使指结构体数组里第i个元素，i+1是这个角色的备份
	int n[LEN]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int ll[LEN]={0,5,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int one[LEN]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	while(comparr(player[i].exp,player[i+1].lexp)==1){
		if(comparr(player[i].lever,ll)==-1){//50级以下
			plusarr(n,one);
			mularr(player[i+1].lexp,1.3);
			mularr(player[i+1].hp,1.5);
			mularr(player[i+1].hd,1.5);
			mularr(player[i+1].attack,1.5);
			mularr(player[i+1].defence,1.5);
		}else{//50级以上
			plusarr(n,one);
			mularr(player[i+1].lexp,2.0);
			mularr(player[i+1].hp,1.8);
			mularr(player[i+1].hd,1.8);
		}
		
	}
	if(comparr(n,one)==1){
		SlowDisplay("蛙趣，连升");
		putarr(n);
		SlowDisplay("级！\n");
	}else if(comparr(n,one)==0){
		SlowDisplay("升级了！");
	}
	plusarr(player[i+1].lever,n);
	copyarr(player[i].lever ,player[i+1].lever);
	copyarr(player[i].lexp ,player[i+1].lexp);
	copyarr(player[i].hp ,player[i+1].hp);
	copyarr(player[i].hd ,player[i+1].hd);
	copyarr(player[i].attack ,player[i+1].attack);
	copyarr(player[i].defence,player[i+1].defence);
	iniarr(player[i].exp);
}



void attackact(struct Player *player,struct Monster monster){//战斗模块                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        妈的，写累死了都没写完。??                                                                                                            
	system("cls");//清除控制台
	SlowDisplay(monster.name);SlowDisplay(monster.v);SlowDisplay("！\n");
	int k=1;//有一方血量为零或逃跑成功时k=0
	int harm[LEN];//攻击产生的伤害
	int hpplus[LEN];//技能或道具回复的血量
	copyarr(harm,ablank);
	while(k){//循环战斗过程

	 //下面是玩家的回合
	 PlayerRand:
		puts("\n现在要怎么办？");
		puts("=============================================================================");
		puts("1. 攻击   2. 使用物品   3.  查看   4.  逃跑\n\n");
		switch(fflush(stdin),choosenum=-1,scanf("%d",&choosenum),system("cls"),choosenum){//读入玩家选择并判断;fflush(stdin)刷新输入缓冲区
			case 1:
				puts("要用什么技能？");
				int s_index=0;
				for(int i=0;i<spnum;i++)//创建当前技能列表
					if(skill_list[i].stu)//判断这个技能是否习得
						sarr[s_index++]=skill_list[i];
				//s_index=0;
				puts("=============================================================================");
				for(int i=0;i<s_index;i++)//输出当前技能列表
					printf("   %d.  %s",i,sarr[i].name);
				if(random()==1)
					printf("   114514.   什么都不做\n");
				printf("\n");
				puts("=============================================================================\n\n");
				//现在技能列表按照其在技能数组里的索引值排列了
				choosenum=-1;
				scanf("%d",&choosenum);//读入选择的技能
				fflush(stdin);
				if(choosenum==114514){
					SlowDisplay(player->name);
					SlowDisplay("说：\"我什么都做不到！\"\n\n");
					usleep(1000*1000);
					printf("突然！天空中出现一道光芒！\n");
					SlowDisplay("某处传来一个声音：“版本之子，启动！”\n");
					player->attp=&wq[3];
					player->defp=&fj[7];
					mularr(player->attack,9999);
					mularr(player->hp,9999);
					mularr(player->hd,9999);
					usleep(500*1000);
					printf("%s得到了加强！\n%s得到了超模神器！\n\n",player->name,player->name);
					break;
				}else if(choosenum>(sizeof(sarr)/sizeof(sarr[0]))||choosenum==-1){
					printf("………………\n");
					SlowDisplay(waring_content);
					system("cls");//清除控制台
					goto PlayerRand;
				}

				if(comparr(player->hd,sarr[choosenum].sp)!=1){//如果蓝不够
					SlowDisplay("你的法力不足！！\n");
					goto PlayerRand;//重新开始这回合
				}
				minusarr(player->hd,sarr[choosenum].sp);//减少蓝
				
				copyarr(harm,player->attack);//读取攻击力
				
				plusarr(harm,player->attp->num);//武器加成
				
				mularr(harm,sarr[choosenum].damage);//计算伤害倍率

				minusarr(harm,monster.defence);//计算最终伤害

				SlowDisplay(player->name);
				SlowDisplay("使出了");
				SlowDisplay(sarr[choosenum].name);
				printf("！");
				SlowDisplay(sarr[choosenum].suffix);
				SlowDisplay(monster.name);
				SlowDisplay("  受到了  ");
				COLOR(sarr[choosenum].ele);
				putarr(harm);
				COLOR(7);
				SlowDisplay("  点伤害!\n");
				minusarr(monster.hp,harm);//减少怪物的hp
				
				break;//攻击-跳出玩家回合
				
				
			case 2:
				SlowDisplay("要使用什么？");
				puts("\n=============================================================================");
				int i=0;
				//struct Prop *head=&sl;
				SlowDisplay("\n攻击道具：\n");
				for(int gji=0,t=0;gji<50;gji++){
					if(gj[gji].havenum){
						parr[i]=&gj[gji];
						printf("%d . %s:%d个\t",i,parr[i]->name,parr[i]->havenum);
						i++,t++;
						if(t==5)printf("\n\n"),t=1;
					}
				}
				SlowDisplay("\n防御道具：\n");
				for(int fyi=0,t=0;fyi<50;fyi++){
					if(fy[fyi].havenum){
						parr[i]=&fy[fyi];
						printf("%d . %s:%d个\t",i,parr[i]->name,parr[i]->havenum);
						i++,t++;
						if(t==5)printf("\n\n"),t=1;
					}
				}
				SlowDisplay("\n回复道具：\n");
				for(int hfi=0,t=0;hfi<50;hfi++){
					if(hf[hfi].havenum){
						parr[i]=&hf[hfi];
						printf("%d . %s:%d个\t",i,parr[i]->name,parr[i]->havenum);
						i++,t++;
						if(t==5)printf("\n\n"),t=1;
					}
				}
				puts("\n=============================================================================");
				fflush(stdin);
				choosenum=-1;
				scanf("%d",&choosenum);
				if(choosenum<=-1&&choosenum>i){
				printf("………………\n");
				SlowDisplay(waring_content);
				system("cls");//清除控制台
				goto PlayerRand;
				}
				puts("\n");
				SlowDisplay(player->name);
				SlowDisplay(" 使用了 ");
				SlowDisplay(parr[choosenum]->name);
				puts("!!\n");
				switch(parr[choosenum]->type){
					case 2://攻击型道具
						copyarr(harm,parr[choosenum]->num);//读取攻击力

						minusarr(harm,monster.defence);//计算最终伤害
						
						SlowDisplay(monster.name);
						SlowDisplay("  受到了  ");
						COLOR(parr[choosenum]->ele);
						putarr(harm);
						COLOR(7);
						SlowDisplay("  点伤害!\n");
						minusarr(monster.hp,harm);//减少怪物的hp
						parr[choosenum]->havenum-=1;
						break;//判断物品类型-跳到使用物品
						
					case 3://回复型道具
						copyarr(hpplus,parr[choosenum]->num);
						plusarr(player->hp,hpplus);
						SlowDisplay(player->name);
						SlowDisplay(" 回复了 ");
						putarr(hpplus);
						SlowDisplay("点血！\n");
						if(comparr(player->hp,player[1].hp)==1){
							copyarr(player->hp,player[1].hp);
							SlowDisplay("回满了！\n");
						}else{
							SlowDisplay("现在有 ");
							putarr(player->hp);
							SlowDisplay(" 点血量！\n");
						}
						parr[choosenum]->havenum-=1;
						break;//判断物品类型-跳到使用物品

				}
				system("cls");//清除控制台
				break;//使用物品-跳出玩家回合
				
				
			case 3:
				SlowDisplay(player->name);
				SlowDisplay(":\nhp:");
				putarr(player->hp);printf("/");putarr(player[1].hp);
				SlowDisplay("\nhd:");
				putarr(player->hd);printf("/");putarr(player[1].hd);
				SlowDisplay("\n攻击力:");
				putarr(player->attack);
				SlowDisplay("\n防御力:");
				putarr(player->defence);
				SlowDisplay("\n:当前武器:");
				printf("%s",player->attp->name);
				SlowDisplay("\n:当前防具:");
				printf("%s",player->defp->name);
				SlowDisplay("\n:当前等级:");
				putarr(player->lever);
				
				printf("\n");
				SlowDisplay(monster.name);
				SlowDisplay(":\nhp:");
				putarr(monster.hp);
				
				SlowDisplay("\n攻击力:");
				putarr(monster.attack);
				SlowDisplay("\n防御力:");
				putarr(monster.defence);
				printf("\n\n\n按任意键继续。。");
				getchar();
				system("cls");//清除控制台
				goto PlayerRand;//查看-重新开始玩家回合
			case 4:
				if(random()<50){
					SlowDisplay("逃跑成功！耶！");
					iniarr(monster.exp);
					monster.exp[0]=1;//如果逃跑则没有经验
					k=0;
					//goto ENDATTACK;
				}else{
					SlowDisplay("逃跑失败！这下该怎么办啊啊啊啊啊！！\n");
					break;//逃跑-跳出玩家回合
				}
			default:
				printf("………………\n");
				SlowDisplay(waring_content);
				goto PlayerRand;
		}
		if(comparr(monster.hp,ablank)!=1){//如果怪物被击败
			SlowDisplay(monster.name);
			SlowDisplay(" 被击败了！！\n");
			if(random()<addpro){//判断是否掉落物品
				monster.prop->havenum+=1;
				SlowDisplay("获得了 ");
				SlowDisplay(monster.prop->name);
				printf("！\n");
				//printf("%d",monster.prop->havenum);
			}
			plusarr(player->exp,monster.exp);
			leverUP(0);
			k=0;
			break;//结束战斗
		}
	 
	 //MonsterRand:
	//下面是怪物的回合
		if(k){
			if(random()<=escpro){
				SlowDisplay(monster.name);
				SlowDisplay(" 逃跑了！\0");
				iniarr(monster.exp);
				monster.exp[0]=1;//如果逃跑则没有经验
				break;
			}
			copyarr(harm,monster.attack);
		
			//putarr(harm);printf("攻击力\n");
			//putarr(player->hp);printf("初始血量\n");
			
			if(random()<=monster.pro){
				struct Skill* msnum=monster.snum;
		
				mularr(harm,msnum->damage);
		
				//minusarr(harm,player->defence);//计算伤害
		
				//putarr(harm);printf("伤害\n");
				//printf("com%d\n",comparr(harm,ablank));
				int fdef[LEN];
				copyarr(fdef,player->defence);
				plusarr(fdef,player->defp->num);//防具加成
				if(comparr(harm,fdef)!=1){//如果防御太高则伤害为1
					iniarr(harm);
					harm[0]=1;
					//printf("\n修正后\n");
				}else{
					minusarr(harm,fdef);//计算伤害
				}
		
				SlowDisplay(monster.name);
				SlowDisplay(" 使用了 ");
				SlowDisplay(msnum->name);
				SlowDisplay("!!\n\n");
				SlowDisplay(player->name);
				SlowDisplay("受到了 ");
				putarr(harm);
				SlowDisplay(" 点伤害！\n\n");
				minusarr(player->hp,harm);
				//putarr(player->hp);printf("血量\n");
			}else{
				SlowDisplay(monster.name);
				SlowDisplay("走神了！什么都没做！");
			}
			if(comparr(player->hp,ablank)!=1){//玩家被击败
				SlowDisplay(player->name);
				SlowDisplay("被击败了！！\n");
				iniarr(player->hp);
				player->hp[0]=1;
				break;//结束战斗
			}
		}
				
				
	}
		
	//ENDATTACK:
		SlowDisplay("战斗结束！\n\n");
		printf("\n\n按任意键继续。。");
		getchar();
		system("cls");//清除控制台
	
}

void hotel(void){//回满状态的函数，在旅馆等地点调用
	player[0]=player[1];
	
}

void bag(void){//察看背包及其它操作函数
	int hpplus[LEN];//技能或道具回复的血量
	int k=1;
	
	while(k){
		system("cls");//清除控制台
		SlowDisplay("\n背包内容：\n");
		puts("\n=============================================================================");
		SlowDisplay("1.防具\t\t2.武器\t\t3.道具\t\t4.纪念品\t\t114514.点错了。。。。。\n\n");
		fflush(stdin);
		choosenum=-1;
		scanf("%d",&choosenum);
		fflush(stdin);
		int i=0;
		switch(choosenum){
			case -1:
				system("cls");//清除控制台
				printf("………………\n");
				SlowDisplay(waring_content);
				break;
			case 1:
				system("cls");//清除控制台
				SlowDisplay("\n防具：\n");
				for(int fji=0,t=0;fji<50;fji++){
					if(fj[fji].havenum){
						parr[i]=&fj[fji];
						printf("%d . %s:%d个\t",i,parr[i]->name,parr[i]->havenum);
						i++,t++;
						if(t==5)printf("\n\n"),t=0;
					}
				}
				int fjnum=i;
				printf("\n");
				fflush(stdin);
				choosenum=-1;
				scanf("%d",&choosenum);
				fflush(stdin);
				if(choosenum==-1||choosenum>fjnum){
					printf("………………\n");
					SlowDisplay(waring_content);
					break;
				}
				SlowDisplay(player[0].name);
				SlowDisplay("将");
				SlowDisplay(player[0].defp->name);
				SlowDisplay("换成了");
				SlowDisplay(parr[choosenum]->name);
				printf("!\n");
				player[0].defp=parr[choosenum];
				player[1]=player[0];
			
				break;
			case 2:
				system("cls");//清除控制台
				SlowDisplay("\n武器：\n");
				for(int wqi=0,t=0;wqi<50;wqi++){
					if(wq[wqi].havenum){
						parr[i]=&wq[wqi];
						printf("%d . %s:%d个\t",i,parr[i]->name,parr[i]->havenum);
						i++,t++;
						if(t==5)printf("\n\n"),t=0;
					}
				}
				int wqnum=i;
				printf("\n");
				fflush(stdin);
				choosenum=-1;
				scanf("%d",&choosenum);
				fflush(stdin);
				if(choosenum==-1||choosenum>wqnum){
					printf("………………\n");
					SlowDisplay(waring_content);
					break;
				}
				SlowDisplay(player[0].name);
				SlowDisplay("将");
				SlowDisplay(player[0].attp->name);
				SlowDisplay("换成了");
				SlowDisplay(parr[choosenum]->name);
				printf("!\n");
				player[0].attp=parr[choosenum];
				player[1]=player[0];
				break;
			case 3:
				system("cls");//清除控制台
				SlowDisplay("\n攻击道具：\n");
				for(int gji=0,t=0;gji<50;gji++){
					if(gj[gji].havenum){
						parr[i]=&gj[gji];
						printf("%d . %s:%d个\t",i,parr[i]->name,parr[i]->havenum);
						i++,t++;
						if(t==5)printf("\n\n"),t=0;
					}
				}
				int gjnum=i-1;
				SlowDisplay("\n防御道具：\n");
				for(int fyi=0,t=0;fyi<50;fyi++){
					if(fy[fyi].havenum){
						parr[i]=&fy[fyi];
						printf("%d . %s:%d个\t",i,parr[i]->name,parr[i]->havenum);
						i++,t++;
						if(t==5)printf("\n\n"),t=0;
					}
				}
				int fynum=i;
				SlowDisplay("\n回复道具：\n");
				for(int hfi=0,t=0;hfi<50;hfi++){
					if(hf[hfi].havenum){
						parr[i]=&hf[hfi];
						printf("%d . %s:%d个\t",i,parr[i]->name,parr[i]->havenum);
						i++,t++;
						if(t==5)printf("\n\n"),t=0;
					}
				}
				int hfnum=i;
				SlowDisplay("\n您想使用啥？先说好非战斗状态不能使用攻击和防御道具哦！\n");
				printf("\n");
				
				fflush(stdin);
				choosenum=-1;
				scanf("%d",&choosenum);
				fflush(stdin);
				if(choosenum<=-1||choosenum>hfnum){
					printf("………………\n");
					SlowDisplay(waring_content);
					usleep(1500*1000);
					break;
				}
				if(choosenum>=0&&choosenum<=gjnum){
					SlowDisplay("哈？你是不是没看见我说的啥？指望攻击道具虚空索敌？\n");
					usleep(1500*1000);
				}
				else if(choosenum<fynum){
					SlowDisplay("…………芳斯塔夫说过：叠甲死路一条。\n");
					usleep(1500*1000);

				}else if(choosenum<hfnum){
					copyarr(hpplus,parr[choosenum]->num);
					plusarr(player[0].hp,hpplus);
					parr[choosenum]->havenum-=1;
					SlowDisplay(player[0].name);
					SlowDisplay("使用了");
					SlowDisplay(parr[choosenum]->name);
					SlowDisplay(" ,回复了 ");
					putarr(hpplus);
					SlowDisplay("点血！\n");
					if(comparr(player[0].hp,player[1].hp)==1){
						copyarr(player[0].hp,player[1].hp);
						SlowDisplay("回满了！\n");
					}
					SlowDisplay("现在有 ");
					putarr(player[0].hp);
					SlowDisplay(" 点血量！\n");
				}
				
				break;
			case 4:{
				system("cls");//清除控制台
				SlowDisplay("用WASD或方向键选择物品：\n");
				int i=0,t=1;
				for(int si=0;si<50;si++){
					if(souvenir[si].have){
						soarr[i]=&souvenir[si];
						printf("%d.  %s\t\t",i,soarr[i]->name);
						i++,t++;
						if(t==5)printf("\n\n"),t=1;
					}
				}
				puts("\n=============================================================================");
				printf("\n");
				choosenum=0;
				t=1;
				int kg=1;//判断是否退出纪念品查看
				int printf_kg=1;//是否输出纪念品列表
				while(1){
					usleep(1);
					if(_kbhit()){
						int choosechar=getch();
						switch(choosechar){
							case 'w':{
								if(choosenum==0)kg=0;
								choosenum-=4;
								if(choosenum<0)choosenum=0;
								break;
							}
							case 'W':{
								if(choosenum==0)kg=0;
								choosenum-=4;
								if(choosenum<0)choosenum=0;
								break;
							}
							case 'a':{
								if(choosenum==0)kg=0;
								choosenum-=1;
								if(choosenum<0)choosenum=0;
								break;
							}
							case 'A':{
								if(choosenum==0)kg=0;
								choosenum-=1;
								if(choosenum<0)choosenum=0;
								break;
							}
							case 's':{
								if(choosenum==i-1)kg=0;
								choosenum+=4;
								if(choosenum>i-1)choosenum=i-1;
								break;
							}
							case 'S':{
								if(choosenum==i-1)kg=0;
								choosenum+=4;
								if(choosenum>i-1)choosenum=i-1;
								break;
							}
							case 'd':{
								if(choosenum==i-1)kg=0;
								choosenum+=1;
								if(choosenum>i-1)choosenum=i-1;
								break;
							}
							case 'D':{
								if(choosenum==i-1)kg=0;
								choosenum+=1;
								if(choosenum>i-1)choosenum=i-1;
								break;
							}
							case 'r':{
								if(soarr[choosenum]->read){
									MS=10;
									SlowDisplay(soarr[choosenum]->content);
									//printf("\n\n\n\n\n(按任意键继续……)\n");
									//getchar();
									printf_kg=0;
								}
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
							
							
							
							case 0xE0:{//判断是不是方向键。当按下一个方向键时，实际上会生成一系列字符。通常情况下，方向键会发送一个转义序列，以ASCII码的形式表示。
								int ch = _getch();
                				switch (ch) {
									case 72:{//上
										if(choosenum==0)kg=0;
										choosenum-=4;
										if(choosenum<0)choosenum=0;
										break;
									}
									case 80:{//下
										if(choosenum==i-1)kg=0;
										choosenum+=4;
										if(choosenum>i-1)choosenum=i-1;
										break;
									}
									case 77:{//右
										if(choosenum==i-1)kg=0;
										choosenum+=1;
										if(choosenum>i-1)choosenum=i-1;
										break;
									}
									case 75:{//左
										if(choosenum==0)kg=0;
										choosenum-=1;
										if(choosenum<0)choosenum=0;
										break;
									}
               					}
								break;
							}
							case 0:{//有些键盘第一个字符可能是0
								int ch = _getch();
                				switch (ch) {
									case 72:{//上
										if(choosenum==0)kg=0;
										choosenum-=4;
										if(choosenum<0)choosenum=0;
										break;
									}
									case 80:{//下
										if(choosenum==i-1)kg=0;
										choosenum+=4;
										if(choosenum>i-1)choosenum=i-1;
										break;
									}
									case 77:{//右
										if(choosenum==i-1)kg=0;
										choosenum+=1;
										if(choosenum>i-1)choosenum=i-1;
										break;
									}
									case 75:{//左
										if(choosenum==0)kg=0;
										choosenum-=1;
										if(choosenum<0)choosenum=0;
										break;
									}
               					}
								break;
							}
							
							default:
								break;
						}
						if(kg==0)break;
						if(printf_kg){
							system("cls");//清除控制台
							printf("用WASD或方向键选择物品：\n");
							for(int si=0;si<i;si++){
								if(si==choosenum){
									COLOR(240);
									printf("%d.  %s",si,soarr[si]->name);
									COLOR(7);
									printf("\t\t");
									t++;
								}else{
									printf("%d.  %s\t\t",si,soarr[si]->name);
									t++;
								}
								if(t==5)printf("\n\n"),t=1;
							}
							t=1;
							puts("\n=============================================================================");
							if(soarr[choosenum]->read)printf("可阅读(R)。\n");
							printf("%s\n",soarr[choosenum]->intro);
							printf("\n");
						}
						
						printf_kg=1;
					}
				}
				break;
			}
				
			case 114514:{
				system("cls");//清除控制台
				k=0;
				break;
			}

				
		}
		
		
	}
}
	
	
	


	
	





















