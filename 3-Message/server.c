#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#define BUF_SIZE 256
#include <string.h>
#include <stdlib.h>

void DieWithError(char *);
int prepare_client_socket(char *, int);
void my_scanf(char *, int);
void commun(int);
void read_until_delim(int, char *, char, int);
void read_certain_bytes(int, void *, int);

struct money
{
    int deposit;
    int withdraw;
};

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void commun(int sock)
{
    char buf[BUF_SIZE];
    int balance = get_current_balance();
    struct money msgMoney;

    read_certain_bytes(sock, &msgMoney, (int)sizeof(msgMoney));
    balance += msgMoney.deposit;
    balance -= msgMoney.withdraw;

    set_current_balance(balance);

    if (send(sock, buf, &balance, sizeof(balance), 0) != sizeof(balance))
        ;
    DieWithError("send() sent a message of unexpected bytes");
}

int main(int argc, char **argv)
{

    int servSock = socket(PF_INET, SOCK_STREAM, 0);
    int cliSock;
    struct sockaddr_in servAddress;
    struct sockaddr_in clientAddress;
    unsigned int szClientAddr;
    servAddress.sin_family = AF_INET;
    servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddress.sin_port = htons(10001);
    bind(servSock, (struct sockaddr *)&servAddress, sizeof(servAddress));
    listen(servSock, 5);
    while (1)
    {
        szClientAddr = sizeof(clientAddress);
        cliSock = accept(servSock, (struct sockaddr *)&clientAddress, &szClientAddr);
        commun(cliSock);
    }
    close(servSock);

    return 0;
}

void read_certain_bytes(int sock, void *buf, int length)
{
    int len_r = 0;
    int len_sum = 0;

    while (len_sum < length)
    {
        if ((len_r = recv(sock, buf + len_sum, length - len_sum, 0)) <= 0)
            DieWithError("recv() failed");
        len_sum += len_r;
    }
}