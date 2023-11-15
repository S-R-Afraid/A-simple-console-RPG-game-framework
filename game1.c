#include <stdlib.h>
#include <stdio.h>
#include <time.h> //suiji
#include <string.h>
#include <unistd.h>	 //包含延迟函数的头文件，例如 usleep
#include <windows.h> //颜色函数
//每次保存时不要以utf-8格式
#define LEN 15//数据的位数
#define MS 50//输出间隔
#define escpro 5//怪物逃跑的概率
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

struct Player{//玩家结构体
	char *name;//结构体定义的时候不能被初始化
	int hd[LEN];//血
	int hp[LEN];//蓝
	int attack[LEN];//攻击力
	int defence[LEN];//防御力
	int miss;//敏捷，即闪避概率
	int sp[LEN];//技能列表
	int lever[LEN];//等级
	int exp[LEN];//经验
	int defnum;//防御道具编号
	int attnum;//武器编号
};

struct Monster{//怪物结构体
	char *name;
	int hd[LEN];
	int attack[LEN];
	int defence[LEN];
	int miss;
	int snum;//技能名称
	int pro;//发动技能的概率
	int exp[LEN];//怪物能给与的经验
	char *v;//怪物出场提示
};

struct Map{//地图结构体
	char *name;
	int n;//在当前地图中可以做出的行为的数量
	char v[10][20];//在当前地图中可以做出的行为
	int vn[10];//行为对应的代码
};

struct Skill{//技能结构体
	int num;//编号
	char *name;//技能名称
	int sp[LEN];//消耗蓝量
	int damage;//攻击力倍率
	int boo;//是否对血量有影响，加血1，减血-1，没有影响0
	int hd[LEN];//一些技能会增加或消耗血量
	int ele;//元素种类，15物理，12火，9水，3冰，5雷, 4特殊
};

