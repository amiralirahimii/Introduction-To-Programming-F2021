#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include<stdlib.h>
#include<string.h>

#define USERS_FILE_PATH "users.txt"
#define HALLS_FILE_PATH "halls.txt"
#define FOODS_FILE_PATH "foods.txt"
#define COUPONS_FILE_PATH "coupons.txt"

#define LOGIN_WORDS_NEEDED 3
#define SIGNUP_WORDS_NEEDED 4
#define LOGOUT_WORDS_NEEDED 1
#define ADD_HALL_WORDS_NEEDED 4
#define ADD_FOOD_WORDS_NEEDED 7
#define ADD_COUPON_WORDS_NEEDED 4
#define STUDENT_ID_SIZE 9


typedef struct Student {
	char* username;
	int student_id;
	char* password;
}Student;

typedef struct Hall {
	char* name;
	int id;
	int capacity;
}Hall;

typedef struct Food {
	char* name;
	int id;
	int price;
	int capacity;
	int hall_id;
	int day;
}Foods;

typedef struct Coupon {
	int id;
	int percentage;
	int capacity;
}Coupon;


void put_users_in_array(FILE* users, Student** students, int* number_of_students) {
	if (fgetc(users)==EOF) {//if the file is empty, it returns back
		return;
	}
	else {//we find out there is 1 student at least and we resume code
		*number_of_students = 1;
		fseek(users, 0, SEEK_SET);//cause fgetc at first and it seeked 1 forward and we start again
	}
	while (true) {
		*students = (Student*)realloc(*students, *number_of_students * sizeof(Student));
		int size_of_username = 1;
		(*students)[*number_of_students - 1].username = (char*)malloc(size_of_username * sizeof(char));//here we malloc 1 and see if it is more,we realloc
		char c;
		while ((c = fgetc(users)) != ' ') {//reading username
			((*students)[*number_of_students - 1].username)[size_of_username - 1] = c;
			size_of_username++;
			(*students)[*number_of_students - 1].username = (char*)realloc((*students)[*number_of_students - 1].username, size_of_username * sizeof(char));
		}
		(*students)[*number_of_students - 1].username[size_of_username - 1] = '\0';

		char* id = (char*)malloc((STUDENT_ID_SIZE + 1) * sizeof(char));//we know the student_id is a 9 digit number and we use +1 cause we need '\0' at the end
		int i = 0;
		while ((c = fgetc(users)) != ' ') {
			id[i] = c;
			i++;
		}
		id[i] = '\0';
		(*students)[*number_of_students - 1].student_id = atoi(id);
		free(id);//we dont need id any more

		int size_of_password = 1;
		(*students)[*number_of_students - 1].password = (char*)malloc(size_of_password * sizeof(char));
		while ((c = fgetc(users)) != '\n' && c != EOF) {
			(*students)[*number_of_students - 1].password[size_of_password - 1] = c;
			size_of_password++;
			(*students)[*number_of_students - 1].password = (char*)realloc((*students)[*number_of_students - 1].password, size_of_password * sizeof(char));
		}
		(*students)[*number_of_students - 1].password[size_of_password - 1] = '\0';
		if (c == EOF)
			break;
		(*number_of_students)++;
	}
}

