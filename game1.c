#include "function.h"


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
		}else if(text[i] == '\\' && text[i + 1] == 'n') {
			printf("\n");
			i++;
		}else if(text[i] == '\\' && text[i + 1] == 't') {
			printf("\t");
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
	int t_round=0;//总回合数
	int roundnum=-1;//当前是列表里第几个人行动
	long harm;//攻击产生的伤害
	long hpplus;//技能或道具回复的血量
	int win_p;//判断玩家是否胜利
	int win_m;//判断怪物是否胜利
	//copyarr(harm,ablank);
	struct Player *player;
	struct Monster monsterarr[egroup.num];
	struct Monster *monster;
	int player_buff_num[TeamNum];
	int monster_buff_num[egroup.num];//玩家和怪物的buff数量
	struct Prop *attp;
	struct Prop *defp;
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
	win_p=1,win_m=1;
	for(int i=0; i<TeamNum; i++) {//由于可能会在战斗中使用道具使速度改变，因此放在回合开始后初始化
		speedlist[i]=players[i].speed;
		if(players[i].hp>0)win_m=0;
	}
	for(int i=TeamNum; i<actnum; i++) {
		speedlist[i]=monsterarr[i-TeamNum].speed;
		if(monsterarr[i-TeamNum].hp>0)win_p=0;
	}
	if(win_p||win_m) {
		goto ENDATTACK;
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
	t_round++;
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
	attp=index2prop(player->attp);
	defp=index2prop(player->defp);
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
						if(player->hp>player->f_hp)player->hp=player->f_hp;
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
					puts(" ");
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

				if((choosenum==114514||choosenum==s_index-1)&&cheat) {
					SlowDisplay(player->name,0);
					SlowDisplay("说：\"我什么都做不到！\"\n\n",0);
					usleep(500*1000);
					SlowDisplay("突然！天空中出现一道光芒！\n",0);
					SlowDisplay("某处传来一个声音：“密码正确。版本之子，启动！”\n",0);
					player->attp=3;
					player->defp=7;
					player->attack*=9999;
					player->hp=999999999;
					player->hd=999999999;
					player->f_hp=999999999;
					player->f_hd=999999999;
					usleep(500*1000);
					printf("%s得到了加强！\n%s得到了超模神器！\n\n",player->name,player->name);
					goto PlayerRound;
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

						harm+=attp->num;//武器加成

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
						if(sarr[skillchoosenum].status) {
							monster_buff[monsterchoose[i]][monster_buff_num[monsterchoose[i]]++]=buffs[sarr[skillchoosenum].status];//给怪挂buff
							SlowDisplay(monster->name,0);
							SlowDisplay("  被附加了  ",0);
							COLOR(buffs[sarr[skillchoosenum].status].n);
							printf("%s",buffs[sarr[skillchoosenum].status].name);
							COLOR(7);
							SlowDisplay(" 状态!\n",0);
						}

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

						harm+=attp->num;//武器加成

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
						if(sarr[choosenum].status) {
							monster_buff[i][monster_buff_num[i]++]=buffs[sarr[choosenum].status];//给怪挂buff
							SlowDisplay(monster->name,0);
							SlowDisplay("  被附加了  ",0);
							COLOR(buffs[sarr[choosenum].status].n);
							printf("%s",buffs[sarr[choosenum].status].name);
							COLOR(7);
							SlowDisplay(" 状态!\n",0);
						}

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

							harm+=attp->num;//武器加成

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
							if(sarr[skillchoosenum].status) {
								player_buff[teamchoose[i]][player_buff_num[teamchoose[i]]++]=buffs[sarr[skillchoosenum].status];//给队友上buff
								SlowDisplay(teamer->name,0);
								SlowDisplay("  被附加了  ",0);
								COLOR(buffs[sarr[skillchoosenum].status].n);
								printf("%s",buffs[sarr[skillchoosenum].status].name);
								COLOR(7);
								SlowDisplay(" 状态!\n",0);
							}


						}
					}  else { //负面效应
						for(int i=0; i<teamchoosenum; i++) {
							harm=player->attack;//读取攻击力

							harm+=attp->num;//武器加成

							harm*=sarr[skillchoosenum].damage;//计算伤害倍率
							struct Player *teamer=&players[teamchoose[i]];

							SlowDisplay(teamer->name,0);
							SlowDisplay("  受到了  ",0);
							COLOR(sarr[skillchoosenum].ele);
							printf("%ld",0-harm);
							COLOR(7);
							SlowDisplay("  点痛击!\n",0);
							teamer->hp-=harm;
							if(sarr[skillchoosenum].status) {
								player_buff[teamchoose[i]][player_buff_num[teamchoose[i]]++]=buffs[sarr[skillchoosenum].status];//给队友上buff
								SlowDisplay(teamer->name,0);
								SlowDisplay("  被附加了  ",0);
								COLOR(buffs[sarr[skillchoosenum].status].n);
								printf("%s",buffs[sarr[skillchoosenum].status].name);
								COLOR(7);
								SlowDisplay(" 状态!\n",0);//减少队友的hp
							}

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
					printf("%s",index2prop(players[i].attp)->name);
					SlowDisplay("\n:当前防具:",0);
					printf("%s",index2prop(players[i].defp)->name);
					SlowDisplay("\n:当前等级:",0);
					printf("%ld",players[i].lever);
					SlowDisplay("\n暴击率：",0);
					printf("%f%%",players[i].crit_rate*100);
					SlowDisplay("\n暴击伤害：",0);
					printf("%f%%\n\n",players[i].crit_damage*100);
				}


				for(int pp=0; pp<egroup.num; pp++) {
					if(monsterarr[pp].hp>0) {
						printf("\n");
						SlowDisplay(monsterarr[pp].name,0);
						SlowDisplay(":\nhp:",0);
						printf("%ld",monsterarr[pp].hp);
						SlowDisplay("\n攻击力:",0);
						printf("%ld",monsterarr[pp].attack);
						SlowDisplay("\n防御力:",0);
						printf("%ld\n",monsterarr[pp].defence);
					}

				}

				wait();
				system("cls");//清除控制台
				goto PlayerRound;//查看-重新开始玩家回合

			}

		case 4: {
				if(e) {
					if(random()<50) {
						SlowDisplay("逃跑成功！耶！",0);
						//iniarr(monster->exp);
						//monster->exp[0]=1;//如果逃跑则没有经验

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
	printf("%s开始行动了！\n",monster->name);
	if(monster_buff_num[actlist[roundnum]-TeamNum]) {//对buff进行处理
		for(int i=0; i<monster_buff_num[actlist[roundnum]-TeamNum]; i++) {
			for(int j=0; j<4; j++) {
				if(monster_buff[actlist[roundnum]-TeamNum][i].t_gain[j]) {
					SlowDisplay(monster->name,0);
					SlowDisplay("受到了来自状态",0);
					COLOR(monster_buff[actlist[roundnum]-TeamNum][i].n);
					printf(" %s ",monster_buff[actlist[roundnum]-TeamNum][i].name);
					COLOR(7);
					SlowDisplay("施加的",0);
					COLOR(monster_buff[actlist[roundnum]-TeamNum][i].n);
					printf(" %ld ",monster_buff[actlist[roundnum]-TeamNum][i].t_gain[j]);
					COLOR(7);
					if(j==0) {
						if(monster_buff[actlist[roundnum]-TeamNum][i].t_gain[j]>0) {
							SlowDisplay("点攻击力提高！",0);
						} else {
							SlowDisplay("点攻击力降低！",0);
						}
						monster->attack+=monster_buff[actlist[roundnum]-TeamNum][i].t_gain[j];
						if(monster->attack<0)monster->attack=1;
					}
					if(j==1) {
						if(monster_buff[actlist[roundnum]-TeamNum][i].t_gain[j]>0) {
							SlowDisplay("点防御力提高！",0);
						} else {
							SlowDisplay("点防御力降低！",0);
						}
						monster->defence+=monster_buff[actlist[roundnum]-TeamNum][i].t_gain[j];
						if(monster->defence<0)monster->defence=1;
					}
					if(j==2) {
						if(monster_buff[actlist[roundnum]-TeamNum][i].t_gain[j]>0) {
							SlowDisplay("点生命值提高！",0);
						} else {
							SlowDisplay("点生命值降低！",0);
						}
						monster->hp+=monster_buff[actlist[roundnum]-TeamNum][i].t_gain[j];
						if(monster->hp<0) {
							SlowDisplay("太棒啦！居然被状态打败了！！",0);
							goto StartAction;
						}
					}
					if(j==3) {
						if(monster_buff[actlist[roundnum]-TeamNum][i].t_gain[j]>0) {
							SlowDisplay("点速度提高！",0);
						} else {
							SlowDisplay("点速度降低！",0);
						}
						monster->speed+=monster_buff[actlist[roundnum]-TeamNum][i].t_gain[j];
						if(monster->speed<0)monster->speed=1;
					}
					puts(" ");
				}
			}
			monster_buff[actlist[roundnum]-TeamNum][i].t--;
			if(monster_buff[actlist[roundnum]-TeamNum][i].t==0) { //该buff已过期
				monster_buff_num[actlist[roundnum]-TeamNum]--;
				monster_buff[actlist[roundnum]-TeamNum][i] = monster_buff[actlist[roundnum]-TeamNum][monster_buff_num[actlist[roundnum]-TeamNum]];
				//把列表最后一位移到当前位置
			}
		}
	}
	struct Att_action act=monster->action[0];
	for(int i=1; i<monster->action_num; i++) { //遍历行动列表，决定执行哪一个行动
		if(monster->action[i].skill[1]>act.skill[1]) {//判断优先级
			if(random()<=monster->action[i].c) {//判断发动概率
				switch(monster->action[i].condition) {
					case 0: {
							act=monster->action[i];
							break;
						}
					case 1: {
							if((t_round-monster->action[i].a)%monster->action[i].b==0) {
								act=monster->action[i];
							}
							break;
						}
					case 2: {
							float c=monster->hp/monster->f_hp*100;
							if(c<monster->action[i].b&&c>monster->action[i].a) {
								act=monster->action[i];
							}
							break;
						}
					case 3: {
							for(int j=0; j<monster_buff_num[actlist[roundnum]-TeamNum]; j++) {
								struct BUFF tbf=monster_buff[actlist[roundnum]-TeamNum][j];
								if(tbf.name==buffs[monster->action[i].a].name&&tbf.gain[0]==buffs[monster->action[i].a].gain[0]) {
									//由于从buff结构体无法获得编号所以通过比较名字和效果
									act=monster->action[i];
									break;
								}
							}
							break;
						}
					case 4: {
							for(int j=0; j<TeamNum; j++) {
								if(players[j].lever>=monster->action[i].a) {
									act=monster->action[i];
								}
							}
							break;
						}
				}
			}
		}
	}//到这里已经选中要执行哪个技能了
	//skill_list[num]
	int skillchoosenum=act.skill[0];
	struct Skill m_skill=skill_list[skillchoosenum];
	SlowDisplay(monster->name,0);
	SlowDisplay("使出了 ",0);
	SlowDisplay(m_skill.name,0);
	printf("!\n");
	//下面要判断对谁使用。
	//治疗类技能只能对自己使用，单体攻击对玩家“相对最肉”的角色使用，复数随机挑选
	switch(m_skill.c) {
		case 1: { //对玩家使用
				if(m_skill.a_num==6) { //全体攻击
					for(int i=0; i<TeamNum; i++) {
						if(players[i].hp<=0) {
							continue;
						}
						defp=index2prop(player->defp);
						harm=monster->attack;
						harm*=m_skill.damage;
						harm-=players[i].defence;
						SlowDisplay(players[i].name,0);
						SlowDisplay("受到了",0);
						COLOR(m_skill.ele);
						printf(" %ld ",harm);
						COLOR(7);
						SlowDisplay("点伤害！\n",0);
						players[i].hp-=harm;
						if(m_skill.status) {
							player_buff[i][player_buff_num[i]++]=buffs[m_skill.status];//给玩家上buff
							SlowDisplay(players[i].name,0);
							SlowDisplay("  被附加了  ",0);
							COLOR(buffs[m_skill.status].n);
							printf("%s",buffs[m_skill.status].name);
							COLOR(7);
							SlowDisplay(" 状态!\n",0);
						}
						if(players[i].hp<0) {
							printf("%s",players[i].name);
							SlowDisplay("倒下了！\n",0);
							player_buff_num[i]=0;
						}
					}
				} else if(m_skill.a_num==1) { //单体攻击
					double r=100000.0;//初始化伤害-生命比率
					int playerchoose=0;
					for(int i=0; i<TeamNum; i++) {//这个for循环选出最肉的一个人
						if(players[i].hp<=0) {
							continue;
						}
						harm=monster->attack;
						harm*=m_skill.damage;
						harm-=players[i].defence;
						double tr=harm/players[i].hp;
						if(tr<r) {
							r=tr;
							playerchoose=i;
						}
					}
					harm=monster->attack;
					harm*=m_skill.damage;
					harm-=players[playerchoose].defence;
					SlowDisplay(players[playerchoose].name,0);
					SlowDisplay("受到了",0);
					COLOR(m_skill.ele);
					printf(" %ld ",harm);
					COLOR(7);
					SlowDisplay("点伤害！\n",0);
					players[playerchoose].hp-=harm;
					if(m_skill.status) {
						player_buff[playerchoose][player_buff_num[playerchoose]++]=buffs[m_skill.status];//给玩家上buff
						SlowDisplay(players[playerchoose].name,0);
						SlowDisplay("  被附加了  ",0);
						COLOR(buffs[m_skill.status].n);
						printf("%s",buffs[m_skill.status].name);
						COLOR(7);
						SlowDisplay(" 状态!\n",0);
					}
					if(players[playerchoose].hp<0) {
						printf("%s",players[playerchoose].name);
						SlowDisplay("倒下了！\n",0);
						player_buff_num[playerchoose]=0;
					}
				} else {//复数攻击
					for(int i=0; i<m_skill.a_num; i++) {
						int n=1;
						int j=srandom(n)%TeamNum;
						while(players[j].hp<=0) {
							j=srandom(++n)%TeamNum;
						}
						harm=monster->attack;
						harm*=m_skill.damage;
						harm-=players[j].defence;
						SlowDisplay(players[j].name,0);
						SlowDisplay("受到了",0);
						COLOR(m_skill.ele);
						printf(" %ld ",harm);
						COLOR(7);
						SlowDisplay("点伤害！\n",0);
						players[j].hp-=harm;
						if(m_skill.status) {
							player_buff[j][player_buff_num[j]++]=buffs[m_skill.status];//给玩家上buff
							SlowDisplay(players[j].name,0);
							SlowDisplay("  被附加了  ",0);
							COLOR(buffs[m_skill.status].n);
							printf("%s",buffs[m_skill.status].name);
							COLOR(7);
							SlowDisplay(" 状态!\n",0);
						}
						if(players[j].hp<0) {
							printf("%s",players[j].name);
							SlowDisplay("倒下了！\n",0);
							player_buff_num[j]=0;
						}
					}
				}
				break;
			}
		case 0: { //对怪物使用
				if(m_skill.damage<0) {
					harm=monster->attack;
					harm*=m_skill.damage;
					monster->hp-=harm;
					SlowDisplay(monster->name,0);
					SlowDisplay(" 受到了 ",0);
					COLOR(m_skill.ele);
					printf("%ld",0-harm);
					COLOR(7);
					SlowDisplay(" 点治疗！",0);
					if(m_skill.status) {
						monster_buff[actlist[roundnum]-TeamNum][monster_buff_num[actlist[roundnum]-TeamNum]++]=buffs[m_skill.status];//给自己上buff
						SlowDisplay(monster->name,0);
						SlowDisplay("  被附加了  ",0);
						COLOR(buffs[m_skill.status].n);
						printf("%s",buffs[m_skill.status].name);
						COLOR(7);
						SlowDisplay(" 状态!\n",0);
					}
				} else if(m_skill.damage==0) {
					if(m_skill.status) {
						monster_buff[actlist[roundnum]-TeamNum][monster_buff_num[actlist[roundnum]-TeamNum]++]=buffs[m_skill.status];//给自己上buff
						SlowDisplay(monster->name,0);
						SlowDisplay("  被附加了  ",0);
						COLOR(buffs[m_skill.status].n);
						printf("%s",buffs[m_skill.status].name);
						COLOR(7);
						SlowDisplay(" 状态!\n",0);
					} else {
						SlowDisplay("\n什么也没发生！\n",0);
					}

				} else {
					harm=monster->attack;
					harm*=m_skill.damage;
					monster->hp-=harm;
					SlowDisplay(monster->name,0);
					SlowDisplay(" 自残了 ",0);
					COLOR(m_skill.ele);
					printf("%ld",harm);
					COLOR(7);
					SlowDisplay(" 点！",0);
					if(m_skill.status) {
						monster_buff[actlist[roundnum]-TeamNum][monster_buff_num[actlist[roundnum]-TeamNum]++]=buffs[m_skill.status];//给自己上buff
						SlowDisplay(monster->name,0);
						SlowDisplay("  被附加了  ",0);
						COLOR(buffs[m_skill.status].n);
						printf("%s",buffs[m_skill.status].name);
						COLOR(7);
						SlowDisplay(" 状态!\n",0);
					}
				}
				break;
			}
			//这里不需要补大括号
	}
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
	wait();
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
					SlowDisplay(index2prop(player->defp)->name,0);
					SlowDisplay("换成了",0);
					SlowDisplay(parr[choosenum]->name,0);
					printf("!\n");
					index2prop(player->defp)->havenum+=1;
					player->defp=parr[choosenum]->n;
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
					SlowDisplay(index2prop(player->attp)->name,0);
					SlowDisplay("换成了",0);
					SlowDisplay(parr[choosenum]->name,0);
					printf("!\n");
					index2prop(player->attp)->havenum+=1;
					player->attp=parr[choosenum]->n;
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


void conversation(struct NPC *npc) {
	check_npc_task(npc);
	struct ConNode *con=&conversations[npc->con];
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
			//----这里发放经验---------------------
			for(int i=0; i<TeamNum; i++) {
				plusarr(players[i].exp,change_task->give_exp);
			}
			for(int i=0; i<TeamNum; i++) {
				leverUP(i);
			}
			plusarr(change_task->give_gold,gold);
			change_task->finish=1;//结束任务
			tasks[tasks[con->check_task].next].start=1;//自动接取后续任务
			con=&conversations[con->finish_next_con[1]];
			
			npc->con=npc->nomal_con;//这个任务结束了，将NPC的对话起点改回去

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
			npcs[tasks[num].npc_s].con=tasks[num].con_s;//更改对话起点
		}
		if(kg==2||kg==3) {//任务已经开启或已经完成，但未提交
			npcs[tasks[num].npc_f].con=tasks[num].con_f;//更改对话起点
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
	relics_num[n]++;
}


int srandom(int n) { //以当前时间（秒为单位）和指定数值为种子生成一个0～99的随机数
	srand((unsigned)time(NULL)+n);
	return rand() % 100;
}

void count(void) { //计算攻击力等等数值，用于更换圣遗物、升级后
	for(int i=0; i<TeamNum; i++) {
		long hp=players[i].lever*players[i].hparr[1]+players[i].hparr[2];
		long hd=players[i].lever*players[i].hdarr[1]+players[i].hdarr[2];
		long def=players[i].lever*players[i].defarr[1]+players[i].defarr[2];
		long att=players[i].lever*players[i].attarr[1]+players[i].attarr[2];
		//计算基础值
		float crit_rate=0;//暴击率
		float crit_damage=0;//暴击伤害倍率
		float damage_rate=0;//攻击力提高率
		float hp_rate=0;
		float def_rate=0;
		float miss=0;//闪避
		float cure=0;//治疗加成
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
		hp *= hp_rate;
		def *= def_rate;
		att *= damage_rate;
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
