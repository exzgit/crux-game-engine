# RECS — R Entity Component System

RECS adalah implementasi ECS minimal dalam C++ (C++20). README singkat ini menjelaskan cara membangun, menjalankan tes, dan contoh penggunaan API dasar.

**Prasyarat**
- Compiler C++ dengan dukungan C++20 (mis. `g++` atau `clang++`).
- `meson` dan `ninja` untuk membangun proyek.

**Membangun proyek**
Jalankan perintah berikut dari direktori proyek:

```bash
meson setup build
ninja -C build
```

Jika sudah ada direktori `build`, cukup jalankan:

```bash
ninja -C build
```

**Menjalankan tes contoh**
Setelah berhasil dibangun, jalankan binari tes:

```bash
./build/resc_test
```

Seharusnya menampilkan pesan dan menyelesaikan semua tes jika semuanya benar.

**Contoh penggunaan API (ringkas)**
Contoh ini menunjukkan operasi dasar: membuat world, membuat entitas, menambahkan komponen, dan melakukan query.

```cpp
#include "recs/world.h"
#include "components.h"

int main() {
    World world;

    // Buat entitas
    Entity e = world.create_entity();

    // Tambah komponen
    world.add<Position>(e);
    world.add<Velocity>(e);

    // Akses komponen
    auto &p = world.get<Position>(e);
    auto &v = world.get<Velocity>(e);
    p.x = 1.0f; p.y = 2.0f;
    v.x = 0.5f; v.y = 0.0f;

    // Query: iterasi semua entitas yang punya Position dan Velocity
    world.query().for_each<Position, Velocity>([](Position &pos, Velocity &vel){
        pos.x += vel.x;
        pos.y += vel.y;
    });

    return 0;
}
```

**Catatan pengembangan**
- Library ini menggunakan layout chunked (structure-of-arrays) untuk penyimpanan komponen.
- Beberapa debug `fprintf` mungkin masih ada di kode sumber; ini membantu saat mengembangkan. Saya bisa membersihkannya jika Anda mau.

Jika Anda ingin, saya bisa:
- Menghapus semua debug print (membuat output bersih),
- Menambahkan dokumentasi lebih lengkap untuk setiap header di `include/recs/`,
- Menambahkan contoh program kecil di `examples/`.

Mau saya lanjutkan dengan membersihkan debug print atau menambahkan dokumentasi lebih rinci?

## Cara Kerja

Berikut penjelasan singkat tentang arsitektur dan alur kerja RECS (dalam bahasa Indonesia).

- **Entitas (`Entity`)**: entitas hanyalah sebuah identifier (`index` + `generation`). Tidak menyimpan data komponen sendiri.

- **Komponen**: tipe data polos (struct) yang diregistrasi melalui `ComponentRegistry`. Registry memberi `ComponentTypeID` unik untuk tiap tipe komponen dan menyimpan `size` serta `alignment`.

- **Archetype (`Archetype`)**: representasi struktur komponen (signature). Semua entitas yang memiliki himpunan komponen yang sama akan ditempatkan pada satu `Archetype`.

- **ArchetypeSignature**: menyimpan daftar `ComponentTypeID` terurut yang menjadi identitas archetype. Digunakan untuk mencocokkan query dan migrasi struktur.

- **Chunk**: unit penyimpanan fisik untuk sejumlah entitas pada sebuah archetype. RECS menggunakan layout Structure-Of-Arrays (SoA): untuk tiap tipe komponen disediakan blok memori berukuran `component_size * entity_capacity`. `Chunk` bertanggung jawab menyimpan data komponen per-entity (index baris/row) dan peta `entity_ids` untuk melacak pemilik tiap baris.

- **Penyimpanan dan alokasi**:
    - Saat sebuah `Archetype` dibuat, ia membuat `Chunk` ketika dibutuhkan. `Chunk::compute_layout` menghitung blok untuk masing-masing tipe komponen sehingga total memori tiap chunk tidak melebihi `CHUNK_SIZE`.
    - Ketika entitas ditambahkan (`World::create_entity` atau `Archetype::add_entity`), entitas mendapat baris pada `Chunk` dan komponen baru (jika ada) dialokasikan pada `Chunk` tujuan.

- **Migrasi archetype (menambahkan/membuang komponen)**:
    - `World::add<T>(entity)` membuat `ArchetypeSignature` baru (asal signature + `T`) dan mencari/ membuat `Archetype` tujuan.
    - Data komponen dari `Chunk` sumber disalin ke `Chunk` tujuan (baris per baris menggunakan `Chunk::move_entity` yang menyalin blok komponen yang sesuai). Lokasi entitas kemudian diperbarui.
    - Jika baris terakhir dipindahkan, chunk sumber mengisi slot kosong dengan data dari baris terakhir (kompak) dan memperbarui `entity_ids` sehingga indeks baris tetap konsisten.

- **Query (`Query`)**: membuat daftar `Archetype*` yang cocok (menggunakan `ArchetypeSignature::is_subset_of`). `Query::for_each<Components...>(fn)` memanggil callback `fn` untuk tiap entitas pada archetype yang benar-benar memiliki semua tipe komponen yang diminta. Iterasi memanggil `Archetype::for_each` yang mengakses array komponen SoA dan memanggil `fn` dengan referensi ke komponen tiap baris.

- **Sistem (`System`)**: dikemas sebagai objek yang mengimplementasikan method `run(World&, float)`. Sistem ditambahkan ke `World` dengan `add_system<T>(...)` dan dijalankan oleh `World::run_systems(delta)` yang memanggil `system->run(*this, delta)`.

- **ComponentRegistry**: singleton global (`ComponentRegistry::instance()`) untuk mendapatkan `ComponentTypeID` dengan `type_id<T>()` dan metadata ukuran/align.

Catatan implementasi & pengembangan
- Untuk kecepatan, layout SoA mempermudah iterasi data komponen saat menjalankan query.
- Migrasi archetype melibatkan salinan memori; strategi saat ini menyalin hanya blok komponen yang ada di archetype tujuan.
- Kode sekarang menyertakan beberapa debug `fprintf` yang membantu saat mengembangkan. Saya bisa menghapusnya untuk rilis bersih.

Jika Anda ingin, saya dapat melanjutkan dengan:
- Menghapus semua debug print dan menjalankan test final bersih.
- Menambah dokumentasi header yang lebih terperinci di `include/recs/`.
- Menambahkan contoh `examples/` yang menjalankan skenario kecil.

Pilih salah satu tindakan di atas jika ingin saya lanjutkan.