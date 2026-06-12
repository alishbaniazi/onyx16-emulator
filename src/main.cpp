#include "pch.h"
#include "Header.h"
#include <iostream>
using namespace std;



int main() {
    MemoryModule ram;
    keyboard kb;
    phosphorusDisplay display;
    graphicAdapter gpu;
    mainBoard mb;
    psu ps(50.0f);
    Processor cpu;

    //making connections
    gpu.setdisplay(&display);

    mb.addRAM(&ram);
    mb.addKeyboard(&kb);
    mb.addgraphicAdapter(&gpu);
    mb.addDisplay(&display);
    mb.addPSU(&ps);

   
    ps.connectMainBoard(&mb);

    cpu.connectMboard(&mb);

    //menu
    cout << "\033[33m"<<"========================================" << endl;
    cout << "        ONYX-16 BOOT INTERFACE          " << endl;
    cout << "========================================" << endl;
    cout << " [1] Hello World  (hello.txt)" << endl;
    cout << " [2] Calculator   (calculator.txt)" << endl;
    cout << " [3] Auth System  (auth.txt)" << endl;
    cout << " [4] Custom script" << endl;
    cout << "========================================" << "\033[0m" << endl;
    cout << "Select: ";

    int choice;
    cin >> choice;

    string filename;
    if (choice == 1) 
        filename = "hello.txt";
    else if (choice == 2)
        filename = "calculator.txt";
    else if (choice == 3)
        filename = "auth.txt";
    else {
        cout << "Enter filename: ";
        cin >> filename;
    }

    
    cout << "\033[31m"<<"\n[BIOS] Booting " << filename << "..." << endl;
    Interpreter interp;
    cout << "\033[34m";
    interp.loadProgramAndFlash(filename, ram);
    cout << "\033[0m";
    
    cpu.run();
    cpu.printState();
    // Step 9 — render display
    display.render();

    return 0;
}



