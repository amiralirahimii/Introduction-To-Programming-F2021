#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include<stdlib.h>
#include<string.h>

#define USERS_FILE_PATH "users.txt"
#define FOOD_REPORT_FILE_PATH "food_report.txt"
#define HALL_REPORT_FILE_PATH "hall_report.txt"

#define STUDENT_ID_SIZE 9
#define LOGIN_WORDS_NEEDED 3
#define SIGNUP_WORDS_NEEDED 4
#define LOGOUT_WORDS_NEEDED 1
#define ADD_HALL_WORDS_NEEDED 4
#define ADD_FOOD_WORDS_NEEDED 7
#define ADD_COUPON_WORDS_NEEDED 4
#define FOOD_REPORT_WORDS_NEEDED 1
#define HALL_REPORT_WORDS_NEEDED 1
#define GET_MONEY_WORDS_NEEDED 2
#define SHOW_MONEY_WORDS_NEEDED 1
#define MENU_WORDS_NEEDED 1
#define RESERVE_WORDS_NEEDED 2
#define RESERVE_DISCOUNT_WORDS_NEEDED 3
#define SHOW_RESERVED_WORDS_NEEDED 1
#define EXIT_WORDS_NEEDED 1
#define ALL_ORDER_NUMBERS 15

typedef struct Hall {
	char* name;
	int id;
	int capacity;
	int reserved[7];//for each day
	int profit[7];//for each day
	Hall* next;
}Hall;

typedef struct Food {
	char* name;
	int id;
	int price;
	int capacity;
	Hall* hall;//for save which hall is this food serving in(hall_id saves in here)
	int day;
	int reserved;
	int profit;
	Food* next;
}Foods;

typedef struct Food_reserve {
	Food* food;
	Food_reserve* next;
}Food_reserve;

typedef struct Student {
	char* username;
	int student_id;
	char* password;
	int money;
	Food_reserve *food_reserved[7];//we want to save the food which user resrves
	Student* next;
}Student;

typedef struct Coupon {
	int id;
	int percentage;
	int capacity;
	int reserved;
	Coupon* next;
}Coupon;


void put_users_in_list(FILE* users, Student** head_of_students) {
	if (fgetc(users) == EOF) {//if the file is empty, it returns back
		return;
	}
	else {//we find out there is 1 student at least and we resume code
		fseek(users, 0, SEEK_SET);//cause fgetc at first and it seeked 1 forward and we start again
	}
	Student* students;//a temperory variable to add this student in the list
	Student* head = *head_of_students;//locate where we are at in list
	while (true) {
		students = (Student*)malloc(1 * sizeof(Student));
		int size_of_username = 1;
		students->username = (char*)malloc(size_of_username * sizeof(char));//here we malloc 1 and see if it is more,we realloc
		char c;
		while ((c = fgetc(users)) != ' ') {//reading username
			students->username[size_of_username - 1] = c;
			size_of_username++;
			students->username = (char*)realloc(students->username, size_of_username * sizeof(char));
		}
		students->username[size_of_username - 1] = '\0';

		char* id = (char*)malloc((STUDENT_ID_SIZE + 1) * sizeof(char));//we know the student_id is a 9 digit number and we use +1 cause we need '\0' at the end
		int i = 0;
		while ((c = fgetc(users)) != ' ') {
			id[i] = c;
			i++;
		}
		id[i] = '\0';
		students->student_id = atoi(id);
		free(id);//we dont need id any more

		int size_of_password = 1;
		students->password = (char*)malloc(size_of_password * sizeof(char));
		while ((c = fgetc(users)) != '\n' && c != EOF) {
			students->password[size_of_password - 1] = c;
			size_of_password++;
			students->password = (char*)realloc(students->password, size_of_password * sizeof(char));
		}
		students->password[size_of_password - 1] = '\0';
		students->money = 0;
		for (int i = 0; i < 7; i++) {
			students->food_reserved[i] = NULL;
		}

		if (*head_of_students == NULL) {
			*head_of_students = students;
			head = *head_of_students;
		}
		else {
			head->next = students;
			head = head->next;
		}
		if (c == EOF) {
			head->next = NULL;
			break;
		}
	}
}

