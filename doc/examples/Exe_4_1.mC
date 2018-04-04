
‫‪void del(char s[])
{
  int i;
  int j;
   for(i=0,s[i],i++)
   {
     for(j=(i+1),s[j],j++)
     {
       if(s[i]=s[j]){
         strcpy(&s[j],&s[j+1])
         j=i
       }
     }
   }

}

void main()
{

	int n;
	n=11;
	n = read_int();
	print_n();

	int result;
	result = del(s);


	print_int(n);
	print("the result is");
	print_int(result);
	print_n();
}
