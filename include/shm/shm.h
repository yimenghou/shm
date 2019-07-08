#pragma once

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <cstring>
#include <cstdlib>
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>

#include "shm/utils.h"

namespace shm {

namespace ipc = boost::interprocess;

struct shmRemover {
public:
  shmRemover(const std::string& shm_name):name_(shm_name) {}
  shmRemover operator=(const shmRemover & obj) = delete;
  shmRemover(const shmRemover & obj) = delete;
  virtual ~shmRemover() {ipc::shared_memory_object::remove(name_.c_str());}
private:
  std::string name_;
};

class ShareMemoryHandler {
public:

  explicit ShareMemoryHandler(const std::string& shm_name,
                              const size_t& size) noexcept:
    name_(shm_name), size_(size), remover_(shm_name) {

    if (!exist_) {
      shm_obj_.reset(new ipc::shared_memory_object(ipc::open_or_create, name_.c_str(), ipc::read_write));
      shm_obj_->truncate(size_);
      shm_reg_.reset(new ipc::mapped_region(*shm_obj_, ipc::read_write));
      std::memset(shm_reg_->get_address(), 0, shm_reg_->get_size());
      exist_ = true;
    }
  }

  inline char* read() const {
    return static_cast<char*>(shm_reg_->get_address());
  }

  inline bool write(const char* data) {
    char* mem = read();
    for(std::size_t i = 0; i < shm_reg_->get_size(); ++i) {
      *(mem+i) = *(data+i); 
    }
  }

  inline bool write(const std::string& data) {
    const char *cstr = data.c_str();
    char* mem = read();
    for(std::size_t i = 0; i < shm_reg_->get_size(); ++i) {
      *(mem+i) = *(cstr+i); 
    }
  }

  template<typename T>
  inline bool write(const typename std::enable_if<is_stl_container<T>::value, T>::type& data) {
    using mem_t = typename T::value_type;
    static_assert(std::is_arithmetic<mem_t>::value);
    size_t ci = 0;
    for(const auto& c:data) {
      const char* cstr = reinterpret_cast<const char*>(&c);
      std::cout << "c: " << c << ", cstr: " << cstr <<std::endl;
      std::cout << "cstr length: " << strlen(cstr) << std::endl;
      auto mem = read();
      for(int i = 0; i < strlen(cstr); ++i) {
        *(mem + ci++) = *(cstr+i);
      }
    }
  }

  template<typename T>
  inline typename std::enable_if<is_stl_container<T>::value, T>::type read() const {
    using mem_t = typename T::value_type;
    static_assert(std::is_arithmetic<mem_t>::value);
    T data_out;
    for(const char* it = read(); *it; ++it) {
      data_out.push_back(*it);
    }
    return data_out;
  }

  void spin() {
    while(true) {
      using namespace std::literals::chrono_literals;
      std::this_thread::sleep_for(100ms);
    }
  }

  void spinOnce() {
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(100ms);
  }

  ~ShareMemoryHandler() {};

private:
  std::string name_;
  bool exist_;
  size_t size_;
  std::unique_ptr<ipc::shared_memory_object> shm_obj_;
  std::unique_ptr<ipc::mapped_region> shm_reg_;
  shmRemover remover_;
};

}