void put_halls_in_array(FILE* halls_file, Hall** halls, int* number_of_halls) {
	if (fgetc(halls_file) == EOF) {//if the file is empty, it returns back
		return;
	}
	else {//we find out there is 1 student at least and we resume code
		*number_of_halls = 1;
		fseek(halls_file, 0, SEEK_SET);//cause fgetc at first and it seeked 1 forward and we start again
	}
	while (true) {
		*halls = (Hall*)realloc(*halls, *number_of_halls * sizeof(Hall));
		int size_of_name = 1;
		(*halls)[*number_of_halls - 1].name = (char*)malloc(size_of_name * sizeof(char));//here we malloc 1 and see if it is more,we realloc
		char c;
		while ((c = fgetc(halls_file)) != ' ') {//reading name
			((*halls)[*number_of_halls - 1].name)[size_of_name - 1] = c;//could have make a char* and first save it in char* then strlen the char* and malloc this len,its easier to understand
			size_of_name++;
			(*halls)[*number_of_halls - 1].name = (char*)realloc((*halls)[*number_of_halls - 1].name, size_of_name * sizeof(char));
		}
		(*halls)[*number_of_halls - 1].name[size_of_name - 1] = '\0';

		int size_of_id=1;//for digits number of id
		char* id = (char*)malloc(size_of_id * sizeof(char));
		while ((c = fgetc(halls_file)) != ' ') {
			id[size_of_id - 1] = c;
			size_of_id++;
			id = (char*)realloc(id, size_of_id * sizeof(char));
		}
		id[size_of_id - 1] = '\0';
		(*halls)[*number_of_halls - 1].id = atoi(id);
		free(id);//we dont need id any more

		int size_of_capacity = 1;//for digits number of capacity
		char* capacity = (char*)malloc(size_of_capacity * sizeof(char));
		while ((c = fgetc(halls_file)) != '\n' && c != EOF) {
			capacity[size_of_capacity - 1] = c;
			size_of_capacity++;
			capacity = (char*)realloc(capacity, size_of_capacity * sizeof(char));
		}
		capacity[size_of_capacity - 1] = '\0';
		(*halls)[*number_of_halls - 1].capacity = atoi(capacity);
		free(capacity);//we dont need capacity any more
		if (c == EOF) {
			break;
		}
		(*number_of_halls)++;
	}
}

void put_foods_in_array(FILE* foods_file, Food** foods, int* number_of_foods) {
	if (fgetc(foods_file) == EOF) {//if the file is empty, it returns back
		return;
	}
	else {//we find out there is 1 student at least and we resume code
		*number_of_foods = 1;
		fseek(foods_file, 0, SEEK_SET);//cause fgetc at first and it seeked 1 forward and we start again
	}
	while (true) {
		//name
		*foods = (Food*)realloc(*foods, *number_of_foods * sizeof(Food));
		int size_of_name = 1;
		(*foods)[*number_of_foods - 1].name = (char*)malloc(size_of_name * sizeof(char));//here we malloc 1 and see if it is more,we realloc
		char c;
		while ((c = fgetc(foods_file)) != ' ') {//reading name
			((*foods)[*number_of_foods - 1].name)[size_of_name - 1] = c;//could have make a char* and first save it in char* then strlen the char* and malloc this len,its easier to understand
			size_of_name++;
			(*foods)[*number_of_foods - 1].name = (char*)realloc((*foods)[*number_of_foods - 1].name, size_of_name * sizeof(char));
		}
		(*foods)[*number_of_foods - 1].name[size_of_name - 1] = '\0';

		//food_id
		int size_of_id = 1;//for digits number of id
		char* id = (char*)malloc(size_of_id * sizeof(char));
		while ((c = fgetc(foods_file)) != ' ') {
			id[size_of_id - 1] = c;
			size_of_id++;
			id = (char*)realloc(id, size_of_id * sizeof(char));
		}
		id[size_of_id - 1] = '\0';
		(*foods)[*number_of_foods - 1].id = atoi(id);
		free(id);//we dont need id any more

		//price
		int size_of_price = 1;//for digits number of price
		char* price = (char*)malloc(size_of_price * sizeof(char));
		while ((c = fgetc(foods_file)) != ' ') {
			price[size_of_price - 1] = c;
			size_of_price++;
			price = (char*)realloc(price, size_of_price * sizeof(char));
		}
		price[size_of_price - 1] = '\0';
		(*foods)[*number_of_foods - 1].price = atoi(price);
		free(price);//we dont need price any more

		//capacity
		int size_of_capacity = 1;//for digits number of capacity
		char* capacity = (char*)malloc(size_of_capacity * sizeof(char));
		while ((c = fgetc(foods_file)) != ' ') {
			capacity[size_of_capacity - 1] = c;
			size_of_capacity++;
			capacity = (char*)realloc(capacity, size_of_capacity * sizeof(char));
		}
		capacity[size_of_capacity - 1] = '\0';
		(*foods)[*number_of_foods - 1].capacity = atoi(capacity);
		free(capacity);//we dont need capacity any more

		//hall_id
		int size_of_hall_id = 1;//for digits number of hall_id
		char* hall_id = (char*)malloc(size_of_hall_id * sizeof(char));
		while ((c = fgetc(foods_file)) != ' ') {
			hall_id[size_of_hall_id - 1] = c;
			size_of_hall_id++;
			hall_id = (char*)realloc(hall_id, size_of_hall_id * sizeof(char));
		}
		hall_id[size_of_hall_id - 1] = '\0';
		(*foods)[*number_of_foods - 1].hall_id = atoi(hall_id);
		free(hall_id);//we dont need hall_id any more

		//day
		int size_of_day = 1;//for digits number of day(we know day is 1 digit but we write like this to be similar to the top lines
		char* day = (char*)malloc(size_of_day * sizeof(char));
		while ((c = fgetc(foods_file)) != '\n' && c != EOF) {
			day[size_of_day - 1] = c;
			size_of_day++;
			day = (char*)realloc(day, size_of_day * sizeof(char));
		}
		day[size_of_day - 1] = '\0';
		(*foods)[*number_of_foods - 1].day = atoi(day);
		free(day);//we dont need day any more

		if (c == EOF) {
			break;
		}
		(*number_of_foods)++;
	}
}


