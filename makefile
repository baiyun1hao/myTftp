CC=gcc

myclient:  myserver tftpclient.c
	mkdir -p client
	$(CC) tftpclient.c common.c -o client/tftp_c

myserver: tftpserver.c  common.c
	$(CC) tftpserver.c common.c -o tftp_s 

clean:
	rm -f client/tftp_c tftp_s 