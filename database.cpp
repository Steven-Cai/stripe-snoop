#include <string.h>
#include "database.h"

// ================================================================= 1 Field

//Checks for Barnes and Noble Reader Advantage Card
//500XXXXXXX
cardType bnreader(track2 t2)
{
	cardType card;

	//has 10 characters
	if(strlen(t2.fields[0])!=10) return card;;
	//has 500 constant
	if(strncmp(t2.fields[0],"500",3)!=0) return card;;
	//WE ARE GOOD!
	card.setName("Barnes and Noble Reader's Advantage Card");
	card.add("Membership Number",formatter("XXX-XXXX-XXX",t2.fields[0]));

	return card;
}

//Radisson Gold Rewards Card
//6015XXXXXXXXXXXX 16 digits
cardType radisson(track2 t2)
{

	cardType card;

	//1st is 16
	if(strlen(t2.fields[0])!=16) return card;
	//starts with a 6015
	if(strncmp(t2.fields[0],"6015",4)!=0) return card;

	//WE ARE GOOD!
	card.setName("Radisson Gold Rewards Card");
	card.add("Membership Number",formatter("XXXX XXXX XXXX XXXX",t2.fields[0]));

	return card;
}

// ================================================================= 2 Fields

//Checks for Visa Credit Card
//4xxxxxxxxxxxxxx@=YYMM101xxxxxxxx 16 digits
//4xxxxxxxxxxx@=YYMM101xxxxxxxx 13 digits
cardType visa(track2 t2)
{
	char * tmp;
	FILE * fin;
	char tmp2[80];
	cardType card;
	bool flag;
	memset(tmp2,0,80);

	//is 16 or 13 characters
		if(strlen(t2.fields[0]) !=16 && strlen(t2.fields[0])!=13) return card;
	//passes mod10
	if(!mod10check(t2.fields[0])) return card;
	//has 101 constant
	tmp=t2.fields[1];
	if(strncmp(&tmp[4],"101",3)!=0) return card;
	//starts with a 4
	if(t2.fields[0][0]!='4') return card;
	//has a valid month
	if(!isMonth(&t2.fields[1][2])) return card;

	//WE ARE GOOD!
	card.setName("Visa Credit Card");
	card.add("Account Number",formatter("XXXX XXXX XXXX XXXX",t2.fields[0]));
	//Get the expiration month
	strncpy(tmp2,&tmp[2],2); //grab month
	strcpy(tmp2, monthName(atoi(tmp2)));
	//get the year
	sprintf(tmp2,"%s \'%c%c",tmp2, tmp[0], tmp[1]);
	card.add("Expires",tmp2);
	memset(tmp2,0,80);
	//get E-pin
	strncpy(tmp2,&tmp[8],7);
	card.add("Encrypted PIN",tmp2);
	/*get issuing bank!*/
	if( (fin=fopen(VISADATABASE,"r")) == NULL) {
		printf("Error: visa(): Cannot open \"%s\"\n",VISADATABASE);
	} else {
		tmp = new char[80];
		memset(tmp,0,80);
		memset(tmp2,0,80);
		flag = false;
		while(!feof(fin))
		{
			fgets(tmp2,80,fin);
			if(strlen(tmp2)>6) {
				if( strncmp(t2.fields[0],tmp2,4) == 0) {
					strcpy(tmp, &tmp2[5]);
					flag = true;
					break;
				}
			}
		}
		if(flag)
			card.add("Issuing Bank", &tmp2[5]);
		else {
			card.add("Issuing Bank", "Unknown");
			card.addUnknowns("Issuing Bank Prefix is unknown");
		}
		fclose(fin);
		delete [] tmp;
	}

	return card;
}