void put_coupons_in_array(FILE* coupons_file, Coupon** coupons, int* number_of_coupons) {
	if (fgetc(coupons_file) == EOF) {//if the file is empty, it returns back
		return;
	}
	else {//we find out there is 1 student at least and we resume code
		*number_of_coupons = 1;
		fseek(coupons_file, 0, SEEK_SET);//cause fgetc at first and it seeked 1 forward and we start again
	}
	
	while (true) {
		//copoun_id
		*coupons = (Coupon*)realloc(*coupons, *number_of_coupons * sizeof(Coupon));
		int size_of_id = 1;//for digits number of id
		char* id = (char*)malloc(size_of_id * sizeof(char));
		char c;

		while ((c = fgetc(coupons_file)) != ' ') {
			id[size_of_id - 1] = c;
			size_of_id++;
			id = (char*)realloc(id, size_of_id * sizeof(char));
		}
		id[size_of_id - 1] = '\0';
		(*coupons)[*number_of_coupons - 1].id = atoi(id);
		free(id);//we dont need id any more

		//percentage
		int size_of_percentage = 1;//for digits number of percentage
		char* percentage = (char*)malloc(size_of_percentage * sizeof(char));
		while ((c = fgetc(coupons_file)) != ' ') {
			percentage[size_of_percentage - 1] = c;
			size_of_percentage++;
			percentage = (char*)realloc(percentage, size_of_percentage * sizeof(char));
		}
		percentage[size_of_percentage - 1] = '\0';
		(*coupons)[*number_of_coupons - 1].percentage = atoi(percentage);
		free(percentage);//we dont need char* percentage any more

		//capacity
		int size_of_capacity = 1;//for digits number of capacity
		char* capacity = (char*)malloc(size_of_capacity * sizeof(char));
		while ((c = fgetc(coupons_file)) != '\n' && c != EOF) {
			capacity[size_of_capacity - 1] = c;
			size_of_capacity++;
			capacity = (char*)realloc(capacity, size_of_capacity * sizeof(char));
		}
		capacity[size_of_capacity - 1] = '\0';
		(*coupons)[*number_of_coupons - 1].capacity = atoi(capacity);
		free(capacity);//we dont need char* capacity any more

		if (c == EOF) {
			break;
		}
		(*number_of_coupons)++;
	}
}


