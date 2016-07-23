#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "rtp.h"




/* GIVEN Function:
 * Handles creating the client's socket and determining the correct
 * information to communicate to the remote server
 */
CONN_INFO* setup_socket(char* ip, char* port){
	struct addrinfo *connections, *conn = NULL;
	struct addrinfo info;
	memset(&info, 0, sizeof(struct addrinfo));
	int sock = 0;

	info.ai_family = AF_INET;
	info.ai_socktype = SOCK_DGRAM;
	info.ai_protocol = IPPROTO_UDP;
	getaddrinfo(ip, port, &info, &connections);

	/*for loop to determine corr addr info*/
	for(conn = connections; conn != NULL; conn = conn->ai_next){
		sock = socket(conn->ai_family, conn->ai_socktype, conn->ai_protocol);
		if(sock <0){
			if(DEBUG)
				perror("Failed to create socket\n");
			continue;
		}
		if(DEBUG)
			printf("Created a socket to use.\n");
		break;
	}
	if(conn == NULL){
		perror("Failed to find and bind a socket\n");
		return NULL;
	}
	CONN_INFO* conn_info = malloc(sizeof(CONN_INFO));
	conn_info->socket = sock;
	conn_info->remote_addr = conn->ai_addr;
	conn_info->addrlen = conn->ai_addrlen;
	return conn_info;
}

void shutdown_socket(CONN_INFO *connection){
	if(connection)
		close(connection->socket);
}

/* 
 * ===========================================================================
 *
 *			STUDENT CODE STARTS HERE. PLEASE COMPLETE ALL FIXMES
 *
 * ===========================================================================
 */


/*
 *  Returns a number computed based on the data in the buffer.
 */
static int checksum(char *buffer, int length){
  int i, tally = 0;

  for(i = 0; i < length; i++) {
    tally += *(buffer + i);
  }

  return tally;
	/*  ----  FIXME  ----
	 *
	 *  The goal is to return a number that is determined by the contents
	 *  of the buffer passed in as a parameter.  There a multitude of ways
	 *  to implement this function.  For simplicity, simply sum the ascii
	 *  values of all the characters in the buffer, and return the total.
	 */ 
}

/*
 *  Converts the given buffer into an array of PACKETs and returns
 *  the array.  The value of (*count) should be updated so that it 
 *  contains the length of the array created.
 */
static PACKET* packetize(char *buffer, int length, int *count){
  int i, arr_size = length / MAX_PAYLOAD_LENGTH; // set length of packet array
  PACKET *packets, *curr_packet;

  // adjust length of packet array to accomodate a final partially filled packet
  if (length % MAX_PAYLOAD_LENGTH > 0) {
    arr_size++;
  }

  *count = arr_size;  // update count

  packets = calloc(arr_size, sizeof(PACKET));  // allocate space for packet array
  
  // write the message into the packets
   for(i = 0; i < length; i++) {
     curr_packet = packets + (i / MAX_PAYLOAD_LENGTH);  // get current packet
     
     curr_packet->payload[i % MAX_PAYLOAD_LENGTH] = *(buffer + i); // set payload

     // if finishing up a packet then perform housekeeping
     if( i % MAX_PAYLOAD_LENGTH == MAX_PAYLOAD_LENGTH - 1) {
       curr_packet->type = DATA;
       curr_packet->payload_length = MAX_PAYLOAD_LENGTH;
       curr_packet->checksum = checksum(curr_packet->payload, curr_packet->payload_length);
     }
    
     // if on last packet then perform special housekeeping
     if(i == length - 1) {
       curr_packet->type = LAST_DATA;
       curr_packet->payload_length = (length % MAX_PAYLOAD_LENGTH) + 1;
       curr_packet->checksum = checksum(curr_packet->payload, curr_packet->payload_length);
     } 
  }
	/*  ----  FIXME  ----
	 *  The goal is to turn the buffer into an array of packets.
	 *  You should allocate the space for an array of packets and
	 *  return a pointer to the first element in that array.  Each 
	 *  packet's type should be set to DATA except the last, as it 
	 *  should be LAST_DATA type. The integer pointed to by 'count' 
	 *  should be updated to indicate the number of packets in the 
	 *  array.
	 */
   return packets;
}

