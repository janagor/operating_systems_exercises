#include "iostream"
#include "zad6.hpp"

int main() {
  VirtualDisk vd;
  int choice;
  std::string source, dest;

  while (true) {
    std::cout << "\nVirtual Disk Operations:\n";
    std::cout << "1. Create virtual disk\n";
    std::cout << "2. Open virtual disk\n";
    std::cout << "3. Copy file to disk\n";
    std::cout << "4. Copy file from disk\n";
    std::cout << "5. List files\n";
    std::cout << "6. Delete file\n";
    std::cout << "7. Show disk map\n";
    std::cout << "8. Delete virtual disk\n";
    std::cout << "9. Close Disk\n";
    std::cout << "10. Exit\n";
    std::cout << "Choice: ";
    std::cin >> choice;

    switch (choice) {
    case 1:
      std::cout << "Enter disk filename: ";
      std::cin >> source;
      vd.createDisk(source);
      break;
    case 2:
      std::cout << "Enter disk filename: ";
      std::cin >> source;
      vd.openDisk(source);
      break;
    case 3:
      std::cout << "Enter source filename: ";
      std::cin >> source;
      std::cout << "Enter destination filename: ";
      std::cin >> dest;
      vd.copyToDisk(source, dest);
      break;
    case 4:
      std::cout << "Enter source filename on disk: ";
      std::cin >> source;
      std::cout << "Enter destination filename: ";
      std::cin >> dest;
      vd.copyFromDisk(source, dest);
      break;
    case 5:
      vd.listFiles();
      break;
    case 6:
      std::cout << "Enter filename to delete: ";
      std::cin >> source;
      vd.deleteFile(source);
      break;
    case 7:
      vd.showMap();
      break;
    case 8:
      vd.deleteDisk();
      break;
    case 9:
      vd.closeDisk();
      break;
    case 10:
      return 0;
    default:
      std::cout << "Invalid choice!\n";
    }
  }
}
