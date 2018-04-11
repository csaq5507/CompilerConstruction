/* Example of how to work with arrays in mCc */

int init_array(int [3] array, int length_of_array, int default_value)  {
	int i;
	i = 0;
	while (i < length_of_array) {
		array[i] = default_value; 
		i = i +1;
	}
	return array;
}  

int add_array(int [3] first_array, int [1] second_array, int length_of_array) {
	int i;
	i = 0;
	int [4] ret_array;
	while (i < length_of_array) {
		ret_array[i] = first_array[i] + second_array[i];
		i = i +1;
	}
	return ret_array;
}

int main() { 
	int [50] ()array;
 
	init_array(values, 50, 0); 
	 
	print("The value at location 7 is "); 
	print_int(array[7]);
	print("\n");
 
	array[7] = 12;
	
	print("The value at location 7 is "); 
	print_int(array[7]);
	print("\n");
	
	array = add_array(array, array, 50);
	
	print("The value at location 7 is "); 
	print_int(array[7]);
	print("\n");
	
	return 0; 
} 
