# BSAEngine

BSAEngine, modern C++ (C++17) kullanılarak geliştirilmekte olan bir oyun motoru projesidir. Motorun temel yapıtaşları performans ve genişletilebilirlik odaklı olarak inşa edilmektedir.

## Mevcut Özellikler

* **Loglama Sistemi:** `spdlog` entegrasyonu ile konsol ve dosyaya (`logs/BSAEngine.log`) renkli ve seviye bazlı loglama yeteneği. Motor logları (`BSA_ENGINE_*`) ve Client logları (`BSA_*`) birbirinden ayrılmıştır.
* **Hafıza Yönetimi (Memory Allocators):** Temel allocator arayüzü üzerine inşa edilen üç farklı bellek yönetim stratejisi:
  * `LinearAllocator`: En hızlı ayırma, toplu serbest bırakma (bump pointer).
  * `StackAllocator`: LIFO tarzı ayırma ve son ayrılan bloğu adım adım geri alma desteği.
  * `PoolAllocator`: Bağlı liste (free list) kullanarak sabit boyutlu blokların ayrılması ve herhangi bir sırayla serbest bırakılması (Örn: ECS Bileşenleri için).

## Proje Yapısı

```
BSAEngine/
├── CMakeLists.txt     # Modern CMake yapılandırması
├── include/           # Dışa açık header (.h) dosyaları
│   └── BSAEngine/
│       ├── Log/
│       └── Memory/
└── src/               # Kaynak (.cpp) dosyaları
    ├── Log/
    ├── Memory/
    └── main.cpp
```

## Derleme Gereksinimleri

* C++17 destekli bir derleyici (MSVC, GCC, Clang vs.)
* **CMake** (Sürüm 3.20 veya üzeri)
* Git (Bağımlılıkların `FetchContent` ile çekilmesi için)

## Nasıl Derlenir?

Bağımlılıklar (şu an sadece `spdlog`), CMake aracılığıyla build aşamasında otomatik indirilip derlenecektir.

```bash
# Projeyi yapılandır
cmake -B build -S .

# Projeyi derle (Örn: Debug modu)
cmake --build build --config Debug

# Çalıştır
./bin/Debug/Debug/BSAEngine.exe   # (veya ./bin/Debug/BSAEngine)
```

## Lisans

*(Lisans bilgisi buraya eklenecektir)*
