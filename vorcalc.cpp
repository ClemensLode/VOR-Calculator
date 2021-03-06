#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <windows.h>
#include <iostream.h>
#include <fstream.h>
#include <time.h>


#define RUNNINGS 10


/*noch einfuegen: mehrere Laeufe hintereinander, ergebnisse+loesungen zusammenspeichern und beste finden und in Datei umgeben
(thema: user friendlyness)

dann:
andere rassen einfuegen

~ Nahkampf*/




// reading binary file

#define RESTART 0
// 0: Initialize all random
// 1: Initialize all 0
// 2: Initialize all with 1 Std Infantry Troup

#define MUTATIONS 100
#define MUTATION_FAKTOR 500

#define MAX_UNITS 25

#define EMPTY 0
#define COMMANDER 1
#define SERGEANT 2
#define STDINFANTRY 3
#define ELITEINFANTRY 4
#define SCOUTINFANTRY 5
#define SPECIALFORCE 6
#define ATDDRONE 7
#define HTDDRONE 8
#define ARES 9
#define SUPPLYDRONE 10
#define WENDIGO 11
#define FENRIS 12
#define ODIN 13
#define LOKI 13

#define SMALL 0
#define MEDIUM 1
#define LARGE 2

//...

#define PITBULL 0
#define ROTTWEILER 1
#define DRACO 2
#define BULLDOG 3
#define SNIPER 4
#define HARBINGER 5
#define PLASMA 6
#define QUAD 7
#define APE 8
#define MASTIFF 9
#define DART 10



#define MAX_PLAYER 200

#define TROUP_TYPES 15
#define WEAPON_TYPES 11

#define REPRODUCTION 20

#define MAX_GENERATIONS 50

unsigned char run,y,rfit;
unsigned short afit;
unsigned short PTS;

struct save
{
	struct Unit
	{
		unsigned char type;
		unsigned char S;//Special Abilities and Armament
	} unit[MAX_UNITS];
	unsigned short fitness;
	unsigned short PTs;
	unsigned char force[TROUP_TYPES];
	unsigned char weaps[TROUP_TYPES][WEAPON_TYPES];
} Save[RUNNINGS];



struct s_Name
{
	char Name[20];
};

const s_Name nameT[TROUP_TYPES]=
{"EMPTY","Commander","Sergeant","Std. Infantry","Elite Infantry","Scout Squad","Special Forces","ADT Drone Squad","HTD Drone","Ares","SUPPLYDRONE","WENDIGO","FENRIS","ODIN","LOKI"};

const s_Name nameTs[TROUP_TYPES]=
{"-","Com","Ser","Std","Elt","Sct","SpF","ADT","HTD","Are","Sup","WEN","FEN","ODI","LOK"};

const s_Name nameW[WEAPON_TYPES]=
{"Pitbull","Rottweiler HMG","Draco","Bulldog","Sniper","Harbinger","Plasma","Quad Missile","Ape","Mastiff","Dart"};

const s_Name nameWs[WEAPON_TYPES]=
{"P","R","D","B","S","H","Pl","Q","A","Ma","Da"};


struct s_EStatistics
{
	unsigned char life,size,armor,body,count;
};
#define ENEMY_TYPES 10
s_EStatistics Estats[ENEMY_TYPES];
unsigned short enemy_count,generation;


unsigned char num;


struct s_TStatistics
{
	unsigned short RC,Cost;
};

const s_TStatistics Tstats[TROUP_TYPES]=
{
	{0,0},
	{6,50},
	{6,38},
	{5,28},
	{6,32},
	{5,22},
	{6,50},
	{6,55},
	{6,125},
	{6,100},
	{0,45}, //supply
	{6,310},
	{6,280},
	{6,175},
	{6,215}   
};

struct s_WStatistics
{
	signed char RC;
	unsigned char POW;
	signed char AM;
	unsigned char USES,DAMAGE,AUTO,AREA,AIMING,Cost;
};

const s_WStatistics Wstats[WEAPON_TYPES]=
{
	{0,4,-1,2,1,0,0,1,0},
	{0,7,-3,1,1,1,0,1,52},
	{-1,8,-2,1,2,0,1,0,65},
	{-1,7,-2,1,1,0,1,0,20},
	{2,7,-5,1,2,0,0,1,55},
	{-1,6,-3,1,1,0,3,0,75}, //harbinger
	{-1,7,0,1,2,0,0,0,65}, //plasma
	{1,8,-4,1,2,0,0,0,70}, //quad
	{-1,5,-1,1,1,1,0,0,0}, //ape
	{0,9,-5,1,2,0,0,0,0}, //mastiff
	{1,6,0,1,1,0,0,0,0} // Dart

};




unsigned char x,i,j,A,B,tempb;
unsigned short tempw;
long n;

class Player
{
public:

	unsigned char force[TROUP_TYPES];
	unsigned char weaps[TROUP_TYPES][WEAPON_TYPES];

	struct Unit
	{
		unsigned char type;
		unsigned char S;//Special Abilities and Armament
	} unit[MAX_UNITS];

	unsigned short fitness;
	unsigned short PTs;
	unsigned char tod,Supply;
	unsigned char target;

	void ChooseTarget()
	{
		for(x=0;x<num-1;x++)
			if(rand()%enemy_count<(Estats[x].count*Estats[x].life))
				target=x;
			else target=num-1;
	}