void get_input(char*** user_input, int* number_of_words) { //this function read user input
	char c;//to read char by char from user
	int i = 0;//to see which word we are in (ex:word number 1)
	int j = 0;//to see which element of the word we are in(ex: element 3 of word i)
	*user_input = (char**)malloc((i + 1) * sizeof(char*));
	(*user_input)[i] = (char*)malloc((j + 1) * sizeof(char));
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
	*number_of_words = i + 1;//cause i use i=0 for student 1;
}

void delete_user_input(char*** user_input, int number_of_words) {
	for (int i = 0; i < number_of_words; i++) {//no need to change number of words because it will be changed automaticly in next loop
		free((*user_input)[i]);
	}
	free(*user_input);
}

int check_login(char** user_input, int number_of_words, Student* head_of_students, Student**login, int* login_status) {
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

	if (strcmp(user_input[1], "admin") == 0 && strcmp(user_input[2], "admin") == 0) {
		printf("Welcome admin\n");
		*login_status = -1;
		return 1;
	}

	Student* head = head_of_students;
	while (head != NULL) {
		if (strcmp(user_input[1], head->username) == 0 && strcmp(user_input[2], head->password) == 0) {
			printf("welcome %s\n", user_input[1]);
			*login_status = 1;
			*login = head;
			return 1;
		}
		head = head->next;
	}
	printf("wrong username or password\n");
}

int append_new_user(char** user_input) {
	FILE* users;
	users = fopen(USERS_FILE_PATH, "a+");
	if (!users) {
		printf("Users File Didnt open\n");
		return 0;
	}
	if (fgetc(users) != EOF) {
		fseek(users, -1, SEEK_CUR);//we move forward to check, now we move backward
		fputc('\n', users);
	}
	fprintf(users, "%s ", user_input[1]);//username added
	fprintf(users, "%s ", user_input[2]);//student_id added
	fprintf(users, "%s", user_input[3]);//password added(didnt put space)
	fclose(users);
	return 1;
}

void put_new_user_in_students_list(char** user_input, Student** head_of_students) {
	Student* append = (Student*)malloc(1 * sizeof(Student));

	append->username = (char*)malloc(strlen(user_input[1]) * sizeof(char));
	strcpy(append->username, user_input[1]);
	append->student_id = atoi(user_input[2]);
	append->password = (char*)malloc(strlen(user_input[3]) * sizeof(char));
	strcpy(append->password, user_input[3]);
	append->money = 0;
	for (int i = 0; i < 7; i++) {
		append->food_reserved[i] = NULL;
	}
	append->next = NULL;

	if (*head_of_students == NULL) {
		*head_of_students = append;
	}
	else {
		Student* head = *head_of_students;
		while (head->next != NULL) {
			head = head->next;
		}
		head->next = append;
	}
}

int check_signup(char** user_input, int number_of_words, Student** head_of_students, int login_status) {
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

	Student* head = *head_of_students;
	while (head != NULL) {
		if (strcmp(user_input[1], head->username) == 0) {
			printf("user already exists\n");
			return 1;
		}
		head = head->next;
	}
	int check;//if file donot open it will not print done
	check = append_new_user(user_input);
	put_new_user_in_students_list(user_input, head_of_students);//using this func to prevent opening file again
	if (check) {
		printf("done\n");
	}
	return 1;
}

int check_logout(char** user_input, int number_of_words, int* login_status, Student** login) {
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
	login = NULL;
	printf("done\n");
}

void put_new_hall_in_halls_list(char** user_input, Hall** head_of_halls) {
	//here we dont need so many reallocs cause we have the user input length from before
	Hall* append = (Hall*)malloc(1 * sizeof(Hall));

	append->name = (char*)malloc(strlen(user_input[1]) * sizeof(char));
	strcpy(append->name, user_input[1]);
	append->id = atoi(user_input[2]);
	append->capacity = atoi(user_input[3]);
	for (int i = 0; i < 7; i++) {
		append->reserved[i] = 0;
		append->profit[i] = 0;
	}
	append->next = NULL;

	if (*head_of_halls == NULL) {
		*head_of_halls = append;
	}
	else {
		Hall* head = *head_of_halls;
		while (head->next != NULL) {
			head = head->next;
		}
		head->next = append;
	}
}

