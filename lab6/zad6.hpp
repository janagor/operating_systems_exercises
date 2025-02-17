#pragma once

#include <cstring>
#include <fstream>
#include <string>
#include <vector>

const int BLOCK_SIZE = 512;
const int DISK_SIZE = 1024 * 1024;
const int NUM_BLOCKS = DISK_SIZE / BLOCK_SIZE;

struct Inode {
public:
  Inode(const std::string &name = "", int size = 0, int start = -1,
        int blocks = 0);

  void serialize(std::ofstream &out) const;
  void deserialize(std::ifstream &in);

public:
  std::string fileName;
  int fileSize;
  int startBlock;
  int numBlocks;
};

class VirtualDisk {
public:
  VirtualDisk();

  void createDisk(const std::string &filename);
  void openDisk(const std::string &filename);

  void closeDisk();
  void deleteFile(const std::string &filename);
  void deleteDisk();

  void copyToDisk(const std::string &source, const std::string &dest);
  void copyFromDisk(const std::string &source, const std::string &dest);

  void listFiles();
  void showMap();

private:
  void saveDisk(std::ofstream &out);
  void loadDisk(std::ifstream &in);

private:
  std::fstream disk;
  std::vector<Inode> inodes;
  std::vector<bool> blockMap;
  std::string diskFileName;
};
