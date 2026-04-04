# Eld Mart Slam - Indonesian Localization (Project Coffee SDK 7)

Proyek ini adalah versi lokalisasi bahasa Indonesia dari game **Eld Mart Slam** yang dibangun menggunakan **mata_SDK 7**.

### 🤖 Pengembang AI
Seluruh proses modifikasi, penerjemahan, dan konfigurasi sistem build pada repository ini dilakukan melalui bantuan **Gemini CLI (AI Agent)**.

### 📍 Repository Asal
Proyek ini di-fork dan dimodifikasi dari repository asli milik **mata0319**:
- **Original Repo:** [GitLab - Project_Coffee_SDK_7](https://gitlab.com/mata0319/Project_Coffee_SDK_7.git)

### 🛠️ Apa Saja yang Dilakukan?
Berikut adalah perubahan utama yang telah diimplementasikan dalam versi ini:
1.  **Indonesian Localization:** Menerjemahkan seluruh UI utama, intro, disclaimer, dan pesan error dari bahasa Korea ke bahasa Indonesia.
2.  **Multi-language Support (XML-based):** Mengubah sistem teks *hardcoded* menjadi sistem kamus berbasis file XML (`Indonesian.xml`, `English.xml`, `Korean.xml`).
3.  **Language Selector:** Menambahkan opsi pemilihan bahasa di menu Pengaturan (*Settings*) yang tersimpan secara permanen.
4.  **Automated Build System:** Menambahkan workflow **GitHub Actions** agar proyek ini bisa di-build secara otomatis menjadi file `.exe` setiap kali ada perubahan kode.

### 🚀 Cara Mendapatkan File .EXE
Anda tidak perlu menginstall Visual Studio untuk menjalankan game ini. Cukup buka tab [**Actions**](https://github.com/nyanpoketto-kujira/ed-mart-slam/actions) di repository ini, pilih build terbaru, dan download file **Eld-Mart-Slam-Exe** dari bagian *Artifacts*.

### 📜 Lisensi & Hak Cipta
- Karakter dan aset dalam game ini adalah milik **EPIDGames** (Fan-made project).
- Engine game asli (`mata_SDK`) dikembangkan oleh **mata0319**.