	unsigned char Allowed() //0 => death, everything else => PTS
	{
		// Zu brutal ...
		for(i=0;i<TROUP_TYPES;i++)
		{
			force[i]=0;
			for(x=0;x<WEAPON_TYPES;x++)
				weaps[i][x]=0;
		}
		PTs=0;
		for(i=0;i<MAX_UNITS;i++)
		if(unit[i].type>0)
		{
			if((unit[i].type==COMMANDER)&&(force[COMMANDER]==1))
			{
				unit[i].type=0;
				unit[i].S=0;
			}
			else
			{
				PTs+=Tstats[unit[i].type].Cost;
				if((unit[i].type<ATDDRONE)||(unit[i].type==ARES)) 
				{
					PTs+=Wstats[unit[i].S].Cost;
					weaps[unit[i].type][unit[i].S]++;
				}
				force[unit[i].type]++;
			}
		}
	

		if(PTs>PTS)
			return(0);
		
		for(i=1;i<WEAPON_TYPES;i++)
		{
			if(weaps[COMMANDER][i]>0)
				return(0);
			if(weaps[SERGEANT][i]>0)
				return(0);
		}

		for(i=1;i<WEAPON_TYPES;i++)
			if((i!=SNIPER)&&(weaps[SCOUTINFANTRY][i]>0))
				return(0);

		if(weaps[SCOUTINFANTRY][SNIPER]>force[SCOUTINFANTRY]/2)
			return(0);
		
		if( (weaps[STDINFANTRY][SNIPER]>0) || (weaps[ELITEINFANTRY][SNIPER]>0))
			return(0);

		for(i=HARBINGER;i<WEAPON_TYPES;i++)
			if( (weaps[STDINFANTRY][i]>0) || (weaps[ELITEINFANTRY][i]>0) || (weaps[SCOUTINFANTRY][i]>0) || (weaps[SPECIALFORCE][i]>0))
				return(0);
		
		

		if( (weaps[STDINFANTRY][BULLDOG]>force[STDINFANTRY]/4) || (weaps[STDINFANTRY][ROTTWEILER]+weaps[STDINFANTRY][DRACO]>force[STDINFANTRY]/4) ||
			(weaps[ELITEINFANTRY][BULLDOG]>force[ELITEINFANTRY]/2) || (weaps[ELITEINFANTRY][ROTTWEILER]+weaps[ELITEINFANTRY][DRACO]>force[ELITEINFANTRY]/4))
			return(0);

		// Vehicles
		if( (force[ARES]+force[WENDIGO]+force[FENRIS]+force[ODIN]+force[LOKI]>(PTS/1000)+1))
			return(0);
		for(i=0;i<HARBINGER;i++)
			if(weaps[ARES][i]>0)
				return(0);
		if(weaps[ARES][APE]>0)
			return(0);

/*		for(i=0;i<TROUP_TYPES;i++)
			for(x=0;x<WEAPON_TYPES;x++)
				if(weaps[i][x]>force[i])
					return(0); //Multiple Weapons*/

		if( ((force[STDINFANTRY]>0)&&(force[STDINFANTRY]<4)) || ((force[ELITEINFANTRY]>0)&&(force[ELITEINFANTRY]<4)) || (force[SCOUTINFANTRY]==1))
			return(0);

		//Sergeants
		if( (force[SERGEANT]<((force[STDINFANTRY]+7)/8)+((force[ELITEINFANTRY]+7)/8)/*+(force[BRAWLERINFANTRY]/8)+(force[ARMOREDASSAULT]/8)*/)||
			(force[SERGEANT]>(force[STDINFANTRY]/4+force[ELITEINFANTRY]/4)/*+(force[BRAWLERINFANTRY]/4)+(force[ARMOREDASSAULT]/6)*/))
			return(0);

		if(force[STDINFANTRY]+force[ELITEINFANTRY]>force[SERGEANT]*8)
			return(0);

		if(force[STDINFANTRY]+force[ELITEINFANTRY]<force[SERGEANT]*4)
			return(0);
		//<> Infantry Troups

		if( ((force[ELITEINFANTRY]+7)/8>force[STDINFANTRY]/4) || /*(force[BRAWLERINFANTRY]>force[STDINFANTRY]) || (force[HELLHOUNDINFANTRY]>force[STDINFANTRY]) ||*/
			((force[SCOUTINFANTRY]+3)/4>force[STDINFANTRY]/4) || /*(force[ARMOREDASSAULT]*2>force[STDINFANTRY]+force[ELITEINFANTRY]+force[BRAWLERINFANTRY]) ||*/
			((force[ATDDRONE]+2)/3>force[SERGEANT]/*Brawler, armored abziehen!*/) || 
			(force[HTDDRONE]*2>force[SERGEANT]) || (force[SUPPLYDRONE]>force[SERGEANT]+force[SCOUTINFANTRY]/2 /*brawler, armored...*/)
			)
			return(0);


//		 Special Forces
		if( 
			(force[SPECIALFORCE]>8) || ((force[SPECIALFORCE]<4)&&(force[SPECIALFORCE]>0)) )
			return(0);

		//Commander
		if(force[COMMANDER]>1)
			return(0);

		Supply=force[SUPPLYDRONE]*3;
//		if(force[SUPPLYDRONE]*4>=force[SPECIALFORCE]+force[STDINFANTRY]+force[ELITEINFANTRY]+force[SCOUTINFANTRY])
//			Supply=1;
//		else Supply=0;

		return(1);
	}