//Checks for Mastercard Credit Card
//5xxxxxxxxxxxxxx@=YYMM101xxxxxxxx 16 digits
cardType mastercard(track2 t2)
{
	char * tmp;
	FILE * fin;
	char tmp2[80];
	cardType card;
	bool flag;
	memset(tmp2,0,80);

	//is 16 characters
		if(strlen(t2.fields[0]) !=16) return card;
	//passes mod10
	if(!mod10check(t2.fields[0])) return card;
	//has 101 constant
	tmp=t2.fields[1];
	if(strncmp(&tmp[4],"101",3)!=0) return card;
	//starts with a 51, 52, 53, 54, or 55
	if(t2.fields[0][0]!='5' && 
		(t2.fields[0][1] != '1'
		|| t2.fields[0][1] != '2'
		|| t2.fields[0][1] != '3'
		|| t2.fields[0][1] != '4'
		|| t2.fields[0][1] != '5'))
		 return card;
	//has a valid month
	if(!isMonth(&t2.fields[1][2])) return card;

	//WE ARE GOOD!
	card.setName("Mastercard Credit Card");
	card.add("Account Number",formatter("XXXX XXXX XXXX XXXX",t2.fields[0]));
	//Get the expiration month
	strncpy(tmp2,&tmp[2],2); //grab month
	strcpy(tmp2, monthName(atoi(tmp2)));
	//get the year
	sprintf(tmp2,"%s \'%c%c",tmp2, tmp[0], tmp[1]);
	card.add("Expires",tmp2);
	memset(tmp2,0,80);
	/*get issuing bank!*/
	if( (fin=fopen(MASTERCARDDATABASE,"r")) == NULL) {
		printf("Error: mastercard(): Cannot open \"%s\"\n",MASTERCARDDATABASE);
	} else {
		tmp = new char[80];
		memset(tmp,0,80);
		memset(tmp2,0,80);
		flag = false;
		while(!feof(fin))
		{
			fgets(tmp2,80,fin);
			if(strlen(tmp2)>6) {
				if( strncmp(t2.fields[0],tmp2,4) == 0) {
					strcpy(tmp, &tmp2[5]);
					flag = true;
					break;
				}
			}
		}
		if(flag)
			card.add("Issuing Bank", &tmp2[5]);
		else {
			card.add("Issuing Bank", "Unknown");
			card.addUnknowns("Issuing Bank Prefix is unknown");
		}
		fclose(fin);
		delete [] tmp;
	}

	card.addUnknowns("Encrypted PIN existence or location is unknown");

	return card;
}

//American Express Credit Card
//34xxxxxxxxxxxx@=YYMM101xxxxxxxx BLUE 15 digits, starts 34 or 37
//34xxxxxxxxxxxx@=YYMMxxxxxxxx regular 15 digits, starts 34 or 37
cardType amex(track2 t2)
{
	char tmp2[80];
	cardType card;
	memset(tmp2,0,80);
	bool flag=true;

	//1st is 15 characters
	if(strlen(t2.fields[0]) !=15) return card;
	//2nd has >=12 characters
	if(strlen(t2.fields[1]) <12) return card;
	//passes mod10
	if(!mod10check(t2.fields[0])) return card;
	//has 34 or 37 prefix!
	if(strncmp(t2.fields[0],"34",2)!=0 &&
	 strncmp(t2.fields[0],"37",2)!=0) return card;
	//has a valid month
	if(!isMonth(&t2.fields[1][2])) return card;

	//WE ARE GOOD!

	if(strncmp(&t2.fields[1][4],"101",3)==0) {
		card.setName("American Express Blue Card");
		flag=false;
	} else
		card.setName("American Express Credit Card");

	card.add("Account Number",formatter("XXXX XXXXXX XXXXX",t2.fields[0]));
	//Get the expiration month
	strncpy(tmp2,&t2.fields[1][2],2); //grab month
	strcpy(tmp2, monthName(atoi(tmp2)));
	//get the year
	sprintf(tmp2,"%s \'%c%c",tmp2, t2.fields[1][0], t2.fields[1][1]);
	card.add("Expires",tmp2);

	if(flag) card.addUnknowns("Card Type (Gold card, Green card)");

	return card;
}