int check_add_hall(char** user_input, int number_of_words, Hall** head_of_halls, int login_status) {
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

	Hall* head = *head_of_halls;
	while (head != NULL) {
		if (atoi(user_input[2]) == head->id) {
			printf("hall_id is not unique\n");
			return 1;
		}
		head = head->next;
	}

	put_new_hall_in_halls_list(user_input, head_of_halls);//using this func to prevent opening file again
	printf("done\n");
	return 1;
}

void put_new_food_in_foods_list(char** user_input, Food** head_of_foods,Hall* save_hall) {
	//here we dont need so many reallocs cause we have the user input length from before
	Food* append = (Food*)malloc(1 * sizeof(Food));

	append->name = (char*)malloc(strlen(user_input[1]) * sizeof(char));
	strcpy(append->name, user_input[1]);
	append->id = atoi(user_input[2]);
	append->price = atoi(user_input[3]);
	append->capacity = atoi(user_input[4]);
	append->hall = save_hall;//user input[5] is here(contains all the hall information)
	append->day = atoi(user_input[6]);
	append->reserved= 0;//when admin add it, its new food and noone ordered it before
	append->profit = 0;//when admin add it, its new food and noone ordered it before
	append->next = NULL;

	if (*head_of_foods == NULL) {
		*head_of_foods = append;
	}
	else {
		Food* head = *head_of_foods;
		while (head->next != NULL) {
			head = head->next;
		}
		head->next = append;
	}
}

int check_add_food(char** user_input, int number_of_words, Food** head_of_foods, Hall* head_of_halls, int login_status) {
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

	Food* head_food = *head_of_foods;
	while (head_food != NULL) {
		if (atoi(user_input[2]) == head_food->id) {
			printf("Food_id is not unique\n");
			return 1;
		}
		head_food = head_food->next;
	}

	//check if day and hall_id is valid below

	int check_halls_id = 0;//if the hall_id is found,it gets 1
	Hall* head_hall = head_of_halls;
	Hall* save_hall = NULL;
	while (head_hall != NULL) {
		if (atoi(user_input[5]) == head_hall->id) {
			check_halls_id = 1;
			save_hall = head_hall;
			break;
		}
		head_hall = head_hall->next;
	}

	int check_correct_day = 0;//if the day is correct,it gets 1
	int check_day = atoi(user_input[6]);//to make the input int
	if (check_day > 0 && check_day < 8) {
		check_correct_day = 1;
	}
	if (check_halls_id == 0) {
		printf("hall_id do not exists\n");
		return 1;
	}
	if (check_correct_day == 0) {
		printf("Please enter a day between 1 and 7\n");
		return 1;
	}

	put_new_food_in_foods_list(user_input, head_of_foods,save_hall);//using this func to prevent opening file again
	printf("done\n");
	return 1;
}

void put_new_coupon_in_coupons_list(char** user_input, Coupon** head_of_coupons) {
	Coupon* append = (Coupon*)malloc(1 * sizeof(Coupon));

	append->id = atoi(user_input[1]);
	append->percentage = atoi(user_input[2]);
	append->capacity = atoi(user_input[3]);
	append->reserved = 0;
	append->next = NULL;

	if (*head_of_coupons == NULL) {
		*head_of_coupons = append;
	}
	else {
		Coupon* head = *head_of_coupons;
		while (head->next != NULL) {
			head = head->next;
		}
		head->next = append;
	}
}

int check_add_coupon(char** user_input, int number_of_words, Coupon** head_of_coupons, int login_status) {
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

	Coupon* head = *head_of_coupons;
	while (head != NULL) {//check if coupon_id is unique
		if (atoi(user_input[1]) == head->id) {
			printf("coupon_id is not unique\n");
			return 1;
		}
		head = head->next;
	}

	int check_correct_percentage = 0;//if the percentage is correct,it gets 1 (between 0 to 100)
	int check_percentage = atoi(user_input[2]);//to make the input int
	if (check_percentage > 0 && check_percentage <= 100) {
		check_correct_percentage = 1;
	}

	if (check_correct_percentage == 0) {
		printf("Please enter a percentage between 0 and 100(0 is not accepted as well)\n");
		return 1;
	}

	put_new_coupon_in_coupons_list(user_input, head_of_coupons);//using this func to prevent opening file again
	printf("done\n");
	return 1;
}

