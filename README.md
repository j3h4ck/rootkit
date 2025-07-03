# 🔧 r00tkit – Minimal Kernel-Mode Rootkit (Educational Purposes Only)

**r00tkit** is a minimal Windows kernel-mode driver (rootkit-style) designed for educational and demonstration purposes. It allows controlled communication between user-mode and kernel-mode via IOCTL, with capabilities such as message logging and basic process termination from kernel space.

⚠️ **This project is for learning and research only. Do not use it maliciously.**

---

## 🧠 Features

- Simple kernel-mode driver setup using `IoCreateDevice` and symbolic link.
- Handles basic `IOCTL` communication from user-mode.
- Supports commands:
  - `print <message>` – Print a custom message from user-mode to the kernel debugger.
  - `kill <pid>` – Terminate a process from kernel-mode by PID (if permissions allow).
- Clean unload routine to remove device and symbolic link.
- User-mode client (written in C) to send commands to the driver.
- Can by used to kill EDR processes

---

## 🗂️ Project Structure

📁 Driver1
- ├── Driver1.cpp # Kernel-mode driver source code
- ├── Com.cpp # User-mode interface to communicate with the driver

---

## 🛠️ Build Instructions

### 🧷 Driver (Kernel-mode)
1. Open the project in Visual Studio with the **Windows Driver Kit (WDK)** installed.
2. Set the configuration to **x64** and **Release**.
3. Compile as a kernel-mode driver (KMDF or WDM, depending on your setup).
4. Use tools like `sc.exe` or `OSR Loader` to load the `.sys` file.
   ```bash
   sc.exe create Driver1 type= kernel binPath= "C:\Path\To\Driver1.sys"
   sc.exe start Driver1


### 👨‍💻 User-Mode Client
1. Compile with any C compiler (e.g., MSVC, MinGW).
2. Run from admin shell:
   ```bash
   Com.exe

### 📚 Author
Developed by Jehad Abudagga

YouTube: https://www.youtube.com/@j3h4ck