//Checks for Discover Credit Card
//5xxxxxxxxxxxxxx@=YYMM101xxxxxxxx 16 digits
cardType discovercard(track2 t2)
{
	char tmp2[80];
	cardType card;
	memset(tmp2,0,80);

	//is 16 characters
		if(strlen(t2.fields[0]) !=16) return card;
	//passes mod10
	if(!mod10check(t2.fields[0])) return card;
	//starts with a 6011
	if(t2.fields[0][0]!='6'
		|| t2.fields[0][1] != '0'
		|| t2.fields[0][2] != '1'
		|| t2.fields[0][3] != '1')
		 return card;

	//has a valid month
	if(!isMonth(&t2.fields[1][2])) return card;

	//WE ARE GOOD!
	card.setName("Discover Credit Card");
	card.add("Account Number",formatter("XXXX XXXX XXXX XXXX",t2.fields[0]));
	//Get the expiration month
	strncpy(tmp2,(char *)&(t2.fields[1][2]),2); //grab month
	strcpy(tmp2, monthName(atoi(tmp2)));
	//get the year
	sprintf(tmp2,"%s \'%c%c",tmp2, t2.fields[1][0], t2.fields[1][1]);
	card.add("Expires",tmp2);
	memset(tmp2,0,80);
	/*get issuing bank!*/
	card.add("Issuing Bank", "Discover");
	card.addUnknowns("Encrypted PIN existence or location is unknown");

	return card;
}
//Checks for AAA card
//4xxxxxxxxxxxxxx@=YYMM101xxxxxxxxx
cardType AAA(track2 t2)
{
	char tmp2[80];
	cardType card;
	memset(tmp2,0,80);

	//1st is 16 characters
	if(strlen(t2.fields[0])!=16) return card;;
	//snd is 20 characters
	if(strlen(t2.fields[1])!=20) return card;;
	//first character is a 4
	if(strncmp(t2.fields[0],"4",1)!=0) return card;;
	//passes mod10
	if(!mod10check(t2.fields[0])) return card;
	//has 101 constant
	if(strncmp(&t2.fields[1][4],"101",3)!=0) return card;
	//check for "4976" at end of field 2
	if(strncmp(&t2.fields[1][16],"4976",4)!=0) return card;
	//has a valid month
	if(!isMonth(&t2.fields[1][2])) return card;

	//WE ARE GOOD!
	card.setName("American Automobile Association Membership Card");
	card.add("Account Number",formatter("XXX XXX XXXXXXXXX",t2.fields[0]));
	//Get the expiration month*/
	strncpy(tmp2,&t2.fields[1][2],2); //grab month
	strcpy(tmp2, monthName(atoi(tmp2)));
	//get the year
	sprintf(tmp2,"%s \'%c%c",tmp2, t2.fields[1][0], t2.fields[1][1]);
	card.add("Expires",tmp2);

	card.addUnknowns("State it was issued in, Level of Membership?, Member Since");

	return card;
}

//Checks for Barnes and Noble Gift card
//5045xxxxxxxxxxx=xxxx...
cardType bngift(track2 t2)
{
	cardType card;

	//1st is 15 characters
	if(strlen(t2.fields[0])!=15) return card;;
	//passes mod10
	if(!mod10check(t2.fields[0])) return card;
	//has 5045 constant
	if(strncmp(t2.fields[0],"5045",4)!=0) return card;;

	//WE ARE GOOD!
	card.setName("Barnes and Noble Giftcard");
	card.add("Account Number",t2.fields[0]);

	card.addUnknowns("Store Number or State it was issued in, Original Value");
	return card;
}

//Checks for Kroger card
//603xxxxxxxxxxxx=xxx sometimes =xxxx
cardType kroger(track2 t2)
{
	cardType card;

	//1st is 15 characters
	if(strlen(t2.fields[0])!=15) return card;;
	//snd is at least 3 characters
	if(strlen(t2.fields[1])<3) return card;;
	//has 603 constant
	if(strncmp(t2.fields[0],"603",3)!=0) return card;;

	//WE ARE GOOD!
	card.setName("Kroger Plus Shopping Card");
	//UNKNOWN
	card.add("Account Number",t2.fields[0]);
	card.addUnknowns("Store Number or State it was issued in");
	return card;
}