	void Calculate()
	{
		fitness=0;
		for(i=0;i<MAX_UNITS;i++)
		{
			ChooseTarget();
			if((unit[i].type>0)&&(unit[i].type<SPECIALFORCE))
			{
				fitness+=
					
					(
					(Supply==0)*(Tstats[unit[i].type].RC+Wstats[unit[i].S].RC+Estats[target].size-2+Wstats[unit[i].S].AIMING)
				   +(Supply>0)*
			  (10-
			    (10-(Tstats[unit[i].type].RC+Wstats[unit[i].S].RC+Estats[target].size-1+Wstats[unit[i].S].AIMING))*			
				(10-(Tstats[unit[i].type].RC+Wstats[unit[i].S].RC+Estats[target].size-1+Wstats[unit[i].S].AIMING))/10
			   ))
				
					*((Wstats[unit[i].S].DAMAGE)-(Wstats[unit[i].S].DAMAGE>Estats[target].life))
					*((Wstats[unit[i].S].POW+5-Estats[target].body)-(Wstats[unit[i].S].POW-5>Estats[target].body)*(Wstats[unit[i].S].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[unit[i].S].AM))-(Estats[target].armor+Wstats[unit[i].S].AM<0)*(10-Estats[target].armor-(Wstats[unit[i].S].AM))%10)
					*(Wstats[unit[i].S].USES)
					*(Wstats[unit[i].S].AREA+1)
					*(1+Wstats[unit[i].S].AUTO*Estats[target].size)
					//kritischer Treffer
					+
					((Wstats[unit[i].S].DAMAGE)-(Wstats[unit[i].S].DAMAGE>Estats[target].life))
					*((Wstats[unit[i].S].POW+5-Estats[target].body)-(Wstats[unit[i].S].POW-5>Estats[target].body)*(Wstats[unit[i].S].POW+5-Estats[target].body)%10)
					*(Wstats[unit[i].S].USES)
					*(Wstats[unit[i].S].AREA+1)
					*(1+Wstats[unit[i].S].AUTO*Estats[target].size);
					if(Supply>0) Supply--;
			}
			else if(unit[i].type==SPECIALFORCE)
			{
				fitness+=
					(			
					(Supply==0)*(Tstats[unit[i].type].RC+Wstats[unit[i].S].RC+Estats[target].size-1+(unit[i].S==SNIPER))
					+(Supply>1)
					*(10-( (unit[i].S!=SNIPER)*
					 (10-(Tstats[unit[i].type].RC+Wstats[unit[i].S].RC+Estats[target].size))
					*(10-(Tstats[unit[i].type].RC+Wstats[unit[i].S].RC+Estats[target].size))/10)))
					
					*((Wstats[unit[i].S].DAMAGE)-(Wstats[unit[i].S].DAMAGE>Estats[target].life))
					*((Wstats[unit[i].S].POW+5-Estats[target].body)-(Wstats[unit[i].S].POW-5>Estats[target].body)*(Wstats[unit[i].S].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[unit[i].S].AM-1))-(Estats[target].armor+Wstats[unit[i].S].AM<0)*(10-Estats[target].armor-(Wstats[unit[i].S].AM))%10)
					*(Wstats[unit[i].S].USES)
					*(Wstats[unit[i].S].AREA+1)
					*(1+Wstats[unit[i].S].AUTO*Estats[target].size)
					//kritischer Treffer
					+
					((Wstats[unit[i].S].DAMAGE)-(Wstats[unit[i].S].DAMAGE>Estats[target].life))
					*((Wstats[unit[i].S].POW+5-Estats[target].body)-(Wstats[unit[i].S].POW-5>Estats[target].body)*(Wstats[unit[i].S].POW+5-Estats[target].body)%10)
					*(Wstats[unit[i].S].USES)
					*(Wstats[unit[i].S].AREA+1)
					*(1+Wstats[unit[i].S].AUTO*Estats[target].size);
					if((Supply>0)&&(unit[i].S!=SNIPER)) Supply--;
			}			


