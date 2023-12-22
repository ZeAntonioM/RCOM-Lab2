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
#define HOST_REGEX      "%*[^/]//%[^/]"
#define HOST_AT_REGEX   "%*[^/]//%*[^@]@%[^/]"
#define PATH_REGEX  "%*[^/]//%*[^/]/%s"
#define USER_REGEX      "%*[^/]//%[^:/]"
#define PASS_REGEX      "%*[^/]//%*[^:]:%[^@\n$]"
#define RESPCODE_REGEX  "%d"
#define PASSIVE_REGEX   "%*[^(](%d,%d,%d,%d,%d,%d)%*[^\n$)]"


/** Parse URL and extract components
 * 
 * @param url char* URL to parse
 * @param username char* Variable where the username will be stored
 * @param password char* Variable where the password will be stored
 * @param host char* Variable where the host will be stored
 * @param path char* Variable where the path/resource will be stored
 * @return int Returns 0 on success, -1 on failure
 */
int parse_url(char * url, char * username, char * password, char * host, char * path);

/** Create a socket and connect to the server
 * 
 * @param host char* The host to connect to
 * @param port char* The port to connect to
 * @return int Returns the socket file descriptor on success, 1 on failure
 */
int create_socket(char * host, char * port);

/** Log in to the FTP server
 * 
 * @param socket int The socket file descriptor
 * @param username char* The username for login
 * @param password char* The password for login
 * @return int Returns 0 on success, 1 on failure
 */
int login(int socket, char * username, char * password);

/** Set passive mode for data transfer
 * 
 * @param socket int The socket file descriptor
 * @param ip char* Variable where the IP address for passive mode will be stored
 * @param port char* Variable where the port for passive mode will be stored
 * @return int Returns 0 on success, 1 on failure
 */
int setPassiveMode(const int socket, char *ip, char *port);

/** Send a transfer request to the server
 * 
 * @param socket int The socket file descriptor
 * @param path char* The path/resource to request
 * @return int Returns 0 on success, 1 on failure
 */
int transfer_request(int socket, char * path);

/** Download a file from the server
 * 
 * @param socket int The socket file descriptor for data transfer
 * @param filename char* The name of the file to be downloaded
 * @return int Returns 0 on success, 1 on failure
 */
int get_file(int socket, char * filename);

/** Read the response code from the server
 * 
 * @param socket int The socket file descriptor
 * @return int Returns the response code on success, 1 on failure
 */
int read_response_code(int socket);

/** Read the response from the server
 * 
 * @param socket int The socket file descriptor
 * @param response char* Variable where the response will be stored
 * @param response_len int The length of the response buffer
 * @return int Returns 0 on success, 1 on failure
 */
int read_response(int socket, char * response, int response_len);