//Generic ATM Card
//xxxxxxxxxxxxxx@=YYMM101xxxxxxxx 16 digits
cardType atm(track2 t2)
{
	char tmp2[80];
	cardType card;
	memset(tmp2,0,80);

	//is 16 characters
	if(strlen(t2.fields[0]) !=16) return card;
	//passes mod10
	if(!mod10check(t2.fields[0])) return card;
	//has 120 constant
	if(strncmp(&t2.fields[1][4],"120",3)!=0) return card;
	//has a valid month
	if(!isMonth(&t2.fields[1][2])) return card;

	//WE ARE GOOD!
	card.setName("Generic ATM/Banking Card");
	card.add("Account Number",formatter("XXXX XXXX XXXX XXXX",t2.fields[0]));
	//Get the expiration month
	strncpy(tmp2,&t2.fields[1][2],2); //grab month
	strcpy(tmp2, monthName(atoi(tmp2)));
	//get the year
	sprintf(tmp2,"%s \'%c%c",tmp2, t2.fields[1][0], t2.fields[1][1]);
	card.add("Expires",tmp2);
	memset(tmp2,0,80);
	//get E-pin
	strncpy(tmp2,&t2.fields[1][8],7);
	card.add("Encrypted PIN",tmp2);

	return card;
}

//BP Gas Card
//7211xxxxxxxxxx=YYMMxxxxxxxxxxxx 14 digits
cardType bpgas(track2 t2)
{
	char tmp2[80];
	cardType card;
	memset(tmp2,0,80);

	//is 16 characters
	if(strlen(t2.fields[0]) !=14) return card;
	//passes mod10
	if(!mod10check(t2.fields[0])) return card;
	//has 7211 constant
	if(strncmp(t2.fields[0],"7211",4)!=0) return card;
	//has a valid month
	if(!isMonth(&t2.fields[1][2])) return card;

	//WE ARE GOOD!
	card.setName("British Petroleum (BP) Gasoline Card");
	strncpy(tmp2,&t2.fields[0][4],10);
	strncpy(&tmp2[10],&t2.fields[1][7],4);
	card.add("Account Number",formatter("XXX XXX XXX X XXXX",tmp2));
	memset(tmp2,0,80);
	strncpy(tmp2,&t2.fields[1][2],2); //grab month
	strcpy(tmp2, monthName(atoi(tmp2)));
	//get the year
	sprintf(tmp2,"%s \'%c%c",tmp2, t2.fields[1][0], t2.fields[1][1]);
	card.add("Expires",tmp2);

	card.setNotes("Cards are Issued and Managed by Citibank");
	card.addUnknowns("Store Number or State it was issued in, Member Since");
	return card;
}

//Delta Crown Room Club Card
//4xxxxxxxxxx=MMYY
cardType deltacrc(track2 t2)
{
	char tmp2[80];
	cardType card;
	memset(tmp2,0,80);

	//1st has is 11 characters
	if(strlen(t2.fields[0]) !=11) return card;
	//2nd has 4 characters
	if(strlen(t2.fields[1]) !=4) return card;
	//Account number passes mod10
	if(!mod10check(&t2.fields[0][1])) return card;
	//starts with a 4
	if(t2.fields[0][0]!='4') return card;
	//has a valid month
	if(!isMonth(&t2.fields[1][0])) return card;

	//WE ARE GOOD!
	card.setName("Delta Crown Room Club Card");
	card.add("Membership Number",formatter("XXX XXX XXXX", &t2.fields[0][1]));
	memset(tmp2,0,80);
	strncpy(tmp2,&t2.fields[1][0],2); //grab month
	strcpy(tmp2, monthName(atoi(tmp2)));
	//get the year
	sprintf(tmp2,"%s \'%c%c",tmp2, t2.fields[1][2], t2.fields[1][3]);
	card.add("Expires",tmp2);

	return card;
}

//Home Depot Consumer Credit Card
//6035xxxxxxxxxxxx@=XXXX101xxxxxxxx 16 digits
cardType homedepotcc(track2 t2)
{

	cardType card;

	//1st is 16
	if(strlen(t2.fields[0])!=16) return card;
	//passes mod10
	if(!mod10check(t2.fields[0])) return card;
	//has 101 constant
	if(strncmp(&t2.fields[1][4],"101",3)!=0) return card;
	//starts with a 6035
	if(strncmp(t2.fields[0],"6035",4)) return card;

	//WE ARE GOOD!
	card.setName("Home Depot Consumer Credit Card");
	card.add("Account Number",formatter("XXXX XXXX XXXX XXXX",t2.fields[0]));

	card.setNotes("Card never expires. Is operated and Managed by Citibank");
	card.addUnknowns("Store Number or State it was issued in");
	return card;
}

