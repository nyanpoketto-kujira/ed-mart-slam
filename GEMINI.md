# Eld Mart Slam - Gemini Notes

Catatan khusus untuk pengembangan dan build proyek "Eld Mart Slam" menggunakan SDK Mata.

## Build Guide (Manual)

Jika ingin melakukan build dari command line (PowerShell/CMD), gunakan perintah berikut:

### 1. Lokasi MSBuild
Di mesin ini, MSBuild terletak di:
`C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe`

### 2. Perintah Build
Lakukan build dengan konfigurasi Release x64 (sesuai target game):
```powershell
& "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe" mata_SDK.sln /p:Configuration=Release /p:Platform=x64
```

### 3. Rebuild (Jika ada error cache)
```powershell
& "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe" mata_SDK.sln /p:Configuration=Release /p:Platform=x64 /t:Rebuild
```

---

## Mekanik Fever Mode
Fitur baru yang ditambahkan:
- **Global Stats**: Tersimpan di `SDK::GLOBAL.FeverState` dan `SDK::GLOBAL.FeverMeter`.
- **Reset**: Dilakukan di `PlayMode.Start()` untuk memastikan Fever bersih saat restart.
- **Ketergantungan**: Setiap file yang mengakses Fever Mode **WAJIB** melakukan `#include <SDK_Resource.h>`.
- **Fitur Gameplay**:
    - **Omni-Break**: Tombol apa saja (Kiri/Kanan/Bawah) menghancurkan barang saat Fever aktif.
    - **Double Points**: Skor ganda (20 poin) saat menghancurkan barang.
    - **NPC Perk**: Menendang NPC memberikan +5 poin ke Fever Meter.
    - **BGM Booster**: Musik dipercepat ke 1.2x saat Fever aktif.

---

## Pembersihan Repository (Post-Rewrite)
Karena riwayat Git telah di-rewrite untuk menghapus file sampah berukuran besar:
1. Pastikan `.gitignore` sudah diupdate (sudah dilakukan).
2. Jika bekerja di mesin lain, lakukan:
   ```bash
   git fetch origin
   git reset --hard origin/main
   ```
3. Hindari meng-commit file `*.exe.bak`, `*.exe.last`, atau file diff raksasa ke dalam repo.
