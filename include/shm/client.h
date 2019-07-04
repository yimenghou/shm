#pragma once

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include <memory>

namespace shm {

namespace ipc = boost::interprocess;

//Remove shared memory on construction and destruction
struct shmRemover {
public:
  shmRemover(const std::string& shm_name):name_(shm_name) {}
  shmRemover operator=(const shmRemover & obj) = delete;
  shmRemover(const shmRemover & obj) = delete;
  virtual ~shmRemover() {ipc::shared_memory_object::remove(name_.c_str());}
private:
  std::string name_;
};

class ShareMemoryHelper {
public:

  ShareMemoryHelper(const std::string& shm_name,
                    const size_t& size):
    name_(shm_name), size_(size), remover_(shm_name) {
    try{
      shm_obj_.reset(new ipc::shared_memory_object(ipc::open_only, name_.c_str(), ipc::read_write));
      exist_ = true;
    } catch (ipc::interprocess_exception &ex) {
      std::cout << ex.what() << "create a new region for shm" << std::endl;
    }

    if (!exist_) {
      shm_obj_.reset(new ipc::shared_memory_object(ipc::create_only, name_.c_str(), ipc::read_write));
      shm_obj_->truncate(size_);
      shm_reg_.reset(new ipc::mapped_region(*shm_obj_, ipc::read_write));
      std::memset(shm_reg_->get_address(), 0, shm_reg_->get_size());
    }
  }

  inline char* Read() const {
    return static_cast<char*>(shm_reg_->get_address());
  }

  inline bool Write(const char* data) {
  char* mem = Read();
   for(std::size_t i = 0; i < shm_reg_->get_size(); ++i) {
      *(mem+i) = *(data+i); 
      std::cout << "I can see!" << *(mem+i);
   }
  }

  ~ShareMemoryHelper();

private:
  std::string name_;
  bool exist_;
  size_t size_;
  std::unique_ptr<ipc::shared_memory_object> shm_obj_;
  std::unique_ptr<ipc::mapped_region> shm_reg_;
  shmRemover remover_;
};

}