			else if(unit[i].type==ATDDRONE)
			{
				fitness+=(Tstats[unit[i].type].RC+Wstats[ROTTWEILER].RC+Estats[target].size-2)
					*((Wstats[ROTTWEILER].DAMAGE)-(Wstats[ROTTWEILER].DAMAGE>Estats[target].life))
					*((Wstats[ROTTWEILER].POW+5-Estats[target].body)-(Wstats[ROTTWEILER].POW-5>Estats[target].body)*(Wstats[ROTTWEILER].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[ROTTWEILER].AM))-(Estats[target].armor+Wstats[ROTTWEILER].AM<0)*(10-Estats[target].armor-(Wstats[ROTTWEILER].AM))%10)
					*(Wstats[ROTTWEILER].USES)
					*(Wstats[ROTTWEILER].AREA+1)
					*(1+Wstats[ROTTWEILER].AUTO*Estats[target].size)
					+
					((Wstats[ROTTWEILER].DAMAGE)-(Wstats[ROTTWEILER].DAMAGE>Estats[target].life))
					*((Wstats[ROTTWEILER].POW+5-Estats[target].body)-(Wstats[ROTTWEILER].POW-5>Estats[target].body)*(Wstats[ROTTWEILER].POW+5-Estats[target].body)%10)
					*(Wstats[ROTTWEILER].USES)
					*(Wstats[ROTTWEILER].AREA+1)
					*(1+Wstats[ROTTWEILER].AUTO*Estats[target].size);
			}		
			else if(unit[i].type==HTDDRONE)
			{
				fitness+=(Tstats[unit[i].type].RC+Wstats[ROTTWEILER].RC+Estats[target].size-2)
					*((Wstats[ROTTWEILER].DAMAGE)-(Wstats[ROTTWEILER].DAMAGE>Estats[target].life))
					*((Wstats[ROTTWEILER].POW+5-Estats[target].body)-(Wstats[ROTTWEILER].POW-5>Estats[target].body)*(Wstats[ROTTWEILER].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[ROTTWEILER].AM))-(Estats[target].armor+Wstats[ROTTWEILER].AM<0)*(10-Estats[target].armor-(Wstats[ROTTWEILER].AM))%10)
					*(Wstats[ROTTWEILER].USES)
					*(Wstats[ROTTWEILER].AREA+1)
					*(1+Wstats[ROTTWEILER].AUTO*Estats[target].size)
					+
					((Wstats[ROTTWEILER].DAMAGE)-(Wstats[ROTTWEILER].DAMAGE>Estats[target].life))
					*((Wstats[ROTTWEILER].POW+5-Estats[target].body)-(Wstats[ROTTWEILER].POW-5>Estats[target].body)*(Wstats[ROTTWEILER].POW+5-Estats[target].body)%10)
					*(Wstats[ROTTWEILER].USES)
					*(Wstats[ROTTWEILER].AREA+1)
					*(1+Wstats[ROTTWEILER].AUTO*Estats[target].size);				
				fitness+=(Tstats[unit[i].type].RC+Wstats[DRACO].RC+Estats[target].size-2)
					*((Wstats[DRACO].DAMAGE)-(Wstats[DRACO].DAMAGE>Estats[target].life))
					*((Wstats[DRACO].POW+5-Estats[target].body)-(Wstats[DRACO].POW-5>Estats[target].body)*(Wstats[DRACO].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[DRACO].AM))-(Estats[target].armor+Wstats[DRACO].AM<0)*(10-Estats[target].armor-(Wstats[DRACO].AM))%10)
					*(Wstats[DRACO].USES)
					*(Wstats[DRACO].AREA+1)
					*(1+Wstats[DRACO].AUTO*Estats[target].size)
					+
					((Wstats[DRACO].DAMAGE)-(Wstats[DRACO].DAMAGE>Estats[target].life))
					*((Wstats[DRACO].POW+5-Estats[target].body)-(Wstats[DRACO].POW-5>Estats[target].body)*(Wstats[DRACO].POW+5-Estats[target].body)%10)
					*(Wstats[DRACO].USES)
					*(Wstats[DRACO].AREA+1)
					*(1+Wstats[DRACO].AUTO*Estats[target].size);		
			}
			else if(unit[i].type==ARES)
			{
				fitness+=
					(1-(unit[i].S==HARBINGER)*0.5)
					*(Tstats[unit[i].type].RC+Wstats[APE].RC+Estats[target].size-2)
					*((Wstats[APE].DAMAGE)-(Wstats[APE].DAMAGE>Estats[target].life))
					*((Wstats[APE].POW+5-Estats[target].body)-(Wstats[APE].POW-5>Estats[target].body)*(Wstats[APE].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[APE].AM))-(Estats[target].armor+Wstats[APE].AM<0)*(10-Estats[target].armor-(Wstats[APE].AM))%10)
					*(Wstats[APE].USES)
					*(Wstats[APE].AREA+1)
					*(1+Wstats[APE].AUTO*(Estats[target].size+1))
					+
					(1-(unit[i].S==HARBINGER)*0.5)
					*((Wstats[APE].DAMAGE)-(Wstats[APE].DAMAGE>Estats[target].life))
					*((Wstats[APE].POW+5-Estats[target].body)-(Wstats[APE].POW-5>Estats[target].body)*(Wstats[APE].POW+5-Estats[target].body)%10)
					*(Wstats[APE].USES)
					*(Wstats[APE].AREA+1)
					*(1+Wstats[APE].AUTO*(Estats[target].size+1));				
				fitness+=
					(1-(unit[i].S==HARBINGER)*0.5)
					*(Tstats[unit[i].type].RC+Wstats[unit[i].S].RC+Estats[target].size-2)
					*((Wstats[unit[i].S].DAMAGE)-(Wstats[unit[i].S].DAMAGE>Estats[target].life))
					*((Wstats[unit[i].S].POW+5-Estats[target].body)-(Wstats[unit[i].S].POW-5>Estats[target].body)*(Wstats[unit[i].S].POW+5-Estats[target].body)%10)
					*(
					10*(unit[i].S==PLASMA)+(unit[i].S!=PLASMA)*
					( (10-Estats[target].armor-(Wstats[unit[i].S].AM))-(Estats[target].armor+Wstats[unit[i].S].AM<0)*(10-Estats[target].armor-(Wstats[unit[i].S].AM))%10))
					*(Wstats[unit[i].S].USES)
					*(Wstats[unit[i].S].AREA+1)
					*(1+Wstats[unit[i].S].AUTO*Estats[target].size)
					+
					(1-(unit[i].S==HARBINGER)*0.5)
					*((Wstats[unit[i].S].DAMAGE)-(Wstats[unit[i].S].DAMAGE>Estats[target].life))
					*((Wstats[unit[i].S].POW+5-Estats[target].body)-(Wstats[unit[i].S].POW-5>Estats[target].body)*(Wstats[unit[i].S].POW+5-Estats[target].body)%10)
					*(Wstats[unit[i].S].USES)
					*(Wstats[unit[i].S].AREA+1)
					*(1+Wstats[unit[i].S].AUTO*Estats[target].size);		
			}
			else if(unit[i].type==WENDIGO)
			{
					fitness+=
					2*(Tstats[WENDIGO].RC+Wstats[APE].RC+Estats[target].size-2)
					*((Wstats[APE].DAMAGE)-(Wstats[APE].DAMAGE>Estats[target].life))
					*((Wstats[APE].POW+5-Estats[target].body)-(Wstats[APE].POW-5>Estats[target].body)*(Wstats[APE].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[APE].AM))-(Estats[target].armor+Wstats[APE].AM<0)*(10-Estats[target].armor-(Wstats[APE].AM))%10)
					*(Wstats[APE].USES)
					*(Wstats[APE].AREA+1)
					*(1+Wstats[APE].AUTO*(Estats[target].size+1))
					+
					2*((Wstats[APE].DAMAGE)-(Wstats[APE].DAMAGE>Estats[target].life))
					*((Wstats[APE].POW+5-Estats[target].body)-(Wstats[APE].POW-5>Estats[target].body)*(Wstats[APE].POW+5-Estats[target].body)%10)
					*(Wstats[APE].USES)
					*(Wstats[APE].AREA+1)
					*(1+Wstats[APE].AUTO*(Estats[target].size+1));				
					fitness+= //plasma
					(Tstats[WENDIGO].RC+Wstats[PLASMA].RC+Estats[target].size-2)
					*((Wstats[PLASMA].DAMAGE)-(Wstats[PLASMA].DAMAGE>Estats[target].life))
					*((Wstats[PLASMA].POW+5-Estats[target].body)-(Wstats[PLASMA].POW-5>Estats[target].body)*(Wstats[PLASMA].POW+5-Estats[target].body)%10)					*(Wstats[PLASMA].USES)
					*(Wstats[PLASMA].AREA+1)
					+
					((Wstats[PLASMA].DAMAGE)-(Wstats[PLASMA].DAMAGE>Estats[target].life))
					*((Wstats[PLASMA].POW+5-Estats[target].body)-(Wstats[PLASMA].POW-5>Estats[target].body)*(Wstats[PLASMA].POW+5-Estats[target].body)%10)
					*(Wstats[PLASMA].USES)
					*(Wstats[PLASMA].AREA+1);				
			}
			else if(unit[i].type==FENRIS)
			{
					fitness+=
					(Tstats[FENRIS].RC+Wstats[APE].RC+Estats[target].size-2)
					*((Wstats[APE].DAMAGE)-(Wstats[APE].DAMAGE>Estats[target].life))
					*((Wstats[APE].POW+5-Estats[target].body)-(Wstats[APE].POW-5>Estats[target].body)*(Wstats[APE].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[APE].AM))-(Estats[target].armor+Wstats[APE].AM<0)*(10-Estats[target].armor-(Wstats[APE].AM))%10)
					*(Wstats[APE].USES)
					*(Wstats[APE].AREA+1)
					*(1+Wstats[APE].AUTO*(Estats[target].size+1))
					+
					((Wstats[APE].DAMAGE)-(Wstats[APE].DAMAGE>Estats[target].life))
					*((Wstats[APE].POW+5-Estats[target].body)-(Wstats[APE].POW-5>Estats[target].body)*(Wstats[APE].POW+5-Estats[target].body)%10)
					*(Wstats[APE].USES)
					*(Wstats[APE].AREA+1)
					*(1+Wstats[APE].AUTO*(Estats[target].size+1));				

					fitness+=
					(Tstats[FENRIS].RC+Wstats[QUAD].RC+Estats[target].size-2)
					*((Wstats[QUAD].DAMAGE)-(Wstats[QUAD].DAMAGE>Estats[target].life))
					*((Wstats[QUAD].POW+5-Estats[target].body)-(Wstats[QUAD].POW-5>Estats[target].body)*(Wstats[QUAD].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[QUAD].AM))-(Estats[target].armor+Wstats[QUAD].AM<0)*(10-Estats[target].armor-(Wstats[QUAD].AM))%10)
					*(Wstats[QUAD].USES)
					*(Wstats[QUAD].AREA+1)
					*(1+Wstats[QUAD].AUTO*Estats[target].size)
					+
					((Wstats[QUAD].DAMAGE)-(Wstats[QUAD].DAMAGE>Estats[target].life))
					*((Wstats[QUAD].POW+5-Estats[target].body)-(Wstats[QUAD].POW-5>Estats[target].body)*(Wstats[QUAD].POW+5-Estats[target].body)%10)
					*(Wstats[QUAD].USES)
					*(Wstats[QUAD].AREA+1)
					*(1+Wstats[QUAD].AUTO*Estats[target].size);				

					fitness+=
					(Tstats[FENRIS].RC+Wstats[MASTIFF].RC+Estats[target].size-2)
					*((Wstats[MASTIFF].DAMAGE)-(Wstats[MASTIFF].DAMAGE>Estats[target].life))
					*((Wstats[MASTIFF].POW+5-Estats[target].body)-(Wstats[MASTIFF].POW-5>Estats[target].body)*(Wstats[MASTIFF].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[MASTIFF].AM))-(Estats[target].armor+Wstats[MASTIFF].AM<0)*(10-Estats[target].armor-(Wstats[MASTIFF].AM))%10)
					*(Wstats[MASTIFF].USES)
					*(Wstats[MASTIFF].AREA+1)
					*(1+Wstats[MASTIFF].AUTO*Estats[target].size)
					+
					((Wstats[MASTIFF].DAMAGE)-(Wstats[MASTIFF].DAMAGE>Estats[target].life))
					*((Wstats[MASTIFF].POW+5-Estats[target].body)-(Wstats[MASTIFF].POW-5>Estats[target].body)*(Wstats[MASTIFF].POW+5-Estats[target].body)%10)
					*(Wstats[MASTIFF].USES)
					*(Wstats[MASTIFF].AREA+1)
					*(1+Wstats[MASTIFF].AUTO*Estats[target].size);				

			}
			else if(unit[i].type==ODIN)
			{
					fitness+=
					2*(Tstats[ODIN].RC+Wstats[QUAD].RC+Estats[target].size-2)
					*((Wstats[QUAD].DAMAGE)-(Wstats[QUAD].DAMAGE>Estats[target].life))
					*((Wstats[QUAD].POW+5-Estats[target].body)-(Wstats[QUAD].POW-5>Estats[target].body)*(Wstats[QUAD].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[QUAD].AM))-(Estats[target].armor+Wstats[QUAD].AM<0)*(10-Estats[target].armor-(Wstats[QUAD].AM))%10)
					*(Wstats[QUAD].USES)
					*(Wstats[QUAD].AREA+1)
					*(1+Wstats[QUAD].AUTO*Estats[target].size)
					+
					2*((Wstats[QUAD].DAMAGE)-(Wstats[QUAD].DAMAGE>Estats[target].life))
					*((Wstats[QUAD].POW+5-Estats[target].body)-(Wstats[QUAD].POW-5>Estats[target].body)*(Wstats[QUAD].POW+5-Estats[target].body)%10)
					*(Wstats[QUAD].USES)
					*(Wstats[QUAD].AREA+1)
					*(1+Wstats[QUAD].AUTO*Estats[target].size);				
			}
			else if(unit[i].type==LOKI)
			{
					fitness+=
					2*(Tstats[LOKI].RC+Wstats[DART].RC+Estats[target].size-2)
					*((Wstats[DART].DAMAGE)-(Wstats[DART].DAMAGE>Estats[target].life))
					*((Wstats[DART].POW+5-Estats[target].body)-(Wstats[DART].POW-5>Estats[target].body)*(Wstats[DART].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[DART].AM))-(Estats[target].armor+Wstats[DART].AM<0)*(10-Estats[target].armor-(Wstats[DART].AM))%10)
					*(Wstats[DART].USES)
					*(Wstats[DART].AREA+1)
					*(1+Wstats[DART].AUTO*Estats[target].size)
					+
					2*((Wstats[DART].DAMAGE)-(Wstats[DART].DAMAGE>Estats[target].life))
					*((Wstats[DART].POW+5-Estats[target].body)-(Wstats[DART].POW-5>Estats[target].body)*(Wstats[DART].POW+5-Estats[target].body)%10)
					*(Wstats[DART].USES)
					*(Wstats[DART].AREA+1)
					*(1+Wstats[DART].AUTO*Estats[target].size);				

					fitness+=
					(Tstats[LOKI].RC+Wstats[ROTTWEILER].RC+Estats[target].size-2)
					*((Wstats[ROTTWEILER].DAMAGE)-(Wstats[ROTTWEILER].DAMAGE>Estats[target].life))
					*((Wstats[ROTTWEILER].POW+5-Estats[target].body)-(Wstats[ROTTWEILER].POW-5>Estats[target].body)*(Wstats[ROTTWEILER].POW+5-Estats[target].body)%10)
					*( (10-Estats[target].armor-(Wstats[ROTTWEILER].AM))-(Estats[target].armor+Wstats[ROTTWEILER].AM<0)*(10-Estats[target].armor-(Wstats[ROTTWEILER].AM))%10)
					*(Wstats[ROTTWEILER].USES)
					*(Wstats[ROTTWEILER].AREA+1)
					*(1+Wstats[ROTTWEILER].AUTO*Estats[target].size)
					+
					2*((Wstats[ROTTWEILER].DAMAGE)-(Wstats[ROTTWEILER].DAMAGE>Estats[target].life))
					*((Wstats[ROTTWEILER].POW+5-Estats[target].body)-(Wstats[ROTTWEILER].POW-5>Estats[target].body)*(Wstats[ROTTWEILER].POW+5-Estats[target].body)%10)
					*(Wstats[ROTTWEILER].USES)
					*(Wstats[ROTTWEILER].AREA+1)
					*(1+Wstats[ROTTWEILER].AUTO*Estats[target].size);				
					
			}

			}
		}


