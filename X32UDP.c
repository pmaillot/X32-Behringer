/*
 * X32UDP.c
 *	(POSIX compliant version, Linux...)
 *  Created on: June 2, 2015
 *      Author: Patrick-Gilles Maillot
 *
 *      Copyright 2015, Patrick-Gilles Maillot
 *      This software is distributed under he GNU GENERAL PUBLIC LICENSE.
 *
 *      This software allows connecting to a remote X32 or XAIR system using
 *      UDP protocol; It provides a set of connect, send and receive functions.
 *      The receive mode is non-blocking, i.e. a timeout enables returning from
 *      the call even if no response is obtained by the server.
 *
 *      Send and Receive buffers are provided by the caller. No provision is
 *      made in this package to keep or buffer data for deferred action or
 *      transfers.
 *
 *      Note: This package was updated to use select() timeouts rather than
 *      calls to poll() to handle non-blocking IO.
 */

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//
#define BSIZE	512		// MAX receive buffer size
//
struct sockaddr_in	Xip;
struct sockaddr*	Xip_addr = (struct sockaddr *)&Xip;
socklen_t			Xip_len = sizeof(Xip);		// length of addresses
int 				Xfd;						// X32 socket
fd_set 				ufd;
//
int		r_len, p_status;	// length and status for receiving
//
//
int X32Connect(char *Xip_str, char *Xport_str) {
//
// Initialize communication with X32 server at IP ip and PORT port//
// Load the X32 address we connect to; we're a client to X32, keep it simple.
//
// Input: String - Pointer to IP in the form "123.123.123.123"
// Input: String - Pointer to destination port in the form "12345"
//
// Returns int:
// -3 : Error on Sending data
// -2 : Socket creation error
// -1 : Error on polling for data
//  0 : No error, no connection (timeout)
//  1 : Connected (connection validated with X32)
//
char	r_buf[128];			// receive buffer for /info command test
char	Info[8] = "/info";	// testing connection with /info request
struct timeval		timeout;

//
// Create UDP socket
	if ((Xfd = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		return -2; // An error occurred on socket creation
	}
// Server sockaddr_in structure
	memset (&Xip, 0, sizeof(Xip));				// Clear structure
	Xip.sin_family = AF_INET;					// Internet/IP
	Xip.sin_addr.s_addr = inet_addr(Xip_str);	// IP address
	Xip.sin_port = htons(atoi(Xport_str));		// server port
//
// Validate connection by sending a /info command
	if (sendto (Xfd, Info, 8, 0, Xip_addr, Xip_len) < 0) {
		return (-3);
	}
//register for receiving X32 console updates
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000; //Set timeout for non blocking recvfrom(): 500ms
	FD_ZERO (&ufd);
	FD_SET (Xfd, &ufd);							// X32 sent something?
	if ((p_status = select(Xfd+1, &ufd, NULL, NULL, &timeout)) > 0) {
		r_len = recvfrom(Xfd, r_buf, 128, 0, 0, 0);	// Get answer and
		if ((strncmp(r_buf, Info, 5)) == 0) {		// test data (5 bytes)
			return 1;	// Connected
		}
	} else if (p_status < 0) {
		return -1; // Error on reading (not connected)
	}
// Not connected on timeout after 500ms
	return 0;
}

int X32Send(char *buffer, int lentgh) {
//
// Sends data to X32 server at IP and PORT previously set
// with X32Connect()
//
// Input: String - Pointer to data buffer to send
// Input: integer - Length of data in bytes
//
// Returns int:
//   -1 : Error on Sending data
// >= 0 : Actual length of data sent, no error
//
// Just send data
	return (sendto (Xfd, buffer, lentgh, 0, Xip_addr, Xip_len));
}


int X32Recv(char *buffer, int time_out) {
//
// Receives data from X32 server
//
// Input: String - Pointer to buffer to save data to,
//                 should be capable of receiving 512 bytes
// Input: integer - time_out (in us) for receiving data:
//					depending on systems capabilities, positive and non-zero
//					values may also result in no data (start with a value of 10ms)
//
// Returns int:
// -1 : Error on reading data
//  0 : No error, timeout reached or no data
// >0 : data length in the buffer
//
	struct timeval		timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = time_out; //Set timeout for non blocking recvfrom()
	FD_ZERO (&ufd);
	FD_SET (Xfd, &ufd);							// X32 sent something?
	if ((p_status = select(Xfd+1, &ufd, NULL, NULL, &timeout)) > 0) {
		return recvfrom(Xfd, buffer, BSIZE, 0, 0, 0);// return length
	} else if (p_status < 0) {
		return -1; //An error occurred on polling
	}
	return 0; // No error, timeout
}


//
// Test purpose only - comment when linking the package to an application
//
#include <stdio.h>
int main() {

	char 	r_buf[512];
	char	s_buf[] = {"/status\0"};
	int		r_len = 0;
	int		s_len = 0;
	int		ten_mills = 10000;
	int		status;

	status = X32Connect("192.168.0.64", "10023");
	printf ("Connection status: %d\n", status);

	if (status) {
		s_len = X32Send(s_buf, 8);
		printf ("Send status: %d\n", s_len);
		if (s_len) {
			if ((r_len = X32Recv(r_buf, ten_mills)) >= 0) {
				printf ("Recv status: %d\n", r_len);
				s_len = 0;
				while (r_len--) {
					if (r_buf[s_len] < ' ') putchar('~');
					else putchar(r_buf[s_len]);
					s_len++;
				}
			} else {
				printf ("Recv error: %d\n", r_len);
			}
		}
	}
	return 0;
}


