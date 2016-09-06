#include<stdio.h>
#include<math.h>
void main()
{
	float a,b;
	int d;
	while(1){
	printf("input operator---> 1:+,2:-,3:*,4:/,5:^2,6:sqrt (0 to exit)\n===>");
	scanf("%d",&d);
	if(d == 0) break;
	switch(d)
	{
		case 1:

		printf("input two number a,b:");
		scanf("%f %f",&a,&b);
		printf("%f+%f=",a,b);
		printf("%f\n",a+b);break;
		case 2:

		printf("input two number a,b:");
		scanf("%f %f",&a,&b);
		printf("%f-%f=",a,b);
		printf("%f\n",a-b);break;
		case 3:
		
		printf("input two number a,b:");
		scanf("%f %f",&a,&b);
		printf("%f*%f=",a,b);
		printf("%f\n",a*b);break;
		case 4:
		
		printf("input two number a,b:");
		scanf("%f %f",&a,&b);
		printf("%f/%f=",a,b);
		printf("%f\n",a/b);break;

		case 5:

		printf("input one number a:");
		scanf("%f",&a);
		printf("%f^2=",a);
		printf("%f\n",a*a);break;
		case 6:
		printf("input one number a:");
		scanf("%f",&a);
		printf("%f^0.5=",a);
		printf("%f\n",sqrt(a));break;
		default:printf("input error\n");
	}
	}
}
