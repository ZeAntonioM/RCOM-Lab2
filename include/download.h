#define DEAFAULT_USERNAME "anonymous"
#define DEAFAULT_PASSWORD "anonymous"

#define BUFFER_SIZE 1024
#define DOWNLOAD_DIR "downloads/"

#define LOGED_IN_CODE 220
#define PASSWORD_CODE 331
#define LOGIN_SUCCESSFULL_CODE 230
#define PASSIVE_MODE_CODE 227
#define START_TRANSFER_CODE 150
#define TRANSFER_COMPLETED_CODE 226
#define N_TRIES 3

#define AT              "%*[^/]//%s@"
#define BAR             "/"
#define HOST_REGEX      "%*[^/]//%[^/]"
#define HOST_AT_REGEX   "%*[^/]//%*[^@]@%[^/]"
#define RESOURCE_REGEX  "%*[^/]//%*[^/]/%s"
#define FILENAME_REGEX  "[^/]+%$"
#define USER_REGEX      "%*[^/]//%[^:/]"
#define PASS_REGEX      "%*[^/]//%*[^:]:%[^@\n$]"
#define RESPCODE_REGEX  "%d"
#define PASSIVE_REGEX   "%*[^(](%d,%d,%d,%d,%d,%d)%*[^\n$)]"



int parse_url(char * url, char * username, char * password, char * host, char * path);

int create_socket(char * host, char * port);

int login(int socket, char * username, char * password );

int setPassiveMode(const int socket, char *ip, char *port) ;

int transfer_request(int socket, char * path);

int get_file(int socket, char * filename);

int read_response_code(int socket);

int read_response (int socket, char * response, int response_len);