void get_input(char*** user_input, int* number_of_words) { //this function read user input
	char c;//to read char by char from user
	int i=0;//to see which word we are in (ex:word number 1)
	int j=0;//to see which element of the word we are in(ex: element 3 of word i)
	*user_input = (char**)malloc((i+1) * sizeof(char*));
	(*user_input)[i] =(char*)malloc((j+1)*sizeof(char));
	while ((c = getchar()) != '\n') {
		if (c != ' ') {
			(*user_input)[i][j] = c;
			j++;
			(*user_input)[i] = (char*)realloc((*user_input)[i], (j + 1) * sizeof(char));
		}
		else {
			(*user_input)[i][j] = NULL;
			i++;
			(*user_input) = (char**)realloc((*user_input), (i + 1) * sizeof(char*));
			j = 0;
			(*user_input)[i] = (char*)malloc((j + 1) * sizeof(char));
		}
	}
	(*user_input)[i][j] = NULL;//when c='\n' we dont NULL the last block
	*number_of_words = i+1;//cause i use i=0 for student 1;
}

void delete_user_input(char*** user_input, int number_of_words) {
	for (int i = 0; i < number_of_words;i++) {//no need to change number of words because it will be changed automaticly in next loop
		free((*user_input)[i]);
	}
	free(*user_input);
}

int check_login(char** user_input, int number_of_words, Student* students, int number_of_students ,int* login_status) {
	if (strcmp(user_input[0], "login") != 0) {
		return 0;
	}

	if (number_of_words != LOGIN_WORDS_NEEDED) {
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}

	if (*login_status != 0) {
		printf("You are already login,please logout to login again\n");
		return 1;
	}
	
	if (strcmp(user_input[1], "admin") == 0 && strcmp(user_input[1], "admin") == 0) {
		printf("Welcome admin\n");
		*login_status = -1;
		return 1;
	}

	for (int i = 0; i < number_of_students; i++) {//if the student number=0 it doesnt enter in thisfor and just go to the printf
		if (strcmp(user_input[1], students[i].username) == 0 && strcmp(user_input[2], students[i].password) == 0) {
			printf("welcome %s\n", user_input[1]);
			*login_status = i+1;//cause we set i=0 for student 1
			return 1;
		}
	}
	printf("wrong username or password\n");
}

void append_new_user(char** user_input) {
	FILE* users;
	users = fopen(USERS_FILE_PATH, "a+");
	if (!users) {
		printf("Users File Didnt open");
	}
	if (fgetc(users) != EOF) {
		fseek(users, -1, SEEK_CUR);//we move forward to check, now we move backward
		fputc('\n', users);
	}
	fprintf(users, "%s ", user_input[1]);//username added
	fprintf(users, "%s ", user_input[2]);//student_id added
	fprintf(users, "%s", user_input[3]);//password added(didnt put space)
	fclose(users);
}

void put_new_user_in_students_list(char** user_input, Student** students,int* number_of_students) {
	//here we dont need so many reallocs cause we have the user input from before
	(*number_of_students)++;
	*students =(Student*)realloc(*students, *number_of_students * sizeof(Student));

	(*students)[*number_of_students - 1].username = (char*)malloc(strlen(user_input[1]) * sizeof(char));
	strcpy((*students)[*number_of_students - 1].username, user_input[1]);

	(*students)[*number_of_students - 1].student_id = atoi(user_input[2]);

	(*students)[*number_of_students - 1].password = (char*)malloc(strlen(user_input[3]) * sizeof(char));
	strcpy((*students)[*number_of_students - 1].password, user_input[3]);

}

