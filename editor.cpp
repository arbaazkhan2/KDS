#include<fstream.h>
#include<iostream.h>
#include<conio.h>
#include<string.h>
#include<stdio.h>

int main()
{   clrscr();
    ifstream abc;
    int ctr=0,i;
    char line[111], x[3],*ptr,j;
    strcpy(x,"0");
    ofstream gps;
    abc.open("log.txt");
    gps.open("gps.txt");
    abc.seekg(0);
    while(!abc.eof())
    {
         abc.read(line,109);
			ctr = ctr+108;
         //cout<<line<<endl; getch();
         for(i=0;i<=109;i++)
        	{  //cout<<i;
			if(line[i] == 'G' && line[i+1] == 'P' && line[i+2] == 'S' && line[i+3] == ',')
	        {  	ctr = ctr + i-108;
           //cout<<endl<<ctr<<endl;
					abc.seekg(ctr);
               strcpy(x,"0");
					while(strcmp((x),"A")!=0)
           		{
						abc.read(x,1);
                  gps.write(x,1);
                  //cout<<x;
						abc.seekg(++ctr);
					}
         	}
			}

    }
    cout<<"done";
    abc.close();
    gps.close();
getch();
return 0;
}

