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
struct editorConfig {
    struct termios org_termios;
};
struct editorConfig E;
/*** terminal ***/
void die(const char *s){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}
void disableRawMode() {
   if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.org_termios) == -1) die("tcsetattr"); 
}
void enableRawMode() {
    if(tcgetattr(STDIN_FILENO, &E.org_termios) == -1) die("tcsetattr");
    atexit(disableRawMode);
    struct termios raw = E.org_termios;
    raw.c_iflag &= ~(BRKINT|INPCK|ISTRIP|IXON| ICRNL);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= ~(CS8);
    raw.c_lflag &= ~(ECHO| ICANON| ISIG| IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] =1;
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*** output ***/
void editorDrawRows() {
    for(int y = 0;y< 24;y++){
        write(STDOUT_FILENO, "~\r\n",3);
    }
}

void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H",3);
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
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);

            exit(0);
            break;
    }
}

/*** init ***/
int main() {
    enableRawMode();
    while(1){
        editorRefreshScreen();
        editorProcessKeypress();
    }
    disableRawMode();
    return 0;
}