int check_food_report(char** user_input, int number_of_words,Food* head_of_foods, int login_status) {
	if (strcmp(user_input[0], "food_report") != 0) {
		return 0;
	}
	if (number_of_words != FOOD_REPORT_WORDS_NEEDED) {//admin using food_report but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (login_status != -1) {
		if (login_status == 0) {
			printf("access denied\n");
		}
		else {
			printf("permission denied\n");
		}
		return 1;
	}
	Food* head = head_of_foods;
	if (head == NULL) {
		printf("there's no food available\n");
		return 1;
	}
	FILE* food_report = fopen(FOOD_REPORT_FILE_PATH, "w");
	if (!food_report) {
		printf("food_report file didn't open\n");
		return 1;
	}
	while (head != NULL) {
		fprintf(food_report, "%d %d %d %d\n", head->id, head->capacity, head->reserved, head->profit);
		head = head->next;
	}
	fclose(food_report);
	printf("done\n");
	return 1;
}

int check_hall_report(char** user_input, int number_of_words, Hall* head_of_halls, int login_status) {
	if (strcmp(user_input[0], "hall_report") != 0) {
		return 0;
	}
	if (number_of_words != HALL_REPORT_WORDS_NEEDED) {//admin using hall_report but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (login_status != -1) {
		if (login_status == 0) {
			printf("access denied\n");
		}
		else {
			printf("permission denied\n");
		}
		return 1;
	}
	Hall* head = head_of_halls;
	if (head == NULL) {
		printf("there's no hall\n");
		return 1;
	}
	FILE* hall_report = fopen(HALL_REPORT_FILE_PATH, "w");
	if (!hall_report) {
		printf("hall_report file didn't open\n");
		return 1;
	}
	while (head != NULL) {
		fprintf(hall_report, "%d %s %d\n", head->id, head->name, head->capacity);
		for (int i = 0; i < 7; i++) {
			fprintf(hall_report, "%d %d %d\n", i+1, head->reserved[i], head->profit[i]);
		}
		head = head->next;
	}
	fclose(hall_report);
	printf("done\n");
	return 1;
}

int check_get_money(char** user_input, int number_of_words, Student** login, int login_status) {
	if (strcmp(user_input[0], "get_money") != 0) {
		return 0;
	}
	if (number_of_words != GET_MONEY_WORDS_NEEDED) {//user using get_money but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (login_status != 1) {
		if (login_status == 0) {
			printf("access denied\n");
		}
		else {
			printf("Admin is not able to use get_money\n");
		}
		return 1;
	}
	(*login)->money += atoi(user_input[1]);
	printf("done\n");
	return 1;
}

int check_show_money(char** user_input, int number_of_words, Student** login, int login_status) {
	if (strcmp(user_input[0], "show_money") != 0) {
		return 0;
	}
	if (number_of_words != SHOW_MONEY_WORDS_NEEDED) {//user using get_money but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (login_status != 1) {
		if (login_status == 0) {
			printf("access denied\n");
		}
		else {
			printf("Admin is not able to use show_money\n");
		}
		return 1;
	}
	printf("%d\n", (*login)->money);
	return 1;
}

int check_menu(char** user_input, int number_of_words, Food* head_of_foods, int login_status) {
	if (strcmp(user_input[0], "menu") != 0) {
		return 0;
	}
	if (number_of_words != MENU_WORDS_NEEDED) {//user using menu but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (login_status == 0) {
		printf("access denied\n");
		return 1;
	}

	for (int i = 1; i <= 7; i++) {
		Food* head = head_of_foods;
		printf("%d:\n", i);
		while (head != NULL) {
			if (head->day == i) {
				int amount_left = head->capacity - head->reserved;
				printf("%s %d %d %s %d\n", head->name, head->price, head->id, head->hall->name, amount_left);
			}
			head = head->next;
		}
	}
	return 1;
}