//Neiman Marcus Charge Card
//3XXXXXXXX= 9 digits
cardType neimanmarcus(track2 t2)
{

	cardType card;

	//1st is 9
	if(strlen(t2.fields[0])!=9) return card;
	//2nd is 0
	if(strlen(t2.fields[1])!=0) return card;
	//passes mod10
	if(!mod10check(t2.fields[0])) return card;
	//starts with a 3
	if(t2.fields[0][0]!='3') return card;

	//WE ARE GOOD!
	card.setName("Neiman Marcus Charge Card");
	card.add("Account Number",formatter("XXXX XXXX X",t2.fields[0]));

	card.addUnknowns("Full prefix for card, Store Number or State it was issued in");
	card.setNotes("These Charge Cards have no expiration date, which is rare for charge cards");
	return card;
}

//COSTCO Business Membership Card
//7001XXXXXXXXXXXX=24127XXXXXXXXXXXXXXX 16 digits
cardType costcob(track2 t2)
{

	cardType card;
	char tmp[3] = {0,0,0};

	//1st is 16
	if(strlen(t2.fields[0])!=16) return card;
	//2ns is at least 7
	if(strlen(t2.fields[0])<7) return card;
	//1st starts with a 7001
	if(strncmp(t2.fields[0],"7001",4)!=0) return card;
	//2nd starts with 2412799
	if(strncmp(t2.fields[1],"24127",5)!=0) return card;
	//WE ARE GOOD!
	card.setName("COSTCO Membership Card");
	card.add("Membership Number",formatter("XXXX XXXX XXXX",&t2.fields[0][4]));
	strncpy(tmp,&t2.fields[1][5],2);

	card.add("Member Since",tmp);

	card.addUnknowns("Member Since, Type of card (Business or Standard");
	card.setNotes("These cards are either Standard or Business Class. Stripe Snoop cannot currently tell the difference");

	return card;
}

//US Air Frequent Traveler Card
//7XXXXXXXX=XXXXXXXX 9 digits
cardType usair(track2 t2)
{

	cardType card;
	char tmp2[20];
	memset(tmp2,0,20);


	//1st is 9
	if(strlen(t2.fields[0])!=9) return card;
	//2nd is 8
	if(strlen(t2.fields[1])!=8) return card;
	//starts with a 7
	if(t2.fields[0][0]!='7') return card;
	//has a valid month
	if(!isMonth(&t2.fields[1][2])) return card;

	//WE ARE GOOD!
	card.setName("US Air Frequent Traveler Card");
	card.add("Membership Number",formatter("XXX XXX XXX",t2.fields[0]));
	strncpy(tmp2,&t2.fields[1][2],2); //grab month
	strcpy(tmp2, monthName(atoi(tmp2)));
	//get the year
	sprintf(tmp2,"%s \'%c%c",tmp2, t2.fields[1][0], t2.fields[1][1]);
	card.add("Member Since",tmp2);

	card.addUnknowns("When this card was issued");

	return card;
}

//Walden Books Reader Card
//5XXXXXXXX= 9 digits
cardType walden(track2 t2)
{

	cardType card;

	//1st is 9
	if(strlen(t2.fields[0])!=9) return card;
	//2nd is 0
	if(strlen(t2.fields[1])!=0) return card;
	//passes mod10
	if(!mod10check(t2.fields[0])) return card;
	//starts with a 3
	if(t2.fields[0][0]!='5') return card;

	//WE ARE GOOD!
	card.setName("Walden Books Reader Card");
	card.add("Account Number",formatter("XXX XXX XXX",t2.fields[0]));

	return card;
}


//Checks for AAMVA Driver's License
//636XXXxxxxxxxxx@=YYMMCCYYMMDD? 16 digits

