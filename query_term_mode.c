#include <termios.h>
#include <unistd.h>

#include <stdio.h>

struct termios old;
struct termios tio;
int flg = 0;

/*
TCSANOW   - change occurs immediately
TCSADRAIN - change occurs after all output written to fd has been transmitted.
TCSAFLUSH - change occurs after all output written to the object referred by fd has been transmitted, 
            and all input that has been received but not read will be discarded before the change is made.
*/

void set_raw(){
  // printf("set_raw\n");
  if (tcgetattr(0, &tio) != -1) {
    if (!flg) {
      old = tio;
      flg = 1;
    }
    tio.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(0, TCSANOW, &tio);
    // tcsetattr(0, TCSADRAIN, &tio);
    // tcsetattr(0, TCSAFLUSH, &tio);
  }
}

void set_normal(){
  if (flg) {
    tcsetattr(0, TCSANOW, &old);
    return;
  }
  if (tcgetattr(0, &tio) != -1) {
    tio.c_lflag |= (ECHO | ICANON);
    tcsetattr(0, TCSANOW, &tio);
  }
}

enum {
 UNKNOWN = 0,
     SET = 1,
   RESET = 2,
    SET1 = 3,
  RESET1 = 4
} stat_val;


typedef struct {
  int num;
  int val;
} mode_val;

mode_val mode_stat[] = {
  {1002, -1},
  {1006, -1},
  {  25, -1},
  {   1, -1},
  {1049, -1},
  {2004, -1},
  {1004, -1},
  {1000, -1},
};

int set_mode(int num, int val){
  if (val==1)
    printf("\e[?%dh", num);
  else if(val==2)
    printf("\e[?%dl", num);
  else
    fprintf(stderr, "unknown val\n");
  return 0;
}

int query_mode(int num){
  // fprintf(stderr, "query: %d\n", num);
  // int num = 1049;
  printf("\e[?%d$p", num);
  // ^[[?1049;0$y  unkown
  // ^[[?1049;2$y  unset
  // ^[[?1049;1$y  set
  int out;
  int val;
  scanf("\e[?%d;%d$y", &out, &val);
  // printf("got: %4d %d\n", out, val);
  return val;
}

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

void save_all(){
  for(size_t i=0; i<ARRAY_SIZE(mode_stat); i++){
    mode_stat[i].val = query_mode(mode_stat[i].num);
  }
}
void load_all(){
  for(size_t i=0; i<ARRAY_SIZE(mode_stat); i++){
    set_mode(mode_stat[i].num, mode_stat[i].val);
  }
}

void show_all(){
  for(size_t i=0; i<ARRAY_SIZE(mode_stat); i++){
    printf(" - %4d: %d\n", mode_stat[i].num, mode_stat[i].val);
  } 
}

// ------------- api
unsigned char modes[8] = {0};

void save_mode(unsigned char *modes){
  set_raw();
  save_all();
  set_normal();

  // show_all();

  for(size_t i=0; i<ARRAY_SIZE(mode_stat); i++){
    modes[i] = mode_stat[i].val;
    // printf("\r - %4d: %d\n", mode_stat[i].num, mode_stat[i].val);
  }
}

void load_mode(unsigned char *modes){
  for(size_t i=0; i<ARRAY_SIZE(mode_stat); i++){
    mode_stat[i].val = modes[i];
  }

  set_raw();
  load_all();
  set_normal();
}

void send_mode(unsigned char *modes, int fd){
  char buf[1024] = {0};
  int pos = 0;
  for(size_t i=0; i<ARRAY_SIZE(mode_stat); i++){
    int num = mode_stat[i].num;
    int val = modes[i];
    if (val==1)
      pos += sprintf(&buf[pos], "\e[?%dh", num);
    else if(val==2)
      pos += sprintf(&buf[pos], "\e[?%dl", num);
  }
  fprintf(stderr, "send_mode: %d bytes\n", pos);
  write(fd, buf, pos);
}

void show_mode(unsigned char *modes){
  for(size_t i=0; i<ARRAY_SIZE(mode_stat); i++){
    printf("\r - %4d: %d\n", mode_stat[i].num, modes[i]);
  }
}

#ifdef TEST_MAIN

// #include <sys/ioctl.h>

int main(int argc, char *argv[]) {

  // printf("size: %ld\n", sizeof(struct winsize));

  // sleep(1);
  printf("-- now into raw\n");
  set_raw();
  // sleep(3);

  save_all();

  printf("-- now exit raw\n");
  set_normal();

  show_all();
  // sleep(1);

}

// --------- test

void test1(){
  query_mode(1002);
  query_mode(1006);
  query_mode(25);
  query_mode(1);
  query_mode(1049);
  query_mode(2004);
  query_mode(1004);
}

#endif
