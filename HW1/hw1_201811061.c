#include <stdio.h>
#include <stdlib.h>
typedef unsigned char* pointer;

// Print bit representation of the given data
// This code was implemented in our lecture
void print_bit(pointer a, int len) {
    for (int i = 0; i < len; ++i) {
        for (int j = 7; j >= 0; --j) {
            printf("%d", (a[i] >> j) & 0x01);
        }
        printf(" ");
    }
    printf("\n");
}


// Problem 1
void reverse_bit(pointer a, int len) {
	pointer b = malloc(sizeof(len));
    for (int i = 0; i < len; ++i) {
		unsigned char temp = 0;
	    for (int j = 7; j >= 0; --j) {
			temp = temp + (((a[len-i-1] >> j) & 0x01) << (7-j));
	    }
		b[i] = temp;
    }
	for (int i = 0; i < len; i++){
		a[i] = b[i];
	}
	free(b);
}


// Problem 2
void split_bit(pointer a, pointer out1, pointer out2, int len) {
	unsigned char temp1 = 0;
	unsigned char temp2 = 0;
	for(int i = 0; i < len; i++){
		for(int j = 0; j < 8; j++){
			if(j % 2 != 0){
				if(i % 2 != 0){
					temp1 = temp1 + (((a[i] >> j) & 0x01) << ((j-1)/2));
				}
				else {
					temp1 = temp1 +(((a[i] >> j) & 0x01) << ((j-1)/2 + 4));
				}
			}
			else {
				if(i % 2 != 0){
					temp2 = temp2 + (((a[i] >> j) & 0x01) << (j/2));
				}
				else {
					temp2 = temp2 + (((a[i] >> j) & 0x01) << ((j/2) + 4));
				}
			}
		}
		if(i % 2 != 0){
			out1[(i-1)/2] = temp1;
			out2[(i-1)/2] = temp2;
			temp1 = 0;
			temp2 = 0;
		}
	}
}


// Problem 3
unsigned int mul_four_plus_one(unsigned int a) {
    unsigned int* aptr = &a;
	unsigned int result;
	result = ((aptr[0] << 2) | 0x01);
    return result; 
}

// Problem 4
unsigned int convert_endian(unsigned int a) {
    pointer aptr = (pointer)&a;
	pointer temp = malloc(sizeof(int));
	
	for(int i = 0; i < 4; i++){
		temp[i] = aptr[3-i];
	}
	unsigned int* ptr = (unsigned int*) temp;
	unsigned int result = *ptr;
	free(temp);
    return result; 
}


// Problem 5
void get_date(unsigned int date, int* pYear, int* pMonth, int* pDay) {
    pointer iptr = (pointer)&date;
	int day = 0;
	int month = 0;
	int year = 0;
	for(int i = 0; i < 5; i++){
		day = day + (((iptr[0] >> i) & 0x01) << i);
	}
	for(int i = 5; i < 8; i++){
		month = month + (((iptr[0] >> i) & 0x01) << (i-5));
	}
	month = month + ((iptr[1] & 0x01) << 3);
	for(int i = 1; i < 8; i++){
		year = year + (((iptr[1] >> i) & 0x01) << (i-1));
	}
	for(int i = 0; i < 8; i++){
		year = year + (((iptr[2] >> i) & 0x01) << (7+i));
	}
	for(int i = 0; i < 8; i++){
		year = year + (((iptr[3] >> i) & 0x01) << (15+i));
	}

    *pYear = year; // modify this
    *pMonth = month; // modify this
    *pDay = day; // modify this
}


int main() {
    // You don't need to touch the main function
    printf("Problem 1\n");
    unsigned int v1 = 0x1234CDEF;
    print_bit((pointer)&v1, sizeof(v1));
    reverse_bit((pointer)&v1, sizeof(v1));
    print_bit((pointer)&v1, sizeof(v1));


    printf("Problem 2\n");
    unsigned int v2 = 0x1234CDEF;
    unsigned short out1 = 0, out2 = 0;
    print_bit((pointer)&v2, sizeof(v2));
    split_bit((pointer)&v2, (pointer)&out1, (pointer)&out2, sizeof(v2));
    print_bit((pointer)&out1, sizeof(out1));
    print_bit((pointer)&out2, sizeof(out2));


    printf("Problem 3\n");
    unsigned int v3 = 100;
    unsigned int v3_ret = mul_four_plus_one(v3);
    printf("%u*4+1 = %u\n", v3, v3_ret);
    print_bit((pointer)&v3, sizeof(v3));
    print_bit((pointer)&v3_ret, sizeof(v3_ret));


    printf("Problem 4\n");
    unsigned int v4 = 0x12345678;
    unsigned int v4_ret = convert_endian(v4);
    print_bit((pointer)&v4, sizeof(v4));
    print_bit((pointer)&v4_ret, sizeof(v4_ret));


    printf("Problem 5\n");
    unsigned int date = 1035391;
    int year, month, day;
    print_bit((pointer)&date, sizeof(date));
    get_date(date, &year, &month, &day);
    printf("%d -> %d/%d/%d\n", date, year, month, day);

    return 0;
}