cardType aamva(track2 t2)
{
	char day[3]={0,0,0};
	char year[5]={0,0,0,0,0};

	int i,j;
	FILE * fin;
	char tmp2[80];
	cardType card;
	memset(tmp2,0,80);

	//1st is at least 7 characters
	if(strlen(t2.fields[0]) < 7) return card;
	//2nd is at least 12 characters
	if(strlen(t2.fields[0]) < 12) return card;
	//starts with "636"
	if(strncmp(t2.fields[0],"636",3)!=0) return card;
 	//has a valid month for expire, or 77 88 99
	if(!isMonth(&t2.fields[1][2]) && strncmp(&t2.fields[1][2],"77",2)!=0
		&& strncmp(&t2.fields[1][2],"88",2)!=0
		&& strncmp(&t2.fields[1][2],"99",2)!=0)
			return card;
	//has a valid DOB
	//funny month fix
	if(t2.fields[1][8]!='1')
		t2.fields[1][8] = '0';

	if(!dateValid(&t2.fields[1][8], &t2.fields[1][10], &t2.fields[1][4])) return card;
	//WE ARE GOOD!
	card.setName("AAMVA Compliant North American Driver's License");

	/*get issuing State/Territory!*/
	if( (fin=fopen(AAMVADATABASE,"r")) == NULL) {
		printf("Error: aamva(): Cannot open \"%s\"\n",AAMVADATABASE);
	} else {
		memset(tmp2,0,80);
		bool flag = false;
		while(!feof(fin))
		{
			fgets(tmp2,80,fin);
			if(strlen(tmp2)>8) {
				if( strncmp(t2.fields[0],tmp2,6) == 0) {
					flag = true;
					break;
				}
			}
		}
		//kill trailing CRLF
		tmp2[6 + strlen(&tmp2[7])]=0;
		if(flag)
			card.add("Issuing Territory", &tmp2[7]);
		else
			card.add("Issuing Territory", "Unknown");
		fclose(fin);
	}
	card.add("License Number",&t2.fields[0][6]);

	//get the DOB
	memset(tmp2,0,80);
	strncpy(tmp2,&t2.fields[1][8],2); //grab month
	strncpy(day,&t2.fields[1][10],2);
	strncpy(year,&t2.fields[1][4],4);
	i = atoi(tmp2);
	sprintf(tmp2,"%s %s, %s",monthName(i),day,year);
	card.add("Date of Birth",tmp2);


	//Get the expiration Date
	memset(year,0,5);
	strncpy(year,&t2.fields[1][0],2);;
	j=atoi(year);
	j = expandYear(j);
	if(strncmp(&t2.fields[1][2],"77",2)==0) {
		strcpy(tmp2,"Never");
	} else if(strncmp(&t2.fields[1][2],"88",2)==0) {
		sprintf(tmp2,"%s %d, %d",monthName(i),lastDotm(i,j),j);
	} else if(strncmp(&t2.fields[1][2],"99",2)==0) {
		sprintf(tmp2,"%s %s, %d",monthName(i),day,j);
	} else {
		//plain expiration date
		strncpy(tmp2,&t2.fields[1][2],2); //grab month
		sprintf(tmp2,"%s %d",monthName(atoi(tmp2)),j);
	}
	card.add("Expires",tmp2);

	card.setNotes("This Standard is deployed not only in the US, but parts of Canada as well. Some States/Territories may use letters in their License Numbers. If this is the case, these numbers will be encoded using 2 numbers as follows: A=01, B=02 ... Z=26.");

	return card;
}

//Old California Driver's License
//Don't know what leading numbers identify
//Uses similar encoding to AAMVA
//First two digits are letter, then 7 digits of driver's license number
//Second field is expiration
//Third field is birthday
//600xxxLLNNNNNNN=YYMM=YYYYMMDD