unsigned char ChooseGuy()
{
	if(rand()%(MAX_UNITS*2)==0) return(COMMANDER);
else 
	if(rand()%MAX_UNITS==0) return(SERGEANT);
else 
	if(rand()%(MAX_UNITS/3)==0) return(STDINFANTRY);
else 
	if(rand()%(MAX_UNITS/2)==0) return(ELITEINFANTRY);
else 
	if(rand()%(MAX_UNITS/2)==0) return(SCOUTINFANTRY);
else 
	if(rand()%(MAX_UNITS/3)==0) return(SPECIALFORCE);
else 
	if(rand()%(MAX_UNITS)==0) return(ATDDRONE);
else 
	if(rand()%(MAX_UNITS*2)==0) return(HTDDRONE);
else 
	if(rand()%(MAX_UNITS)==0) return(SUPPLYDRONE);
else 
	if(rand()%(MAX_UNITS*2)==0) return(ARES);
else 
	if(rand()%(MAX_UNITS*2)==0) return(WENDIGO);
else
	if(rand()%(MAX_UNITS*2)==0) return(FENRIS);
else 
	if(rand()%(MAX_UNITS*2)==0) return(ODIN);
else 
	if(rand()%(MAX_UNITS*2)==0) return(LOKI);
else 
	return(EMPTY);
}



