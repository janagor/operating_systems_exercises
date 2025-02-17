#include "zad6.hpp"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

Inode::Inode(const std::string &name, int size, int start, int blocks)
    : fileName(name), fileSize(size), startBlock(start), numBlocks(blocks) {}

void Inode::serialize(std::ofstream &out) const {
  size_t nameSize = fileName.size();
  out.write(reinterpret_cast<const char *>(&nameSize), sizeof(nameSize));
  out.write(fileName.c_str(), nameSize);
  out.write(reinterpret_cast<const char *>(&fileSize), sizeof(fileSize));
  out.write(reinterpret_cast<const char *>(&startBlock), sizeof(startBlock));
  out.write(reinterpret_cast<const char *>(&numBlocks), sizeof(numBlocks));
}

void Inode::deserialize(std::ifstream &in) {
  size_t nameSize;
  in.read(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
  char *nameBuffer = new char[nameSize + 1];
  in.read(nameBuffer, nameSize);
  nameBuffer[nameSize] = '\0';
  fileName = std::string(nameBuffer);
  delete[] nameBuffer;

  in.read(reinterpret_cast<char *>(&fileSize), sizeof(fileSize));
  in.read(reinterpret_cast<char *>(&startBlock), sizeof(startBlock));
  in.read(reinterpret_cast<char *>(&numBlocks), sizeof(numBlocks));
}

VirtualDisk::VirtualDisk()
    : inodes({}), blockMap(NUM_BLOCKS, false), diskFileName("") {}

void VirtualDisk::saveDisk(std::ofstream &out) {
  for (bool block : blockMap) {
    char state = block ? 1 : 0;
    out.write(&state, sizeof(state));
  }

  size_t inodeCount = inodes.size();
  out.write(reinterpret_cast<const char *>(&inodeCount), sizeof(inodeCount));

  for (const auto &inode : inodes) {
    inode.serialize(out);
  }
}

void VirtualDisk::createDisk(const std::string &filename) {
  std::ofstream out(filename, std::ios::out | std::ios::binary);
  if (!out) {
    std::cerr << "Error creating disk!\n";
    return;
  }
  diskFileName = filename;

  saveDisk(out);
  out.close();
  std::cout << "Disk created successfully!\n";
}

void VirtualDisk::openDisk(const std::string &filename) {
  disk.open(filename, std::ios::in | std::ios::out | std::ios::binary);
  if (!disk) {
    std::cerr << "Error opening disk!\n";
    return;
  }
  diskFileName = filename;
  std::ifstream in(filename, std::ios::binary);
  if (!in) {
    std::cerr << "Error opening disk!\n";
    return;
  }

  loadDisk(in);
  std::cout << "Disk opened successfully: " << filename << std::endl;
}

void VirtualDisk::loadDisk(std::ifstream &in) {
  // Wczytujemy mapę bloków
  for (size_t i = 0; i < blockMap.size(); ++i) {
    char state;
    in.read(&state, sizeof(state));
    blockMap.at(i) = (state == 1);
  }

  // Wczytujemy liczbę inodów
  size_t inodeCount;
  in.read(reinterpret_cast<char *>(&inodeCount), sizeof(inodeCount));

  // Wczytujemy inody
  inodes.clear();
  for (size_t i = 0; i < inodeCount; ++i) {
    Inode inode;
    inode.deserialize(in);
    inodes.push_back(inode);
  }
}

void VirtualDisk::closeDisk() {
  if (disk.is_open()) {
    disk.close();
  }
  std::ofstream out(diskFileName, std::ios::binary);
  if (out.is_open()) {
    saveDisk(out);
    out.close();
    std::cout << "Disk successfully saved and closed.\n";
  } else {
    std::cerr << "Error opening file to save disk data.\n";
  }
}

void VirtualDisk::copyToDisk(const std::string &sourcePath,
                             const std::string &destFileName) {
  std::ifstream source(sourcePath, std::ios::binary | std::ios::ate);
  if (!source) {
    std::cerr << "Nie można otworzyć pliku źródłowego: " << sourcePath
              << std::endl;
    return;
  }

  std::streamsize fileSize = source.tellg();
  source.seekg(0, std::ios::beg);

  std::vector<char> buffer(fileSize);
  if (!source.read(buffer.data(), fileSize)) {
    std::cerr << "Błąd podczas odczytu pliku źródłowego: " << sourcePath
              << std::endl;
    return;
  }

  int requiredBlocks = (fileSize + BLOCK_SIZE - 1) / BLOCK_SIZE;
  int startBlock = -1;
  int freeBlocks = 0;

  // Szukamy miejsca na dysku wirtualnym
  for (int i = 0; i < NUM_BLOCKS; ++i) {
    if (!blockMap[i]) {
      if (freeBlocks == 0)
        startBlock = i;
      freeBlocks++;
      if (freeBlocks == requiredBlocks)
        break;
    } else {
      freeBlocks = 0;
    }
  }

  if (freeBlocks < requiredBlocks) {
    std::cerr << "Brak wystarczającego miejsca na dysku wirtualnym."
              << std::endl;
    return;
  }

  for (int i = startBlock; i < startBlock + requiredBlocks; ++i) {
    blockMap[i] = true;
  }

  inodes.push_back(
      {destFileName, static_cast<int>(fileSize), startBlock, requiredBlocks});

  if (!disk) {
    std::cerr << "Nie można otworzyć dysku wirtualnego do zapisu danych."
              << std::endl;
    return;
  }

  disk.seekp(startBlock * BLOCK_SIZE);
  disk.write(buffer.data(), fileSize);

  std::cout << "Plik " << destFileName
            << " został skopiowany na dysk wirtualny." << std::endl;
}

void VirtualDisk::copyFromDisk(const std::string &source,
                               const std::string &dest) {
  for (const auto &inode : inodes) {
    if (inode.fileName == source) { // Match by filename (correct field name)
      std::ofstream out(dest, std::ios::binary);
      if (!out) {
        std::cerr << "Error creating destination file!\n";
        return;
      }

      char buffer[BLOCK_SIZE];
      for (int i = 0; i < inode.numBlocks; ++i) {
        disk.seekg((inode.startBlock + i) * BLOCK_SIZE, std::ios::beg);
        disk.read(buffer, BLOCK_SIZE);
        out.write(buffer, BLOCK_SIZE);
      }

      std::cout << "File copied from virtual disk to " << dest << "!\n";
      return;
    }
  }

  std::cerr << "File not found on virtual disk!\n";
}
void VirtualDisk::listFiles() {
  std::cout << std::left << std::setw(20) << "Filename" << std::setw(10)
            << "Size" << "Start Block\n";

  for (const auto &inode : inodes) {
    if (!inode.fileName.empty()) {
      std::cout << std::left << std::setw(20) << inode.fileName << std::setw(10)
                << inode.fileSize << inode.startBlock << '\n';
    }
  }
}

void VirtualDisk::deleteFile(const std::string &filename) {
  for (auto it = inodes.begin(); it != inodes.end(); ++it) {
    if (it->fileName == filename) {
      for (int i = 0; i < it->numBlocks; ++i) {
        blockMap[it->startBlock + i] = false;
      }

      inodes.erase(it);

      std::cout << "File " << filename << " deleted from virtual disk!\n";
      return;
    }
  }

  std::cerr << "File " << filename << " not found on virtual disk!\n";
}

void VirtualDisk::deleteDisk() {
  if (disk.is_open()) {
    disk.close();
  }
  if (!diskFileName.empty()) {
    // Delete the disk file using the stored filename
    if (remove(diskFileName.c_str()) == 0) {
      std::cout << "Virtual disk " << diskFileName << " deleted!\n";
    } else {
      std::cerr << "Error deleting virtual disk " << diskFileName << "!\n";
    }
  } else {
    std::cerr << "No disk file open to delete!\n";
  }
}

void VirtualDisk::showMap() {
  std::cout << "Disk block map:\n";
  for (size_t i = 0; i < blockMap.size(); i++) {
    if (blockMap[i])
      std::cout << "Block " << i << ": "
                << "Occupied" << '\n';
  }
}