int check_signup(char** user_input, int number_of_words, Student** students, int* number_of_students, int login_status) {
	if (strcmp(user_input[0], "signup") != 0) {
		return 0;
	}

	if (number_of_words != SIGNUP_WORDS_NEEDED) {
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}

	if (login_status != 0) {
		printf("You are already login,please logout to signup\n");
		return 1;
	}
	

	if (strcmp(user_input[1], "admin") == 0) {
		printf("You cant use admin as your username\n");
		return 1;
	}

	for (int i = 0; i < *number_of_students; i++) {
		if (strcmp(user_input[1], (*students)[i].username) == 0) {
			printf("user already exists\n");
			return 1;
		}
	}

	append_new_user(user_input);
	put_new_user_in_students_list(user_input, students, number_of_students);//using this func to prevent opening file again
	printf("done\n");
	return 1;
}


int check_logout(char** user_input, int number_of_words, int* login_status ) {
	if (strcmp(user_input[0], "logout") != 0) {
		return 0;
	}
	if (number_of_words != LOGOUT_WORDS_NEEDED) {//user using logout but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (*login_status == 0) {
		printf("access denied\n");//a not login user is using a command
		return 1;
	}
	*login_status = 0;
	printf("done\n");
}

void append_new_hall(char ** user_input) {
	FILE* halls;
	halls = fopen(HALLS_FILE_PATH, "a+");
	if (!halls) {
		printf("Halls File Didnt open");
	}
	if (fgetc(halls) != EOF) {
		fseek(halls, -1, SEEK_CUR);//we move forward to check, now we move backward
		fputc('\n', halls);
	}
	fprintf(halls, "%s ", user_input[1]);//name added
	fprintf(halls, "%s ", user_input[2]);//id added
	fprintf(halls, "%s", user_input[3]);//capacity added(didnt put space)
	fclose(halls);
}

void put_new_hall_in_halls_list(char** user_input,Hall** halls,int *number_of_halls) {
	//here we dont need so many reallocs cause we have the user input length from before
	(*number_of_halls)++;
	*halls = (Hall*)realloc(*halls, *number_of_halls * sizeof(Hall));

	(*halls)[*number_of_halls - 1].name = (char*)malloc(strlen(user_input[1]) * sizeof(char));
	strcpy((*halls)[*number_of_halls - 1].name, user_input[1]);

	(*halls)[*number_of_halls - 1].id = atoi(user_input[2]);

	(*halls)[*number_of_halls - 1].capacity = atoi(user_input[3]);
}

int check_add_hall(char** user_input, int number_of_words, Hall** halls, int* number_of_halls, int login_status) {
	if (strcmp(user_input[0], "add_hall") != 0) {
		return 0;
	}
	if (number_of_words != ADD_HALL_WORDS_NEEDED) {//user using logout but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (login_status != -1) {
		if (login_status == 0) {
			printf("access denied\n");
		}
		else {
			printf("Permission denied\n");
		}
		return 1;
	}
	for (int i = 0; i < *number_of_halls; i++) {
		if ((*halls)[i].id == atoi(user_input[2])) {
			printf("hall_id is not unique\n");
			return 1;
		}
	}
	append_new_hall(user_input);
	put_new_hall_in_halls_list(user_input, halls, number_of_halls);//using this func to prevent opening file again
	printf("done\n");
	return 1;
}

void append_new_food(char** user_input) {
	FILE* foods;
	foods = fopen(FOODS_FILE_PATH, "a+");
	if (!foods) {
		printf("foods File Didnt open");
	}
	if (fgetc(foods) != EOF) {
		fseek(foods, -1, SEEK_CUR);//we move forward to check, now we move backward
		fputc('\n', foods);
	}
	for (int i = 1; i < ADD_FOOD_WORDS_NEEDED-1; i++) {//from i=1 to i=5
		fprintf(foods, "%s ", user_input[i]);
	}
	fprintf(foods, "%s", user_input[6]);//day added(didnt put space)
	fclose(foods);
}