void save_in_show_reserved(Student* login, Food* head) {
	Food_reserve* append = (Food_reserve*)malloc(1 * sizeof(Food_reserve));
	append->food = head;
	append->next = NULL;
	if (login->food_reserved[head->day - 1] == NULL) {
		login->food_reserved[head->day - 1] = append;
	}
	else {
		Food_reserve* temp = login->food_reserved[head->day - 1];
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = append;
	}
}

int check_reserve(char** user_input, int number_of_words, Food* head_of_foods, int login_status, Student* login) {
	if (strcmp(user_input[0], "reserve") != 0) {
		return 0;
	}
	if (number_of_words != RESERVE_WORDS_NEEDED) {//user using reserve but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (login_status != 1) {
		if (login_status == 0) {
			printf("access denied\n");
		}
		else {
			printf("Admin is not able to use reserve\n");
		}
		return 1;
	}
	Food* head = head_of_foods;
	while (head != NULL) {
		if (atoi(user_input[1]) == head->id) {
			if (head->reserved == head->capacity)//check if it the food is not fully reserved
				break;
			if (login->money < head->price)//check if user has enough money
				break;
			if (head->hall->reserved[head->day-1] == head->hall->capacity)//check if it the food's hall is not fully reserved(it is head->day-1 because if the day is 2 it should be saved in reserved[1])
				break;
			//if its not breaked, it means everything is fine and user can reserve the food
			login->money -= head->price;
			head->hall->reserved[head->day-1]++;
			head->reserved++;
			head->profit += head->price;
			head->hall->profit[head->day-1] += head->price;
			save_in_show_reserved(login, head);
			printf("done\n");
			return 1;
		}
		head = head->next;
	}
	printf("not possible\n");
	return 1;
}

int check_reserve_discount(char** user_input, int number_of_words, Food* head_of_foods, int login_status, Student* login,Coupon* head_of_coupons) {
	if (strcmp(user_input[0], "reserve_discount") != 0) {
		return 0;
	}
	if (number_of_words != RESERVE_DISCOUNT_WORDS_NEEDED) {//user using reserve_discount but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (login_status != 1) {
		if (login_status == 0) {
			printf("access denied\n");
		}
		else {
			printf("Admin is not able to use reserve_discount\n");
		}
		return 1;
	}
	Coupon* find_coupon = NULL;
	while (head_of_coupons != NULL) {//here it checks if this coupon exists
		if (atoi(user_input[2]) == head_of_coupons->id) {
			find_coupon = head_of_coupons;
			break;
		}
		head_of_coupons = head_of_coupons->next;
	}
	if (find_coupon == NULL) {
		printf("not possible\n");
		return 1;
	}
	double discount = find_coupon->percentage / 100.0;
	
	while (head_of_foods != NULL) {
		if (atoi(user_input[1]) == head_of_foods->id) {
			int food_price_with_discount = (head_of_foods->price) - (int)(discount * (head_of_foods)->price);
			if (head_of_foods->reserved == head_of_foods->capacity)//check if it the food is not fully reserved
				break;
			if (login->money < food_price_with_discount)//check if user has enough money
				break;
			if (head_of_foods->hall->reserved[head_of_foods->day - 1] == head_of_foods->hall->capacity)//check if it the food's hall is not fully reserved
				break;
			if (find_coupon->reserved == find_coupon->capacity) //check if it the coupon is not fully reserved
				break;
			//if its not breaked, it means everything is fine and user can reserve the food
			login->money -= food_price_with_discount;
			head_of_foods->hall->reserved[head_of_foods->day - 1]++;
			head_of_foods->reserved++;
			find_coupon->reserved++;
			head_of_foods->profit += food_price_with_discount;
			head_of_foods->hall->profit[head_of_foods->day - 1] += food_price_with_discount;
			save_in_show_reserved(login, head_of_foods);
			printf("done\n");
			return 1;
		}
		head_of_foods = head_of_foods->next;
	}
	printf("not possible\n");
	return 1;
}

