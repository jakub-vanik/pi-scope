#include "application.h"

#include <csignal>

Application application;

void handler(int signum)
{
  application.terminate();
}

int main(int argc, char *argv[])
{
  signal(SIGINT, handler);
  signal(SIGTERM, handler);
  application.run();
  return 0;
}
