/*** includes ***/
#include<termios.h>
#include<ctype.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include <unistd.h>

/*** defines ***/
#define CTRL_KEY(k) ((k)& (0x1f))

/*** data ***/
struct termios org_termios;

/*** terminal ***/
void die(const char *s){
    perror(s);
    exit(1);
}
void disableRawMode() {
   if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &org_termios) == -1) die("tcsetattr"); 
}
void enableRawMode() {
    if(tcgetattr(STDIN_FILENO, &org_termios) == -1) die("tcsetattr");
    atexit(disableRawMode);
    struct termios raw = org_termios;
    raw.c_iflag &= ~(BRKINT|INPCK|ISTRIP|IXON| ICRNL);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= ~(CS8);
    raw.c_lflag &= ~(ECHO| ICANON| ISIG| IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] =1;
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*** input ***/
char editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  return c;
}

void editorProcessKeypress() {
    char c = editorReadKey();

    switch(c) {
        case CTRL_KEY('q'):
            exit(0);
            break;
    }
}

/*** init ***/
int main() {
    enableRawMode();
    while(1){
        editorProcessKeypress();
    }
    disableRawMode();
    return 0;
}