void put_new_food_in_foods_list(char** user_input, Food** foods, int* number_of_foods) {
	//here we dont need so many reallocs cause we have the user input length from before
	(*number_of_foods)++;
	*foods = (Food*)realloc(*foods, *number_of_foods * sizeof(Food));

	(*foods)[*number_of_foods - 1].name = (char*)malloc(strlen(user_input[1]) * sizeof(char));
	strcpy((*foods)[*number_of_foods - 1].name, user_input[1]);

	(*foods)[*number_of_foods - 1].id = atoi(user_input[2]);

	(*foods)[*number_of_foods - 1].price = atoi(user_input[3]);

	(*foods)[*number_of_foods - 1].capacity = atoi(user_input[4]);

	(*foods)[*number_of_foods - 1].hall_id = atoi(user_input[5]);

	(*foods)[*number_of_foods - 1].day = atoi(user_input[6]);
}


int check_add_food(char** user_input, int number_of_words, Food** foods ,Hall* halls,int* number_of_foods, int number_of_halls, int login_status) {
	if (strcmp(user_input[0], "add_food") != 0) {
		return 0;
	}
	if (number_of_words != ADD_FOOD_WORDS_NEEDED) {//user using add_food but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (login_status != -1) {
		if (login_status == 0) {
			printf("access denied\n");
		}
		else {
			printf("Permission denied\n");
		}
		return 1;
	}
	for (int i = 0; i < *number_of_foods; i++) {//check if food_id is unique
		if ((*foods)[i].id == atoi(user_input[2])) {
			printf("Food_id is not unique\n");
			return 1;
		}
	}

	//check if day and hall_id is valid below

	int check_halls_id = 0;//if the hall_id is found,it gets 1
	for (int i = 0; i < number_of_halls; i++) {
		if (halls[i].id == atoi(user_input[5])) {
			check_halls_id = 1;
			break;
		}
	}
	int check_correct_day = 0;//if the day is correct,it gets 1
	int check_day = atoi(user_input[6]);//to make the input int
	if (check_day > 0 && check_day < 8) {
		check_correct_day = 1;
	}
	if (check_halls_id == 0) {
		printf("wrong hall_id\n");
		return 1;
	}
	if (check_correct_day == 0) {
		printf("Please enter a day between 1 and 7\n");
		return 1;
	}

	append_new_food(user_input);
	put_new_food_in_foods_list(user_input, foods, number_of_foods);//using this func to prevent opening file again
	printf("done\n");
	return 1;
}

void append_new_coupon(char** user_input) {
	FILE* coupons;
	coupons = fopen(COUPONS_FILE_PATH, "a+");
	if (!coupons) {
		printf("coupons File Didnt open");
	}
	if (fgetc(coupons) != EOF) {
		fseek(coupons, -1, SEEK_CUR);//we move forward to check, now we move backward
		fputc('\n', coupons);
	}//we dont need \n if no users exist
	fprintf(coupons, "%s ", user_input[1]);//id added
	fprintf(coupons, "%s ", user_input[2]);//percentage added
	fprintf(coupons, "%s", user_input[3]);//capacity added(didnt put space)
	fclose(coupons);
}

void put_new_coupon_in_coupons_list(char** user_input, Coupon** coupons, int* number_of_coupons) {
	//here we dont need so many reallocs cause we have the user input length from before
	(*number_of_coupons)++;
	*coupons = (Coupon*)realloc(*coupons, *number_of_coupons * sizeof(Coupon));

	(*coupons)[*number_of_coupons - 1].id = atoi(user_input[1]);

	(*coupons)[*number_of_coupons - 1].percentage = atoi(user_input[2]);

	(*coupons)[*number_of_coupons - 1].capacity = atoi(user_input[3]);
}

