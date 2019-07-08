#include <shm/shm.h>

int main(int argc, char *argv[])
{
  shm::ShareMemoryHandler shm_send("dummy", 10000);

  double n = 0;

  std::vector<double> data;

  double a = 3.14;
  auto b = reinterpret_cast<const char*>(&a);

  while(true) {

    // std::string data = std::to_string(n++);
    data.push_back(n);
    n += 1.1;

    // std::cout << "send vector: ";
    // for(auto& d:data) {
    //   std::cout << d << " ";
    // }
    // std::cout << std::endl;    

    shm_send.write<std::vector<double>>(data);
    shm_send.spinOnce();
    auto data_read = shm_send.read();
  }

  return 0;
}