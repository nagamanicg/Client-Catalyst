#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>

#define PORT 80       	// HTTP typically runs on port 80
#define MAX_BUFFER 4096   // Buffer size for storing request and response data

// Function to handle errors and exit the program
void handle_error(const char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}

// Function to Base64-encode a string (for Basic Authentication)
char *base64_encode(const char *input) {
	static char encoded[256];  // Buffer to store the Base64-encoded output
	const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int input_length = strlen(input);
	int i = 0, j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (input_length--) {
    	char_array_3[i++] = *(input++);
    	if (i == 3) {
        	char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        	char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + (char_array_3[1] >> 4);
        	char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + (char_array_3[2] >> 6);
        	char_array_4[3] = char_array_3[2] & 0x3f;

        	for (i = 0; i < 4; i++)
            	encoded[j++] = base64_chars[char_array_4[i]];

        	i = 0;
    	}
	}

	// Handle remaining characters (if any)
	if (i) {
    	for (int k = i; k < 3; k++)
        	char_array_3[k] = '\0';

    	char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    	char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + (char_array_3[1] >> 4);
    	char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + (char_array_3[2] >> 6);
    	char_array_4[3] = char_array_3[2] & 0x3f;

    	for (int k = 0; k < i + 1; k++)
        	encoded[j++] = base64_chars[char_array_4[k]];
	}

	encoded[j] = '\0';
	return encoded;
}

int main() {
	char server_ip[256]; 	// IP address of the server
	char auth[256];     	// Username and password for Basic Authentication
	char webpage[256];  	// URL path to the desired resource
	char cookie_value[256];  // Cookie value for the request

	// Step 1: Get the server IP address from the user
	printf("Enter the server IP or hostname: ");
	fgets(server_ip, sizeof(server_ip), stdin);
	server_ip[strcspn(server_ip, "\n")] = '\0';  // Remove trailing newline

	// Step 2: Get authentication details from the user
	printf("Enter the username:password for Basic Authentication: ");
	fgets(auth, sizeof(auth), stdin);
	auth[strcspn(auth, "\n")] = '\0';

	// Step 3: Get the webpage/resource URL
	printf("Enter the webpage or resource URL (e.g., /auth_cookie_test.php): ");
	fgets(webpage, sizeof(webpage), stdin);
	webpage[strcspn(webpage, "\n")] = '\0';

	// Step 4: Get the cookie value
	printf("Enter the cookie value to set (e.g., test_cookie=myvalue): ");
	fgets(cookie_value, sizeof(cookie_value), stdin);
	cookie_value[strcspn(cookie_value, "\n")] = '\0';

	int sockfd;
	struct sockaddr_in server_addr;
	char request[MAX_BUFFER];   // Buffer to store the HTTP request
	char response[MAX_BUFFER];  // Buffer to store the server response

	// Step 5: Create a TCP socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    	handle_error("Socket creation failed");

	// Step 6: Set up the server address structure
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	// Convert the server IP address to a usable format
	if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    	handle_error("Invalid server IP address");

	// Set a timeout of 5 seconds for socket operations
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    	handle_error("Failed to set socket timeout");

	// Step 7: Connect to the server
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    	handle_error("Failed to connect to the server");

	printf("Connected to server %s\n", server_ip);

	// Step 8: Encode authentication information in Base64
	char auth_encoded[256];
	snprintf(auth_encoded, sizeof(auth_encoded), "%s", base64_encode(auth));

	// Step 9: Construct the HTTP GET request
	snprintf(request, sizeof(request),
         	"GET %s HTTP/1.1\r\n"
         	"Host: %s\r\n"
         	"User-Agent: MyCClient/1.0\r\n"
         	"Accept-Encoding: gzip, deflate\r\n"
         	"Authorization: Basic %s\r\n"
         	"Cookie: %s\r\n"
         	"Connection: close\r\n"
         	"\r\n",
         	webpage, server_ip, auth_encoded, cookie_value);

	// **Debug: Print the request before sending it**
	printf("\n--- HTTP Request Sent to Server ---\n");
	printf("%s", request);
	printf("--- End of Request ---\n\n");

	// Step 10: Send the request to the server
	if (send(sockfd, request, strlen(request), 0) < 0)
    	handle_error("Failed to send request");

	printf("Request successfully sent to server.\n\n");

	// **Receive and print the server response**
	int bytes_received;
	printf("--- Server Response Start ---\n");

	while ((bytes_received = recv(sockfd, response, MAX_BUFFER - 1, 0)) > 0) {
    	response[bytes_received] = '\0';
    	printf("%s", response);
	}
printf("--- Server Response End ---\n");
	if (bytes_received == 0) {
    	printf("\nServer has closed the connection.\n");
	} else if (bytes_received < 0) {
    	printf("\nError receiving data from the server.\n");
    	perror("recv()");
	}

	

	// Step 11: Close the socket connection
	close(sockfd);
	return 0;
}

