#include <shm/shm.h>

int main(int argc, char *argv[])
{
  shm::ShareMemoryHandler shm_send("dummy", 10000);

  int n = 0;

  std::vector<int> data;

  while(true) {

    // std::string data = std::to_string(n++);
    data.push_back(n);
    n += 1;

    shm_send.write<std::vector<int>>(data);
    shm_send.spinOnce();
    auto data_read = shm_send.read();
  }

  return 0;
}