int check_show_reserved(char** user_input, int number_of_words, Student* login,int login_status) {
	if (strcmp(user_input[0], "show_reserved") != 0) {
		return 0;
	}
	if (number_of_words != SHOW_RESERVED_WORDS_NEEDED) {//user using show_reserved but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (login_status != 1) {
		if (login_status == 0) {
			printf("access denied\n");
		}
		else {
			printf("Admin is not able to use show_reserved\n");
		}
		return 1;
	}
	for (int i = 0; i < 7; i++) {
		printf("%d:\n", i + 1);
		Food_reserve* head= login->food_reserved[i];
		while (head != NULL) {
			printf("%s %d %d %s\n", head->food->name, head->food->price, head->food->id, head->food->hall->name);
			head = head->next;
		}
	}
}

int check_exit(char** user_input, int number_of_words, int login_status) {
	if (strcmp(user_input[0], "exit") != 0) {//if returned -1 so we should execute program
		return 0;
	}
	if (number_of_words != EXIT_WORDS_NEEDED) {//user using exit but not in correct format
		printf("incorrect inputs (wrong number of inputs or wrong format)\n");
		return 1;
	}
	if (login_status != -1) {
		if (login_status == 0) {
			printf("access denied\n");
		}
		else {
			printf("permission denied\n");
		}
		return 1;
	}
	return -1;
}

void check_invalid_command(int check[ALL_ORDER_NUMBERS]) {
	int check_all_checks = 0;//it become 1 if one of this checks is 1
	for (int i = 0; i < ALL_ORDER_NUMBERS; i++) {
		if (check[i] == 0) {
			continue;
		}
		check_all_checks = 1;
		break;
	}
	if (check_all_checks == 0) {
		printf("inavlid command\n");
	}
}

int main() {
	FILE* users;
	users = fopen(USERS_FILE_PATH, "a+");
	if (!users) {//case of error
		printf("Users File Did'nt open\n");
		return 0;
	}
	//we put all of this NULL cause we want to know if the list is empty in functions
	Student* head_of_students = NULL;
	Hall* head_of_halls = NULL;
	Food* head_of_foods = NULL;
	Coupon* head_of_coupons = NULL;

	put_users_in_list(users, &head_of_students);//reading users from the file
	fclose(users);

	//the user starts doing
	int login_status = 0;//it will change to 1 in case of not admin login and -1 for admin login
	Student* login = NULL;

	while (true) {
		char** user_input;
		int number_of_words;
		get_input(&user_input, &number_of_words);//save user input in user_input
		int check[ALL_ORDER_NUMBERS] = { 0 };//to see if all of them is 0, its invalid command
		check[0] = check_login(user_input, number_of_words, head_of_students, &login, &login_status);
		check[1] = check_signup(user_input, number_of_words, &head_of_students, login_status);//we give &students cause we want realloc the student in signup
		check[2] = check_logout(user_input, number_of_words, &login_status, &login);
		check[3] = check_add_hall(user_input, number_of_words, &head_of_halls, login_status);
		check[4] = check_add_food(user_input, number_of_words, &head_of_foods, head_of_halls, login_status);
		check[5] = check_add_coupon(user_input, number_of_words, &head_of_coupons, login_status);
		check[6] = check_food_report(user_input, number_of_words, head_of_foods, login_status);
		check[7] = check_hall_report(user_input, number_of_words, head_of_halls, login_status);
		check[8] = check_get_money(user_input, number_of_words, &login, login_status);
		check[9] = check_show_money(user_input, number_of_words, &login, login_status);
		check[10] = check_menu(user_input, number_of_words, head_of_foods, login_status);
		check[11] = check_reserve(user_input, number_of_words, head_of_foods, login_status, login);
		check[12] = check_reserve_discount(user_input, number_of_words, head_of_foods, login_status, login, head_of_coupons);
		check[13] = check_show_reserved(user_input, number_of_words, login, login_status);
		check[14] = check_exit(user_input, number_of_words, login_status);
		if (check[14] == -1)return 0;//in case of using exit
		check_invalid_command(check);//this function print invalid command based of check values(if one of the check array values get 1 ,it means the command was valid)
		delete_user_input(&user_input, number_of_words);//free the memory allocated for user_input
	}
}