cardType oldcadmv(track2 t2)
{
	char day[3]={0,0,0};
	char year[5]={0,0,0,0,0};

	int i,j;
	char tmp2[80];
	cardType card;
	memset(tmp2,0,80);

	//1st is at least 15 characters
	if(strlen(t2.fields[0]) < 15) return card;
	//starts with "600"
	if(strncmp(t2.fields[0],"600",3)!=0) return card;
 	//has a valid month for expire
	if(!isMonth(&t2.fields[1][2]))
			return card;
	//has a valid DOB

	if(!dateValid(&t2.fields[2][4], &t2.fields[2][6], &t2.fields[2][0])) return card;
	//WE ARE GOOD!
	card.setName("Old Style California Driver's License");

	memset(tmp2,0,80);

	strncpy(tmp2,&t2.fields[0][6],2);

	i = atoi(tmp2);

	sprintf(tmp2,"%c%s",numToAlpha(i),&t2.fields[0][8]);

	card.add("License Number",tmp2);

	//get the DOB
	memset(tmp2,0,80);
	strncpy(tmp2,&t2.fields[2][4],2); //grab month
	strncpy(day,&t2.fields[2][6],2); //grab day
	strncpy(year,&t2.fields[2][0],4); //grab year
	i = atoi(tmp2);
	sprintf(tmp2,"%s %s, %s",monthName(i),day,year);
	card.add("Date of Birth",tmp2);


	//Get the expiration Date
	memset(year,0,5);
	strncpy(year,&t2.fields[1][0],2);;
	j=atoi(year);
	j = expandYear(j);

	//plain expiration date
	strncpy(tmp2,&t2.fields[1][2],2); //grab month
	sprintf(tmp2,"%s %d",monthName(atoi(tmp2)),j);

	card.add("Expires",tmp2);

	card.setNotes("Pre-AAMVA style California Drivers License");

	return card;
}


// ================================================================= 4 Fields

//Checks for Georgia Institute of Technology Buzzcard
//OLD: 1570=xxxSSNxxx=0x=60177000xxxxxxxx 00 or 02
//NEW: 1570=90xxxxxxx=00=60177000xxxxxxxx
//PARK or TEMP 1570=000000000=00=60177000xxxxxxxx

cardType buzzcard(track2 t2)
{
	cardType card;

	//first field is 1570
	if(strcmp(t2.fields[0],"1570") !=0) return card;
	//2nd field is 9 characters
	if(strlen(t2.fields[1]) !=9) return card;
	//3rd field ==00 || 02
	if( strcmp(t2.fields[2],"00")!=0 && strcmp(t2.fields[2],"02")!=0 )
		return card;
	//4th field starts with 60177000
	if(strncmp(t2.fields[3],"60177000",8) !=0) return card;

	//WE ARE GOOD!
	//which type, old, new, or parking/temp?
	if(strncmp(t2.fields[1],"90",2)!=0) {
		card.setName("Georgia Tech Buzzcard - Pre 2002 version");
		card.add("SSN/Student Number",formatter("XXX-XX-XXXX",t2.fields[1]));
	} else if(strncmp(t2.fields[1],"000000000",9)==0) {
		card.setName("Georgia Tech Parking or Temporary Card");
	} else {
		card.setName("Georgia Tech Buzzcard - GTID Version");
		card.add("GTID",formatter("XXX-XX-XXXX",t2.fields[1]));
	}
	card.add("Database ID",t2.fields[3]);
	return card;
}

cardType lookup2(track2 t2)
{
	cardType result;

	//do only the tests that matter
	if(t2.numFields==1)	{

		result = bnreader(t2);
		if(result.numFields>0) return result;
		result = radisson(t2);
		if(result.numFields>0) return result;
	} else if (t2.numFields==2) {
		result = AAA(t2);
		if(result.numFields>0) return result;
		result = visa(t2);
		if(result.numFields>0) return result;
		result = mastercard(t2);
		if(result.numFields>0) return result;
		result = amex(t2);
		if(result.numFields>0) return result;
		result = discovercard(t2);
		if(result.numFields>0) return result;
		result = kroger(t2);
		if(result.numFields>0) return result;
		result = atm(t2);
		if(result.numFields>0) return result;
		result = bngift(t2);
		if(result.numFields>0) return result;
		result = bpgas(t2);
		if(result.numFields>0) return result;
		result = deltacrc(t2);
		if(result.numFields>0) return result;
		result = homedepotcc(t2);
		if(result.numFields>0) return result;
		result = neimanmarcus(t2);
		if(result.numFields>0) return result;
		result = costcob(t2);
		if(result.numFields>0) return result;
		result = usair(t2);
		if(result.numFields>0) return result;
		result = walden(t2);
		if(result.numFields>0) return result;
		result = aamva(t2);
		if(result.numFields>0) return result;
	} else if (t2.numFields==3)
	{
		result = oldcadmv(t2);
		if(result.numFields>0) return result;
	} else if (t2.numFields==4) {
		result = buzzcard(t2);
		if(result.numFields>0) return result;

	}

	return result;
}