struct Prop{//定义道具结构体
	char *name;
	char num[LEN];//道具的数值，如增加的防御力、攻击力，或造成的伤害、回复的血量等等
	int ele;//如果是伤害，伤害的元素类型
};
/*------------初始化玩家------------------*/
struct Player player={
	.name="文止墨\0",
	.hd={0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
	.hp={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.attack={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.defence={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.miss=10,
	.sp={0,1,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	.lever={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.exp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.defnum=0,
	.attnum=0,
};

/*-----------下面初始化技能---------*/
struct Skill blank={
	.num=-1,
	.name="\0",
	.sp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.damage=0,
	.boo=0,
	.hd={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.ele=4,
};

struct Skill zero={
	.num=0,
	.name="普通攻击\0",
	.sp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.damage=1,
	.boo=0,
	.hd={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.ele=15,
};
struct Skill one={
	.num=1,
	.name="弹跳碰撞\0",
	.sp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.damage=1,
	.boo=0,
	.hd={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.ele=15,
};

struct Skill two={
	.num=2,
	.name="[Error] conflicting types for 'attackact'\0",
	.sp={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.damage=2,
	.boo=0,
	.hd={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.ele=4,
};
/*-----------下面初始化各个怪物-----------*/
struct Monster slm={
	.name="史莱姆\0",
	.hd={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},//初始100血
	.attack={0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.defence={0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.snum=1,
	.pro=20,
	.exp={0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	.v="跳了过来\0",
};
struct Monster error={
	.name="ERROR怪\0",
	.hd={0,0,0,0,0,0,0,9,9,9,9,9,9,9,9},
	.attack={0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.defence={0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	.snum=2,
	.pro=20,
	.exp={0,0,0,2,0,0,0,0,0,0,0,0,0,0},
	.v="出现了\0",
};
struct Monster monster={
	.name="\0",
	.hd={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
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
	
void pulsarr(int a1[],int a2[]);
//高精度加法，第二个加到第一个里面。如果已经达到最大位无法进位，则最大位不变

void minusarr(int a1[],int a2[]);
//高精度减法，第一个减去第二个

void mularr(int a[],int n);
//高精度乘法

int comparr(int ary1[],int ary2[]);
//高精度比较两个数的大小，前者大于后者返回1，小于返回-1，等于返回0

void putheart(void);
//在屏幕上输出一个爱心！

void SlowDisplay(const char *text);
//输出字符函数

void putarr(int a[]);
//输出数据的函数

void attackact(struct Player player,struct Monster monster);
//攻击行为函数。运行此函数将进入战斗直到一方倒下或逃跑（未完成）

int random();
//以当前时间（秒为单位）为种子生成一个0～9的随机数

struct Skill readskill(int s);
//通过技能编号读取技能信息的函数


/*---------定义变量，因为函数也要用到这些变量，所以定义到main外面--------*/
int choosenum=-1;//记录玩家的选择
int slank[LEN]={0};//空数组，用于判断血量等是否为0；
struct Skill sarr[LEN];//储存当前技能列表

/*---------main函数-----------*/
int main()
{
	monster=error;
	attackact(player,monster);
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

void pulsarr(int a1[],int a2[]){//高精度加法，第二个加到第一个里面。如果已经达到最大位无法进位，则最大位不变
	int length=LEN;
	while(a1[length]==9)length--;//如果最大位已经是9了，则认为新的最大位为上一位。避免出现9xxxxxxxxx + 1的结果反而比原数小的情况
	for(int i=0;i<length;i++)
		a1[1]+=a2[i];
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
	}
}

void minusarr(int a1[],int a2[]){//高精度减法，第一个减去第二个。
	int length=LEN;
	if(a1[LEN]>=0){//判断这个数是不是已经小于0了
		for(int i=0;i<LEN;i++)
			a1[i]-=a2[i];
		for(int i=0;i<LEN;i++){
			if(a1[i]<0){
				if(i!=length-1){
					a1[i+1]-=1;
					a1[i]+=10;
				}
				else{
					a1[i]=9;
				}
			}
		}
	}
	
}

void mularr(int a[],int n){//高精度乘法
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
int comparr(int ary1[],int ary2[]){//比较，前者大于后者返回1，小于返回-1，等于返回0
	int boo=1;
	for(int k=LEN;k>=0;k--){
		if(ary1[k]>ary2[k]){
			break;
		}
		if(ary1[k]<ary2[k]){
			boo=-1;
		}
		if(k==0)boo=0;
	}
	return boo;
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

void SlowDisplay(const char *text)
{
	for (int i = 0; text[i] != '\0'; i++)
	{
		putchar(text[i]);		 // 逐字符显示
		fflush(stdout);			 // 刷新输出缓冲区，确保字符立即显示
		usleep(MS* 1000); // 延迟指定的毫秒数
	}
}

void putarr(int a[]){//输出数据的函数
	int boo=0;
	for(int i=LEN;i>=0;i--){
		if(a[i]!=0)boo=1;
		if(boo==1)printf("%d",a[i]);
	}
}

int random(){//以当前时间（秒为单位）为种子生成一个0～99的随机数
	srand((unsigned)time(NULL));
	return rand() % 100;
}

struct Skill readskill(int s){//通过技能编号读取技能信息的函数
	struct Skill re;
	if(s==0)re=zero;
	if(s==1)re=one;
	if(s==2)re=two;
	
	return re;
}


void attackact(struct Player player,struct Monster monster){//战斗模块(未完成）
	SlowDisplay(monster.name);SlowDisplay(monster.v);SlowDisplay("！\n");
	int k=1;//有一方血量为零或逃跑成功时k=0
	int harm[LEN];//攻击产生的伤害
	while(k){//循环战斗过程
	
	//下面是玩家的回合
		puts("\n现在要怎么办？");
		puts("=============================================================================");
		puts("1. 攻击   2. 使用物品   3.  查看对方   4.  逃跑");
		switch(scanf("%d",&choosenum),choosenum){//读入玩家选择并判断
			case 1:
				puts("要用什么技能？");
				int s_index=0;
				for(int i=0;i<LEN;i++)//创建当前技能列表
					if(player.sp[i]!=-1)
						sarr[s_index++]=readskill(player.sp[i]);
				//s_index=0;
				puts("=============================================================================");
				for(int i=0;i<s_index;i++)//输出当前技能列表
					printf("%d.  %s",i,sarr[i].name);
				printf("\n");
				puts("=============================================================================");
				//现在技能列表按照其在技能数组里的索引值输出了
				scanf("%d",&choosenum);//读入选择的技能
				copyarr(harm,player.attack);//读取攻击力
				mularr(harm,sarr[choosenum].damage);//计算伤害倍率
				minusarr(harm,monster.defence);//计算最终伤害
				SlowDisplay(player.name);
				SlowDisplay("使用了");
				SlowDisplay(sarr[choosenum].name);
				printf("！\n\n");
				SlowDisplay(monster.name);
				SlowDisplay("  受到了  ");
				COLOR(sarr[choosenum].ele);
				putarr(harm);
				COLOR(7);
				printf("  点伤害!\n");
				
		}
	//下面是怪物的回合
		
		
	}
}