/*
 * Send a message via RTP using the connection information
 * given on UDP socket functions sendto() and recvfrom()
 */
int rtp_send_message(CONN_INFO *connection, MESSAGE*msg){
	/* ---- FIXME ----
	 * The goal of this function is to turn the message buffer
	 * into packets and then, using stop-n-wait RTP protocol,
	 * send the packets and re-send if the response is a NACK.
	 * If the response is an ACK, then you may send the next one
	 */
}

/*
 * Receive a message via RTP using the connection information
 * given on UDP socket functions sendto() and recvfrom()
 */
MESSAGE* rtp_receive_message(CONN_INFO *connection){
  MESSAGE *msg = calloc(1, sizeof(MESSAGE));
  PACKET *packet =(PACKET*) calloc(1, sizeof(PACKET));
  int i, buff_capacity, curr_checksum;

  msg->buffer = (char*) calloc(MAX_PAYLOAD_LENGTH, sizeof(char));
  msg->length = 0;
  buff_capacity = MAX_PAYLOAD_LENGTH;

  do {
    recvfrom(connection->socket, packet, sizeof(PACKET), 0, NULL, 0);

    if(packet->type == DATA) {
      curr_checksum = checksum(packet->payload, packet->payload_length);
    
      if( packet->checksum == curr_checksum) {
        for(i = 0; i < packet->payload_length; i++) {
          if(msg->length == buff_capacity) {
            msg->buffer = resize_buffer(msg->buffer, &buff_capacity);
          }
          
          msg->buffer[msg->length] = packet->payload[i];
          msg->length++;
        }
        // SEND ACK
        sendto(connection->socket, build_ACK(), sizeof(PACKET), 0, connection->remote_addr, connection->addrlen);
      } else {
        // SEND NACK
        sendto(connection->socket, build_NACK(), sizeof(PACKET), 0, connection->remote_addr, connection->addrlen);
      }
    }
  } while(packet->type != LAST_DATA);


	/* ---- FIXME ----
	 * The goal of this function is to handle 
	 * receiving a message from the remote server using
	 * recvfrom and the connection info given. You must 
	 * dynamically resize a buffer as you receive a packet
	 * and only add it to the message if the data is considered
	 * valid. The function should return the full message, so it
	 * must continue receiving packets and sending response 
	 * ACK/NACK packets until a LAST_DATA packet is successfully 
	 * received.
	 */

  return msg;
}

/*
 * Takes in a buffer and copies its contents to a new buffer with 
 * twice the capacity of the original
 * @param buffer A pointer to the old buffer
 * @param buff_capacity A pointer to the capacity of the old buffer
 */
char* resize_buffer(char *buffer, int *buff_capacity) {
  int i;
  char *new_buffer = (char*) calloc(2*(*buff_capacity), sizeof(char));
  
  // copy contents of old buffer to new buffer
  for(i = 0; i < *buff_capacity; i++) {
    new_buffer[i] = buffer[i];
  }

  // free the old buffer
  free(buffer); 
  
  // increase the capacity 
  *buff_capacity = 2*(*buff_capacity);

  return new_buffer;
}


PACKET* build_ACK() {
  PACKET *ack = (PACKET*) calloc(1, sizeof(PACKET));
  
  ack->type = ACK;
  ack->checksum = 0;
  ack->payload_length = 0;

  return ack;
}

PACKET* build_NACK() {
  PACKET *nack = (PACKET*) calloc(1, sizeof(PACKET));
  
  nack->type = NACK;
  nack->checksum = 0;
  nack->payload_length = 0;

  return nack;
}