void ChooseWeapon(unsigned char w)
{
	switch(unit[w].type)
	{
		case EMPTY:unit[w].S=0;break;
		case COMMANDER:
		case SERGEANT:unit[w].S=PITBULL;break;
		case STDINFANTRY:if(rand()%8==0) 
						 unit[w].S=rand()%3+1;
					 else unit[w].S=0;break;
		case ELITEINFANTRY:if(rand()%8==0) 
							 unit[w].S=rand()%2+1;
						 else 
						    if(rand()%4==0)
							 unit[w].S=3;
						else
							unit[w].S=0;break;
		case SCOUTINFANTRY:if(rand()%4==0)
						   unit[w].S=4;
							else
								unit[w].S=0;break;
		case SPECIALFORCE:unit[w].S=rand()%5;break;
		case HTDDRONE:unit[w].S=0;break;
		case ATDDRONE:unit[w].S=0;break;
		case ARES:unit[w].S=rand()%3+5;break;
		default:unit[w].S=0;break;
	}
};

void Mutate()
{
for(j=0;j<MUTATIONS;j++)
	{
		if((rand()%MUTATION_FAKTOR==0))
		{
			x=rand()%MAX_UNITS;
			unit[x].type=ChooseGuy();
			ChooseWeapon(x);
		if((rand()%MUTATION_FAKTOR==0))
			ChooseWeapon(rand()%MAX_UNITS);
		}
	}
}

