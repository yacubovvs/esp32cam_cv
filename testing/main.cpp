#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
#include "cv_filters.cpp"
#include <time.h>
#define bmp_header_size     54



int main()
{

  int one = 1, client_fd;
  struct sockaddr_in svr_addr, cli_addr;
  socklen_t sin_len = sizeof(cli_addr);
 
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) err(1, "can't open socket");
 
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
 
  int port = 8090;
  svr_addr.sin_family = AF_INET;
  svr_addr.sin_addr.s_addr = INADDR_ANY;
  svr_addr.sin_port = htons(port);
 
  if (bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr)) == -1) {
    close(sock);
    err(1, "Can't bind");
  }
 
  listen(sock, 5);
  while (1) {
    client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
    printf("New connection\n");
 
    if (client_fd == -1) {
      perror("Can't accept");
      continue;
    }

    /*
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # #                                     READ FILE                                     # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    */


    FILE* f = fopen("data.bmp", "rb");
    unsigned char info[bmp_header_size];
    fread(info, sizeof(unsigned char), bmp_header_size, f); 
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];
    if(height<0) height*=-1;
    long size = 3 * width * height;
    unsigned char data[size];
    fread(data, sizeof(unsigned char), size, f); 
    fclose(f);

    //Creating frame->data() like in Arduino
    unsigned char frame_data[bmp_header_size + size];
    for(int i=0; i<bmp_header_size; i++) frame_data[i]=info[i];            //  BMP file header
    for(long i=0; i<size; i++) frame_data[bmp_header_size + i]=data[i];    //  BMP file data

    /*
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # #                                    SEND IMAGE                                     # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    */

    //filter_contrast_blackWhite(frame_data);
    cv_applyFilters(frame_data);

    char response[] = "HTTP/1.1 200 OK\r\nContent-Type: image/bmp\r\nCache-Control: no-cache\r\nAllow: GET\r\nAccept-Ranges: bytes\r\nKeep-Alive: timeout=150, max=300\r\nContent-Length: 9999999999\r\n\r\n";

    write(client_fd, response, sizeof(response) - 1);         //  Headers
    //write(client_fd, frame_data, (long)sizeof(frame_data) - 0);     //  File data
    
    int buff_size = 128;
    unsigned char buff[buff_size];
    int buff_position = 0;
    for(long i=0; i<bmp_header_size + size; i++){
      if(i%50==0)usleep(1);
      buff[buff_position] = frame_data[i];
      buff_position ++;

      if(buff_position==buff_size){
        int res = write(client_fd, buff, sizeof(buff));
        //printf("Результат %d ... %d из %d \n", res, i, bmp_header_size + size);
        buff_position = 0;
      }
    }
    
    write(client_fd, buff, sizeof(buff));

    /*
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # #                              FINISHING CONNECTION                                 # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    */

    close(client_fd);
  }
}