int check_add_coupon(char** user_input, int number_of_words, Coupon** coupons, int* number_of_coupons, int login_status) {
	if (strcmp(user_input[0], "add_coupon") != 0) {
		return 0;
	}
	if (number_of_words != ADD_COUPON_WORDS_NEEDED) {//user using add_coupon but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (login_status != -1) {
		if (login_status == 0) {
			printf("access denied\n");
		}
		else {
			printf("Permission denied\n");
		}
		return 1;
	}
	for (int i = 0; i < *number_of_coupons; i++) {//check if coupon_id is unique
		if ((*coupons)[i].id == atoi(user_input[1])) {
			printf("coupon_id is not unique\n");
			return 1;
		}
	}

	int check_correct_percentage = 0;//if the percentage is correct,it gets 1 (between 0 to 100
	int check_percentage = atoi(user_input[2]);//to make the input int
	if (check_percentage > 0 && check_percentage <= 100) {
		check_correct_percentage = 1;
	}
	
	if (check_correct_percentage == 0) {
		printf("Please enter a percentage between 0 and 100(0 is not accepted as well)\n");
		return 1;
	}

	append_new_coupon(user_input);
	put_new_coupon_in_coupons_list(user_input, coupons, number_of_coupons);//using this func to prevent opening file again
	printf("done\n");
	return 1;
}

int main() {
	FILE* users;
	users = fopen(USERS_FILE_PATH, "a+");
	if (!users) {//case of error
		printf("Users File Did'nt open\n");
		return 0;
	}
	FILE* halls_file;
	halls_file = fopen(HALLS_FILE_PATH, "a+");
	if (!halls_file) {//case of error
		printf("Halls File Did'nt open\n");
		return 0;
	}
	FILE* foods_file;
	foods_file = fopen(FOODS_FILE_PATH, "a+");
	if (!foods_file) {//case of error
		printf("Foods File Did'nt open\n");
		return 0;
	}
	FILE* coupons_file;
	coupons_file = fopen(COUPONS_FILE_PATH, "a+");
	if (!coupons_file) {//case of error
		printf("Coupons File Did'nt open\n");
		return 0;
	}
	int number_of_students = 0;
	int number_of_halls = 0;
	int number_of_foods = 0;
	int number_of_coupons = 0;
	//we put all of this NULL cause we want realloc at first
	Student* students=NULL;
	Hall* halls=NULL;
	Food* foods=NULL;
	Coupon* coupons=NULL;
	put_users_in_array(users, &students, &number_of_students);//reading users from the file
	put_halls_in_array(halls_file,&halls,&number_of_halls);//reading halls from the file
	put_foods_in_array(foods_file, &foods, &number_of_foods);//reading foods from the file
	put_coupons_in_array(coupons_file, &coupons, &number_of_coupons);//reading coupons from the file
	fclose(users);
	fclose(halls_file);
	fclose(foods_file);
	fclose(coupons_file);

	//the user starts doing
	int login_status=0;//it will change to the number of student from the list that logins(like when student 5th in list logins it get 5) and -1 for admin
	int admin_permissions = 0;//if admin logins it get 1 and we give permissions
	while (true) {
		char** user_input;
		int number_of_words;
		get_input(&user_input,&number_of_words);//save user input in user_input
		int check[6] = { 0 };//to see if all of them is 0, its invalid command
		check[0] = check_login(user_input, number_of_words, students, number_of_students, &login_status);
		check[1] = check_signup(user_input, number_of_words, &students, &number_of_students,login_status);//we give &students cause we want realloc the student in signup
		check[2] = check_logout(user_input, number_of_words ,&login_status);
		check[3] = check_add_hall(user_input, number_of_words, &halls, &number_of_halls, login_status);
		check[4] = check_add_food(user_input, number_of_words, &foods, halls, &number_of_foods, number_of_halls, login_status);
		check[5] = check_add_coupon(user_input, number_of_words, &coupons, &number_of_coupons, login_status);
		if (check[0] == 0&& check[1] == 0 && check[2] == 0 && check[3] == 0 && check[4] == 0 && check[5] == 0) {
			printf("invalid command\n");
		}
		delete_user_input(&user_input, number_of_words);//free the memory allocated for user_input
	}
}