void Init()
{
	fitness=0;
	if(RESTART==2)
	{
		unit[0].type=STDINFANTRY;
		unit[0].S=PITBULL;
		unit[1].type=STDINFANTRY;
		unit[1].S=PITBULL;
		unit[2].type=STDINFANTRY;
		unit[2].S=PITBULL;
		unit[3].type=STDINFANTRY;
		unit[3].S=BULLDOG;
		unit[4].type=SERGEANT;
		unit[4].S=PITBULL;
	}
	else
	for(j=0;j<MAX_UNITS;j++)
	{
		if(RESTART==1)
			unit[j].type=0;
			else 
			unit[j].type=ChooseGuy();
		
		ChooseWeapon(j);
	}
}

} player[MAX_PLAYER];




int main(int argc, char* argv[])
{
  const char * filename = "enemy.army";
  unsigned char pos,calc;
  unsigned char * buffer;
  long size;
  generation=0;
  ifstream file (filename, ios::in|ios::binary|ios::ate);
  size = file.tellg();
  file.seekg (0, ios::beg);
  buffer = new unsigned char [size];
  file.read (buffer, size);
  file.close();


  //# LIFE SIZE ARMOR BODY NUMBER
	n=1;
	num=0;
	pos=0;
	PTS=(buffer[0]-48)*250+250;
	while(n<size)
	{
		if((buffer[n]>=48)&&(buffer[n]<58))
		{
			if(pos>4)
			{
				pos=0;
				num++;
			}
			switch(pos)
			{
				case 0:Estats[num].life=buffer[n]-48;pos++;break;
				case 1:Estats[num].size=buffer[n]-48;pos++;break;
				case 2:Estats[num].armor=buffer[n]-48;pos++;break;
				case 3:Estats[num].body=buffer[n]-48;pos++;break;
				case 4:Estats[num].count=buffer[n]-48;pos++;break;
			}
		}
		n++;
	}
	for(i=0;i<num;i++)
		enemy_count+=Estats[i].life*Estats[i].count;
 

	srand(time(NULL));
	 HANDLE hStdOut=GetStdHandle(STD_OUTPUT_HANDLE);
	 COORD coord={0,0};
	COORD coord2={25,22};
	SetConsoleCursorPosition(hStdOut,coord2);
	printf("This is Freeware. Brought to you by clawsoftware.de");

	for(i=0;i<MAX_PLAYER;i++)
		player[i].Init();
	run=0;

	while( (kbhit()==0) && (run<RUNNINGS))
	{
		for(i=0;i<MAX_PLAYER;i++)
			player[i].Mutate();
		
		for(j=0;j<MAX_PLAYER;j++)
		{
			if(player[j].Allowed()==0)
			{
				
				player[j].PTs=0;
				player[j].fitness=0;
				player[j].tod=1;
			}
			else
			{
				player[j].tod=0;
				player[j].Calculate();
			};
		};
		for(i=0;i<MAX_PLAYER-1;i++)
			for(j=0;j<MAX_PLAYER-i+1;j++)
				if(player[j].fitness<player[j+1].fitness)
				{
					tempw=player[j].fitness;
					player[j].fitness=player[j+1].fitness;
					player[j+1].fitness=tempw;

					tempw=player[j].PTs;
					player[j].PTs=player[j+1].PTs;
					player[j+1].PTs=tempw;

					tempb=player[j].tod;
					player[j].tod=player[j+1].tod;
					player[j+1].tod=tempb;

					for(x=0;x<MAX_UNITS;x++)
					{
						tempb=player[j].unit[x].type;
						player[j].unit[x].type=player[j+1].unit[x].type;
						player[j+1].unit[x].type=tempb;

						tempb=player[j].unit[x].S;
						player[j].unit[x].S=player[j+1].unit[x].S;
						player[j+1].unit[x].S=tempb;
					}
				}	
				A=0;
				while((player[A].tod>0)&&(A<MAX_PLAYER-1)) A++;
				B=A;
				while((player[B].tod>0)&&(B<MAX_PLAYER-1)) B++;
				if(player[B].fitness==0) B=A;

				if((player[A].fitness>0)&&(player[B].fitness>0))
				for(i=0;i<REPRODUCTION;i++)
					for(x=0;x<MAX_UNITS;x++)
					{
						player[MAX_PLAYER-1-i*2].unit[x].type=player[A].unit[x].type;
						player[MAX_PLAYER-1-i*2].unit[x].S=player[A].unit[x].S;
						player[MAX_PLAYER-1-i*2].tod=0;

						player[MAX_PLAYER-i*2].unit[x].type=player[B].unit[x].type;
						player[MAX_PLAYER-i*2].unit[x].S=player[B].unit[x].S;
						player[MAX_PLAYER-i*2].tod=0;
					}
		
		if(player[0].fitness>0)
		{
			generation++;
			rfit++;
		}

		SetConsoleCursorPosition(hStdOut,coord);

			calc++;
			if(calc>2)
				calc=1;
		printf("Calculating ... %c\n",calc);
		printf("%i runs remaining . . .  \n",10-run);
		printf("Some Debug Output:\n");
		printf("Fit: %i, PTs: %i    Generation: %i               \n",player[0].fitness,player[0].PTs,generation);

		for(i=1;i<TROUP_TYPES;i++)
			printf("%i x %s            \n",player[0].force[i],nameT[i].Name);
		
		
		if(player[0].fitness>afit)
		{
			afit=player[0].fitness;
			rfit=0;
		}
				
		
		if(rfit>MAX_GENERATIONS)
		{
			afit=0;
			rfit=0;
			generation=0;
			Save[run].fitness=player[0].fitness;
			Save[run].PTs=player[0].PTs;
			for(x=0;x<MAX_UNITS;x++)
			{
				Save[run].unit[x].type=player[0].unit[x].type;
				Save[run].unit[x].S=player[0].unit[x].S;
			}
			for(i=0;i<TROUP_TYPES;i++)
			{
				Save[run].force[i]=player[0].force[i];
				for(j=0;j<WEAPON_TYPES;j++)
					Save[run].weaps[i][j]=player[0].weaps[i][j];
			}
			run++;
			for(i=0;i<MAX_PLAYER;i++)
				player[i].Init();
		}

}


for(i=0;i<run;i++)
	for(j=0;j<run-i+2;j++)
		if(Save[j].fitness<Save[j+1].fitness)
		{
			tempw=Save[j].fitness;
			Save[j].fitness=Save[j+1].fitness;
			Save[j+1].fitness=tempw;
			tempw=Save[j].PTs;
			Save[j].PTs=Save[j+1].PTs;
			Save[j+1].PTs=tempw;
			for(x=0;x<MAX_UNITS;x++)
			{
				tempb=Save[j].unit[x].type;
				Save[j].unit[x].type=Save[j+1].unit[x].type;
				Save[j+1].unit[x].type=tempb;
				tempb=Save[j].unit[x].S;
				Save[j].unit[x].S=Save[j+1].unit[x].S;
				Save[j+1].unit[x].S=tempb;
			}
			for(x=0;x<TROUP_TYPES;x++)
			{
				tempb=Save[j].force[x];
				Save[j].force[x]=Save[j+1].force[x];
				Save[j+1].force[x]=tempb;
				for(y=0;y<WEAPON_TYPES;y++)
				{
					tempb=Save[j].weaps[x][y];
					Save[j].weaps[x][y]=Save[j+1].weaps[x][y];
					Save[j+1].weaps[x][y]=tempb;
				}
			}
		}			

SetConsoleCursorPosition(hStdOut,coord);

	printf("Calculating completed.														 \n",calc);
	printf("Final fitness: %i (%i PTs used)                  \n",Save[0].fitness,Save[0].PTs);
	printf("  UNION Counter - Force:						            				 \n");
	
for(i=1;i<TROUP_TYPES;i++)
	if(Save[0].force[i]>0)
	{
		printf("%i x %s (%i pts)                              \n",Save[0].force[i],nameT[i].Name,Save[0].force[i]*Tstats[i].Cost);
		if((i<ATDDRONE)||(i==ARES))
		{
			for(j=0;j<WEAPON_TYPES;j++)
				if(Save[0].weaps[i][j]>0)
					printf("  o %i x %s (+%i pts)",Save[0].weaps[i][j],nameW[j].Name,Save[0].weaps[i][j]*Wstats[j].Cost);
			printf("\n");
		}  
	}

	printf("																			 \n");
	delete[] buffer;
	x=getch();
	x=getch();
	return 0;
}

