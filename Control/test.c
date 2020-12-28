<<<<<<< HEAD
#include "../symtab/jsonapi.h"

int main(int argc, char **argv) {
	
	// convert_object_to_struct_user();
	// convert_object_to_struct_room();
	// convert_object_to_struct_message();
	// convert_object_to_struct_friend();
	User user;
	user.id = json_object_new_int(1);
	user.is_admin = json_object_new_boolean(FALSE);
	user.status = json_object_new_boolean(FALSE);
	user.created_at = json_object_new_string("12121212121");
	user.name = json_object_new_string("Tuan Anh");
	user.username = json_object_new_string("anh.nt");
	user.password = json_object_new_string("1234455");

	// const char *a = convert_object_to_json_user(user);
	// printf("%s", a);
	// user_db test = getUserDB(user);
	// printf("%s", test.username);
	
	user_db userdb;
	int check = check_user("hello");
	if (check==1){
		userdb = getUser("hello");
		printf("%s\n", userdb.username); 
	}
	
	return 1;
=======
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

int main(int argc, char const *argv[]){
CURL *curl;
CURLcode res;
long http_code;

curl = curl_easy_init();
curl_global_init(CURL_GLOBAL_ALL);

if(curl){

    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8000/api/user/2/?status=0");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    


    //enable to spit out information for debugging
    //curl_easy_setopt(curl, CURLOPT_VERBOSE,1L); 

    res = curl_easy_perform(curl);

    if (res != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res) );
    }

    printf("\nget http return code\n");
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("http code: %lu\n", http_code );


    curl_easy_cleanup(curl);
    curl_global_cleanup();

    }
return 0;
>>>>>>> 2e54436a8b7b3146af368fc7ac9ca1d57d04e588
}